#include "../inc/ft_ping.h"

t_all g_all = {{0}, {0}, {0}, NULL, NULL, -1, 0, 0, 0, 0, {0}};

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
    test = ip_str;
    freeaddrinfo(res);
    return test;
}

void	inthandler(int sig)
{
	if (sig == 2)
	{
		printf("\n--- %s ping statistics ---\n", g_all.addr);
        printf("%d packets transmitted, %d received, %.1f%% packet loss\n", g_all.packet_sent + 1, g_all.packet_receive, (float)(g_all.packet_sent - g_all.packet_receive) * 100 / g_all.packet_sent);
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", g_all.min, g_all.avg / (g_all.packet_sent + 1), g_all.max, calc_stddev());
        exit(0);
	}
}

void    ping_help()
{
    printf("Usage\n  ping [options] <destination>\n\nOptions:\n  <destination>      dns name or ip address\n  -h                 print help and exit\n");
    return;
}

void    send_ping(int sockfd)
{
    struct timeval  start_time;
    struct timeval  end_time;
    char recv_buffer[PACKET_SIZE];
    struct msghdr recv_msg;
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    struct sockaddr_in dest_addr;
    
    memset(&dest_addr, 0, sizeof(dest_addr));
    memset(&recv_msg, 0, sizeof(recv_msg));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, g_all.hostname, &(dest_addr.sin_addr));
    struct iovec iov[1];
    iov[0].iov_base = recv_buffer;
    iov[0].iov_len = PACKET_SIZE;
    recv_msg.msg_name = &recv_addr;
    recv_msg.msg_namelen = addr_len;
    recv_msg.msg_iov = iov;
    recv_msg.msg_iovlen = 1;

    while (1)
    {
        
        gettimeofday(&start_time, NULL);
        ssize_t bytes_sent = sendto(sockfd, &g_all.icmp_send, ICMP_HDR_SIZE, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (bytes_sent < 0) {
            perror("sendto");
            exit(1);
        }
        g_all.packet_sent++;
        ssize_t bytes_received = recvmsg(sockfd, &recv_msg, 0);
        if (bytes_received < 0) {
            perror("recvmsg");
            exit(1);
        }
        if (g_all.packet_receive == 0)
            printf("PING %s (%s) %d bytes of data\n", g_all.addr, g_all.hostname, PACKET_SIZE);
        g_all.packet_receive++;
        g_all.ip  = *(t_ipv4_header*)recv_buffer;
        g_all.icmp_receive = *(t_icmp_header*)(recv_buffer + (g_all.ip.ihl * 4));
        if (g_all.icmp_receive.type == ICMP_ECHOREPLY);
        else 
            printf("Ping failed!\n");
        gettimeofday(&end_time, NULL);
        
        double long time_diff = ((end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec) / 1000.000);
        g_all.data[g_all.packet_sent] = time_diff;
        if (time_diff < g_all.min || g_all.min == 0)
            g_all.min = time_diff;
        if (time_diff > g_all.max)
            g_all.max = time_diff;
        g_all.avg = g_all.avg + time_diff;
        printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n", bytes_received, inet_ntoa(recv_addr.sin_addr), g_all.packet_sent, g_all.ip.ttl, time_diff);
        usleep(1000000);
        
    }
    return ;
}

int main(int argc, char **argv) 
{
    if (argc < 2) {
        printf("Usage: %s <hostname>\n", argv[0]);
        return 1;
    }
    else if (argc == 3)
    {
        if (ft_strncmp(delete_space(argv[1]), "-h", ft_strlen(argv[1])) == 0 || ft_strncmp(delete_space(argv[2]), "-h", ft_strlen(argv[2])) == 0)
            ping_help();
        exit(1);
    }
    g_all.addr = argv[1];
    g_all.hostname = get_the_adresse(argv[1]);
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    signal(SIGQUIT, inthandler);
	signal(SIGINT, inthandler);

    // Remplissage du header ICMP
    g_all.icmp_send.type = ICMP_ECHO;
    //g_all.icmp_send.code = 0;
    //g_all.icmp_send.checksum = 0;
    g_all.icmp_send.un.echo.id = htons(getpid());
    //g_all.icmp_send.un.echo.sequence = 0;
    g_all.icmp_send.checksum = checksum((unsigned short*)&g_all.icmp_send, ICMP_HDR_SIZE);
    
    send_ping(sockfd);
    
    close(sockfd);
    return 0;
}