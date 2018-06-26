/*
 * =====================================================================================
 *
 *       Filename:  covert_wrappers.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  05/29/2018 11:59:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "covert_wrappers.h"

void covert_send(char *sip, char *dip, unsigned short sport, unsigned short dport, int ipid, int seq, char message[BUFSIZE]) {
    int ch, bytes_sent;
    int sending_socket;
    struct sockaddr_in sin;
    unsigned int sip_binary, dip_binary;

    sip_binary = host_convert(sip);
    dip_binary = host_convert(dip);

    for(unsigned int i = 0; i < strlen(message); i++) {
        struct send_tcp packet;
        ch = message[i];

        sleep(1);

        //create IP header
        packet.ip.ihl = 5;
        packet.ip.version = 4;
        packet.ip.tos = 0;
        packet.ip.tot_len = htons(40);

        //check if we are forging IPID
        if(ipid == 0) {
            packet.ip.id = generate_rand(255.0);    //random, to not look suspecious
        } else {
            packet.ip.id = ch;  //enter a single ASCII character into the field
        }

        packet.ip.frag_off = 0;
        packet.ip.ttl = 64;
        packet.ip.protocol = IPPROTO_TCP;
        packet.ip.check = 0;
        packet.ip.saddr = sip_binary;
        packet.ip.daddr = dip_binary;

        //create TCP header
        //check if source port was set
        if(sport == 0) {
            packet.tcp.source = generate_rand(10000.0);
        } else {
            packet.tcp.source = htons(sport);
        }

        //check if we are forging SEQ
        if(seq == 0) {
            packet.tcp.seq = generate_rand(10000.0);
        } else {
            packet.tcp.seq = ch;
        }

        packet.tcp.dest = htons(dport);
        packet.tcp.ack_seq = 0;
        packet.tcp.res1 = 0;
        packet.tcp.doff = 5;
        packet.tcp.fin = 0;
        packet.tcp.syn = 1;
        packet.tcp.rst = 0;
        packet.tcp.psh = 0;
        packet.tcp.ack = 0;
        packet.tcp.urg = 0;
        packet.tcp.res2 = 0;
        packet.tcp.window = htons(512);
        packet.tcp.check = 0;
        packet.tcp.urg_ptr = 0;

        //creat socket struct
        sin.sin_family = AF_INET;
        sin.sin_port = packet.tcp.source;
        sin.sin_addr.s_addr = packet.ip.daddr;

        //open socket for sending
        sending_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

        if(sending_socket < 0) {
            perror("sending socket failed to open (root maybe required)");
            exit(1);
        }

        //create an IP checksum value
        packet.ip.check = checksum((unsigned short *) &send_tcp.ip, 20);

        pseudo_header.source_address = packet.ip.saddr;
        pseudo_header.dest_address = packet.ip.daddr;
        pseudo_header.placeholder = 0;
        pseudo_header.protocol = IPPROTO_TCP;
        pseudo_header.tcp_length = htons(20);

        //copy packet's tcp into pseudo header tcp
        bcopy((char *) &packet.tcp, (char *) &pseudo_header.tcp, 20);

        //create a TCP checksum value
        packet.tcp.check = checksum((unsigned short *) &pseudo_header, 32);

        //send the packet
        if((bytes_sent = sendto(sending_socket, &packet, 40, 0, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        //if((bytes_sent = send(sending_socket, &packet, 40, 0, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
            perror("sendto");
        }
        printf("Sending Data(%d): %c\n", bytes_sent, ch);
    }
}

char covert_recv(char *sip, unsigned short sport, int ipid, int seq, int ack) {
    int recv_socket, n, bytes_recv;
    unsigned int sip_binary;
    //struct recv_tcp recv_packet;
    sip_binary = host_convert(sip);

    if((n = recv_socket = socket(AF_INET, SOCK_RAW, 6)) < 0) {
        perror("receiving socket failed to open (root maybe required)");
    }

    bytes_recv = read(recv_socket, (struct recv_tcp *)&recv_tcp, 9999);

    if(sport == 0) {    //from any port
        if((recv_tcp.tcp.syn == 1) && (recv_tcp.ip.saddr == sip_binary)) {
            if(ipid == 1) {
                printf("Receiving Data(%d): %c\n", bytes_recv, recv_tcp.ip.id);
                //fprintf(output, "%c", recv_tcp.ip.id);
                //fflush(output);
                return recv_tcp.ip.id;
            } else if(seq == 1) {
                printf("Receiving Data(%d): %c\n", bytes_recv, recv_tcp.tcp.seq);
                //fprintf(output, "%c", recv_tcp.tcp.seq);
                //fflush(output);
                return recv_tcp.tcp.seq;

            //Bounced packets
                //client must send packet with server's source IP to another host.
                    //flags: --client --sip <the server> --ack?
            } else if(ack == 1) {
                printf("Receiving Data: %c\n", recv_tcp.tcp.ack_seq);
                //fprintf(output, "%c", recv_tcp.tcp.ack_seq);
                //fflush(output);
                return recv_tcp.tcp.ack_seq;
            }
        }

    //doesn't check source IP in case we're bouncing off hosts
    } else {
        if((recv_tcp.tcp.syn == 1) && (ntohs(recv_tcp.tcp.dest) == sport)) {
            if(ipid == 1) {
                //printf("Receiving Data(%d): %c\n", bytes_recv, recv_tcp.ip.id);
                //fprintf(output, "%c", recv_tcp.ip.id);
                //fflush(output);
                return recv_tcp.ip.id;
            } else if(seq == 1) {
                //printf("Receiving Data(%d): %c\n", bytes_recv, recv_tcp.tcp.seq);
                //fprintf(output, "%c", recv_tcp.tcp.seq);
                //fflush(output);
                return recv_tcp.tcp.seq;

            //Bounced packets
                //client must send packet with server's source IP to another host.
                    //flags: --client --sip <the server> --ack?
            } else if(ack == 1) {
                //printf("Receiving Data: %c\n", recv_tcp.tcp.ack_seq);
                //fprintf(output, "%c", recv_tcp.tcp.ack_seq);
                //fflush(output);
                return recv_tcp.tcp.ack_seq;
            }
        }
    }

    close(recv_socket);
    return 0;
}

int generate_rand() {
    return 1 + (int)(10000.0 * rand() / RAND_MAX + 1.0);
}

unsigned int host_convert(char* hostname) {
    struct in_addr i;
    struct hostent *h;

    i.s_addr = inet_addr(hostname);

    if(i.s_addr == 1) {
        h = gethostbyname(hostname);

        if(h == NULL) {
            fprintf(stderr, "cannot resolve %s\n", hostname);
            exit(0);
        }

        //bcopy(h->h_addr, (char *)&i.s_addr, h->h_length);
    }

    return i.s_addr;
}

/* Copyright (c)1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * dupliated in all such forms and that any documentation, advertising
 * materials, and other materials related to such distribution and use
 * acknowledge that the software was developed by the University of
 * California, Berkeley. The name of the University may not be used
 * to endorse or promote products derived from this software without
 * specific prior written permission. THIS SOFTWARE IS PROVIDED ``AS
 * IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHATIBILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE
 */

unsigned short checksum(unsigned short *ptr, int nbytes)
{
	register long		sum;		/* assumes long == 32 bits */
	u_short			oddbyte;
	register u_short	answer;		/* assumes u_short == 16 bits */

	/*
	 * Our algorithm is simple, using a 32-bit accumulator (sum),
	 * we add sequential 16-bit words to it, and at the end, fold back
	 * all the carry bits from the top 16 bits into the lower 16 bits.
	 */

	sum = 0;
	while (nbytes > 1)  {
		sum += *ptr++;
		nbytes -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nbytes == 1) {
		oddbyte = 0;		/* make sure top half is zero */
		*((u_char *) &oddbyte) = *(u_char *)ptr;   /* one byte only */
		sum += oddbyte;
	}

	/*
	 * Add back carry outs from top 16 bits to low 16 bits.
	 */

	sum  = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;		/* ones-complement, then truncate to 16 bits */
	return(answer);
} /* end in_cksm() */
