#include "emuinterfacer.h"
#include "lib/pine/src/pine.h"

namespace pcsx2reader {

PINE::PCSX2 *ipc;

bool IsEmuOpen() {
    if (!ipc) return false;
    try {
        std::unique_ptr<char[]> game_id(ipc->GetGameID());
        return true;
    } catch(PINE::PCSX2::IPCStatus status) {
        qDebug() << "PINE returned " << status;
        return false;
    }
}

void SetupIPC() {
    ipc = new PINE::PCSX2();
}

bool read(uint32_t addr, void *out, uint32_t size) {
    //if (!ipc || !out || size == 0) return false; // Validate input
    if(!ipc) return false;

    uint8_t *outPtr = reinterpret_cast<uint8_t *>(out);
    const uint32_t maxBatchSize = 500; // Max size per batch for IPC

    for (uint32_t i = 0; i < size; i += maxBatchSize) {
        uint32_t batchSize = std::min(maxBatchSize, size - i);
        ipc->InitializeBatch();

        for (uint32_t j = 0; j < batchSize; ++j) {
            ipc->Read<uint8_t, true>(addr + i + j);
        }

        auto batchResult = ipc->FinalizeBatch();
        ipc->SendCommand(batchResult);

        for (uint32_t j = 0; j < batchSize; ++j) {
            uint8_t data = ipc->GetReply<PINE::PCSX2::MsgRead8>(batchResult, j);
            outPtr[i + j] = data;
        }
    }

    return true;
}

bool write(uint32_t addr, void *data, uint32_t size) {
    //if (!ipc || !data || size == 0) return false; // Validate input
    if(!ipc) return false;

    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(data);
    const uint32_t maxBatchSize = 500; // Max size per batch for IPC

    for (uint32_t i = 0; i < size; i += maxBatchSize) {
        uint32_t batchSize = std::min(maxBatchSize, size - i);
        ipc->InitializeBatch();

        for (uint32_t j = 0; j < batchSize; ++j) {
            ipc->Write<uint8_t, true>(addr + i + j, dataPtr[i + j]);
        }

        auto res = ipc->FinalizeBatch();
        ipc->SendCommand(res);
    }

    return true;
}

}
