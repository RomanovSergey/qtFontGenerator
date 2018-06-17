
#include <QString>
#include <QImage>
#include <iostream>
#include <vector>

#include "helper.h"

using namespace std;

void fillSymInfo(QImage &img, vector <SymInfo> &vsym, int &maxW)
{
    // Пробежимся по всей длине строки с символами, определим самый широкий символ
    int maxwidth = 0;
    int curwidth = 0;
    int symcnt = 0; // count symbols
    int beginSymbol = -1;
    QRgb pix;
    const QRgb black = qRgb(0,0,0);

    for (int x = 0; x < img.width(); x++) {
        bool whiteLine = true;
        for (int y = 0; y < img.height(); y++) {
            pix = img.pixel(x , y);
            if ( pix == black ) {
                whiteLine = false;
                if (beginSymbol == -1) {
                    beginSymbol = x;
                }
                break;
            }
        }
        if (whiteLine == false) {
            curwidth++;
        } else {
            if (curwidth > maxwidth) {
                maxwidth = curwidth;
            }
            if (curwidth != 0) {
                symcnt++;
                //cout << "curwidth=" << curwidth << ", begSym=" << beginSymbol << endl;
                vsym.push_back( SymInfo(curwidth, beginSymbol, "non_comment") );
            }
            curwidth = 0;
            beginSymbol = -1;
        }
    }
    cout << "maxwidth=" << maxwidth << endl;
    cout << "symcnt=" << symcnt << endl;
    maxW = maxwidth;
}

