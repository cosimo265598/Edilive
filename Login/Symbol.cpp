#include <sstream>
#include "Symbol.h"

Symbol::Symbol(QChar c, QString s, std::vector<int> pos, QString id,QTextCharFormat new_fmt) : id(id), car(c){
    this->siteId=s;
    this->posfraz=pos;
    this->fmtChar=new_fmt;
}


QString Symbol::getSite() {
    return this->siteId;
}

std::vector<int> Symbol::getPosFraz() {
    return this->posfraz;
}
QString Symbol::getStringPosFraz(){
    std::stringstream ss;
    for(int x : posfraz)
        ss<<x;
    return ss.str().data();
}

QChar Symbol::getCar(){
    return this->car;
}


QString Symbol::getId() {
    return this->id;
}

QTextCharFormat Symbol::getFmt()
{
    return this->fmtChar;
}
