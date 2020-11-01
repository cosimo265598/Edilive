#ifndef SYMBOL_H
#define SYMBOL_H

#include <QString>
#include <vector>
#include <QByteArray>
#include <QChar>
#include <QTextCharFormat>

class Symbol {
private:
    QTextCharFormat fmtChar;
    QChar car;
    QString siteId;
    std::vector<int> posfraz;
    QString id;
public:
    Symbol(QChar c, QString s, std::vector<int> pos, QString id,QTextCharFormat new_fmt);
    QString getSite();
    QChar getCar();
    std::vector<int> getPosFraz();
    QString getStringPosFraz();
    QString getId();
    QTextCharFormat getFmt();

};

#endif // SYMBOL_H
