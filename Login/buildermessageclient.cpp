#include "buildermessageclient.h"

#include <QCryptographicHash>
#include <sstream>
#include <QDebug>

void BuilderMessageClient::MessageSendToServer(QByteArray &byte,QJsonDocument jsonToSend)
{
    QDataStream out(&byte, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out << jsonToSend;
}

QJsonDocument BuilderMessageClient::MessageCursorChange(int pos, QString user, QString site)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",21);
    objtosend.insert("pos",pos);
    objtosend.insert("siteid", site);
    objtosend.insert("user",user);
    jsondoc.setObject(objtosend);
    return jsondoc;

}
void BuilderMessageClient::MessageSendToServer(QByteArray &byte, QByteArray &appendByte)
{
    QDataStream out(&byte, QIODevice::WriteOnly | QIODevice::Append);
    out.setVersion(QDataStream::Qt_5_13);
    out << appendByte;
}

//fileName mi serve per trovare il workspace, da sostituire con fileURI. Metto già il nome del campo aggiornato
QJsonDocument BuilderMessageClient::MessageInsert(QChar car, std::vector<int> posf, QString id, QString siteid,QTextCharFormat fmt )
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",18);
    objtosend.insert("car",QString(car));

    QJsonArray array;
    for(int i : posf)
        array.append(i);

    objtosend.insert("posfraz",array);
    objtosend.insert("id",id);
    objtosend.insert("siteid",siteid);

    QBuffer out;
    QDataStream data(&out);
    out.open(QIODevice::WriteOnly);
    data<<fmt;

    objtosend.insert("format",QLatin1String(out.data().toBase64()));
    jsondoc.setObject(objtosend);
    //qDebug() << "Sto mandando al server carattere " << car<< " con posf=" <<array<<" con id="<<id;

    return jsondoc;


}

//fileName mi serve per trovare il workspace, da sostituire con fileURI. Metto già il nome del campo aggiornato
QJsonDocument BuilderMessageClient::MessageDelete(QString fileName, QChar car, std::vector<int> posf, QString id, QString siteid)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",19);
    objtosend.insert("car",QString(car));

    QJsonArray array;
    for(int i : posf)
        array.append(i);

    objtosend.insert("posfraz",array);
    objtosend.insert("id",id);
    objtosend.insert("siteid",siteid);
    jsondoc.setObject(objtosend);
    return jsondoc;

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

QJsonDocument BuilderMessageClient::MessageCreateNewFile(QString fileName)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",12);
    objtosend.insert("auth",true);
    objtosend.insert("fileName",fileName);
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

// CHECK FIELD!!!!!!!!
QJsonDocument BuilderMessageClient::MessageOpenFile(QString URI, QString fileName)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",11);
    objtosend.insert("auth",true);
    objtosend.insert("fileName", fileName);
    objtosend.insert("URI",URI);
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

QJsonDocument BuilderMessageClient::MessagedDeleteFile(QString URI, QString fileName)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",17);
    objtosend.insert("auth",true);
    objtosend.insert("URI",URI);
    objtosend.insert("fileName", fileName);
    jsondoc.setObject(objtosend);
    return jsondoc;
}

QJsonDocument BuilderMessageClient::MessagedUpdateProfileRequest(QString nickname, QString password, QByteArray serializedImage)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",7);
    objtosend.insert("auth",true);

    if(nickname!=nullptr)
        objtosend.insert("nickname",nickname);
    else
        objtosend.insert("nickname","");

    if(password!=nullptr)
        objtosend.insert("password",password);
    else
        objtosend.insert("password","");

    if(serializedImage != nullptr)
        objtosend.insert("icon",QString::fromLatin1(serializedImage.toBase64().data()));
    else{
         objtosend.insert("icon","");
    }

    jsondoc.setObject(objtosend);
    return jsondoc;
}

// SARA' URI invece di filename?

QJsonDocument BuilderMessageClient::MessagedCloseEditor(QString fileName)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",20);
    objtosend.insert("auth",true);
     objtosend.insert("nomefile",fileName);
    jsondoc.setObject(objtosend);
    return jsondoc;
}

QJsonDocument BuilderMessageClient::MessageRemoveClientWorkspace(QString URI)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("URI", URI);
    objtosend.insert("auth",true);
    objtosend.insert("type",20);
    jsondoc.setObject(objtosend);
    return jsondoc;
}

QJsonDocument BuilderMessageClient::MessageShareFile(QString username, QString URI)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("URI", URI);
    objtosend.insert("username", username);
    objtosend.insert("auth",true);
    objtosend.insert("type",22);
    jsondoc.setObject(objtosend);
    return jsondoc;
}
