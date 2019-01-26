/*****************************************************************************
 *                                                                           *
 *                               G L O B A L S                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Globale Variablen des Systems.                           *
 *                                                                           *
 * Autor:           Michael Schoettner, 1.1.2017                             *
 *****************************************************************************/

#include "kernel/Globals.h"


//
// Kernel-Klassen
//
CPU              cpu;           // CPU-spezifische Funktionen
PIC              pic;           // Interrupt-Controller
IntDispatcher    intdis;        // Unterbrechungsverteilung
Scheduler        scheduler;     // Scheduler
BIOS             bios;          // Schnittstelle zum 16-Bit BIOS
unsigned int     total_mem;     // RAM total
unsigned long    systime=0;     // wird all 10ms hochgezaehlt
bool             forceSwitch=0; // gesetzt in Timer-Interrupt,
                                // wenn Thread-Wechsel erfolgen soll

//
// Geräte-Treiber-Klassen
//
PCSPK            pcspk;         // PC-Lautsprecher
CGA_Stream       kout;          // Ausgabe-Strom fuer Kernel
Keyboard         kb;            // Tastatur
VESA             vesa;          // VESA-Treiber
PIT              pit(10000);    // Zeitgeber (10ms)
