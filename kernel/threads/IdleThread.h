/*****************************************************************************
 *                                                                           *
 *                             I D L E T H R E A D                           *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Wird nur aktiviert, wenn kein Thread arbeiten moechte.   *
 *                                                                           *
 * Autor:           Michael, Schoettner, HHU, 15.12.2016                     *
 *****************************************************************************/

#ifndef __IdleThread_include__
#define __IdleThread_include__


#include "kernel/threads/Thread.h"
#include "kernel/Globals.h"


class IdleThread : public Thread {
    
private:
    IdleThread(const Thread &copy); // Verhindere Kopieren

public:
    IdleThread (unsigned int* stack) : Thread (stack) {  }
    
    void run () {
        while (1) {
            scheduler.yield ();
        }
    }
    
 };

#endif
