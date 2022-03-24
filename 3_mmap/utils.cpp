#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"

bool MMapManager::reserve(off_t size) {
    // Open mmap file and get file size.
    int fd = open(m_file.data(), O_RDWR | O_CREAT, 0666);
    if (fd == -1) return false;
    static struct stat stat_buf;
    fstat(fd, &stat_buf);
    const off_t &capacity = stat_buf.st_size;

    // Check if resize is necessary.
    if (capacity >= size && m_size >= size && m_data != nullptr) return true;
    
    // Create mmap with new size.
    off_t new_capacity = capacity;
    if (capacity < size) new_capacity = std::max(size, capacity * 2);
    void *new_mmap = mmap(nullptr, new_capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (new_mmap == MAP_FAILED) return false;

    // Expand file size if necessary.
    if (capacity < new_capacity) {
        lseek(fd, new_capacity - 1, SEEK_SET);
        write(fd, "", 1);
    }
    close(fd);

    // Copy old data if there is.
    if (m_data) {
        memcpy(new_mmap, m_data, m_size);
    }

    close_mmap();
    m_data = new_mmap;
    m_size = new_capacity;
    return true;
}

void MMapManager::close_mmap() {
    if (m_data) {
        munmap(m_data, m_size);
        m_data = nullptr;
        m_size = 0;
    }
}
