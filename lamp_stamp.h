#ifndef LAMP_STAMP_H
#define LAMP_STAMP_H

typedef struct lamp_stamp 
{
    int machine_num;
    int counter;
} lamp_stamp;

int compare_stamp(lamp_stamp *ls1, lamp_stamp *ls2);
lamp_stamp max_stamp(lamp_stamp ls1, lamp_stamp ls2);

#endif