;*****************************************************************************
;*                                                                           *
;*                               T H R E A D                                 *
;*                                                                           *
;*---------------------------------------------------------------------------*
;* Beschreibung:    Assemblerdarstellung der struct ThreadState aus          *
;*                  ThreadState.h                                            *
;*                                                                           *
;*                  Die Reihenfolge der Registerbezeichnungen muss unbedingt *
;*                  mit der von struct ThreadState uebereinstimmen.          *
;*                                                                           *
;* Autor:           Olaf Spinczyk, TU Dortmund                               *
;*                  Michael Schoettner, HHU, 25.8.2016                       *
;*****************************************************************************

; Alle Register des Intel 80386 Prozessors

; Die folgenden Angaben belegen ebx_offset mit dem Wert 0, esi_offset mit 4
; edi_offset mit 8 usw. Dies entspricht den Abstaenden der Elemente ebx, esi,
; edi usw. der Struktur ThreadState zum Anfang der Struktur (in Bytes).

[ABSOLUTE 0]
ebx_offset:	resd 1
esi_offset:	resd 1
edi_offset:	resd 1
ebp_offset:	resd 1
esp_offset:	resd 1
eax_offset:	resd 1
ecx_offset:	resd 1
edx_offset:	resd 1
efl_offset:	resd 1
