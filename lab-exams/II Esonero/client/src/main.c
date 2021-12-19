/*
 ============================================================================
 Name        : main.c
 Author      : Alberto Saltarelli
 Version     : 0.1
 Copyright   : Copyright © 2021 calculator_client - Alberto Saltarelli
 Description : A simple calculator in C (client-side)
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include "api.h"
#include "std_utils.h"
#include <string.h>

#define NO_ERROR 0
#define MAX_INPUT_SZ 30

/// Populate the operation struct from the user input
/// @param operation  a reference to a struct operation
///
/// It returns 0 if the operation is populated. Otherwise -1 if the user types '='.
int populate_operation(struct calc_operation * operation);

int main(int argc, char *argv[]) {
    char * server_name;
    char * server_address;
    int port;
    struct hostent *server_to;
    struct hostent *server_from;
    
    if (argc > 1) {
        char * token = strtok(argv[1], ":");
        server_name = token;
        token = strtok(NULL, ":");
        port = atoi(token);
    } else {
        server_name = SERVER_NAME;
        port = SERVER_PORT;
    }
 
#if defined WIN32
    // Initialize Winsock
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (result != NO_ERROR) {
        printf("Error at WSAStartup()\n");
        return -1;
    }
#endif
    
    server_to = gethostbyname(server_name);
    
    if (server_to == NULL) {
        printf("gethostbyname() failed. Can't resolve '%s'\n", server_name);
        exit(EXIT_FAILURE);
    } else {
        server_address = inet_ntoa(*((struct in_addr*) server_to->h_addr_list[0]));
    }
    
    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in fromAddr;
    unsigned int fromSize;
    struct calc_operation operation;
    int calc_operation_l = sizeof(calc_operation);
    calc_result result;
    int result_l = sizeof(calc_result);
    
    printf("+: Addition\n-: Subtraction\n/: Division\n*: Multiplication\n\n");

    while (1) {
        if (populate_operation(&operation) == -1) {
            // Close connection
            printf("\nGoodbye\n");
            clearwinsock();
            return NO_ERROR;
        }
        
        // Create socket
        if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
            printf("socket creation failed.\n");
            clearwinsock();
            return -1;
        }
        
        // Create server address
        memset(&echoServAddr, 0, sizeof(echoServAddr));
        echoServAddr.sin_family = AF_INET;
        echoServAddr.sin_port = htons(port);
        echoServAddr.sin_addr.s_addr = inet_addr(server_address);
        
        // Send operation to server
        if (sendto(sock, &operation, calc_operation_l, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != calc_operation_l) {
            printf("sendto() sent different number of bytes than expected");
            fflush(stdout);
            continue;
        }
        
        // Get result from server
        fromSize = sizeof(fromAddr);

        if (recvfrom(sock, &result, result_l, 0, (struct sockaddr*)&fromAddr, &fromSize) != result_l) {
            printf("recv() failed or connection closed prematurely");
            fflush(stdout);
            continue;
        }
        
        if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
            printf("Error: received a packet from unknown source.\n");
            fflush(stdout);
            continue;
        }
        
        server_from = gethostbyaddr(&fromAddr.sin_addr, 4, AF_INET);
        
        if (server_from == NULL) {
            printf("\nReceived result from server ip %s: '%i %c %i = %g'\n\n", inet_ntoa(fromAddr.sin_addr), operation.op1, operation.operator, operation.op2, result);
        } else {
            printf("\nReceived result from server %s, ip %s: '%i %c %i = %g'\n\n", server_from->h_name, inet_ntoa(fromAddr.sin_addr), operation.op1, operation.operator, operation.op2, result);
        }
        
        fflush(stdout);
    }
}
    
int populate_operation(struct calc_operation * operation) {
    char input[MAX_INPUT_SZ];
    char *ptr;
    int operation_valid = 0;

    while (operation_valid == 0) {
        if (scan_line("Enter operation (separated by a whitespace, e.g \"+ 3 5\", \"* -3 5\", \"/ 3 -5\"): ", input, sizeof(input)) == 0) {
            ptr = strtok(input, " ");
            (*operation).operator = ptr[0];

            if (operation->operator == '=') {
                return -1;
            } else if (operation->operator == '+' || operation->operator == '-' || operation->operator == '/' || operation->operator == '*') {
                ptr = strtok(NULL, " ");

                if (ptr == NULL) {
                    (*operation).op1 = 0;
                } else {
                    (*operation).op1 = atoi(ptr);
                }

                ptr = strtok(NULL, " ");

                if (ptr == NULL) {
                    (*operation).op2 = 0;
                } else {
                    (*operation).op2 = atoi(ptr);
                }

                if (operation->operator == '/' && operation->op2 == 0) {
                    printf("\nError\n\n");
                } else {
                    operation_valid = 1;
                }
            } else {
                printf("\nError\n\n");
            }
        }
    }

    return 0;
}

