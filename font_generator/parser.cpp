#include "parser.h"

#include <QFile>
#include <QDomDocument>
#include <QTextStream>

#include <iostream>

using namespace std;

int Parser::toParse(const QString &xmlName)
{
    QString errMsg;
    int errorLine;
    int errorColumn;

    QDomDocument doc("mydocument");
    QFile file(xmlName);
    if ( !file.open(QIODevice::ReadOnly) ) {
        cout << "error: can't open file: " << xmlName.toStdString() << endl;
        return -1;
    }
    if ( !doc.setContent(&file, false, &errMsg, &errorLine, &errorColumn) ) {
        cout << "err: can't setContent: " << errMsg.toStdString() << endl;
        cout << "errorLine: " << errorLine << ", errorColumn: " << errorColumn << endl;
        file.close();
        return -1;
    }
    file.close();

    cout << __FUNCTION__ << "begin:" << endl;
    int pos = 0;
    QString code;
    uint16_t icode;
    bool ok;
    QString name;
    QString comment;

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if( (!e.isNull()) && (e.tagName() == QString("sym")) )
        {
            //cout << qPrintable(e.tagName()) << endl;
            code = e.attribute(QString("code"));
            icode = code.toUInt(&ok, 0);
            if ( ok == false ) {
                cout << "err: can't convert code(" << qPrintable(code) << ") to UInt" << endl;
                return -1;
            }
            name = e.attribute(QString("name"));
            comment = e.attribute(QString("comment"));

            xdata.push_back(Xmldata(pos,icode,name,comment));
            pos++;
        } else {
            cout << __FUNCTION__ << ": err: " << "null or tagName false" << endl;
        }
        n = n.nextSibling();
    }

    for (auto &it: xdata) {
        cout << "pos" << it.pos;
        cout << ": code=" << QString("0x%1").arg(it.icode, 4, 16, QLatin1Char('0')).toStdString();
        cout << ", name=" <<
                qPrintable(it.name) << ", comment=" << qPrintable(it.comment) << endl;
    }

    cout << endl << "To sort parsed data ==============>>>" << endl;
    xdata.sort();

    for (auto &it: xdata) {
        cout << "code=" << QString("0x%1").arg(it.icode, 4, 16, QLatin1Char('0')).toStdString();
        cout << ", position=" << it.pos << endl;;
    }

    return 0;
}


int Parser::outCCodeFile(QString &outName, FontInfo &f)
{
    int fsize = f.vsym.size();

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

    try {
        for (auto &it: xdata) {
            int pos = it.pos;
            if (pos > fsize) {
                cout << "image has less chars, then cur pos=" << pos << endl;
                continue;
            }
            code = QString("FON_T %1[LEN] = { // %2\n").arg(it.name).arg(it.comment);
            for (int pg = 0; pg < f.pages; pg++) {
                code += QString("  ");
                for (int sx = 0; sx < f.maxWidth; sx++) {
                    code += QString("0x%1, ")
                            .arg(f.vsym.at(pos).vdata.at(pg*f.maxWidth + sx),2,16,QLatin1Char('0'));
                }
                code += QString("\n");
            }
            code += QString("};\n");
            outputStream << code;
        }

        code = QString("\n"
                       "typedef struct {\n"
                       "  uint16_t code;\n"
                       "  const char* img;\n"
                       "} TUcode_t;\n\n");
        outputStream << code;

        code  = QString("// for the binary search algorithm, the value of "
                        "'code' should follow in ascending order!\n");
        code += QString("static TUcode_t fontX[] = {\n");
        outputStream << code;

        for (auto &it: xdata) {
            code = QString("  { 0x%1, %2 }, // %3\n")
                    .arg(it.icode, 4, 16, QLatin1Char('0'))
                    .arg(it.name).arg(it.comment);
            outputStream << code;
        }
        code = QString("};\n\n");
        outputStream << code;

        code = QString(
                    "// through the binary search algorithm\n"
                    "const char* getFontX( uint16_t key) {\n"
                    "    int len = sizeof( fontX ) / sizeof ( TUcode_t );\n"
                    "    int found = 0;\n"
                    "    int high = len - 1, low = 0;\n"
                    "    int middle = (high + low) / 2;\n"
                    "    while ( !found && high >= low ){\n"
                    "        if ( key == fontX[middle].code ) {\n"
                    "            found = 1;\n"
                    "            break;\n"
                    "        } else if (key < fontX[middle].code ) {\n"
                    "            high = middle - 1;\n"
                    "        } else {\n"
                    "            low = middle + 1;\n"
                    "        }\n"
                    "        middle = (high + low) / 2;\n"
                    "    }\n"
                    "    // if not found, then return symbol '?'\n"
                    "    return (found == 1) ? fontX[middle].img : f_0x3F ;\n"
                    "}\n\n"
                    );
        outputStream << code;

    } catch (const std::out_of_range& oor) {
        cerr << "Out of Range error: " << oor.what() << endl;
    }

    file.close();
    return 0;
}

