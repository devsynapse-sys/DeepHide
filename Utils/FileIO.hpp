#ifndef FILEIO_HPP
#define FILEIO_HPP

#include <vector>
#include <string>
#include <fstream>

bool read_file(const std::string& filename, std::vector<uint8_t>& data) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) return false;
    data.assign(std::istreambuf_iterator<char>(ifs), {});
    return true;
}

bool write_file(const std::string& filename, const std::vector<uint8_t>& data) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) return false;
    ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

#endif