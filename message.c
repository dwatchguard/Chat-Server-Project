#include "linked_list.h"
#include "message.h"
#include <stdio.h>
#include <string.h>

message *init_message(char *poster, char *line, int number, lamp_stamp timestamp) {
	message * temp = malloc(sizeof(message));
	memcpy(temp->poster_name, poster, MAX_USERNAME_LEN);
    memcpy(temp->text, line, MAX_MESS_LEN);
	temp->likes = malloc(sizeof(llist));
	temp->unlikes = malloc(sizeof(llist));
	temp->num_likes = 0;
	temp->update_num = number;
	temp->timestamp = timestamp;
	return temp;
}

void like_message(message* line, packet command) { //TODO: Add code to consider unlike timestamps
	if (command.packet_type == LIKE_COMMAND) {
		like_payload data;
		memcpy(&data, &command.payload, sizeof(data));
		if (memcmp(data.username, line->poster_name, MAX_USERNAME_LEN) == 0) {
			return;
		}
		like* gotten_like = get_like(line, data.username);
		like* gotten_unlike = get_unlike(line, data.username);
		if(gotten_unlike == NULL) {
			if (gotten_like == NULL) { //It has not been liked or unliked, so we like it
				like* new_like = malloc(sizeof(like));
				memcpy(new_like->username, data.username, MAX_USERNAME_LEN);
				new_like->timestamp = command.timestamp;
				add_to_end(line -> likes, new_like, sizeof(like));
				line->num_likes++;
				return;
			} else {//Set the timestamp to be the correct one.
			gotten_like->timestamp = max(gotten_like->timestamp, command.timestamp);
			return;
			}
		} else {
			if (compare_stamp(gotten_unlike->timestamp, command.timestamp) < 0) {
				remove_from_list(line->unlikes, gotten_unlike, sizeof(like));
				like* new_like = malloc(sizeof(like));
				memcpy(new_like->username, data.username, MAX_USERNAME_LEN);
				new_like->timestamp = command.timestamp;
				add_to_end(line -> likes, new_like, sizeof(like));
				line->num_likes++;
			} 		
		}
	}
}
/*
	int diff = difftime(command.timestamp, get_like(line, data.user_name).timestamp);
			if (diff >= 0) {
*/

void unlike_message(message* line, packet command) { //TODO: Add code to consider like timestamps
	if (command.packet_type == UNLIKE_COMMAND) {
		like_payload data;
		memcpy(&data, &command.payload, sizeof(data));
		if (memcmp(data.username, line->poster_name, MAX_USERNAME_LEN) == 0) {
			return;
		}
		like* gotten_like = get_like(line, data.username);
		like* gotten_unlike = get_unlike(line, data.username);
		printf("%p\n",gotten_like);
		if(gotten_like == NULL) {
			if (gotten_unlike == NULL) { //It has not been liked or unliked, so we unlike it
				like* new_unlike = malloc(sizeof(like));
				memcpy(new_unlike->username, data.username, MAX_USERNAME_LEN);
				new_unlike->timestamp = command.timestamp;
				add_to_end(line -> unlikes, new_unlike, sizeof(like));
				return;
			} else {//Set the timestamp to be the correct one.
			gotten_unlike->timestamp = max(gotten_unlike->timestamp, command.timestamp);
			return;
			}
		} else {
			if (compare_stamp(gotten_unlike->timestamp, command.timestamp) < 0) {
			printf("here");
				remove_from_list(line->likes, gotten_like, sizeof(like));
				like* new_unlike = malloc(sizeof(like));
				memcpy(new_unlike->username, data.username, MAX_USERNAME_LEN);
				new_unlike->timestamp = command.timestamp;
				add_to_end(line -> unlikes, new_unlike, sizeof(like));
				line->num_likes--;
			} 		
		}
	}
}

like *get_like(message* line, char* name)
{
	//printf("testing get\n");
	llist *list = line ->likes;
    node *temp = list->head;
    while (temp != NULL)
    {
		like* temp_like = temp->ptr;

		//printf("Name1: %s\nName2: %s\n", temp_like->username, name);
		int compare = memcmp(temp_like->username, name, MAX_USERNAME_LEN);
        if (compare == 0)
        {
			//printf("got something\n");
            return temp -> ptr;
		}
        temp = temp->next;
    }
	//printf("got nothing\n");
    return NULL;
}

like *get_unlike(message* line, char* name)
{
	//printf("testing get\n");
	llist *list = line ->unlikes;
    node *temp = list->head;
    while (temp != NULL)
    {
		like* temp_like = temp->ptr;

		//printf("Name1: %s\nName2: %s\n", temp_like->username, name);
		int compare = memcmp(temp_like->username, name, MAX_USERNAME_LEN);
        if (compare == 0)
        {
			//printf("got something\n");
            return temp -> ptr;
		}
        temp = temp->next;
    }
	//printf("got nothing\n");
    return NULL;
}