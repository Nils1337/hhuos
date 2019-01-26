/*****************************************************************************
 *                                                                           *
 *                            L F B G R A P H I C S                          *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Zeichenfunktionen fuer Grafikmodi, die auf einem         *
 *                  linearen Framebuffer basieren. Verwendet in VESA und     *
 *                  QemuVGA.                                                 *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 19.9.2016                       *
 *****************************************************************************/

#ifndef __LFBgraphics_include__
#define __LFBgraphics_include__


// Hilfsfunktionen um Farbwerte fuer einen Pixel zu erzeugen
#define RGB_24(r,g,b) (unsigned int) ((r << 16) + (g << 8) + b )


class LFBgraphics {
    
private:
    LFBgraphics (const LFBgraphics &copy);  // Verhindere Kopieren


public:
    int xres, yres;         // Aufloesung in Pixel
    int bpp;                // Farbtiefe (Bits per Pixel)
    int lfb;                // Adresse des Linearen Framebuffers

    
    LFBgraphics () {}
    
    void drawPixel        (int x,  int y,  int col);

    /* hier koennen weitere eigene Grafikfunktionen implementiert werden */
};

#endif
