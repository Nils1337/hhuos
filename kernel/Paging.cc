/*****************************************************************************
 *                                                                           *
 *                              P A G I N G                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Rudimentaeres Paging: 1:1 Mapping fuer gesamten logischen*
 *                  Adressraum. logische Adresse = physikalische Adresse     *
 *                                                                           *
 *                  Page-Directory (alle Eintraege present, read/write       *
 *                      1. Eintrag zeigt auf eine Page-Table (4 KB Eintraege)*
 *                      2. restl. Eintraege sind 4 MB Seiten (ohne Page-Tab.)*
 *                                                                           *
 *                  Page-Table (Logische Adressen 0 - 4 MB)                  *
 *                      1. Eintrag not present, read-only                    *
 *                          -> Null-Pointer abfangen                         *
 *                      2. restl. Eintraege present & read/write             *
 *                                                                           *
 * Memory-Laylout                                                            *
 *                                                                           *
 *                  boot.asm                                                 *
 *                      0x07c0: Bootsector vom BIOS geladen                  *
 *                      0x0060: Boot-Code verschiebt sich hier hin           *
 *                      0x9000: Setup-Code (max. 64K inkl. Stack) vom        *
 *                              Bootsector-Code geladen                      *
 *                  setup.asm                                                *
 *                      0x1000: System-Code (max. 512K) geladen              *
 *                  BIOS-Aufruf                                              *
 *                     0x24000: Parameter fuer BIOS-Aufurf                   *
 *                     0x25000: Altes ESP sichern, vor BIOS-Aufruf           *
 *                     0x26000: 16-Bit Code-Segment fuer BIOS-Aufurf         *
 *                  System-Code                                              *
 *                    0x100000:	System-Code, kopiert nach Umschalten in      *
 *                              den Protected Mode kopiert (GRUB kann nur    *
 *                              an Adressen >1M laden)                       *
 *           Globale Variablen: Direkt nach dem Code liegen die globalen     *
 *                              Variablen.                                   *
 *                      Paging:                                              *
 *                    0x200000:	Page-Directory                               *
 *                    0x201000:	Page-Table                                   *
 *                    0x202000:	erste allozierbare Page (via Paging.cc)      *
 *                    0x3FF000:	Anfang der letzten allozierbaren Page        *
 *                        Heap:                                              *
 *                    0x400000:	Start-Adresse der Heap-Verwaltung            *
 *                        Ende: Letzte Adresse des phys. Speichers           *
 *                                                                           *
 *                                                                           *
 * Autor:           Michael Schoettner, 15.12.2018                           *
 *****************************************************************************/
#include "kernel/Globals.h"
#include "kernel/Paging.h"


// Bits fuer Eintraege in der Page-Table
#define PAGE_PRESENT        0x001
#define PAGE_WRITEABLE      0x002
#define PAGE_BIGSIZE        0x080
#define PAGE_RESERVED       0x800   // Bit 11 ist frei fuer das OS

// Adresse des Page-Directory (benoetigt 4 KB)
#define PAGE_DIRECTORY      0x200000

// Adresse der Page-Table (benoetigt 4 KB)
#define PAGE_TABLE          0x201000

// Start- und End-Adresse der 4 KB Seiten die durch die Page-Table adressiert werden 
#define FST_ALLOCABLE_PAGE  0x202000
#define LST_ALLOCABLE_PAGE  0x3FF000


// Externe Funktionen in startup.asm 
extern "C" {
    void paging_on (unsigned int* p_pdir);		// Paging einschalten
    void invalidate_tlb_entry(unsigned int* ptr);	// Page in TLB invalid.
}


/*****************************************************************************
 * Funktion:        pg_alloc_page                                            *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Alloziert eine 4 KB Seite. Allozieren heisst hier        *
 *                  lediglich Setzen eines eigenen RESERVED-Bits.            *
 *****************************************************************************/
 unsigned int * pg_alloc_page() {
    unsigned int *p_page;
    
    p_page = (unsigned int*) PAGE_TABLE;
    
    // 1. Eintrag ist fuer Null-Pointer-Exception reserviert
    // ausserdem liegt an die Page-Table an Adresse PAGE_TABLE
    // somit ist est PAGE_TABLE + 4 KB frei (bis max. 3 MB, da beginnt der Heap)
    for (int i = 1; i < 1024; i++) {
        p_page ++;
        // pruefe ob Page frei
        if ( ((*p_page) & PAGE_RESERVED) == 0) {
            *p_page = ( *p_page | PAGE_RESERVED);
            return (unsigned int*)(i<<12);
        }
    }
    return 0;
}


/*****************************************************************************
 * Funktion:        pg_write_protect_page                                    *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Schreibschutz fuer die uebergebene Seite aktivieren.     *
 *                  Dies fuer das Debugging nuetzlich.                       *
 *****************************************************************************/
void pg_write_protect_page(unsigned int *p_page) {
    invalidate_tlb_entry(p_page); // in startup.asm
 
    int idx = (unsigned int)p_page >> 12;
   
    // ausserhalb Page ?
    if (idx < 1 || idx > 1023) return ;

    p_page = (unsigned int*) PAGE_TABLE;
    p_page += idx;
    *p_page = ( *p_page & ~PAGE_WRITEABLE);
}


/*****************************************************************************
 * Funktion:        pg_notpresent_page                                       *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Seite als ausgelagert markieren. Nur fuer Testzwecke.    *
 *****************************************************************************/
void pg_notpresent_page(unsigned int *p_page) {
    invalidate_tlb_entry(p_page); // in startup.asm
 
    int idx = (unsigned int)p_page >> 12;
   
    // ausserhalb Page ?
    if (idx < 1 || idx > 1023) return ;

    p_page = (unsigned int*) PAGE_TABLE;
    p_page += idx;
    *p_page = ( *p_page & ~PAGE_PRESENT);
}


/*****************************************************************************
 * Funktion:        pg_init                                                  *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Page-Tables einrichten und Paging mithilfe von           * 
 *                  startup.asm aktivieren.                                  *
 *****************************************************************************/
void pg_init() {
    unsigned int i;
    unsigned int *p_pdir;   // Zeiger auf Page-Directory
    unsigned int *p_page;   // Zeiger auf einzige Page-Table fuer 4 KB Pages
    unsigned int num_pages; // Anzahl 4 MB Pages die phys. Adressraum umfassen
    
    // wie viele 4 MB Seiten sollen als 'Present' angelegt werden,
    // sodass genau der physikalische Adressraum abgedeckt ist?
    num_pages = total_mem/(4096*1024);
    
    // kout << "pg_init: " << total_mem << endl;
    
    // kout << "   total_mem: " << total_mem << endl;
    // kout << "   #pages: " << total_mem/(4096*1024) << endl;
    

    //
    // Aufbau des Page-Directory
    //
    
    // Eintrag 0: Zeiger auf 4 KB Page-Table
    p_pdir = (unsigned int*) PAGE_DIRECTORY;
    *p_pdir = PAGE_TABLE | PAGE_WRITEABLE | PAGE_PRESENT;

    // Eintraege 1-1023: Direktes Mapping (1:1) auf 4 MB Pages (ohne Page-Table)
    for (i = 1; i < 1024; i++) {
        p_pdir ++;
        if (i>num_pages)
            *p_pdir = ( (i<<22) | PAGE_BIGSIZE);
        else
            *p_pdir = ( (i<<22) | PAGE_BIGSIZE | PAGE_WRITEABLE | PAGE_PRESENT);
    }
    

    //
    // 1. Page-Table
    //
    p_page = (unsigned int*) PAGE_TABLE;
    
    // ersten Eintrag loeschen -> not present, write protected -> Null-Pointer abfangen
    *p_page = PAGE_RESERVED | PAGE_PRESENT;

    // Eintraege 1-1023: Direktes Mapping (1:1) auf 4 KB page frames
    for (i = 1; i < 1024; i++) {
        p_page ++;
        
        // Seiten unter FST_ALLOCABLE_PAGE reservieren, damit diese nicht
        // alloziert werden und das System kaputt geht
        if ( (i<<12) >= FST_ALLOCABLE_PAGE)
            *p_page = ((i << 12) | PAGE_WRITEABLE | PAGE_PRESENT);
        else
            *p_page = ((i << 12) | PAGE_WRITEABLE | PAGE_PRESENT | PAGE_RESERVED);
    }
    
    // Paging aktivieren (in startup.asm)
    paging_on( (unsigned int*) PAGE_DIRECTORY );
}
