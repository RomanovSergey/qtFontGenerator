#ifndef HELPER_H
#define HELPER_H

class SymInfo
{
public:
    SymInfo(int w, int start, const char* com) :
        width(w), startX(start), note(com) { //cout << "con-tor" << endl;
    }
    ~SymInfo() { //cout << "destructor" << endl;
    }
    int width;
    int startX;
    QString note;
    std::vector<uint8_t> vdata;
};

void fillSymInfo(QImage &img, std::vector <SymInfo> &vsym, int &maxW);

#endif // HELPER_H
