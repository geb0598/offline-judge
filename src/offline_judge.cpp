#include <filesystem>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "exit_status.h"

#include "offline_judge.h"

#include "compilation_result.h"
#include "execution_result.h"
#include "judge_result.h"
#include "submission_result.h"

namespace oj {

std::shared_ptr<ExecutionResult> OfflineJudge::Execute (
    const std::filesystem::path& program, 
    int                          time_limit_sec,
    int                          time_limit_usec,
    int                          memory_limit_mb,
    const std::string&           input,
    const std::filesystem::path& output_file
) const {
    if (!std::filesystem::exists(program)) {
        int status = CreateExitStatus(ExitStatus::EXECUTION_PROGRAM_NOT_EXIST);
        std::string output;
        rusage usage;
        return CreateExecutionResult(status, program, input, output, usage);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        throw std::runtime_error("ERROR::OfflineJudge: Failed to open a pipe.");
    }

    pid_t pid = fork();
    if (pid < 0) {
        throw std::runtime_error("ERROR::OfflineJudge: Failed to fork a process with " + program.string() + ".");
    }
    if (pid == 0) {
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            exit(static_cast<int>(ExitStatus::EXECUTION_DUP_FAILURE));
        }
        close(pipefd[1]);

        SetTimeLimit(time_limit_sec, time_limit_usec);

        SetMemoryUsageLimit(memory_limit_mb);

        execl(program.string().c_str(), program.string().c_str(), nullptr);

        exit(static_cast<int>(ExitStatus::EXECUTION_EXEC_FAILURE));
    } else {
        close(pipefd[1]);

        std::string output = ReadFileDescriptiorToString(pipefd[0]);

        close(pipefd[0]);

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            throw std::runtime_error("ERROR::OfflineJudge: Failed to wait a child process.");
        }

        rusage usage;
        if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
            throw std::runtime_error("ERROR::OfflineJudge: Failed to get resource usage from a child process.");
        }

        if (!std::filesystem::is_empty(output_file)) {
            WriteStringToFile(output_file, output);
        }

        return CreateExecutionResult(status, program, input, output, usage);
    }
}

std::shared_ptr<ExecutionResult> OfflineJudge::ExecuteWithFile (
        const std::filesystem::path& program, 
        int                          time_limit_sec,
        int                          time_limit_usec,
        int                          memory_limit_mb,
        const std::filesystem::path& input_file,
        const std::filesystem::path& output_file
) const {
    if (!std::filesystem::exists(program)) {
        int status = CreateExitStatus(ExitStatus::EXECUTION_PROGRAM_NOT_EXIST);
        std::string input;
        std::string output;
        rusage usage;
        return CreateExecutionResult(status, program, input, output, usage);
    }

    if (!std::filesystem::exists(input_file)) {
        int status = CreateExitStatus(ExitStatus::EXECUTION_INPUT_NOT_EXIST);
        std::string input;
        std::string output;
        rusage usage;
        return CreateExecutionResult(status, program, input, output, usage);
    }

    std::string input = ReadFileToString(input_file);
    std::shared_ptr<ExecutionResult> result = Execute(program, time_limit_sec, time_limit_usec, memory_limit, input, output_file);

    return result;
}

std::shared_ptr<JudgeResult> OfflineJudge::Judge(const std::string& user_answer, const std::string& correct_answer) const {

}

bool OfflineJudge::IsModifiedLaterThan(const std::filesystem::path& lhs, const std::filesystem::path& rhs) const {
    if (!std::filesystem::exists(lhs) || !std::filesystem::exists(rhs)) {
        throw std::runtime_error("ERROR::OfflineJudge: " + lhs.string() + " and/or " + rhs.string() + " isn't exist.");
    }

    return std::filesystem::last_write_time(lhs) > std::filesystem::last_write_time(rhs);
}

int WriteStringToFile(const std::filesystem::path& file, const std::string& s) {
    std::ofstream out(file);
    if (!out.is_open()) {
        throw std::runtime_error("ERROR::OfflineJudge: Failed to open a file " + file.string() + ".");
    }
    out << s;
}

std::string OfflineJudge::ReadFileToString(const std::filesystem::path& file) const {
    if (!std::filesystem::exists(file)) {
        throw std::runtime_error("ERROR::OfflineJudge: " + file.string() + " doesn't exist.");
    }

    std::ifstream in(file);
    if (!in.is_open()) {
        throw std::runtime_error("ERROR::OfflineJudge: Failed to open a file " + file.string() + ".");
    }

    std::ostringstream sstream;
    sstream << in.rdbuf();
    return sstream.str();
}

std::string OfflineJudge::ReadFileDescriptiorToString(int fd) const {
    std::string s;
    char buffer[256];
    ssize_t bytes;
    while ((bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        s += buffer;
    }

    if (bytes < 0) {
        throw std::runtime_error("ERROR::OfflineJudge: Failed to read from file descriptor.");
    }
    return s;
}

void OfflineJudge::SetMemoryUsageLimit(int memory_limit_mb) const {
    if (memory_limit_mb == 0) {
        return;
    }

    rlimit limit;
    limit.rlim_cur = memory_limit_mb * 1024 * 1024;
    limit.rlim_max = memory_limit_mb * 1024 * 1024;

    if (setrlimit(RLIMIT_AS, &limit) == -1) {
        throw std::runtime_error("ERROR::OfflineJudge: Failed to set memory limit.");
    }
}

void OfflineJudge::SetTimeLimit(int time_limit_sec, int time_limit_usec, void (*handler)(int) = nullptr) const {
    if (time_limit_sec == 0 && time_limit_usec == 0) {
        return;
    }

    if (handler != nullptr) {
        signal(SIGALRM, handler);
    }

    itimerval timer;

    timer.it_value.tv_sec = time_limit_sec;
    timer.it_value.tv_usec = time_limit_usec;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, nullptr) == -1) {
        throw std::runtime_error("ERROR::OfflineJudge: Failed to set timer.");
    }
}

}