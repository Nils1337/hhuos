/*****************************************************************************
 *                                                                           *
 *                                B I O S                                    *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    BIOS-Schnittstelle                                       *
 *                                                                           *
 * Autor:           Michael Schoettner, 19.9.2016                            *
 *****************************************************************************/

#include "kernel/Globals.h"
#include "kernel/BIOS.h"


// 16-Bit Code aufrufen, siehe Konstruktor und Aufruf in startup.asm
extern "C" { void bios_call(); }

// in startup.asm im GDT-Eintrag so festgeschrieben!
#define BIOS16_CODE_MEMORY_START    0x24000     

// Parameter fuer BIOS-Aufrufe (Register)
#define BIOS16_PARAM_BASE           0x26000


// Zeiger auf Speichbereich fuer Parameter fuer BIOS-Aufruf (siehe BIOS.h)
struct BIOScall_params* BC_params = (struct BIOScall_params*)BIOS16_PARAM_BASE;


/*****************************************************************************
 * Methode:         BIOS::BIOS                                               *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Konstruktor. Baut manuell ein 16-Bit Code Segment fuer   * 
 *                  den BIOS-Aufruf. Startadresse dieser Funktion steht      *
 *                  im 4. GDT-Eintrag (siehe startup.asm).                   *
 *****************************************************************************/
BIOS::BIOS() {
    unsigned char *codeAddr = (unsigned char*)BIOS16_CODE_MEMORY_START;
    
    // mov eax, 25000
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0xB8;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x50;   codeAddr++;
    *codeAddr = 0x02;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // mov [eax], esp
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x67;   codeAddr++;
    *codeAddr = 0x89;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    
    
    
    // mov eax,cr0
    *codeAddr = 0x0F;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    *codeAddr = 0xC0;   codeAddr++;
    
    // and eax, 7FFEFFFE
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x25;   codeAddr++;
    *codeAddr = 0xFE;   codeAddr++;
    *codeAddr = 0xFF;   codeAddr++;
    *codeAddr = 0xFE;   codeAddr++;
    *codeAddr = 0x7F;   codeAddr++;
    
    // mov cr0, eax
    *codeAddr = 0x0F;   codeAddr++;
    *codeAddr = 0x22;   codeAddr++;
    *codeAddr = 0xC0;   codeAddr++;
    
    // jmp 2400:001B flush pipeline & switch decoding unit
    // 2400:001B (2400<<4 = 24000 + 1B)
    *codeAddr = 0xEA;   codeAddr++;
    *codeAddr = 0x1B;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x24;   codeAddr++;
    
    // mov dx,2400
    *codeAddr = 0xBA;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x24;   codeAddr++;
    
    // mov ss,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xD2;   codeAddr++;
    
    // mov gs,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xEA;   codeAddr++;
    
    // mov esp,2000 -> BIOS16_PARAM_BASE 0x260000
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0xBC;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // pop ds
    *codeAddr = 0x1F;   codeAddr++;
    
    // pop es
    *codeAddr = 0x07;   codeAddr++;
    
    // pop fs
    *codeAddr = 0x0f;   codeAddr++;
    *codeAddr = 0xa1;   codeAddr++;
    
    // pop ax -> we have to pop something for symmetry
    *codeAddr = 0x58;   codeAddr++;
    
    // popad
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x61;   codeAddr++;
    
    // interrupt number (written here)
    *codeAddr = 0xCD;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // pushad
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x60;   codeAddr++;
    
    // pushf
    *codeAddr = 0x9C;   codeAddr++;
    
    // push fs
    *codeAddr = 0x0f;   codeAddr++;
    *codeAddr = 0xa0;   codeAddr++;
    
    // push es
    *codeAddr = 0x06;   codeAddr++;
    
    // push ds
    *codeAddr = 0x1E;   codeAddr++;
    
    // mov eax,cr0
    *codeAddr = 0x0F;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    *codeAddr = 0xC0;   codeAddr++;
    
    // or eax, 00010001 (protected mode without paging)
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x0D;   codeAddr++;
    *codeAddr = 0x01;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x01;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;

    // mov cr0, eax
    *codeAddr = 0x0F;   codeAddr++;
    *codeAddr = 0x22;   codeAddr++;
    *codeAddr = 0xC0;   codeAddr++;
    
    // jmp 0018:0049, flush pipeline & switch decoding
    // 0018:0049
    *codeAddr = 0xEA;   codeAddr++;
    *codeAddr = 0x49;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x18;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // mov dx,0010
    *codeAddr = 0xBA;   codeAddr++;
    *codeAddr = 0x10;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // mov ds,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xDA;   codeAddr++;
    
    // mov es,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xC2;   codeAddr++;
    
    // mov es,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xE2;   codeAddr++;
    
    // mov fs,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xEA;   codeAddr++;
    
    // mov ss,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xD2;   codeAddr++;
    
    // mov eax, 25000
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0xB8;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x50;   codeAddr++;
    *codeAddr = 0x02;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // mov esp, [eax]
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x67;   codeAddr++;
    *codeAddr = 0x8B;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    
    // far ret
    *codeAddr = 0x66;	codeAddr++;
    *codeAddr = 0xCB;   codeAddr++;
}


/*****************************************************************************
 * Methode:         BIOS::Int                                                *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Fuehrt einen BIOS-Aufruf per Software-Interrupt durch.   *
 *****************************************************************************/
void BIOS::Int(int inter) {
    unsigned char *ptr = (unsigned char*)BIOS16_CODE_MEMORY_START;

    
    // Interrupt-Nummer in 16-Bit Code-Segment schreiben (unschoen, aber ...)
    *(ptr+48) = (unsigned char)inter;
    
    cpu.disable_int ();             // Interrupts abschalten
    
    bios_call ();
    
    cpu.enable_int ();
}
