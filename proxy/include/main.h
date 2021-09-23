#ifndef	 __MY__PROXY
#	define __MY__PROXY
#include <cstdio>
#include <cstdint>
#include <thread>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>


//#include <windows.h>

//#define _2_2			0x0202

#define PORT_SIZE	2

#define SOCKS_VERS_5 5

#define PROXY_FIELD_VERS						0
#define PROXY_FIELD_CMD							1
//#define PROXY_FIELD_RSV							2
#define PROXY_FIELD_ATYPE						3
#define PROXY_FIELD_ADDR						4
#define PROXY_FIELD_RPL							PROXY_FIELD_CMD
#define PROXY_FIELD_N_METHODS				PROXY_FIELD_CMD
#define PROXY_FIELD_METHODS					PROXY_FIELD_CMD + 1

#define PROXY_HEAD_LEN 			4

#define PROXY_AUTH_NONE			 0
#define PROXY_AUTH_GSSAPI		 1
#define PROXY_AUTH_USER_PASS 2

#define PROXY_CONNECT				1
#define PROXY_BIND					2
#define PROXY_UDP_ASSOCIATE 3

#define PROXY_IPV4 				1
#define PROXY_DOMAINNAME 	3
#define PROXY_IPV6	 			4

#define PROXY_ADDR_LEN_V4	 		4
#define PROXY_ADDR_LEN_V6	 		16


struct proxy_auth_methods{
	uint8_t ver,
					nmethods;

};
struct proxy_repond_method{
	uint8_t ver,
					method;
};

/*/
///	SOPORTE PARA IPV6
/*/
struct proxy_request_ipv6{
	uint8_t 	pr6_ver,
						pr6_cmd,
						pr6_rsv,
						pr6_atype,
					 	pr6_dst_addr_ipv6[16];
	uint16_t 	pr6_dst_port;
};
struct proxy_replay_ipv6{
	uint8_t 	pr6_ver,
						pr6_rep,
						pr6_rsv,
						pr6_atype,
					 	pr6_bnd_addr_ipv6[16];
	uint16_t 	pr6_bnd_port;
};

/*/
///	SOPORTE PARA IPV4
/*/
struct proxy_request{
	uint8_t 	pr_ver,
						pr_cmd,
						pr_rsv,
						pr_atype;
	uint32_t 	pr_dst_addr;
	uint16_t 	pr_dst_port;
};
struct proxy_replay{
	uint8_t 	pr_ver,
						pr_rep,
						pr_rsv,
						pr_atype;
	uint32_t 	pr_bnd_addr;
	uint16_t 	pr_bnd_port;
};
//WSADATA ws;
void proxy(uint8_t );
void (*auth_cliente)();
uint8_t connect_client_target(uint8_t*, uint8_t&, uint8_t&);
#endif //__MY__PROXY