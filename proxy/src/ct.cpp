#include <cstdio>
#include <windows.h>
#include <cstdint> 

WSADATA ws;
int main(int argc, char const *argv[])
{
	uint8_t data[3]{
		5,
		1,
		0,
	};
	
	if (WSAStartup(0x0202 ,&ws ) )
	{
		printf("error Startup\n");
		return 0;
	}
	sockaddr addr, server_info;
	memset(&addr, 0, sizeof( addr ) );
	memset(&server_info, 0, sizeof( server_info ) );

	addr.sa_family 	= AF_INET;
	uint16_t port 	=	 htons(8081 );
	uint32_t dir 		=  inet_addr("10.0.0.120");

	memcpy( addr.sa_data, &port , 2 );
	memcpy( &addr.sa_data[2], &dir, 4 );
	
	uint8_t main_proxy;
	if ((main_proxy = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) < 0 )
	{
		printf("ERROR CREATE SOCKET\n");
		WSACleanup();
		closesocket(main_proxy);
		getchar();
		return 0;
	}
	if (connect(main_proxy, &addr, sizeof(sockaddr)) != 0)
	{
		printf("ERROR: %d connect to client_target\n", WSAGetLastError());
		getchar();
		return 0;
	}
	uint8_t resp_serv[10]{
		5,
		1,
		0,
		1
	};
	port = htons(8082);
	dir = inet_addr("10.0.0.120");
	memcpy(&resp_serv[4],&dir, 4);
	memcpy(&resp_serv[4+4],&port, 2);
	
	printf("esperando datos...\n");
	if (send(main_proxy, (char*)data, sizeof(data), 0) < 0){
		printf("SEND I ERROR: %d\n", errno);
		closesocket(main_proxy);
		getchar();
		return 0;
	}
	uint8_t resp_recv = recv(main_proxy, (char*)data, sizeof(data), 0);
	if( resp_serv <= 0){
		printf("error %d\n",WSAGetLastError() );
		closesocket(main_proxy);
		getchar();
		return 0;
	}
	if( send(main_proxy, (char*)resp_serv, sizeof(resp_serv), 0)< 0){
		printf("SEND II ERROR: %d\n", errno);
		closesocket(main_proxy);
		getchar();
		return 0;
	}
	printf("sent in port %x\n",*(uint16_t*)&resp_serv[4+4] );
	getchar();
	return 0;
}