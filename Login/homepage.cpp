#include "homepage.h"
#include "ui_homepage.h"
#include <QtCore/QDebug>
#include <QCoreApplication>
#include <QDir>
#include "buildermessageclient.h"

QT_USE_NAMESPACE

HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomePage),
    pixmap(new QPixmap()),
    eventFilter(new EventFilterImpl(this))
{
    ui->setupUi(this);
}

HomePage::~HomePage()
{
    delete ui;
    delete eventFilter;
    delete pixmap;
}

void HomePage::onFileHandlerClicked(){

    emit fileHandlerClicked(dynamic_cast<FileHandler *>(QObject::sender())->getFilename());
}

//??????
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
         QString fileName=diag.textValue();
         if(fileName.isEmpty() || fileName.isNull()){
             QMessageBox::warning(this,tr("WARNING"),
                        tr("Il nome del file non è valido.\n"));
             return;
         }

         emit createNewFileRequest(fileName);
    }
}

void HomePage::on_pushButton_aggiorna_vista_clicked()
{
    /*
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageOpenDir());
    client_socket->sendBinaryMessage(out);
    */
}

void HomePage::on_pushButton_profile_page_clicked()
{
    emit updateAccountClicked();
}


void HomePage::onReceivedFileHandlers(QJsonArray paths){

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

void HomePage::onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage){
    ui->username->setText(username);
    //this->pixmap->loadFromData(serializedImage);
    //ui->accountImage->setPixmap(*pixmap);
    loadImage();
}

void HomePage::loadImage(){

    QString path = QDir().homePath()+ "/QtProjects/pds-project/myservertest/Login/images/default.png";
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    this->pixmap->loadFromData(file.readAll());
    ui->accountImage->setPixmap(*pixmap);
    file.close();

}

void HomePage::onNewFileCreationFailure(QString errorMessage){
    QMessageBox::critical(this, tr("WARNING"),errorMessage,QMessageBox::Ok);
}

void HomePage::on_pushButton_Logout_clicked()
{
////TODO
}
