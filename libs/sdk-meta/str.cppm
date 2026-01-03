module;

export module sdk:str;

import :types;

export constexpr inline bool cstrEq(char const* a, char const* b) {
    if (a == nullptr || b == nullptr) {
        return a == b;
    }
    while (*a && *b) {
        if (*a != *b) {
            return false;
        }
        a++;
        b++;
    }
    return *a == *b; // both should be '\0' at this point
}

export constexpr inline usize strlen(char const* str) {
    if (str == nullptr) {
        return 0;
    }
    usize len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

export constexpr inline int strcmp(char const* a, char const* b) {
    if (a == nullptr || b == nullptr) {
        return (a == b) ? 0 : (a == nullptr ? -1 : 1);
    }
    while (*a && *b) {
        if (*a != *b) {
            return (*a < *b) ? -1 : 1;
        }
        a++;
        b++;
    }
    if (*a == *b) {
        return 0;
    }
    return (*a == '\0') ? -1 : 1;
}

export constexpr inline int strncmp(char const* a, char const* b, usize n) {
    if (a == nullptr || b == nullptr) {
        return (a == b) ? 0 : (a == nullptr ? -1 : 1);
    }
    for (usize i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return (a[i] < b[i]) ? -1 : 1;
        }
        if (a[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

export constexpr inline char* strcpy(char* dest, char const* src) {
    if (dest == nullptr || src == nullptr) {
        return dest;
    }
    char* originalDest = dest;
    while ((*dest++ = *src++) != '\0')
        ;
    return originalDest;
}

export constexpr inline char* strncpy(char* dest, char const* src, usize n) {
    if (dest == nullptr || src == nullptr) {
        return dest;
    }
    char* originalDest = dest;
    usize i            = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return originalDest;
}

export constexpr inline void memset(void* s, int c, usize n) {
    if (s == nullptr) {
        return;
    }
    u8* ptr = static_cast<u8*>(s);
    for (usize i = 0; i < n; i++) {
        ptr[i] = static_cast<u8>(c);
    }
}

export constexpr inline void* memcpy(void* dest, void const* src, usize n) {
    if (dest == nullptr || src == nullptr) {
        return dest;
    }
    u8*       d = static_cast<u8*>(dest);
    u8 const* s = static_cast<u8 const*>(src);
    for (usize i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

export constexpr inline void* memmove(void* dest, void const* src, usize n) {
    if (dest == nullptr || src == nullptr) {
        return dest;
    }
    u8*       d = static_cast<u8*>(dest);
    u8 const* s = static_cast<u8 const*>(src);
    if (d < s) {
        for (usize i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else if (d > s) {
        for (usize i = n; i != 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return dest;
}
