#ifndef ENGINE_LIBRARY_H
#define ENGINE_LIBRARY_H

#include <functional>
#include <utility>

template<typename T1, typename T2>
struct std::hash<std::pair<T1, T2>> {
    size_t operator()(const pair<T1, T2>& p) const {
        auto h1 = hash<T1>{}(p.first);
        auto h2 = hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

void test();

#endif //ENGINE_LIBRARY_H