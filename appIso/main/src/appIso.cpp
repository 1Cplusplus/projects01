#include "../inc/appiso.h"
#include "../../formats/joliet/src/joliet.cpp"
#include "../../boot/src/boot.cpp"
#include "../../formats/udf/src/udf.cpp"

int main(int argc, char const *argv[]){
	
	if (argc < 2 ){
		make_iso();
		printf("aaa\n");
		fprintf(stderr,"Proporcione el archivo iso a leer");
		getchar();
		return 0;
	} 
	
	// switch( GetFileAttributesA(argv[1]) ){
	// 	case FILE_ATTRIBUTE_ARCHIVE:{
	// 		printf("%s is a file\n",argv[1] );
	// 	};break;
	// 	case FILE_ATTRIBUTE_DIRECTORY:{
	// 		printf("%s is a dir \n",argv[1] );
	// 	};break;
	// }	
	// if (argv[1] == 'c' || argv[1] == 'C' ){
	// 	create_iso_file();
	// }else{
	// 	look_iso_file();
	// }
	iso_file = fopen(argv[1],"rb"); 
	if (iso_file == NULL)	{ 
		fprintf(stderr,"No file");
		// getchar();
		return 0; 
	} 
	
	uint32_t lba ;
	uint32_t size;
	uint8_t  isDir;
	if (  argv[ 2 ] != NULL ){
		lba		= atoi( argv[3] );
		size 	= atoi( argv[4] );
		isDir	= atoi( argv[5] );
	}
	uint8_t count_desc = 0;
	/*/
	 * desc_file:	ESPACIO EN EL CUAL SE ALMACENAN TODOS LOS DESCRICTORES DEL ARCHIVO ISO
	/*/
	uint8_t *desc_file = 	load_descriptors(count_desc/*, desc_file*/ );
	
// udf_desc_file: PARA COMPROBAR SI EL SISTEMA DE ARCHIVO ES UDF
	uint8_t * udf_desc_file = new uint8_t[ISO_BLOCK_SIZE ];
	fread(udf_desc_file, 6,1,iso_file);// AVANZA 6 BYTE EN BUSQUEDA DEL "BEGINIG EXTENDED AREA"
	
// udf_bea01_p: PUNTERO A LA "BEGINIG EXTENDED AREA"
	udf_bea01_p = (bea01*)udf_desc_file;
	
// BEA01:"BEGINIG EXTENDED AREA" 	
	if(memcmp(BEA01, udf_bea01_p->std_id, 3) == 0){
		fprintf(stderr,"la app no maneja archivos UDF\n");
		delete[] udf_desc_file;
		delete[] desc_file;
		fclose (iso_file);
		getchar();
		return 0;
	}
	fseek(iso_file, -6, SEEK_CUR);	// LO CONTRARIO A LA LINEA 50
	
	
	if (desc_file == NULL){
		fprintf(stderr,"MAIN: no memory");
		// getchar();
		return 0;
	} 
	// setbuff(iso_file, desc_file)
	
	if (desc_file[0] != ISO_DESC_PRIMARY){
		fprintf(stderr, "Formato iso desconocido");
		delete[] desc_file;
		// getchar();
		return 0;
	} 
	pvd_p = (primary_vol_desc*)desc_file;
	// printf(" PrimaryVolumeDescriptor %4s\n",pvd_p->pvd_stdId ); 
	// printf("vol set id %s\n",pvd_p->pvd_vol_set_id );
	// printf("pvd_copyriht_file_id %s\n",pvd_p->pvd_copyriht_file_id );
	// printf("pvd_abstract_file_id %s\n",pvd_p->pvd_abstract_file_id );
	// printf("pvd_bibliographic_file_id %s\n",pvd_p->pvd_bibliographic_file_id );
	// printf("pvd_path_table_size %u\n", *(uint32_t*)pvd_p->pvd_path_table_size );
	
	// uint64_t vol_space = ((*(uint32_t*)pvd_p->pvd_vol_space_size) )*  ISO_BLOCK_SIZE;
	// printf("volumen space size %llu\n",vol_space ) ;
	for (uint8_t i = 1; i < count_desc; ++i){
/*/
 * SE COMPRUEBA EL TIPO DEL SEGUNDO DESCRIPTOR.
/*/ 
		switch ( desc_file[ i * ISO_BLOCK_SIZE ]  ){
			case ISO_DESC_BOOT:{ 
				bvd_p = (boot_vol_desc*)(desc_file + (ISO_BLOCK_SIZE * i));
				iso_boot();
				fprintf(stderr,"is booteable \n");
				i = 5;	 
			};break;
			case ISO_DESC_SUPPLEMENTARY:{
				svd_p = (supplementary_vol_desc*)( desc_file + (ISO_BLOCK_SIZE * 1) );
				printf("%d\n",svd_p->svd_vdt );
				if( argc > 2 && argv[2]!=NULL ){
						iso_joliet( lba, size, isDir );//  ACCEDER A UN DIRECTORIO ESPECIFICO EN EL ARCHIVO ISO
						i = 5;
						break;
				}
				iso_joliet( );
				i = 5;
			}break; 
			case ISO_DESC_PARTITION:{
				fprintf(stderr,"particion\n");
			}break;
			case ISO_DESC_TERMINATOR:{
				
				if( argc > 2 && argv[2]!=NULL ){
					iso9660( lba, size, isDir );//  ACCEDER A UN DIRECTORIO ESPECIFICO EN EL ARCHIVO ISO
					break;
				}
				iso9660();
			}break;
			default:fprintf(stderr,"MAIN: Formato iso desconocido\n");
		}
	}// END OF BUCLE FOR
	//dir_record_p = (dir_record*)(file+dir_records+68);
	//printf("%d\n",(unsigned short) file[17*2048+88] );
	delete[] desc_file;
	fclose(iso_file);
	getchar();
	
	return 0;
	
} //				FUNCTION MAIN  **********************************


uint8_t* load_descriptors(uint8_t&count_desc/*, uint8_t*descriptors*/){
	fseek(iso_file, ISO_VOL_SYS_AREA_SIZE, SEEK_SET );
	uint8_t*tmp_descriptors = new uint8_t[ISO_BLOCK_SIZE];
	// descriptors = NULL;
	if (tmp_descriptors == NULL)
	{
		fprintf(stderr,"LOAD_DESCRIPTORS: no memory\n");
		return tmp_descriptors;
	}
	do{
		fread(tmp_descriptors, ISO_BLOCK_SIZE,1,iso_file);
		switch(tmp_descriptors[0]){
			case ISO_DESC_BOOT					: count_desc++; break;
			case ISO_DESC_PRIMARY				: count_desc++; break;
			case ISO_DESC_SUPPLEMENTARY	: count_desc++; break;
			case ISO_DESC_PARTITION			: count_desc++; break;
			case ISO_DESC_TERMINATOR		: count_desc++; break;
			default:{
				if (count_desc == 0 ){ 
					tmp_descriptors[0] == ISO_DESC_TERMINATOR;
					fprintf(stderr,"load_descriptors: formato iso desconocido\n");
				}
				if (count_desc > 5){
					tmp_descriptors[0] == ISO_DESC_TERMINATOR;
					count_desc = 0;
					fprintf(stderr,"load_descriptors: formato iso desconocido, posible descriptor terminador ausente\n");
					break;
				}
				count_desc++;	
			}
		}
	}while(  tmp_descriptors[ 0 ] != ISO_DESC_TERMINATOR );
	delete[] tmp_descriptors;
	if (count_desc == 0){
		fprintf(stderr,"LOAD_DESCRIPTORS: ... \n");
		return (tmp_descriptors = NULL);
	}
	uint8_t*descriptors = new uint8_t[count_desc* ISO_BLOCK_SIZE];
	if (descriptors == NULL){
		return descriptors;
	}
	fseek(iso_file, ISO_VOL_SYS_AREA_SIZE, SEEK_SET );
	fread(descriptors, ISO_BLOCK_SIZE, count_desc, iso_file);;
	
	// printf("LOAD_DESCRIPTORS: count_desc = %d\n", count_desc);
	return descriptors;
}




void make_iso(){
	
	primary_vol_desc pvd ;
	pvd.pvd_vdt 			= 1;// Volume Descriptor Type
	memcpy(pvd.pvd_stdId, "CD001", 5);
	pvd.pvd_version = 1;
	pvd.pvd_unused_1 = 0;
	memset(pvd.pvd_sys_id, 0, 32);
	memcpy(pvd.pvd_vol_id, "VOL 1",5);memset(&pvd.pvd_vol_id[5], 0, 27);
	memset(pvd.pvd_unused_2, 0, 8);
	*(uint32_t*) pvd.pvd_vol_space_size 			= 0x00000016u;	
	*(uint32_t*)&pvd.pvd_vol_space_size[32] 	= 0x16000000u; // / numeros de blockes logicos que ocupa el volumen
	memset(pvd.pvd_unused_3, 0, 32);
	
	*(uint16_t*) pvd.pvd_vol_set_size 	 			 = 0x0001;
	*(uint16_t*)&pvd.pvd_vol_set_size[2] 			 = 0x0100;				
	
	*(uint16_t*) pvd.pvd_vol_secu_number 			 = 0x0001;	
	*(uint16_t*)&pvd.pvd_vol_secu_number[2] 	 = 0x0100;						
	
	*(uint16_t*) pvd.pvd_logical_block_size 	 = 0x0800;
	*(uint16_t*)&pvd.pvd_logical_block_size[2] = 0x0008;	
	
	pvd.pvd_path_table_size[8];
	pvd.pvd_addr_L_path_table[4];
	pvd.pvd_addr_opt_L_path_table[4];
	pvd.pvd_addr_m_path_table[4];
	pvd.pvd_addr_opt_m_path_table[4];
	//========================================= DIRECTORIO ROOT ===========================================
	dir_record_p = (dir_record*)pvd.pvd_root_dir;
 	dir_record_p -> dr_len_dir_record 										 = 32;
 	dir_record_p -> dr_extended_attr_record_len 					 = 0;
 	*(uint32_t*) dir_record_p -> dr_LBA_extent 						 = 0x00000015u;
 	*(uint32_t*)&dir_record_p -> dr_LBA_extent[4] 				 = 0x15000000u;
 																										 										
 	memset(dir_record_p -> dr_data_len, 0, 8);
 	memset(dir_record_p -> dr_recording_date_time, 0, 7);
 	dir_record_p -> dr_file_flag 													 = 0x02;
 	dir_record_p -> dr_file_unit_size											 = 0;
 	dir_record_p -> dr_Interleave_gap_size								 = 0;
 	*(uint16_t*) dir_record_p -> dr_vol_sequence_number 	 = 0x0001;	 	
 	*(uint16_t*)&dir_record_p -> dr_vol_sequence_number[2] = 0x0100;	 	

 	dir_record_p -> dr_len_file_id 	= 1;
 	dir_record_p -> dr_file_id[0] 	= 0;
	//========================================= DIRECTORIO ROOT ===========================================
	memcpy(pvd.pvd_vol_set_id,"VOL SET 1", 9); memset(&pvd.pvd_vol_set_id[9], 0, 119);
	pvd.pvd_pub_id[128];
	pvd.pvd_data_preparer_id[128];
	pvd.pvd_app_id[128];
	pvd.pvd_copyriht_file_id[38];
	pvd.pvd_abstract_file_id[36];
	pvd.pvd_bibliographic_file_id[37];
	pvd.pvd_Vol_creation_date_time[17];
	pvd.pvd_Vol_modification_date_time[17];
	pvd.pvd_Vol_expiration_date_time[17];
	pvd.pvd_Vol_effective_date_time[17];
	pvd.pvd_file_struct_version;
	pvd.pvd_unused_4;
	pvd.pvd_app_used[512];
	pvd.pvd_rsv[653];

	
	fin_vol_desc fvd = {
		255,
		"",
		1
	};
	memcpy(fvd.fvd_stdId, "CD001", 5);
	
	//======================================== BOOT RECORD ==========================================
	boot_vol_desc bd;
	bd.bvd_vdt = 0x00;// Volume Descriptor Type
	memcpy(bd.bvd_stdId,"CD001", 5);
	bd.bvd_version = 0x01;
	memcpy(bd.bvd_boot_sys_id,"EL TORITO SPECIFICATION",23);memset(&bd.bvd_boot_sys_id[23], 0, 9 );
	memset(bd.bvd_unused,0,32);
	*(uint32_t*)bd.bvd_lba_catalog = 0x00000013u;
	memset(bd.bvd_unused2,0,1973);
	//======================================== BOOT RECORD ==========================================
	
	//=================================== BOOT VALIDATION ENTRY =====================================
	boot_validation_entry bve ;
	bve.bve_head_id 		= 1;
	bve.bve_platform_id = 0;//0 = 80x86, 1 = Power PC, 2 = MAC
	memset(bve.bve_rsv, 0, 2);
	memcpy(bve.bve_id_string,"B1NAR10", 7); memset(&bve.bve_id_string[7], 0, 17);
	memset(bve.bve_chk_sum, 0, 2);
	*(uint16_t*)bve.bve_boot_mark = 0xAA55;
	//=================================== BOOT VALIDATION ENTRY =====================================

	//===================================== BOOT INITIAL ENTRY ======================================
	boot_initial_entry bie ;
	bie.bie_boot_indicator	=	0x88u;// 0x88 == booteable, 0x00 = not booteable
	bie.bie_boot_media_type = 0x00u;
	bie.bie_load_sement 		= 0x0000u;
	bie.bie_sys_type 				= 0;
	bie.bie_unused 					= 0;
	bie.bie_sector_count		= 0x0001u;
	bie.bie_load_rba 				= 0x00000014u;//20 en decimal
	memset(bie.bie_unused2, 0, 20);
		
	//===================================== BOOT INITIAL ENTRY ======================================
	
	FILE*file = fopen("myfirstiso.iso","wb");
	if (file == NULL)
	{
		printf("make_iso: ERROR TO CREATE\n");
		getchar();
		return;
	}
	
	fseek(file, ISO_VOL_SYS_AREA_SIZE, SEEK_SET);
	fwrite(&pvd, 1, ISO_BLOCK_SIZE,file);
	fwrite(&bd, 1, ISO_BLOCK_SIZE,file);
	fwrite(&fvd, 1, ISO_BLOCK_SIZE,file);
	
	fwrite(&bve, 1, 32,file);
	fwrite(&bie, 1, 32,file);
	
	fseek(file, ISO_BLOCK_SIZE - 64, SEEK_CUR);
	
	FILE*boot_image = fopen("asm.img","rb");
	if (boot_image == NULL){
		printf("make_iso: ERROR TO LOAD\n");
		getchar();
		return;
	}
	uint8_t* boot_buff = new uint8_t[ISO_BLOCK_SIZE];
	if (boot_buff == NULL){
		printf("make_iso: no memory\n");
		getchar();
		return;
	}
	fread(boot_buff, ISO_BLOCK_SIZE, 1, boot_image);
	fwrite(boot_buff, ISO_BLOCK_SIZE, 1, file );
	
	fclose(boot_image);
	fclose(file);
	delete[]boot_buff;
	printf("======== bve _vol_desc ===%u\n",sizeof(bve));
	printf("======== bie _vol_desc ===%u\n",sizeof(bie));
	// printf("======== fin _vol_desc ===%u\n",sizeof(fvd));
}