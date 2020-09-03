#include "clientfilesystem.h"
#include "ui_clientfilesystem.h"
#include <QtCore/QDebug>
#include <QCoreApplication>
#include "buildermessageclient.h"

QT_USE_NAMESPACE

ClientFilesystem::ClientFilesystem(QWidget *parent,QWebSocket *socket) :
    QMainWindow(parent), ui(new Ui::ClientFilesystem), client_socket(socket)
{
    ui->setupUi(this);
    this->eventFilter = new EventFilterImpl(this);
    client_socket->sendBinaryMessage(BuilderMessageClient::MessageOpenDir().toJson());
}


ClientFilesystem::~ClientFilesystem()
{
    delete ui;
    delete eventFilter;
}

void ClientFilesystem::onFileHandlerClicked(){
    QJsonObject openDir{};
    openDir["type"] = "openFile";
    openDir["path"] = dynamic_cast<FileHandler *>(QObject::sender())->getPath();
    client_socket->sendBinaryMessage(QJsonDocument{openDir}.toJson(QJsonDocument::Indented));
}

void ClientFilesystem::createHomepage(QJsonArray paths){

    // clean the view for future update // da rivedere
    while(ui-> gridLayout->count() ) {
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
                    {ui->InfoFile_label->setText("File Selected:\t"+filename+"\tOwner: "+
                                                 dir["owner"].toString()+"\tLast Modified: "+
                                                 dir["lastModified"].toString());   });
        connect(item, &FileHandler::doubleClicked,this, &ClientFilesystem::onFileHandlerClicked);
        ui->gridLayout->addWidget(item, row, column,{Qt::AlignTop,Qt::AlignLeft});
        qDebug()<<row<< " "<<column;
        column = (++column)%6;
        if(column==0) row++;
    }
}

void ClientFilesystem::openReceivedFile(QByteArray data){
    QString nomeuser="cosimo";
    QFile file("/home/"+nomeuser+"/tmp/prova.txt"); // change path
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}

void ClientFilesystem::on_pushButton_new_file_clicked()
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
         this->client_socket->sendBinaryMessage(BuilderMessageClient::MessageCreateNewFile(filename).toJson());
    }

}

void ClientFilesystem::on_pushButton_aggiorna_vista_clicked()
{
    client_socket->sendBinaryMessage(BuilderMessageClient::MessageOpenDir().toJson());
}
