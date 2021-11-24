/*
 ============================================================================
 Name        : api.c
 Author      : Alberto Saltarelli
 Version     : 0.1
 Copyright   : Copyright © 2021 calculator_server - Alberto Saltarelli
 Description : A simple calculator in C (server-side)
 ============================================================================
 */

#include <stdio.h>
#include "api.h"

void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}
