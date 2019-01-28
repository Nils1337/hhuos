#include "kernel/threads/Thread.h"
#include "kernel/Globals.h"

class PlaySound : public Thread {
    
private:
    PlaySound (const PlaySound &copy); // Verhindere Kopieren

public:
    // Gib dem Anwendungsthread einen Stack.
    PlaySound (unsigned int* stack) : Thread (stack) { }

    // Thread-Startmethode
    void run () {
        pcspk.tetris();
    }

 };