#include "user.h"

User::User()
{
    age=25;
    nome="COSIMO";
    cognome="MANISI";
}

User::User(QString n,QString c,int a): age(a),nome(n), cognome(c){}

int User::getAge(){
    return age;
}
QString User::getNome(){
    return nome;
}
QString User::getCognome(){
    return cognome;
}
QString User::print(){
    QString out("Nome: "+nome+" Cognome: "+cognome+" Age: "+QString::number(age));
            return out;
}

QDataStream& operator<<(QDataStream & out, const User& s){
    out<<s.nome<<s.cognome<<s.age;
    return out;

}
QDataStream &operator>>(QDataStream & out, User& s){
    out>>s.nome>>s.cognome>>s.age;
    return out;
}
