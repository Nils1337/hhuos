#include "lib/Semaphore.h"
#include "kernel/Globals.h"
#include "lib/CAS.h"

void Semaphore::p() {
    lock.acquire();
    if (counter > 0) {
        counter--;
        lock.free();
    } else {
        cpu.disable_int();
        lock.free();
        waitQueue.enqueue(scheduler.active());
        scheduler.block();
    }

    // while (true) {
    //     unsigned int c = counter;
    //     if (c > 0) {
    //         if (CAS(&counter, c, c - 1)) {
    //             break;
    //         }
    //     } else {
    //         cpu.disable_int();
    //         waitQueue.enqueue(scheduler.active());
    //         scheduler.block();
    //     }
    // }
}

void Semaphore::v () {
    lock.acquire();
    if (waitQueue.count() > 0) {
        Thread *deblock = (Thread *) waitQueue.dequeue();
        scheduler.deblock(*deblock);
    } else {
        counter++;
    }
    lock.free();
}