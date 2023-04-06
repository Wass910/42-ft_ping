#ifndef FT_PING
# define FT_PING

#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

#define PACKET_SIZE 64
#define IP_HDR_SIZE 20
#define ICMP_HDR_SIZE 8

typedef struct s_ipv4_header{
    unsigned int ihl:4;
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
}           t_ipv4_header;

typedef struct s_icmp_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union {
        struct {
            uint16_t id;
            uint16_t sequence;
        } echo;
        uint32_t gateway;
        struct {
            uint16_t __glibc_reserved;
            uint16_t mtu;
        } frag;
    } un;
}           t_icmp_header;

typedef struct s_all {
    t_ipv4_header   ip;
    t_icmp_header   icmp;
    char            *hostname;
}           t_all;

extern t_all    g_all;
#endif