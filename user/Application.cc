/*****************************************************************************
 *                                                                           *
 *                         A P P L I C A T I O N                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 21.8.2016                       *
 *****************************************************************************/

#include "kernel/Globals.h"
#include "user/Application.h"
#include "user/Loop.h"

unsigned int stack[3][1024];

/*****************************************************************************
 * Methode:         Application::run                                         *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Der Anwendungsthread erzeugt drei Threads die Zaehler    *
 *                  ausgeben und terminiert sich selbst.                     *
 *****************************************************************************/
void Application::run () {

    Loop loop(0, &stack[0][1024]);
    Loop loop2(1, &stack[1][1024]);
    Loop loop3(2, &stack[2][1024]);

    scheduler.ready(loop);
    scheduler.ready(loop2);
    scheduler.ready(loop3);

    // for (int i=0; i<1000;i++) {
    //     scheduler.yield();
    // }

    //scheduler.kill(loop2);
    
    // selbst terminieren
    scheduler.exit ();
}
