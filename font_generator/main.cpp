#include <QCoreApplication>
#include <QImage>
#include <iostream>

#include "helper.h"

using namespace std;

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("fontGenerator");
    QCoreApplication::setApplicationVersion("1.0");

    QString picName; // image's file name
    QString txtName; // text file name with a comment to symbols (in a sequential order)
    QString outName; // output name (result)
    if ( parse(a, picName, txtName, outName) < 0 ) {
        return -1;
    }

    fontInfo f;
    QImage img( picName );
    if ( img.isNull() ) {
        cout << "Error: Can't load image file: " << picName.toStdString() << endl;
        cout << "Exit" << endl;
        return -1;
    }
    cout << "Format image: " << endl << "\t";
    QString str;
    if ( imageFormat(img, str) < 0 ) {
        cout << str.toStdString() << endl;
        return -1;
    }
    cout << str.toStdString() << endl;
    cout << "Image width  = " << img.width() << endl;
    cout << "Image height = " << img.height() << endl;
    if ( img.height() % 8 != 0 ) {
        cout << "Error: height % 8 != 0" << endl;
        return -1;
    }
    f.pages = img.height() / 8;
    cout << "Pages = " << f.pages << endl;
    cout << endl;

    fillSymInfo(img, f); // to fill "vector <SymInfo> vsym;"
    outCCodeFile(outName, f); // to generate C-code with font array
    cout << "Done" << endl;
    return 0;
}

