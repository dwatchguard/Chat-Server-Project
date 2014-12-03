#ifndef PACKET_H
#define PACKET_H
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "net_include.h"

typedef struct packet
{
	int packet_type;
    int machine_num;
	int update_num;
	struct timeval timestamp;
    char payload[PAYLOAD];
} packet;

typedef struct like_payload
{
	int line_number;
	char username[MAX_USERNAME_LEN];
	char room_name[MAX_ROOM_NAME_LEN];
}like_payload;

typedef struct message_payload
{
	char username[MAX_USERNAME_LEN];
	char room_name[MAX_ROOM_NAME_LEN];
	char message[MAX_MESS_LEN];
}message_payload;

typedef struct unlike_payload {
	int line_number;
	char username[MAX_USERNAME_LEN];
	char room_name[MAX_ROOM_NAME_LEN];
} unlike_payload;

typedef struct join_payload {
	char room_name[MAX_ROOM_NAME_LEN];
	char username[MAX_USERNAME_LEN];
} join_payload;

typedef struct leave_payload {
	char room_name[MAX_ROOM_NAME_LEN];
	char username[MAX_USERNAME_LEN];
} leave_payload;

typedef struct status_packet { //used for merging
	int packet_type;
	int machine_num;
	int machine_group[NUM_MACHINES];
	int max_recvd[NUM_MACHINES];
} status_packet;

typedef struct client_packet
{
    int packet_type;
    char type;
    char data[MAX_ROOM_NAME_LEN];
    char username[MAX_USERNAME_LEN];
    char room_name[MAX_ROOM_NAME_LEN];
	char new_name[MAX_USERNAME_LEN]; //Only used to change name, otherwise it is left blank we have room for this
	char new_room[MAX_ROOM_NAME_LEN];
} client_packet;


#endif 