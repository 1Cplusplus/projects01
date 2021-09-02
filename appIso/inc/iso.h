#include <cstdio>
#include <string.h>
#include <cstdint>
 
#ifndef ISO_H
#define ISO_H

#define ISO_DESC_BOOT 					0u
#define ISO_DESC_PRIMARY 				1u
#define ISO_DESC_SUPPLEMENTARY 	2u
#define ISO_DESC_PARTITION 			3u
#define ISO_DESC_TERMINATOR 		255u
#define ISO_BLOCK_SIZE 					2048u
#define ISO_VOL_SYS_AREA_SIZE 	2048*16u


struct vol;
struct attr_files;
struct dir_record;
struct fin_vol_desc;
struct primary_vol_desc; 
struct supplementary_vol_desc;

 //extern vol;
 extern dir_record *dir_record_p;
 //extern fin_vol_desc;
 extern supplementary_vol_desc *svd_p;
 extern primary_vol_desc *pvd_p;


extern bool joliet;
extern FILE*iso_file; 

char iso_joliet(const uint32_t&lba = 0,const uint32_t&size = 0,const uint8_t&isDir = 0);
char show_dir(const uint8_t* );
char iso9660( const uint32_t&lba = 0,const uint32_t&size = 0,const uint8_t&isDir = 0  );


#endif

