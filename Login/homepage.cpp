#include "homepage.h"
#include "ui_homepage.h"
#include <QtCore/QDebug>
#include <QCoreApplication>
#include "buildermessageclient.h"

QT_USE_NAMESPACE

HomePage::HomePage(QWebSocket *socket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomePage),
    client_socket(socket),
    profilePage(new ProfilePage(this,client_socket))
{
    ui->setupUi(this);
    ui->stackedWidget->addWidget(profilePage);
    connect(profilePage, &ProfilePage::returnToHome, [this](){ui->stackedWidget->setCurrentIndex(0);});
    this->eventFilter = new EventFilterImpl(this);

    HomePage::show();

    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageOpenDir());
    client_socket->sendBinaryMessage(out);
}

HomePage::~HomePage()
{
    delete ui;
    delete eventFilter;
}

void HomePage::onFileHandlerClicked(){
    QString nomefile = dynamic_cast<FileHandler *>(QObject::sender())->getFilename();

    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageOpenFile(nomefile));
    client_socket->sendBinaryMessage(out);

}

void HomePage::createHomepage(QJsonArray paths){

    // clean the view for future update // da rivedere
    while(ui->gridLayout->count() ) {
        QWidget* widget = ui->gridLayout->itemAt(0)->widget();
        if( widget ) {
        ui->gridLayout->layout()->removeWidget(widget);
        delete widget;
        }
    }
    int row = 0;
    int column = 0;

    for(auto entry: paths){
        QJsonObject dir = entry.toObject();
        QString filename = dir["filename"].toString();
        listfile.append(filename);


        FileHandler *item = new FileHandler(this, QIcon(":/icons_pack/document_480px.png"),filename, "./"+dir["filename"].toString(), dir["owner"].toString(), dir["lastModified"].toString(),dir["lastRead"].toString());
        item->installEventFilter(eventFilter);
        connect(item, &FileHandler::clicked,[filename,dir,this]()
                    {ui->infoLabel->setText("File Selected:\t"+filename+"\tOwner: "+
                                                 dir["owner"].toString()+"\tLast Modified: "+
                                                 dir["lastModified"].toString()+ "\tSize: "+
                                                 dir["size"].toString() );   });
        connect(item, &FileHandler::doubleClicked,this, &HomePage::onFileHandlerClicked);
        ui->gridLayout->addWidget(item, row, column,{Qt::AlignTop,Qt::AlignLeft});
        qDebug()<<row<< " "<<column;
        column = (++column)%6;
        if(column==0) row++;
    }
}

ProfilePage *HomePage::getProfilePage()
{
    return this->profilePage;
}

void HomePage::openReceivedFile(QByteArray data){
    QString nomeuser="cosimo";
    QFile file("/home/"+nomeuser+"/tmp/prova.txt"); // change path
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}

void HomePage::on_pushButton_new_file_clicked()
{

    QInputDialog diag;
    diag.setParent(this);
    diag.setTextValue("senzanome");
    diag.setWindowFlags(Qt::Dialog);
    diag.setWindowTitle(tr("Creazione nuovo file"));
    diag.setLabelText(tr("Nome file: "));
    diag.resize(400,0);
    int ret = diag.exec();

    if (ret == QDialog::Accepted){
         QString filename=diag.textValue();
         if(filename.isEmpty() || filename.isNull()){
             QMessageBox::warning(this,tr("Errore creazione file"),
                        tr("Il nome del file non è valido.\n"));
             return;
         }
         QByteArray out;
         BuilderMessageClient::MessageSendToServer(
                     out,
                     BuilderMessageClient::MessageCreateNewFile(filename));
         client_socket->sendBinaryMessage(out);
    }
}

void HomePage::on_pushButton_aggiorna_vista_clicked()
{
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageOpenDir());
    client_socket->sendBinaryMessage(out);
}

void HomePage::on_pushButton_profile_page_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
