#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalLayoutWidget->setStyleSheet("background-color:white;");
    ui->label_logo->setPixmap(QPixmap(":/assert/logo.png").scaledToHeight(ui->horizontalLayoutWidget->height()));

    // Initialize buttons
    initButton();

    // Initialize the QWebEngineView object
    initWebview();

    // Initialize the table of user information
    initTable();

    // Initialize casdoorSDK
    initCasdoorSDK();

    // Initialize the url of the login page
    initSigninUrl();

    // Initialize the TcpServer object
    initTcpServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initButton()
{
    ui->pushButton_signout->hide();
    ui->pushButton_signin->show();
}

void MainWindow::initWebview()
{
    // Initialize the QWebEngineView object
    m_webview = new QWebEngineView(this);
    m_webview->resize(this->width(), this->height());

    // Hide the webview
    m_webview->hide();

    connect(m_webview, SIGNAL(urlChanged(QUrl)), this, SLOT(on_auth_code_received(QUrl)));
}

void MainWindow::initTable()
{
    // Initialize the QTableView object
    m_tableview = new QTableView(this);
    m_tableview->move(190, 80);
    m_tableview->resize(340, 150);

    // Hide the table
    m_tableview->hide();
}

void MainWindow::initCasdoorSDK()
{
    m_casdoor = getCasdoorSDK();
}

void MainWindow::initSigninUrl()
{
    QString *url = new QString("");
    url->append(QString("%1/login/oauth/authorize").arg(m_casdoor->getEndPoint().c_str()));
    url->append(QString("?client_id=%1").arg(m_casdoor->getClientId().c_str()));
    url->append(QString("&response_type=%1").arg(response_type.c_str()));
    url->append(QString("&redirect_uri=%1").arg(redirect_url.c_str()));
    url->append(QString("&scope=%1").arg(scope.c_str()));
    url->append(QString("&state=%1").arg(app_name.c_str()));

    m_signin_url = new QUrl(*url);
}

void MainWindow::initTcpServer()
{
    // Initialize the TcpServer object and listen on port 8080
    m_tcpserver = new QTcpServer(this);
    if(!m_tcpserver->listen(QHostAddress::LocalHost, 8080)) {
        qDebug() << m_tcpserver->errorString();
        close();
    }

    connect(m_tcpserver, SIGNAL(newConnection()), this, SLOT(on_tcp_connected()));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // Override the resizeEvent function to set the control adaptive page
    ui->horizontalLayoutWidget->resize(this->width(), ui->horizontalLayoutWidget->height());
    ui->pushButton_signin->move((this->width() - ui->pushButton_signin->width()) * 0.5, ui->pushButton_signin->y());
    ui->pushButton_signout->move((this->width() - ui->pushButton_signout->width()) * 0.5, ui->pushButton_signout->y());

    m_webview->resize(this->width(), this->height());

    m_tableview->move((this->width() - m_tableview->width()) * 0.5, m_tableview->y());
}

void MainWindow::setWebView()
{
    // Load and display the login page of Casdoor
    m_webview->page()->load(*m_signin_url);
    m_webview->show();
}

void MainWindow::sendMessage()
{
    // Return empty response
    QString response = "";

    // Get the socket of the established connection
    QTcpSocket *tcpsocket = m_tcpserver->nextPendingConnection();

    // Send data
    tcpsocket->write(response.toUtf8());
    // Disconnect
    tcpsocket->disconnectFromHost();
    tcpsocket->close();
    tcpsocket = NULL;
}

CasdoorConfig* MainWindow::getCasdoorSDK()
{
    CasdoorConfig* casdoor = new CasdoorConfig(
        endpoint,
        client_id,
        client_secret,
        certificate,
        org_name
    );

    return casdoor;
}

void MainWindow::on_tcp_connected()
{
    // Hide the webview
    m_webview->hide();

    // Send response data
    sendMessage();

    // Show the button of signout
    ui->pushButton_signout->show();
}

void MainWindow::on_auth_code_received(QUrl url)
{
    if(url.toString().startsWith("http://localhost:8080/callback")) {
        // Parse the code
        QString code = "";
        QStringList querys = url.query().split("&");
        for(const QString& query : querys) {
            QStringList pair = query.split("=");
            if(pair[0] == "code") {
                code = pair[1];
                break;
            }
        }

        // Get token and parse it with the JWT library
        std::string token = m_casdoor->GetOAuthToken(code.toStdString());
        auto decoded = m_casdoor->ParseJwtToken(token);

        // Initialize tabular data model
        QStandardItemModel* model = new QStandardItemModel();

        // Set table title
        model->setHorizontalHeaderLabels({"Name", "Value"});

        // Fit all columns so that each element fills the space
        m_tableview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // Load a total of 4 rows of data, each row has 2 columns of data
        model->setItem(0, 0, new QStandardItem("user name"));
        model->setItem(0, 1, new QStandardItem(decoded.get_payload_claims().at("name").as_string().c_str()));

        model->setItem(1, 0, new QStandardItem("user id"));
        model->setItem(1, 1, new QStandardItem(decoded.get_payload_claims().at("id").as_string().c_str()));

        model->setItem(2, 0, new QStandardItem("user owner"));
        model->setItem(2, 1, new QStandardItem(decoded.get_payload_claims().at("owner").as_string().c_str()));

        model->setItem(3, 0, new QStandardItem("user application"));
        model->setItem(3, 1, new QStandardItem(decoded.get_payload_claims().at("signupApplication").as_string().c_str()));

        // Set tableview data and show table
        m_tableview->setModel(model);
        m_tableview->show();

        // Let the webview return to the login page after
        m_webview->back();
    }
}

void MainWindow::on_pushButton_signin_clicked()
{
    // hide the button of signin
    ui->pushButton_signin->hide();

    // Show the login page of Casdoor
    setWebView();
}

void MainWindow::on_pushButton_signout_clicked()
{
    // Hide the table of user information
    m_tableview->hide();

    // Hide the button of signout
    ui->pushButton_signout->hide();

    // Show the button of signin
    ui->pushButton_signin->show();
}
