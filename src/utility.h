#pragma once


#ifndef NDEBUG

#include <iostream>

#define DEBUG_MSG(msg) do { \
    std::cerr << "[DEBUG MESSAGE]: " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl; \
} while (0)

#else

#define DEBUG_MSG(msg)

#endif

#include <filesystem>

void replaceExtension(const std::string& directory, const std::string& source_extension, const std::string& target_extension) {
    if (!std::filesystem::is_directory(directory)) {
        throw std::runtime_error(directory + " is not a directory");
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (!std::filesystem::is_regular_file(entry)) {
            continue;
        }

        auto source_path = entry.path();
        if (source_path.extension() == source_extension) {
            auto target_path = entry.path();
            target_path.replace_extension(target_extension);
            std::filesystem::rename(source_path, target_path);
            DEBUG_MSG("Replace " + source_path.string() + " to " + target_path.string());
        }
    }
}