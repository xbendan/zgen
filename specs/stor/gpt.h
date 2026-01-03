#pragma once

#include <persistence-partition/mbr.h>
#include <sdk-meta/array.h>
#include <sdk-meta/types.h>
#include <sdk-meta/uuid.h>

namespace Gpt {

static constexpr inline u64 SIGNATURE = 0x5452'4150'2049'4645; // "EFI PART"

static constexpr inline Mbr::Entry PROTECTIVE_MASTER_BOOT_RECORD = {
    .status    = { 0 },
    .chsBegin  = { 0x00, 0x02, 0x00 },
    .type      = 0xEE,
    .chsEnd    = { 0xFF, 0xFF, 0xFF },
    .lbaBegin  = 1,
    .lbaLength = 0xFFFF'FFFF,
};

struct [[gnu::packed]] Entry {
    Uuid            parId;
    Uuid            uid;
    u64             lbaBegin;
    u64             lbaEnd;
    u64             flags;
    Array<char, 72> name;
};

struct [[gnu::packed]] Table {
    u64  signature;
    u32  revision;
    u32  headerSize;
    u32  checksum;
    u32  __reserved__0;
    u64  lbaTable;
    u64  lbaBackup;
    u64  lbaBegin;
    u64  lbaEnd;
    Uuid diskIdentifier;
    u64  lbaEntries;
    u32  entrySize;
    u32  checksumEntries;

    Array<u8, 512 - 0x5c> __reserved__1;
};

} // namespace Gpt
