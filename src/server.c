/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/21/2018 05:06:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include "covert_wrappers.h"

void start_server(char *sip, unsigned short sport, int ipid, int seq, int ack, char* filename) {
    char input;
    FILE *file;

    printf("Server started\n\n");

    if((file = fopen(filename, "wb")) == NULL) {
        perror("fopen can't open file");
        exit(1);
    }

    while(1) {
        input = covert_recv(sip, sport, ipid, seq, ack);
        if(input != 0) {
            printf("Output: %c\n", input);
            fprintf(file, "%c", input);
            input = 0;
        } else if (input == EOF){
            return;
        }
    }
}
