#ifndef USERDATA_H
#define USERDATA_H

#include <QImage>
#include <QList>
#include <QByteArray>

#define MAX_NAME_LENGTH	  50			  // 50 characters
#define MAX_IMAGE_SIZE	  5*1024*1024      // 5 MByte

class UserData
{
    /* Operators for QDataStream serialization and deserialization */
    friend QDataStream& operator>>(QDataStream& in, UserData& user);			// Input deserialization
    friend QDataStream& operator<<(QDataStream& out, const UserData& user);		// Output serialization

private:

    QString m_username;
    int m_userId;
    QString m_nickname;
    QByteArray m_passwd;		// hashed
    QByteArray m_salt;			// randomly generated
    QImage m_icon;				// nullable, check with QImage::isNull()
    QList<QString> m_documents;

    static const QString saltCharacters;

public:

    UserData();	 // Use this to construct an empty user and populate the fields later

    UserData(QString username, int userId, QString nickname, QString passwd, QImage icon = QImage());
    UserData(QString username, int userId, QString nickname, QByteArray passhash, QByteArray salt, QImage icon);

    ~UserData();

    bool isEmpty();
    /* getters */
    int getUserId() const;
    QString getUsername() const;
    QString getNickname() const;
    QByteArray getPasswordHash() const;
    QByteArray getSalt() const;
    QImage getIcon() const;
    QList<QString> getDocuments() const;
    bool hasDocument(QString uri) const;
    QString getURIat(int index) const;

    /* setters */
    void addDocument(QString docUri);
    void removeDocument(QString uri);
    void setNickname(QString newNickname);
    void setIcon(QImage newIcon);
    void setPassword(QString newPassword);
    void update(QString nickname, QImage icon, QString password);
    void rollback(const UserData& backup);
    void generateSalt(QByteArray &salt);
};

#endif // USERDATA_H
