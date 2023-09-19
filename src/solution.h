#pragma once

#include <chrono>
#include <string>

class Solution {
public:
    Solution(const std::string& compiler = "g++", const std::string& compiler_option = "-std=c++17");

    bool isUpdated() const;
    void compile() const;

    std::string getSourceFilename() const;
    std::string getBinaryFilename() const;

    void printInfo() const;

private:
    std::string source_filename_;
    std::string binary_filename_;

    std::string compiler_;
    std::string compiler_option_;

    bool hasFile(const std::string& filename) const;
    std::time_t getLastModifiedTime(const std::string& filename) const;
};