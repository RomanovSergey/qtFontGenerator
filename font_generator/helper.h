#ifndef HELPER_H
#define HELPER_H

#include "fontinfo.h"

#include <QString>
#include <QImage>
#include <QCoreApplication>


int outCCodeFile(QString &outName, FontInfo &f);

int imageFormat(QImage &img, QString &str);
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
int cmdLineParse(QCoreApplication &a, QString &picName, QString &xmlName, QString &outName);

#endif // HELPER_H
