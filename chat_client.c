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



#include "sp.h"
#include "net_include.h"
#include "packet.h"
#include "chatroom.c"
#include "linked_list.c"
#include "lamp_stamp.c"
#include "message.c"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define int32u unsigned int

static	char	User[80];
static  char    Spread_name[80];

static  char    Private_group[MAX_GROUP_NAME];
static  mailbox Mbox;
static	int	    Num_sent;
//static	unsigned int	Previous_len;

static  int     To_exit = 0;

#define MAX_VSSETS      10

static  char    username[MAX_USERNAME_LEN] = "";
static  char    server[MAX_SERVER_NAME_LEN] = "";
static  char    room_name[MAX_ROOM_NAME_LEN] = "";
static  chatroom *room;

static  char    connected_servers[MAX_MEMBERS][MAX_GROUP_NAME];
static  int     num_connected_servers;
static  int     servers_connected[NUM_MACHINES];

static	void	Print_menu();
static	void	User_command();
static	void	Read_message();
static	void	Usage( int argc, char *argv[] );
static  void	Bye();

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

        int unum = 1;
        int newUser = 0;
        char unumS[15];
        while (newUser == 0) {
            strcpy( User, "user");
            sprintf(unumS, "%d", unum);
            strcat( User, unumS);
	        ret = SP_connect_timeout( Spread_name, User, 0, 1, &Mbox, Private_group, test_timeout );
	        if( ret != ACCEPT_SESSION ) 
	        {
	            if (ret == -6) {
	                unum++;
	            }
	            else {
		            SP_error( ret );
		            Bye();
		        }
	        }
	        else {
	            newUser = 1;
	        }
	    }
	    printf("User: connected to %s with private group %s\n", Spread_name, Private_group );

	    E_init();

	    E_attach_fd( 0, READ_FD, User_command, 0, NULL, LOW_PRIORITY );

	    E_attach_fd( Mbox, READ_FD, Read_message, 0, NULL, HIGH_PRIORITY );

	    Print_menu();

	    printf("\nUser> ");
	    fflush(stdout);

	    Num_sent = 0;

	    E_handle_events();

	    return( 0 );
}

static	void	User_command()
{
	char	command[130];
	//char	mess[MAX_MESSLEN];
	char	group[80];
	unsigned int	mess_len;
	int	ret;
	int	i;
	char    buff[MAX_MESSLEN] = "";
	
	client_packet   pack;
	pack.packet_type = CLIENT_PACKET;
	strcpy(pack.username, username);
    strcpy(pack.room_name, room_name);

	for( i=0; i < sizeof(command); i++ ) command[i] = 0;
	if( fgets( command, 130, stdin ) == NULL ) 
            Bye();

	switch( command[0] )
	{
	    case 'u':
	        if (strlen(command) < 3) {
	            printf("Please enter a command with two arguments\n");
	            break;
	        }
			ret = sscanf( &command[2], "%s", username );
			if (strlen(server) != 0) {
			    pack.type = 'u';
			    strcpy(pack.data, username);
                ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack), (char *) &pack );
                if( ret < 0 ) 
                {
                    SP_error( ret );
                    Bye();
                }
			}
			break;
	    case 'c':
	        if (strlen(command) < 2) {
	            printf("Please enter a command with two arguments\n");
	            break;
	        }
	        if (strlen(username) == 0) {
	            printf("Select a username before connecting to a server.\n");
	            break;
	        }
			ret = sscanf( &command[2], "%s", server );
			pack.type = 'c';
			strcpy(pack.data, username);
			ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack), (char *) &pack );
			if( ret < 0 ) 
			{
				SP_error( ret );
				Bye();
			}
			break;
		case 'j':
	        if (strlen(command) < 3) {
	            printf("Please enter a command with two arguments\n");
	            break;
	        }
	        if (strcmp(room_name, "") != 0) {
                client_packet pack2;
	            pack2.packet_type = CLIENT_PACKET;
	            strcpy(pack2.username, username);
                strcpy(pack2.room_name, room_name);
                pack2.type = 'l';
                ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack2), (char *) &pack2 );
                if( ret < 0 ) 
                {
                    SP_error( ret );
                    Bye();
                }
	        }
			pack.type = 'j';
			sscanf( &command[2], "%s", pack.data );
			if (strcmp(room_name, pack.data) != 0) {
                ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack), (char *) &pack );
                if( ret < 0 ) 
                {
                    SP_error( ret );
                    Bye();
                }
                sscanf( &command[2], "%s", room_name);
                room = create_room(room_name);
                //add_user(room, username); we should add ourselves to the chat through the message we receive from the server
            }
		    break;
		case 'a':
			printf("enter message: ");
            mess_len = 0;
            if (fgets(&group[mess_len], 80, stdin) == NULL)
			    Bye();
            if (group[mess_len] == '\n')
                break;
            mess_len += strlen( &group[mess_len] );
			pack.type = 'a';
			strcpy(pack.data, group);
			ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack), (char *) &pack );
			if( ret < 0 ) 
			{
				SP_error( ret );
				Bye();
			}
		    break;
		case 'l':
	        if (strlen(command) < 3) {
	            printf("Please enter a command with two arguments\n");
	            break;
	        }
			pack.type = 'l';
			sscanf( &command[2], "%s", pack.data );
			ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack), (char *) &pack );
			if( ret < 0 ) 
			{
				SP_error( ret );
				Bye();
			}
			strcpy(room_name, "");
		    break;
		case 'r':
	        if (strlen(command) < 3) {
	            printf("Please enter a command with two arguments\n");
	            break;
	        }
            pack.type = 'r';
			sscanf( &command[2], "%s", pack.data );
			ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack), (char *) &pack );
			if( ret < 0 ) 
			{
				SP_error( ret );
				Bye();
			}
		    break;
		case 'h':
			//pack.type = 'h';
			//ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack), (char *) &pack );
			//if( ret < 0 ) 
			//{
			//	SP_error( ret );
			//	Bye();
			//}
			get_entire_history(room);
		    break;
		case 'v':
			//pack.type = 'v';
			//ret= SP_multicast( Mbox, SAFE_MESS, server, 1, sizeof(pack), (char *) &pack );
			//if( ret < 0 ) 
			//{
			//	SP_error( ret );
			//	Bye();
			//}
            strcat(buff, "\n");
            for( i=0; i < num_connected_servers; i++ ) {
                strcat(buff, "\t");
                strcat(buff, &connected_servers[i][0]);
                strcat(buff, "\n");
            }
            printf("%s", buff);
		    break;

		default:
			printf("\nUnknown commnad\n");
			Print_menu();

			break;
	}
	printf("\nUser> ");
	fflush(stdout);

}

static	void	Print_menu()
{//ucjalrhv
	printf("\n");
	printf("==========\n");
	printf("User Menu:\n");
	printf("----------\n");
	printf("\n");
	printf("\tu <username> -- login with or change user name\n");
	printf("\tc <server> -- connect to a chat server\n");
	printf("\tj <room> -- join a chat\n");
    printf("\ta -- append to a chat\n");
	printf("\tl <line number> -- mark line as 'liked'\n");
	printf("\tr <line number> -- remove your 'like' from a line\n");
	printf("\th -- print the chat's history \n");
	printf("\tv -- print the view of chat server's in the current network component\n");
	fflush(stdout);
}

/* FIXME: The user.c code does not use memcpy()s to avoid bus errors when
 *        dereferencing a pointer into a potentially misaligned buffer */

static	void	Read_message()
{

static	char		 mess[MAX_MESSLEN];
        char		 sender[MAX_GROUP_NAME];
        char		 target_groups[MAX_MEMBERS][MAX_GROUP_NAME];
        membership_info  memb_info;
//       vs_set_info      vssets[MAX_VSSETS];
//        unsigned int     my_vsset_index;
//        int              num_vs_sets;
//        char             members[MAX_MEMBERS][MAX_GROUP_NAME];
        int		 num_groups;
        int		 service_type;
        int16		 mess_type;
        int		 endian_mismatch;
        int		 ret;

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
//==================================================================================================	
	    int *packet_type = (int *) mess;
	    packet *command = (packet *) mess;
	    if (*packet_type == APPEND_COMMAND) {
	        add_message(room, *command);
	    }
	    else if (*packet_type == LIKE_COMMAND) {
	        like_message_at(room, *command);
	    }
	    else if (*packet_type == UNLIKE_COMMAND) {
	        unlike_message_at(room, *command);
	    }
	    else if (*packet_type == JOIN_COMMAND) {
	        add_user(room, command->username, command->machine_num);
	    }
	    else if (*packet_type == LEAVE_COMMAND) {
	        remove_user(room, command->username, command->machine_num);
	    }
	    else if (*packet_type == MEMB_PACKET) {
	        memb_packet *pack = (memb_packet *) mess;
	        memcpy(connected_servers, pack->connected_servers, sizeof(connected_servers));
	        num_connected_servers = pack->num_connected_servers;
	        for (int i = 0; i < NUM_MACHINES; i++) {
	            servers_connected[i] = 0;
	        }
	        for (int i = 0; i < num_connected_servers; i++) {
	            servers_connected[connected_servers[i][1] - '0'] = 1;
	        }
	    }
/////////////////////////////////////////////////////////////////////////////////////////////////////
	}else if( Is_membership_mess( service_type ) )
        {
                ret = SP_get_memb_info( mess, service_type, &memb_info );
                if (ret < 0) {
                        printf("BUG: membership message does not have valid body\n");
                        SP_error( ret );
                        exit( 1 );
                }
        }

	printf("\n");
	printf("User> ");
	fflush(stdout);
}

static	void	Usage(int argc, char *argv[])
{
	//sprintf( User, "user" );
	sprintf( Spread_name, "10070");
}
static  void	Bye()
{
	To_exit = 1;

	printf("\nBye.\n");

	SP_disconnect( Mbox );

	exit( 0 );
}
