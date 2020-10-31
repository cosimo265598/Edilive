#include "buildermessage.h"
#include <QBuffer>
#include <QDebug>
#include <QDir>
#include <sstream>

void BuilderMessage::MessageSendToClient(QByteArray &byte,QJsonDocument jsonToSend)
{
    QDataStream out(&byte, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out << jsonToSend;
}


void BuilderMessage::MessageSendToClient(QByteArray &byte, QByteArray &appendByte)
{
    QDataStream out(&byte, QIODevice::WriteOnly | QIODevice::Append);
    out.setVersion(QDataStream::Qt_5_13);
    out << appendByte;
}

void BuilderMessage::MessageSendToClient(QByteArray &byte, SharedFile *newfile){

    QDataStream out(&byte, QIODevice::WriteOnly | QIODevice::Append);
    out.setVersion(QDataStream::Qt_5_13);

    unsigned long i=0;
    std::vector<Symbol> symbols = newfile->getSymbols();

    size_t max_json = 128 MByte;
    int last = 0;

    while(!last){

        QJsonObject json{{"remain",-1},{"symbols", QJsonArray{}}}; // empty jsonObj, just to have a first size
        size_t cur_json = 0;

        qDebug() << cur_json;

        QJsonArray symbolArray;

        for(; i<symbols.size(); i++){
            //qDebug() << "              "       << i;
            std::vector<int> posf = symbols[i].getPosFraz();
            std::string s(1,symbols[i].getCar());
            QJsonArray posArray;
            for(int val: posf)
               posArray.append(val);

            //qDebug() << "posf: " << posf << "    posArray:  " << posArray;

            QJsonObject obj{
                                {"car",QString::fromStdString(s)},
                                {"posfraz",posArray},
                                {"id",QString::fromStdString(symbols[i].getId())},
                                {"siteid",QString::fromStdString(symbols[i].getSite())}
                           };
            cur_json += sizeof (obj);
            qDebug() << cur_json;
            if(cur_json < max_json){
                symbolArray.append(obj);
            }

            else{
                break; // ottimizzazione, ora viene ricreato di nuovo l'ultimo obj
            }
        }

        if (i >= symbols.size())
            last = 1;

        json.insert("last",last);
        json.insert("symbols",symbolArray);
        QJsonDocument doc = QJsonDocument(json);
        // qDebug() << "\n=============================================================\n";
        //qDebug() << json["last"].toInt();
        //qDebug() << json["symbols"].toArray();
        out << doc;
    }
}


QJsonDocument BuilderMessage::MessageInsert(char car, std::vector<int> posf, QString id, QString siteid/*, QString iniziale*/)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",50);
    std::string s(1,car);
    objtosend.insert("car",QString::fromStdString(s));
    //objtosend.insert("iniziale",iniziale);
    std::string posfstringa="";
    for(unsigned long i=0; i<posf.size(); i++){
        std::stringstream ss;
        ss << posf[i];
        std::string str = ss.str();
        if(i==posf.size()-1)
            posfstringa.append(str);
        else
            posfstringa.append(str).append("-");
    }

    objtosend.insert("posfraz",QString::fromStdString(posfstringa));
    objtosend.insert("id",id);
    objtosend.insert("siteid",siteid);
    jsondoc.setObject(objtosend);
    return jsondoc;

}


QJsonDocument BuilderMessage::MessageConflictInsert(char car, std::vector<int> newposf, std::vector<int> oldposf, QString id, QString siteid)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",51);
    std::string s(1,car);
    objtosend.insert("car",QString::fromStdString(s));
    std::string posfstringa="";
    for(int i=0; i<newposf.size(); i++){
        std::stringstream ss;
        ss << newposf[i];
        std::string str = ss.str();
        if(i==newposf.size()-1)
            posfstringa.append(str);
        else
            posfstringa.append(str).append("-");
    }
    objtosend.insert("newposfraz",QString::fromStdString(posfstringa));
    posfstringa="";
    for(int i=0; i<oldposf.size(); i++){
        std::stringstream ss;
        ss << oldposf[i];
        std::string str = ss.str();
        if(i==oldposf.size()-1)
            posfstringa.append(str);
        else
            posfstringa.append(str).append("-");
    }
    objtosend.insert("newposfraz",QString::fromStdString(posfstringa));
    objtosend.insert("id",id);
    objtosend.insert("siteid",siteid);
    jsondoc.setObject(objtosend);
    return jsondoc;

}

QJsonDocument BuilderMessage::MessageDelete(char car, std::vector<int> posf, QString id, QString siteid)
{
    QJsonDocument jsondoc;
    QJsonObject objtosend;
    objtosend.insert("type",52);
    std::string s(1,car);
    objtosend.insert("car",QString::fromStdString(s));
    std::string posfstringa="";
    for(int i=0; i<posf.size(); i++){
        std::stringstream ss;
        ss << posf[i];
        std::string str = ss.str();
        if(i==posf.size()-1)
            posfstringa.append(str);
        else
            posfstringa.append(str).append("-");
    }
    objtosend.insert("posfraz",QString::fromStdString(posfstringa));
    objtosend.insert("id",id);
    objtosend.insert("siteid",siteid);
    jsondoc.setObject(objtosend);
    return jsondoc;

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

QJsonDocument BuilderMessage::MessageAccountCreationError(QString data)
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

QJsonDocument BuilderMessage::MessageOpenDirOfClient(QJsonArray listfiles, QString onReload)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",10);
    json.insert("onReload", onReload);
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

QJsonDocument BuilderMessage::MessageHeaderFile(QString fileName, QString creator, QList<QSharedPointer<Client>> connected)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    QJsonArray array;



    for(auto client : connected){
        QByteArray icon;
        QBuffer buffer(&icon);
        buffer.open(QIODevice::WriteOnly);
        client->getUser()->getIcon().save(&buffer);

        array.append(QJsonObject{
                         {"username",client->getUser()->getUsername()},
                         {"nickname", client->getUser()->getNickname()},
                         {"icon", QLatin1String(buffer.data().toBase64())}
                     });
        buffer.close();
    }

    json.insert("type",11);
    json.insert("fileName",fileName);
    json.insert("creator",creator);
    json.insert("connected",array);
    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageAccountInfo(QString username, QString nickname, QImage ico)
{
    bool presentIcon=false;
    if(!ico.isNull())
        presentIcon=true;

    qDebug()  << presentIcon;

    QByteArray icon;
    QBuffer buffer(&icon);
    buffer.open(QIODevice::WriteOnly);
    ico.save(&buffer);

    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",17);
    json.insert("username",username);
    json.insert("nickname",nickname);
    json.insert("icon",QLatin1String(buffer.data().toBase64()));
    json.insert("icon_present", presentIcon);
    jsondoc.setObject(json);

    buffer.close();

    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageAccountUpdateError(QString error)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",21);
    json.insert("error",error);
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

QJsonDocument BuilderMessage::MessageAccountUpdateSuccess(QString msg){
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",20);
    json.insert("message",msg);
    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageInsertClientWorkspace(QString username, QString nickname, QImage ico)
{
    bool presentIcon=false;
    if(!ico.isNull())
        presentIcon=true;

    qDebug()  << presentIcon;

    QByteArray icon;
    QBuffer buffer(&icon);
    buffer.open(QIODevice::WriteOnly);
    ico.save(&buffer);

    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",100);
    json.insert("username",username);
    json.insert("nickname",nickname);
    json.insert("icon",QLatin1String(buffer.data().toBase64()));
    json.insert("icon_present", presentIcon);
    jsondoc.setObject(json);

    buffer.close();

    jsondoc.setObject(json);
    return jsondoc;

}

QJsonDocument BuilderMessage::MessageRemoveClientFromWorkspace(QString username){
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",101);
    json.insert("username", username);
    jsondoc.setObject(json);
    return jsondoc;
}

QJsonDocument BuilderMessage::MessageRequestShareFile(QString URI, QString userSending)
{
    QJsonDocument jsondoc;
    QJsonObject json;
    json.insert("type",102);
    json.insert("userSending", userSending);
    json.insert("URI", URI);
    jsondoc.setObject(json);
    return jsondoc;
}

