#ifndef __WEB_SOCK__
#define __WEB_SOCK__
#include "../include/regex.h"
#include "../include/sha1.h"
#include "../include/base64.h"

// #include <windows.h>
#include <thread>
// #include <chrono>
using namespace std;

#define PRINT_ERROR(err) printf("%s -> %d\n", err, WSAGetLastError());getchar();return 0;

		
class web_sock{
 	private:
 		char ws2_32;
 		char count;
 		
 		uint8_t close_sms[6]{0x88, 4, 0x03, 0xe8, 'H', 's'};
		char const*saludo(char const*, char*);
		// std::string hex2dec(const std::string & hex){
		// 	std::string ret = "";
		// 	char dec[ hex.length() ];
		// 	size_t len=  hex.length();
			
		// 	for (uint8_t i = 0; i < len; i++ )
		// 		switch( hex[ i ] ){
		// 			case '0':{ dec[ i ] = 0 ;};break;
		// 			case '1':{ dec[ i ] = 1 ;};break;
		// 			case '2':{ dec[ i ] = 2 ;};break;
		// 			case '3':{ dec[ i ] = 3 ;};break;
		// 			case '4':{ dec[ i ] = 4 ;};break;
		// 			case '5':{ dec[ i ] = 5 ;};break;
		// 			case '6':{ dec[ i ] = 6 ;};break;
		// 			case '7':{ dec[ i ] = 7 ;};break;
		// 			case '8':{ dec[ i ] = 8 ;};break;
		// 			case '9':{ dec[ i ] = 9 ;};break;
		// 			case 'a':{ dec[ i ] = 10 ;};break;
		// 			case 'b':{ dec[ i ] = 11 ;};break;
		// 			case 'c':{ dec[ i ] = 12 ;};break;
		// 			case 'd':{ dec[ i ] = 13 ;};break;
		// 			case 'e':{ dec[ i ] = 14 ;};break;
		// 			case 'f':{ dec[ i ] = 15 ;};break;
		// 		}
				
		// 	for (char i = 0; i < len; ++i)		
		// 		ret += dec[i++] * 16 + dec[i];

		// 	return ret;
		// }
		void send_close(int, uint8_t*);
		void complete_frame(int &conexion, bool ctl_frame = false);
		void fragment_frame(int &conexion, bool ctl_frame = false);
		void control_frames(const uint8_t &opcion, int &conexion);
		void proc_frames_heads(const int& con, uint64_t& body_size, char* mask_key);
		
		
		void proc_frames(const uint8_t& opt, int& con){
			switch(opt)
			{
				case 0x88:  
				case 0x8A:  
				case 0x89: 	control_frames(opt, con);	break;
				case 0x82:  
				case 0x81:	complete_frame(con); 		break;
				case 0x00:	
				case 0x01:	
				case 0x02:	
				case 0x80:	fragment_frame(con); 		break;
				default:{
					send_close(con, close_sms );
					printf("no opt*** %x\n", opt );
				}
				
			}
		}
	public:
		web_sock();
		~web_sock();
		int ini();
};/*/	END CLASS WEB_SOCK	/*/

#endif //__WEB_SOCK__