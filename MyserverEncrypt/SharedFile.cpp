#include <iostream>
#include "SharedFile.h"
#include <QDebug>
#include <QFile>


SharedFile::SharedFile(QString id, QString c) : counter(0){
    this->siteId=id;
    this->creator = c;
}

QString SharedFile::to_string() {
    QString str;
    for(Symbol s : this->symbols)
        str.append(s.getCar());
    return str;
}


QString SharedFile::getSite() {
    return this->siteId;
}

std::vector<Symbol> SharedFile::getSymbols(){
    return this->symbols;
}


void SharedFile::readExistingFile(QString filePath){
    int countline=0, countchar=0;
    QString line;
    QFile myfile(filePath);
    if (!myfile.open(QIODevice::ReadOnly))
        return;

    ///////
    QChar c;
    QTextCharFormat fmt;
    QDataStream docFileStream(&myfile);
    while (!docFileStream.atEnd()){
        docFileStream >> c;
        docFileStream >> fmt;
        if (countchar == 0)
            localInsert(-1, -2, c, "server","NOID",fmt);
        else
            localInsert(countchar-1, -2, c, "server","NOID",fmt);
        countchar++;
    }

    myfile.close();
    //////
    /*
    QTextStream in(&myfile);
    while ( !in.atEnd() ){
        for(QString car: in.readLine()){
            for(QChar c: car){
                if (countchar == 0 && countline == 0)
                    localInsert(-1, -2, c, "server");
                else
                    localInsert(countchar-1, -2, c, "server");
                countchar++;

            }
        }
        localInsert(countchar-1, -2, QChar::ParagraphSeparator, "server");
        countchar++;
        countline++;
    }*/

}

Symbol SharedFile::localInsert(int indexA, int indexB, QChar value, QString chiamante, QString id,QTextCharFormat newfmt){
    std::vector<int> posA;
    std::vector<int> posB;
    if(indexA==-1)
        posA.push_back(0);
    else
        posA=symbols[indexA].getPosFraz();
    if(indexB==-2)
        posB.push_back(base);
    else
        posB=symbols[indexB].getPosFraz();
    auto it = this->symbols.begin();
    std::stringstream ss, ss2, ss3;
    if(id=="NOID") {
        ss << this->siteId.toUtf8().constData();
        ss2 << counter;
        counter++;
    }
    if(chiamante!="server"){
        //salvo in ss3 l'username del client
        ss3 << chiamante.toUtf8().constData();
    }
    std::vector<int> pos;
    int depth=0, interval=0;
    while(interval<1) {
        if(posA.size()<=depth)
            posA.push_back(0);
        if(posB.size()<=depth)
            posB.push_back(base); //forse bisogna passare base*2*depth per raddoppiare la base ad ogni discesa nell'albero delle pos fraz
        interval = posB[depth] - posA[depth] -1;
        depth++;
    }
    int step;
    if(boundary>interval)
        step=interval;
    else
        step=boundary;

    int pn=5;
    if(posneg.size()>depth){
        pn= posneg[depth];
        if(pn==0)
            pn=1;
        else
            pn=0;
    }
    else {
        pn=rand() % 2;
        posneg.push_back(pn);
    }
    if(pn==1) {
        int addVal = std::rand() % ( step - 1 + 1 ) +1;
        int i;
        for(i = 0; i<depth-1; i++)
            pos.push_back(posA[i]);
        pos.push_back(posA[i]+addVal);
    }
    else if(pn==0) {
        int subVal = std::rand() % ( step - 1 + 1 ) +1  ;
        int i;
        for(i = 0; i<depth-1; i++)
            pos.push_back(posA[i]);
        if(posB[i]-posA[i]>1)
            pos.push_back(posB[i]-subVal);
        else
            pos.push_back(base-subVal); //oppure 2*depth*base se decidiamo di raddoppiare la base ad ogni discesa in profondita
    }
    QString idsim;
    if(id=="NOID"){
        if(chiamante!="server")
            idsim=QString::fromStdString(ss.str())+QString("_")+QString::fromStdString(ss3.str())+QString("_")+QString::fromStdString(ss2.str().data());
        else
            idsim=QString::fromStdString(ss.str())+QString("_")+QString::fromStdString(ss2.str());
    }
    else
        idsim=id;

    Symbol s(value, siteId, pos, idsim,newfmt);
    if(indexB==-2 || symbols.empty()){
        symbols.push_back(s);
    }
    else
        symbols.insert(it+indexB, s);

    return s;
}


void SharedFile::localErase(Symbol s) {
    if(symbols.empty())
        return;
    auto it = this->symbols.begin();
    int i;
    for(i=0; i<this->symbols.size(); i++){
        if(symbols[i].getStringPosFraz()==s.getStringPosFraz() && symbols[i].getId()==s.getId()){
            symbols.erase(it+i);
            break;
        }
    }
}


int SharedFile::recursiveInsert(Symbol s, Symbol dainserire, int i, int depth){
    if(i==depth)
        return 0;
    if(s.getPosFraz()[i]>dainserire.getPosFraz()[i])
        return 1;
    if(s.getPosFraz()[i]==dainserire.getPosFraz()[i])
        return recursiveInsert(s, dainserire, i+1, depth);
}

int SharedFile::localInsert(Symbol ricevuto, QString chiamante) {
    auto it = this->symbols.begin();
    if (symbols.size() == 0) {
        symbols.push_back(ricevuto);
        return 0;
    } else {
        bool fatto = false;
        for (int k = 0; k < symbols.size(); k++) {
            if (symbols[k].getStringPosFraz() == ricevuto.getStringPosFraz() &&
                symbols[k].getId() != ricevuto.getId() && chiamante == "server") {
                //CONFLITTO
                localInsert(k, k + 1, ricevuto.getCar(), "server", ricevuto.getId(),ricevuto.getFmt());
                return 1;
            }
            int depth=symbols[k].getPosFraz().size();
            int inserire;
            if(symbols[k].getPosFraz().size()>ricevuto.getPosFraz().size()){
                int diff=symbols[k].getPosFraz().size()-ricevuto.getPosFraz().size();
                std::vector<int> pf= ricevuto.getPosFraz();
                for(int l=0; l<diff; l++)
                    pf.push_back(0);
                inserire = recursiveInsert(symbols[k], Symbol(ricevuto.getCar(), ricevuto.getSite(), pf, ricevuto.getId(),ricevuto.getFmt()), 0, depth);
            }
            else if(symbols[k].getPosFraz().size()<ricevuto.getPosFraz().size()){
                depth=ricevuto.getPosFraz().size();
                int diff=ricevuto.getPosFraz().size()-symbols[k].getPosFraz().size();
                std::vector<int> pf= symbols[k].getPosFraz();
                for(int l=0; l<diff; l++)
                    pf.push_back(0);
                inserire = recursiveInsert(Symbol(symbols[k].getCar(), symbols[k].getSite(), pf, symbols[k].getId(),symbols[k].getFmt()), ricevuto, 0, depth);

            }
            else
                inserire = recursiveInsert(symbols[k], ricevuto, 0, depth);
            if (inserire == 1) {
                symbols.insert(it + k, ricevuto);
                fatto = true;
                break;
            }
        }
        if (fatto == false) {
            symbols.push_back(ricevuto);
        }
        return 0;
    }
}
