#include "../include/main.h"


int main(int argc, char const *argv[]){
	sockaddr server_inf;
	memset(&server_inf, 0, sizeof(sockaddr));
	
	server_inf.sa_family 	= AF_INET;
	uint16_t port					=	htons(21);
	uint32_t addr					=	INADDR_ANY;
	
	memcpy(server_inf.sa_data, &port, 2 );
	memcpy(&server_inf.sa_data[2], &addr, 4 );
	
	uint8_t ftp_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ftp_server< 0 ){
		printf("SOCKET... ERROR: %d\n", errno);
		getchar();
		return 0;
	}
	
	setsockopt(ftp_server, SOL_SOCKET, SO_REUSEADDR, (char*)true, 1);
	
	if ( bind(ftp_server, &server_inf, sizeof(sockaddr)) < 0)	{
		printf("BIND... ERROR: %d\n", errno);
		close(ftp_server);
		getchar();
		return 0;
	}
	if (listen(ftp_server, 10) < 0 ){
		printf("LISTEN... ERROR: %d\n", errno);
		close(ftp_server);
		getchar();
		return 0;
	}
	close(ftp_server);
	ftp_server = 0;
	
	while( ftp_server ){
		uint8_t remote_host = accept(ftp_server,NULL, NULL);
		if (remote_host < 0){
		printf("ACCEPT... ERROR: %d\n", errno);
		close(ftp_server);
		getchar();
		}
		
		
/*/////////		///////////
//////// END WHILE ///////
*/////////	 ////////////
	}
		printf("%c %c\n",65,97 );
	
	printf("conexion principal finalizada.\n");
	return 0;
}

#include "./ftp.cpp"