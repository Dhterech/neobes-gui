#include "neodata.h"

std::vector<e_soundboard_t> Soundboards;
std::vector<e_suggestrecord_t> Records;
std::vector<std::vector<commandbuffer_t>> ModeCommands;
std::vector<scenemode_t> Modes;
currentstage_t StageInfo;
FileHistory ProjectInfo;
uint32_t ModeSize;
int OopsSize;
int CurrentRegion;
int CurrentStage;
bool VSMode = false;
bool PALMode = false;
bool SUBMode = false;
QString logHistory;

int subcount; // remove later

// USA PAL JP JP-PROTO
const int GAME_FRAMERATE[4] = {60, 50, 60, 60};
const int RESOURCE_LIST_BASE[4] = {0x1C49480, 0x1C46100, 0x1C59880, 0x1C5B380};
const int CURRENT_STAGE[4] = {0x386930, 0x1C63A74, 0x396DCC, 0x3998dc};

void neodata::Log(QString string) {
    logHistory += string + "\n";
    qDebug() << string;
}

void neodata::CloseProject() {
    Records.clear();
    ModeCommands.clear();
    Modes.clear();
}

uint32_t neodata::CalcAvailableStorage() {
    uint32_t size = sizeof(suggestrecord_t) * Records.size();

    for(e_suggestrecord_t &record : Records) {
        for(e_suggestvariant_t &variant : record.variants) {
            if(variant.islinked) continue;
            int linesize = (PALMode ? sizeof(suggestline_t_pal) : sizeof(suggestline_t));
            size += linesize * variant.lines.size();
            for(e_suggestline_t &line : variant.lines) {
                size += sizeof(suggestbutton_t) * line.buttons.size();
            }
        }
    }

    for(int i = 0; i < ModeSize; i++) size += sizeof(commandbuffer_t) * ModeCommands[i].size();
    return size + OopsSize;
}

#include "config.h"
void neodata::ImportStageInfo() {
    int tmpReg = (CurrentRegion == 2 ? 0 : CurrentRegion); // NTSC-J == NTSC
    if (CurrentRegion == 3) tmpReg = 2;

    PALMode = CurrentRegion == 1;
    StageInfo.name = stages[CurrentStage].name;
    StageInfo.bpm = stages[CurrentStage].bpm;
    StageInfo.stagemodelistbase = stages[CurrentStage].regions[tmpReg].stagemodelistbase;
    StageInfo.keytablebase = stages[CurrentStage].regions[tmpReg].keytablebase;
    StageInfo.buttondatabase = StageInfo.keytablebase + stages[CurrentStage].regions[tmpReg].keytablesize;
    StageInfo.buttondataend = StageInfo.stagemodelistbase - 1;
    // Versus Mode Compatibility
    VSMode = CurrentStage > 9;
    //numowners = (isVSMode) ? 1 : 3;
    OopsSize = (VSMode ? 3 : 2) * 6 * sizeof(suggestbutton_t);
    ModeSize = stages[CurrentStage].modescenes;
}

FileHistory getFileHistory(QString tmpFileName, int stage) {
    FileHistory out;
    QFileInfo fileInfo(tmpFileName);

    out.fileName = fileInfo.absoluteFilePath();
    out.prettyFileName = fileInfo.fileName();
    out.fileDate = fileInfo.lastModified().toString("yyyy-MM-dd HH:mm");
    out.lastUsed = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
    out.stage = stages[stage].name;

    return out;
}

/* Old Data Trickery */

int neodata::FixBesmsProject() {
    ImportStageInfo();

    neodata::Log(" Status: Initiating old BESMS project fix...");

    // Store old scene jobs
    std::vector<std::vector<commandbuffer_t>> OldModeCommands;
    OldModeCommands = ModeCommands;

    // Clear and update with PCSX2 data (with the necessary data we need)
    ModeCommands.clear();
    Modes.clear();
    pcxs2GetModelist(StageInfo.stagemodelistbase, ModeSize);
    pcsx2GetComBuffers();

    for(int scene = 0; scene < 9; scene++) {
        for(int buffer = 0; buffer < ModeCommands[scene].size(); buffer++) {
            std::vector<commandbuffer_t> &sceneJobs = ModeCommands[scene];
            std::vector<commandbuffer_t> &oldSceneJobs = OldModeCommands[scene];

            if(sceneJobs[buffer].cmd_id == SCENECMD_SETRECORD || (sceneJobs[buffer].cmd_id == SCENECMD_ACTIVATE && sceneJobs[buffer].arg1 == 0xE)) {
                // Replace the scene addresses with the new one
                uint32_t oldraddr = oldSceneJobs[buffer].cmd_id == SCENECMD_SETRECORD ? oldSceneJobs[buffer].arg4 : oldSceneJobs[buffer].arg2;
                uint32_t raddr = sceneJobs[buffer].cmd_id == SCENECMD_SETRECORD ? sceneJobs[buffer].arg4 : sceneJobs[buffer].arg2;

                for(e_suggestrecord_t &record : Records) {
                    if(record.address == oldraddr) record.address = raddr;
                }
            } else {
                // Replace the scene job with the original one
                sceneJobs = oldSceneJobs;
            }
        }
    }

    // Get every record and delete duplicate records
    for (int record = 0; record < Records.size(); record++) {
        for (int record2 = record+1; record2 < Records.size(); record2++) {
            if(Records[record2].address == Records[record].address) {
                neodata::Log("[BESMSFix] Deleting a duplicate record: " + QString::number(record2) + " has same address as " + QString::number(record));
                Records.erase(Records.begin() + record2);
            }
        }
    }

    // As the records and scene jobs are correctly fixed
    // It is now able to load the missing records if needed
    if (ModeSize > 8) pcsx2ParseComRecords(10, ModeSize-1);
    return 0;
}

/* Import/Export */

std::fstream rawFile;
uint32_t tmpu32;
int32_t tmpi32;

#define WRITE(x) rawFile.write(reinterpret_cast<char *>(&x), sizeof(x));
void saveScene(std::vector<commandbuffer_t> &buffer) {
    tmpu32 = buffer.size(); WRITE(tmpu32);
    for(commandbuffer_t &cmd : buffer) { WRITE(cmd); }
}

int neodata::SaveToBes(QString fileName) {
    rawFile.open(fileName.toUtf8(), std::ios_base::out | std::ios_base::binary);
    if(!rawFile.is_open()) return errno;//GetLastError();

    WRITE(CurrentStage);
    for(int i = 0; i < 9; i++) saveScene(ModeCommands[i]);

    tmpu32 = Records.size(); WRITE(tmpu32);
    for(e_suggestrecord_t &record : Records) {
        WRITE(record.type);
        WRITE(record.lengthinsubdots);
        WRITE(record.soundboardid);
        for(e_suggestvariant_t &variant : record.variants) {
            tmpi32 = variant.islinked ? variant.linknum : -1; WRITE(tmpi32);
            tmpu32 = variant.lines.size(); WRITE(tmpu32);
            for(e_suggestline_t &line : variant.lines) {
                WRITE(line.type);
                WRITE(line.coolmodethreshold);
                if(SUBMode) {for(int susb = 0; susb < subcount; susb++) {
                        WRITE(line.ptr_subtitles[susb]);
                    }}
                WRITE(line.owner);
                WRITE(line.timestamp_start);
                WRITE(line.timestamp_end);
                WRITE(line.oopscount);
                WRITE(line.ptr_oops);

                tmpu32 = line.buttons.size();
                WRITE(tmpu32);
                for(suggestbutton_t &button : line.buttons) WRITE(button);
            }
        }
        if(VSMode) WRITE(record.vs_data);
    }

    tmpu32 = Soundboards.size(); WRITE(tmpu32);
    for(e_soundboard_t &sb : Soundboards) {
        WRITE(sb.bd.len);
        rawFile.write(reinterpret_cast<char *>(sb.bd.bytes), sb.bd.len);

        tmpu32 = sb.keys.size(); WRITE(tmpu32);
        for(ptrkey_t &key : sb.keys) WRITE(key);
        WRITE(sb.prog);
        tmpu32 = sb.sounds.size(); WRITE(tmpu32);
        for(e_sound_t &sound : sb.sounds) WRITE(sound);
    }

    if(ModeSize > 9) { // NeoBesms Extra Data
        WRITE(ModeSize);
        for(int i = 9; i < ModeSize; i++) saveScene(ModeCommands[i]); // Stage 8 / VS
    }

    ProjectInfo = getFileHistory(fileName, CurrentStage);

    rawFile.close();
    return 0;
}
#undef WRITE

#define READ(x) rawFile.read(reinterpret_cast<char *>(&x), sizeof(x));
void loadSceneCommands() {
    std::vector<commandbuffer_t> buffer;
    READ(tmpu32); buffer.resize(tmpu32);
    for(commandbuffer_t &cmd : buffer) READ(cmd);
    ModeCommands.push_back(buffer);
}

int neodata::LoadFromBes(QString fileName) {
    rawFile.open(fileName.toUtf8(), std::ios_base::in | std::ios_base::binary);
    if(!rawFile.is_open()) return errno;

    ProjectInfo = getFileHistory(fileName, CurrentStage);
    SettingsManager::instance().addFileToHistory(ProjectInfo);

    CloseProject();

    READ(tmpu32); CurrentStage = tmpu32;
    ImportStageInfo();

    for(int i = 0; i < 9; i++) loadSceneCommands();

    READ(tmpu32); Records.resize(tmpu32);
    for(e_suggestrecord_t &record : Records) {
        READ(record.type);
        READ(record.lengthinsubdots);
        READ(record.soundboardid);
        for(e_suggestvariant_t &variant : record.variants) {
            READ(tmpi32); variant.islinked = bool(tmpi32 >= 0);
            variant.linknum = tmpi32;

            READ(tmpu32); variant.lines.resize(tmpu32);
            for(e_suggestline_t &line : variant.lines) {
                READ(line.type);
                READ(line.coolmodethreshold);
                if(SUBMode) {for(int susb = 0; susb < subcount; susb++) {
                        READ(line.ptr_subtitles[susb]);
                    }}
                READ(line.owner);
                READ(line.timestamp_start);
                READ(line.timestamp_end);
                READ(line.oopscount);
                READ(line.ptr_oops);
                READ(tmpu32);
                line.buttons.resize(tmpu32);
                for(suggestbutton_t &button : line.buttons) READ(button);
            }
        }
        if(VSMode) READ(record.vs_data);
    }

    READ(tmpu32); Soundboards.resize(tmpu32);
    for(e_soundboard_t &sb : Soundboards) {
        READ(sb.bd.len);
        sb.bd.bytes = (char *)malloc(sb.bd.len);
        rawFile.read(reinterpret_cast<char*>(sb.bd.bytes), sb.bd.len);

        READ(tmpu32); sb.keys.resize(tmpu32);
        for(int k = 0; k < sb.keys.size(); k++) READ(sb.keys[k]);

        READ(sb.prog);
        READ(tmpu32); sb.sounds.resize(tmpu32);
        for(int k = 0; k < sb.sounds.size(); k++) READ(sb.sounds[k]);
    };

    if(rawFile.peek() != EOF) { // NeoBesms Extra data
        Log("Found extra data on: " + QString::number(rawFile.tellg()));
        READ(tmpu32); ModeSize = tmpu32;
        if(ModeSize > 9) { // Extra scenes
            for(int i = 9; i < ModeSize; i++) loadSceneCommands();
        }
    } else {
        ModeSize = 9;
    }

    rawFile.close();

    getProjectRecordAddresses();

    // Old project needs upgrade, warn user
    if(ModeSize != stages[CurrentStage].modescenes) return 254;

    return 0;
}
#undef READ

int neodata::SaveToEmu() {
    if(!pcsx2reader::IsEmuOpen()) return 1;

    CurrentRegion = pcsx2reader::GetGameRegion();
    if(CurrentRegion == 2 && SettingsManager::instance().bhvDebugJP()) CurrentRegion = 3;
    if(CurrentRegion == -1) return 5;

    uint32_t loadedStage = 0xFFFF;
    pcsx2reader::read(CURRENT_STAGE[CurrentRegion], &loadedStage, 4);
    if(loadedStage != CurrentStage + 1) return 3;

    try {
        // Automatically apply fix for older project files
        if(Modes.size() != stages[CurrentStage].modescenes) {
            neodata::FixBesmsProject();
        }

        ImportStageInfo();

        if(CalcAvailableStorage() > StageInfo.buttondataend - StageInfo.buttondatabase + 1) return 6;

        // Project File doesn't have modes in it. If it's zeroed out, get from the running emulator
        if(Modes.size() == 0) pcxs2GetModelist(StageInfo.stagemodelistbase, ModeSize);

        bool result = pcsx2upload(Records, Modes, ModeCommands, StageInfo, VSMode, PALMode, SUBMode, OopsSize, ModeSize);
        if(result == false) return 4;
    } catch(...) { return 2; }
    return 0;
}

int neodata::LoadFromEmu() {
    if(!pcsx2reader::IsEmuOpen()) return 1;

    CurrentRegion = pcsx2reader::GetGameRegion();
    if(CurrentRegion == -1) return 5;

    ProjectInfo.prettyFileName = "New Project";

    Log(" Status: Reading current stage...");

    // JP Specific fixes
    if(CurrentRegion == 2 && SettingsManager::instance().bhvDebugJP()) CurrentRegion = 3;

    pcsx2reader::read(CURRENT_STAGE[CurrentRegion], &CurrentStage, 1); CurrentStage--;
    if(CurrentStage < 0 || CurrentStage > 18) return 3;
    CloseProject();

    Log(" Status: Getting stage info & sound database...");
    ImportStageInfo();
    uint32_t hdlistbase = findhdbase(RESOURCE_LIST_BASE[CurrentRegion]);
    uint32_t bdlistbase = findbdbase(RESOURCE_LIST_BASE[CurrentRegion]);
    int numhd = getnumhd(hdlistbase);

    Log(" Status: Reading game lines...");
    try {
        pcxs2GetModelist(StageInfo.stagemodelistbase, ModeSize);
        pcsx2GetComBuffers();
        pcsx2ParseComRecords(0, ModeSize);
        pcsx2GetSoundboards(hdlistbase, bdlistbase, numhd);
        pcsx2GetKeytables(StageInfo.keytablebase, numhd, 0);
    } catch(...) { return 2; }
    return 0;
}

int neodata::UploadOLMToEmu(QString fileName) {
    if(!pcsx2reader::IsEmuOpen()) return 1;
    return olmupload(fileName);
}

int neodata::DownloadOLMFromEmu(QString fileName, int size) {
    if(!pcsx2reader::IsEmuOpen()) return 1;
    rawFile.open(fileName.toUtf8(), std::ios_base::out | std::ios_base::binary);
    if(!rawFile.is_open()) return 4;

    char boo[size];
    pcsx2reader::read(OLM_LINK_ADDRESS, boo, size);

    rawFile.write(boo, size);
    rawFile.close();
    return 0;
}
