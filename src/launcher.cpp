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


Launcher::LaunchResult::LaunchResult(int status, const std::string& filename, double elapsed_time) : filename_(filename), elapsed_time_(elapsed_time) {
    setStatus(status);
}

const Launcher::LaunchResult::LaunchStatus& Launcher::LaunchResult::getStatus() const {
    return status_;
}

const std::string& Launcher::LaunchResult::getFilename() const {
    return filename_;
}

const double Launcher::LaunchResult::getElapsedTime() const {
    return elapsed_time_;
}

void Launcher::LaunchResult::setStatus(int status) {
    if (WIFEXITED(status)) {
        status_ = LaunchStatus::SUCCESS;
    } else if (WIFSIGNALED(status)) {
        if (WTERMSIG(status) == SIGSEGV) {
            status_ = LaunchStatus::SEGFAULT;
        } else if (WTERMSIG(status) == SIGTERM) {
            status_ = LaunchStatus::TIMEOUT;
        } else {
            status_ = LaunchStatus::OTHER_ERROR;
        }
    } else {
        status_ = LaunchStatus::OTHER_ERROR;
    }
}

void Launcher::LaunchResult::printInfo() const {
    std::cout << "[LaunchResult Info]\n";
    std::cout << "\t[Result]: \n" << filename_ << '\n';
    std::cout << "\t[Elapsed Time]: " << elapsed_time_ << " sec\n";

}

const Launcher& Launcher::getLauncher() {
    static Launcher launcher;
    return launcher;
}

Launcher::LaunchResult Launcher::launch(const std::string& bin_filename, const std::string& input_filename, const std::string& output_filename, const double timeout_seconds) const {
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Failed to fork a process");
    } else if (pid == 0) {
        int input_fd = open(input_filename.c_str(), O_RDONLY);

        if (input_fd == -1) {
            DEBUG_MSG("Failed to open an input file: " + input_filename);
            exit(EXIT_FAILURE);
        }

        if (dup2(input_fd, STDIN_FILENO) == -1) {
            DEBUG_MSG("Failed to redirect standard input to other file");
            exit(EXIT_FAILURE);
        }

        int output_fd = open(output_filename.c_str(), O_WRONLY);

        if (output_fd == -1) {
            DEBUG_MSG("Failed to open an ouput file: " + output_filename);
            exit(EXIT_FAILURE);
        }

        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            DEBUG_MSG("Failed to redirect standard output to other file");
            exit(EXIT_FAILURE);
        }
        execl(bin_filename.c_str(), bin_filename.c_str(), nullptr); 
    }
    auto start = std::chrono::system_clock::now();

    int status;
    double elapsed_time;
    while (true) {
        elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count();
        if (elapsed_time >= timeout_seconds) {
            kill(pid, SIGTERM);
        }

        if (waitpid(pid, &status, WNOHANG) != 0) {
            break;
        }
    }
    return LaunchResult(status, output_filename, elapsed_time);
}