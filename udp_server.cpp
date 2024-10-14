#include <iostream>
#include <cstring> 
#include <stdlib.h>     
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>     
#include <net/if.h>
#include <sys/ioctl.h>

#define MULTICAST_GROUP "239.255.255.2"    // 多播地址 239.255.255.2
#define MULTICAST_PORT 30094               // 接收数据的端口号 30094
#define ENO1_IP_ADDR "172.20.2.1"          // eno1网卡ip地址
#define RADAR_IP_ADDR "172.20.2.82"        // 雷达ip地址
#define BUFFER_SIZE 1024*1024              // 缓冲区大小

int main() {
    // 1. 创建socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
    }
    
    // 2. 绑定本地ip和多播组端口
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MULTICAST_PORT);
    // servaddr.sin_addr.s_addr = INADDR_ANY;                       // 接收来自任何地址的数据
    servaddr.sin_addr.s_addr = inet_addr(ENO1_IP_ADDR);

    // 设置端口复用
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR error");
        return -1;
    }

    int ttl = 10;
    setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));    
    int rcvbuf = BUFFER_SIZE;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));


    // 3. 绑定多播组端口
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind error !");
    }

    // 4. 加入多播组239.255.255.2
    // 4.1 加入一个源特定多播组
    struct ip_mreq_source group; 
    memset(&group, 0, sizeof(group));
    group.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);    // 设置多播地址
    group.imr_sourceaddr.s_addr = inet_addr(RADAR_IP_ADDR);     // 源地址
    // group.imr_interface.s_addr = htonl(INADDR_ANY);         
    group.imr_interface.s_addr = inet_addr(ENO1_IP_ADDR);       // 主机ip地址

    int n = setsockopt(sockfd, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, &group, sizeof(group));
    if (n < 0) {
        perror("Error joining multicast group");
        return -1;
    } else {
        printf("Successfully joined multicast group: %s\n", MULTICAST_GROUP);
    }
    
    // // 4.2 加入一个多播组
    // struct ip_mreq group;
    // memset(&group, 0, sizeof(group));
    // group.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);   // 设置多播地址
    // // group.imr_interface.s_addr = htonl(INADDR_ANY);         
    // group.imr_interface.s_addr = inet_addr(ENO1_IP_ADDR);      // 主机ip地址

    // int n = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));
    // printf("setsockopt n = %d\n", n);
    // if (n < 0) {
    //     perror("Error joining multicast group");
    // } else {
    //     printf("Successfully joined multicast group: %s\n", MULTICAST_GROUP);
    // }
    
    // printf("UdpSer sockfd = %d, start \n", sockfd);
    
    // 设置超时时间
    struct timeval timeout;
    // setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    fd_set fds;
    
    // 接收数据
    while (true) {
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        // 监视是否有数据可读，select 函数在有数据可读、发生错误或超时时返回。
        int r = select(sockfd + 1, &fds, nullptr, nullptr, &timeout);
        if (r < 0) {
            perror("select error");
        } else if (r == 0) {
            printf("Timeout, no data received.\n");
        } else {
            printf("Data available to read.\n");
            char buff[BUFFER_SIZE];
            struct sockaddr_in cliaddr;
            memset(&cliaddr, 0, sizeof(cliaddr));
            socklen_t addrLen = sizeof(cliaddr);
            int len = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&cliaddr, &addrLen);
            if (len < 0) {
                std::cerr << "recv error: " << strerror(errno) << std::endl;
                continue;
            } else {
                printf("recv sockfd=%d %d byte, [%s] addrLen=%d, cliIp=%s, cliPort=%d\n",
                        sockfd, len, buff, addrLen, inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
            }
        }

    }
    
    // // 离开多播组
    // setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &group, sizeof(group));
    // 离开源多播组
    setsockopt(sockfd, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, &group, sizeof(group));

    // 关闭
    close(sockfd);
    
    return 0;
}
