#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"

#include <QGuiApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QUrlQuery>
#include <QUuid>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>

CallbackPage::CallbackPage(const QUrl& redirectUri, QObject* parent)
    : QWebEnginePage(parent), m_redirectUri(redirectUri)
{
}

bool CallbackPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
    if (isMainFrame && url.adjusted(QUrl::RemoveQuery | QUrl::RemoveFragment) == m_redirectUri) {
        emit callbackReceived(url);
        return false;
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

static casdoor::Config casdoorConfig()
{
    casdoor::Config config;
    config.endpoint = kCasdoorEndpoint;
    config.client_id = kClientId;
    config.client_secret = kClientSecret;
    config.certificate = kCertificate;
    config.organization_name = kOrganizationName;
    config.application_name = kApplicationName;
    return config;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_casdoor(casdoorConfig())
{
    ui->setupUi(this);
    ui->horizontalLayoutWidget->setStyleSheet("background-color:white;");
    ui->label_logo->setPixmap(QPixmap(":/assert/logo.png").scaledToHeight(ui->horizontalLayoutWidget->height()));
    ui->pushButton_signout->hide();

    auto* page = new CallbackPage(QUrl(kRedirectUri), this);
    connect(page, &CallbackPage::callbackReceived, this, &MainWindow::onCallbackReceived);
    m_webview = new QWebEngineView(this);
    m_webview->setPage(page);
    m_webview->hide();

    m_tableview = new QTableView(this);
    m_tableview->move(140, 80);
    m_tableview->resize(440, 160);
    m_tableview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableview->verticalHeader()->hide();
    m_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableview->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    ui->horizontalLayoutWidget->resize(width(), ui->horizontalLayoutWidget->height());
    ui->pushButton_signin->move((width() - ui->pushButton_signin->width()) / 2, ui->pushButton_signin->y());
    ui->pushButton_signout->move((width() - ui->pushButton_signout->width()) / 2, ui->pushButton_signout->y());
    m_webview->resize(width(), height());
    m_tableview->move((width() - m_tableview->width()) / 2, m_tableview->y());
}

void MainWindow::on_pushButton_signin_clicked()
{
    // A random state ties the callback to this sign-in attempt.
    m_state = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString signinUrl = QString::fromStdString(m_casdoor.GetSigninUrl(kRedirectUri, m_state.toStdString()));

    ui->pushButton_signin->hide();
    m_webview->load(QUrl(signinUrl));
    m_webview->show();
}

void MainWindow::onCallbackReceived(const QUrl& url)
{
    m_webview->hide();
    m_webview->setUrl(QUrl("about:blank"));

    QUrlQuery query(url);
    QString code = query.queryItemValue("code", QUrl::FullyDecoded);
    QString state = query.queryItemValue("state", QUrl::FullyDecoded);
    if (code.isEmpty() || state != m_state) {
        QMessageBox::warning(this, "Sign in failed", "The callback has no code or an unexpected state:\n" + url.toString());
        ui->pushButton_signin->show();
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    try {
        casdoor::Token token = m_casdoor.GetOAuthToken(code.toStdString());
        casdoor::Claims claims = m_casdoor.ParseJwtToken(token.access_token);
        QGuiApplication::restoreOverrideCursor();
        showUser(claims);
    } catch (const casdoor::Error& e) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, "Sign in failed", e.what());
        ui->pushButton_signin->show();
    }
}

void MainWindow::showUser(const casdoor::Claims& claims)
{
    const std::pair<QString, std::string> rows[] = {
        {"Name", claims.name},
        {"Display name", claims.display_name},
        {"Email", claims.email},
        {"Organization", claims.owner},
        {"User ID", claims.id},
    };

    auto* model = new QStandardItemModel(m_tableview);
    model->setHorizontalHeaderLabels({"Field", "Value"});
    for (const auto& row : rows) {
        model->appendRow({new QStandardItem(row.first), new QStandardItem(QString::fromStdString(row.second))});
    }

    if (m_tableview->model() != nullptr) {
        m_tableview->model()->deleteLater();
    }
    m_tableview->setModel(model);
    m_tableview->show();
    ui->pushButton_signout->show();
}

void MainWindow::on_pushButton_signout_clicked()
{
    // Forget the Casdoor session of the embedded browser, so that the next
    // sign-in asks for the password again.
    m_webview->page()->profile()->cookieStore()->deleteAllCookies();

    m_tableview->hide();
    ui->pushButton_signout->hide();
    ui->pushButton_signin->show();
}
