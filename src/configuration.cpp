#include "configuration.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

Configuration& Configuration::getConfig() {
    static Configuration config;
    return config;
}

const std::string& Configuration::getValue(const std::string& key) const {
    auto it = config_map_.find(key);
    if (it == config_map_.end()) {
        throw std::out_of_range("Key not found in configuration: " + key);
    } else {
        return it->second;
    }
}

Configuration::Configuration(std::string filename) : filename_(filename) {
    try {
        parseConfig();
    } catch (const std::exception& e) {
        throw;
    }
}

void Configuration::parseConfig() {
    std::ifstream file(filename_);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open configuration file: " + filename_);
    }

    std::string line, section;
    while (std::getline(file, line)) {
        if (line.empty())
            continue;
        
        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.length() - 2);
            trim(section);
            std::for_each(section.begin(), section.end(), [](char& c) { c = std::tolower(c); });
        } else {
            std::stringstream ss(line);
            std::string key, value;
            if (std::getline(ss, key, '=') && std::getline(ss, value)) {
                trim(key);
                trim(value);
                config_map_[section + "_" + key] = value;
            } else {
                throw std::runtime_error("Malformed line in configuration file: " + line);
            }
        }
    }
}

void Configuration::leftTrim(std::string& s) const {
    auto pos = s.find_first_not_of(' ');
    if (pos != std::string::npos) {
        s.erase(0, pos);
    } else {
        s.clear();
    }
}

void Configuration::rightTrim(std::string& s) const {
    auto pos = s.find_last_not_of(' ');
    if (pos != std::string::npos) {
        s.erase(pos + 1, s.length() - (pos + 1));
    } else {
        s.clear();
    }
}

void Configuration::trim(std::string& s) const {
    leftTrim(s);
    rightTrim(s);
}

void Configuration::test() const {
    for (auto [k, v] : config_map_) {
        std::cout << k << ' ' << v << '\n';
    }
}