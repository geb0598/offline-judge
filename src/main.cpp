#include "launcher.h"
#include "offlinejudge.h"
#include "solution.h"
#include "testcase.h"
#include "utility.h"

#include "mini/ini.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;
    file.read(ini);

    DEBUG_MSG("Initializing Launcher...");
    auto& launcher = Launcher::GetLauncher();
    DEBUG_MSG("Complete to initialize Launcher");

    DEBUG_MSG("Initializing Offline Judge...");
    auto& offline_judge = OfflineJudge::GetOfflineJudge();
    DEBUG_MSG("Complete to initialize Offline Judge");

    DEBUG_MSG("Initializing TestCaseContainer...");
    TestCaseContainer tc_container(ini["testcase"]["dir"], ini["testcase"]["in"], ini["testcase"]["out"]);  
    DEBUG_MSG("Complete to initialize TestCaseContainer");

    DEBUG_MSG("Initializing User Solution...");
    Solution solution(ini["user"]["src"], ini["user"]["bin"], ini["user"]["out"]);
    DEBUG_MSG("Complete to initialize User Solution");

    DEBUG_MSG("Compiling User Solution...");
    solution.Compile(ini["compiler"]["compiler"], ini["compiler"]["option"]);
    DEBUG_MSG("Complete to compile User Solution");
    for (size_t i = 0; i < tc_container.size(); ++i) {
        auto tc = tc_container[i];
        auto launch_result = launcher.Launch(
            solution.get_binary_path(), 
            tc.get_input_path(), 
            solution.get_output_path(),
            std::chrono::seconds(10)
        );
        auto judge_result = offline_judge.Judge(tc.get_id(), launch_result.get_result(), tc.get_output());

        std::cout << std::left;
        std::cout << std::setw(10) << "[TEST CASE] : " << i << std::endl;
        std::cout << std::setw(10) << "[USER]" << '|' << "[EXAMPLE]" << std::endl;
        std::istringstream user_stream(launch_result.get_result()), example_stream(tc.get_output());
        for (size_t i = 0; i < judge_result.get_line_count(); ++i) {
            std::string user_string, example_string;
            std::getline(user_stream, user_string);
            std::getline(example_stream, example_string);
            switch (judge_result.get_line_status(i)) {
                case JudgeResult::LineStatus::JRL_CORRECT:
                    std::cout << ANSI_GREEN;
                    break;
                case JudgeResult::LineStatus::JRL_WRONG:
                    std::cout << ANSI_RED;
                    break;
                case JudgeResult::LineStatus::JRL_ILL_FORMED:
                    std::cout << ANSI_YELLOW;
                    break;
            }
            std::cout << std::setw(10) << user_string << '|' << example_string << std::endl;
            std::cout << ANSI_RESET;
        }
    }
}