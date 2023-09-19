#include "testcase.h"
#include "utility.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

TestCaseContainer::TestCase::TestCase(const std::string& input, const std::string& output) : input_(input), output_(output) {}

TestCaseContainer::TestCase::TestCase(std::string&& input, std::string&& output) : input_(std::move(input)), output_(std::move(output)) {}

const size_t TestCaseContainer::TestCase::getId() const {
    return id;
}

const std::string& TestCaseContainer::TestCase::getInput() const {
    return input_;
}

const std::string& TestCaseContainer::TestCase::getOutput() const {
    return output_;
}

void TestCaseContainer::TestCase::printInfo() const {

}

TestCaseContainer::TestCaseContainer() {};

TestCaseContainer::TestCaseContainer(const std::string& directory, const std::string& input_extension, const std::string& output_extension) {
    initialize(directory, input_extension, output_extension);
}

void TestCaseContainer::addTestCaseFile(const std::string& input_filename, const std::string& output_filename) {
    test_case_filenames_.emplace_back(input_filename, output_filename);
}

void TestCaseContainer::initialize(const std::string& directory, const std::string& input_extension, const std::string& output_extension) {
    if (!std::filesystem::is_directory(directory)) {
        throw std::runtime_error(directory + " is not a directory");
    }

    test_case_filenames_.clear();

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
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

        addTestCaseFile(input_path.string(), output_path.string());

       }
}

size_t TestCaseContainer::size() const {
    return test_case_filenames_.size();
}

bool TestCaseContainer::empty() const {
    return test_case_filenames_.empty();
}

TestCaseContainer::TestCase TestCaseContainer::operator[](size_t index) const {
    if (index >= test_case_filenames_.size()) {
        throw std::out_of_range("Out of range");
    }

    auto [input_filename, output_filename] = test_case_filenames_[index];
    std::ifstream input_file(input_filename);
    if (!input_file.is_open()) {
        throw std::runtime_error("Failed to open a file: " + input_filename);
    }

    std::ifstream output_file(output_filename);
    if (!output_file.is_open()) {
        throw std::runtime_error("Failed to open a file: " + output_filename); 
    }

    std::ostringstream input_stream;
    input_stream << input_file.rdbuf();
    std::string input_string = input_stream.str();

    std::ostringstream output_stream;
    output_stream << output_file.rdbuf();
    std::string output_string = output_stream.str();
    
    return TestCase(std::move(input_string), std::move(output_string));
}

TestCaseContainer::TestCase TestCaseContainer::at(size_t index) const {
    return (*this)[index];
}


void TestCaseContainer::printInfo() const {

}