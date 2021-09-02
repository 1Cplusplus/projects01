#include "../include/WEB SOCKET SERVER.h"
#include <ws2tcpip.h>
 WSAData wsa;
/*/	CONSTRUCTOR	/*/
web_sock::web_sock() : count(0){
	ws2_32 = true;
	if ( WSAStartup( MAKEWORD(2, 2), &wsa ) != 0){
		printf("StratUp\n");
		ws2_32 = false;
	}
	thread( [this](){ if( ini() == 0 )printf("INI 2 FUERA*****\n"); } ).detach();
	
}
/*/	DESTRUCTOR	/*/
web_sock::~web_sock(){
	WSACleanup();
}
void web_sock::send_close(int con, uint8_t* close_sms ){
	send(con, (char*)close_sms, sizeof close_sms, 0);
	closesocket(con);
	con = 0;
}
int web_sock::ini(){
	if (! ws2_32) return 0;
	
	int sock_main = 0;

	addrinfo info, *pinfo, end_p;
	{
		memset(&info, 0, sizeof info);
		memset(&end_p, 0, sizeof (addrinfo));
		
		info.ai_family		=	AF_INET;
		info.ai_protocol	=	IPPROTO_TCP;
		info.ai_socktype	=	SOCK_STREAM;
	}
	if (( sock_main = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) < 0 )
		{	PRINT_ERROR("Socket") }
	char sock_opt = 1;
	int o = setsockopt(sock_main, SOL_SOCKET, SO_REUSEADDR,  &sock_opt, sizeof(int));
	 if( o != 0 ){ PRINT_ERROR("setopt");}
	if (getaddrinfo("0.0.0.0", "8081", &info, &pinfo) != 0){	PRINT_ERROR("GET info")	}
	if (bind(sock_main, pinfo->ai_addr, sizeof(sockaddr) ) != 0){	PRINT_ERROR("BIND")	}
	if ( listen(sock_main, 5) < 0){	PRINT_ERROR("listen")	}
	// printf("%s\n", inet_ntoa(*(in_addr*)pinfo->ai_addr));
	int sr = 0;
	unsigned long long end_p_size = sizeof(sockaddr);
	
	while(1){
		
		if ( ( sr = accept(sock_main, pinfo->ai_addr, (int*)&end_p_size ) ) < 0 )
		{
			closesocket(sock_main);
			PRINT_ERROR("accept")
		}
		
		/*/												/*/
		/*/	PROCESAR CADA HILO DE CONEXION	/*/
		/*/												/*/
		auto fn = [this](int con){
			count++;
			char buff[1024]{'\0'};
			char respuesta[400]{'\0'};
			recv(con, buff, sizeof buff, 0 );
			saludo(buff, respuesta);
			if( respuesta == NULL){
				
				send(con, (char*) close_sms, 6, 0);
				closesocket(con);
				return 0;
			};
			send(con, respuesta, strlen(respuesta), 0);
			
			uint8_t opt = 0;
			while( con > 0){
				int conexion = recv(con,(char*) &opt, 1, 0);
				// printf("%x\n", opt);
				if (conexion < 0){ printf("ERROR %d  -> %d\n", WSAGetLastError(), con );break;}
				proc_frames(opt, con);
			}
			printf("\ncon: %d CLOSED!\n", count) ;
			count--;
			
		};
		thread(fn, sr).detach();
	}/*/	END MAIN WHILE 	/*/
	closesocket(sock_main);
	
};/*/	END FUNCION INI	/*/
	
	void web_sock:: proc_frames_heads(const int& con, uint64_t& body_size, char* mask_key){
			uint8_t 
				opt 	= 0,
				len 	= 0,
				mask	= 0,
				offset	= 0;
				
			body_size	=	0;
			
			recv(con, (char*)&opt, 1, 0);
			
			len		  = (opt & 0x7f);
			body_size = len;
			
			if ( (opt & 0x80 ) == 0x80) mask = 1;
			if ( len == 126  ) recv(con, (char*)body_size, 2, 0);
			if ( len == 127  ) recv(con, (char*)body_size, 8, 0);;
			// if ( offset > 0  ) body_size			= 0;

			// for (char i = 0; i < offset; ++i)
			// {
			// 	recv(con, (char*)&opt, 1, 0);
			// 	body_size <<= 8;
			// 	body_size += opt;
			// }
			if (mask)
				recv(con, (char*)mask_key, 4, 0);
			else mask_key = NULL;
		}
	/*/										/*/
	/*/	PROCESAR FRAMES PARCIALES	/*/
	/*/										/*/
void web_sock::fragment_frame(int & con, bool ctl_frm){
	
	uint64_t body_size = 0;
	uint8_t mask_key[4]{0};
	
	/*/														/*/
	/*/	PROCESA LAS CABECERAS DE CADA FRAME		/*/
	/*/														/*/
	proc_frames_heads(con, body_size, (char*)mask_key);
	// string file_name	= RUTA;
	// file_name			+= "file";
	// FILE *file			= fopen(file_name.c_str(), "ab");
	
	// uint8_t no_file = 0;
	
	// if (file == NULL){
	// 	no_file = 1;
	// 	printf("file no creado\n");
	// }
	
	uint8_t body[ body_size ]{0};
	recv(con, (char*)body, body_size, 0);
	
	/*/	DESENMASCARAR  	/*/
	if (mask_key != NULL)	
		for (int i = 0; i < body_size; ++i)
			body[ i ] = body[ i ] ^ mask_key[i % 4];
	
	// if ( ! no_file)
	// 	fwrite(body, 1, body_size, file);
	printf("%s\n",body );
	// fclose(file);
}

	/*/										/*/
	/*/	PROCESAR FRAMES COMPLETOS	/*/
	/*/										/*/
void web_sock::complete_frame(int & con,  bool ctl_frm){
	printf("complete_frame\n");
	uint64_t body_size = 0;
	uint8_t mask_key[4]{0};
	
	/*/														/*/
	/*/	PROCESA LAS CABECERAS DE CADA FRAME		/*/
	/*/														/*/
	proc_frames_heads(con, body_size, (char*)mask_key);
	
	uint64_t
		byte_leidos 		= 0,
		total_byte_leidos 	= 0,
		bytes_restantes 	= body_size;
	
	uint8_t body[ 1024*100 ]{0};
	
	while( bytes_restantes > 0 ){
		byte_leidos = recv(con, (char*)body, bytes_restantes, 0);
		total_byte_leidos += byte_leidos;
		bytes_restantes = body_size - total_byte_leidos ;
		
		// printf("body size = %d\n", body_size );
		if (mask_key != NULL){
			for (uint64_t i = 0; i < byte_leidos; ++i)
				body[ i ] = body[ i ] ^ mask_key[ i % 4 ];
		}
		
		// para controlar los codigos en los cierres de conexion
		if (ctl_frm){
			uint16_t close_code = 0;
			for (int i = 0; i < 2; ++i)
			{
				close_code <<= 8;
				close_code  += body[ i ];
			}
			printf("[ %u - %s }\n",close_code, ( body+2 ) );
		
		}else 	printf("[ %s }\n",  body );
		
	}/*/	END WHILE	/*/
	// recv(con,NULL,0,0);
	// send(con, (char*)sms, sizeof( sms ), 0 );
	// closesocket(con);
	// con = 0;
}



void web_sock::control_frames(const uint8_t& opt, int& con){
	switch(opt){
		case 0x88:{
			
			complete_frame( con, true );
			
			send(con, (char*) close_sms, sizeof close_sms, 0);
			
			closesocket(con);
			
			
			// for (int i = 0; i < 6; ++i)
			// 	printf("%d\n",sms[i] );
				
			con = 0;
			
		} break;
	}
}
	/*/										/*/
	/*/	PROCESAR EL HANDSHAKE		/*/
	/*/										/*/
//258EAFA5-E914-47DA-95CA-C5AB0DC85B11
char const* web_sock::saludo(const char*buf_saludo, char *resp){
	
	const short key_len = 60;
	uint8_t sha1_key[20]{'\0'};
	uint8_t key[ key_len ] {'\0'};
	char *ret = resp;
	
	strcpy(ret, u8"HTTP/1.1 101 Switching Protocols\r\n"
				"Connection: Upgrade\r\n"
				"Upgrade: websocket\r\n");
				
	regex_t re1;
	regmatch_t m[2];
	memset(&re1, 0, sizeof re1);
	
	if (regcomp(&re1, "Sec-WebSocket-Key: ([0-9a-zA-Z+/=]+)", REG_EXTENDED) != 0)	{
		printf("ERROR regex COMP\n");
		// getchar();
		ret = NULL;
		return "";
	}
	if (regexec(&re1, buf_saludo, 2, m, 0 ) != 0)	{
		printf("ERROR regex EXEC\n");
		// getchar();
		ret = NULL;
		return "";
	}
	uint32_t len = 1;
	if (re1.re_nsub == 0){
		printf("NO MATCHS\n");
		ret = NULL;	
		return "";
	}

	uint32_t 
		start = m[ 1 ].rm_so,
		fin   = m[ 1 ].rm_eo;
	for(uint32_t i = 0; start != fin; i++, start++)
		// #define substr(str_o, ini, end, out) 
		key[ i ] = buf_saludo[ start  ];
	
	regfree(&re1);
	

	strcat((char*)key, u8"258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
	sha1( key,  key_len, sha1_key );
	for (int i = 0; i < key_len ; ++i)
		key[i] = '\0'; 
	
	 base64_encode((char*)sha1_key, sizeof sha1_key, (char*)key, key_len ) ;
	// printf("%s\n", key );
	strcat(ret, u8"Sec-WebSocket-Accept: " );
	strcat(ret, (char*)key );
	strcat(ret, "\r\n\r\n");
	// printf("%s\n", resp );
	return ret ;
	
	
}
