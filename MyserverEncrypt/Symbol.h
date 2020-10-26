#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>

class Symbol {
private:
    char car;
    std::string siteId;
    std::vector<int> posfraz;
    std::string id;
public:
    Symbol(char c, std::string s, std::vector<int> pos, std::string id);
    std::string getSite();
    char getCar();
    std::vector<int> getPosFraz();
    float getFloatPosFraz();
    std::string getId();
    std::string getFloatStringPosFraz();


};

#endif // SYMBOL_H
