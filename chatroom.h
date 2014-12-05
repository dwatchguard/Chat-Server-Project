#ifndef CHATROOM_H
#define CHATROOM_H
#include <stdbool.h>
#include <stdlib.h>
#include "user.h"
#include "linked_list.h"
#include "net_include.h"
#include "lamp_stamp.h"
#include "message.h"
 
 //struct representing a chatroom
typedef struct chatroom
{
    char name[MAX_ROOM_NAME_LEN];
	llist *users[NUM_MACHINES];
	llist *local_users;
	llist *messages;
	llist *pending_likes;
	llist *pending_unlikes;
} chatroom;
 
chatroom* create_room(char* room_name);
void add_user(chatroom *chat,char* username, int machine_num);
void add_local_user(chatroom *chat,user *new_user);
void remove_user(chatroom *chat,char* username, int machine_num);
void remove_local_user(chatroom *chat,user *leaver);
 
llist* get_all_usernames(chatroom *chat);

void add_message(chatroom * chat, packet command);
int like_message_at(chatroom *chat, packet command);
int unlike_message_at(chatroom *chat, packet command);
char* get_history(chatroom *chat);
char* get_entire_history(chatroom *chat);
message* get_message(chatroom* chat, int message_number);
void add_pending_likes(chatroom *chat);
void sort_messages(chatroom *chat);
#endif  