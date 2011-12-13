#ifndef STRING_H
#define STRING_H

#include "Vector.h"
#include <cstring>

namespace Utils {

namespace Private {
__attribute__((transaction_safe))
int strncmp(const char *s1, const char *s2, size_t n);
} // namespace Private


class String: public Utils::Vector<char> {
public:
    __attribute__((transaction_safe))
    String()
        : Vector() {
        // nothing
    }

    __attribute__((transaction_safe))
    String(const String& s)
        : Vector(s) {
        // nothing
    }

    bool operator<(const String& vec) const {
        if (m_size < vec.m_size) {
            return true;
        } else if (m_size > vec.m_size) {
            return false;
        }

        return (Private::strncmp(m_data, vec.m_data, m_size) < 0);
    }
};

} // namespace Utils

#endif // STRING_H


