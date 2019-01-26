#include "kernel/threads/Thread.h"


class PagingTest : public Thread {
    
private:
    PagingTest (const PagingTest &copy); // Verhindere Kopieren

public:
    // Gib dem Anwendungsthread einen Stack.
    PagingTest (unsigned int* stack) : Thread (stack) { }

    // Thread-Startmethode
    void run ();
};