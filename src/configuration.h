#pragma once

#include <map>
#include <string>

class Configuration {
public:
    static Configuration& getConfig();

    const std::string& getValue(const std::string& key) const;

    void test() const;
    
private:
    Configuration(const std::string filename = "./config");
    Configuration(const Configuration& config) {};
    ~Configuration() {};

    void parseConfig();

    void leftTrim(std::string& s) const;
    void rightTrim(std::string& s) const;
    void trim(std::string& s) const;

    const std::string filename_;

    std::map<std::string, std::string> config_map_;
};