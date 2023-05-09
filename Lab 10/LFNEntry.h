#pragma pack(push,1)
struct LFNEntry {
	unsigned char sequenceNumber;
	char name0[10];
	char attribute; //always 15
	char zero; //always zero
	char checksum;
	char name1[12];
	unsigned short alsozero; //always zero
	char name2[4];
};
#pragma pack(pop)