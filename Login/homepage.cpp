#include "homepage.h"

QT_USE_NAMESPACE

HomePage::HomePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomePage),
    row(0),
    column(0),
    eventFilter(new EventFilterImpl(this)),
    selected(nullptr)
{
    ui->setupUi(this);
    //Prototype -> QApplication::focusChanged(QWidget *old, QWidget *now)
    connect(qApp, &QApplication::focusChanged, this, &HomePage::onFocusChange);

}
HomePage::~HomePage()
{
    delete ui;
    delete eventFilter;
    delete selected;
}

void HomePage::onFileHandlerDbClicked(){

    emit fileHandlerDbClicked(dynamic_cast<FileHandler *>(QObject::sender())->getFilename());
}

void HomePage::onFileHandlerClicked(){
    qDebug() << "File Handler";
    selected = dynamic_cast<FileHandler *>(QObject::sender());
    selected->setFocus();

    //During the construction of every FileHandler, a style sheet is set in order to have border:none
    // If element is highlighted, I want a border shown
    selected->setStyleSheet("");
}

void HomePage::onFocusChange(QWidget *old, QWidget *now){
    if(qobject_cast<FileHandler *>(old) != nullptr && qobject_cast<QScrollArea *>(now) != nullptr){
        //Reset the original stylesheet with no border if the element is no more focused
        selected->setStyleSheet("QToolButton{border:none;}");
        selected = nullptr;

    }else if(qobject_cast<FileHandler *>(old) != nullptr && qobject_cast<QPushButton *>(now) != nullptr &&  qobject_cast<QPushButton *>(now)->objectName()=="pushButton_delete"){
        qDebug() << "delete";
        deleteFile();
    }
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

//NON credo serva
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
        connect(item, &FileHandler::doubleClicked,this, &HomePage::onFileHandlerDbClicked);
        connect(item, &FileHandler::clicked,this, &HomePage::onFileHandlerClicked);

        ui->gridLayout->addWidget(item, row, column,{Qt::AlignTop,Qt::AlignLeft});
        column = (++column)%6;
        if(column==0) row++;
    }
}

void HomePage::onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage){
    ui->username->setText(username);
    loadImage(serializedImage);
}

void HomePage::loadImage(QByteArray serializedImage){
    QPixmap pixmap;
    if (serializedImage == nullptr){
        pixmap.load(":/icons_pack/avatar_default.png");
    }else{
        qDebug() << "load";
        pixmap.loadFromData(serializedImage,"PNG"); //Check if PNG, what happens if not?
    }

    ui->accountImage->setPixmap( pixmap.scaled(150, 150, Qt::KeepAspectRatio,Qt::SmoothTransformation));
}


void HomePage::onNewFileCreationFailure(QString errorMessage){
    QMessageBox::critical(this, tr("WARNING"),errorMessage,QMessageBox::Ok);
}

void HomePage::on_pushButton_Logout_clicked()
{
////TODO
}

void HomePage::deleteFile(){
     QMessageBox delMsgBox{QMessageBox::Warning,tr("WARNING"),"Delete the selected file?",QMessageBox::Ok,this};
     delMsgBox.addButton(QMessageBox::Cancel);
     if(delMsgBox.exec()==QMessageBox::Ok){
        qDebug() << "OK";
        QString fileName = selected -> getFilename();
        selected = nullptr;
        emit deleteFileRequest(fileName);
     }
}
