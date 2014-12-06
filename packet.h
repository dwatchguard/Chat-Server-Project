#ifndef PACKET_H
#define PACKET_H
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "net_include.h"
#include "lamp_stamp.h"

typedef struct packet
{
	int packet_type;
    int machine_num;
	int update_num;
	lamp_stamp timestamp;
    //char payload[PAYLOAD];
	int line_number;
	char username[MAX_USERNAME_LEN];
	char room_name[MAX_ROOM_NAME_LEN];
	char message[MAX_MESS_LEN];
} packet;


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
	char new_room[MAX_ROOM_NAME_LEN];//Only used for a join.
} client_packet;

typedef struct memb_packet
{
    int packet_type;
    char connected_servers[MAX_MEMBERS][MAX_GROUP_NAME];
    int num_connected_servers;
} memb_packet;


#endif 