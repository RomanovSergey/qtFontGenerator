#include "fontinfo.h"

#include <iostream>

using namespace std;

void FontInfo::fillSymInfo(QImage &img) // to fill "vector <SymInfo> vsym;"
{
    // to find maxWidth of all given symbols
    this->maxWidth = 0;
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
            if (curwidth > this->maxWidth) {
                this->maxWidth = curwidth;
            }
            if (curwidth != 0) {
                symcnt++;
                this->vsym.push_back( SymInfo(curwidth, beginSymbol) );
            }
            curwidth = 0;
            beginSymbol = -1;
        }
    }
    cout << "maxwidth=" << this->maxWidth << endl;
    cout << "symcnt=" << symcnt << endl;

    for (auto &it : this->vsym) {
        for (int pg = 0; pg < this->pages; pg++) {
            int space = this->maxWidth - it.width;
            if ( this->alg.alg == Alg::front ) {
                for (int n = 0; n < it.width; n++) {
                    uint8_t byte = 0;
                    for (int sy = 0; sy < 8; sy++) {
                        if ( black == img.pixel(it.startX + n, pg * 8 + sy) ) {
                            byte |= (1 << sy);
                        }
                    }
                    it.vdata.push_back(byte);
                }
                for (int n = 0; n < space; n++) {
                    it.vdata.push_back(0);
                }
            } else if ( this->alg.alg == Alg::end ) {
                for (int n = 0; n < space; n++) {
                    it.vdata.push_back(0);
                }
                for (int n = 0; n < it.width; n++) {
                    uint8_t byte = 0;
                    for (int sy = 0; sy < 8; sy++) {
                        if ( black == img.pixel(it.startX + n, pg * 8 + sy) ) {
                            byte |= (1 << sy);
                        }
                    }
                    it.vdata.push_back(byte);
                }
            } else if ( this->alg.alg == Alg::middle ) {
                int start  = space / 2;
                int finish = space - start;
                for (int n = 0; n < start; n++) {
                    it.vdata.push_back(0);
                }
                for (int n = 0; n < it.width; n++) {
                    uint8_t byte = 0;
                    for (int sy = 0; sy < 8; sy++) {
                        if ( black == img.pixel(it.startX + n, pg * 8 + sy) ) {
                            byte |= (1 << sy);
                        }
                    }
                    it.vdata.push_back(byte);
                }
                for (int n = 0; n < finish; n++) {
                    it.vdata.push_back(0);
                }
            } else {
                cout << "error: no algorithm" << endl;
            }
        }
    }
}


