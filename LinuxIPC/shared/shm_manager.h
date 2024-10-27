#pragma once

#include <sys/types.h>

namespace ipc_shared
{
  class SharedMemoryManager {
  private:
    int m_shmid = 0;
    void *m_data = nullptr;
    size_t m_size = 0;
    int m_holder = false;

  public:
    SharedMemoryManager(key_t key, size_t size);
    ~SharedMemoryManager();
    SharedMemoryManager(const SharedMemoryManager&) = delete;

    inline void *data() noexcept {
      return m_data;
    }

    inline size_t size() const noexcept {
      return m_size;
    }

    inline bool holder() const noexcept {
      return m_holder;
    }

  private:
    void close();
  };
}
