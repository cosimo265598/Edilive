#include <iostream>
#include "SharedFile.h"

SharedFile::SharedFile(std::string id, std::string c) : counter(0){
    this->siteId=id;
    this->creator = c;
}

void SharedFile::readExistingFile(std::string filePath){
    int countline=0, countchar=0;
    std::string line;
    std::ifstream myfile;
    myfile.open (filePath, std::ifstream::in);
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
            for(char car:line){
                if(car!='\0') {
                    if (countchar == 0 && countline == 0)
                        localInsert(-1, -2, car, "server");
                    else
                        localInsert(countchar-1, -2, car, "server");
                    countchar++;
                }
            }
            localInsert(countchar-1, -2, '\n', "server");
            countchar++;
            countline++;
        }
        myfile.close();
    }
    else
        std::cout << "Error opening file."<<std::endl;
}

Symbol SharedFile::localInsert(int indexA, int indexB, char value, std::string chiamante, std::string id){
    /*if(id!="NOID")
        printf("Sono in insert indicizzata per inserire tra:\n"
               "-indice=%d con posfraz=%f\n"
               "-indice=%d con posfraz=%f\n",
               indexA, symbols[indexA].getFloatPosFraz(),
               indexB, symbols[indexB].getFloatPosFraz());*/
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
        ss << this->siteId;
        ss2 << counter;
        counter++;
    }
    if(chiamante!="server"){
        //salvo in ss3 l'username del client
        ss3 << chiamante;
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
    std::string idsim;
    if(id=="NOID"){
        if(chiamante!="server")
            idsim=ss.str()+"_"+ss3.str()+"_"+ss2.str();
        else
            idsim=ss.str()+"_"+ss2.str();
    }
    else
        idsim=id;

    Symbol s(value, siteId, pos, idsim);
    if(indexB==-2 || symbols.empty()){
        symbols.push_back(s);
    }
    else{
        symbols.insert(it+indexB, s);
    }


    /*if(chiamante=="client"){
        per client: creo messaggio e lo mando al server notificando 'inserimento
    }*/
    return s;

    //per server: SOLO SE NON C'È STATO CONFLITTO creo messaggio e lo mando a tutti i client per notificare l'inserimento
    //so che c'è stato conflitto se id!="NOID"
}


int SharedFile::localInsert(Symbol ricevuto, std::string chiamante) {
    auto it = this->symbols.begin();
    int indice=-1;
    if (symbols.size() == 0) {
        symbols.push_back(ricevuto);
    } else {
        int depth = ricevuto.getPosFraz().size();
        int fatto = 0;
        //printf("NUOVO INSERIMENTO: ");
        //printf("Voglio inserire %c con posfraz %f\n", ricevuto.getCar(), ricevuto.getFloatPosFraz());
        for (int j = 0; j < symbols.size(); j++) { //migliorabile l'algoritmo di ricerca
            //printf("Ho trovato car %c con posfraz %f\n", symbols[j].getCar(), symbols[j].getFloatPosFraz());
            if(symbols[j].getFloatPosFraz()>ricevuto.getFloatPosFraz()) {
                symbols.insert(it + j, ricevuto);
                fatto=1;
                indice=j;
                break;
            }
            else if(chiamante=="server" && symbols[j].getFloatPosFraz()==ricevuto.getFloatPosFraz()){
                //printf("PosFraz uguale\n");
                //CONFLITTO
                //scelgo nuova pos per ricevuto che stia tra attuale j e j+1 e inserisco
                localInsert(j, j+1, ricevuto.getCar(), "server", ricevuto.getId());
                indice=j+1;
                fatto=1;
                break;
            }
        }
        if(fatto==0){ //caso in cui il nuovo simbolo va in coda al file
            symbols.push_back(ricevuto);
            indice = symbols.size()-1;
        }

        //if(chiamante=="server") {
            //SE NON C'È STATO CONFLITTO:
                 //creo messaggio per avvisare tutti gli i client tranne il mandante che è stato aggiunto un carattere
            //SE C'È STATO CONFLITTO:
                 //creo messaggio per avvisare tutti gli i client tranne il mandante che è stato aggiunto un carattere
                 //creo messaggio per il client mandante per fargli modificare la pos fraz usando l'identificativo del carattere
        //}
    }
    return indice;
}


/*
int SharedFile::localInsert(Symbol ricevuto, std::string chiamante) {
    auto it = this->symbols.begin();
    int pos = -1;
    if (symbols.size() == 0) {
        symbols.push_back(ricevuto);
    } else {
        int depth = ricevuto.getPosFraz().size();
        int fatto = 0;
        //printf("NUOVO INSERIMENTO: ");
        //printf("Voglio inserire %c con posfraz %f\n", ricevuto.getCar(), ricevuto.getFloatPosFraz());
        for (int j = 0; j < symbols.size(); j++) { //migliorabile l'algoritmo di ricerca
            //printf("Ho trovato car %c con posfraz %f\n", symbols[j].getCar(), symbols[j].getFloatPosFraz());
            if(symbols[j].getFloatPosFraz()>ricevuto.getFloatPosFraz()) {
                pos = j;
                symbols.insert(it + j, ricevuto);
                fatto=1;
                break;
            }
            else if(chiamante=="server" && symbols[j].getFloatPosFraz()==ricevuto.getFloatPosFraz()){
                //printf("PosFraz uguale\n");
                //CONFLITTO
                //scelgo nuova pos per ricevuto che stia tra attuale j e j+1 e inserisco
                localInsert(j, j+1, ricevuto.getCar(), "server", ricevuto.getId());
                fatto=1;
                brstd::vector<Symbol> SharedFile::getSymbols(){
    return this->symbols;
}eak;
            }
        }
        if(fatto==0){//caso in cui il nuovo simbolo va in coda al file
            symbols.push_back(ricevuto);
            pos = symbols.size()-1;
        }

        //if(chiamante=="server") {
            //SE NON C'È STATO CONFLITTO:
                 //creo messaggio per avvisare tutti gli i client tranne il mandante che è stato aggiunto un carattere
            //SE C'È STATO CONFLITTO:
                 //creo messaggio per avvisare tutti gli i client tranne il mandante che è stato aggiunto un carattere
                 //creo messaggio per il client mandante per fargli modificare la pos fraz usando l'identificativo del carattere
        //}
    }
    return pos;
}

*/

Symbol SharedFile::localErase(int index, char c) {
    std::vector<int> v;
    if(index<0 || index>this->symbols.size()-1)
        return Symbol('a', "invalid", v, "invalid");

    auto it = this->symbols.begin();
    Symbol s = symbols[index];
    symbols.erase(it+index);
    //avviso il server di aver cancellato quel simbolo usando pos fraz + id per identificare il carattere
    return s;
}


void SharedFile::localErase(Symbol s, std::string chiamante) {
    if(symbols.empty())
        return;
    auto it = this->symbols.begin();
    int i;
    for(i=0; i<this->symbols.size(); i++)
        if(symbols[i].getFloatPosFraz()==s.getFloatPosFraz() && symbols[i].getId()==s.getId()){
            symbols.erase(it+i);
            break;
        }
    /*if(chiamante=="server"){
        //informa tutti i clients tranne il mandante che hai cancellato il carattere identificando con id e posfraz
    }*/
}

std::string SharedFile::to_string() {
    std::string str;
    for(Symbol s : this->symbols)
        str+=s.getCar();
    return str;
}


std::string SharedFile::getSite() {
    return this->siteId;
}

std::vector<Symbol> SharedFile::getSymbols(){
    return this->symbols;
}

