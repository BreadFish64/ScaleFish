#include "file_util.hpp"

#include <fstream>
#include <iterator>

std::string StringFromFile(const std::filesystem::path& path) {
    std::ifstream file{path};
    return {std::istreambuf_iterator<char>{file}, {}};
}
