#include "chatroom.h"
#include "net_include.h"

/**
*	Creates a chat room with the given name. handles memory allocation.
*/
chatroom* create_room(char room_name[]) {
	chatroom* temp_room = malloc(sizeof(chatroom));
	memcpy(temp_room->name,room_name, MAX_ROOM_NAME_LEN);
	for (int i = 0; i < NUM_MACHINES; i++) {
		temp_room->users[i]		= initlist();
	}

	temp_room->local_users 	= initlist();
	temp_room->messages 	= initlist();
	temp_room->pending_likes = initlist();
	temp_room->pending_unlikes = initlist();
	return temp_room;
}
/**
*	Adds a user to the given chatroom
*/
void add_user(chatroom *chat,char* username, int machine_num) {
	add_to_end(chat->users[machine_num], username, sizeof(MAX_USERNAME_LEN));
}
/**
*	Adds a new local user to the chatroom
*/
void add_local_user(chatroom *chat, user *new_user) {
	//add_to_end(chat->users, new_user->name, sizeof(MAX_USERNAME_LEN));
	add_to_end(chat->local_users, new_user, sizeof(user));
}
void remove_user(chatroom *chat,char *username, int machine_num) {
	list_remove(chat->users[machine_num], username, sizeof(MAX_USERNAME_LEN));
}
void remove_local_user(chatroom *chat,user *leaver) {
	//list_remove(chat->users[ma], leaver->name, sizeof(MAX_USERNAME_LEN));
	list_remove(chat->local_users, leaver, sizeof(user));
}
 
llist* get_all_usernames(chatroom *chat) {
	llist * usernames = initlist();
	node *temp;
	for (int i = 0; i < NUM_MACHINES; i++) {
	temp = chat->users[i]->head;
		while (temp != NULL)
		{
			if (!list_has(usernames, temp->ptr, MAX_USERNAME_LEN)) //todo make sure this works
			{
				add_to_end(usernames, temp->ptr, MAX_USERNAME_LEN);
			}
			temp = temp->next;
		}
	}
    return usernames;
}
void add_message(chatroom * chat, packet command) {
	if (command.packet_type != APPEND_COMMAND) {
		return;
	}
	//message_payload data;
	//command(&data, command.payload, sizeof(data));
	message *new_message = init_message(command.username, command.message, command.update_num, command.timestamp);
	add_to_end(chat->messages, new_message, MAX_MESS_LEN);	
	add_pending_likes(chat);
	sort_messages(chat);
}
int like_message_at(chatroom *chat, packet command) {

	//like_payload data;
	//command(&data, command.payload, sizeof(data));
	message *mess = get_message(chat, command.line_number);
    if (mess == NULL) {
        add_to_end(chat->pending_likes, &command, sizeof(command));
        return 0;
    }
    else {
	    like_message(mess,command);
	    return 1;
	}
	
}
int unlike_message_at(chatroom *chat, packet command) {
	//like_payload data;
	//command(&data, command.payload, sizeof(data));
    message *mess = get_message(chat, command.line_number);
    if (mess == NULL) {
        add_to_end(chat->pending_unlikes, &command, sizeof(command));
        return 0;
    }
    else {
	    unlike_message(mess,command);
	    return 1;
	}
}
char* get_history(chatroom *chat, int *servers_connected, char *buff) {
    strcat(buff, "");
    strcat(buff, "Room: ");
    strcat(buff, chat->name);
    strcat(buff, "\n");
    strcat(buff, "Attendees: ");
    
    int first = 1;
    node *user_node;
    user *u;
    for (int i = 0; i < NUM_MACHINES; i++) {
        if (servers_connected[i] == 1) {
            user_node = chat->users[i]->head;
            for (int j = 0; j < chat->users[i]->size; j++) {
                u = (user *) user_node->ptr;
                if (first == 0) {
                    strcat(buff, ", ");
                }
                strcat(buff, u->name);
                user_node = user_node->next;
            }
        }
    }
    strcat(buff, ".\n");
    char str[15];
    char line_str[MAX_LINE_LENGTH];
    int num_messages = RECENT_NUM;
    node *message_node;
    message *mess;
    if (chat->messages->size > RECENT_NUM) {
        message_node = chat->messages->tail;
        for (int i = 0; i < RECENT_NUM; i++) {
            message_node = message_node->prev;
        }
    }
    else {
        message_node = chat->messages->head;
    }
    if (chat->messages->size < RECENT_NUM) {
        num_messages = chat->messages->size;
    }
    for (int i = 0; i < num_messages; i++) {
        mess = (message *) message_node->ptr;
        sprintf(str, "%d", i);
        strcat(line_str, str);
        strcat(line_str, ". ");
        strcat(line_str, mess->poster_name);
        strcat(line_str, ": ");
        strcat(line_str, mess->text);
        for (int j = 0; j < 80 - strlen(line_str); j++) {
            strcat(line_str, " ");
        }
        if (mess->num_likes > 0) {
            strcat(line_str, "Likes: ");
            sprintf(str, "%d", mess->num_likes);
            strcat(line_str, str);
        }
        strcat(buff, line_str);
    }
    return buff;
}

char* get_entire_history(chatroom *chat) {
return NULL;
}

message* get_message(chatroom* chat, int message_number) {
	node *temp =  chat->messages->head;
	for (int i = 0; i < chat->messages->size; i++) {
		message* temp_mess = (message*)temp->ptr;
		if (temp_mess->update_num == message_number) { 
			return temp_mess;	
		}	
	}
	return NULL;
}

void add_pending_likes(chatroom *chat) {
    node *temp = chat->pending_likes->head;
    for (int i = 0; i < chat->pending_likes->size; i++) {
        packet *temp_pack = (packet *) temp->ptr;
        if (like_message_at(chat, *temp_pack) == 1) {
            list_remove(chat->pending_likes, temp_pack, sizeof(packet));
        }
        temp = temp->next;
    }
    temp = chat->pending_unlikes->head;
    for (int i = 0; i < chat->pending_unlikes->size; i++) {
        packet *temp_pack = (packet *) temp->ptr;
        if (unlike_message_at(chat, *temp_pack) == 1) {
            list_remove(chat->pending_unlikes, temp_pack, sizeof(packet));
        }
        temp = temp->next;
    }
}

void sort_messages(chatroom *chat) {
    node *temp = chat->messages->tail;
    node *prev;
    int size = chat->messages->size;
    int swapped = 0;
    while (swapped == 0) {
        for (int i = 0; i < size - 1; i++) {
            prev = temp->prev;
            message *temp_mess = (message *) temp->ptr;
            message *temp_prev = (message *) prev->ptr;
            if (compare_stamp(&temp_mess->timestamp, &temp_prev->timestamp) < 0) {
                node *temptemp = temp;
                temp = prev;
                prev = temptemp;
                swapped = 1;
            }
            temp = prev;
        }
    }
}
