/*****************************************************************************
 *                                                                           *
 *                                M A I N                                    *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Startroutine, wird direkt vom Bootlader angesprungen,    *
 *                  nachdem dieser in den Protected Mode geschaltet hat und  *
 *                  die GDT und IDT initalisiert hat.                        *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 21.12.2018                      *
 *****************************************************************************/


#include "kernel/Globals.h"
#include "kernel/MemMgmt.h"
#include "kernel/Paging.h"
#include "user/Application.h"
#include "user/PlaySound.h"


// Stack fuer den Hauptthread der Anwendung
static unsigned int appl_stack[1024];


int main() {
    // Bildschirm loeschen.
    kout.clear ();

    // Speicherverwaltung initialisieren
    mm_init();

    // Tastatur-Unterbrechungsroutine einstoepseln
    kb.plugin ();

    // Zeitgeber starten
    pit.plugin ();

    // Paging aktivieren
    pg_init ();

    // Interrupts erlauben (Tastatur)
    cpu.enable_int ();

    Application app(&appl_stack[1023]);
    scheduler.ready(app);

    // Scheduler starten
    scheduler.schedule ();

    return 0;
 }


