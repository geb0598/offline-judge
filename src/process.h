#include <filesystem>
#include <vector>
#include <string>
#include <memory>

namespace oj {

class Process {
public:
    static std::unique_ptr<Process> CreateProcess (
        const std::filesystem::path&    program,
        const std::vector<std::string>& args,
        bool                            should_open_pipe = false) {
        if (HasInstance()) {
            return nullptr;
        } else {
            return std::make_unique<Process>(program, args, should_open_pipe);
        }
    }

    static bool HasInstance() {
        return has_instance_;
    }

    ~Process();
    Process (
        const std::filesystem::path&    program,
        const std::vector<std::string>& args,
        bool                            should_open_pipe = false);
    Process (
        const std::filesystem::path&    program,
        const std::vector<std::string>& args,
        int                             pipefd[2]);
    Process(const Process& other) = delete;
    Process(Process&& other) noexcept = delete;

    Process& operator=(const Process& other) = delete;
    Process& operator=(Process&& other) noexcept = delete;

    void Execute();
    void SetSignalHandler(int sig, void (*handler)(int)) const;
    void SetMemoryLimit(int memory_limit_mb, void (*handler)(int) = nullptr) const;
    void SetTimeLimit(int time_limit_sec, int time_limit_usec, void (*handler)(int) = nullptr) const;
    void RedirectReadPipe(int fd, bool should_close_read_pipe, bool is_child);
    void RedirectWritePipe(int fd, bool should_close_write_pipe, bool is_child);
    void ReadFromPipe(std::ostream& os, bool is_child) const;
    void WriteToPipe(std::istream& is, bool is_child) const;
    bool IsChildProcess() const;
    bool IsParentProcess() const;

private:
    static constexpr int KB = 1024;
    static constexpr int MB = 1024 * 1024;

    static bool has_instance_;

    void                      OpenPipe();
    void                      ClosePipe();
    void                      CloseFileDescriptor(int fd);
    void                      RedirectFileDescriptor(int source, int target, bool should_close_source);
    std::unique_ptr<char* []> GetCArgs() const;

    pid_t                     pid_;
    int                       pipefd_[2];
    std::filesystem::path     program_;
    std::vector<std::string>  args_;




};

}