#include <filesystem>
#include <vector>
#include <string>

#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "process.h"

namespace oj {

Process::~Process () {
    ClosePipe();
    has_instance_ = false;
}

Process::Process (
    const std::filesystem::path&    program,
    const std::vector<std::string>& args,
    bool                            should_open_pipe) 
    : program_(program), args_(args) {
    if (has_instance_) {
        throw std::runtime_error("ERROR::Process: Process can have only one instance.");
    }

    if (should_open_pipe) {
        OpenPipe();
    } else {
        pipefd_[0] = -1;
        pipefd_[1] = -1;
    }

    if ((pid_ = fork()) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to fork a process.");
    }

    has_instance_ = true;
}

void Process::Execute() {
    if (!IsChildProcess()) {
        return;
    }

    ClosePipe();

    std::unique_ptr<char*[]> c_args = GetCArgs();
    execv(program_.string().c_str(), c_args.get());
    exit(EXIT_FAILURE);
}

void Process::SetSignalHandler(int sig, void (*handler)(int)) const {
    if (!IsChildProcess()) {
        return;
    }

    if(signal(sig, handler) == SIG_ERR) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to set signal handler.");
    }
}

void Process::SetMemoryLimit(int memory_limit_mb, void (*handler)(int)) const {
    if (!IsChildProcess() || memory_limit_mb == 0) {
        return;
    }

    if (handler != nullptr) {
        SetSignalHandler(SIGSEGV, handler);
    }

    rlimit limit;
    limit.rlim_cur = memory_limit_mb * MB;
    limit.rlim_max = memory_limit_mb * MB;
    if (setrlimit(RLIMIT_AS, &limit) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to set memory limit.");
    }
}

void Process::SetTimeLimit(int time_limit_sec, int time_limit_usec, void (*handler)(int)) const {
    if (!IsChildProcess() || (time_limit_sec == 0 && time_limit_usec == 0)) {
        return;
    }

    if (handler != nullptr) {
        SetSignalHandler(SIGALRM, handler);
    }

    itimerval timer;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    timer.it_value.tv_sec = time_limit_sec;
    timer.it_value.tv_usec = time_limit_usec;

    if (setitimer(ITIMER_REAL, &timer, nullptr) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to set a timer.");
    }
}

void Process::RedirectReadPipe(int fd, bool should_close_read_pipe, bool is_child) {
    if (IsChildProcess() != is_child) {
        return;
    }

    if (pipefd_[0] == -1) {
        throw std::runtime_error("ERROR::Process: Read pipe is not opened.");
    }

    RedirectFileDescriptor(pipefd_[0], fd, should_close_read_pipe);
}

void Process::RedirectWritePipe(int fd, bool should_close_write_pipe, bool is_child) {
    if (IsChildProcess() != is_child) {
        return;
    }

    if (pipefd_[1] == -1) {
        throw std::runtime_error("ERROR::Process: Write pipe is not opened.");
    }

    RedirectFileDescriptor(pipefd_[1], fd, should_close_write_pipe);
}

void Process::ReadFromPipe(std::ostream& os, bool is_child) const {
    if (IsChildProcess() != is_child) {
        return;
    }

    if (pipefd_[0] == -1) {
        throw std::runtime_error("ERROR::Process: Read pipe is not opened.");
    }

    char buf[256];
    ssize_t bytes;
    while ((bytes = read(pipefd_[0], buf, sizeof(buf))) > 0) {
        os.write(buf, bytes);
    }

    if (bytes < 0) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to read from a pipe.");
    }
}

void Process::WriteToPipe(std::istream& is, bool is_child) const {
    if (IsChildProcess() != is_child) {
        return;
    }

    if (pipefd_[1] == -1) {
        throw std::runtime_error("ERROR::Process: Write pipe is not opened.");
    }

    char buf[256];
    while (is.read(buf, sizeof(buf)) || is.gcount() > 0) {
        ssize_t total_bytes = 0; 
        ssize_t bytes_to_write = is.gcount();
        while (total_bytes < bytes_to_write) {
            ssize_t bytes = write(pipefd_[1], buf + total_bytes, bytes_to_write - total_bytes);
            if (bytes < 0) {
                throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to write to a pipe.");
            }
            total_bytes += bytes;
        }
    }
}

bool Process::IsChildProcess() const {
    return pid_ == 0;
}

bool Process::IsParentProcess() const {
    return !IsChildProcess();
}

bool Process::has_instance_ = false;

void Process::OpenPipe() {
    if (pipe(pipefd_) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to open a pipe.");
    }
}

void Process::ClosePipe() {
    CloseFileDescriptor(pipefd_[0]);
    CloseFileDescriptor(pipefd_[1]);
}

void Process::CloseFileDescriptor(int fd) {
    if (fd == -1) {
        return;
    }

    if (close(fd) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to close a file descriptor.");
    }

    fd = -1;
}

void Process::RedirectFileDescriptor(int source, int target, bool should_close_source) {
    if (dup2(source, target) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to duplicate a file descriptor.");
    }

    if (should_close_source) {
        CloseFileDescriptor(source);
    }
}

std::unique_ptr<char*[]> Process::GetCArgs() const {
    std::unique_ptr<char*[]> args(new char*[args_.size() + 1]);
    for (size_t i = 0; i < args_.size(); ++i) {
        args[i] = const_cast<char*>(args_[i].c_str());
    }
    args[args_.size()] = nullptr;
    return args;
}

}