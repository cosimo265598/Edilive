#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QDebug>
#include <QtWebSockets/QWebSocket>
#include <QCoreApplication>
#include <QBuffer>
#include <iostream>
#include <QDebug>
#include <QCursor>
#include "user.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_webSocket, &QWebSocket::connected, this, &MainWindow::onConnected);
    connect(&m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),this, &MainWindow::onSslErrors);
    connect(ui->inviatesto,&QPlainTextEdit::textChanged,this,&MainWindow::change);
    connect(ui->inviatesto,&QPlainTextEdit::cursorPositionChanged,this,&MainWindow::changePosition);

    m_webSocket.open(QStringLiteral("wss://localhost:1234"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnected()
{
    ui->ricevuta->appendPlainText("WebSocket connected");
    connect(&m_webSocket, &QWebSocket::textMessageReceived,this, &MainWindow::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived,this, &MainWindow::onTextMessageReceivedBytes);
}

void MainWindow::onTextMessageReceived(QString message)
{
    ui->ricevuta->appendPlainText("Message received:\t"+message);
    QTextCursor c(ui->inviatesto->textCursor());
    c.setPosition(0,QTextCursor::KeepAnchor);
    bool oldState = ui->inviatesto->blockSignals(true);
    //ui->inviatesto->appendPlainText(message);
    c.insertText(message);
    ui->inviatesto->blockSignals(oldState);
}

void MainWindow::onSslErrors(const QList<QSslError> &errors)
{

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.

    m_webSocket.ignoreSslErrors();
}


void MainWindow::on_invia_clicked()
{
    QString message(ui->inviatesto->toPlainText());
    m_webSocket.sendTextMessage(message);
    //QTextCursor c(ui->inviatesto->textCursor());
    //c.setPosition(2,QTextCursor::MoveAnchor);
    //c.insertText("PROVA AGGIUNTA");

    //m_webSocket.sendTextMessage(c&);
}

void MainWindow::change(){
    QString message(ui->inviatesto->toPlainText());
    m_webSocket.sendTextMessage(message);
}

void MainWindow::changePosition(){
   //Posizione cursore modificato
    //QTextCursor c(ui->inviatesto->textCursor());
    //m_webSocket.sendTextMessage(QString::number(c.position()));

    //***************** PROVA invio sul buffer oggetti serializzati ******************
    User a("new","user",10);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    //out<<QString("Posizione: ")<<(quint32)c.position();
    out<<a;

    m_webSocket.sendBinaryMessage(block);

}

void MainWindow::onTextMessageReceivedBytes(const QByteArray& message){

    QByteArray b(message);
    QBuffer buffer(&b);
    buffer.open(QIODevice::ReadOnly);

    QDataStream streamSentToObject(&buffer);


    ui->ricevuta->appendPlainText("Message received:\t"+QString(b)+"----" + message);

}




void MainWindow::on_pushButton_clicked()
{
    //***************** PROVA invio sul buffer oggetti serializzati ******************
    User a("I'm new user","my psw",10);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    out<<(quint16)0<<QString("Hello message tests");
    m_webSocket.sendBinaryMessage(block);

}
