#ifndef SHAREDFILE_H
#define SHAREDFILE_H

#include <string>
#include <string.h>
#include <sstream>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>

#include "Symbol.h"


class SharedFile{
private:
    QString siteId ; //ci mettiamo il nome del path nel server del file
    std::vector<Symbol> symbols;
    std::vector<int> posneg;
    int counter;
    QString creator;
    int base = 16, boundary=10;

public:

    //costruttore per definire un nuovo file vuoto
    SharedFile(QString id, QString c);
    //SharedFile(const SharedFile& other);

    //METODO PER COSTRUIRE LA STRUTTURA DI UN FILE PREESISTENTE NEL FILESYSTEM DEL SERVER
    //da chiamare subito dopo il costruttore
    //il metodo legge il file e chiama ripetutamente localInsert con indici per creare la struttura in ram
    //Come chiamare correttamente la funzione dal MainWIndow?
    //Quando ho il QString che identifica il path del file da aprire in mainwindow, lo converto in std::string e lo passo alla funz
    //es:
    //    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+fileName);
    //    SharedFile newfile(path.tostdString(), nome utente creatore)
    //    newfile.readExistingFile(pathdelfile);
    void readExistingFile(QString filePath);

    //Questa funzione è usata in tre situazioni:
    //     - DAL CLIENT: Per il suo inserimento locale dall'editor.
    //                   Serve un metodo per tradurre gli indici d'editor in indici di vettore
    //                   di symbols perchè la funzione riceve questi ultimi.
    //                   Quando si sta inserendo un carattere in testa al file indexA=-1;
    //                   Quando si sta inserendo un carattere in coda al file indexB=-2;
    //                   Quando si sta inserendo il primo carattere file indexA=-1 e indexB=-2;
    //     - DAL SERVER in due diverse situazioni:
    //                   1) Costruzione della struttura di caratteri di un file preesistente nel filesystem.
    //                      In questo caso il metodo è chiamato ripetutamente dalla readExistingFile.
    //                   2) Ricalcolo di una posizione frazionaria dovuta al conflitto di due caratteri ricevuti da client.
    //Per l'uso della funzione è necessario specificare il chiamante (client o server) e
    //l'id del carattere solo nel caso in cui la si stia chiamando a seguito di un conflitto di posfraz.
    Symbol localInsert(int indexA, int indexB, QChar value, QString chiamante, QString id = "NOID",QTextCharFormat new_fmt=QTextCharFormat());

    //Questa funzione è usata in due situazioni:
    //     - DAL CLIENT: Quando gli arriva un carattere da inserire dal server.
    //     - DAL SERVER: Quando gli arriva un carattere da inserire da un client.
    //In entrambi i casi, è necessario prima convertire i dati riguardanti il carattere ricevuto
    //in un symbol da passare alla funzione.
    //Chiamando la funzione bisogna specificare il lato da cui si sta usando (client o server)
    int localInsert(Symbol ricevuto, QString chiamante);

    int recursiveInsert(Symbol s, Symbol dainserire, int i, int depth);

    //Cancellazione di un carattere a seguito di aver ricevuto un messaggio che diceva di cancellarlo:
    //    - Lato Client : se il server gli ha comunicato di cancellare un carattere;
    //    - Lato Server : se un client gli ha comunicato di cancellare un carattere;
    void localErase(Symbol s);

    //Ritorna una stringa con l'intero contenuto del file
    QString to_string();

    //Ritorna il codice identificativo del file
    QString getSite();

    std::vector<Symbol> getSymbols();

};


#endif // SHAREDFILE_H
