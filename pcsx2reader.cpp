#include "types.h"
#include "pine.h"

namespace pcsx2reader {

PINE::PCSX2 *ipc;

bool IsEmuOpen() {
    try {
        char *ver = ipc->Version<false>();
        return ver[0] != 0x00;
    } catch(...) {
        return false;
    }
}

void SetupIPC() {
    ipc = new PINE::PCSX2();
    return;
}

bool read(uint32_t addr, void *out, uint32_t size) {
    uint8_t *outPtr = reinterpret_cast<uint8_t *>(out);

    ipc->InitializeBatch();
    for (int siz = 0; siz < size; siz++, addr++) {
        ipc->Read<uint8_t, true>(addr);
    }

    auto batchResult = ipc->FinalizeBatch();
    ipc->SendCommand(batchResult);

    for (int siz = 0; siz < size; siz++) {
        uint8_t data = ipc->GetReply<PINE::PCSX2::MsgRead8>(batchResult, siz);
        *outPtr++ = data;
    }

    return true;
}

bool readExtended(uint32_t addr, void *out, uint32_t size) { // uses segments of 64 bits to accomodate larger data
    uint8_t *outPtr = reinterpret_cast<uint8_t *>(out);

    ipc->InitializeBatch();
    uint32_t numIterations = size / sizeof(uint64_t);
    uint32_t remainder = size % sizeof(uint64_t);

    for (int i = 0; i < numIterations; i++, addr += sizeof(uint64_t)) {
        ipc->Read<uint64_t, true>(addr);
    }

    if (remainder > 0) {
        ipc->Read<uint64_t, true>(addr); // Read an additional 64 bits to cover the remainder
    }

    auto batchResult = ipc->FinalizeBatch();
    ipc->SendCommand(batchResult);

    for (int i = 0; i < numIterations; i++) {
        uint64_t data = ipc->GetReply<PINE::PCSX2::MsgRead64>(batchResult, i);
        memcpy(outPtr, &data, sizeof(uint64_t));
        outPtr += sizeof(uint64_t);
    }

    if (remainder > 0) {
        uint64_t data = ipc->GetReply<PINE::PCSX2::MsgRead64>(batchResult, numIterations);
        memcpy(outPtr, &data, remainder);
    }

    return true;
}

bool write(uint32_t addr, void *out, uint32_t size) {
    const uint8_t *dataPtr = reinterpret_cast<const uint8_t *>(out);

    ipc->InitializeBatch();
    for (uint32_t i = 0; i < size; ++i) {
        uint8_t chunk = dataPtr[i];
        ipc->Write<uint8_t, true>(addr + i, chunk);
    }
    auto res = ipc->FinalizeBatch();
    ipc->SendCommand(res);

    return true;
}

#include <cstdint>
#include <algorithm>  // For std::min

bool writeExtended(uint32_t addr, void *out, uint32_t size) { // more edgy but that solves for a while
    const uint8_t *dataPtr = reinterpret_cast<const uint8_t *>(out);

    // Determine the maximum size per IPC message
    const uint32_t maxIpcSize = 500;//24999;//49999;//650000;

    // Calculate the maximum number of bytes per IPC message
    const uint32_t maxBytesPerIpc = maxIpcSize;

    // Iterate over bytes and send them in batches
    for (uint32_t i = 0; i < size; i += maxBytesPerIpc) {
        uint32_t bytesInBatch = std::min(maxBytesPerIpc, size - i);
        ipc->InitializeBatch();

        for (uint32_t j = 0; j < bytesInBatch; ++j) {
            ipc->Write<uint8_t, true>(addr + i + j, dataPtr[i + j]);
        }

        auto res = ipc->FinalizeBatch();
        ipc->SendCommand(res);
    }

    return true;
}

}
