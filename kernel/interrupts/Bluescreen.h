/*****************************************************************************
 *                                                                           *
 *                          B L U E S C R E E N                              *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Ein Bluescreen, falls eine x86 Exception auftritt. Evt.  *
 *                  ist der Stack und oder Heap kaputt, weswegen hier nicht  *
 *                  kout etc. verwendet wird.                                *
 *                                                                           *
 * Autor:           Michael Schoettner, 2.2.2017                             *
 *****************************************************************************/

#ifndef __Bluescreen_include__
#define __Bluescreen_include__

// dump blue screen (will not return)
void bs_dump (unsigned int exceptionNr);

#endif


