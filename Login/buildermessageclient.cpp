#include "buildermessageclient.h"

#include <QCryptographicHash>

void BuilderMessageClient::MessageSendToServer(QByteArray &byte,QJsonDocument jsonToSend)
{
    QDataStream out(&byte, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    out << jsonToSend;
}

void BuilderMessageClient::MessageSendToServer(QByteArray &byte, QByteArray &appendByte)
{
    QDataStream out(&byte, QIODevice::WriteOnly | QIODevice::Append);
    out.setVersion(QDataStream::Qt_5_14);
    out << appendByte;
}


QJsonDocument BuilderMessageClient::MessageTest(QString data)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",852147963);
    objtosend.insert("data",data);
    jsondoc.setObject(objtosend);
    return jsondoc;

}

QJsonDocument BuilderMessageClient::MessageLogin(QString username)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",1);
    objtosend.insert("username",username);
    jsondoc.setObject(objtosend);

    return jsondoc;

}

QJsonDocument BuilderMessageClient::MessageLoginUnlock(QString salt, QString nonce, QString psw)
{
    QCryptographicHash hash1(QCryptographicHash::Sha512);
    QCryptographicHash hash2(QCryptographicHash::Sha512);

    hash1.addData(psw.toUtf8());
    hash1.addData(salt.toUtf8());			// Solve the authentication challenge by hashing the nonce
                                            // with the inserted password and user's salt
    hash2.addData(hash1.result());
    hash2.addData(nonce.toUtf8());

    QJsonObject json;
    QJsonDocument jdoc;
    json.insert("type",3);
    json.insert("challange",hash2.result().toHex().data());
    jdoc.setObject(json);
    return jdoc;
}

QJsonDocument BuilderMessageClient::MessageRegisterAccount(QString usr, QString passwd)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",6);
    objtosend.insert("username",usr);
    objtosend.insert("password",passwd);
    jsondoc.setObject(objtosend);
    return jsondoc;

}

QJsonDocument BuilderMessageClient::MessageOpenDir()
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",10);
    objtosend.insert("auth",true);
    jsondoc.setObject(objtosend);
    return jsondoc;
}

QJsonDocument BuilderMessageClient::MessageCreateNewFile(QString nomefile)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",12);
    objtosend.insert("auth",true);
    objtosend.insert("nomefile",nomefile);
    jsondoc.setObject(objtosend);
    return jsondoc;
}

QJsonDocument BuilderMessageClient::MessageGenericErrorFile(QString data)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",13);
    objtosend.insert("auth",true);
    objtosend.insert("error",data);
    jsondoc.setObject(objtosend);
    return jsondoc;
}

QJsonDocument BuilderMessageClient::MessageOpenFile(QString nomefile)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",11);
    objtosend.insert("auth",true);
    objtosend.insert("nomefile",nomefile);
    jsondoc.setObject(objtosend);
    return jsondoc;
}

QJsonDocument BuilderMessageClient::MessageSubscriberInfoRequest()
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",16);
    objtosend.insert("auth",true);
    jsondoc.setObject(objtosend);
    return jsondoc;
}

QJsonDocument BuilderMessageClient::MessagedDeleteFile(QString nomefile)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",17);
    objtosend.insert("auth",true);
    objtosend.insert("nomefile",nomefile);
    jsondoc.setObject(objtosend);
    return jsondoc;
}
