#pragma pack(push,1)
struct VBR
{
	char jmp[3];
	char oem[8];
	unsigned short bytes_per_sector;
	unsigned char sectors_per_cluster;
	unsigned short reserved_sectors;
	unsigned char num_fats;
	unsigned short UNUSED_num_root_dir_entries;
	unsigned short UNUSED_num_sectors_small;
	unsigned char id ;
	unsigned short UNUSED_sectors_per_fat_12_16;
	unsigned short sectors_per_track;
	unsigned short num_heads;
	unsigned int first_sector;
	unsigned int num_sectors;
	unsigned int sectors_per_fat;
	unsigned short flags;
	unsigned short version;
	unsigned int root_cluster;
	unsigned short fsinfo_sector;
	unsigned short backup_boot_sector;
	char reservedField[12];
	unsigned char drive_number;
	unsigned char flags2;
	unsigned char signature;
	unsigned int serial_number;
	char label[11];
	char identifier[8];
	char code[420];
	unsigned short checksum;
};
#pragma pack(pop)