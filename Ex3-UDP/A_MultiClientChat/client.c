#include<stdio.h>
#include<stdlib.h>

#ifndef udp_socket
	#include "udp_socket.h"
#endif	

#ifndef msg_io
	#include "msg_io.h"
#endif

void main(){

	int self_socket = make_socket();
	if(self_socket<0){
		printf("\nCould not create socket. Retry!\n");
		return;
	}

	char *server_ip = (char*)malloc(sizeof(char)*IP_STRING_LEN);
	printf("\nEnter Echo-Server IP Address: ");
	scanf(" %s", server_ip);
	struct sockaddr_in server_addr = wrap_address(server_ip, SERVER_PORT);
	int server_addr_len = sizeof(server_addr);
	
	// Structure to store the message source address
	struct sockaddr_in *source_addr = malloc(sizeof(struct sockaddr_in));
	int source_addr_len = sizeof(struct sockaddr_in);

	char *msg_buffer = (char*)malloc(sizeof(char)*MSG_BUFFER_SIZE);
	int msg_size = 0;
	printf("\n\nDelimit Ping Messages with ';'\nEnter 'FORGETCLIENT;' to terminate connection\n");
	do {
		bzero(msg_buffer, MSG_BUFFER_SIZE);
		printf("\nEnter Ping Message: ");
		scanf(" %[^;]s", msg_buffer);
		// Consume the last newline character from read-buffer
		getchar();   
		msg_size = send_message(self_socket, msg_buffer, &server_addr, server_addr_len);
		// Reading back
		bzero(msg_buffer, MSG_BUFFER_SIZE);
		msg_size = receive_message(self_socket, msg_buffer, source_addr, &source_addr_len);
		if(strcmp(msg_buffer, SERVER_REJECT_STRING)==0){
			printf("\nServer is busy. Exiting...\n");
			break;
		}
		else if(strcmp(msg_buffer, TERMINATION_ACK_STRING)==0){
			printf("\nExiting...\n");
			break;
		}
		printf("SERVER replied: %s\n", msg_buffer, MSG_BUFFER_SIZE);
	}while(1==1);

	destroy_socket(self_socket);
	return;
}