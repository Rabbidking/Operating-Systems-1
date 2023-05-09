#pragma pack(push,1)
struct DirEntry {
	char base[8];
	char ext[3];
	unsigned char attributes;
	unsigned char reserved;
	unsigned char creationTimeSecondsTenths;
	unsigned short creationTime;
	unsigned short creationDate;
	unsigned short lastAccessDate;
	unsigned short clusterHigh;
	unsigned short lastModifiedTime;
	unsigned short lastModifiedDate;
	unsigned short clusterLow;
	unsigned int size;
};
#pragma pack(pop)