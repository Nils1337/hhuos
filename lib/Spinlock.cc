 #include "lib/Spinlock.h"
#include "lib/CAS.h"

void Spinlock::acquire()
{
    while (CAS(&lock, 0, 1) == 0) ;
}

void Spinlock::free()
{
    lock = 0;
}