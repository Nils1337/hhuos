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

void Loop::run() {
    while (true) {
        s->p();
        kout.setpos(12 + (id * 20), 10);
        kout << "Loop [" << id << "]: " << counter++;
        kout.flush();
        s->v();
    }
} 
