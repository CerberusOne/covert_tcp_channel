/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  05/21/2018 05:18:18 PM
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

void start_client(char *sip, char *dip, unsigned short sport, unsigned short dport,
        int ipid, int seq) {
    char input[BUFSIZE];
    FILE *file;


    puts("Client started\n\n");

    if(sip == 0 && dip == 0) {
        printf("Source and destination hosts required\n");
    }

    if(sport == 0) {
        printf("Source Port: random");
    }

    while(1) {
        printf("Input: ");
        fgets(input, BUFSIZE, stdin);
//        if((n = sscanf("%s", input)) >= 0) {
            //sleep(1); /* notes do this for reading from a file, probably not necessary here*/

            //covert_send(sip, dip, sport, dport, ipid, seq, 0, input);
            covert_send(sip, dip, sport, dport, ipid, seq, input);
 //       }
    }
}

