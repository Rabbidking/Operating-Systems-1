#pragma once
#pragma pack(push,1)

struct BootInfo
{
	unsigned flags;
	unsigned memLower; //always 640K
	unsigned memUpper; //amount of memory above 1MB mark
	unsigned long long framebufferAddress;
	unsigned framebufferPitch;
	unsigned framebufferWidth;
	unsigned framebufferHeight;
	unsigned char framebufferBpp;
	unsigned char framebufferType;
	unsigned char framebufferRedPos;
	unsigned char framebufferRedMask;
	unsigned char framebufferGreenPos;
	unsigned char framebufferGreenMask;
	unsigned char framebufferBluePos;
	unsigned char framebufferBlueMask;
};
#pragma pack(pop)