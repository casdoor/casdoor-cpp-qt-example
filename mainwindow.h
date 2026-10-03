#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QUrl>
#include <QWebEnginePage>
#include <QWebEngineView>

#include <casdoor/casdoor.h>

namespace Ui {
class MainWindow;
}

// CallbackPage stops the embedded browser from loading the redirect URI and
// hands that URL, which carries the authorization code, to the app instead.
class CallbackPage : public QWebEnginePage
{
    Q_OBJECT

public:
    CallbackPage(const QUrl& redirectUri, QObject* parent);

signals:
    void callbackReceived(const QUrl& url);

protected:
    bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override;

private:
    QUrl m_redirectUri;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void on_pushButton_signin_clicked();
    void on_pushButton_signout_clicked();
    void onCallbackReceived(const QUrl& url);

private:
    void showUser(const casdoor::Claims& claims);

    Ui::MainWindow* ui;
    QWebEngineView* m_webview;
    QTableView* m_tableview;
    casdoor::Client m_casdoor;
    QString m_state;
};

#endif // MAINWINDOW_H
