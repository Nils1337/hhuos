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

#ifndef __loop_include__
#define __loop_include__

#include "kernel/threads/Thread.h"

class Loop : public Thread {

private:
    int id;
    int counter;

public:
    Loop (int id, unsigned int* stack): Thread(stack), id(id)  {
        counter = 0;
    }

    virtual void run();

};

#endif
