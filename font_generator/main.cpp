#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <vector>

#include "helper.h"

using namespace std;



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

    int pages = img.height() / 8; // + ((img.height() % 8) > 0 ? 1 : 0);
    cout << "Pages = " << pages << endl;
    cout << endl;

    vector <SymInfo> vsym;
    int maxWidth = 0;
    fillSymInfo(img, vsym, maxWidth);

    // will create C-code of symbols

    QFile file( outName );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Error: Can't open file (" << outName.toStdString() << ")" << endl;
        return -1;
    }
    QTextStream outputStream(&file);
    QString code;
    code =  QString("//maxWidth=%1, pages=%2\n").arg(maxWidth).arg(pages);
    code += QString("\n#define FON_T static const char\n");
    code += QString("#define LEN  %1\n\n").arg(maxWidth * pages );
    outputStream << code;

    const QRgb black = qRgb(0,0,0);
    for (auto &it : vsym) {
        cout << "start=" << it.startX << ", width=" << it.width << ", com=" << it.note.toStdString() << endl;

        for (int pg = 0; pg < pages; pg++) {
            for (int sx = 0; sx < it.width; sx++) {
                uint8_t byte = 0;
                for (int sy = 0; sy < 8; sy++) {
                    if ( black == img.pixel(it.startX + sx, pg * 8 + sy) ) {
                        byte |= (1 << sy);
                    }
                }
                it.vdata.push_back(byte);
            }
            for (int sx = it.width; sx < maxWidth; sx++) {
                uint8_t byte = 0;
                it.vdata.push_back(byte);
            }
        }
        code = QString("FON_T f_%1[LEN] = { \n").arg(it.note);
        //outputStream << code;
        for (int pg = 0; pg < pages; pg++) {
            code += QString("  ");
            for (int sx = 0; sx < maxWidth; sx++) {
                code += QString("0x%1, ").arg(it.vdata.at(pg*maxWidth + sx), 2, 16, QLatin1Char('0'));
            }
            code += QString("\n");
        }

        code += QString("}; // %1\n").arg(it.note);
        outputStream << code;
    }


    /*QString code = QString("\nconst uint8_t img[%1] = {\n").arg(DISP_X * (DISP_Y / 8));

    for ( int y = 0; y < DISP_Y / 8; y++ ) {
        code += QString("   ");
        for ( int x = 0; x < DISP_X; x++ ) {
            code += QString("0x%1, ").arg( coor[y*DISP_X + x], 2, 16, QLatin1Char( '0' ) );
        }
        code += QString("\n");
    }

    code += QString("};\n\n");

    cout << code.toStdString() << endl;

    // Сохраним текстовое содержимое code в файл
    QFile file( outName );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cout << "Error: Can't open file (" << outName.toStdString() << ")" << endl;
        return -1;
    }
    QTextStream outputStream(&file);
    //code.replace("\n","\r\n");
    outputStream << code;*/
    file.close();

    cout << "Done" << endl;
    return 0;
}

