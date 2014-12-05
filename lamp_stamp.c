#include "lamp_stamp.h"

int compare_stamp(lamp_stamp *ls1, lamp_stamp *ls2) {
    if (ls1->counter > ls2->counter) {
        return 1;
    }
    else if (ls2->counter > ls1->counter) {
        return -1;
    }
    else if (ls1->machine_num > ls2->machine_num) {
        return 1;
    }
    else {
        return -1;
    }
}

lamp_stamp max_stamp(lamp_stamp ls1, lamp_stamp ls2) {
    if (compare_stamp(&ls1, &ls2) < 0) {
        return ls2;
    }
    else {
        return ls1;
    }
}
