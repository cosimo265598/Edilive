#include <sstream>
#include "Symbol.h"

Symbol::Symbol(char c, std::string s, std::vector<int> pos, std::string id) : id(id), car(c){
    this->siteId=s;
    this->posfraz=pos;
}


std::string Symbol::getSite() {
    return this->siteId;
}

std::vector<int> Symbol::getPosFraz() {
    return this->posfraz;
}

float Symbol::getFloatPosFraz(){
    std::stringstream ss;
    int count=0;
    for(int x : posfraz){
        if(count==1)
            ss<<".";
        ss<<x;
        count++;
    }
    std::string str = ss.str();
    float f = std::stof(str);
    return f;
}

char Symbol::getCar(){
    return this->car;
}

std::string Symbol::getId() {
    return this->id;
}
std::string Symbol::getFloatStringPosFraz(){
    std::stringstream ss;
    int count=0;
    for(int x : posfraz){
        if(count==1)
            ss<<".";
        ss<<x;
        count++;
    }
    return ss.str();
}