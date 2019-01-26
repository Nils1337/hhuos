/*****************************************************************************
 *                                                                           *
 *                                   P I T                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Programmable Interval Timer.                             *
 *                                                                           *
 * Autor:           Michael Schoettner, 25.8.2016                            *
 *****************************************************************************/

#include "devices/PIT.h"
#include "kernel/IOport.h"
#include "kernel/Globals.h"


/*****************************************************************************
 * Methode:         PIT::interval                                            *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Zeitinervall programmieren.                              *
 *                                                                           *
 * Parameter:                                                                *
 *      us:         Zeitintervall in Mikrosekunden, nachdem periodisch ein   * 
 *                  Interrupt erzeugt werden soll.                           *
 *****************************************************************************/
void PIT::interval (int us) {
    ctrl_port.outb(0x36); // select channel 0, lobyte/hibyte access mode, mode 3

    int counter = us * 1000 / time_base; // approximate counter (round down)
    unsigned char lobyte = counter;
    unsigned char hibyte = counter >> 8;

    kout << "PIT init" << endl;
    kout << (int) lobyte << endl;
    kout << (counter >> 8) << endl;

    channel0_port.outb(lobyte);
    channel0_port.outb(hibyte);

    timer_interval = us;
}


/*****************************************************************************
 * Methode:         PIT::plugin                                              *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Unterbrechungen fuer den Zeitgeber erlauben. Ab sofort   *
 *                  wird bei Ablauf des definierten Zeitintervalls die       *
 *                  Methode 'trigger' aufgerufen.                            *
 *****************************************************************************/
void PIT::plugin () {
    intdis.assign(IntDispatcher::timer, *this);
    pic.allow(PIC::timer);
}


/*****************************************************************************
 * Methode:         PIT::trigger                                             *
 *---------------------------------------------------------------------------*
 * Beschreibung:    ISR fuer den Zeitgeber. Wird aufgerufen, wenn der        * 
 *                  Zeitgeber eine Unterbrechung ausloest. Anzeige der Uhr   *
 *                  aktualisieren und Thread wechseln durch Setzen der       *
 *                  Variable 'threadSwitch', wird in 'int_disp' behandelt.   *
 *****************************************************************************/
void PIT::trigger () {
    
    // alle 10ms, Systemzeit weitersetzen
    systime++;

    // if (systime % 100 == 0) {
    //     int x, y;
    //     kout.flush();
    //     kout.getpos(x, y);

    //     if (c == 0) {
    //         c = '|';
    //     }
    //     if (c == '|') {
    //         c = '/';
    //     } else if (c == '/') {
    //         c = '-';
    //     } else if (c == '-') {
    //         c = '\\';
    //     } else if (c == '\\') {
    //         c = '|';
    //     }
    //     kout.setpos(CGA::COLUMNS-1, 0);
    //     kout << c;
    //     kout.flush();
    //     kout.setpos(x, y);
    // }

    // Bei jedem Tick einen Threadwechsel ausloesen.
    // Aber nur wenn der Scheduler bereits fertig intialisiert wurde
    // und ein weiterer Thread rechnen moechte
    if ( scheduler.isInitialized() ) {
        if ( scheduler.prepare_preemption() )
            forceSwitch=1;
    }
}


