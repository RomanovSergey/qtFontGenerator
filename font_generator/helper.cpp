#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <vector>

#include "helper.h"

using namespace std;

void fillSymInfo(QImage &img, fontInfo &f)
{
    // to find maxWidth of all given symbols
    f.maxWidth = 0;
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
            if (curwidth > f.maxWidth) {
                f.maxWidth = curwidth;
            }
            if (curwidth != 0) {
                symcnt++;
                f.vsym.push_back( SymInfo(curwidth, beginSymbol, "non_comment") );
            }
            curwidth = 0;
            beginSymbol = -1;
        }
    }
    cout << "maxwidth=" << f.maxWidth << endl;
    cout << "symcnt=" << symcnt << endl;

    for (auto &it : f.vsym) {
        for (int pg = 0; pg < f.pages; pg++) {
            int space = f.maxWidth - it.width;
            if ( f.alg.alg == Alg::front ) {
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
            } else if ( f.alg.alg == Alg::end ) {
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
            } else if ( f.alg.alg == Alg::middle ) {
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

int outCCodeFile(QString &outName, fontInfo &f)
{
    QFile file( outName ); // output c-code file
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Error: Can't open file (" << outName.toStdString() << ")" << endl;
        return -1;
    }
    QTextStream outputStream(&file);
    QString code;
    code =  QString("//maxWidth=%1, pages=%2\n").arg(f.maxWidth).arg(f.pages);
    code += QString("\n#define FON_T static const char\n");
    code += QString("#define LEN  %1\n\n").arg(f.maxWidth * f.pages );
    outputStream << code;

    int cnt = 0;
    for (auto &it : f.vsym) {
        code = QString("FON_T f_$a%1[LEN] = { // $b%1\n").arg(cnt); //it.note);
        //outputStream << code;
        for (int pg = 0; pg < f.pages; pg++) {
            code += QString("  ");
            for (int sx = 0; sx < f.maxWidth; sx++) {
                code += QString("0x%1, ")
                        .arg(it.vdata.at(pg * f.maxWidth + sx), 2, 16, QLatin1Char('0'));
            }
            code += QString("\n");
        }
        code += QString("};\n"); // %1\n").arg(it.note);
        outputStream << code;
        cnt++;
    }
    file.close();
return 0;
}

int imageFormat(QImage &img, QString &str) {
    switch ( img.format() ) {
    case QImage::Format_Invalid:
        str = "Error: Format_Invalid";
        return -1;
    case QImage::Format_Mono:
        str = "Format_Mono";
        break;
    case QImage::Format_MonoLSB:
        str = "Format_MonoLSB";
        break;
    case QImage::Format_Indexed8:
        str = "Format_Indexed8";
        break;
    case QImage::Format_RGB32:
        str = "Format_RGB32";
        break;
    case QImage::Format_ARGB32:
        str = "Format_ARGB32";
        break;
    default:
        str = "other: " + img.format();
    }
    return 0;
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

int parse(QCoreApplication &a, QString &picName, QString &txtName, QString &outName)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("program for generate C code font from line *.png image");
    parser.addHelpOption();
    parser.addVersionOption();
    // An option with a value
    QCommandLineOption optImageName( QStringList() << "i" << "input",
                                     "input file name (*.png or *.bmp or *.jpeg ...) <name>.",
                                     "name");
    parser.addOption( optImageName );
    QCommandLineOption optTextName( QStringList() << "t" << "text",
                                    "text file name <name>.",
                                    "name");
    parser.addOption( optTextName );
    QCommandLineOption optOutName( QStringList() << "o" << "output",
                                   "output file name (*.c) <name>.",
                                   "name");
    optOutName.setDefaultValue("default.c");
    parser.addOption( optOutName );
    parser.process( a ); // Process the actual command line arguments given by the user
    picName = parser.value( optImageName );
    if ( picName.isEmpty() ) {
        cout << "Please set picture's file name (-i FileName)" << endl;
        return -1;
    }
    cout << "Picture name is: " << endl << "\t" << picName.toStdString() << endl;
    outName = parser.value( optOutName );
    cout << "Output name is: " << endl << "\t" << outName.toStdString() << endl;

    txtName = parser.value( optTextName );
    if ( txtName.isEmpty() ) {
        cout << "Please set text name (-t FileName) (with comment of characters)" << endl;
        return -1;
    }
    return 0;
}
