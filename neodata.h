#ifndef NEODATA_H
#define NEODATA_H

#include "audio/soundsystem.h"
#include "data/ptr2struct.h"
#include "data/ptr2stages.h"
#include "io/ptr2reader.h"
#include "io/emuinterfacer.h"
#include "types.h"

#include <fstream>

#include <QDebug>

extern std::vector<e_soundboard_t> Soundboards;
extern std::vector<e_suggestrecord_t> Records;
extern std::vector<std::vector<commandbuffer_t>> ModeCommands;
extern std::vector<scenemode_t> Modes;
extern currentstage_t StageInfo;
extern uint32_t ModeSize;
extern int subcount; // remove
extern int OopsSize;
extern int CurrentRegion;
extern int CurrentStage;
extern bool VSMode;
extern bool PALMode;
extern bool SUBMode;
extern QString logHistory;

class neodata {
public:
    void static Log(QString string);
    void static CloseProject();
    void static ImportStageInfo();
    uint32_t static CalcAvailableStorage();

    int static LoadFromBes(QString fileName);
    int static LoadFromEmu();
    int static LoadExtraFromEmu();
    //int static LoadFromOlm(QString fileName);

    int static SaveToBes(QString fileName);
    int static SaveToEmu();

    int static UploadOLMToEmu(QString fileName);
    int static DownloadOLMFromEmu(QString fileName, int size);
    //int static PatchOlm(QString fileName, QString olmFile);
};

#endif // NEODATA_H
