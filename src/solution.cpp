#include "solution.h"

#include "utility.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <ctime>
#include <cstdlib>

#include <sys/stat.h>

Solution::Solution(std::filesystem::path source_path, std::filesystem::path binary_path, std::filesystem::path output_path):
    source_path_(source_path),
    binary_path_(binary_path),
    output_path_(output_path) {}

bool Solution::IsUpdated() const {
    if (!std::filesystem::exists(source_path_)) {
        throw std::runtime_error("Failed to open file: " + source_path_.string());
    }

    return !std::filesystem::exists(binary_path_) || GetLastModifiedTime(source_path_) > GetLastModifiedTime(binary_path_);
}

void Solution::Compile(const std::string& compiler, const std::string& compiler_option) const {
    if (!IsUpdated()) {
        DEBUG_MSG("File is already up-to-date");
        return;
    }
    
    std::string compile_command = compiler + " " + compiler_option + " " + source_path_.string() + " -o " + binary_path_.string();

    int compile_result = std::system(compile_command.c_str());
    if (compile_result != 0) {
        throw std::runtime_error("Failed to compile source file: " + source_path_.string());
    } else {
        DEBUG_MSG("Compilation Successful");
        DEBUG_MSG("File created at " + binary_path_.string());
    }
}

const std::filesystem::path& Solution::get_source_path() const {
    return source_path_;
}

const std::filesystem::path& Solution::get_binary_path() const {
    return binary_path_;
}

const std::filesystem::path& Solution::get_output_path() const {
    return output_path_;
}

void Solution::Print() const {
}

std::chrono::system_clock::time_point Solution::GetLastModifiedTime(const std::filesystem::path& path) const { 
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    struct stat file_stat;
    if (stat(path.c_str(), &file_stat) == -1) {
        throw std::runtime_error("Failed to fetch file status: " + path.string());
    }
    return std::chrono::system_clock::from_time_t(file_stat.st_mtim.tv_sec);
}