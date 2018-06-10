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

void start_client(unsigned int sip, unsigned int dip, unsigned short sport, unsigned short dport,
        int ipid, int seq) {
    int n;
    char input[BUFSIZE];

    puts("Client started\n\n");

    if(sip == 0 && dip == 0) {
        printf("Source and destination hosts required\n");
    }

    if(sport == 0) {
        printf("Source Port: random");
    }

    while((n = scanf("%s", input)) == 1) {
        printf("input: %s\n", input);
        //sleep(1); /* notes do this for reading from a file, probably not necessary here*/

        forge_packet(sip, dip, sport, dport, ipid, seq, 0, input);
        //create IP header
        //forge_packet(sip, dip, sport, dport, ipid, seq, ack, input);
            //check to see if we are doing ipid encoding
            //true: set the ipid to appropriate value
            //false: set the ipid to random value

        //create TCP header

        //check to see if we are doing src port
        //check if we are doing seq

        //create socket struct
        //create socket

        //make IP header checksum

        //make a pseudo header

        //send the packet
        printf("sending: %s\n", input);
    }

}

