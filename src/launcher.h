#pragma once

#include <chrono>
#include <filesystem>
#include <iostream>
#include <vector>
#include <string>

#include <wait.h>
#include <sys/types.h>

class LaunchResult {
public:
    enum class Status {
        LR_SUCCESS,
        LR_TIMEOUT,
        LR_SEGFAULT,
        LR_OTHER_ERROR,
        LR_UNKNOWN
    };

    LaunchResult(const std::filesystem::path& path);
    LaunchResult(const std::filesystem::path& path, int status, std::chrono::system_clock::duration elapsed_time);

    std::filesystem::path get_result_path() const;
    std::string get_result() const;
    Status get_status() const;
    std::chrono::system_clock::duration get_elapsed_time() const;

    void Print() const;

private:
    Status ParseStatus(int status) const;

    std::filesystem::path result_path_;
    Status status_;
    std::chrono::system_clock::duration elapsed_time_;
};

class Launcher {
public:
    static const Launcher& GetLauncher();
    LaunchResult Launch(
        const std::filesystem::path& binary_path, 
        const std::filesystem::path& input_path, 
        const std::filesystem::path& output_path, 
        std::chrono::seconds timeout) const;

private:
    Launcher() {};
    Launcher(const Launcher& launcher) = delete;
    ~Launcher() {};
};