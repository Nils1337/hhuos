/*****************************************************************************
 *                                                                           *
 *                                    C G A                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Mit Hilfe dieser Klasse kann man auf den Bildschirm des  *
 *                  PCs zugreifen. Der Zugriff erfolgt direkt auf der Hard-  *
 *                  wareebene, d.h. ueber den Bildschirmspeicher und den     *
 *                  I/O-Ports der Grafikkarte.                               *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *                  Aenderungen von Michael Schoettner, HHU, 21.8.2016       *
 *****************************************************************************/
#include "devices/CGA.h"


/*****************************************************************************
 * Methode:         CGA::setpos                                              *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Setzen des Cursors in Spalte x und Zeile y.              *
 *****************************************************************************/
void CGA::setpos (int x, int y) {
    int pos;
    int high, low;

    if (x<0 || x>=COLUMNS || y<0 || y>ROWS)
        return ;

    pos  = y * COLUMNS + x;
    low  = pos & 0xff;
    high = (pos & 0x3f00) >> 8;

    index_port.outb (14);    // Cursor high auswaehlen
    data_port.outb( high);   // Daten schreiben
    
    index_port.outb (15);    // Cursor low auswaehlen
    data_port.outb (low);    // Daten schreiben
}


/*****************************************************************************
 * Methode:         CGA::getpos                                              *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Abfragem der Cursorposition                              *
 *                                                                           *
 * Rückgabewerte:   x und y                                                  *
 *****************************************************************************/
void CGA::getpos (int &x, int &y) {
    int high, low;
    int pos;

    index_port.outb (14);    // Cursor high auswaehlen
    high = data_port.inb (); // Daten lesen
    index_port.outb (15);    // Cursor low auswaehlen
    low = data_port.inb ();  // Daten lesen

    pos = (high & 0x3f) <<8 | (low & 0xff);
    x  = pos % COLUMNS;
    y  = pos / COLUMNS;
 }


/*****************************************************************************
 * Methode:         CGA::show                                                *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Anzeige eines Zeichens mit Attribut an einer bestimmten  *
 *                  Stelle auf dem Bildschirm.                               *
 *                                                                           *
 * Parameter:                                                                *
 *      x,y         Position des Zeichens                                    *
 *      character   Das auszugebende Zeichen                                 *
 *      attrib      Attributbyte fuer das Zeichen                            *
 *****************************************************************************/
void CGA::show (int x, int y, char character, unsigned char attrib) {
    int pos;

    if (x<0 || x>=COLUMNS || y<0 || y>ROWS)
        return ;

    pos = (y * COLUMNS + x) * 2;
    *(char*)(CGA_START + pos)              = character;
    *(unsigned char*)(CGA_START + pos + 1) = attrib;
}


/*****************************************************************************
 * Methode:         CGA::print                                               *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Anzeige mehrerer Zeichen ab der aktuellen Cursorposition *
 *                  '\n' fuer Zeilenvorschub.                                *
 *                                                                           *
 * Parameter:                                                                *
 *      string      Auszugebende Zeichenkette                                *
 *      n           Laenger der Zeichenkette                                 *
 *      attrib      Attributbyte fuer alle Zeichen der Zeichenkette          *
 *****************************************************************************/
void CGA::print (char* string, int n, unsigned char attrib) {
    int x, y;
    char* pos;
    
    getpos(x, y);
    pos = (char*)CGA_START + (y * COLUMNS + x) * 2;

    while(n) {
        switch (*string) {
            case '\n':
                x=0;
                y++;
                pos = (char*)CGA_START + (y * COLUMNS + x) * 2;
                break;
            default:
                *pos = *string;
                *(unsigned char*)(pos + 1) = attrib;
                pos += 2;
                x++;
                if (x >= COLUMNS) {
                    x=0;
                    y++;
                    pos = (char*)CGA_START + (y * COLUMNS + x) * 2;
                }
                break;
        }
        string++;
        if (y >= ROWS) {
            scrollup();
            y--;
            pos = (char*)CGA_START + (y * COLUMNS + x) * 2;
        }
        n--;
    }
    setpos (x, y);
}


/*****************************************************************************
 * Methode:         CGA::scrollup                                            *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Verschiebt den Bildschirminhalt um eine Zeile nach oben. *
 *                  Die neue Zeile am unteren Bildrand wird mit Leerzeichen  *
 *                  gefuellt.                                                *
 *****************************************************************************/
void CGA::scrollup () {
    long *dstptr = (long*)CGA_START;
    long *srcptr = (long*)(CGA_START + COLUMNS * 2);
    int counter;

    
    counter = ((ROWS - 1) * COLUMNS * 2) / sizeof (long);

    // Zeilen nach oben schieben
    while (counter>0) {
        *(dstptr++) = *(srcptr++);
        counter --;
    }

    // untere Zeileit Leerzeichen fuellen
    counter = (COLUMNS * 2) / sizeof (long);
    while (counter>0) {
      *(dstptr++) = ((((STD_ATTR << 8) | ' ') << 8) | STD_ATTR) << 8 | ' ';
      counter--;
    }
 }


/*****************************************************************************
 * Methode:         CGA::clear                                               *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Lösche den Textbildschirm.                               *
 *****************************************************************************/
void CGA::clear () {
    int x,y;
    
    // Bildschirm loeschen
    for (y=0; y<25; y++)
        for (x=0; x<80; x++)
            show (x, y, ' ', STD_ATTR);
    setpos (0,0);
}


/*****************************************************************************
 * Methode:         CGA::attribute                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Hilfsfunktion zur Erzeugung eines Attribut-Bytes aus     *
 *                  Hintergrund- und Vordergrundfarbe und der Angabe, ob das *
 *                  Zeichen blinkend darzustellen ist.                       *
 *                                                                           *
 * Parameter:                                                                *
 *      bg          Background color                                         *
 *      fg          Foreground color                                         *
 *      blink       ywa/no                                                   *
 *****************************************************************************/
unsigned char attribute (CGA::color bg, CGA::color fg, bool blink) {
    return (blink ? 0x80 : 0) | ((bg & 0x7)<<4) | (fg & 0xf);
}
