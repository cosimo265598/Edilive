#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <clientfilesystem.h>
#include <textedit.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_sign_clicked();
    void fieldForSign(bool state);

    void on_back_clicked();

    void on_login_clicked();
    void onConnected();
    void onSslErrors(const QList<QSslError> &errors);
    void on_sign_account_clicked();
    void MessageReceivedFromServer(const QByteArray& message);
    void Registeruser();
    void StartNewWindows();
    void StartEditorText(QString fileeditor);

private:
    Ui::MainWindow *ui;
    ClientFilesystem *secondWindows;
    TextEdit *editor;
    QSharedPointer<QWebSocket> m_webSocket;
    QString urlForConnection;

};
#endif // MAINWINDOW_H
