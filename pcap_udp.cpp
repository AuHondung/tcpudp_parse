#include <iostream>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <net/ethernet.h> // 包含以太网头部定义

// 回调函数，当捕获到数据包时会被调用
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    // 获取 IP 头部
    int len = sizeof(struct ether_header);
    std::cout << "len: " << len << std::endl;
    struct ip *ip_header = (struct ip *)(packet + len + 4); // 跳过以太网头部
    
    struct udphdr *udp_header = (struct udphdr *)(packet + len + 4 + ip_header->ip_hl * 4); // 跳过IP头部 + vlen头部（4）

    // 打印源IP和目的IP地址
    printf("Source IP: %s\n", inet_ntoa(ip_header->ip_src));
    printf("Destination IP: %s\n", inet_ntoa(ip_header->ip_dst));

    // 打印源端口和目的端口
    printf("Source Port: %d\n", ntohs(udp_header->uh_sport));
    printf("Destination Port: %d\n", ntohs(udp_header->uh_dport));

    // 获取UDP负载数据长度
    int udp_payload_len = ntohs(udp_header->uh_ulen) - sizeof(struct udphdr);
    
    // 获取UDP负载数据的起始位置
    const u_char *payload = (u_char *)(packet + len + 4 + ip_header->ip_hl * 4 + sizeof(struct udphdr));

    // 打印捕获到的数据包长度
    printf("Captured packet length: %d bytes\n", pkthdr->len);

    // 打印UDP负载数据
    printf("UDP Payload (%d bytes):\n", udp_payload_len);
    for (int i = 0; i < udp_payload_len; i++) {
        printf("%02x ", payload[i]);                     // 按16进制打印负载数据
        if ((i + 1) % 16 == 0) printf("\n");
    }
    
    printf("\n");
    

    printf("--------------------------------------------------\n");
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];  // 用于存储错误信息
    pcap_t *handle;                 // 捕获句柄
    struct bpf_program fp;          // 编译过滤规则
    char filter_exp[] = "udp dst port 30094";      // 过滤表达式
    bpf_u_int32 net;                // 网络地址

    // 打开指定的网络接口 eno1
    handle = pcap_open_live("eno1", BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device eno1: %s\n", errbuf);
        return 2;
    }

    // 编译过滤器
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }

    // 设置过滤器
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }

    // // 开始捕获数据包
    // pcap_loop(handle, 0, packet_handler, NULL);

    while (true) {
        struct pcap_pkthdr header;    // 数据包头
        const u_char *packet = pcap_next(handle, &header);  // 捕获的数据包

        if (packet == NULL) {
            printf("Timeout or error while capturing a packet\n");
            continue;
        }

        // 获取以太网头部的长度
        int len = sizeof(struct ether_header);
        std::cout << "len: " << len << std::endl;

        // 获取 IP 头部，跳过以太网头部
        struct ip *ip_header = (struct ip *)(packet + len + 4); // 跳过vlan头部（4）

        // 获取 UDP 头部
        struct udphdr *udp_header = (struct udphdr *)(packet + len + 4 + ip_header->ip_hl * 4); // 跳过IP头部

        // 打印源IP和目的IP地址
        printf("Source IP: %s\n", inet_ntoa(ip_header->ip_src));
        printf("Destination IP: %s\n", inet_ntoa(ip_header->ip_dst));

        // 打印源端口和目的端口
        printf("Source Port: %d\n", ntohs(udp_header->uh_sport));
        printf("Destination Port: %d\n", ntohs(udp_header->uh_dport));

        // 获取UDP负载数据长度
        int udp_payload_len = ntohs(udp_header->uh_ulen) - sizeof(struct udphdr);
        
        // 获取UDP负载数据的起始位置
        const u_char *payload = (u_char *)(packet + len + 4 + ip_header->ip_hl * 4 + sizeof(struct udphdr));

        // 打印捕获到的数据包长度
        printf("Captured packet length: %d bytes\n", header.len);

        // 打印UDP负载数据
        printf("UDP Payload (%d bytes):\n", udp_payload_len);
        for (int i = 0; i < udp_payload_len; i++) {
            printf("%02x ", payload[i]); // 按16进制打印负载数据
            if ((i + 1) % 16 == 0) printf("\n");
        }

        printf("\n--------------------------------------------------\n");
    }

    // 关闭句柄
    pcap_close(handle);
    return 0;
}
