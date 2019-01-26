/*****************************************************************************
 *                                                                           *
 *                          D I S P A T C H E R                              *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung des Dispatchers.                         *
 *                  Der Dispatcher verwaltet den life-Pointer, der den       *
 *                  jeweils aktiven Thread angibt. Mit start() wird der      *
 *                  life Pointer initialisiert und der erste Thread ge-      *
 *                  startet, alle weiteren Kontextwechsel werden mit         *
 *                  dispatch() ausgeloest. active() liefert den life Pointer *
 *                  zurueck.                                                 *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *                  Michael Schoettner, HHU, 30.8.2016                       *
 *****************************************************************************/

#ifndef __dispatch_include__
#define __dispatch_include__

#include "kernel/threads/Thread.h"

class Dispatcher {
    
private:
    Thread* life;     // aktiver Thread

    // Variablen fuer erzwungenen Thread-Wechsel in startup.asm
    unsigned int *regs_life;
    unsigned int *regs_next;
    

    Dispatcher(const Dispatcher &copy); // Verhindere Kopieren

public:
    // Initialisierung.
    Dispatcher ();
    
    // Thread erstmalig aufstarten
    void start (Thread& first);
    
    // CPU auf next umschalten
    void dispatch (Thread& next);
    
    // welcher Thread hat aktuell die CPU
    Thread* active () { return life; }
    
    // Thread-Wechsel soll erzwungen werden. 
    // Wird von prepare_preemption des Schedulers gerufen.
    bool prepare_thread_switch (Thread* next);
};

#endif
