#pragma once

namespace Sys {

struct [[nodiscard]] UserBuffer {
public:
    UserBuffer() = delete;
};

struct [[nodiscard]] UserOrKernelBuffer { };

} // namespace Sys
