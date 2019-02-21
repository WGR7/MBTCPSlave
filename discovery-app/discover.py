import socket

# current ip of client
my_ip = socket.gethostbyname(socket.gethostname())

# prepare socket
source_port = 1235
dest_port = 1234
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
client.bind((my_ip, source_port))

# greetings
data1 = bytearray("hello",'utf-8')
# real data
data2 = bytearray(6)
data2[0] = int(my_ip.split('.')[0])
data2[1] = int(my_ip.split('.')[1])
data2[2] = int(my_ip.split('.')[2])
data2[3] = int(my_ip.split('.')[3])
data2[4] = source_port >> 8
data2[5] = source_port & 0x00ff

# send frame
client.sendto(data1+data2, ("<broadcast>", dest_port))