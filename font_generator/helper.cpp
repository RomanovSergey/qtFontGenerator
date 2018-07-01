
#include "helper.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <vector>

using namespace std;

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

int cmdLineParse(QCoreApplication &a, QString &picName, QString &xmlName, QString &outName)
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
    QCommandLineOption optTextName( QStringList() << "x" << "xml",
                                    "xml file name <name>.",
                                    "name");
    parser.addOption( optTextName );
    QCommandLineOption optOutName( QStringList() << "o" << "output",
                                   "output file name (*.c) <name>. Default=default.c",
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

    xmlName = parser.value( optTextName );
    if ( xmlName.isEmpty() ) {
        cout << "Please set text name (-t FileName) (with comment of characters)" << endl;
        return -1;
    }
    return 0;
}
