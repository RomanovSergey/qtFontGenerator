
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
