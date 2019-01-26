/*****************************************************************************
 *                                                                           *
 *                                  P I C                                    *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Mit Hilfe des PICs koennen Hardware-Unterbrechungen      *
 *                  einzeln zugelassen oder unterdrueckt werden. Auf diese   *
 *                  Weise wird also bestimmt, ob die Unterbrechung eines     *
 *                  Geraetes ueberhaupt an den Prozessor weitergegeben wird. *
 *                  Selbst dann erfolgt eine Aktivierung der Unterbrechungs- *
 *                  routine nur, wenn der Prozessor bereit ist, auf Unter-   *
 *                  brechungen zu reagieren. Dies kann mit Hilfe der Klasse  *
 *                  CPU festgelegt werden.                                   *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *****************************************************************************/

#include "kernel/interrupts/PIC.h"
#include "kernel/IOport.h"


static IOport IMR1 (0x21);    // interrupt mask register von PIC 1
static IOport IMR2 (0xa1);    // interrupt mask register von PIC 2


/*****************************************************************************
 * Methode:         PIC::allow                                               *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Sorgt dafuer, dass Unterbrechungen des angegebenen       *
 *                  Geraetes vom PIC an den Prozessor weitergereicht werden. *
 *                                                                           *
 *                  Um eine Unterbrechungsbehandlung zu ermoeglichen, muss   *
 *                  zusaetzlich CPU::enable_int() aufgerufen werden. Dies    *
 *                  erfolgt nicht hier, sondern an anderer passender Stelle. *
 *                                                                           *
 * Parameter:                                                                *
 *      interrupt:  IRQ der erlaubt werden soll                              *
 *****************************************************************************/
void PIC::allow (int interrupt) {
    unsigned char imr;
    unsigned char mask = 1;
    if (0 <= interrupt && interrupt <= 15) {
        imr = (interrupt < 8) ? IMR1.inb() : IMR2.inb();
        mask = mask << (interrupt % 8);
        mask = ~mask & imr;
        if (interrupt < 8) {
            IMR1.outb(mask);
        } else {
            IMR2.outb(mask);
        }
    } else {
    //TODO: Bluescreen or Error -> wrong call/parameter
    } 

    // if (0 <= interrupt && interrupt < 8) {
    //     unsigned char mask = IMR1.inb();
    //     mask &= ~(1U << interrupt);
    //     IMR1.outb(mask);
    // } else if (8 <= interrupt && interrupt < 16) {
    //     unsigned char mask = IMR2.inb();
    //     mask &= ~(1U << (interrupt - 8));
    //     IMR2.outb(mask); 
    // }  
}


/*****************************************************************************
 * Methode:         PIC::forbid                                              *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Unterdrueckt mit Hilfe des PICs einen bestimmten         *
 *                  Hardware-Interrupt.                                      *
 *                                                                           *
 * Parameter:                                                                *
 *      interrupt:  IRQ der maskiert werden soll                             *
 *****************************************************************************/
void PIC::forbid (int interrupt) {
    if (0 <= interrupt && interrupt < 8) {
        unsigned char mask = IMR1.inb();
        mask |= 1U << interrupt;
        IMR1.outb(mask);
    } else if (8 <= interrupt && interrupt < 16) {
        unsigned char mask = IMR2.inb();
        mask |= 1U << (interrupt - 8);
        IMR2.outb(mask); 
    }
}


/*****************************************************************************
 * Methode:         PIC::status                                              *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Liefert den aktuellen Zustand des Maskierbits eines      *
 *                  bestimmten Hardware-Interrupts.                          *
 *                                                                           *
 * Parameter:                                                                *
 *      interrupt:  IRQ dessen Status erfragt werden soll                    *
 *****************************************************************************/
bool PIC::status (int interrupt) {
    if (0 <= interrupt && interrupt < 8) {
        unsigned char mask = IMR1.inb();
        return (mask >> interrupt) & 1U;
    } else if (8 <= interrupt && interrupt < 16) {
        unsigned char mask = IMR2.inb();
        return (mask >> (interrupt-8)) & 1U;
    }
    return false;
}
 
