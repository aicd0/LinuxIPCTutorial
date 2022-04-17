#include <sys/shm.h>
#include <exception>
#include "shm_manager.h"

using namespace std;

namespace ipc_shared
{
  SharedMemoryManager::SharedMemoryManager(key_t key, size_t size) {
    // Create or get shared memory segment.
    m_holder = false;
    m_shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
    
    if (m_shmid == -1) {
      m_shmid = shmget(key, size, 0666);

      if (m_shmid == -1) {
        close();
        throw exception();
      }
    }
    else {
      m_holder = true;
    }

    // Get buffer pointer.
    void *_data = shmat(m_shmid, nullptr, 0);

    if (_data == (void*)-1) {
      close();
      throw exception();
    }
    else {
      m_data = _data;
    }

    m_size = size;
  }

  SharedMemoryManager::~SharedMemoryManager() {
    close();
  }

  void SharedMemoryManager::close() {
    if (m_data) {
      if (shmdt(m_data) == -1) {
        throw exception();
      }
      m_data = nullptr;
    }
    
    if (m_holder) {
      if (shmctl(m_shmid, IPC_RMID, nullptr) == -1) {
        throw exception();
      }
      m_holder = false;
    }
  }
}
