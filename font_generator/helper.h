#ifndef HELPER_H
#define HELPER_H

#include <vector>
#include <QString>

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

class fontInfo
{
public:
    fontInfo() : alg(Alg::middle), pages(0), maxWidth(0) {}

    std::vector <SymInfo> vsym;
    Alg alg;
    int pages;
    int maxWidth;
};


void fillSymInfo(QImage &img, fontInfo &f);
int outCCodeFile(QString &outName, fontInfo &f);
int imageFormat(QImage &img, QString &str);
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
int parse(QCoreApplication &a, QString &picName, QString &txtName, QString &outName);

#endif // HELPER_H
