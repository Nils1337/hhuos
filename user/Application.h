/*****************************************************************************
 *                                                                           *
 *                         A P P L I C A T I O N                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 16.12.2016                      *
 *****************************************************************************/
#ifndef __application_include__
#define __application_include__


#include "kernel/threads/Thread.h"


class Application : public Thread {
    
private:
    Application (const Application &copy); // Verhindere Kopieren

public:
    // Gib dem Anwendungsthread einen Stack.
    Application (unsigned int* stack) : Thread (stack) { }

    // Thread-Startmethode
    void run ();

 };

#endif
