#ifndef USERDATA_H
#define USERDATA_H

#include <QMap>
#include <QList>
#include <QByteArray>

#define MAX_NAME_LENGTH	  50			  // 50 characters
#define MAX_IMAGE_SIZE	  5*1024*1024      // 5 MByte

struct file_t{
    QString fileName;
    QString URI;
    QString creator;
    QString created;
};

class UserData
{
    /* Operators for QDataStream serialization and deserialization */
    //friend QDataStream& operator>>(QDataStream& in, UserData& user);			// Input deserialization
    //friend QDataStream& operator<<(QDataStream& out, const UserData& user);		// Output serialization

private:

    QString m_username;
    int m_userId;
    QString m_nickname;
    QByteArray m_passwd;		// hashed
    QByteArray m_salt;			// randomly generated
    QByteArray m_icon;
    QMap<QString, file_t> files;

    static const QString saltCharacters;

public:

    UserData();	 // Use this to construct an empty user and populate the fields later

    UserData(QString username, int userId, QString nickname, QString passwd, QByteArray icon = QByteArray());
    UserData(QString username, int userId, QString nickname, QByteArray passhash, QByteArray salt, QByteArray icon);

    ~UserData();

    bool isEmpty();
    /* getters */
    int getUserId() const;
    QString getUsername() const;
    QString getNickname() const;
    QByteArray getPasswordHash() const;
    QByteArray getSalt() const;
    QByteArray getIcon() const;
    QList<file_t> getFiles() const;
    bool hasFile(QString fileName) const;
    //QString getURIat(int index) const;

    /* setters */
    void addFile(file_t file);
    void removeDocument(QString fileName);
    void setNickname(QString newNickname);
    void setIcon(QByteArray newIcon);
    void setPassword(QString newPassword);
    void update(QString nickname, QString password, QByteArray icon);
    //void rollback(const UserData& backup);
    void generateSalt(QByteArray &salt);
    void setUserId(int id);

};

#endif // USERDATA_H
