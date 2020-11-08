#include "client.h"

#include <QCryptographicHash>


// Set of characters that will be used to generate random sequences as nonce
const QString Client::nonceCharacters = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

void Client::setUser(UserData *user)
{
    activeUser = user;
}

Client::Client(QWebSocket* s) :
    socket(s), activeUser(nullptr), logged(false)
{
}

Client::~Client()
{
    // NOTHING, activeUser is owned by Server and therefore it does NOT need to be destroyed
    qDebug()<<"Destructor Client";
}

QWebSocket* Client::getSocket() const
{
    return socket;

}


UserData* Client::getUser() const
{
    return activeUser;
}

int Client::getUserId() const
{
    if (activeUser == nullptr)
        return -1;
    else return activeUser->getUserId();
}

QString Client::getUsername() const
{
    if (activeUser == nullptr)
        return QString();
    else return activeUser->getUsername();
}

void Client::login(UserData* user)
{
    logged = true;
    activeUser = user;
}

void Client::logout()
{
    logged = false;
    activeUser = nullptr;
}

bool Client::isLogged()
{
    return logged;
}

bool Client::authenticate(QByteArray token)
{
    QCryptographicHash hash(QCryptographicHash::Sha512);

    // Internally solve the challenge by MD5-hashing the nonce with
    // the stored user password and compare the results
    hash.addData(activeUser->getPasswordHash());
    hash.addData(this->nonce);
    QString hashpwd(hash.result().toHex().data());
    if(hashpwd.compare(token.data())==0)
        return true;
    return false;
    //return !hash.result().compare(token);
}

QByteArray Client::challenge()
{
    for (int i = 0; i < 32; ++i)	// create a 32-character randomly generated nonce
    {
        int index = qrand() % nonceCharacters.length();
        QChar nextChar = nonceCharacters.at(index);
        nonce.append(nextChar);
    }

    return nonce;
}
