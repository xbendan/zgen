#pragma once

#include <sdk-meta/slice.h>
#include <sdk-meta/types.h>

namespace Sdk::Crypto {

extern u32 crc32(Bytes data, u32 seed = 0xFFFF'FFFF);

enum _ {
    Aes,
    Sha256,
    Sha512,
    Md5,
    Crc32,
    Blake2b,
    Blake3,
    // Add more algorithms as needed
};

template <_>
u32 encode(Bytes data, u32 seed = 0);

template <>
u32 encode<Crc32>(Bytes data, u32 seed) {
    return crc32(data, seed);
}

} // namespace Sdk::Crypto
