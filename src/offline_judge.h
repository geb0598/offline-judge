#ifndef OFFLINE_JUDGE_H
#define OFFLINE_JUDGE_H

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#include "exit_status.h"

#include "compilation_result.h"
#include "execution_result.h"
#include "judge_result.h"
#include "submission_result.h"

namespace oj {

class OfflineJudge {
public:
    static OfflineJudge& GetInstance() {
        static OfflineJudge instance;
        return instance;
    }

    OfflineJudge(const OfflineJudge& other) = delete;
    OfflineJudge(OfflineJudge&& other) noexcept = delete;

    OfflineJudge& operator=(const OfflineJudge& other) = delete;
    OfflineJudge& operator=(OfflineJudge&& other) noexcept = delete;

    template <typename... T>
    std::shared_ptr<CompilationResult> Compile(const std::filesystem::path& source, const std::filesystem::path& target, const std::string& compiler, T... args) {
        std::string options = Concatenate(args...);
        std::string command = Concatenate(compiler, source.string(), "-o", target.string(), options);

        if (!std::filesystem::exists(source)) {
            int status = CreateExitStatus(ExitStatus::COMPILATION_FILE_NOT_EXIST); 
            std::string message;
            return CreateCompilationResult(status, message, command, source, target);
        }

        if (std::filesystem::exists(target) && !IsModifiedLaterThan(source, target)) {
            int status = CreateExitStatus(ExitStatus::COMPILATION_FILE_UP_TO_DATE);
            std::string message;
            return CreateCompilationResult(status, message, command, source, target);
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
            if (dup2(pipefd[1], STDOUT_FILENO) == -1 || dup2(pipefd[1], STDERR_FILENO) == -1) {
                exit(static_cast<int>(ExitStatus::COMPILATION_DUP_FAILURE));
            }
            execl(program.string().c_str(), program.string().c_str(), nullptr);
            exit(static_cast<int>(ExitStatus::COMPILATION_EXEC_FAILURE));
        } else {
            close(pipefd[1]);

            std::string output = ReadFileDescriptiorToString(pipefd[0]);

            close(pipefd[0]);

            int status;
            if (waitpid(pid, &status, 0) == -1) {
                throw std::runtime_error("ERROR::OfflineJudge: Failed to wait a child process.");
            }

            if (WIFEXITED(status)) {
                return CreateCompilationResult(status, message, command, source, target);
            } else {
                throw std::runtime_error("ERROR::OfflineJudge: Failed to close a pipe.");
            }
        }
    }

    /*
    template <typename... T>
    std::shared_ptr<CompilationResult> Compile(const std::filesystem::path& source, const std::filesystem::path& target, const std::string& compiler, T... args) {
        std::string options = Concatenate(args...);
        std::string command = Concatenate(compiler, source.string(), "-o", target.string(), options);

        if (!std::filesystem::exists(source)) {
            int status = CreateExitStatus(ExitStatus::COMPILATION_FILE_NOT_EXIST); 
            std::string message;
            return CreateCompilationResult(status, message, command, source, target);
        }

        if (std::filesystem::exists(target) && !IsModifiedLaterThan(source, target)) {
            int status = CreateExitStatus(ExitStatus::COMPILATION_FILE_UP_TO_DATE);
            std::string message;
            return CreateCompilationResult(status, message, command, source, target);
        }

        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (pipe == nullptr) {
            throw std::runtime_error("ERROR::OfflineJudge: Failed to open a pipe with command \"" + command + "\".");
        }

        std::string message;
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
            message += buffer;
        }

        int status = pclose(pipe.release());
        if (WIFEXITED(status)) {
            return CreateCompilationResult(status, message, command, source, target);
        } else {
            throw std::runtime_error("ERROR::OfflineJudge: Failed to close a pipe.");
        }
    }
    */

    std::shared_ptr<ExecutionResult>   Execute(
        const std::filesystem::path& program, 
        int                          time_limit_sec, 
        int                          time_limit_usec, 
        int                          memory_limit_mb,
        const std::string&           input,
        const std::filesystem::path& output_file = std::filesystem::path()
    ) const;
    std::shared_ptr<ExecutionResult>   ExecuteWithFile (
        const std::filesystem::path& program, 
        int                          time_limit_sec,
        int                          time_limit_usec,
        int                          memory_limit_mb,
        const std::filesystem::path& input_file,
        const std::filesystem::path& output_file = std::filesystem::path()
    ) const;
    std::shared_ptr<JudgeResult>       Judge(const std::string& user_answer, const std::string& correct_answer) const;
    std::shared_ptr<JudgeResult>       JudgeWithFile (
        const std::filesystem::path& user_answer, 
        const std::filesystem::path& correct_answer
    ) const;
    std::shared_ptr<SubmissionResult>  Submit (
        const std::shared_ptr<CompilationResult>&            compilation_result,
        const std::vector<std::shared_ptr<ExecutionResult>>& execution_results,
        const std::vector<std::shared_ptr<JudgeResult>>&     judge_results 
    ) const;

private:
    ~OfflineJudge() = default;

    OfflineJudge() = default;

    static void TimeOutHandler(int signal) {
        exit(static_cast<int>(ExitStatus::EXECUTION_TIMEOUT));
    }

    template <typename... T>
    std::string Concatenate(T... args) {
        std::ostringstream os;
        (os << (os.tellp() ? " " : "") << args, ...);
        return os.str();
    }

    std::string ReadFileToString(const std::filesystem::path& file) const;
    std::string ReadFileDescriptiorToString(int fd) const;
    void        WriteStringToFile(const std::filesystem::path& file, const std::string& s) const;
    void        SetMemoryUsageLimit(int memory_limit_mb) const;
    void        SetTimeLimit(int time_limit_sec, int time_limit_usec, void (*handler)(int) = nullptr) const;
    bool        IsModifiedLaterThan(const std::filesystem::path& lhs, const std::filesystem::path& rhs) const;
};

}

#endif