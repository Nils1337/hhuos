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
 *                    0x3FF000:	letzte allozierbare Page                     *
 *                        Heap:                                              *
 *                    0x400000:	Start-Adresse der Heap-Verwaltung            *
 *                        Ende: Letzte Adresse des phys. Speichers           *
 *                                                                           *
 *                                                                           *
 * Autor:           Michael Schoettner, 2.2.2017                             *
 *****************************************************************************/

#ifndef __Paging_include__
#define __Paging_include__

// ativiert paging
extern void pg_init();

// alloziert eine 4 KB Page
extern unsigned int * pg_alloc_page();

// Schreibschutz auf Seite setzen -> fuer debugging nuetzlich
extern void pg_write_protect_page(unsigned int *p_page);

// gibt eine 4 KB Page frei
extern void pg_free_page(unsigned int *p_page);

#endif


