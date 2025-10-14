#pragma once

namespace Zgen::Core {

struct [[nodiscard]] UserBuffer {
public:
    UserBuffer() = delete;
};

struct [[nodiscard]] UserOrKernelBuffer { };

} // namespace Zgen::Core
