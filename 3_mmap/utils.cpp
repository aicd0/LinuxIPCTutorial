#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"

int MMapManager::reserve(off_t size) {
    // Open mmap file and get file size.
    int fd = open(m_file.data(), O_RDWR | O_CREAT, 0666);
    if (fd < 0) return -1;
    
    int ret = [&] {
        static struct stat stat_buf;
        if (fstat(fd, &stat_buf) < 0) return -1;
        const off_t &capacity = stat_buf.st_size;

        // Check if resize is necessary.
        if (capacity >= size && m_size >= size && m_data != nullptr) return 0;
        
        // Create mmap with new size.
        off_t new_capacity = capacity;
        if (capacity < size) new_capacity = std::max(size, capacity * 2);
        void *new_mmap = mmap(nullptr, new_capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (new_mmap == MAP_FAILED) return -1;

        // Expand file size if necessary.
        if (capacity < new_capacity) {
            lseek(fd, new_capacity - 1, SEEK_SET);
            write(fd, "", 1);
        }

        // Copy old data if there is.
        if (m_data) {
            memcpy(new_mmap, m_data, m_size);
        }

        close_mmap();
        m_data = new_mmap;
        m_size = new_capacity;
        return 0;
    }();

    close(fd);
    return ret;
}

void MMapManager::close_mmap() {
    if (m_data) {
        munmap(m_data, m_size);
        m_data = nullptr;
        m_size = 0;
    }
}
