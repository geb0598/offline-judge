#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <wait.h>
#include <sys/types.h>



class Launcher {
public:
    class LaunchResult {
    public:
        enum class LaunchStatus {
            SUCCESS,
            TIMEOUT,
            SEGFAULT,
            OTHER_ERROR
        };

        LaunchResult(int stat, const std::string& filename, double elapsed_time);

        const LaunchStatus& getStatus() const; 
        const std::string& getFilename() const;
        const double getElapsedTime() const;

        void printInfo() const; 

    private:
        void setStatus(int status);

        LaunchStatus status_;
        std::string filename_;
        double elapsed_time_;
    };

    static const Launcher& getLauncher();
    LaunchResult launch(const std::string& bin_filename, const std::string& input_filename, const std::string& output_filename, const double timeout_seconds) const;

private:
    Launcher() {};
    Launcher(const Launcher& launcher) {};
    ~Launcher() {};
};