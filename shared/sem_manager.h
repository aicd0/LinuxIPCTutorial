#pragma once

#include <sys/types.h>
#include <semaphore.h>
#include "shm_manager.h"

namespace ipc_shared
{
  class SemaphoreManager {
  private:
    SharedMemoryManager m_shm;

  public:
    SemaphoreManager(key_t key, unsigned int value);
    ~SemaphoreManager();
    SemaphoreManager(const SemaphoreManager&) = delete;

    inline int wait() {
      return sem_wait(data());
    }

    inline int post() {
      return sem_post(data());
    }

  private:
    inline sem_t* data() { return reinterpret_cast<sem_t*>(m_shm.data()); }
  };
}
