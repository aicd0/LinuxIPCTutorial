#pragma once

#include <sys/ipc.h>

namespace ipc_demo {
    key_t generate_key(const char *file_path);
}
