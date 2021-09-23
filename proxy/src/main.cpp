#include "../include/main.h"

int main(int argc, char const *argv[])
{

	// if (WSAStartup(_2_2 ,&ws ) )
	// {
	// 	printf("error Startup\n");
	// 	return 0;
	// }
	sockaddr addr, client_info;
	memset(&addr, 0, sizeof( sockaddr ) );
	memset(&client_info, 0, sizeof( sockaddr ) );

	addr.sa_family 	= AF_INET;
	uint16_t port 	=  htons(8081);
	uint32_t dir 		=  inet_addr("10.0.0.152");
	memcpy( addr.sa_data, &port , PORT_SIZE );
	memcpy( &addr.sa_data[PORT_SIZE], &dir, PROXY_ADDR_LEN_V4 ) ;
	
	uint8_t main_proxy;
	if ((main_proxy = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) < 0 )
	{
		printf("ERROR CREATE SOCKET\n");
		//WSACleanup();
		//closesocket(main_proxy);
		close(main_proxy);
		getchar();
		return 0;
	}
	setsockopt(main_proxy, SOL_SOCKET, SO_REUSEADDR, (char*)1, 1);
	
	if (bind(main_proxy, &addr, sizeof(sockaddr)) < 0)
	{
		printf("ERROR BIND %d\n", errno);
		// closesocket(main_proxy);
		// WSACleanup();
		close(main_proxy);
		getchar();
		return 0;
	}
	listen(main_proxy, 10);
	while(true){
	printf("\nwaiting new connections...\n");

		uint8_t client_host = accept(main_proxy, NULL, 0 );
		if ( client_host < 0 )
		{
			printf("ERROR ACCEPT %d\n", errno);
			close(main_proxy);
			//closesocket(main_proxy);
			//WSACleanup();
			getchar();
			return 0;

		}
		
		std::thread(proxy,client_host).detach();
	/*/
	/// FIN WHILE
	/*/
	}
	
	//closesocket(main_proxy);
	//WSACleanup();
	close(main_proxy);
	getchar();
	return 0;
/*/
/// FIN	FUNCION MAIN
/*/
}
#include "proxy.cpp"