
//						*****									BOOT CATALOG 											***************
struct boot_validation_entry{
	uint8_t head_id;
	uint8_t platform_id;//0 = 80x86, 1 = Power PC, 2 = MAC
	uint16_t rsv;
	uint8_t id_string[24];
	uint16_t chk_sum;
	uint16_t boot_mark;// 0X55AA
	 
}*bve_p;
struct boot_initial_entry{
	uint8_t boot_indicator;// 0x88 == booteable, 0x00 = not booteable
	uint8_t boot_media_type;
	uint8_t load_sement[2];
	uint8_t sys_type;
	uint8_t unused;
	uint8_t sector_count[2];
	uint8_t load_rba[4];
	uint8_t unused2[20];
}*bie_p;
//						*****									END BOOT CATALOG 											***************