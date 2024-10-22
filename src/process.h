#ifndef PROCESS_H
#define PROCESS_H

#include <filesystem>
#include <vector>
#include <string>
#include <memory>

#include "file_descriptor.h"

namespace oj {

class Process {
public:
    static std::unique_ptr<Process> CreateProcess ();
    static std::unique_ptr<Process> CreateProcess (
        const std::filesystem::path&    program, 
        const std::vector<std::string>& args,
        const FileDescriptor&           std_in = FD_EMPTY,
        const FileDescriptor&           std_out = FD_EMPTY,
        const FileDescriptor&           std_err = FD_EMPTY);

    static bool HasInstance();

    static void MemoryLimitHandler(int sig);

    static void TimeLimitHandler(int sig);

    static void ExceptionHandler();

    ~Process();
    Process();
    Process(const Process& other) = delete;
    Process(Process&& other) noexcept = delete;

    Process& operator=(const Process& other) = delete;
    Process& operator=(Process&& other) noexcept = delete;

    void                  Fork();
    void                  Execute(const std::filesystem::path& program, const std::vector<std::string>& args);
    void                  Wait();
    void                  OpenPipe();
    void                  ClosePipe();
    void                  ReadFromPipe(std::ostream& out, bool is_child) const;
    void                  WriteToPipe(std::istream& in, bool is_child) const;
    void                  SetSignalHandler(int sig, void (*handler)(int)) const;
    void                  SetTerminateHandler(std::terminate_handler handler = ExceptionHandler) const;
    void                  SetMemoryLimit(int memory_limit_mb, void (*handler)(int) = MemoryLimitHandler) const;
    void                  SetTimeLimit(int time_limit_sec, int time_limit_usec, void (*handler)(int) = TimeLimitHandler) const;

    bool                  is_child() const;
    bool                  is_parent() const;
    bool                  is_forked() const;
    bool                  is_exited() const;
    bool                  is_signaled() const;

    const FileDescriptor& pipe_in() const;
    const FileDescriptor& pipe_out() const;

    int                   status() const;
    int                   execution_time_sec() const;
    int                   execution_time_usec() const;
    int                   memory_usage_kb() const;
    int                   memory_usage_mb() const;

private:
    static constexpr int      KB = 1024;
    static constexpr int      MB = 1024 * 1024;

    static bool               has_instance_;

    std::unique_ptr<char* []> GetCArgs(const std::vector<std::string>& args) const;

    pid_t                     pid_;
    FileDescriptor            pipe_in_;
    FileDescriptor            pipe_out_;
    int                       status_;
    rusage                    usage_;
};

}

#endif