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
#include <time.h>
#include "covert_wrappers.h"

int rand_delay(int delay) ;

void start_client(char *sip, char *dip, unsigned short sport, unsigned short dport,
        int ipid, int seq, int tos, char* filename, int max_delay) {
    char input;
    FILE *file;
    clock_t start;
    int timer_complete = 0, delay = 0;
//    time_t time;
    double passed;


    puts("Client started\n\n");

    if(sip == 0 && dip == 0) {
        printf("Source and destination hosts required\n");
    }

    if(sport == 0) {
        printf("Source Port: random");
    }

    if((file = fopen(filename, "rb")) == NULL) {
        perror("fopen can't open file");
        exit(1);
    }

    while((input = fgetc(file)) != EOF) {
        covert_send(sip, dip, sport, dport, ipid, seq, tos, input); //send the packet
        start = clock();    //start of clock
        timer_complete = 0;    //reset the timer again
        delay = rand_delay(max_delay);
        printf("delay: %d\n", delay);


        //wait for the timer to complete
        while(timer_complete == 0) {
            passed = (clock() - start) / CLOCKS_PER_SEC;
            if(passed >= delay) {
                printf("Delay completed\n");
                timer_complete = 1;
            }
        }
    }

    return;
}

int rand_delay(int delay) {
    return rand() % delay + 1;
}
