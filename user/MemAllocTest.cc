#include "kernel/Globals.h"
#include "user/MemAllocTest.h"
#include "kernel/MemMgmt.h"

void MemAllocTest::run () {
    mm_dump_free_list();

    int *arr1 = new int[128];

    mm_dump_free_list();

    int *arr2 = new int[128];

    mm_dump_free_list();

    delete []arr1;
    
    mm_dump_free_list();

    delete []arr2;

    mm_dump_free_list();
}