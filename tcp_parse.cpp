#include <iostream>
#include <cstring>      // for memset()
#include <sys/socket.h> // for socket functions
#include <arpa/inet.h>  // for inet_addr() and htons()
#include <unistd.h>     // for close()

#define PORT 30123      // 服务器端口
#define BUFFER_SIZE 1500 // 缓冲区大小


int main() {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "172.20.2.82", &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    } else {
    }
        std::cout << "Connecting to server successfully !" << std::endl;

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
    } else {
        std::cout << "Connecting to radar successfully !" << std::endl;
    }
    
    unsigned char buff[BUFFER_SIZE];
    int len = recv(sockfd, buff, BUFFER_SIZE, 0);
    if (len < 0) {
        printf("recv error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    } else if (len = 0) {
        printf("Connection closed !!!\n");
        return -1;
    } else {
        printf("Received %d bytes: %s\n", len, buff);
    }
    
    close(sockfd);

    return 0;
}