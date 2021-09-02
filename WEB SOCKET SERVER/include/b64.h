#ifndef _B64_H_
#define _B64_H_
#include <cctype>
#include <cstring>
// #include <string>
#include <cstdio>
using namespace std;

#ifdef DLL_ON
#	ifdef CREAR_B64_DLL
#		define IO_B64_DLL __declspec(dllexport)
#	else 
#		define IO_B64_DLL __declspec(dllimport)
#	endif/*/	_B64_DLL_	/*/
#endif

#ifdef NO_DLL
    #define IO_B64_DLL ;
#endif/*/   DLL    /*/

#ifdef __cplusplus
	extern "C"{ 
#endif
IO_B64_DLL const char*  base64_encode(char const* , unsigned int len, char*o);
IO_B64_DLL const char*  base64_decode(char const* s);

#ifdef __cplusplus
}
#endif
		
// #ifdef DLL_ON
// #endif

#endif /*/	_B64_H_	/*/