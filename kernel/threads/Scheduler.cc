/*****************************************************************************
 *                                                                           *
 *                          S C H E D U L E R                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung eines einfachen Zeitscheiben-Schedulers. *
 *                  Rechenbereite Threads werden in 'readlist' verwaltet.    *
 *                                                                           *
 * Autor:           Michael, Schoettner, HHU, 30.8.2016                      *
 *****************************************************************************/

#include "kernel/threads/Scheduler.h"
#include "kernel/threads/IdleThread.h"
#include "kernel/Globals.h"


// Leerlauf-Thread
unsigned int IdleThreadStack[1024];
IdleThread idle(&IdleThreadStack[1024]);


/*****************************************************************************
 * Methode:         Scheduler::Scheduler                                     *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Konstruktor des Schedulers. Registriert den              *
 *                  Leerlauf-Thread.                                         *
 *****************************************************************************/
Scheduler::Scheduler () {
    initialized = false;

    // Leerlauf-Thread einfuegen.
    ready (idle);
}

    
/*****************************************************************************
 * Methode:         Scheduler::schedule                                      *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Scheduler starten. Wird nur einmalig aus main.cc gerufen.*
 *****************************************************************************/
void Scheduler::schedule () {
    Thread* first;
    cpu.disable_int();

    first = (Thread*) readyQueue.dequeue();
    initialized = true;
    start (*first);
}


/*****************************************************************************
 * Methode:         Scheduler::ready                                         *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread in Ready-Queue eintragen.                         *
 *                                                                           *
 * Parameter:                                                                *
 *      that        Einzutragender Thread                                    *
 *****************************************************************************/
void Scheduler::ready (Thread& that) {
    // Thread-Wechsel durch PIT verhindern
    cpu.disable_int ();

    readyQueue.enqueue (&that);

    // Thread-Wechsel durch PIT jetzt wieder erlauben
    cpu.enable_int ();
}


/*****************************************************************************
 * Methode:         Scheduler::exit                                          *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread ist fertig und terminiert sich selbst. Er traegt  *
 *                  sich hierzu aus der Ready-Queue aus und wird somit nicht *
 *                  mehr aktiviert.                                          *
 *****************************************************************************/
void Scheduler::exit () {
    // Thread-Wechsel durch PIT verhindern
    cpu.disable_int ();

    // hole naechsten Thread aus ready-Liste.
    Thread* next = (Thread*) readyQueue.dequeue();
    
    // next aktivieren
    dispatch (*next);

    // Interrupts werden in Thread_switch in Thread.asm wieder zugelassen
    // dispatch kehr nicht zurueck
}


/*****************************************************************************
 * Methode:         Scheduler::kill                                          *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread mit 'Gewalt' terminieren.                         *
 *                                                                           *
 * Parameter:                                                                *
 *      that        Zu terminierender Thread                                 *
 *****************************************************************************/
void Scheduler::kill (Thread& that) {
    // Thread-Wechsel durch PIT verhindern
    cpu.disable_int ();

    readyQueue.remove (&that);
    
    // Thread-Wechsel durch PIT jetzt wieder erlauben
    cpu.enable_int ();
}


/*****************************************************************************
 * Methode:         Scheduler::yield                                         *
 *---------------------------------------------------------------------------*
 * Beschreibung:    CPU freiwillig abgeben und Auswahl des naechsten Threads.*
 *****************************************************************************/
void Scheduler::yield () {
    Thread* next;

    // Thread-Wechsel durch PIT verhindern
    cpu.disable_int ();

    // kein anderer Thread rechenbreit?
    if ( isThreadWaiting() == false) {
        // Thread-Wechsel durch PIT jetzt wieder erlauben
        cpu.enable_int ();
        return ;
    }

    next = (Thread*) readyQueue.dequeue ();
    
    readyQueue.enqueue ((Thread*) active());

    dispatch (*next);
    
    // Interrupts werden in Thread_switch in Thread.asm wieder zugelassen
    // dispatch kehr nicht zurueck
}

void Scheduler::block () {
    cpu.disable_int();
    Thread* next;
    next = (Thread*) readyQueue.dequeue ();
    dispatch (*next);
}

void Scheduler::deblock(Thread &that) {
    cpu.disable_int();
    readyQueue.enqueue(&that);
    cpu.enable_int();
}


/*****************************************************************************
 * Methode:         Scheduler::prepare_preemption                            *
 *---------------------------------------------------------------------------*
 * Beschreibung:    CPU soll aktuellem Thread entzogen werden. Wird nur      *
 *                  aus dem Zeitgeber-Interrupt-Handler aufgerufen. Daher    *
 *                  muss nicht gegenueber Interrupts synchronisiert werden.  *
 *                                                                           *
 * Rueckgabewert:   true:   Var. fuer Thread-Wechsel in startup.asm gesetzt  *
 *                  false:  kein anderer Thread will rechnen. Nichts tun.    *
 *****************************************************************************/
bool Scheduler::prepare_preemption () {
    
    // kein anderer Thread rechenbreit?
    // sollte nier auftreten, der Leerlauf-Thread sollte immer bereit sein
    if ( isThreadWaiting() == false)
        return false;
    
    Thread* act  = (Thread*) active();
    Thread* next = (Thread*) readyQueue.dequeue ();
    
    readyQueue.enqueue (act);

    prepare_thread_switch (next);
    
    return true;
}


