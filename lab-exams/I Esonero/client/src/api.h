/*
 * api.h
 *
 *  Created on: Nov 23, 2021
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

#define LOCAL_HOST "127.0.0.1"
#define PROTOPORT 60000 // default protocol port number
#define QLEN 5 // size of request queue (server is not multi-threading, in this example)

typedef struct calc_operation {
    char operator; // +, -, /, *
    int op1;
    int op2;
} calc_operation;

typedef float calc_result;

void clearwinsock(void);

#endif /* API_H_ */
