#include <filesystem>
#include <vector>
#include <string>

#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "process.h"
#include "exit_status.h"

namespace oj {

static std::unique_ptr<Process> CreateProcess () {
    return std::make_unique<Process>();
}

static std::unique_ptr<Process> CreateProcess (
    const std::filesystem::path&    program, 
    const std::vector<std::string>& args,
    const FileDescriptor&           std_in,
    const FileDescriptor&           std_out,
    const FileDescriptor&           std_err) {
    std::unique_ptr<Process> process = std::make_unique<Process>();
    process->Fork();
    if (process->is_child()) {
        if (std_in.is_opened()) {
            FD_STD_IN.Redirect(std_in);
        }

        if (std_out.is_opened()) {
            FD_STD_OUT.Redirect(std_out);
        }

        if (std_err.is_opened()) {
            FD_STD_ERR.Redirect(std_err);
        }
    }
    process->Execute(program, args);
};

bool Process::HasInstance() {
    return has_instance_;
}

void Process::MemoryLimitHandler(int sig) {
    if (errno == ENOMEM) {
        exit(static_cast<int>(ExitStatus::OUT_OF_MEMORY));
    } else {
        signal(SIGSEGV, SIG_DFL);
        raise(SIGSEGV);
    }
}

void Process::TimeLimitHandler(int sig) {
    exit(static_cast<int>(ExitStatus::TIMEOUT));
}

void Process::ExceptionHandler() {
    try {
        std::exception_ptr eptr(std::current_exception());
        if (eptr) {
            std::rethrow_exception(eptr);
        }
    } catch (const std::bad_alloc& e) {
        exit(static_cast<int>(ExitStatus::EXCEPTION_BAD_ALLOC));
    } catch (const std::out_of_range& e) {
        exit(static_cast<int>(ExitStatus::EXCEPTION_OUT_OF_RANGE));
    } catch(const std::length_error& e) {
        exit(static_cast<int>(ExitStatus::EXCEPTION_LENGTH_ERROR));
    } catch(const std::invalid_argument& e) {
        exit(static_cast<int>(ExitStatus::EXCEPTION_INVALID_ARGUMENT));
    } catch (...) {
        exit(static_cast<int>(ExitStatus::EXCEPTION));
    }
}

Process::~Process () {
    ClosePipe();
    has_instance_ = false;
}

Process::Process() : pid_(-1), status_(-1) {
    has_instance_ = true;
}

void Process::Fork() {
    if (is_forked()) {
        throw std::runtime_error("ERROR::Process: Process is already forked.");
    }

    if ((pid_ = fork()) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to fork a process.");
    }
}

void Process::Execute(const std::filesystem::path& program, const std::vector<std::string>& args) {
    if (!is_forked()) {
        throw std::runtime_error("ERROR::Process: Process is not yet forked.");
    }

    if (!is_child()) {
        return;
    }

    std::unique_ptr<char*[]> c_args = GetCArgs(args);
    execv(program.string().c_str(), c_args.get());
    exit(EXIT_FAILURE);
}

void Process::Wait() {
    if (!is_parent()) {
        return;
    }

    if (!is_forked()) {
        throw std::runtime_error("ERROR::Process: Can't wait a process not forked.");
    }

    if (wait4(pid_, &status_, NULL, &usage_) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to wait a child process with error.");
    }
}

void Process::OpenPipe() {
    if (is_forked()) {
        throw std::runtime_error("ERROR::Process: Can't open a pipe after fork.");
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to open a pipe.");
    }
    pipe_in_ = FileDescriptor(pipefd[0]);
    pipe_out_ = FileDescriptor(pipefd[1]);
}

void Process::ClosePipe() {
    pipe_in_.Close();
    pipe_out_.Close();
}

void Process::ReadFromPipe(std::ostream& out, bool is_child) const {
    if (this->is_child() != is_child) {
        return;
    }

    pipe_in_.Read(out);
}

void Process::WriteToPipe(std::istream& in, bool is_child) const {
    if (this->is_child() != is_child) {
        return;
    }

    pipe_out_.Write(in);
}

void Process::SetSignalHandler(int sig, void (*handler)(int)) const {
    if (!is_child()) {
        return;
    }

    if(signal(sig, handler) == SIG_ERR) {
        throw std::system_error(errno, std::generic_category(), "ERROR::Process: Failed to set signal handler.");
    }
}

void Process::SetTerminateHandler(std::terminate_handler handler) const {
    if (!is_child()) {
        return;
    }

    std::set_terminate(handler);
}

void Process::SetMemoryLimit(int memory_limit_mb, void (*handler)(int)) const {
    if (!is_child() || memory_limit_mb == 0) {
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
    if (!is_child() || (time_limit_sec == 0 && time_limit_usec == 0)) {
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

bool Process::is_child() const {
    return pid_ == 0;
}

bool Process::is_parent() const {
    return !is_child();
}

bool Process::is_forked() const {
    return pid_ != -1;
}

bool Process::is_exited() const {
    return WIFEXITED(status_);
}

bool Process::is_signaled() const {
    return WIFSIGNALED(status_);
}

const FileDescriptor& Process::pipe_in() const {
    return pipe_in_;
}

const FileDescriptor& Process::pipe_out() const {
    return pipe_out_;
}

int Process::status() const {
    return status_;
}

int Process::execution_time_sec() const {
    if (!is_exited() && !is_signaled()) {
        throw std::runtime_error("ERROR::Process: Process is still executing.");
    }

    return usage_.ru_utime.tv_sec + usage_.ru_stime.tv_sec;
}

int Process::execution_time_usec() const {
    if (!is_exited() && !is_signaled()) {
        throw std::runtime_error("ERROR::Process: Process is still executing.");
    }    
    
    return usage_.ru_utime.tv_usec + usage_.ru_stime.tv_usec;
}

int Process::memory_usage_kb() const {
    if (!is_exited() && !is_signaled()) {
        throw std::runtime_error("ERROR::Process: Process is still executing.");
    }

    return usage_.ru_maxrss;
}

int Process::memory_usage_mb() const {
    if (!is_exited() && !is_signaled()) {
        throw std::runtime_error("ERROR::Process: Process is still executing.");
    }

    return usage_.ru_maxrss / KB; 
}

bool Process::has_instance_ = false;

std::unique_ptr<char*[]> Process::GetCArgs(const std::vector<std::string>& args) const {
    std::unique_ptr<char*[]> c_args(new char*[args.size() + 1]);
    for (size_t i = 0; i < args.size(); ++i) {
        c_args[i] = const_cast<char*>(args[i].c_str());
    }
    c_args[args.size()] = nullptr;
    return c_args;
}

}