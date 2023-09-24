#pragma once

#include <chrono>
#include <filesystem>
#include <string>

class Solution {
public:
    Solution(std::filesystem::path source_path, std::filesystem::path binary_path, std::filesystem::path output_path);

    bool IsUpdated() const;
    void Compile(const std::string& compiler, const std::string& compiler_option) const;

    const std::filesystem::path& get_source_path() const;
    const std::filesystem::path& get_binary_path() const;
    const std::filesystem::path& get_output_path() const;

    void Print() const;

private:
    std::filesystem::path source_path_;
    std::filesystem::path binary_path_;
    std::filesystem::path output_path_;

    std::chrono::system_clock::time_point GetLastModifiedTime(const std::filesystem::path& path) const;
};