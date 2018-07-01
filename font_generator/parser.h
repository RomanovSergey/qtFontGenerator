#ifndef PARSER_H
#define PARSER_H


#include "fontinfo.h"

#include <QString>
#include <vector>
#include <list>

class Xmldata
{
public:
    Xmldata(int s, uint16_t cod, QString n, QString com) :
        pos(s), icode(cod), name(n), comment(com) {}

    int pos; // symbol position on image, from 0
    uint16_t icode;
    QString name;
    QString comment;

    bool operator<(const Xmldata &r)
    {
        if ( this->icode < r.icode ) {
            return true;
        }
        return false;
    }
};

class Parser
{
public:
    int toParse(const QString &xmlName);
    std::list<Xmldata> xdata;

    int outCCodeFile(QString &outName, FontInfo &f);
};

#endif // PARSER_H
