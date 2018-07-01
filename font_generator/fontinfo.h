#ifndef FONTINFO_H
#define FONTINFO_H

#include <QString>
#include <QImage>
#include <vector>

class Alg {
public:
    enum alg_t {
        middle = 0,
        front,
        end,
    } alg;

    Alg(alg_t a) : alg(a) {}
};

class SymInfo
{
public:
    SymInfo(int w, int start) :
        width(w), startX(start){ //cout << "con-tor" << endl;
    }
    ~SymInfo() { //cout << "destructor" << endl;
    }
    int width;
    int startX;
    std::vector<uint8_t> vdata;
};

class FontInfo
{
public:
    FontInfo() : alg(Alg::middle), pages(0), maxWidth(0) {}

    std::vector <SymInfo> vsym;
    Alg alg;
    int pages;
    int maxWidth;

    void fillSymInfo(QImage &img);
};

#endif // FONTINFO_H
