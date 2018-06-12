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

#define SOCKOPTS "x::c::s:f:"
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
    int arg, client_opt = 0, server_opt = 0, dip, sip,
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
            {"server",  no_argument,        0,  0 },
            {"client",  no_argument,        0,  1 },
            {"dip",     required_argument,  0,  2 },
            {"sip",     required_argument,  0,  3 },
            {"dport",   required_argument,  0,  4 },
            {"sport",   required_argument,  0,  5 },
            {"ipid",    no_argument,        0,  6 },
            {"seq",     no_argument,        0,  7 },
            {"ack",     no_argument,        0,  8 },
            {0,         0,                  0,  0   }
        };

        arg = getopt_long(argc, argv, SOCKOPTS, long_options, &option_index);

        if(arg == -1) {
           break;
        }

        switch (arg) {
            case 0:
                server_opt = 1;
                printf("Entering server mode\n");
                break;
            case 1:
                client_opt = 1;
                printf("Entering client mode\n");
                break;
            case 2:
                dip = host_convert(optarg);
                strncpy(dip_string, optarg, BUFFERSIZE);
                printf("Destination IP: %s\n", dip_string);
                break;
            case 3:
                sip = host_convert(optarg);
                strncpy(sip_string, optarg, BUFFERSIZE);
                printf("Forging source IP (Remote Bounce): %s\n", dip_string);
                break;
            case 4:
                dport = atoi(optarg);
                printf("Destination port: %d\n", dport);
                break;
            case 5:
                sport = atoi(optarg);
                printf("Source port: %d\n", sport);
                break;
            case 6:
                ipid = 1;
                printf("Forging IPID\n");
                break;
            case 7:
                seq = 1;
                printf("Forging SEQ\n");
                break;
            case 8:
                ack = 1;
                printf("Forging ACK for remote bounce\n");
                break;
            default: /*  '?' */
                print_usage();
                exit(1);
        }
    }

    if(client_opt == 0 && server_opt == 0) {
        printf("Using default mode: server\n");
        server_opt = 1;
    } else if (client_opt + server_opt == 2) {
        printf("Invalid argument sequence: client & server\n");
        exit(1);
    }

    //default to ipid mode if no forge arguments were made
    if(ipid + seq + ack == 0) {
        printf("Using default mode: IPID\n");
        ipid = 1;
    } else if (ipid + seq + ack!= 1) {
        printf("Only 1 of IPID, SEQ, or ACK can be used... exiting\n");
        exit(1);
    } else if(dip == 0) {
        dip = host_convert("127.0.0.1");
        printf("No destination provided, using localhost\n");
    }

    if(server_opt)
        start_server(sip, dip, sport, dport, ipid, seq, ack);

    if(client_opt) {
        if(ack == 1) {
            printf("ack decoding only for server\n");
        }

        start_client(sip, dip, sport, dport, ipid, seq);
    }
}



