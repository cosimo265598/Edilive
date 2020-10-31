#include "homepage.h"
#include <QSizePolicy>

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

    emit fileHandlerDbClicked(dynamic_cast<FileHandler *>(QObject::sender())->getURI());
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
    }else if(qobject_cast<FileHandler *>(old) != nullptr && qobject_cast<QPushButton *>(now) != nullptr &&  qobject_cast<QPushButton *>(now)->objectName()=="pushButton_share"){
        shareFile();
    }
}

void HomePage::on_pushButton_new_file_clicked()
{

    QInputDialog diag;
    diag.setParent(this);
    diag.setTextValue("file name here");
    diag.setWindowFlags(Qt::Dialog);
    diag.setWindowTitle(tr("New file creation"));
    diag.setLabelText(tr("file name: "));
    diag.resize(400,0);
    int ret = diag.exec();

    if (ret == QDialog::Accepted){
         QString fileName=diag.textValue();
         if(fileName.isEmpty() || fileName.isNull()){
             QMessageBox::warning(this,tr("WARNING"),
                        tr("Invalid file name.\n"), QMessageBox::Ok);
             return;
         }else if(listfile.contains(fileName)){
             QMessageBox::critical(this, tr("WARNING"),"Duplicated File name", QMessageBox::Ok);
         }else{
             emit createNewFileRequest(fileName);
         }
    }
}

void HomePage::on_pushButton_profile_page_clicked()
{
    emit updateAccountClicked();
}


void HomePage::onReceivedFileHandlers(QJsonArray paths, QString onReload){
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

        sharedFileNameConflictManage(filename);

        FileHandler *item = new FileHandler(this, QIcon(":/icons_pack/document_480px.png"),filename, dir["URI"].toString(), dir["creator"].toString(), dir["created"].toString(),dir["size"].toInt());
        listfile[filename]=item;
        item->installEventFilter(eventFilter);
        connect(item, &FileHandler::clicked,[item, this]()
                    {ui->infoLabel->setText("File Selected:\t"+item->getFileName()+"\tCreator: "+
                                                 item->getCreated() + "\tSize: "+
                                                 item->getSize() +"\tCreated: "+
                                                 item->getCreated());});

        connect(item, &FileHandler::doubleClicked,this, &HomePage::onFileHandlerDbClicked);
        connect(item, &FileHandler::clicked,this, &HomePage::onFileHandlerClicked);

        ui->gridLayout->addWidget(item, row, column,{Qt::AlignTop,Qt::AlignLeft});
        column = (++column)%6;
        if(column==0) row++;
    }
    if(!onReload.isEmpty() && !onReload.isNull()){
        QMessageBox msgBox(QMessageBox::NoIcon,"",onReload,QMessageBox::Ok);
        msgBox.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::WindowCloseButtonHint);
        msgBox.exec();
    }
}

void HomePage::onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage){
    ui->username->setText(username);
    loadImage(serializedImage);
}

void HomePage::loadImage(QByteArray serializedImage){
    QPixmap pixmap;
    if (serializedImage == nullptr){
        //pixmap.load(QDir().homePath()+ "/QtProjects/pds-project/myservertest/Login/images/default.png");
        pixmap.load(QDir().homePath()+ "/QtProjects/pds-project/myservertest/Login/images/default.png");
    }else{
        qDebug() << "load";
        pixmap.loadFromData(serializedImage);
    }

    ui->accountImage->setPixmap( pixmap.scaled(150, 150, Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void HomePage::sharedFileNameConflictManage(QString &fileName)
{
    int i = 1;
    if(listfile.contains(fileName)){
        while(listfile.contains(fileName + '(' + i + ')'))
            i++;
        fileName = fileName + '(' + i + ')';
    }
}


void HomePage::onNewFileCreationFailure(QString errorMessage){
    QMessageBox::critical(this, tr("WARNING"),errorMessage);
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
        QString URI = selected -> getURI();
        selected = nullptr;
        emit deleteFileRequest(URI);
     }
}

void HomePage::shareFile()
{
    qDebug() << "Homepage Share";

    DialogShare *diag = new DialogShare(this, selected->getURI());
    connect(diag, SIGNAL(shareFile(QString, QString)), this, SIGNAL(shareFile(QString, QString)));
    diag->exec();

    selected = nullptr;

}
