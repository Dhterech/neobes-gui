#ifndef EMUINTERFACER_H
#define EMUINTERFACER_H

#include "lib/pine/src/pine.h"
#include "types.h"
#include <cstdint>
#include <algorithm>  // For std::min
#include <cstring>    // For memcpy

namespace pcsx2reader {
    bool read(uint32_t addr, void *out, uint32_t size);
    bool write(uint32_t addr, void *out, uint32_t size);

    bool IsEmuOpen();
    void SetupIPC();
}

#endif // EMUINTERFACER_H
