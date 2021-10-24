#include "../inc/appiso.h"
#include "../../formats/joliet/src/joliet.cpp"
#include "../../boot/src/boot.cpp"

int main(int argc, char const *argv[]){
	
	if (argc < 2 ){
		fprintf(stderr,"Proporcione el archivo iso a leer");
		// getchar();
		return 0;
	} 
	
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

// desc_file:	ESPACIO EN EL CUAL SE ALMACENAN LOS DOS PRIMEROS DESCRICTORES DEL ARCHIVO ISO
	uint8_t *desc_file = new uint8_t[ISO_BLOCK_SIZE * 2u];
	
	if (desc_file == NULL){
		fprintf(stderr,"no memory");
		// getchar();
		return 0;
	}
	fseek(iso_file, ISO_VOL_SYS_AREA_SIZE, SEEK_SET );
	fread(desc_file, ISO_BLOCK_SIZE, 2, iso_file);
	
	if (desc_file[0] != ISO_DESC_PRIMARY){
		fprintf(stderr, "Formato iso desconocido");
		delete[] desc_file;
		// getchar();
		return 0;
	} 
	pvd_p = (primary_vol_desc*)desc_file;
	printf("volumen space size %" PRId64"\n", (*(uint32_t*)pvd_p->pvd_vol_space_size )* ISO_BLOCK_SIZE);
/*/
 * se comprueba el tipo del segundo descriptor.
/*/ 
	switch ( desc_file[ ISO_BLOCK_SIZE ]  ){
		case ISO_DESC_BOOT:{ 
			bvd_p = (boot_vol_desc*)(desc_file + ISO_BLOCK_SIZE);
			iso_boot();
			// fprintf(stderr, "iso boot");
		}break;
		case ISO_DESC_SUPPLEMENTARY:{
			svd_p = (supplementary_vol_desc*)( desc_file + ISO_BLOCK_SIZE );
			if( argc > 2 && argv[2]!=NULL ){
					iso_joliet( lba, size, isDir );//  ACCEDER A UN DIRECTORIO ESPECIFICO EN EL ARCHIVO ISO
					break;
			}
			iso_joliet( );
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
		default:fprintf(stderr,"Formato iso desconocido\n");
	}

	//dir_record_p = (dir_record*)(file+dir_records+68);
	//printf("%d\n",(unsigned short) file[17*2048+88] );
	fclose(iso_file);
	delete[] desc_file;
	getchar();
	
	return 0;
} 


