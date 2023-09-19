#include "configuration.h"
#include "launcher.h"
#include "offlinejudge.h"
#include "solution.h"
#include "testcase.h"

#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Load Configuration...\n";
    auto& config = Configuration::getConfig();
    config.test();
    std::cout << "Complete to load Configuration\n";

    std::cout << "Initialize Launcher...\n";
    auto& launcher = Launcher::getLauncher();
    std::cout << "Complete to initialize Launcher\n";
    
    std::cout << "Load testcase...\n";
    TestCaseContainer testcase;
    std::cout << "Complete to initialize Testcase\n";

    std::cout << "Initialize OfflineJudge\n";
    auto& offline_judge = OfflineJudge::getOfflineJudge();
    std::cout << "Complete to initialize OfflineJudge\n";

    std::cout << "Load User Solution...\n";
    Solution user_solution(SolutionType::USER);
    user_solution.compile();
    std::cout << "Complete to load User Solution\n";
    
    std::cout << "Load Example Solution...\n";
    Solution example_solution(SolutionType::EXAMPLE); 
    example_solution.compile();
    std::cout << "Complete to load Example Solution\n";

    int idx = 0;
    for (auto input : testcase.getInputs()) {
        auto user_result = launcher.launch(user_solution.getBinaryFilename(), input);
        auto example_result = launcher.launch(example_solution.getBinaryFilename(), input);
        auto judge_result = offline_judge.judge(user_result.getFilename(), example_result.getFilename()); 
        std::cout << "[Test case]: " << idx << '\n';
        user_result.printInfo();
        example_result.printInfo();
        for (auto x : judge_result.getLineStats()) {
        if (x == JudgeResult::LineStatus::CORRECT) {
                std::cout << "correct\n";
            } else if (x == JudgeResult::LineStatus::WRONG) {
                std::cout << "wrong\n";
            } else {
                std::cout << "ill-formed\n";
            }
        }
    }
}