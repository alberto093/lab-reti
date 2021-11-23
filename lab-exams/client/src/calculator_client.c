/*
 ============================================================================
 Name        : calculator_client.c
 Author      : Alberto Saltarelli
 Version     : 0.1
 Copyright   : Copyright © 2021 calculator_client - Alberto Saltarelli
 Description : A simple calculator in C (client-side)
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "api.h"


#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {
	in_addr_t address;
	int port;


	if (argc > 2) {
		address = inet_addr(argv[1]);
		port = atoi(argv[2]);

		if (address < 0) {
			printf("bad address %s \n", argv[1]);
			return -1;
		}

		if (port < 0) {
			printf("bad port number %s \n", argv[2]);
			return -1;
		}
	} else {
		address = inet_addr(LOCAL_HOST);
		port = PROTOPORT;
	}

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	// Create socket
	int c_socket;
	c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (c_socket < 0) {
		printf("socket creation failed.\n");
		clearwinsock();
		return -1;
	}

	// Connect to server
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = address;
	sad.sin_port = port;

	if (connect(c_socket, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
		printf("socket connect failed.\n");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}

	printf("Connection established!\n");

	struct calc_operation operation;
	int calc_operation_l = sizeof(calc_operation);
	int operator_valid = 0;
	calc_result result;
	int result_l = sizeof(calc_result);

	while (1) {
		while (operator_valid == 0) {
			printf("+: Addition\n-: Subtraction\n/: Division\n*: Multiplication\n\nEnter operation (separated by a whitespace, e.g \"+ 3 5\", \"* -3 5\", \"/ 3 -5\"): ");
			scanf("%c %d %d", &operation.operator, &operation.op1, &operation.op2);

			if (operation.operator == '=') {
				// Close connection
				printf("\nGoodbye\n");
				closesocket(c_socket);
				clearwinsock();
				system("pause");
				return NO_ERROR;
			} else if (operation.operator == '+' || operation.operator == '-' || operation.operator == '/' || operation.operator == '*') {
				operator_valid = 1;
			} else {
				operator_valid = 0;
			}

			if (operator_valid == 0 || (operation.operator == '/' && operation.op2 == 0)) {
				printf("\nError\n");
			}
		}

		operator_valid = 0;

		if (send(c_socket, &operation, calc_operation_l, 0) != calc_operation_l) {
			printf("send() sent a different number of bytes than expected");
			closesocket(c_socket);
			clearwinsock();
			return -1;
		}

		if (recv(c_socket, &result, result_l, 0) != result_l) {
			printf("recv() failed or connection closed prematurely");
			closesocket(c_socket);
			clearwinsock();
			return -1;
		}

		printf("\nResult: %f\n\n", result);
	}

	closesocket(c_socket);
	clearwinsock();
	return NO_ERROR;
}
