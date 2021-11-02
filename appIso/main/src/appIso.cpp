#include "../inc/appiso.h"
#include "../../formats/joliet/src/joliet.cpp"
#include "../../boot/src/boot.cpp"
#include "../../formats/udf/src/udf.cpp"

int main(int argc, char const *argv[]){

	if (argc < 2 ){
		make_iso();
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
	
//============================================ UDF VIEW ==================================================
// udf_desc_file: PARA COMPROBAR SI EL SISTEMA DE ARCHIVO ES UDF
	uint8_t * udf_desc_file = new uint8_t[ISO_BLOCK_SIZE ];
	fread(udf_desc_file, 6,1,iso_file);// AVANZA 6 BYTE EN BUSQUEDA DEL "BEGINIG EXTENDED AREA"
																														// ^
// udf_bea01_p: PUNTERO A LA "BEGINIG EXTENDED AREA"				// |
	udf_bea01_p = (bea01*)udf_desc_file;											// |
																														// |
// BEA01:"BEGINIG EXTENDED AREA" 														// |
	if(memcmp(BEA01, udf_bea01_p->std_id, 3) == 0){						// |
		fprintf(stderr,"la app no maneja archivos UDF\n");			// |
		delete[] udf_desc_file;																	// |
		delete[] desc_file;																			// |
		fclose (iso_file);																			// |
		getchar();																							// |
		return 0;																								// |
	}																													// |
	fseek(iso_file, -6, SEEK_CUR);	// LO CONTRARIO A ESTA LINEA |
//==========================================================================================================	
	
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
		switch ( desc_file[ i * ISO_BLOCK_SIZE ]  ){// SE COMPRUEBA EL TIPO DEL SEGUNDO DESCRIPTOR.
			case ISO_DESC_BOOT:{ 
				bvd_p = (boot_vol_desc*)(desc_file + (ISO_BLOCK_SIZE * i));
				iso_boot();
				
				// i = 5;	 
			};break;
			case ISO_DESC_SUPPLEMENTARY:{
				svd_p = (supplementary_vol_desc*)( desc_file + (ISO_BLOCK_SIZE * i) );
				
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
	}// =========== END OF BUCLE FOR =============
	
	//dir_record_p = (dir_record*)(file+dir_records+68);
	//printf("%d\n",(unsigned short) file[17*2048+88] );
	delete[] desc_file;
	fclose(iso_file);
	getchar();
	
	return 0;
	
} //				FUNCTION MAIN  **********************************
//================================================== FUN: MAKE_DESC ============================================
void make_desc(const uint8_t& desc_type, void*desc){
	switch(desc_type){
		case ISO_DESC_BOOT:					{ make_boot((boot_vol_desc*)desc);					 						}break;
		case ISO_DESC_SUPPLEMENTARY:{ make_supplementary((supplementary_vol_desc*)desc);	 	}break;
		// case ISO_DESC_PARTITION:		{ make_partition() 			 																}break;
		default:printf("make_desc: descriptor desconocido\n");
	}
}
//===============================================================================================================
void make_supplementary(supplementary_vol_desc*svd){
	svd->svd_vdt 			= 0x02; // Volume Descriptor Type
	memcpy( svd->svd_stdId, "CD001", 5);
	svd->svd_version 	= 0x01;
	svd->svd_vol_flag 	= 0;
	memset(svd->svd_sys_id, 0, 32);
	memcpy(svd->svd_vol_id, "\0V\0O\0L\0 \0\x31",10);memset(&svd->svd_vol_id[10], 0, 17);
	memset(svd->svd_unused_1, 0, 8);
	*(uint32_t*) svd->svd_vol_space_size 			 = 0x00000015u;	
	*(uint32_t*)&svd->svd_vol_space_size[32] 	 = 0x15000000u; // numeros de blockes logicos que ocupa el volumen
	memcpy(svd->svd_escape_sequences, "%/E", 3); memset(&svd->svd_escape_sequences[3], 0, 29);
	*(uint16_t*) svd->svd_vol_set_size 	 			 = 0x0001; 
	*(uint16_t*)&svd->svd_vol_set_size[2] 		 = 0x0100; 
	*(uint16_t*) svd->svd_vol_sequ_number 		 = 0x0001;	
	*(uint16_t*)&svd->svd_vol_sequ_number[2] 	 = 0x0100;		
	*(uint16_t*) svd->svd_logical_block_size 	 = 0x0800;
	*(uint16_t*)&svd->svd_logical_block_size[2] = 0x0008;	
	svd->svd_path_table_size[8];
	svd->svd_addr_L_path_table[4];
	svd->svd_addr_opt_L_path_table[4];
	svd->svd_addr_m_path_table[4];
	svd->svd_addr_opt_m_path_table[4];
		//========================================= DIRECTORIO ROOT =======================================
	dir_record_p = (dir_record*)svd->svd_root_dir;
	
 	dir_record_p -> dr_len_dir_record 										 = 34;
 	dir_record_p -> dr_extended_attr_record_len 					 = 0;
 	*(uint32_t*) dir_record_p -> dr_LBA_extent 						 = 0x00000013u;
 	*(uint32_t*)&dir_record_p -> dr_LBA_extent[4] 				 = 0x13000000u;
 																										 										
 	*(uint32_t*) dir_record_p -> dr_data_len							 = 0x00000800u;
 	*(uint32_t*)&dir_record_p -> dr_data_len[4] 					 = 0x00080000u;
 	
 	memset(dir_record_p -> dr_recording_date_time, 0, 7);
 	dir_record_p -> dr_file_flag 													 = 0x02;
 	dir_record_p -> dr_file_unit_size											 = 0;
 	dir_record_p -> dr_Interleave_gap_size								 = 0;
 	*(uint16_t*) dir_record_p -> dr_vol_sequence_number 	 = 0x0001;	 	
 	*(uint16_t*)&dir_record_p -> dr_vol_sequence_number[2] = 0x0100;	 	

 	dir_record_p -> dr_len_file_id 	= 1;
 	dir_record_p -> dr_file_id[0] 	= 0;
		//========================================= DIRECTORIO ROOT ========================================
	memcpy(svd->svd_vol_set_id,"\0V\0O\0L\0 \0S\0e\0T\0 \0\x31", 18); memset(&svd->svd_vol_set_id[18], 0, 110);
	svd->svd_pub_id[128];
	svd->svd_data_preparer_id[128];
	svd->svd_app_id[128];
	svd->svd_copyriht_file_id[38];
	svd->svd_abstract_file_id[36];
	svd->svd_bibliographic_file_id[37];
	svd->svd_Vol_creation_date_time[17];
	svd->svd_Vol_modification_date_time[17];
	svd->svd_Vol_expiration_date_time[17];
	svd->svd_Vol_effective_date_time[17]; 
	svd->svd_file_struct_version;
	svd->svd_rsv_1;
	svd->svd_app_used[512];
	svd->svd_rsv_2[653]; 
}
//==================================================================================================================
//================================================== FUN: LOAD_DESCRIPTORS ======================================
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
//==========================================================================================================


//=================================================== MAKE_ISO ==================================================
void make_iso(){
	
										//================= PRIMARY VOLUME DESCRIPTOR =======================
	primary_vol_desc pvd ;
	memset(&pvd, 0, sizeof( pvd ) );
	
	pvd.pvd_vdt 			= ISO_DESC_PRIMARY;// Volume Descriptor Type
	memcpy(pvd.pvd_stdId, "CD001", 5);
	pvd.pvd_version 	= 1;
	pvd.pvd_unused_1 	= 0;
	memset(pvd.pvd_sys_id, 0, 32);
	memcpy(pvd.pvd_vol_id, "VOL I",5);memset(&pvd.pvd_vol_id[5], 0, 27);
	memset(pvd.pvd_unused_2, 0, 8);
	*(uint32_t*) pvd.pvd_vol_space_size 			 = 0x00000015u;	
	*(uint32_t*)&pvd.pvd_vol_space_size[32] 	 = 0x15000000u; // numeros de blockes logicos que ocupa el volumen
	memset(pvd.pvd_unused_3, 0, 32);
	
	*(uint16_t*) pvd.pvd_vol_set_size 	 			 = 0x0001;
	*(uint16_t*)&pvd.pvd_vol_set_size[2] 			 = 0x0100;				
	
	*(uint16_t*) pvd.pvd_vol_sequ_number 			 = 0x0001;	
	*(uint16_t*)&pvd.pvd_vol_sequ_number[2] 	 = 0x0100;						
	
	*(uint16_t*) pvd.pvd_logical_block_size 	 = 0x0800;
	*(uint16_t*)&pvd.pvd_logical_block_size[2] = 0x0008;	
	
	pvd.pvd_path_table_size[8];
	pvd.pvd_addr_L_path_table[4];
	pvd.pvd_addr_opt_L_path_table[4];
	pvd.pvd_addr_m_path_table[4];
	pvd.pvd_addr_opt_m_path_table[4];
														//========================== DIRECTORIO ROOT ==================
	dir_record_p = (dir_record*)pvd.pvd_root_dir;
 	dir_record_p -> dr_len_dir_record 										 = 34;
 	dir_record_p -> dr_extended_attr_record_len 					 = 0;
 	*(uint32_t*) dir_record_p -> dr_LBA_extent 						 = 0x00000013u;
 	*(uint32_t*)&dir_record_p -> dr_LBA_extent[4] 				 = 0x13000000u;
 																										 										

 	*(uint32_t*) dir_record_p -> dr_data_len	  					 = 0x00000800u;
 	*(uint32_t*)&dir_record_p -> dr_data_len[4] 					 = 0x00080000u;
 	memset(dir_record_p -> dr_recording_date_time, 0, 7);
 	dir_record_p -> dr_file_flag 													 = 0x02;
 	dir_record_p -> dr_file_unit_size											 = 0;
 	dir_record_p -> dr_Interleave_gap_size								 = 0;
 	*(uint16_t*) dir_record_p -> dr_vol_sequence_number 	 = 0x0001;	 	
 	*(uint16_t*)&dir_record_p -> dr_vol_sequence_number[2] = 0x0100;	 	

 	dir_record_p -> dr_len_file_id 	= 1;
 	dir_record_p -> dr_file_id[0] 	= 0;
														//========================== DIRECTORIO ROOT ==================
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
												//=========================================================
	
	//====================================== TERMINATOR VOLUME DESCRIPTOR =======================================
	terminator_vol_desc tvd;
	tvd.tvd_vdt 			= ISO_DESC_TERMINATOR;// Volume Descriptor Type
	memcpy(tvd.tvd_stdId, "CD001", 5);
	tvd.tvd_version 	= 1;
	memset(tvd.tvd_rsv, 0, 2041 );
	//===========================================================================================================
	

	supplementary_vol_desc svd;
	make_desc(ISO_DESC_SUPPLEMENTARY, &svd);
	
	boot_vol_desc bvd;
	make_desc(ISO_DESC_BOOT, &bvd);
	
	boot_validation_entry bve;
	boot_initial_entry bie;
	make_boot_catalog(bve, bie);
	
	FILE*new_file = fopen("myfirstiso.iso","w+b");
	
	if (new_file == NULL)
	{
		printf("make_iso: ERROR TO CREATE\n");
		getchar();
		return;
	}
	fseek(new_file, ISO_VOL_SYS_AREA_SIZE, SEEK_SET);
	
	fwrite(&pvd, 1, ISO_BLOCK_SIZE,new_file);
	fwrite(&svd, 1, ISO_BLOCK_SIZE,new_file);	
	// fwrite(&bvd, 1, ISO_BLOCK_SIZE,new_file);
	fwrite(&tvd, 1, ISO_BLOCK_SIZE,new_file) ; 
	
	// fwrite(svd.svd_root_dir, 34, 1, new_file);
	// svd.svd_root_dir[33] = 1;
	// fwrite(svd.svd_root_dir, 1, 34, new_file);
	// svd.svd_root_dir[33] = 0;
	uint8_t *extend_root = new uint8_t[ISO_BLOCK_SIZE];
	if (extend_root == NULL)
	{
		printf("make_iso: no memory by root \n");
		return;
	}
	// memset(extend_root, 0, ISO_BLOCK_SIZE);
	
	memcpy(extend_root, svd.svd_root_dir, 34);
	svd.svd_root_dir[33] = 1;
	memcpy(&extend_root[34], svd.svd_root_dir, 34);
	svd.svd_root_dir[33] = 0;
	fwrite(extend_root, 1, ISO_BLOCK_SIZE, new_file) ;


	// fwrite(&bve, 1, 32,new_file);
	// fwrite(&bie, 1, 32,new_file);
	
	// COMPLETA LOS 2048 KB PARA EL EXTEND DEL BOOT CATALOG
	// fseek(new_file, ISO_BLOCK_SIZE - BOOT_CATALOG_SIZE_SI, SEEK_CUR);
	
	FILE*boot_image = fopen("asm.img","r+b");
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
	count_lba += 1;
	
	
	fread(boot_buff, 1, ISO_BLOCK_SIZE, boot_image) ;
	
	fwrite(boot_buff, 1, ISO_BLOCK_SIZE, new_file ) ;
	record_files(new_file, (dir_record*)svd.svd_root_dir, *(uint32_t*)( (dir_record*)svd.svd_root_dir) ->dr_LBA_extent );
	
	fclose(boot_image); 
	fclose(new_file);
	delete[]boot_buff;
	delete[]extend_root;
	
}
//=============================================================================================================
void record_files(FILE*file, dir_record*parent_dir, uint32_t&lba_parent_dir){
		fseek(file, ISO_BLOCK_SIZE * lba_parent_dir, SEEK_SET);
		uint32_t ini_dir = ftell(file);
		
		uint8_t*dir = new uint8_t[ ISO_BLOCK_SIZE ];
		if (dir == NULL)	{
			printf("record_files: no memory\n");
			return;
		}
		memset(dir, 0, ISO_BLOCK_SIZE);
		fread(dir, 1, ISO_BLOCK_SIZE, file);
		uint8_t pos_new_record = 0;
		while( (dir_record_p = (dir_record*) &dir[pos_new_record] ) -> dr_len_dir_record >= 0 ){
			pos_new_record += dir_record_p->dr_len_dir_record;
			
			if (dir_record_p -> dr_len_dir_record > 0)continue;
			if (!pos_new_record){
				fseek(file, ini_dir, SEEK_SET);
			}else	fseek(file, (ini_dir + pos_new_record), SEEK_SET);
			
			dir_record *new_record = new dir_record;
			if (new_record == NULL){
				printf("record_files: no memory by new_record\n");
				delete[]dir;
				return;
			}
			
			new_record->dr_len_dir_record = 34;
			*(uint16_t*) new_record->dr_vol_sequence_number 		= 0x0001;
			*(uint16_t*)&new_record->dr_vol_sequence_number[2] 	= 0x0100;
			new_record->dr_file_unit_size = 0;
			new_record->dr_len_file_id 		= 1;
			new_record->dr_Interleave_gap_size = 0;
			*(uint32_t*)new_record->dr_data_len 			= 0x00000200u;
			*(uint32_t*)&new_record->dr_data_len[4] 	= 0x00020000u;
			*(uint32_t*)new_record->dr_LBA_extent 		= 0x00000014u;
			*(uint32_t*)&new_record->dr_LBA_extent[4] = 0x14000000u;
			memset(new_record->dr_recording_date_time, 0, 7);
			new_record->dr_file_id[0] = 'a';			
			new_record->dr_file_flag	= 0;
			new_record->dr_extended_attr_record_len = 0;
			
			
			printf("----------%u\n",  fwrite(new_record, 1, sizeof(dir_record), file) );
			pos_new_record +=  dir_record_p->dr_len_dir_record;
			
			
			delete new_record;
			break;
		}
		
		delete[] dir;
}