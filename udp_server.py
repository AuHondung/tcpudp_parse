import socket
import struct

multicast_group = '239.255.255.2'
server_port = 30094
buff_size = 1500
multicast_ttl = 2

# s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

# 允许端口重用
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('172.20.2.1', server_port))

# 设置组播接口
s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton('172.20.2.1'))
# s.bind(('127.0.0.1', server_port))
# s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton('127.0.0.1'))

# 设置组播TTL值
s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, multicast_ttl)

# 加入组播组
mreq = struct.pack("4sl", socket.inet_aton(multicast_group), socket.INADDR_ANY)
s.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

print("Listening on port %d" % server_port)

while True:
    print("Waiting for data...")
    data, addr = s.recvfrom(buff_size)
    print("Data received!")  

    