#pragma once

#include <string>

class MMapManager {
private:
    std::string m_file;
    void *m_data = nullptr;
    off_t m_size = 0;

public:
    MMapManager(const char *file) :
        m_file(file) {}

    ~MMapManager() {
        close_mmap();
    }

    inline void *data() noexcept {
        return m_data;
    }

    inline off_t size() const noexcept {
        return m_size;
    }

    bool reserve(off_t size);

private:
    void close_mmap();
};
