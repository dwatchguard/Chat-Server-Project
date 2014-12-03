#ifndef USER_H
#define USER_H
#include <stdbool.h>
#include <stdlib.h>
 
 //struct representing a chatroom
typedef struct user
{
    char name[MAX_USERNAME_LEN];
	char Private_group[MAX_GROUP_NAME];
} user;
 
#endif  