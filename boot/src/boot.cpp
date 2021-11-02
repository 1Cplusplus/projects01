 
//  EL TORITO SPECIFICATION
uint8_t iso_boot(){
	// fprintf(stderr, "iso boot \n");
	uint8_t boot_catalog[ 64 ];
	memset(boot_catalog, 0, sizeof( boot_catalog ) );
	fseek(iso_file, (*(uint32_t*)bvd_p->bvd_lba_catalog) * ISO_BLOCK_SIZE, SEEK_SET);
	fread(boot_catalog, 64, 1, iso_file);
	
	
	bve_p = (boot_validation_entry*)boot_catalog;
	bie_p = (boot_initial_entry*)(boot_catalog + 32); 
	
	if( *(uint16_t*)bve_p->bve_boot_mark == 0xAA55u ) printf("Booteable \n");
	
	return 0;
}

void make_boot(boot_vol_desc*bd){
	
	//======================================== BOOT RECORD ==========================================
	// boot_vol_desc bd;
	bd->bvd_vdt = ISO_DESC_BOOT; // Volume Descriptor Type
	memcpy(bd->bvd_stdId,"CD001", 5);
	bd->bvd_version = 0x01;
	memcpy(bd->bvd_boot_sys_id,"EL TORITO SPECIFICATION",23);memset(&bd->bvd_boot_sys_id[23], 0, 9 );
	memset(bd->bvd_unused,0,32);
	*(uint32_t*)bd->bvd_lba_catalog = 0x00000013u;
	memset(bd->bvd_unused2,0,1973);
	//===============================================================================================
}
void make_boot_catalog(boot_validation_entry&bve, boot_initial_entry&bie){
	

	//=================================== BOOT VALIDATION ENTRY =====================================
	// boot_validation_entry bve ;
	bve.bve_head_id 		= 1;
	bve.bve_platform_id = 0;//0 = 80x86, 1 = Power PC, 2 = MAC
	memset(bve.bve_rsv, 0, 2);
	memcpy(bve.bve_id_string,"B1NAR10", 7); memset(&bve.bve_id_string[7], 0, 17);
	memset(bve.bve_chk_sum, 0, 2);
	*(uint16_t*)bve.bve_boot_mark = 0xAA55;
	//===============================================================================================

	//===================================== BOOT INITIAL ENTRY ======================================
	// boot_initial_entry bie ;
	bie.bie_boot_indicator	=	0x88u;// 0x88 = booteable, 0x00 = not booteable
	bie.bie_boot_media_type = 0x00u;
	bie.bie_load_sement 		= 0x0000u;
	bie.bie_sys_type 				= 0;
	bie.bie_unused 					= 0;
	bie.bie_sector_count		= 0x0001u;
	bie.bie_load_rba 				= 0x00000015u;//20 en decimal
	memset(bie.bie_unused2, 0, 20);
		
	//===============================================================================================
}
