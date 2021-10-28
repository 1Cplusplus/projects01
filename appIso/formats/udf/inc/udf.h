
#define NSR02 "NSR02"
#define NSR03 "NSR03"
#define BEA01 "BEA01"

struct charspec {
	uint8_t CharacterSetType;
	uint8_t CharacterSetInfo[63];
};
struct timestamp { /* ISO 13346 1/7.3 */
	uint16_t TypeAndTimezone;
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint8_t Centiseconds;
	uint8_t HundredsofMicroseconds;
	uint8_t Microseconds;
};

struct EntityID { /* ISO 13346 1/7.4 */
	uint8_t Flags;
	char Identifier[23];
	char IdentifierSuffix[8];
};
struct tag { /* ISO 13346 3/7.2 */
	uint16_t TagIdentifier;
	uint16_t DescriptorVersion;
	uint8_t TagChecksum;
	uint16_t Reserved;
	uint16_t TagSerialNumber;
	uint16_t DescriptorCRC;
	uint16_t DescriptorCRCLength;
	uint32_t TagLocation;
};
struct extent_ad{
	uint8_t length[4 ];
	uint8_t location[4 ];
};
struct PrimaryVolumeDescriptor { /* ISO 13346 3/10.1 */
		struct tag DescriptorTag;
		uint32_t VolumeDescriptorSequenceNumber;
		uint32_t PrimaryVolumeDescriptorNumber;
		uint8_t VolumeIdentifier[32];
		uint16_t VolumeSequenceNumber;
		uint16_t MaximumVolumeSequenceNumber;
		uint16_t InterchangeLevel;
		uint16_t MaximumInterchangeLevel;
		uint32_t CharacterSetList;
		uint32_t MaximumCharacterSetList;
		uint8_t VolumeSetIdentifier[128];
		struct charspec DescriptorCharacterSet;
		struct charspec ExplanatoryCharacterSet;
		struct extent_ad VolumeAbstract;
		struct extent_ad VolumeCopyrightNotice;
		struct EntityID ApplicationIdentifier;
		struct timestamp RecordingDateandTime;
		struct EntityID ImplementationIdentifier;
		uint8_t ImplementationUse[64];
		uint32_t PredecessorVolumeDescriptorSequenceLocation;
		uint16_t Flags;
		uint8_t Reserved[22];
}*udf_pvd_p;;

struct avdsp {
 	tag dtag;
 	extent_ad exMain;
 	extent_ad exRsv;
 }*avdsp_p; 
struct bea01{ // BEGINING EXTENDED AREA
	uint8_t st;
	uint8_t std_id[5]	;
	uint8_t str_v;
	uint8_t rsv[2041];
}*udf_bea01_p;