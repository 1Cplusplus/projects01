
char iso_joliet(const uint32_t&lba = 0, const uint32_t&size = 0, const uint8_t&isDir = 0, const char* fileName="" ){
	// printf("iso_joliet\n"); 
	// printf("svd_addr_L_path_table %u\n", *(uint32_t*)svd_p->svd_addr_L_path_table );

	typeIso = new uint8_t[strlen("iso_joliet")];
	strcpy( (char*)typeIso, "iso_joliet" );
	uint32_t 	pos 			 = 0;// DIRECCION LBA DEL DIRECTORIO AL CUAL SE QUIERE ACCEDER
	uint16_t* BLOCK_SIZE = (uint16_t*)svd_p->svd_logical_block_size; 
/*/
 *	VOL_SPACE_SIZE: Tamaño del volumen en bloques de 2048 byte;
/*/
	uint32_t* vol_space_size = ( uint32_t*   ) svd_p->svd_vol_space_size; 
						dir_record_p   = ( dir_record* ) svd_p->svd_root_dir;
	uint32_t*	addr_root			 = ( uint32_t*   ) dir_record_p->dr_LBA_extent;
	
								pos = lba_parent = *addr_root;
	if (lba != 0)	pos = lba_parent = lba;
	 
	uint8_t 	count_records = 0;
	uint8_t * data_area 		= new uint8_t[ (*BLOCK_SIZE ) ];
	if (data_area == NULL){
		fprintf(stderr,"no memory"); 
		delete[]typeIso;  
		return 0;
	}
	fseek(iso_file, *BLOCK_SIZE * (pos), SEEK_SET);
	fread(data_area, 1, *BLOCK_SIZE, iso_file);
	
	if ( pos == *addr_root || isDir == 1 ){
		show_dir( data_area );
	}else {
		read_file(data_area, (uint32_t&)size, fileName); 
		// fprintf(stderr, "isDir => %u Esto es un archivo!", isDir );
	}
	//printf("iso size:  %d \n",(  data_area_len )* ISO_DESC_SIZE);
	
	delete[]data_area;  
	delete[]typeIso;  
	return 0; 
}
 
/*/
 *	END OF ISO_JOLIE
/*/
char iso9660( const uint32_t&lba = 0,const uint32_t&size = 0,const uint8_t&isDir = 0  ){
// printf("iso9660\n");
	typeIso = new uint8_t[strlen("iso_9660")];
	strcpy( (char*)typeIso, "iso_9660" );
	uint32_t 	pos								= 0;// DIRECCION LBA DEL DIRECTORIO AL CUAL SE QUIERE ACCEDER
	uint16_t* const BLOCK_SIZE 	= (uint16_t*)pvd_p->pvd_logical_block_size;
/*/
 *	VOL_SPACE_SIZE: Tamaño del volumen en bloques de 2048 byte;
/*/
	uint32_t *vol_space_size 		= (uint32_t*) pvd_p->pvd_vol_space_size;
						dir_record_p 			= ( dir_record* )pvd_p->pvd_root_dir;
	uint32_t*	addr_root					= (uint32_t*)dir_record_p->dr_LBA_extent;
	
	pos = lba_parent = *addr_root;
	
	if (lba != 0){
		pos= lba_parent = lba;
	}
	
	uint8_t 	count_records = 0;
	uint8_t * data_area 		= new uint8_t[ *BLOCK_SIZE ];
	if (data_area == NULL){
		fprintf(stderr,"ISO_9660: no memory\n");
		delete[]typeIso;
		return 0;
	}
	fseek(iso_file, *BLOCK_SIZE * (pos), SEEK_SET);
	fread( data_area, 1, *BLOCK_SIZE, iso_file);
	
	if ( pos == *addr_root || isDir == 1 ){
		show_dir( data_area );
	}else {
		// read_file(); 
		fprintf(stderr, "ISO_9660: isDir => %u Esto es un archivo!", isDir );
	}
	//printf("iso size:  %d \n",(  data_area_len )* ISO_DESC_SIZE);
	
	
	delete[]data_area;
	delete[]typeIso;
	return 0;
}
/*/
 *	END OF ISO9660
/*/

char show_dir(const uint8_t*  active_dir){
	uint8_t 	count_records( 0 ), count_files( 0 ), count_dirs( 0 );	
	uint32_t i( 68 );//Se inicializa en "68" para omitir lo 2 primeros directorio( carpeta ".","..")
	/*/
	* 	ACTIVE_DIR:		 es el directorio( carpeta ) el cual se esta evaluando.
	* 	I:						 representa el indice de cada registro en el directorio.
	* 	COUNT_RECORDS: es la cantidad de registros( archivos ) en el directorio( carpeta ).
	/*/
	
	// ESTE "WHILE" SE UTILIZA PARA CONOCER LA CANTIDAD DE ARCHIVOS EN UN DIRECTORIO	
	while( (dir_record_p = (dir_record*)&(active_dir [i] ))->dr_len_dir_record > 0 ){
		i += dir_record_p->dr_len_dir_record;
		count_records++;
		switch( dir_record_p->dr_file_flag ){
			case 0x02:{
				count_dirs++;	//	SIN USO
			}break;
			default:{
				count_files++;//	SIN USO
			}break;
		}
		
	}
	
	if (!count_records){
		fprintf(stderr, "SHOW_DIR: empty" );
		return 0;
	}
	
	i = 68;
	uint32_t *lba[count_records];				//	LBA DE TODOS LOS ARCHIVOS EN EL DIRECTORIO
	uint8_t  *name_file;								//	ALMACENA DE MANERA TEMPORAL EN NOMBRE DE LOS ARCHIVOS
	uint8_t  *filesName[count_records]; //	CONTIENE TODOS LOS NOMBRES DE LOS ARCHIVOS
	uint8_t   type_file[count_records];	//	CONTIENE EL TIPO DE CADA REGISTRO, "SI ES UN ARCHIVO O CARPETA"
	uint8_t   lenIdFiles[count_records];//	ALMACENA LA LONGITUD DE LOS NOMBRES DE CADA ARCHIVO
	uint8_t   lenIdDir[count_dirs];			//	SIN USO
	uint8_t  *dirsName[count_dirs];			//	SIN USO
	count_records = 0;
	
	//	ESTE "WHILE" SE UTILIZA PARA RECORRER CADA DIRECTORIO Y CONOCER SUS ARCHIVOS Y PROPIEDADES
	while( (dir_record_p = (dir_record*)&(active_dir [i] ))->dr_len_dir_record > 0 ){
		i 				+= dir_record_p->dr_len_dir_record;
	 	name_file = new uint8_t[ dir_record_p->dr_len_file_id ];
	
		memcpy(name_file, dir_record_p->dr_file_id, dir_record_p->dr_len_file_id );
		
		switch (dir_record_p->dr_file_flag){// SE UTILIZA PARA SABER SI EL REGISTRO ES UNA CARPETA O NO
			case 0x02: { //	EL REGISTRO ES UN DIRECTORIO
				filesName	[ count_records ] 	= name_file;
				lenIdFiles[ count_records ] 	= dir_record_p->dr_len_file_id; 
				lba[count_records]						= (uint32_t*)dir_record_p->dr_LBA_extent;
				type_file[count_records]			= 1;
			}break; 
			default:{	//	EL REGISTRO ES UN ARCHIVO
				uint32_t *temp_p = new uint32_t;
				memcpy(temp_p, dir_record_p->dr_data_len, sizeof( uint32_t ));
				filesName	[ count_records ] 	= name_file;
				lenIdFiles[ count_records ] 	= dir_record_p->dr_len_file_id; 
				lba[count_records]						= (uint32_t*)dir_record_p->dr_LBA_extent;
				type_file[count_records]			= 0;
				// printf("\n%d\n",dir_record_p->dr_file_unit_size );
				delete temp_p;
			}  
		} 
		count_records++;
	} 
	//*************** END WHILE ****************//
	
	//	SE UTILIZA PARA CREAR LA METADATA Y ENVIARLA JUNTO CON LOS DATOS
	createJson(filesName,lenIdFiles, lba, type_file, count_records);
	for (uint8_t i = 0; i < count_records ; i++){
		delete[] filesName[ i ];//	SE LIBERA EL ESPACIO ASIGNADO A CADA NOMBRE DE LOS ARCHIVOS
	}
 
	//printf("%d Archivos en esta carpeta\n", count_records );
	return 0;
}
/*/
 *	END OF SHOW DIR 
/*/

void read_file(uint8_t* data, uint32_t& size, const char* fileName="tmp_file"){
	FILE*file = fopen(fileName,"wb");
	if (file == NULL){
		fprintf(stderr,"READ_FILE: file\n");
		return;
	}
	fwrite(data, size, 1, file);
	fclose(file);
}
