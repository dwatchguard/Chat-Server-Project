#include <stdio.h>
 
#include <stdlib.h>
 
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include "include/sp.h"

#include <errno.h>
#include <stdbool.h>
#include <time.h>
 
 
/*#define PORT       5855*/
#define PORT         10070

#define MAX_MEMBERS     100
#define MAX_MESSLEN     102400

#define NUM_MACHINES 5
#define MAX_MESS_LEN    	80
#define MAX_USERNAME_LEN 	80
#define MAX_ROOM_NAME_LEN 	80
#define MAX_SERVER_NAME_LEN 80
#define PAYLOAD     1200
#define MAX_LINE_LENGTH     240
 
/*Message Types*/
#define APPEND_COMMAND 0
#define LIKE_COMMAND 1
#define UNLIKE_COMMAND 2
#define JOIN_COMMAND 3
#define LEAVE_COMMAND 4

#define CLIENT_PACKET 10
#define MEMB_PACKET 11
#define STATUS_PACKET 12
