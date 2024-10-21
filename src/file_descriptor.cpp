#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "file_descriptor.h"

namespace oj {

FileDescriptor::~FileDescriptor() {
    Close();
}

FileDescriptor::FileDescriptor(int fd) : fd_(fd), is_owner_(false) {}

FileDescriptor::FileDescriptor(const std::filesystem::path& file, Flag flag) : fd_(-1), is_owner_(true) {
    Open(file, flag);
}

FileDescriptor::FileDescriptor(FileDescriptor&& other) noexcept 
    : fd_(std::exchange(other.fd_, -1)),
      is_owner_(std::exchange(other.is_owner_, false)) {}

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) noexcept {
    if (this != &other) {
        Close();
        fd_ = std::exchange(other.fd_, -1);
        is_owner_ = std::exchange(other.is_owner_, false);
    }
    return *this;
}

void FileDescriptor::Open(const std::filesystem::path& file, Flag flag) {
    mode_t mode = 0;
    if (!std::filesystem::exists(file)) {
        flag |= static_cast<FileDescriptor::Flag>(O_CREAT);
        mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 
    } 

    Close();
    int fd = open(file.string().c_str(), static_cast<int>(flag), mode);
    if (fd == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::FileDescriptor: Failed to open file " + file.string() + ".");
    }

    fd_ = fd;
}

void FileDescriptor::Close() {
    if (is_owner_ && fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}

void FileDescriptor::Redirect(const FileDescriptor& other) {
    if (dup2(other.fd_, fd_) == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::FileDescriptor: Failed to redirect file descriptor.");
    } 
}

void FileDescriptor::Read(std::ostream& out) {
    if (!is_readable()) {
        throw std::runtime_error("ERROR::FileDescriptor: File is not open for reading.");
    }

    char buf[256];
    ssize_t bytes;
    while ((bytes = read(fd_, buf, sizeof(buf))) > 0) {
        out.write(buf, bytes);
    }

    if (bytes < 0) {
        throw std::system_error(errno, std::generic_category(), "ERROR::FileDescriptor: Failed to read from a file.");
    }
}

void FileDescriptor::Write(std::istream& in) {
    if (!is_writable()) {
        throw std::runtime_error("ERROR::FileDescriptor: File is not open for writing.");
    }

    char buf[256];
    while (in.read(buf, sizeof(buf)) || in.gcount() > 0) {
        ssize_t total_bytes = 0;
        ssize_t bytes_to_write = in.gcount();
        while (total_bytes < bytes_to_write) {
            ssize_t bytes = write(fd_, buf + total_bytes, bytes_to_write - total_bytes);
            if (bytes < 0) {
                throw std::system_error(errno, std::generic_category(), "ERROR::FileDescriptor: Failed to write to a file.");
            }
            total_bytes += bytes;
        }
    }
}

int FileDescriptor::fd() const {
    return fd_;
}

FileDescriptor::Flag FileDescriptor::flag() const {
    int fd_flag = fcntl(fd_, F_GETFL);
    if (fd_flag == -1) {
        throw std::system_error(errno, std::generic_category(), "ERROR::FileDescriptor: Failed to get file status flag.");
    }

    return static_cast<FileDescriptor::Flag>(fd_flag);
}

bool FileDescriptor::is_opened() const {
    return fd_ != -1;
}

bool FileDescriptor::is_readable() const {
    if (!is_opened()) {
        return false;
    }

    return is_set(Flag::IN | Flag::INOUT);
}

bool FileDescriptor::is_writable() const {
    if (!is_opened()) {
        return false;
    }

    return is_set(Flag::OUT | Flag::INOUT);
}

bool FileDescriptor::is_set(Flag flag) const {
    return static_cast<int>(this->flag() & flag) != 0;
}

}