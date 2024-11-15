#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <filesystem>
#include <string>
#include <vector>

#include <sys/resource.h>

namespace oj {

class Subprocess {
public:
    ~Subprocess();
    Subprocess (
        const std::filesystem::path&    program,
        const std::vector<std::string>& args,
        std::istream*                   std_in = nullptr,
        std::ostream*                   std_out = nullptr,
        std::ostream*                   std_err = nullptr,
        int                             time_limit_sec = 0,
        int                             time_limit_usec = 0,
        int                             memory_limit_mb = 0
    );
    Subprocess(const Subprocess& other) = delete;
    Subprocess(Subprocess&& other) = delete;

    Subprocess& operator=(const Subprocess& other) = delete;
    Subprocess& operator=(Subprocess&& other) = delete;

    int    Poll();
    int    Wait();

    int    pid() const;
    int    status() const;
    rusage usage() const;

private:
    static void ExceptionHandler(); 
    static void MemoryLimitHandler(int sig);
    static void TimeLimitHandler(int sig);

    void OpenPipe();
    void ClosePipe();
    void ReadFromPipe(std::ostream* out);
    void WriteToPipe(std::istream* in);

    void SetTerminateHandler(std::terminate_handler handler = ExceptionHandler);
    void SetMemoryLimit(int memory_limit_mb, void (*handler)(int) = MemoryLimitHandler);
    void SetTimeLimit(int time_limit_sec, int time_limit_usec, void (*handler)(int) = TimeLimitHandler);

    bool is_parent();
    bool is_forked();

    pid_t  pid_;
    int    pipefd_[2];
    int    status_;
    rusage usage_;
};

}

#endif