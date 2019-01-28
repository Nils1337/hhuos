#ifndef __Spinlock_include__
#define __Spinlock_include__

class Spinlock {

    private:
    Spinlock(const Spinlock &copy); // Verhindere Kopieren
    unsigned int lock;

    public:
    Spinlock(): lock(0) {}
    void acquire();
    void free();
};

#endif