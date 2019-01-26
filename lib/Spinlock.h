class Spinlock {

    private:
    Spinlock(const Spinlock &copy); // Verhindere Kopieren
    unsigned int lock;

    public:
    Spinlock(): lock(0) {}
    void acquire();
    void free();
};