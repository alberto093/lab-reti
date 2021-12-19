/*
 * api.h
 *
 *  Created on: Dec 19, 2021
 *      Author: Alberto Saltarelli
 */

#ifndef API_H_
#define API_H_

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

#define SERVER_NAME "localhost"
#define SERVER_PORT 56700 // default protocol port number

typedef struct calc_operation {
    char operator; // +, -, /, *
    int op1;
    int op2;
} calc_operation;

typedef float calc_result;

void clearwinsock(void);

#endif /* API_H_ */
