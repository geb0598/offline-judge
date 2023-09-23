#include "launcher.h"
#include "utility.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

LaunchResult::LaunchResult(const std::filesystem::path& path) :
    path_(path),
    status_(),
    elapsed_time_() {}

LaunchResult::LaunchResult(const std::filesystem::path& path, int status, std::chrono::system_clock::duration elapsed_time) :
    path_(path),
    status_(ParseStatus(status)),
    elapsed_time_(elapsed_time) {}

std::filesystem::path LaunchResult::get_path() const {
    return path_;
}

LaunchResult::Status LaunchResult::get_status() const {
    return status_;
}

std::chrono::system_clock::duration LaunchResult::get_elapsed_time() const {
    return elapsed_time_;
}

LaunchResult::Status LaunchResult::ParseStatus(int status) const {
    if (WIFEXITED(status)) {
        return Status::LR_SUCCESS;
    }

    if (WIFSIGNALED(status)) {
        switch (WTERMSIG(status)) {
            case SIGSEGV:
                return Status::LR_SEGFAULT;
            case SIGTERM:
                return Status::LR_TIMEOUT;
            default:
                return Status::LR_OTHER_ERROR;
        }
    }

    return Status::LR_UNKNOWN;
}

void LaunchResult::Print() const {

}

const Launcher& Launcher::getLauncher() {
    static Launcher launcher;
    return launcher;
}

LaunchResult Launcher::Launch(
        const std::filesystem::path binary_path, 
        const std::filesystem::path& input_path, 
        const std::filesystem::path& output_path, 
        std::chrono::system_clock::duration timeout) const {
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Failed to fork a process");
    } else if (pid == 0) {
        int input_fd = open(input_path.c_str(), O_RDONLY);

        if (input_fd == -1) {
            DEBUG_MSG("Failed to open an input file: " + input_path.string());
            exit(EXIT_FAILURE);
        }

        if (dup2(input_fd, STDIN_FILENO) == -1) {
            DEBUG_MSG("Failed to redirect standard input to other file");
            exit(EXIT_FAILURE);
        }

        int output_fd = open(output_path.c_str(), O_WRONLY);

        if (output_fd == -1) {
            DEBUG_MSG("Failed to open an ouput file: " + output_path.string());
            exit(EXIT_FAILURE);
        }

        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            DEBUG_MSG("Failed to redirect standard output to other file");
            exit(EXIT_FAILURE);
        }
        execl(binary_path.c_str(), binary_path.c_str(), nullptr); 
    }
    auto start = std::chrono::system_clock::now();

    int status;
    std::chrono::system_clock::duration elapsed_time;
    while (true) {
        elapsed_time = std::chrono::system_clock::now() - start;
        if (elapsed_time >= timeout) {
            kill(pid, SIGTERM);
        }

        if (waitpid(pid, &status, WNOHANG) != 0) {
            break;
        }
    }
    return LaunchResult(output_path, status, elapsed_time);
}