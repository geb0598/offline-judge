#include "testcase.h"
#include "utility.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

TestCase::TestCase(size_t id, std::filesystem::path input_path, std::filesystem::path output_path) :
    id_(id),
    input_path_(input_path),
    output_path_(output_path) {}

const size_t TestCase::get_id() const {
    return id_;
}

std::string TestCase::get_input() const {
    std::ifstream input_file(input_path_);
    if (!input_file.is_open()) {
        throw std::runtime_error("Failed to open a file: " + input_path_.string());
    }

    std::ostringstream input_stream;
    input_stream << input_file.rdbuf();
    return input_stream.str();
}

std::string TestCase::get_output() const {
    std::ifstream output_file(output_path_);
    if (!output_file.is_open()) {
        throw std::runtime_error("Failed to open a file: " + output_path_.string()); 
    }

    std::ostringstream output_stream;
    output_stream << output_file.rdbuf();
    return output_stream.str();
}

const std::filesystem::path& TestCase::get_input_path() const {
    return input_path_;
}

const std::filesystem::path& TestCase::get_output_path() const {
    return output_path_;
}

void TestCase::Print() const {

}

TestCaseContainer::TestCaseContainer() {};
TestCaseContainer::TestCaseContainer(
        const std::filesystem::path& directory_path,
        const std::filesystem::path& input_extension, 
        const std::filesystem::path& output_extension) {
            Initialize(directory_path, input_extension, output_extension);
        }

void TestCaseContainer::Initialize(
        const std::filesystem::path& directory_path, 
        const std::filesystem::path& input_extension, 
        const std::filesystem::path& output_extension) {
    if (!std::filesystem::is_directory(directory_path)) {
        throw std::runtime_error(directory_path.string() + " is not a directory");
    }

    test_case_vec_.clear();

    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        if (!std::filesystem::is_regular_file(entry)) {
            continue;
        }

        auto input_path = entry.path();
        if (input_path.extension() != input_extension) {
            continue;
        }

        auto output_path = entry.path();
        output_path.replace_extension(output_extension);
        if (!std::filesystem::is_regular_file(output_path)) {
            throw std::runtime_error(output_path.string() + " does not exist or not a regular file");
        }

        AddTestCaseFile(input_path, output_path);
        }
}
void TestCaseContainer::AddTestCaseFile(const std::filesystem::path& input_path, const std::filesystem::path& output_path) {
    test_case_vec_.emplace_back(input_path, output_path);
}

void TestCaseContainer::ReplaceExtension(
        const std::filesystem::path& directory_path, 
        const std::filesystem::path& source_extension, 
        const std::string& target_extension) const {
    if (!std::filesystem::is_directory(directory_path)) {
        throw std::runtime_error(directory_path.string() + " is not a directory");
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        if (!std::filesystem::is_regular_file(entry)) {
            continue;
        }

        auto source_path = entry.path();
        if (source_path.extension() == source_extension) {
            auto target_path = entry.path();
            target_path.replace_extension(target_extension);
            std::filesystem::rename(source_path, target_path);
            DEBUG_MSG("Replace " + source_path.string() + " to " + target_path.string());
        }
    }
}

size_t TestCaseContainer::size() const {
    return test_case_vec_.size();
}

bool TestCaseContainer::empty() const {
    return test_case_vec_.empty();
}

TestCase TestCaseContainer::operator[](size_t index) const {
    if (index >= test_case_vec_.size()) {
        throw std::out_of_range("Out of range");
    }

    auto [input_path, output_path] = test_case_vec_[index];

    return TestCase(index, input_path, output_path);
}

TestCase TestCaseContainer::at(size_t index) const {
    return (*this).at(index);
}

void TestCaseContainer::Print() const {

}