// Node for the linked list
#include "linked_list.h"

 llist* initlist() {
	llist* temp_list = malloc(sizeof(llist));
	temp_list->head = NULL;
	temp_list->tail = NULL;
	temp_list->size = 0;
	return temp_list;
 }
 
// Adds a node to the end of the linked list with the given data
void add_to_end(llist *list, void *data, int data_size)
{
    /* We can have repeats
	if (list_has(list, data)) {
        return;
    }
	*/
    node *new = initnode(data, data_size);
    new->next = NULL;
	new->prev = list->tail;
	
    if (list->head == NULL)
    {
        list->head = new;
    }
    if (list->tail != NULL)
    {
        list->tail->next = new;
        list->tail = new;
    }
    else
    {
        list->tail = new;
    }
	list->size++;
}
 
// Initializes a node with the given data
node *initnode(void *data, int data_size)
{
    node *ptr = malloc(sizeof(node));
    if (ptr == NULL)
    {
        return NULL;
    }
    else
    {
        ptr->ptr = data;
		ptr->size = data_size;
        return ptr;
    }
}
 
// Pops and returns pointer to the least recently added node to the linked list
void* pop(llist *list)
{
    void *ret = NULL;
    node *next_node = NULL;
    node *head = list->head;
 
    if (head == NULL)
    {
        return NULL;
    }
    next_node = head->next;
    if (next_node == NULL)
    {
        list->tail = NULL;
    } else {
		next_node->prev = NULL;
	}
    ret = head->ptr;
    free(head);
    list->head = next_node;
    list->size--;
    return ret;
}



//Removes the given element from the list
void remove_from_list(llist *list, void *element, int element_size) {
    node *temp = list->head;
    while (temp != NULL)
    {
		int size = max(element_size, temp->size);
        if (memcmp(temp -> ptr, element, size) == 0) //todo make sure this works
        {
            if (temp->prev != NULL) {
				temp->prev->next = temp->next;
			} else {//it must be the head
				list->head = temp->next;
			}
			if (temp->next != NULL) {
				temp->next->prev = temp->prev;
			} else {//removing the tail
				list->tail = temp->prev;
			}
			/*
			printf("%p\n", temp);
			printf("%p\n", temp->next);
			printf("%p\n", temp->prev);
			*/
			printf("here\n");
			free(temp);
			printf("still here\n");
			list->size--;
			return;
        }
        temp = temp->next;
    }
    return;
}


 
// Returns if the list has a node with the given data
bool list_has(llist *list, void *data, int data_size)
{
    node *temp = list->head;
    while (temp != NULL)
    {
		int size = max(data_size, temp->size);
        if (memcmp(temp -> ptr, data, size) == 0) //todo make sure this works
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}
//returns the nth element of the list.
void* index_at(llist *list, int index) {
	node * temp = list->head;
	for (int i = 0; i <index && temp != NULL; i++) {
	}
	return temp;
}

int max(int a, int b) {
	if (a > b)
		return a;
	return b;
}

void* get_element_at(llist * list, int element_num) {
	if ( element_num < 0 || element_num >= list->size) {
		return NULL;
	}
	node * temp = list->head;
	for (int i = 0; i < element_num; i++){
		temp = temp-> next;
	}	
	return temp->ptr;
}