
void proxy(uint8_t host){
	uint8_t data[ PROXY_HEAD_LEN ]{0};
	memset(data, 0, sizeof (data));
	proxy_auth_methods*p_auth = (proxy_auth_methods*)data;
	if( recv(host, (char*)data, sizeof (data), 0 ) <= 0){
		printf("[[[ conexion cerrada... ERROR:( %d ) ]]]\n",errno/*, WSAGetLastError()*/);
		//closesocket(host);
		close(host);
		return;
	}
	if (data[ PROXY_FIELD_VERS ] != SOCKS_VERS_5){
		//closesocket(host);
		close(host);
		printf("///// Version %d no soportada /////\n", data[ PROXY_FIELD_VERS ]);
		return;
	}
	uint8_t *methods = & data[ PROXY_FIELD_METHODS ];

	for (uint8_t i = 0; i < data[ PROXY_FIELD_N_METHODS ]; ++i)
		switch( methods[i] ){
			case PROXY_AUTH_GSSAPI		:{//  AUN NO SOPORTADO  **************
				// p_auth->nmethods = AUTH_GSSAPI;			
				// i = p_auth->nmethods;
			 } ;break;
			case PROXY_AUTH_USER_PASS	:{//  AUN NO SOPORTADO  **************
				// p_auth->nmethods = AUTH_USER_PASS; 	
				// i = p_auth->nmethods;
			 } ;break;
			case PROXY_AUTH_NONE			:{
				p_auth->nmethods = PROXY_AUTH_NONE;	
				i = p_auth->nmethods;
			 } ;break;
			default:{
				printf("---method no soportado----\n");
				//closesocket(host);
				close(host);
				return;
			}
		}

	proxy_repond_method *resp_method = (proxy_repond_method*)data;
	send(host, (char*)resp_method, sizeof(proxy_repond_method), 0);

	if (resp_method -> method != PROXY_AUTH_NONE)auth_cliente();
	
	uint8_t request_data[ sizeof(proxy_request_ipv6) ]{0};
	if( recv(host, (char*)request_data, PROXY_HEAD_LEN/* = 3*/, 0 )<= 0){
		printf("RECV !@ [[[ conexion cerrada... ERROR: %d]]]\n", errno/*WSAGetLastError()*/);
		return;
	}

	if (request_data[ PROXY_FIELD_VERS ] != SOCKS_VERS_5){
		printf("**** Version %d no soportada ****\n", request_data[ PROXY_FIELD_VERS ]);
		//closesocket(host);
		close(host);
		return;
	}
	uint8_t PROXY_FIELD_PORT, len_addr, domainname[255], size; 
	switch(request_data[ PROXY_FIELD_ATYPE ]){
		case PROXY_IPV4				:{
		//	printf("PROXY_IPV4\n");
			recv(host, (char*)& request_data[PROXY_FIELD_ADDR], PROXY_ADDR_LEN_V4 + PORT_SIZE, 0);
			PROXY_FIELD_PORT 	= PROXY_FIELD_ADDR + PROXY_ADDR_LEN_V4;
			len_addr					= PROXY_ADDR_LEN_V4;
			size = PROXY_HEAD_LEN+PORT_SIZE+PROXY_ADDR_LEN_V4;
	};break;
		case PROXY_IPV6				:{
			printf("PROXY_IPV6 \n");
			recv(host, (char*)& request_data[PROXY_FIELD_ATYPE + 1], PROXY_ADDR_LEN_V6 + PORT_SIZE, 0);
			PROXY_FIELD_PORT = PROXY_FIELD_ADDR + PROXY_ADDR_LEN_V6;
			len_addr					= PROXY_ADDR_LEN_V6;
		};break;
		case PROXY_DOMAINNAME	:{
			printf("PROXY_DOMAINNAME\n");
			
		};break;
	}
	uint8_t host_target = connect_client_target(request_data, PROXY_FIELD_PORT, len_addr);
	if (host_target == 0)	{
		close(host);
		return ;
	}
	uint8_t rpl_data[ size ];
	uint32_t addr = inet_addr("10.0.0.152");
	uint16_t port = htons(8081);
	memcpy(rpl_data, request_data, sizeof(rpl_data) );
	rpl_data[PROXY_FIELD_RPL] = 0;
	// memcpy(&rpl_data[ PROXY_FIELD_ADDR ], &addr, len_addr );
	// memcpy(&rpl_data[ PROXY_FIELD_PORT ], &port, PORT_SIZE );

	uint8_t tmp_buff[1024*5] = "the best";
	send(host, rpl_data, size,0);
	while(host_target ){
		if( recv(host, tmp_buff, sizeof( tmp_buff ) , 0) <= 0){
			printf("XXX ERROR %d\n",errno);
			close(host);
			close(host_target);
			return;
		} 
		if (send(host_target, tmp_buff,sizeof(tmp_buff),0) <0 )		{
			printf("SEND 1 XXX ERROR %d\n", errno);
			close(host);
			close(host_target);
			return;
		} 
		if( recv(host_target, tmp_buff, sizeof( tmp_buff ) , 0) <= 0){
			printf("REC 2 XXX ERROR %d\n", errno);
			close(host);
			close(host_target);
			return;
		}
		if (send(host, tmp_buff,sizeof(tmp_buff),0) <0 ){
			printf("SEND 2 XXX ERROR %d \n", errno);
			close(host);
			close(host_target);
			return;
		}  

	}
	printf("SOCKS V%d\n",request_data[ PROXY_FIELD_VERS ]);
	close(host);	
	close(host_target);	
/*/
///	FIN PROXY
/*/
}
uint8_t connect_client_target(uint8_t * info_connect, uint8_t & FIELD_PORT, uint8_t&len_addr){
	sockaddr ct;//ct: client target
	memset(&ct, 0, sizeof(sockaddr));
	ct.sa_family 	= AF_INET;

	memcpy(ct.sa_data, & info_connect[FIELD_PORT], PORT_SIZE);
	memcpy(&ct.sa_data[PORT_SIZE]	,& info_connect[PROXY_FIELD_ADDR]	, len_addr);
	
	printf("%u\n",ntohs(*(uint16_t*)&info_connect[FIELD_PORT] ) );
	
	uint8_t client_target = socket(AF_INET, SOCK_STREAM	, IPPROTO_TCP);
	if (client_target < 0)
	{
		printf("ERROR: %d socket to client_target\n", errno);
		return 0;
	}
	if(connect(client_target, &ct, sizeof(sockaddr))  <  0 ){
		printf("ERROR: %d connect to client_target\n", errno/*WSAGetLastError()*/);
		//closesocket(client_target);
		close(client_target);
		return 0;
	}
	printf("CONNECTED....\n");
	return client_target;
}
