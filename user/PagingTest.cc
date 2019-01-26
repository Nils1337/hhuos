#include "kernel/Globals.h"
#include "user/PagingTest.h"
#include "kernel/Paging.h"

void PagingTest::run () {
    unsigned int *pg = pg_alloc_page();
    //pg_write_protect_page(pg);
    *pg = 1;
}