/*
 * =====================================================================================
 *
 *       Filename:  covert_socket.c
 *
 *    Description:  Creates IP, TCP, UDP or ICMP packets with stealth payloads hidden
 *                  inside the packet headers
 *
 *                  Ensure program mis run as root
 *
 *        Version:  1.0
 *        Created:  05/09/2018 04:55:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aing Ragunathan
 *
 *
 *          To Do:
 *              -add file transfer
 *              -encrypt packets
 *              -include NAT punch through
 * =====================================================================================
 */
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "server.h"
#include "client.h"

#define SOCKOPTS "c:s:f:"
#define BUFFERSIZE 1024


static void print_usage(void) {
    puts ("Usage options: \n"
            "\t[s]erver -   (default) sets to server mode\n"
            "\t[c]lient -   sets to client mode\n"
            "\t[f]orge  -   forging options\n"
            //forge options
            "\n\t\tIP\n"
            "\t\t[dip]      -   IP destination field\n"
            "\t\t[sip]      -   IP source field\n"
            "\t\t[ipid]     -   (default) IP ID field\n"
            "\n\t\tTCP\n"
            "\t\t[dport]    -   TCP destination port field\n"
            "\t\t[sport]    -   TCP source port field\n"
            "\t\t[seq]      -   TCP SEQ field\n"
            "\t\t[ack]      -   TCP ACK field\n");
}



int main(int argc, char** argv) {
    int arg, client_opt = 0, server_opt = 0, forge_opt = 0, dip, sip,
        ipid = 0, seq = 0, ack = 0;

    char dip_string[BUFFERSIZE], sip_string[BUFFERSIZE];
    unsigned short sport, dport;

    /* make sure user has root privilege */
    if(geteuid() != 0) {
        printf("Must run as root\n");
        exit(1);
    }

    /*  parse arguments */
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"server",  no_argument,        0,  's' },
            {"client",  no_argument,        0,  'c' },
            {"forge",   no_argument,        0,  'f' },
            {0,         0,                  0,  0   }
        };

        arg = getopt_long(argc, argv, SOCKOPTS, long_options, &option_index);

        if(arg == -1) {
           break;
        }

        switch (arg) {
            case 'c':
                client_opt = 1;
                printf("entering client mode\n");
                break;
            case 's':
                server_opt = 1;
                printf("entering server mode\n");
                break;
            case 'f':
                printf("entering forge mode\n");
                forge_opt ++;

                //check which fields to forge
                if(strcmp(optarg, "dip") == 0){
                    printf("entering DIP forge mode\n");
                    dip = host_convert(optarg);
                    strncpy(dip_string, optarg, BUFFERSIZE);
                } else if(strcmp(optarg, "sip") == 0) {
                    printf("entering SIP forge mode\n");
                    sip = host_convert(optarg);
                    strncpy(sip_string, optarg, BUFFERSIZE);
                } else if (strcmp(optarg, "sport") == 0) {
                    printf("entering SPORT forge mode\n");
                    sport = atoi(optarg);
                } else if (strcmp(optarg, "dport") == 0) {
                    printf("entering DPORT forge mode\n");
                    dport = atoi(optarg);
                } else if (strcmp(optarg, "ipid") == 0) {
                    printf("entering IPID forge mode\n");
                    ipid = 1;
                } else if (strcmp(optarg, "seq") == 0) {
                    printf("entering SEQ forge mode\n");
                    seq = 1;
                } else if (strcmp(optarg, "ack") == 0) {
                    printf("entering ACK forge mode\n");
                    ack = 1;
                }
                break;
            default: /*  '?' */
                print_usage();
                exit(1);
        }
    }

    if(client_opt == 0 && server_opt == 0) {
        printf("Using default mode: server\n");
        server_opt = 1;
    }

    //default to ipid mode if no forge arguments were made
    if(!forge_opt) {
        printf("Using default mode: ipid\n");
        ipid = 1;
    } else if (ipid + seq + ack!= 1) {
        printf("Only 1 of IPID, SEQ, or ACK can be used... exiting\n");
        exit(1);
    }

    if(server_opt)
        start_server(sip, dip, sport, dport, ipid, seq, ack);

    if(client_opt) {
        if(ack == 1) {
            printf("ack decoding only for server (-s)\n");
        }

        start_client(sip, dip, sport, dport, ipid, seq);
    }
}



