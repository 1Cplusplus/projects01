 

uint8_t iso_boot(){
	printf("iso boot \n");
	uint8_t boot_catalog[ 64 ];
	memset(boot_catalog, 0, sizeof( boot_catalog ) );
	fseek(iso_file, (*(uint32_t*)bvd_p->bvd_lba_catalog) * ISO_BLOCK_SIZE, SEEK_SET);
	fread(boot_catalog, 64, 1, iso_file);
	
	bve_p = (boot_validation_entry*)boot_catalog;
	bie_p = (boot_initial_entry*)(boot_catalog + 32);
	printf("%x\n", /**/*(uint32_t*) bie_p->load_rba);
	// uint8_t boot_data = new uint8_t[ISO_BLOCK_SIZE*2u];
	// // fseek(iso_file,ISO_BLOCK_SIZE, SEEK_SET);
	// // fread(boot_data,ISO_BLOCK_SIZE,1, iso_file  );
	// if (boot_data[0] != ISO_DESC_TERMINATOR){
	// 	fread(boot_data,ISO_BLOCK_SIZE,1, iso_file );
	// }
	
	
	return 0;
}