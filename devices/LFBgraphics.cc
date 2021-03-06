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

#include "devices/LFBgraphics.h"
#include "user/hhu.cc"

void LFBgraphics::drawSprite() {
    for (int x = 0; x < hhu.width; x++) {
        for (int y = 0; y < hhu.height; y++) {
            int pos = hhu.bytes_per_pixel * (x + y * hhu.width);
            int col = hhu.pixel_data[pos] | (hhu.pixel_data[pos] << 8) | (hhu.pixel_data[pos] << 16);
            drawPixel(x, y, col);
        }
    }
}


void LFBgraphics::drawRect(int x, int y, int width, int height, int col) {    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            drawPixel(x + j, y + i, col);
        }
    }
}

/*****************************************************************************
 * Methode:         LFBgraphics::drawPixel                                   *
 *---------------------------------------------------------------------------*
 * Parameter:       x, y    Koordinaten des Pixels                           *
 *                  col     Farbe                                            *
 *                                                                           *
 * Beschreibung:    Zeichnen eines Pixels.                                   *
 *****************************************************************************/
void LFBgraphics::drawPixel(int x, int y, int col) {
    unsigned char *ptr;
    
    // Pixel ausserhalb des sichtbaren Bereichs?
    if (x<0 || x>=xres || y<0 || y>yres)
        return;
    
    // Adresse des Pixels berechnen und Inhalt schreiben
    switch (bpp) {
        case 8:
            ptr = (unsigned char *)lfb+x+y*xres;
            *ptr = col;
            return;
        case 15:
        case 16:
            ptr = (unsigned char *)lfb+2*x+2*y*xres;
            *ptr = col;
            return;
        case 24:
            ptr = (unsigned char *)lfb+3*x+3*y*xres;
            *ptr = (col & 0xFF); ptr++;
            *ptr = ((col>>8) & 0xFF); ptr++;
            *ptr = ((col>>16) & 0xFF); ptr++;
            return;
        case 32:
            ptr = (unsigned char *)lfb+4*x+4*y*xres;
            *ptr = (col & 0xFF); ptr++;
            *ptr = ((col>>8) & 0xFF); ptr++;
            *ptr = ((col>>16) & 0xFF); ptr++;
            return;
    }
}
