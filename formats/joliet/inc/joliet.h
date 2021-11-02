#include <cstdint>
#include <cstdio>
#include <string.h>

#ifndef JOLIET_H
#define JOLIET_H

#define ISO_DESC_BOOT 					0u
#define ISO_DESC_PRIMARY 				1u
#define ISO_DESC_SUPPLEMENTARY 	2u
#define ISO_DESC_PARTITION 			3u
#define ISO_DESC_TERMINATOR 		255u
#define ISO_BLOCK_SIZE 					2048u
#define ISO_VOL_SYS_AREA_SIZE 	2048*16u

struct vol{
	//uint8_t vol_sys_area [16*2048];
	uint8_t* vol_data_area[1];
}*desc_file_;

struct dir_record{
	uint8_t dr_len_dir_record;
	uint8_t dr_extended_attr_record_len;
	uint8_t dr_LBA_extent[8];
	uint8_t dr_data_len[8];
	uint8_t dr_recording_date_time[7];
	uint8_t dr_file_flag;
	uint8_t dr_file_unit_size;
	uint8_t dr_Interleave_gap_size;
	uint8_t dr_vol_sequence_number[4];
	uint8_t dr_len_file_id;
	uint8_t dr_file_id[1];
	
}*dir_record_p;
extern struct fin_vol_desc{
	uint8_t fvd_vdt;
	uint8_t fvd_stdId[5];
	uint8_t	fvd_version;
	uint8_t fvd_rsv[2041];
}*fvd_p;
struct primary_vol_desc{
	uint8_t pvd_vdt;
	uint8_t pvd_stdId[5];
	uint8_t	pvd_version;
	uint8_t pvd_unused_1;
	uint8_t pvd_sys_id[32];
	uint8_t pvd_vol_id[32];
	uint8_t pvd_unused_2[8];
	uint8_t pvd_vol_space_size[8];
	uint8_t pvd_unused_3[32];
	uint8_t pvd_vol_set_size[4];
	uint8_t pvd_vol_secu_number[4];
	uint8_t pvd_logical_block_size[4];
	uint8_t pvd_path_table_size[8];
	uint8_t pvd_addr_L_path_table[4];
	uint8_t pvd_addr_opt_L_path_table[4];
	uint8_t pvd_addr_m_path_table[4];
	uint8_t pvd_addr_opt_m_path_table[4];
	uint8_t pvd_root_dir[34];
	uint8_t pvd_vol_set_id[128];
	uint8_t pvd_pub_id[128];
	uint8_t pvd_data_preparer_id[128];
	uint8_t pvd_app_id[128];
	uint8_t pvd_copyriht_file_id[38];
	uint8_t pvd_abstract_file_id[36];
	uint8_t pvd_bibliographic_file_id[37];
	uint8_t pvd_Vol_creation_date_time[17];
	uint8_t pvd_Vol_modification_date_time[17];
	uint8_t pvd_Vol_expiration_date_time[17];
	uint8_t pvd_Vol_effective_date_time[17];
	uint8_t pvd_file_struct_version;
	uint8_t pvd_unused_4;
	uint8_t pvd_app_used[512];
	uint8_t pvd_rsv[653];
}*pvd_p;

struct supplementary_vol_desc{
	uint8_t svd_vdt;
	uint8_t svd_stdId[5];
	uint8_t	svd_version;
	uint8_t svd_vol_flag;
	uint8_t svd_sys_id[32];
	uint8_t svd_vol_id[32];
	uint8_t svd_unused_1[8];
	uint8_t svd_vol_space_size[8];
	uint8_t svd_escape_sequences[32];
	uint8_t svd_vol_set_size[4]; 
	uint8_t svd_vol_sequ_number[4];
	uint8_t svd_logical_block_size[4];
	uint8_t svd_path_table_size[8];
	uint8_t svd_addr_L_path_table[4];
	uint8_t svd_addr_opt_L_path_table[4];
	uint8_t svd_addr_m_path_table[4];
	uint8_t svd_addr_opt_m_path_table[4];
	uint8_t svd_root_dir[34];
	uint8_t svd_vol_set_id[128];
	uint8_t svd_pub_id[128];
	uint8_t svd_data_preparer_id[128];
	uint8_t svd_app_id[128];
	uint8_t svd_copyriht_file_id[38];
	uint8_t svd_abstract_file_id[36];
	uint8_t svd_bibliographic_file_id[37];
	uint8_t svd_Vol_creation_date_time[17];
	uint8_t svd_Vol_modification_date_time[17];
	uint8_t svd_Vol_expiration_date_time[17];
	uint8_t svd_Vol_effective_date_time[17]; 
	uint8_t svd_file_struct_version;
	uint8_t svd_rsv_1;
	uint8_t svd_app_used[512];
	uint8_t svd_rsv_2[653]; 
}*svd_p; 
struct attr_files{
  	uint32_t 	af_lba;
  	uint32_t 	af_size;
  	uint8_t		af_is_dir;
}; 
 
uint8_t*typeIso; 
uint8_t lba_parent;
FILE*iso_file;  


char iso_joliet(const uint32_t&lba ,const uint32_t&size ,const uint8_t&isDir );   
char show_dir(const uint8_t* );

	//	SE UTILIZA PARA CREAR LA METADATA Y ENVIARLA JUNTO CON LOS DATOS
char createJson(uint8_t*filesName[], uint8_t*lenIdFiles, uint32_t *lba[],uint8_t*type_file, uint8_t&count_records){

	fprintf(stdout, "{" );

	fprintf(stdout, "\"parent\":{" );
	for (int i = 0; i < count_records; ++i){
		if ( i+1 == count_records){
			fprintf(stdout, "\"%u\":{\"lenId\":%u,\"lba\":%u,\"isDir\":%u}}", i, lenIdFiles[i], *lba[i], type_file[i]);
			continue;		
		}
		fprintf(stdout, "\"%u\":{\"lenId\":%u,\"lba\":%u,\"isDir\":%u},", i, lenIdFiles[i], *lba[i], type_file[i]);
	}		
	fprintf(stdout,",\"lba\":%u,\"typeIso\":\"%s\"}",lba_parent, typeIso );
	for (int i = 0; i < count_records; ++i){
		fwrite(filesName[i], 1, lenIdFiles[i], stdout);
	}
	return 0;
}

#endif
