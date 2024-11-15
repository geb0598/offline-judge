#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "subprocess.h"

namespace oj {

int Subprocess::Poll() {
    if (is_terminated()) {
        return pid_;
    }

    int pid = wait4(pid_, &status_, WNOHANG, &usage_);
    if (pid == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to wait a child process with error.");
    }
    return pid;
}

int Subprocess::Wait() {
    if (is_terminated()) {
        return pid_;
    }

    int pid = wait4(pid_, &status_, NULL, &usage_);
    if (pid == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to wait a child process with error.");
    }
    return pid;
}

void Subprocess::ReadFromPipe(std::ostream& out) {
    pipe_in_->Read(out);
}

void Subprocess::WriteToPipe(std::istream& in) {
    pipe_out_->Write(in);
}

bool Subprocess::is_parent() const {
    return pid_ != 0;
}

bool Subprocess::is_forked() const {
    return pid_ != -1;
}

bool Subprocess::is_terminated() const {
    return is_terminated_;
}

bool Subprocess::is_input_pipe_opened() const {
    return pipe_in_ != nullptr;
}

bool Subprocess::is_output_pipe_opened() const {
    return pipe_out_ != nullptr;
}

int Subprocess::pid() const {
    return pid_;
}

int Subprocess::status() const {
    if (!is_terminated()) {
        throw std::runtime_error("ERROR::Subprocess: Can't get status until the process is terminated.");
    }
    return status_;
}

rusage Subprocess::usage() const {
    return usage_;
}

}