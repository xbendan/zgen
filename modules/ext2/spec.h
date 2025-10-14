#pragma once

#include <sdk-meta/array.h>
#include <sdk-meta/endian.h>
#include <sdk-meta/types.h>
#include <sdk-meta/uuid.h>

namespace ext2 {

struct Superblk {
    u32le inodes;
    u32le blocks;
    u32le blocksRsrv;
    u32le blocksFree;
    u32le inodesFree;
    u32le firstBlock;
    u32le blockSize;
    u32le fragSize;
    u32le blocksPergroup;
    u32le fragsPergroup;
    u32le inodesPergroup;
    u32le whenMount;
    u32le whenLastWrite;
    u16le mountsCount;
    u16le mountsLimit;
    u16le magic;
    u16le state;
    u16le erraction;
    u16le vminor;
    u32le whenLastCheck;
    u32le checkInterval;
    u32le osId;
    u32le vmajor;
    u16le uidRsrv;
    u16le gidRsrv;

    u32le           firstInode;
    u32le           inodeSize;
    u16le           blockGroupNum;
    u16le           featureCompat;
    u16le           featureIncompat;
    u16le           featureRoCompat;
    Uuid            uuid;
    Array<char, 16> volumeName;
    Array<char, 64> lastMounted;
    u32le           bitmapUsage;
    u8              preallocBlocks;
    u8              preallocDirBlocks;
    u16             __reserved__0;
    Uuid            journalUuid;
    u32             journalInode;
    u32             journalDevice;
    u32             orphanInodeHead;
    Array<u32, 4>   hashSeed;
    u8              defHashVersion;
    Array<u8, 3>    __reserved__1;
    u32le           defaultMountOpts;
    u32le           firstMetaBlkGroup;
    Array<u8, 760>  __reserved__2;
};

struct BlockGroupDesc {
    u32le         blockBitmap;
    u32le         inodeBitmap;
    u32le         inodeTable;
    u16le         freeBlocksCount;
    u16le         freeInodesCount;
    u16le         usedDirsCount;
    u16le         pad;
    Array<u8, 12> __reserved__;
};

struct Inode {
    u16le mode;
    u16le uid;
    u32le sizeLo;
    u32le whenAccess;
    u32le whenCreate;
    u32le whenModify;
    u32le whenDelete;
    u16le bitsGroupId;
    u16le links;
    u32le blocks;
    u32le flags;
    u32le osd1;
    u32le blocks[15];
    u32le filever;
    u32le fileacl;
    u32le diracl;
    u32le fragAddress;
    struct {
        u8    fragNum;
        u8    fragSize;
        u16   __reserved__0;
        u32le uidHi;
        u32le gidHi;
        u32   __reserved__1;
    };
};

} // namespace ext2
