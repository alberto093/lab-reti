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
    char * server_name;
    char * server_address;
    int port;
    struct hostent *server;
    struct hostent *client;
    
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
    
    
    server = gethostbyname(server_name);
    
    if (server == NULL) {
        printf("gethostbyname() failed. Can't resolve '%s'\n", server_name);
        exit(EXIT_FAILURE);
    } else {
        server_address = inet_ntoa(*((struct in_addr*) server->h_addr_list[0]));
    }
    
    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    
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
    
    // Bind socket address
    if (bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0) {
        printf("socket bind failed.\n");
        closesocket(sock);
        clearwinsock();
        return -1;
    }
    
    struct calc_operation operation;
    ssize_t calc_operation_l = sizeof(calc_operation);
    calc_result result;
    ssize_t result_l = sizeof(calc_result);
    
    printf("Waiting for a client request...\n");
    
    while (1) {
        cliAddrLen = sizeof(echoClntAddr);
        calc_operation_l = recvfrom(sock, &operation, calc_operation_l, 0, (struct sockaddr*)&echoClntAddr, &cliAddrLen);
        
        printf("Request operation '%c %i %i' from client ", operation.operator, operation.op1, operation.op2);
        
        client = gethostbyaddr(&echoClntAddr.sin_addr, 4, AF_INET);
        if (client == NULL) {
            printf("%s\n", inet_ntoa(echoClntAddr.sin_addr));
        } else {
            printf("%s, ip %s\n", client->h_name, inet_ntoa(echoClntAddr.sin_addr));
        }
        
        fflush(stdout);
        
        result = calculate(&operation);
        
        if (sendto(sock, &result, result_l, 0, (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) != result_l) {
            printf("sendto() sent different number of bytes than expected");
        }
    }
    
    closesocket(sock);
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
