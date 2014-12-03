#include "linked_list.c"
#include "message.c"
#include "chatroom.c"
/*
int main() {

	chatroom* room1 = create_room("THIS IS ROOM1");
	user* user1 = malloc(sizeof(user));	memcpy(user1->name, "Jacob Pines", 80); memcpy(user1->Private_group, "GROUP1", 80);
	user* user2 = malloc(sizeof(user)); memcpy(user2->name, "Torrey Kelly", 80); memcpy(user2->Private_group, "GROUP2", 80);
	
	packet append_packet; append_packet.packet_type = APPEND_COMMAND; append_packet.machine_num = 1; append_packet.update_num = 1; append_packet.timestamp = time(NULL);
	message_payload append_data; memcpy(append_data.username, user1->name, 80); memcpy(append_data.room_name, room1->name, 80); memcpy(append_data.message, "THIS IS A TEST MESSAGE", MAX_MESS_LEN);
	memcpy(append_packet.payload, &append_data, sizeof(append_data));
	
	packet like_packet; like_packet.packet_type = LIKE_COMMAND; like_packet.machine_num = 1; like_packet.update_num = 2; like_packet.timestamp = 20;
	like_payload test_like; test_like.line_number = 1; memcpy(test_like.username,user2->name, 80); memcpy(test_like.room_name, room1->name, 80);	
	memcpy(like_packet.payload, &test_like, sizeof(test_like));
	
	packet unlike_packet; unlike_packet.packet_type = UNLIKE_COMMAND; unlike_packet.machine_num = 1; unlike_packet.update_num = 3; unlike_packet.timestamp = 30;
	like_payload test_unlike; test_unlike.line_number = 1; memcpy(test_unlike.username,user2->name, 80); memcpy(test_unlike.room_name, room1->name, 80);	
	memcpy(unlike_packet.payload, &test_unlike, sizeof(test_unlike));
	
	printf("Adding local user\n");
	add_local_user(room1, user1);
	printf("Adding foreign user\n");
	add_user(room1, user2->name);

	printf("Size of local group %d\n", room1->local_users->size);
	printf("Size of total group %d\n", room1->users->size);
	
	printf("Adding message\n");
	add_message(room1, append_packet);
	printf("Number of Messages %d\n", room1->messages->size);
	message* temp_mess = room1->messages->head->ptr;
	printf("Poster: %s\nText: %s\nLikes: %d\nUpdate Num %d\n\n", temp_mess->poster_name, temp_mess->text, temp_mess->num_likes, temp_mess->update_num);
	printf("Torrey Likes Jacobs Message\n");
	like_message_at(room1, like_packet);
	printf("Poster: %s\nText: %s\nLikes: %d\nUpdate Num %d\n\n", temp_mess->poster_name, temp_mess->text, temp_mess->num_likes, temp_mess->update_num);
	printf("Torrey Unlikes Jacobs Message\n");
	unlike_message_at(room1, unlike_packet);
	printf("Poster: %s\nText: %s\nLikes: %d\nUpdate Num %d\n\n", temp_mess->poster_name, temp_mess->text, temp_mess->num_likes, temp_mess->update_num);
	printf("Torrey Unlikes Jacobs Message\n");
	unlike_message_at(room1, unlike_packet);
	printf("Poster: %s\nText: %s\nLikes: %d\nUpdate Num %d\n\n", temp_mess->poster_name, temp_mess->text, temp_mess->num_likes, temp_mess->update_num);
	printf("Torrey Likes Jacobs Message\n");
	like_message_at(room1, like_packet);
	printf("Poster: %s\nText: %s\nLikes: %d\nUpdate Num %d\n\n", temp_mess->poster_name, temp_mess->text, temp_mess->num_likes, temp_mess->update_num);
	printf("Torrey Likes Jacobs Message\n");
	like_message_at(room1, like_packet);
	printf("Poster: %s\nText: %s\nLikes: %d\nUpdate Num %d\n\n", temp_mess->poster_name, temp_mess->text, temp_mess->num_likes, temp_mess->update_num);
/*


	like_payload test_like;
	
	test_like.line_number = 1;
	char test_username[80] = "testing";
	char test_room_name[80] = "test_room";
	memcpy(test_like.username,test_username, 80);
	memcpy(test_like.room_name, test_room_name, 80);
	
	packet test_packet;
	
 
 	test_packet.packet_type = LIKE_COMMAND;
    test_packet.machine_num = 1;
	test_packet.update_num = 1;
	test_packet.timestamp = time(NULL);
	
    memcpy(&test_packet.payload, &test_like, sizeof(test_like));

	llist* test_list = malloc(sizeof(llist));
	
	add_to_end(test_list, user1, sizeof(user));
	add_to_end(test_list, "cool2", 5);
	add_to_end(test_list, "cool3", 5);	
	
	remove_from_list(test_list, user2, sizeof(user));
	remove_from_list(test_list, "cool3", 5);
	
	printf("test\n");
	printf(pop(test_list));
	printf(pop(test_list));
	printf(pop(test_list));
	printf("\n");

	
	
*/	
/*
return 0;
}
*/