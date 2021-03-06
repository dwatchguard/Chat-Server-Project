#ifndef MESSAGE_H
#define MESSAGE_H
#include <stdbool.h>
#include <stdlib.h>
#include "net_include.h"
#include "packet.h"
#include "lamp_stamp.h"
#include <time.h>
 
typedef struct message
{
	char poster_name[MAX_USERNAME_LEN];
    char text[MAX_MESS_LEN];
	llist *likes;
	llist *unlikes;
	int num_likes;
	int update_num;
	lamp_stamp timestamp;
} message;

typedef struct like 
{
	char username[MAX_USERNAME_LEN];
	lamp_stamp timestamp;
} like;

message *init_message(char *poster, char *line, int number, lamp_stamp timestamp);
void like_message(message* line, packet command);
void unlike_message(message* line, packet command);
like *get_like(message* line, char* name);
like *get_unlike(message* line, char* name);
 
#endif 