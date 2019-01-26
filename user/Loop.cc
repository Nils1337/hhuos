/*****************************************************************************
 *                                                                           *
 *                                 L O O P                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Loop ist eine Koroutine, die nichts weiter tut als einen *
 *                  Zaehler hochzuzaehlen und dies auf dem Bildschirm anzu-  *
 *                  zeigen. Zwischendurch gibt sie die CPU ab.               *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *****************************************************************************/

#include "user/Loop.h"
#include "kernel/Globals.h"
#include "lib/Semaphore.h"

Semaphore *s = new Semaphore(1);
Spinlock *lock = new Spinlock();

void Loop::run() {
    //while (true) {
    for (unsigned int i = 0; i < 100000000; i++) {
        //lock->acquire();
        s->p();
        kout.setpos(12 + (id * 20), 10);
        kout << "Loop [" << id << "]: " << counter++;
        kout.flush();
        s->v();
        //lock->free();
    }
} 
