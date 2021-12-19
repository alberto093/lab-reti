/*
 ============================================================================
 Name        : std_utils.c
 Author      : Alberto Saltarelli
 Version     : 0.1
 Copyright   : Copyright © 2021 calculator_server - Alberto Saltarelli
 Description : A simple calculator in C (client-side)
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include "std_utils.h"

int scan_line(char *prompt, char *buffer, int size) {
    int ch, extra;

    // Get line with buffer overrun protection.
    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (fgets(buffer, size, stdin) == NULL) {
        return -1;
    }

    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buffer[strlen(buffer)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF)) {
            extra = 1;
        }

        return (extra == 1) ? -1 : 0;
    }

    // Otherwise remove newline and give string back to caller.
    buffer[strlen(buffer)-1] = '\0';

    return 0;
}
