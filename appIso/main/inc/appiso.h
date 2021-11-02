#ifndef APPISO_H
#define APPISO_H 


#include <cstdio>
#include <string.h>
#include <cinttypes> 
#include <cstdlib> 
#include <wchar.h>

#include "../../inc/iso.h"
#include "../../boot/inc/boot.h"
#include "../../formats/udf/inc/udf.h"

uint32_t count_lba = 19;

void make_desc(const uint8_t&, void*);
void make_supplementary(supplementary_vol_desc*);
void record_files(FILE*, dir_record*, uint32_t& );
#endif
