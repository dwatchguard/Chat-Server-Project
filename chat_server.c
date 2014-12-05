/*
 * The Spread Toolkit.
 *     
 * The contents of this file are subject to the Spread Open-Source
 * License, Version 1.0 (the ``License''); you may not use
 * this file except in compliance with the License.  You may obtain a
 * copy of the License at:
 *
 * http://www.spread.org/license/
 *
 * or in the file ``license.txt'' found in this distribution.
 *
 * Software distributed under the License is distributed on an AS IS basis, 
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
 * for the specific language governing rights and limitations under the 
 * License.
 *
 * The Creators of Spread are:
 *  Yair Amir, Michal Miskin-Amir, Jonathan Stanton, John Schultz.
 *
 *  Copyright (C) 1993-2009 Spread Concepts LLC <info@spreadconcepts.com>
 *
 *  All Rights Reserved.
 *
 * Major Contributor(s):
 * ---------------
 *    Ryan Caudy           rcaudy@gmail.com - contributions to process groups.
 *    Claudiu Danilov      claudiu@acm.org - scalable wide area support.
 *    Cristina Nita-Rotaru crisn@cs.purdue.edu - group communication security.
 *    Theo Schlossnagle    jesus@omniti.com - Perl, autoconf, old skiplist.
 *    Dan Schoenblum       dansch@cnds.jhu.edu - Java interface.
 *
 */



#include "include/sp.h"
#include "packet.h"
#include "net_include.h"
#include "lamp_stamp.c"
#include "linked_list.c"
#include "chatroom.c"
#include "message.c"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define int32u unsigned int

//#include "test.c"//This is temporary, but this will include all of our data structure functionality

static	char	User[80];
static  char    Spread_name[80];

static  char    Private_group[MAX_GROUP_NAME];
static  mailbox Mbox;
static	int	    Num_sent;
//static	unsigned int	Previous_len;

static  int     To_exit = 0;

#define MAX_MESSLEN     102400
#define MAX_VSSETS      10
#define MAX_MEMBERS     100

static  int     machine_num;
static  int     update_num = 0;
static  int     lamp_counter = 0;
static llist	*chatrooms;
static llist	*users;	


static  char    connected_servers[MAX_MEMBERS][MAX_GROUP_NAME];
static  char    *servers_group = "servers";
static  int     num_connected_servers;

static	void	Read_message();
static	void	Usage( int argc, char *argv[] );
static  void	Bye();
static  chatroom* get_chatroom(char chat_name[]);

int main( int argc, char *argv[] )
{
	    int	    ret;
        int     mver, miver, pver;
        sp_time test_timeout;

        test_timeout.sec = 5;
        test_timeout.usec = 0;

	    Usage( argc, argv );
        if (!SP_version( &mver, &miver, &pver)) 
        {
	          printf("main: Illegal variables passed to SP_version()\n");
	          Bye();
	    }
	    printf("Spread library version is %d.%d.%d\n", mver, miver, pver);

	    ret = SP_connect_timeout( Spread_name, User, 0, 1, &Mbox, Private_group, test_timeout );
	    if( ret != ACCEPT_SESSION ) 
	    {
		    SP_error( ret );
		    Bye();
	    }
	    printf("User: connected to %s with private group %s\n", Spread_name, Private_group );

        machine_num = atoi(argv[1]);
        ret = SP_join( Mbox, argv[1] );
        if( ret < 0 ) SP_error( ret );
        ret = SP_join( Mbox, servers_group );
        if( ret < 0 ) SP_error( ret );
		
		chatrooms = initlist();//allocate our chat rooms
		users = initlist();
		
	    E_init();

	    E_attach_fd( Mbox, READ_FD, Read_message, 0, NULL, HIGH_PRIORITY );

	    printf("\nUser> ");
	    fflush(stdout);

	    Num_sent = 0;

	    E_handle_events();

	    return( 0 );
}


/* FIXME: The user.c code does not use memcpy()s to avoid bus errors when
 *        dereferencing a pointer into a potentially misaligned buffer */

static	void	Read_message()
{

static	char		 mess[MAX_MESSLEN];
        char		 sender[MAX_GROUP_NAME];
        char		 target_groups[MAX_MEMBERS][MAX_GROUP_NAME];
        membership_info  memb_info;
        vs_set_info      vssets[MAX_VSSETS];
        unsigned int     my_vsset_index;
        int              num_vs_sets;
        char             members[MAX_MEMBERS][MAX_GROUP_NAME];
        int		 num_groups;
        int		 service_type;
        int16		 mess_type;
        int		 endian_mismatch;
        int		 i,j;
        int		 ret;
        
        char        send_mess[MAX_MESSLEN] = "";

        service_type = 0;

	ret = SP_receive( Mbox, &service_type, sender, 100, &num_groups, target_groups, 
		&mess_type, &endian_mismatch, sizeof(mess), mess );
	printf("\n============================\n");
	if( ret < 0 ) 
	{
                if ( (ret == GROUPS_TOO_SHORT) || (ret == BUFFER_TOO_SHORT) ) {
                        service_type = DROP_RECV;
                        printf("\n========Buffers or Groups too Short=======\n");
                        ret = SP_receive( Mbox, &service_type, sender, MAX_MEMBERS, &num_groups, target_groups, 
                                          &mess_type, &endian_mismatch, sizeof(mess), mess );
                }
        }
        if (ret < 0 )
        {
		if( ! To_exit )
		{
			SP_error( ret );
			printf("\n============================\n");
			printf("\nBye.\n");
		}
		exit( 0 );
	}
	if( Is_regular_mess( service_type ) )
	{
		mess[ret] = 0;
		if     ( Is_unreliable_mess( service_type ) ) printf("received UNRELIABLE ");
		else if( Is_reliable_mess(   service_type ) ) printf("received RELIABLE ");
		else if( Is_fifo_mess(       service_type ) ) printf("received FIFO ");
		else if( Is_causal_mess(     service_type ) ) printf("received CAUSAL ");
		else if( Is_agreed_mess(     service_type ) ) printf("received AGREED ");
		else if( Is_safe_mess(       service_type ) ) printf("received SAFE ");
		printf("message from %s, of type %d, (endian %d) to %d groups \n(%d bytes): %s\n",
			sender, mess_type, endian_mismatch, num_groups, ret, mess );
//********************************************************************************************
	    int *packet_type = (int *) mess;
	    if (*packet_type == CLIENT_PACKET) {
	        client_packet *pack = (client_packet *) mess;
	        printf("%c\n", pack->type);
	        printf("%s\n", pack->data);
	        
           /* typedef struct packet
            {
                int packet_type;
                int machine_num;
                int update_num;
                time_t timestamp;
                char payload[PAYLOAD];
            } packet;*/
            
           // packet new_pack;
            //new_pack.machine_num = machine_num;
            lamp_stamp ls; 
			chatroom *room = get_chatroom(pack->room_name);
			user temp_user;
			memcpy(temp_user.name, pack->username, MAX_USERNAME_LEN);
			memcpy(temp_user.Private_group, sender, MAX_GROUP_NAME);
			user *new_user;
			packet send_packet;
			int temp_int;
			message temp_message;
			like_payload temp_like_payload;
			
	        switch( pack->type )
	        {
	            case 'u': //TODO: send to the other servers
				if (room == NULL) {
					list_remove(users, &temp_user, sizeof(user));
					user * new_user = malloc(sizeof(user));
					memcpy(new_user->name, pack->new_name, MAX_USERNAME_LEN);
					memcpy(new_user->Private_group, sender, MAX_GROUP_NAME);
					add_to_end(users, &new_user, sizeof(user));
				} else {
					remove_local_user(room, &temp_user);
					user * new_user = malloc(sizeof(user));
					memcpy(new_user->name, pack->new_name, MAX_USERNAME_LEN);
					memcpy(new_user->Private_group, sender, MAX_GROUP_NAME);
					add_local_user(room, new_user);
				}
	                break;
	            case 'c':
					new_user = malloc(sizeof(user));
					memcpy(new_user, &temp_user, sizeof(user));
					add_to_end(users, &new_user, sizeof(user));
	                break;
	            case 'j'://join a chatroom
					if (room == NULL) {
						list_remove(users, &temp_user, sizeof(user));					
					} else {
						remove_local_user(room, &temp_user);
					}
					chatroom * new_room = get_chatroom(pack->new_room);
					if (room == NULL) {
						new_room = create_room(pack->new_room);
						add_to_end(chatrooms, new_room, sizeof(chatroom));
					}
					new_user = malloc(sizeof(user));
					memcpy(new_user, &temp_user, sizeof(user));
					add_local_user(new_room, new_user);					
	                break;
	            case 'a'://append a message
					send_packet.machine_num = machine_num;
                    ls.machine_num = machine_num; ls.counter = ++lamp_counter;
                    send_packet.timestamp = ls;
					//send_packet.timestamp = time(NULL);
					send_packet.update_num = 10*(update_num++) + machine_num;//guaranteed to be unique
					send_packet.packet_type = APPEND_COMMAND;
					message_payload temp_payload;
					memcpy(temp_payload.username, pack->username, MAX_USERNAME_LEN);
					memcpy(temp_payload.room_name, pack->room_name,MAX_ROOM_NAME_LEN);
					memcpy(temp_payload.message, pack->data,MAX_MESS_LEN);
					if (room != NULL) {
						ret= SP_multicast( Mbox, SAFE_MESS, servers_group, 1, sizeof(send_mess), send_mess );
						if( ret < 0 ) 
						{
							SP_error( ret );
							Bye();
						}
					}
	                break;
	            case 'l'://like a message
					send_packet.machine_num = machine_num;
					ls.machine_num = machine_num; ls.counter = ++lamp_counter;
					send_packet.timestamp = ls;
					send_packet.update_num = 10*(update_num++) + machine_num;
					send_packet.packet_type = LIKE_COMMAND;
					
					temp_int = (int)*pack->data;
					if (room->messages->size > 25) {//find the index of the message to like
						temp_int = temp_int -26+ room->messages->size;
					} else {
						temp_int--;
					}
					memcpy(&temp_message, get_element_at(room->messages, temp_int), sizeof(message)); //get the message to like 
					temp_like_payload.line_number = temp_message.update_num; //its update num is the one we send
					
					memcpy(temp_like_payload.username, pack->username, MAX_USERNAME_LEN);
					memcpy(temp_like_payload.room_name, pack->room_name,MAX_ROOM_NAME_LEN);
					memcpy(send_packet.payload, &temp_like_payload, sizeof(temp_like_payload));
					if (room != NULL) {
						ret= SP_multicast( Mbox, SAFE_MESS, servers_group, 1, sizeof(send_packet), (char*)&send_packet );
						if( ret < 0 ) 
						{
							SP_error( ret );
							Bye();
						}
					}
	                break;
	            case 'r'://unlike a message
					send_packet.machine_num = machine_num;
					ls.machine_num = machine_num; ls.counter = ++lamp_counter;
					send_packet.timestamp = ls;
					send_packet.update_num = 10*(update_num++) + machine_num;
					send_packet.packet_type = UNLIKE_COMMAND;
					
					temp_int = (int)*pack->data;
					if (room->messages->size > 25) {//find the index of the message to like
						temp_int = temp_int -26+ room->messages->size;
					} else {
						temp_int--;
					}
					memcpy(&temp_message, get_element_at(room->messages, temp_int), sizeof(message)); //get the message to like 
					temp_like_payload.line_number = temp_message.update_num; //its update num is the one we send
					
					memcpy(temp_like_payload.username, pack->username, MAX_USERNAME_LEN);
					memcpy(temp_like_payload.room_name, pack->room_name, MAX_ROOM_NAME_LEN);
					memcpy(send_packet.payload, &temp_like_payload, sizeof(temp_like_payload));
					if (room != NULL) {
						ret= SP_multicast( Mbox, SAFE_MESS, servers_group, 1, sizeof(send_packet), (char*)&send_packet );
						if( ret < 0 ) 
						{
							SP_error( ret );
							Bye();
						}
					}
	                break;
	            case 'h'://get the entire history
	                break;
	            case 'v'://get the view
	                strcat(send_mess, "\n");
                    for( i=0; i < num_connected_servers; i++ ) {
                        strcat(send_mess, "\t");
                        strcat(send_mess, &connected_servers[i][0]);
                        strcat(send_mess, "\n");
                    }
                    ret= SP_multicast( Mbox, SAFE_MESS, sender, 1, sizeof(send_mess), send_mess );
                    if( ret < 0 ) 
                    {
                        SP_error( ret );
                        Bye();
                    }
                        //printf("\t%s\n", &connected_servers[i][0] );
	                break;
	        }
	    }
/////////////////////////////////////////////////////////////////////////////////////////////////
	}else if( Is_membership_mess( service_type ) )
        {
                ret = SP_get_memb_info( mess, service_type, &memb_info );
                if (ret < 0) {
                        printf("BUG: membership message does not have valid body\n");
                        SP_error( ret );
                        exit( 1 );
                }
//*********************************************************
            if (strcmp(sender, servers_group) == 0) {
                memcpy(connected_servers, target_groups, sizeof(target_groups));
                num_connected_servers = num_groups;
            }
//////////////////////////////////////////////////////////
		if     ( Is_reg_memb_mess( service_type ) )
		{
			printf("Received REGULAR membership for group %s with %d members, where I am member %d:\n",
				sender, num_groups, mess_type );
			for( i=0; i < num_groups; i++ )
				printf("\t%s\n", &target_groups[i][0] );
			printf("grp id is %d %d %d\n",memb_info.gid.id[0], memb_info.gid.id[1], memb_info.gid.id[2] );

			if( Is_caused_join_mess( service_type ) )
			{
				printf("Due to the JOIN of %s\n", memb_info.changed_member );
			}else if( Is_caused_leave_mess( service_type ) ){
				printf("Due to the LEAVE of %s\n", memb_info.changed_member );
			}else if( Is_caused_disconnect_mess( service_type ) ){
				printf("Due to the DISCONNECT of %s\n", memb_info.changed_member );
			}else if( Is_caused_network_mess( service_type ) ){
				printf("Due to NETWORK change with %u VS sets\n", memb_info.num_vs_sets);
                                num_vs_sets = SP_get_vs_sets_info( mess, &vssets[0], MAX_VSSETS, &my_vsset_index );
                                if (num_vs_sets < 0) {
                                        printf("BUG: membership message has more then %d vs sets. Recompile with larger MAX_VSSETS\n", MAX_VSSETS);
                                        SP_error( num_vs_sets );
                                        exit( 1 );
                                }
                                for( i = 0; i < num_vs_sets; i++ )
                                {
                                        printf("%s VS set %d has %u members:\n",
                                               (i  == my_vsset_index) ?
                                               ("LOCAL") : ("OTHER"), i, vssets[i].num_members );
                                        ret = SP_get_vs_set_members(mess, &vssets[i], members, MAX_MEMBERS);
                                        if (ret < 0) {
                                                printf("VS Set has more then %d members. Recompile with larger MAX_MEMBERS\n", MAX_MEMBERS);
                                                SP_error( ret );
                                                exit( 1 );
                                        }
                                        for( j = 0; j < vssets[i].num_members; j++ )
                                                printf("\t%s\n", members[j] );
                                }
			}
		}else if( Is_transition_mess(   service_type ) ) {
			printf("received TRANSITIONAL membership for group %s\n", sender );
		}else if( Is_caused_leave_mess( service_type ) ){
			printf("received membership message that left group %s\n", sender );
		}else printf("received incorrecty membership message of type 0x%x\n", service_type );
        } else if ( Is_reject_mess( service_type ) )
        {
		printf("REJECTED message from %s, of servicetype 0x%x messtype %d, (endian %d) to %d groups \n(%d bytes): %s\n",
			sender, service_type, mess_type, endian_mismatch, num_groups, ret, mess );
	}else printf("received message of unknown message type 0x%x with ret %d\n", service_type, ret);


	printf("\n");
	fflush(stdout);
}

static	void	Usage(int argc, char *argv[])
{
	sprintf( User, "server" );
	sprintf( Spread_name, "10070");

	if (argc != 2)
	{
	    printf("Invalid number of arguments (need 1 have %d)\n", argc - 1);
	    exit(0);
	}
	strcpy( User, argv[1] );
}
static  void	Bye()
{
	To_exit = 1;

	printf("\nBye.\n");

	SP_disconnect( Mbox );

	exit( 0 );
}
static chatroom* get_chatroom(char chat_name[]) {
	node * temp = chatrooms->head;
	while(temp != NULL) {
		chatroom *temp_room = temp->ptr;
		if (memcmp(temp_room->name, chat_name,MAX_ROOM_NAME_LEN) == 0) {
			return temp_room;
		}
		temp = temp->next;
	}
	return NULL;
}
