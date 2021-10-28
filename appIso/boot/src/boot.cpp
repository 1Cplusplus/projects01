 
//  EL TORITO SPECIFICATION
uint8_t iso_boot(){
	// fprintf(stderr, "iso boot \n");
	uint8_t boot_catalog[ 64 ];
	memset(boot_catalog, 0, sizeof( boot_catalog ) );
	fseek(iso_file, (*(uint32_t*)bvd_p->bvd_lba_catalog) * ISO_BLOCK_SIZE, SEEK_SET);
	fread(boot_catalog, 64, 1, iso_file);
	
	bve_p = (boot_validation_entry*)boot_catalog;
	bie_p = (boot_initial_entry*)(boot_catalog + 32); 
	
	printf("bvd_p->bvd_lba_catalog %u\n", /**/*(uint32_t*) bvd_p->bvd_lba_catalog);
	
	
	printf("bie_load_rba %u\n", /**/ bie_p->bie_load_rba);
	printf("bie_boot_indicator %u\n", /**/ bie_p->bie_boot_indicator);
	printf("bve_boot_mark %X\n", /**/ *(uint16_t*)bve_p->bve_boot_mark);

	
	return 0;
}