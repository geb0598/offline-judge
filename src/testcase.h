#pragma once

#include <filesystem>
#include <vector>
#include <string>

class TestCase {
public:
    TestCase(size_t id, std::filesystem::path input_path, std::filesystem::path output_path);

    const size_t get_id() const;
    std::string get_input() const;
    std::string get_output() const;
    const std::filesystem::path& get_input_path() const;
    const std::filesystem::path& get_output_path() const;

    void Print() const;

private:
    size_t id_;
    std::filesystem::path input_path_;
    std::filesystem::path output_path_;
};

class TestCaseContainer {
public:
    TestCaseContainer();
    TestCaseContainer(
        const std::filesystem::path& directory_path,
        const std::filesystem::path& input_extension, 
        const std::filesystem::path& output_extension);

    void Initialize(
        const std::filesystem::path& directory_path, 
        const std::filesystem::path& input_extension, 
        const std::filesystem::path& output_extension);
    void AddTestCaseFile(const std::filesystem::path& input_path, const std::filesystem::path& output_path);
    void ReplaceExtension(
        const std::filesystem::path& directory_path, 
        const std::filesystem::path& source_extension, 
        const std::string& target_extension) const;

    size_t size() const;
    bool empty() const;

    TestCase operator[](size_t index) const;
    TestCase at(size_t index) const;

    void Print() const;

private:
    std::vector<std::pair<std::filesystem::path, std::filesystem::path>> test_case_vec_;
};