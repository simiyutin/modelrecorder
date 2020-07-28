#pragma once

#include <vector>
#include <fstream>

template<typename T>
inline std::vector<T> loadArray(const char * filename) {
    std::ifstream is(filename, std::ios::binary);
    typename std::vector<T>::size_type size;
    is.read((char *) &size, sizeof(size));
    std::vector<T> result(size);
    is.read((char *) result.data(), size * sizeof(T));

    return result;
}

template <typename T>
inline void dumpArray(const std::string & filename, const std::vector<T> & array) {
    std::ofstream os(filename, std::ios::binary);
    auto size = array.size();
    os.write((const char *) &size, sizeof(size));
    os.write((const char *) array.data(), array.size() * sizeof(T));
}