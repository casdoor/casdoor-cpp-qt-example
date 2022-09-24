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
    QString *url = new QString("http://localhost:8000/login/oauth/authorize");
    url->append(QString("?client_id=%1").arg(m_casdoor->getClientId().c_str()));
    url->append(QString("&response_type=%1").arg("code"));
    url->append(QString("&redirect_uri=%1").arg("http://localhost:8080/callback"));
    url->append(QString("&scope=%1").arg("read"));
    url->append(QString("&state=%1").arg("app-built-in"));

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
    std::string cert = R"(-----BEGIN CERTIFICATE-----
MIIE+TCCAuGgAwIBAgIDAeJAMA0GCSqGSIb3DQEBCwUAMDYxHTAbBgNVBAoTFENh
c2Rvb3IgT3JnYW5pemF0aW9uMRUwEwYDVQQDEwxDYXNkb29yIENlcnQwHhcNMjEx
MDE1MDgxMTUyWhcNNDExMDE1MDgxMTUyWjA2MR0wGwYDVQQKExRDYXNkb29yIE9y
Z2FuaXphdGlvbjEVMBMGA1UEAxMMQ2FzZG9vciBDZXJ0MIICIjANBgkqhkiG9w0B
AQEFAAOCAg8AMIICCgKCAgEAsInpb5E1/ym0f1RfSDSSE8IR7y+lw+RJjI74e5ej
rq4b8zMYk7HeHCyZr/hmNEwEVXnhXu1P0mBeQ5ypp/QGo8vgEmjAETNmzkI1NjOQ
CjCYwUrasO/f/MnI1C0j13vx6mV1kHZjSrKsMhYY1vaxTEP3+VB8Hjg3MHFWrb07
uvFMCJe5W8+0rKErZCKTR8+9VB3janeBz//zQePFVh79bFZate/hLirPK0Go9P1g
OvwIoC1A3sarHTP4Qm/LQRt0rHqZFybdySpyWAQvhNaDFE7mTstRSBb/wUjNCUBD
PTSLVjC04WllSf6Nkfx0Z7KvmbPstSj+btvcqsvRAGtvdsB9h62Kptjs1Yn7GAuo
I3qt/4zoKbiURYxkQJXIvwCQsEftUuk5ew5zuPSlDRLoLByQTLbx0JqLAFNfW3g/
pzSDjgd/60d6HTmvbZni4SmjdyFhXCDb1Kn7N+xTojnfaNkwep2REV+RMc0fx4Gu
hRsnLsmkmUDeyIZ9aBL9oj11YEQfM2JZEq+RVtUx+wB4y8K/tD1bcY+IfnG5rBpw
IDpS262boq4SRSvb3Z7bB0w4ZxvOfJ/1VLoRftjPbLIf0bhfr/AeZMHpIKOXvfz4
yE+hqzi68wdF0VR9xYc/RbSAf7323OsjYnjjEgInUtRohnRgCpjIk/Mt2Kt84Kb0
wn8CAwEAAaMQMA4wDAYDVR0TAQH/BAIwADANBgkqhkiG9w0BAQsFAAOCAgEAn2lf
DKkLX+F1vKRO/5gJ+Plr8P5NKuQkmwH97b8CS2gS1phDyNgIc4/LSdzuf4Awe6ve
C06lVdWSIis8UPUPdjmT2uMPSNjwLxG3QsrimMURNwFlLTfRem/heJe0Zgur9J1M
8haawdSdJjH2RgmFoDeE2r8NVRfhbR8KnCO1ddTJKuS1N0/irHz21W4jt4rxzCvl
2nR42Fybap3O/g2JXMhNNROwZmNjgpsF7XVENCSuFO1jTywLaqjuXCg54IL7XVLG
omKNNNcc8h1FCeKj/nnbGMhodnFWKDTsJcbNmcOPNHo6ixzqMy/Hqc+mWYv7maAG
Jtevs3qgMZ8F9Qzr3HpUc6R3ZYYWDY/xxPisuKftOPZgtH979XC4mdf0WPnOBLqL
2DJ1zaBmjiGJolvb7XNVKcUfDXYw85ZTZQ5b9clI4e+6bmyWqQItlwt+Ati/uFEV
XzCj70B4lALX6xau1kLEpV9O1GERizYRz5P9NJNA7KoO5AVMp9w0DQTkt+LbXnZE
HHnWKy8xHQKZF9sR7YBPGLs/Ac6tviv5Ua15OgJ/8dLRZ/veyFfGo2yZsI+hKVU5
nCCJHBcAyFnm1hdvdwEdH33jDBjNB6ciotJZrf/3VYaIWSalADosHAgMWfXuWP+h
8XKXmzlxuHbTMQYtZPDgspS5aK+S4Q9wb8RRAYo=
-----END CERTIFICATE-----)";
    CasdoorConfig* casdoor = new CasdoorConfig(
        "http://localhost:8000",
        "3efd29ff3e0b14ba1dd7",
        "34cb65d634b06a49f14c6bc49884ce1df55ce518",
        cert,
        "built-in"
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
