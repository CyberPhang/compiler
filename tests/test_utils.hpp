#ifndef TEST_UTILS_
#define TEST_UTILS_

#include <string>
#include <cstdlib>
#include <iostream>
#include "../include/lexer.hpp"

// Macro to act as generic assertion statement.
// Args:
// - a, b - The two items to compare. We assume that the types of a 
//          and b overload operator!=. There must exist an overload
//          of operator<< for std::ostream that accepts the types of
//          a and b.
// Returns true if and only if a is equal to b
#define TTC_ASSERT_EQ(a, b) if ((a) != (b)) { \
    std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ \
              << " expected " << (b) << " got " << (a) << "\n"; \
    return false; \
}

// Concept to define types that support printing to std::ostream
template <typename T>
concept Printable = requires(std::ostream& os, const T& val) {
    { os << val } -> std::convertible_to<std::ostream&>;
};

template <Printable T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i < v.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

// Operator overload for std::vector. The type T must overload <<
// with std::ostream.
template <Printable T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);

#endif  // TEST_UTILS_