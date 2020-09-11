#include "buildermessage.h"


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

QJsonDocument BuilderMessage::MessageFileClientError(QString error)
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

