/*****************************************************************************
 *                                                                           *
 *                          T H R E A D S T A T E                            *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Die Struktur ThreadState dient dazu, bei einem Thread-   *
 *                  wechsel die Werte aller Register zu sichern.             *
 *                                                                           *
 * Achtung:         Fuer den Zugriff auf die Elemente von                    *
 *                  struct ThreadState aus einer Assemblerfunktion           *
 *                  heraus werden in der Datei Thread.inc Namen fuer die     *
 *                  benoetigten Abstaende der einzelnen Elemente zum Anfang  *
 *                  der Struktur definiert. Damit dann auch auf die richtigen*
 *                  Elemente zugegriffen wird, sollten sich die Angaben von  *
 *                  ThreadState.h und Thread.inc exakt entsprechen.          *
 *                  Wer also ThreadState.h aendert, muss auch                *
 *                  Thread.inc anpassen (und umgekehrt.)                     *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *                  Michael Schoettner, HHU, 25.8.2016                       *
 *****************************************************************************/

#ifndef __ThreadState_include__
#define __ThreadState_include__
    
struct ThreadState {
    void *ebx;
    void *esi;
    void *edi;
    void *ebp;
    void *esp;
    // nachfolgend die fluechtige Register
    // wichtig fuer preemptives Multitasking
    void *eax;
    void *ecx;
    void *edx;
    void *efl;
};

#endif

