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
#ifndef __PIC_include__
#define __PIC_include__

class PIC {
    
private:
    PIC(const PIC &copy); // Verhindere Kopieren
public:
    PIC() {}
         
public:
    // Nummern wichtiger an den PIC angeschlossenen Geraete
    enum {
       timer    = 0,   // Programmable Interrupt Timer (PIT)
       keyboard = 1    // Tastatur
    };

    // Weiterleiten von Unterbrechungen eines Geraetes an die CPU
    void allow (int interrupt_device);

    // Unterdruecken eines IRQs
    void forbid (int interrupt_device);

    // Zustand des Maskierbits eines bestimmten IRQs
    bool status (int interrupt_device);
 };

#endif
