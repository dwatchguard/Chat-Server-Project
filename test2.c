#include "linked_list.c"

int main() {
    llist list = *initlist();

    char *data = "somedata";

    add_to_end(&list, data, strlen(data));
    printf("%s\n", pop(&list));
    
    add_to_end(&list, data, strlen(data));
    remove_from_list(&list, data, strlen(data));
	printf("%p\n",list.head);
}
