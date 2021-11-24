/*
 ============================================================================
 Name        : calculator_server.c
 Author      : Alberto Saltarelli
 Version     : 0.1
 Copyright   : Copyright © 2021 calculator_server - Alberto Saltarelli
 Description : A simple calculator in C (server-side)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "api.h"

#define NO_ERROR 0

/* function declaration */
calc_result add(int op1, int op2);
calc_result mult(int op1, int op2);
calc_result sub(int op1, int op2);
calc_result division(int op1, int op2);
calc_result calculate(struct calc_operation *operation);

int main(int argc, char *argv[]) {
    int port;
    if (argc > 1) {
        port = atoi(argv[1]);

        if (port < 0) {
            printf("bad port number %s \n", argv[1]);
            return -1;
        }
    } else {
        port = PROTOPORT;
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

    // Create socket
    int welcome_socket;
    welcome_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (welcome_socket < 0) {
        printf("socket creation failed.\n");
        clearwinsock();
        return -1;
    }

    // Bind socket address
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    sad.sin_port = htons(port);

    if (bind(welcome_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
        printf("socket bind failed.\n");
        closesocket(welcome_socket);
        clearwinsock();
        return -1;
    }

    // Open listening session
    if (listen(welcome_socket, QLEN) < 0) {
        printf("socket listen failed.\n");
        closesocket(welcome_socket);
        clearwinsock();
        return -1;
    }

    // Accept new client request
    struct sockaddr_in cad;
    int client_socket;
    int client_open = 0;
    socklen_t client_len;

    struct calc_operation operation;
    int calc_operation_l = sizeof(calc_operation);
    calc_result result;
    int result_l = sizeof(calc_result);

    printf("\nWaiting for a client to connect...\n");

    while (1) {
        client_len = sizeof(cad);
        if ((client_socket = accept(welcome_socket, (struct sockaddr *) &cad, &client_len)) < 0) {
            printf("socket accept failed.\n");
            closesocket(welcome_socket);
            clearwinsock();
            return -1;
        }

        printf("Connection established with %s:%i", inet_ntoa(cad.sin_addr), cad.sin_port);
        client_open = 1;

        while (client_open == 1) {
            if (recv(client_socket, &operation, calc_operation_l, 0) == calc_operation_l) {
                result = calculate(&operation);
                if (send(client_socket, &result, result_l, 0) != result_l) {
                    client_open = 0;
                }
            } else {
                client_open = 0;
            }
        }

        printf("\nDisconnect client %s:%i\n", inet_ntoa(cad.sin_addr), cad.sin_port);
    }

    closesocket(welcome_socket);
    clearwinsock();
    return NO_ERROR;
}

calc_result add(int op1, int op2) {
    return (float) op1 + op2;
}

calc_result mult(int op1, int op2) {
    return (float) op1 * op2;
}

calc_result sub(int op1, int op2) {
    return (float) op1 - op2;
}

calc_result division(int op1, int op2) {
    if (op2 == 0) {
        return 0;
    } else {
        return (float) op1 / op2;
    }
}

calc_result calculate(struct calc_operation *operation) {
    if (operation->operator == '+') {
        return add(operation->op1, operation->op2);
    } else if (operation->operator == '*') {
        return mult(operation->op1, operation->op2);
    } else if (operation->operator == '-') {
        return sub(operation->op1, operation->op2);
    } else if (operation->operator == '/') {
        return division(operation->op1, operation->op2);
    } else {
        return 0;
    }
}
