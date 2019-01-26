/*****************************************************************************
 *                                                                           *
 *                                   P I T                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Programmable Interval Timer.                             * 
 *                                                                           *
 * Autor:           Michael Schoettner, 23.8.2016                            *
 *****************************************************************************/

#ifndef __PIT_include__
#define __PIT_include__

#include "kernel/interrupts/ISR.h"
#include "kernel/IOport.h"

class PIT : public ISR {
    
private:
    PIT(const PIT &copy); // Verhindere Kopieren
    
    const IOport ctrl_port;    // control register
    const IOport channel0_port;    // counter port for channel 0

private:
    enum { time_base = 838 };  /* ns */
    int timer_interval;
    char c;

public:
    // Zeitgeber initialisieren.
    PIT (int us): ctrl_port (0x43), channel0_port (0x40), c('|') {
        interval (us);
    }
    
    // Konfiguriertes Zeitintervall auslesen.
    int interval () {
        return timer_interval;
    }
    
    // Zeitintervall in Mikrosekunden, nachdem periodisch ein Interrupt
    //erzeugt werden soll.
    void interval (int us);

    // Aktivierung der Unterbrechungen fuer den Zeitgeber
    void plugin ();
    
    // Unterbrechnungsroutine des Zeitgebers.
    void trigger ();
};

#endif
