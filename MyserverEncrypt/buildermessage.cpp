#include "buildermessage.h"
#include <QBuffer>
#include <QDebug>
#include <QDir>

void BuilderMessage::MessageSendToClient(QByteArray &byte,QJsonDocument jsonToSend)
{
    QDataStream out(&byte, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    out << jsonToSend;
}

void BuilderMessage::MessageSendToClient(QByteArray &byte, QByteArray &appendByte)
{
    QDataStream out(&byte, QIODevice::WriteOnly | QIODevice::Append);
    out.setVersion(QDataStream::Qt_5_14);
    out << appendByte;
}


QJsonDocument BuilderMessage::MessageLogin()
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",1);
    jsondoc.setObject(json);
    return  jsondoc;
}

QJsonDocument BuilderMessage::MessageChallege(QString salt,QString nonce)
{
    QJsonDocument jsondoc;
    QJsonObject jsonchallange;
    jsonchallange.insert("type",2);
    jsonchallange.insert("salt",salt);
    jsonchallange.insert("nonce",nonce);
    jsondoc.setObject(jsonchallange);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageChallegePassed(QString data)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",4);
    json.insert("auth",true);
    jsondoc.setObject(json);
    return jsondoc;

}
QJsonDocument BuilderMessage::MessageLoginError(QString data)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",5);
    json.insert("auth",false);
    json.insert("error",data);
    jsondoc.setObject(json);
    return jsondoc;

}

QJsonDocument BuilderMessage::MessageAccountError(QString data)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",9);
    json.insert("error",data);
    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageAccountConfirmed(QString data)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",8);
    json.insert("confirmed",data);
    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageOpenDirOfClient(QJsonArray listfiles)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",10);
    json.insert("files",listfiles);
    // possibilita di aggiungere la visita delle directory
    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageFileCreationError(QString error)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",13);
    json.insert("error",error);
    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageHeaderFile(QString fileName)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",11);
    json.insert("fileName",fileName);
    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageAccountInfo(QString username, QString nickname, QImage ico_real)
{
    bool presentIcon=false;
    if(!ico_real.isNull())
        presentIcon=true;

    //test
    qDebug() << QDir().currentPath()+ "/images/default.png";
    QImage ico(QDir().currentPath()+ "/images/default.png");
    //

    QByteArray icon;
    QBuffer buffer(&icon);
    buffer.open(QIODevice::WriteOnly);
    ico.save(&buffer, "PNG");	// writes image into the bytearray in PNG format

    //

    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",17);
    json.insert("username",username);
    json.insert("nickname",nickname);
    json.insert("icon",QLatin1String(buffer.data().toBase64()));
    json.insert("icon_present", presentIcon);

    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageFileDeletionError(QString error)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",19);
    json.insert("error",error);
    jsondoc.setObject(json);
    return jsondoc;
}

