#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
 
typedef struct node
{
    void *ptr;
    struct node *next;
	struct node *prev;
	int size;
} node;
 
// Struct representing a linked list
typedef struct llist
{
    node *head;
    node *tail;
	int size;
} llist;
 
llist *initlist(); 
node *initnode(void *data, int data_size);
void add_to_end(llist *list, void *data, int data_size);
void *pop(llist *list);
bool list_has(llist *list, void *data, int data_size);
void list_remove(llist *list, void *data, int data_size);
void* index_at(llist *list, int index);
int max(int a, int b);
void* get_element_at(llist * list, int element_num);
#endif  /* LINKED_LIST_H */