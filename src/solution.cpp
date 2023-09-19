#include "configuration.h"
#include "solution.h"

#include "utility.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <ctime>
#include <cstdlib>

#include <sys/stat.h>

Solution::Solution(const std::string& compiler, const std::string& compiler_option):
        compiler_(compiler), compiler_option_(compiler_option) {}

bool Solution::isUpdated() const {
    if (!hasFile(source_filename_)) {
        throw std::runtime_error("Failed to open file: " + source_filename_);
    }

    return !hasFile(binary_filename_) || getLastModifiedTime(source_filename_) > getLastModifiedTime(binary_filename_);
}

void Solution::compile() const {
    if (!isUpdated()) {
        DEBUG_MSG("File is already up-to-date");
        return;
    }
    
    std::string compile_command = compiler_ + " " + compiler_option_ + " " + source_filename_ + " -o " + binary_filename_;

    int compile_result = std::system(compile_command.c_str());
    if (compile_result != 0) {
        throw std::runtime_error("Failed to compile source file: " + source_filename_);
    } else {
        DEBUG_MSG("Compilation Successful");
        DEBUG_MSG("File created at " + binary_filename_);
    }
}

std::string Solution::getSourceFilename() const {
    return source_filename_;
}

std::string Solution::getBinaryFilename() const {
    return binary_filename_;
}

void Solution::printInfo() const {
    std::cout << "[Solution Info]" << std::endl;
    std::cout << "\tCompile Command: " + compiler_ + " " + compiler_option_ + " " + source_filename_ + " -o " + binary_filename_ << std::endl;
    std::cout << "\tSource Filename: " + source_filename_ << std::endl;
    std::cout << "\tLast Modified Time of Source File: " << getLastModifiedTime(source_filename_) << std::endl;
    std::cout << "\tBinary Filename: " + binary_filename_ << std::endl;
    std::cout << "\tLast Modified Time of Binary File: " << getLastModifiedTime(binary_filename_) << std::endl;
}

bool Solution::hasFile(const std::string& filename) const {
    std::ifstream file(filename);
    return file.good();
}

std::time_t Solution::getLastModifiedTime(const std::string& filename) const {
    if (!hasFile(filename)) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    struct stat file_stat;
    if (stat(filename.c_str(), &file_stat) == -1) {
        throw std::runtime_error("Failed to fetch file status: " + filename);
    }
    return file_stat.st_mtim.tv_sec;
}