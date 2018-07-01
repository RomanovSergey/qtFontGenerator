/****************************************************************************
 * Programm: qtFontGenerator
 *
 * created: 2018.07.01
 * autor:   Romanov S.V.
 *
 * Description:
 * To create monochome font for microcontroller's displays as sh1106
 *   from black and white image line (.png, ...) file
 *   and file with info about image line file (code of utf16, name, comment)
 * result: C style code file for include in microcontroller's programm.
 */

/****************************************************************************
 * cmd line params:
 * ./font_generator -h
 * Usage: ./font_generator [options]
 * program for generate C code font from line *.png image
 *
 * Options:
 *   -h, --help           Displays this help.
 *   -v, --version        Displays version information.
 *   -i, --input <name>   input file name (*.png or *.bmp or *.jpeg ...) <name>.
 *   -x, --xml <name>     xml file name <name>.
 *   -o, --output <name>  output file name (*.c) <name>. Default=default.c
 *
 *
 * Example:
 * ./font_generator -i Dejawu_font24x.png -x font_info.xml
 * (will generate default.c file)
 */

/****************************************************************************
 * xml file example:
 *
 * <root>
 *     <sym code="0x30" name="f_0x30" comment="0"/>
 *     <sym code="0x31" name="f_0x31" comment="1"/>
 *     <sym code="0x32" name="f_0x32" comment="2"/>
 * </root>
 *
 */

#include "helper.h"
#include "parser.h"

#include <QImage>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("fontGenerator");
    QCoreApplication::setApplicationVersion("1.0");

    QString picName; // image's file name
    QString xmlName; // text file name with a comment to symbols (in a sequential order)
    QString outName; // output name (result)
    if ( cmdLineParse(a, picName, xmlName, outName) < 0 ) {
        return -1;
    }

    FontInfo f;
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

    Parser pars;
    if ( pars.toParse(xmlName) != 0 ) {
        cout << "Error: can't parse" << endl;
    }

    f.fillSymInfo(img);

    pars.outCCodeFile(outName, f); // to generate C-code with font array

    cout << "Done" << endl;
    return 0;
}

