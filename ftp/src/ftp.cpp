

uint8_t server_pi(uint8_t con_cntl){
	
	while(con_cntl){
		uint8_t ordenes[1024];
///////////////	RECIBE ORDENES FTP /////////////////////////////////
/**/if( recv(con_cntl, ordenes, sizeof(ordenes),0) <= 0){				 //
/**/	printf("\n[[ RECV'1 ERROR: (%d) ]]\n",errno );						//
/**/	close(con_cntl);																				 //
/**/	getchar();																						  //
/**/	return 0;																							 //
/**/}																												//
/////////////////////////////////////////////////////////////		
		
	}
	return 0;
}