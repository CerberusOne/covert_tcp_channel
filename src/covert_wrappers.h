/*
 * =====================================================================================
 *
 *       Filename:  covert_wrappers.h
 *
 *    Description:  Wrapper functions for covert communications
 *
 *        Version:  1.0
 *        Created:  05/09/2018 05:28:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aing Ragunathan
 *
 * =====================================================================================
 */
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <netdb.h>
#include <string.h>

#define _BSD_SOURCE
#define BUFSIZE 1024

struct send_tcp {
    struct iphdr ip;
    struct tcphdr tcp;
} send_tcp;

struct recv_tcp {
    struct iphdr ip;
    struct tcphdr tcp;
    char buffer[BUFSIZE];
} recv_tcp;

struct pseudo_header {
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;
    struct tcphdr tcp;
} pseudo_header;

void covert_send(unsigned int sip, unsigned int dip, unsigned short sport, unsigned short dport,
        int ipid, int seq, int ack, char message[BUFSIZE]);
void covert_recv(unsigned int sip, unsigned int dip, unsigned short sport, unsigned short dport,
        int ipid, int seq, int ack);
int generate_rand();
unsigned int host_convert(char* ip);
unsigned short checksum(unsigned short* ptr, int nbytes);
