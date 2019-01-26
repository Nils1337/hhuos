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


// Hilfsfunktion: Auf Return-Taste warten
void waitForReturn() {
    kb.lastKey='*'; // lastKey loeschen
    while (kb.lastKey!=(char)10) ; // dauernd abfrage (schlechter Stil)
}


// Stack fuer den Hauptthread der Anwendung
static unsigned int appl_stack[1024];


int main() {
    // Bildschirm loeschen.
    kout.clear ();

    // Speicherverwaltung initialisieren
    mm_init();
    
    // Startmeldung ausgeben
    kout << "HHUos 0.9" << endl << "=========" << endl << endl;
    kout << "Freier Speicher: " << (total_mem/1024) << " KB" << endl << endl;

    kout << "Unterstuetzte Funktionen:" << endl;
    kout << "   - Bildschirmausgaben" << endl;
    kout << "   - Sound ueber den PC-Lautsprecher" << endl;
    kout << "   - Tastatureingaben per Interrupt" << endl;
    kout << "   - Preemptives Multitasking" << endl;
    kout << "   - VESA ueber BIOS" << endl;
    kout << "   - Einfache Heap-Verwaltung" << endl;
    kout << "   - Paging und Bluescreen" << endl;
    kout << endl;
    kout.flush ();


    // Tastatur-Unterbrechungsroutine einstoepseln
    kb.plugin ();

    // Zeitgeber starten
    pit.plugin ();

    // Paging aktivieren
    pg_init ();

    // Interrupts erlauben (Tastatur)
    cpu.enable_int ();

    kout << "Bitte <ENTER> druecken um fortzufahren." << endl;
    waitForReturn();
    kout.clear ();
    
    // Anwendung im Scheduler anmelden
    Application demoApp(&appl_stack[1024]);
    scheduler.Scheduler::ready(demoApp);
    
    // Scheduler starten
    scheduler.schedule ();

    return 0;
 }
