#ifndef PCSX2READER_H
#define PCSX2READER_H

namespace pcsx2reader {
    bool read(uint32_t addr, void *out, uint32_t size);
    bool readExtended(uint32_t addr, void *out, uint32_t size);

    bool write(uint32_t addr, void *out, uint32_t size);
    bool writeExtended(uint32_t addr, void *out, uint32_t size);

    bool IsEmuOpen();
    bool SetupIPC();
}

#endif // PCSX2READER_H
