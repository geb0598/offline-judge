#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include <filesystem>

#include <fcntl.h>

namespace oj {

class FileDescriptor {
public:
    enum class Flag : int {
        IN    = O_RDONLY,
        OUT   = O_WRONLY,
        INOUT = O_RDWR,
        APP   = O_APPEND,
        TRUNC = O_TRUNC 
    };

    ~FileDescriptor();
    FileDescriptor(int fd);
    FileDescriptor(const std::filesystem::path& file, Flag flag);
    FileDescriptor(const FileDescriptor& other) = delete;
    FileDescriptor(FileDescriptor&& other) noexcept;

    FileDescriptor& operator=(const FileDescriptor& other) = delete;
    FileDescriptor& operator=(FileDescriptor&& other) noexcept;

    void Open(const std::filesystem::path& file, Flag flag);
    void Close();
    void Redirect(const FileDescriptor& other);

    void Read(std::ostream& out);
    void Write(std::istream& in);

    int  fd() const;
    Flag flag() const;
    bool is_opened() const;
    bool is_readable() const;
    bool is_writable() const;
    bool is_set(Flag flag) const;

private:
    int  fd_;
    bool is_owner_;
};

inline FileDescriptor::Flag operator|(FileDescriptor::Flag lhs, FileDescriptor::Flag rhs) {
    return static_cast<FileDescriptor::Flag>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

inline FileDescriptor::Flag& operator|=(FileDescriptor::Flag& lhs, FileDescriptor::Flag rhs) {
    return lhs = lhs | rhs;
}

inline FileDescriptor::Flag operator&(FileDescriptor::Flag lhs, FileDescriptor::Flag rhs) {
    return static_cast<FileDescriptor::Flag>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

inline FileDescriptor::Flag& operator&=(FileDescriptor::Flag& lhs, FileDescriptor::Flag rhs) {
    return lhs = lhs & rhs;
}

}

#endif