#ifndef EMUINTERFACER_H
#define EMUINTERFACER_H

#include "types.h"
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <QDebug>

namespace pcsx2reader {
    bool read(uint32_t addr, void *out, uint32_t size);
    bool write(uint32_t addr, void *out, uint32_t size);

    bool IsEmuOpen();
    void SetupIPC();
    int GetGameRegion();
}

#endif // EMUINTERFACER_H
