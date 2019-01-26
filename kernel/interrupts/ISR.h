/*****************************************************************************
 *                                                                           *
 *                                   I S R                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Interrupt Service Routine. Jeweils ein Objekt pro ISR.   *
 *                  Erlaubt es einen Kontext mit Variablen fuer die Unter-   *
 *                  brechungsroutine bereitzustellen.                        *
 *                                                                           *
 * Autor:           Michael Schoettner, 30.7.16                              *
*****************************************************************************/
#ifndef __ISR_include__
#define __ISR_include__

class ISR {
    
private:
    ISR (const ISR &copy); // Verhindere Kopieren

public:
    ISR () {}
       
    // Compiler erwartet bei vituellen Methoden, auch einen Destruktor.
    virtual ~ISR () {}

    // Unterbrechungsbehandlungsroutine
    virtual void trigger () = 0;
};

#endif
