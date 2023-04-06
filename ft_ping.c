#include "ft_ping.h"

t_all g_all = {0, 0};

// void hexdump(void *ptr, int buflen) {
//     unsigned char *buf = (unsigned char*)ptr;
//     int i, j;
//     for (i=0; i<buflen; i+=16) {
//         printf("%06x: ", i);
//         for (j=0; j<16 && i+j<buflen; j++) {
//             printf("%02x ", buf[i+j]);
//         }
//         for (; j<16; j++) {
//             printf("   ");
//         }
//         printf(" ");
//         for (j=0; j<16 && i+j<buflen; j++) {
//             printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
//         }
//         printf("\n");
//     }
// }

unsigned short checksum(unsigned short *buf, int len) {
    unsigned long sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(unsigned char*)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

char *get_the_adresse(char *hostname) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        printf("ping: cannot resolve %s: Unknown host\n", hostname);
        exit(1);
    }
    struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
    char ip_str[16];
    char *test;
    inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("IP address of %s is %s\n", hostname, ip_str);
    test = ip_str;
    freeaddrinfo(res);
    return test;
}

void	inthandler(int sig)
{
	if (sig == 2)
	{
		printf("--- %s ping statistics ---\n", "argv[1]");
        exit(1);
	}
}

int main(int argc, char **argv) 
{
    if (argc != 2) {
        printf("Usage: %s <hostname>\n", argv[0]);
        return 1;
    }
    g_all.hostname = get_the_adresse(argv[1]);
    printf("hostname: -%s-\n", g_all.hostname);
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    signal(SIGQUIT, inthandler);
	signal(SIGINT, inthandler);
    // Remplissage du header ICMP
    g_all.icmp.type = ICMP_ECHO;
    g_all.icmp.code = 0;
    g_all.icmp.checksum = 0;
    g_all.icmp.un.echo.id = htons(getpid());
    g_all.icmp.un.echo.sequence = 0;
    g_all.icmp.checksum = checksum((unsigned short*)&g_all.icmp, ICMP_HDR_SIZE);

    // Envoi du paquet ICMP à Google
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    struct msghdr recv_msg;
    memset(&recv_msg, 0, sizeof(recv_msg));
    struct iovec iov[1];
    char recv_buffer[PACKET_SIZE];
    iov[0].iov_base = recv_buffer;
    iov[0].iov_len = PACKET_SIZE;
    recv_msg.msg_name = &recv_addr;
    recv_msg.msg_namelen = addr_len;
    recv_msg.msg_iov = iov;
    recv_msg.msg_iovlen = 1;
    inet_pton(AF_INET, g_all.hostname, &(dest_addr.sin_addr));
    int i = 0;
    struct timeval start_time, end_time;
    long int time;
    while (1)
    {
        
        gettimeofday(&start_time, NULL);
        ssize_t bytes_sent = sendto(sockfd, &g_all.icmp, ICMP_HDR_SIZE, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (bytes_sent < 0) {
            perror("sendto");
            exit(1);
        }

        ssize_t bytes_received = recvmsg(sockfd, &recv_msg, 0);
        if (bytes_received < 0) {
            perror("recvmsg");
            exit(1);
        }
    
        t_ipv4_header* ip_header = (t_ipv4_header*)recv_buffer;
        t_icmp_header* icmp_resp = (t_icmp_header*)(recv_buffer + (ip_header->ihl * 4));
        if (icmp_resp->type == ICMP_ECHOREPLY) {
            printf("Ping successful!\n");
        } else {
            printf("Ping failed!\n");
        }
        gettimeofday(&end_time, NULL);
        //printf
        
        double time_diff = ((end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec) / 1000.000);
        printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", bytes_received, inet_ntoa(recv_addr.sin_addr), i, ip_header->ttl, time_diff);
        // hexdump(recv_buffer, bytes_received);
        // // Analyse de la réponse
        // printf("Received packet from %s\n", inet_ntoa(recv_addr.sin_addr));
        // printf("IP header:\n");
        // //printf("\tVersion: %u\n", ip_header->version);
        // printf("\tHeader length: %u bytes\n", ip_header->ihl * 4);
        // printf("\tType of service: %u\n", ip_header->tos);
        // printf("\tTotal length: %u bytes\n", ntohs(ip_header->tot_len));
        // printf("\tIdentification: %u\n", ntohs(ip_header->id));
        // printf("\tFlags: %u\n", (ntohs(ip_header->frag_off) & 0xE000) >> 13);
        // printf("\tFragment offset: %u\n", ntohs(ip_header->frag_off) & 0x1FFF);
        // printf("\tTime to live: %u\n", ip_header->ttl);
        // printf("\tProtocol: %u\n", ip_header->protocol);
        // printf("\tHeader checksum: 0x%x\n", ntohs(ip_header->check));
        // //printf("\tSource IP address: %s\n", inet_ntoa(ip_header->saddr));
        // //printf("\tDestination IP address: %s\n", inet_ntoa(ip_header->daddr));

        // printf("ICMP header:\n");
        // printf("\tType: %u\n", icmp_resp->type);
        // printf("\tCode: %u\n", icmp_resp->code);
        // printf("\tChecksum: 0x%x\n", ntohs(icmp_resp->checksum));
        // printf("\tIdentifier: %u\n", ntohs(icmp_resp->un.echo.id));
        // printf("\tSequence number: %u\n", ntohs(icmp_resp->un.echo.sequence));
        usleep(1000000);
        //printf("i=%d\n", i);
        i++;
    }
    
    close(sockfd);
    return 0;
}