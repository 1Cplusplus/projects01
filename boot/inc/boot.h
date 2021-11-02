#ifndef _ISO_BOOT_
#define _ISO_BOOT_

//						*****									BOOT CATALOG 											***************
struct boot_validation_entry{
	uint8_t bve_head_id;
	uint8_t bve_platform_id;//0 = 80x86, 1 = Power PC, 2 = MAC
	uint8_t bve_rsv[2];
	uint8_t bve_id_string[24];
	uint8_t bve_chk_sum[2];
	uint8_t bve_boot_mark[2];// 0X55AA
	 
}*bve_p;
struct boot_initial_entry{
	uint8_t bie_boot_indicator;// 0x88 == booteable, 0x00 = not booteable
	uint8_t bie_boot_media_type;
	uint16_t bie_load_sement;
	uint8_t bie_sys_type;
	uint8_t bie_unused;
	uint16_t bie_sector_count;// NUMERO DE SECTORES QUE LA BIOS LEERA DE LA BOOT IMAGE
	uint32_t bie_load_rba;
	uint8_t bie_unused2[20];
}*bie_p;
//						*****									END BOOT CATALOG 											***************

void make_boot(boot_vol_desc*);
void make_boot_catalog(boot_validation_entry&, boot_initial_entry&);

#endif