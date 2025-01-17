#include "userdata.h"

#include <QDataStream>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QDebug>

// carateri per il salt alla passw
const QString UserData::saltCharacters = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");


void UserData::setUserId(int userId)
{
    m_userId = userId;
}

UserData::UserData() : m_userId(-1) { }

UserData::UserData(QString username, int userId, QString nickname, QString passwd, QByteArray icon)
    : m_username(username), m_userId(userId), m_nickname(nickname), m_icon(icon)
{
    QCryptographicHash hash(QCryptographicHash::Sha512);

    generateSalt(m_salt);

    hash.addData(passwd.toUtf8());
    hash.addData(m_salt);

    m_passwd = hash.result();
}

UserData::UserData(QString username, int userId, QString nickname, QByteArray passhash, QByteArray salt, QByteArray icon)
    : m_username(username), m_userId(userId), m_nickname(nickname), m_passwd(passhash), m_salt(salt), m_icon(icon)
{
}

UserData::~UserData() { }

bool UserData::isEmpty()
{
    if(this->m_userId==-1)
        return true;
    return false;
}

QString UserData::getUsername() const
{
    return m_username;
}

int UserData::getUserId() const
{
    return m_userId;
}

QString UserData::getNickname() const
{
    return m_nickname;
}

QByteArray UserData::getPasswordHash() const
{
    return m_passwd;
}

QByteArray UserData::getIcon() const
{
    return m_icon;
}

QByteArray UserData::getSalt() const
{
    return m_salt;
}

bool UserData::hasFile(QString fileName) const
{
    return files.contains(fileName);
}

QList<file_t> UserData::getFiles() const
{
    return files.values();
}

/*
QString UserData::getURIat(int index) const
{
    return m_documents.at(index);
}
*/

void UserData::addFile(file_t file)
{

    /*QString name = file.fileName;
    int i = 1;
    if(this->files.contains(file.fileName)){
        while(files.contains(QString(file.fileName + '(' + QString::number(i) + ')')))
            i++;

        //qDebug()<< "File conflict " << QString(file.fileName + '(' + QString::number(i) + ')');


        name = file.fileName.append(QString('(' + QString::number(i) + ')'));
        //qDebug()<< file.fileName;
    }

    file.fileName = name;*/
    files.insert(file.fileName, file);
}

void UserData::removeDocument(QString fileName)
{
    files.remove(fileName);
}

void UserData::setNickname(QString newNickname)
{
    m_nickname = newNickname;
}

void UserData::setIcon(QByteArray newIcon)
{
    m_icon = newIcon;
}

void UserData::setPassword(QString newPassword)
{
    QCryptographicHash hash(QCryptographicHash::Sha512);
    generateSalt(m_salt);
    hash.addData(newPassword.toUtf8());
    hash.addData(m_salt);

    m_passwd = hash.result();
}

void UserData::update(QString nickname, QString password, QByteArray icon)
{
    //qDebug() << nickname.isEmpty();
    if(!nickname.isEmpty())
        setNickname(nickname);
    if (!icon.isNull())
        setIcon(icon);
    if (!password.isEmpty())
        setPassword(password);
}

/*
void UserData::rollback(const UserData& backup)
{
    m_nickname = backup.getNickname();
    m_icon = backup.getIcon();
    m_passwd = backup.getPasswordHash();
    m_salt = backup.getSalt();
    m_documents = backup.getDocuments();
}
*/
void UserData::generateSalt(QByteArray &m_salt){
    m_salt.clear();
    for (int i = 0; i < 32; ++i)	// 32 caratteri
        m_salt.append(saltCharacters.at(qrand() % saltCharacters.length()));
}


/*
QDataStream& operator>>(QDataStream& in, UserData& user)
{
    // Object deserialized reading field by field from the stream

    in  >> user.m_username
        >> user.m_userId
        >> user.m_nickname
        >> user.m_passwd
        >> user.m_salt
        >> user.m_icon
        >> user.m_documents;

    return in;
}
*/
/*
QDataStream& operator<<(QDataStream& out, const UserData& user)
{
    // Object serialized as the sequence of its member fields

    out << user.m_username
        << user.m_userId
        << user.m_nickname
        << user.m_passwd
        << user.m_salt
        << user.m_icon
        << user.m_documents;

    return out;
}
*/
