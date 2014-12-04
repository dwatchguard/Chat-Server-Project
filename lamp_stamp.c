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
    else if (ls2->machine_num > ls1->machine_num) {
        return -1;
    }
}