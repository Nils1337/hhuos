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

#include "kernel/threads/Dispatch.h"


//
// startup.asm hat 2 Variablen fuer den erzwungenen Thread-Wechsel
// Wir speichern einmalig die Zeiger darauf, um spaeter darauf zugreifen zu
// koennen und damit der Assembler-Code den Thread-Wechsel machen kann.
extern "C" void get_thread_vars (unsigned int** regs_life,
                                 unsigned int** reg_next);


/*****************************************************************************
 * Methode:         Dispatcher::Dispatcher                                   *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Konstruktor. Liest Adressen der Variablen fuer den       *
 *                  Thread-Wechsel aus startup.asm aus.                      *
 *                                                                           *
 * Parameter:                                                                *
 *      first       Zu startender Thread.                                    *
 *****************************************************************************/
Dispatcher::Dispatcher () : life (0) {
    // Variablen fuer Thread-Wechsel auslesen (in startup.asm definiert)
    get_thread_vars(&regs_life, &regs_next);
}


/*****************************************************************************
 * Methode:         Dispatcher::start                                        *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread starten.                                          *
 *                                                                           *
 * Parameter:                                                                *
 *      first       Zu startender Thread.                                    *
 *****************************************************************************/
void Dispatcher::start (Thread& first) {
    if (!life) {
        life = &first;
        life->start();
    }
}


/*****************************************************************************
 * Methode:         Dispatcher::dispatch                                     *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Auf einen gegebenen Thread wechseln. Wird gerufen, wenn  *
 *                  ein Thread CPU freiwillig abgibt.                        *
 *                                                                           *
 * Parameter:                                                                *
 *      next        Thread der die CPU erhalten soll.                        *
 *****************************************************************************/
void Dispatcher::dispatch (Thread& next) {
    Thread* current = life;
    life = &next;
    current->switchTo (next);
}


/*****************************************************************************
 * Methode:         Dispatcher::prepare_thread_switch                        *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread-Wechsel soll erzwungen werden. Wird von           *
 *                  prepare_preemption des Schedulers gerufen.               *
 *                                                                           *
 * Parameter:                                                                *
 *      next        Thread der die CPU erhalten soll.                        *
 *                                                                           *
 * Rueckgabewert:   true:   Var. fuer Thread-Wechsel in startup.asm gesetzt  *
 *                  false:  kein anderer Thread will rechnen. Nichts tun.    *
 *****************************************************************************/
bool Dispatcher::prepare_thread_switch (Thread* next) {
    
    *regs_life = (unsigned int)&(life->regs);
    *regs_next = (unsigned int)&(next->regs);

    life = next;

    return true;
}

