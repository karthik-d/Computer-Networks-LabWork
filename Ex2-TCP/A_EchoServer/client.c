#include<stdio.h>
#include<stdlib.h>

#include "tcp_socket.h"

void main(){

	int self_socket = make_socket();
	if(self_socket<0){
		printf("\nCould not create socket. Retry!\n");
		return;
	}

	char *server_ip = (char*)malloc(sizeof(char)*IP_STRING_LEN);
	printf("\nEnter File-Server IP Address: ");
	scanf(" %s", server_ip);
	if (connect_server(self_socket, server_ip) < 0){
		printf("\nCould not connect to ECHO-Server.\nMake sure the server is running!\n");
		destroy_socket(self_socket);
		return;
	}
	else{
		printf("\nConnected to ECHO-Server");
	}

	char *msg_buffer = (char*)malloc(sizeof(char)*MSG_BUFFER_SIZE);
	int msg_size = 0;
	printf("\n\nDelimit Ping Messages with ';'\nEnter 'ENDSESSION;' to terminate connection\n");
	do {
		bzero(msg_buffer, MSG_BUFFER_SIZE);
		printf("\nEnter Ping Message: ");
		scanf(" %[^;]s", msg_buffer);
		// Consume the last newline character from read-buffer
		getchar();   
		msg_size = write(self_socket, msg_buffer, MSG_BUFFER_SIZE);
		// Reading back
		bzero(msg_buffer, MSG_BUFFER_SIZE);
		msg_size = read(self_socket, msg_buffer, MSG_BUFFER_SIZE);
		// Check if server acknowledged ENDSESSION
		if (check_termination_ack(msg_buffer)){
			printf("\nExiting...\n");
			break;
		}
		printf("SERVER echoed: %s\n", msg_buffer, MSG_BUFFER_SIZE);
	}while(1==1);
}