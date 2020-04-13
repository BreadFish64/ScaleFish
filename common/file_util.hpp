#pragma once

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <tuple>

namespace {
using namespace std::filesystem;
}

std::string StringFromFile(const path& path) {
    std::ifstream file{path};
    return std::string(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
}
