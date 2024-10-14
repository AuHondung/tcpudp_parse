#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    // 创建套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // 设置多播TTL参数
    unsigned char ttl = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        std::cerr << "Error setting multicast TTL" << std::endl;
        return -1;
    }

    // 设置目标地址
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("239.255.255.2"); // 多播地址
    addr.sin_port = htons(30094); // 目标端口

    // 发送数据
    std::string msg = "Hello, Multicast!";
    while(true) {
        if (sendto(sock, msg.c_str(), msg.size(), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Error sending data" << std::endl;
            return -1;
        }
        std::cout << "Data sent to multicast group" << std::endl;
    }


    return 0;
}

