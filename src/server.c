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

void start_server(unsigned int sip, unsigned short sport, int ipid, int seq, int ack) {
    char ch;

    printf("Server started\n\n");

    while(1) {
        ch = covert_recv(sip, sport, ipid, seq, ack);
        printf("Output: %c\n", ch);
    }
}
