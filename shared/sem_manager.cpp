#include <exception>
#include "sem_manager.h"

using namespace std;

namespace ipc_shared {
    SemaphoreManager::SemaphoreManager(key_t key, unsigned int value) :
        m_shm(key, sizeof(sem_t))
    {
        if (m_shm.holder()) {
            if (sem_init(data(), 1, value) == -1) {
                throw exception();
            }
        }
    }

    SemaphoreManager::~SemaphoreManager() {
        if (m_shm.holder()) {
            sem_destroy(data());
        }
    }
}
