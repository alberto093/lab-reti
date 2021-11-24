/*
 ============================================================================
 Name        : main.c
 Author      : Alberto Saltarelli
 Version     : 0.1
 Copyright   : Copyright © 2021 calculator_client - Alberto Saltarelli
 Description : A simple calculator in C (client-side)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
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
    sad.sin_port = htons(port);

    if (connect(c_socket, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
        printf("socket connect failed.\n");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }

    printf("Connection established!\n");

    struct calc_operation operation;
    int calc_operation_l = sizeof(calc_operation);
    calc_result result;
    int result_l = sizeof(calc_result);

    printf("+: Addition\n-: Subtraction\n/: Division\n*: Multiplication\n\n");

    while (1) {
        if (populate_operation(&operation) == -1) {
            // Close connection
            printf("\nGoodbye\n");
            closesocket(c_socket);
            clearwinsock();
            return NO_ERROR;
        }

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

        printf("\nResult: %g\n\n", result);
    }

    closesocket(c_socket);
    clearwinsock();
    return NO_ERROR;
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

