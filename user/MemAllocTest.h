#include "kernel/threads/Thread.h"


class MemAllocTest : public Thread {
    
private:
    MemAllocTest (const MemAllocTest &copy); // Verhindere Kopieren

public:
    // Gib dem Anwendungsthread einen Stack.
    MemAllocTest (unsigned int* stack) : Thread (stack) { }

    // Thread-Startmethode
    void run ();
};