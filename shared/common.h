#pragma once

#include <sys/ipc.h>

namespace ipc_shared {
  key_t generate_key(const char *file_path);
}
