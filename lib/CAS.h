#ifndef __CAS_include__
#define __CAS_include__

/*
    Compare & Swap
    ==============
 
    if old == *ptr then
       *ptr := _new
    return prev
 
    (function inlining improves speed by avoiding functions calls;
     gnu99 requires 'static' to be combined with 'inline')
*/
inline bool CAS(unsigned int *ptr, unsigned int old, unsigned int _new)
{
    unsigned int val;
    
    /*  
        AT&T/UNIX assembly syntax
     
        The 'volatile' keyword after 'asm' indicates that the instruction has important side-effects.
        GCC will not delete a volatile asm if it is reachable. 
     */
    asm volatile("lock;"                            /* prevent race conditions with other cores        */
                 "cmpxchg %1, %2;"                  /* q=64-bit version, %1 = _new; %2 = *ptr          */
                                                    /* constraints */
                 : "=a"(val)                       /* output: =a: RAX -> prev (%0))                             */
                 : "r"(_new), "m"(*ptr), "a"(old)   /* input = %1, %2, %3 (r=register, m=memory, a=accumlator = eax  */
                 : "memory");                       /* ensures assembly block will not be moved by gcc */
    return val == old;
}

/*
    Atomic Set (Char)
    ==============
 
    (function inlining improves speed by avoiding functions calls;
     gnu99 requires 'static' to be combined with 'inline')
*/
inline char S(char *ptr, char _new)
{
    char val;
    
    /*  
        AT&T/UNIX assembly syntax
     
        The 'volatile' keyword after 'asm' indicates that the instruction has important side-effects.
        GCC will not delete a volatile asm if it is reachable. 
     */
    asm volatile("lock;"                            /* prevent race conditions with other cores        */
                 "xchg %1, %2;"                  /* %1 = _new; %2 = *ptr          */
                                                    /* constraints */
                 : "=a"(val)                       /* output: =a: RAX -> prev (%0))                             */
                 : "r"(_new), "m"(*ptr)   /* input = %1, %2, %3 (r=register, m=memory, a=accumlator = eax  */
                 : "memory");                       /* ensures assembly block will not be moved by gcc */
    return val;
}

#endif