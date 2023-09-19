#pragma once

#include <filesystem>
#include <vector>
#include <string>

class TestCaseContainer {
public:
    class TestCase {
    public:
        TestCase(const std::string& input, const std::string& output);
        TestCase(std::string&& input, std::string&& output);

        const size_t getId() const;
        const std::string& getInput() const;
        const std::string& getOutput() const;

        void printInfo() const;

    private:
        size_t id;
        std::string input_;
        std::string output_;
    };

    TestCaseContainer();
    TestCaseContainer(const std::string& directory, const std::string& input_extension, const std::string& output_extension);

    void initialize(const std::string& directory, const std::string& input_extension, const std::string& output_extension=".out");
    void addTestCaseFile(const std::string& input_filename, const std::string& output_filename);

    size_t size() const;
    bool empty() const;

    TestCase operator[](size_t index) const;
    TestCase at(size_t index) const;

    void printInfo() const;

private:
    std::vector<std::pair<std::string, std::string>> test_case_filenames_;
};