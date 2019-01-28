/*****************************************************************************
 *                                                                           *
 *                         A P P L I C A T I O N                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 21.8.2016                       *
 *****************************************************************************/

#include "kernel/Globals.h"
#include "user/Application.h"
#include "user/Loop.h"
#include "user/PlaySound.h"
#include "lib/Semaphore.h"
#include "kernel/Paging.h"
#include "kernel/MemMgmt.h"
#include "user/VBEdemo.h"

/*****************************************************************************
 * Methode:         Application::run                                         *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Der Anwendungsthread erzeugt drei Threads die Zaehler    *
 *                  ausgeben und terminiert sich selbst.                     *
 *****************************************************************************/
void Application::run () {

    showMainMenu();
    waitForInput();

    // selbst terminieren
    scheduler.exit ();
}

void Application::startPlaySoundApp() {
    kout.clear();
    kout << "Playing sound..." << endl;

    unsigned int *stack = new unsigned int[1024];
    PlaySound *sound = new PlaySound(&stack[1024]);
    scheduler.ready(*sound);

    while (kb.get_last_key() != '\b');

    scheduler.kill(*sound);
    pcspk.off();
 }

 void Application::startSemaphoreApp() {
    kout.clear();
    Semaphore *s = new Semaphore(1);

    unsigned int *stack = new unsigned int[1024];
    Loop *loop = new Loop(0, &stack[1024], s);
    scheduler.ready(*loop);

    unsigned int *stack2 = new unsigned int[1024];
    Loop *loop2 = new Loop(1, &stack2[1024], s);
    scheduler.ready(*loop2);

    unsigned int *stack3 = new unsigned int[1024];
    PlaySound *sound = new PlaySound(&stack3[1024]);
    scheduler.ready(*sound);

    while (kb.get_last_key() != '\b');
    scheduler.kill(*loop);
    scheduler.kill(*loop2);
    scheduler.kill(*sound);
    pcspk.off();
 }

 void Application::nullPointerException() {
     int *i = 0;
     *i = 1;
 }

 void Application::writeProtectionFault() {
     unsigned int *pg = pg_alloc_page();
     pg_write_protect_page(pg);
     *pg = 1;
 }

void Application::runGraphicsDemo() {
    unsigned int *stack = new unsigned int[1024];
    VBEdemo *demo = new VBEdemo(&stack[1024]);
    scheduler.ready(*demo);
    while (kb.get_last_key() != '\b');
    scheduler.kill(*demo);

}

void Application::memoryAllocationDemo() {
    kout.clear();
    mm_dump_free_list();
    int *arr1 = new int[128];
    mm_dump_free_list();
    int *arr2 = new int[128];
    mm_dump_free_list();
    delete []arr1;  
    mm_dump_free_list();
    delete []arr2;
    mm_dump_free_list();
    while (kb.get_last_key() != '\b');
 }

 void Application::showMainMenu() {
    kout.clear ();

    kout << "HHUos 1.0" << endl << "=========" << endl << endl;
    kout << "Demos:" << endl;
    kout << "[1] - Sound abspielen" << endl;
    kout << "[2] - Grafikmodus einschalten" << endl;
    kout << "[3] - Memory Allocation Demo" << endl;
    kout << "[4] - Bluescreen (Nullpointer Exception)" << endl;
    kout << "[5] - Bluescreen (Write Protection)" << endl;
    kout << "[6] - Synchronisierung mit Semaphore" << endl;
    kout << endl;
 
    kout << "Bitte Demo auswaehlen [1-6]:";
    kout.flush ();
 }

 void Application::waitForInput() {
    while (true) {
        char c = kb.get_last_key();
        switch (c) {
            case '1':
                startPlaySoundApp();
                showMainMenu();
                break;
            case '2':
                runGraphicsDemo();
                showMainMenu();
                break;
            case '3':
                memoryAllocationDemo();
                showMainMenu();
                break;
            case '4':
                nullPointerException();
                break;            
            case '5':
                writeProtectionFault();
                break;              
            case '6':
                startSemaphoreApp();
                showMainMenu();
                break;
        }
    }
 }
