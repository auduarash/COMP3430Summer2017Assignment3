#ifndef FAT32_H
#define FAT32_H

#include <inttypes.h>

//fat 32 verfification
#define FAT32_ROOT_DIR_SECTORS 0
#define MIN_FAT32_CLUSTER_COUNT 65525

#define _FILE_OFFSET_BITS 64
/* boot sector constants */
#define BS_OEMName_LENGTH 8
#define BS_VolLab_LENGTH 11
#define BS_FilSysType_LENGTH 8 

#pragma pack(push)
#pragma pack(1)
struct fat32BS_struct {
	char BS_jmpBoot[3];
	char BS_OEMName[BS_OEMName_LENGTH];
	uint16_t BPB_BytesPerSec;
	uint8_t BPB_SecPerClus;
	uint16_t BPB_RsvdSecCnt;
	uint8_t BPB_NumFATs;
	uint16_t BPB_RootEntCnt;
	uint16_t BPB_TotSec16;
	uint8_t BPB_Media;
	uint16_t BPB_FATSz16;
	uint16_t BPB_SecPerTrk;
	uint16_t BPB_NumHeads;
	uint32_t BPB_HiddSec;
	uint32_t BPB_TotSec32;
	uint32_t BPB_FATSz32;
	uint16_t BPB_ExtFlags;
	uint8_t BPB_FSVerLow;
	uint8_t BPB_FSVerHigh;
	uint32_t BPB_RootClus;
	uint16_t BPB_FSInfo;
	uint16_t BPB_BkBootSec;
	char BPB_reserved[12];
	uint8_t BS_DrvNum;
	uint8_t BS_Reserved1;
	uint8_t BS_BootSig;
	uint32_t BS_VolID;
	char BS_VolLab[BS_VolLab_LENGTH];
	char BS_FilSysType[BS_FilSysType_LENGTH];
	char BS_CodeReserved[420];
	uint8_t BS_SigA;
	uint8_t BS_SigB;
};
#pragma pack(pop)

typedef struct fat32BS_struct fat32BS;


//directory entry constants
#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LONG_NAME 0x0F
#define DIR_Name_LENGTH 11

#pragma pack(push)
#pragma pack(1)
//fat32 directory entry struct
struct fat32DE_struct {
	char DIR_Name[DIR_Name_LENGTH];
	uint8_t DIR_Attr;
	uint8_t DIR_NTRes;
	uint8_t DIR_CrtTimeTenth;
	uint16_t DIR_CrtTime;
	uint16_t DIR_CrtDate;
	uint16_t DIR_LstAccDate;
	uint16_t DIR_FstClusHI;
	uint16_t DIR_WrtTime;
	uint16_t DIR_WrtDate;
	uint16_t DIR_FstClusLO;
	uint32_t DIR_FileSize;
};
#pragma pack(pop)

typedef struct fat32DE_struct fat32DE;
#endif
