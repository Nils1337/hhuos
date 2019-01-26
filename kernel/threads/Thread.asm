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
;*                  Jetzt werden alle Register beruecksichtigt!              *
;*                                                                           *
;* Autor:           Olaf Spinczyk, TU Dortmund                               *
;*                  Michael Schoettner, HHU, 1.1.2017                        *
;*****************************************************************************

%include "kernel/threads/Thread.inc"

; EXPORTIERTE FUNKTIONEN

[GLOBAL Thread_start]
[GLOBAL Thread_switch]

; IMPLEMENTIERUNG DER FUNKTIONEN

[SECTION .text]

; COROUTINE_START : Startet die erste Coroutine ueberhaupt.
;
; C Prototyp: void Coroutine_start (struct CoroutineState* regs);

Thread_start:

    mov 	eax,[4+esp]             ; Parameter regs ins eax Register
    mov     esp,[esp_offset+eax]	; Stack Pointer umsetzen
    sti
    ret                             ; Sprung zu 'kickoff'



; THREAD_SWITCH : Threadumschaltung. Der aktuelle Registersatz wird
;                 gesichert und der Registersatz des neuen Threads
;                 wird in den Prozessor eingelesen.
;
; C Prototyp: void Thread_switch (struct ThreadState* regs_now,
;                                 struct ThreadState* reg_then);

Thread_switch:

    mov	eax,[4+esp]                 ; regs_now
    mov	[ebx_offset+eax],ebx        ; nicht-fluechtige Register speichern
    mov	[esi_offset+eax],esi
    mov	[edi_offset+eax],edi
    mov	[esp_offset+eax],esp
    mov	[ebp_offset+eax],ebp

    ; fluechtige Register speichern
    mov	[ecx_offset+eax],ecx
    pushf
    pop ecx
    mov	[efl_offset+eax],ecx
    mov	[edx_offset+eax],edx
    mov	[eax_offset+eax],eax

    mov	eax,[8+esp]                 ; regs_then
    mov	ebx,[ebx_offset+eax]        ; Register wieder herstellen
    mov	esi,[esi_offset+eax]
    mov	edi,[edi_offset+eax]
    mov	esp,[esp_offset+eax]
    mov	ebp,[ebp_offset+eax]

    ; fluechtige Register speichern
    mov ecx,[efl_offset+eax]
    push ecx
    popf
    mov	ecx, [ecx_offset+eax]
    mov	edx, [edx_offset+eax]
    mov	eax, [eax_offset+eax]

    sti
    ret                             ; Threadwechsel !

