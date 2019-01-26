/*****************************************************************************
 *                                                                           *
 *                                   C P U                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung einer Abstraktion fuer den Prozessor.    *
 *                  Derzeit wird nur angeboten, Interrupts zuzulassen, zu    *
 *                  verbieten oder den Prozessor anzuhalten.                 *
 *                                                                           *
 * Autor:           Michael Schoettner, 30.7.16                              *
 *****************************************************************************/
#ifndef __CPU_include__
#define __CPU_include__


class CPU {
    
private:
    CPU(const CPU &copy);   // Verhindere Kopieren

public:
    CPU() {}

    // Erlauben von (Hardware-)Interrupts
    inline void enable_int () {
        asm volatile ( "sti" );
    }
    
    // Interrupts werden ignoriert/verboten
    inline void disable_int () {
        asm volatile ( "cli" );
    }
    
    // Prozessor bis zum naechsten Interrupt anhalten
    inline void idle () {
        asm volatile ( "sti;"
                      "hlt"
                      );
    }
    
    // Prozessor anhalten
    inline void halt () {
        asm volatile ( "cli;"
                      "hlt"
                      );
    }
 
    // Time-Stamp-Counter auslesen
    inline unsigned long long int rdtsc() {
        unsigned long long int  ret;
        asm volatile ( "rdtsc" : "=A"(ret) );
        return ret;
    }
};

#endif
