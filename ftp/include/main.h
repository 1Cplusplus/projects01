#ifndef	 __MY__FTP
#	define __MY__FTP

#include <cstdio>
#include <cstdint>
#include <thread>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

uint8_t server_pi(uint8_t);
uint8_t server_dtp(uint8_t&);


#endif // __MY__FTP