/*****************************************************************************
 *                                                                           *
 *                          B L U E S C R E E N                              *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Ein Bluescreen, falls eine x86 Exception auftritt. Evt.  *
 *                  ist der Stack und oder Heap kaputt, weswegen hier nicht  *
 *                  kout etc. verwendet wird.                                *
 *                                                                           *
 * Autor:           Michael Schoettner, 11.12.2018                           *
 *****************************************************************************/
#include "kernel/Globals.h"
#include "devices/CGA.h"


// in startup.asm
extern "C" {
    // CR2 auslesen
    unsigned int get_page_fault_address();
    
    // 1st level interrupt handler in startup.asm sichert Zeiger auf Stackframe
    // unmittelbar nach dem Interrupt und nachdem alle Register mit PUSHAD
    // gesichert wurden
    // |-------------|
    // |    EFLAGS   |
    // |-------------|
    // |      CS     |
    // |-------------|
    // |     EIP     |
    // |-------------|
    // | [ErrorCode] |
    // |-------------|
    // |     EAX     |
    // |-------------|
    // |     ECX     |
    // |-------------|
    // |     EDX     |
    // |-------------|
    // |     EBX     |
    // |-------------|
    // |     ESP     |
    // |-------------|
    // |     EBP     |
    // |-------------|
    // |     ESI     |
    // |-------------|
    // |     EDI     |
    // |-------------| <-- int_esp

    void get_int_esp (unsigned int** esp);

}


void break_on_bluescreen() {
    
    /* wenn auf diese Methode ein breakpoint in GDB gesetzt wird
     so kann man sich mithilfe des Hex-Dumps umsehen
     */
    
}

// Cursor-Position
int bs_xpos = 0;
int bs_ypos = 0;


/*****************************************************************************
 * Konstruktor:     bs_clear                                                 *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Bildschirm loeschen.                                     *
 *****************************************************************************/
void bs_clear() {
    unsigned int x,y;
    unsigned short *ptr = (unsigned short*)0xb8000;
    
    for (x=0; x<80; x++) {
        for (y=0; y<25; y++)
    	*(ptr + y*80 + x) = (short)0x1F00;
    }

    bs_xpos = 0;
    bs_ypos = 0;
}


/*****************************************************************************
 * Konstruktor:     bs_lf                                                    *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Zeilenvorschub.                                          *
 *****************************************************************************/
void bs_lf() {
    bs_ypos++;
    bs_xpos=0;
}


/*****************************************************************************
 * Konstruktor:     bs_print_char                                            *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Ein Zeichen ausgeben.                                    *
 *****************************************************************************/
void bs_print_char(char c) {
    unsigned char *ptr = (unsigned char*)0xb8000;
    
    *(ptr + bs_ypos*80*2 + bs_xpos*2) = c;
    bs_xpos++;
}


/*****************************************************************************
 * Konstruktor:     bs_print_string                                          *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Eine Zeichenkette ausgeben.                              *
 *****************************************************************************/
void bs_print_string(char *str) {
    
    while ( *str != '\0' ) {
        bs_print_char( *str );
        str++;
    }
}


/*****************************************************************************
 * Konstruktor:    bs_printHexDigit                                          *
 *---------------------------------------------------------------------------*
 * Beschreibung:   Ein Hex-Zeichen ausgeben.                                 *
 *****************************************************************************/
void bs_printHexDigit(int c) {
    if (c<10)   bs_print_char('0'+(unsigned char)c);
    else        bs_print_char('A'+(unsigned char)(c-10));
}


/*****************************************************************************
 * Konstruktor:    bs_print_uintHex                                          *
 *---------------------------------------------------------------------------*
 * Beschreibung:   Integer ausgeben.                                         *
 *****************************************************************************/
void bs_print_uintHex(unsigned int c) {
    for (int i=28; i>=0; i=i-4) {
        bs_printHexDigit( (c>>i)&0xF );
    }
}


/*****************************************************************************
 * Konstruktor:     bs_printReg                                              *
 *---------------------------------------------------------------------------*
 * Beschreibung:    String mit Integer ausgeben. Wird verwendet um ein       *
 *                  Register auszugeben.                                     *
 *****************************************************************************/
void bs_printReg(char *str, int value) {
    bs_print_string(str);
    bs_print_uintHex(value);
    bs_print_string("   \0");
}


/*****************************************************************************
 * Konstruktor:     bs_dump                                                  *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Hauptroutine des Bluescreens.                            *
 *****************************************************************************/
void bs_dump (unsigned int exceptionNr) {
    unsigned int *int_esp;
    unsigned int *sptr;
    unsigned int faultAdress;
    unsigned int error_code=0;
    
    bs_clear ();
    bs_print_string("HHUos crashed with Exception \0");

    // page fault?
    if (exceptionNr==14) {
        faultAdress = get_page_fault_address();
        if ((faultAdress & 0xFFFFF000) == 0)
            exceptionNr=0x1B;
    }

    bs_print_uintHex(exceptionNr);
    bs_print_string(" (\0");
    
    // Spruch ausgeben
    switch (exceptionNr) {
        case 0x00: bs_print_string("Divide Error\0");                   break;
        case 0x01: bs_print_string("Debug Exception\0");                break;
        case 0x02: bs_print_string("NMI\0");                            break;
        case 0x03: bs_print_string("Breakpoint Exception\0");           break;
        case 0x04: bs_print_string("Into Exception\0");                 break;
        case 0x05: bs_print_string("Index out of range Exception\0");   break;
        case 0x06: bs_print_string("Invalid Opcode\0");                 break;
        case 0x08: bs_print_string("Double Fault\0");                   break;
        case 0x0D: bs_print_string("General Protection Error\0");       break;
        case 0x0E: bs_print_string("Page Fault\0");                     break;
        case 0x18: bs_print_string("Stack invalid\0");                  break;
        case 0x19: bs_print_string("Return missing\0");                 break;
        case 0x1A: bs_print_string("Type Test Failed\0");               break;
        case 0x1B: bs_print_string("Null pointer exception\0");         break;
        case 0x1C: bs_print_string("MAGIC.StackTest failed\0");         break;
        case 0x1D: bs_print_string("Memory-Panic\0");                   break;
        case 0x1E: bs_print_string("Pageload failed\0");                break;
        case 0x1F: bs_print_string("Stack overflow\0");                 break;
        default:   bs_print_string("unknown\0");
    }
    bs_print_string(")\0"); bs_lf();

    // exception mit error-code auf Stack?
    if ( (exceptionNr>=8 && exceptionNr<=14) || exceptionNr==17 || exceptionNr==30)
        error_code = 1;
 
    // Zeiger auf int_esp ueber startup.asm beschaffen (Stack-Layout siehe Anfang dieser Datei)
    get_int_esp (&int_esp);
    
    // wir müssen den Inhalt auslesen und das als Zeiger verwenden, um den Stack auszulesen
    sptr = (unsigned int*) *int_esp;

    bs_lf();
    
    // wichtigste Register ausgeben
    bs_printReg("EIP=\0", *(sptr + 8 + error_code) );
    bs_printReg("EBP=\0", *(sptr + 2) );
    bs_printReg("ESP=\0", *(sptr + 3) );
    bs_printReg(" CS=\0", *(sptr + 9 + error_code) );
    bs_lf();

    // verbleibende nicht-fluechtige Register ausgeben
    bs_printReg("EBX=\0", *(sptr + 4) );
    bs_printReg("ESI=\0", *(sptr + 1) );
    bs_printReg("EDI=\0", *(sptr) );
    bs_lf();
    
    // verbleibende fluechtige Register ausgeben
    bs_printReg("EDX=\0", *(sptr + 5) );
    bs_printReg("ECX=\0", *(sptr + 6) );
    bs_printReg("EAX=\0", *(sptr + 7) );
    bs_printReg("EFL=\0", *(sptr + 10 + error_code) );
    bs_lf();


    if (exceptionNr==14 || exceptionNr==0x1B) {
        bs_lf();
        bs_print_string("Fault address = \0");
        bs_print_uintHex(faultAdress);
        bs_lf();
        
        bs_print_string("Last useable address = \0");
        bs_print_uintHex(total_mem);
        
        bs_lf();
    }

    if ( (exceptionNr>=8 && exceptionNr<=14) || exceptionNr==17 || exceptionNr==30) {
        bs_print_string("Error code = \0");
        bs_print_uintHex( *(sptr + 8) );
        bs_lf();
    }

    // Calling stack ...
    bs_lf();
    bs_print_string("Calling Stack:\0"); bs_lf();
    int x = 0;
    unsigned int *ebp = (unsigned int *) *(sptr + 2);
    unsigned int raddr;
    
    // solange eip > 1 MB && ebp < 128 MB, max. Aufruftiefe 10
    while ( *ebp>0x100000 &&  *ebp<0x8000000 && x<10 ) {
        
        raddr = *(ebp+1);
        bs_printReg("   raddr=\0", raddr ); bs_lf();

        // dynamische Kette -> zum Aufrufer
        ebp = (unsigned int*) *ebp;
        
        x++;
    }
    if (x==0) {
        bs_print_string("   empty\0"); bs_lf();
    }
    bs_lf();
    
    // nur falls gdb benutzt werden soll
    break_on_bluescreen();
    
    bs_print_string("System halted\0");
}


