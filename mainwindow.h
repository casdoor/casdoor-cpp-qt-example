#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QtNetWork>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>

#include "casdoor_config.h"
#include "jwt-cpp/jwt.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QWebEngineView *m_webview;
    QTcpServer *m_tcpserver;
    QTableView *m_tableview;
    CasdoorConfig *m_casdoor;
    QUrl *m_signin_url;

    void initButton();
    void initWebview();
    void initTable();
    void initCasdoorSDK();
    void initSigninUrl();
    void initTcpServer();

    void resizeEvent(QResizeEvent *event);

    void setWebView();
    void sendMessage();

    CasdoorConfig* getCasdoorSDK();

private slots:
    void on_tcp_connected();
    void on_auth_code_received(QUrl url);
    void on_pushButton_signout_clicked();
    void on_pushButton_signin_clicked();
};

#endif // MAINWINDOW_H
