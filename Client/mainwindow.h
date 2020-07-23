#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QTextCursor>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //friend QDataStream &operator<<(QDataStream &, const QString &);
    //friend QDataStream &operator>>(QDataStream &, QString &);
private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString message);
    void onTextMessageReceivedBytes(const QByteArray &message);

    void onSslErrors(const QList<QSslError> &errors);

    void on_invia_clicked();
    void change();
    void changePosition();
    void on_pushButton_clicked();

private:
    QWebSocket m_webSocket;
    Ui::MainWindow *ui;
    QTextCursor c;
};
#endif // MAINWINDOW_H
