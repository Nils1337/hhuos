#include "lib/Queue.h"


int Queue::count() {
    int size = 0;
    Chain *cur = head;
    while (cur != 0) {
        cur = cur->next;
        size++;
    }

    return size;
}


void Queue::enqueue(Chain* item) {
    if (tail == 0) {
        head = item;
        tail = item;
        item->next = 0;
    } else {
        tail->next = item;
        item->next = 0;
        tail = item;
    }
}


Chain* Queue::dequeue() {
    Chain* deq = head;
    if (deq == 0) {
        return 0;
    }

    if (deq->next == 0) {
        head = 0;
        tail = 0;
    } else {
        head = deq->next;
    }

    return deq;
}

void Queue::remove (Chain* rem) {
    Chain* cur = head;

    if (cur == 0) {
        return;
    }

    if (cur == rem) {
        dequeue();
    }

    while (cur->next != 0 && cur->next != rem) {
        cur = cur->next;
    }

    if (cur->next != 0) {
        cur->next = cur->next->next;
                    
        if (cur->next == 0) {
            tail = cur;
        }
    }
}
