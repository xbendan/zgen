[[noreturn]] void panic(char const* message) {
    __builtin_unreachable();
}
