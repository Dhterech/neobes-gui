#include "data/ptr2struct.h"
#include "data/stageinfo.h"
#include "emuinterfacer.h"
//#include "neodata.h"
#include <fstream>
#include <memory.h>

#define UPLOAD_NOMOVE(addr, data) UPLOAD_SIZE(addr, data, sizeof(data))
#define UPLOAD(addr, data) UPLOAD_NOMOVE(addr,data); addr += sizeof(data);

#define UPLOAD_SIZE(y, x, s) pcsx2reader::write(y, &(x), s)
#define DOWNLOAD_SIZE(y, x, s) pcsx2reader::read(y, &(x), s)

uint32_t getLineOops(PLAYER_CODE owner, uint32_t oopsposition, bool VSMode) {
    if(VSMode) {
        switch(owner) {
        case PLAYER_CODE::PCODE_BOXY:
            return oopsposition;
        case PLAYER_CODE::PCODE_TEACHER:
            return oopsposition + (sizeof(suggestbutton_t) * 6);
        case PLAYER_CODE::PCODE_PARA:
            return oopsposition + (sizeof(suggestbutton_t) * 12);
        default: // The rest doesn't have oops
            return 0;
        }
    } else {
        switch(owner) {
        case PLAYER_CODE::PCODE_TEACHER:
            return oopsposition;
        case PLAYER_CODE::PCODE_PARA:
            return oopsposition + (sizeof(suggestbutton_t) * 6);
        default: // The rest doesn't have oops
            return 0;
        }
    }
    return 0;
}

void updateRecordScCommands(int type, int endtype, uint32_t oldPointer, uint32_t newPointer, std::vector<std::vector<commandbuffer_t>> &ModeCommands) {
    for(int i = type; i <= endtype; i++) {
        for(commandbuffer_t &buffer : ModeCommands[i]) {
            if(buffer.cmd_id != SCENECMD_SETRECORD && buffer.cmd_id != SCENECMD_ACTIVATE) continue;
            if(buffer.cmd_id == SCENECMD_ACTIVATE && buffer.arg1 != 0xE) continue;

            if(buffer.arg2 == oldPointer) buffer.arg2 = newPointer;
            if(buffer.arg4 == oldPointer) buffer.arg4 = newPointer;
        }
    }
}

template<size_t N>
void uploadEditorLine(uint32_t &linebase, uint32_t &buttonbase, uint32_t &oopsbase, bool VSMode, bool SUBMode, e_suggestline_t& line) {
    suggestline_base<N> ps2line;

    ps2line.buttoncount = line.buttons.size();
    ps2line.ptr_buttons = buttonbase;
    for(int b = 0; b < line.buttons.size(); b++) { UPLOAD(buttonbase, line.buttons[b]); }

    ps2line.type = line.type;
    ps2line.coolmodethreshold = line.coolmodethreshold;

    ps2line.chan[0] = line.chan[0];
    ps2line.chan[1] = line.chan[1];

    for(int s = 0; s < N; s++) {
        ps2line.ptr_subtitles[s] = (SUBMode) ? line.ptr_subtitles[s] : uint32_t(~0);
    }

    ps2line.owner = line.owner;
    ps2line.timestamp_start = line.timestamp_start;
    ps2line.timestamp_end = line.timestamp_end;
    ps2line.ptr_oops = getLineOops(line.owner, oopsbase, VSMode);
    ps2line.oopscount = 6;

    UPLOAD(linebase, ps2line);
}

// FIXME: When getting from neodata.h directly, anything after the change breaks.
bool pcsx2upload(std::vector<e_suggestrecord_t> Records, std::vector<scenemode_t> Modes, std::vector<std::vector<commandbuffer_t>> ModeCommands, currentstage_t StageInfo, bool VSMode, bool PALMode, bool SUBMode, int OopsSize, uint32_t ModeSize) {
    uint32_t uploadPos = StageInfo.buttondatabase;
    uploadPos += OopsSize; // do not overwrite oops buttons

    for(int i = 0; i < Records.size(); i++) {
        e_suggestrecord_t &record = Records[i];
        suggestrecord_t ps2record;

        if(i > 1 && record.type == 0) continue; // do not upload ptr2besms repeated cool
        for(int k = 0; k < 17; k++) {
            e_suggestvariant_t &variant = record.variants[k];
            suggestvariant_t &ps2variant = ps2record.variants[k];
            if(variant.islinked) continue;

            int nbuttons = 0;
            for(e_suggestline_t &line : variant.lines) nbuttons += line.buttons.size();
            uint32_t buttonbase = uploadPos;
            uint32_t linebase = buttonbase + (nbuttons * sizeof(suggestbutton_t));

            ps2variant.numlines = variant.lines.size();
            ps2variant.ptr_lines = linebase;

            for(e_suggestline_t &line : variant.lines) {
                if (!PALMode)
                    uploadEditorLine<2>(linebase, buttonbase, StageInfo.buttondatabase, VSMode, SUBMode, line);
                else
                    uploadEditorLine<5>(linebase, buttonbase, StageInfo.buttondatabase, VSMode, SUBMode, line);
            }

            uploadPos = linebase; /* linebase is actually after all lines */
        }

        for(int v = 0; v < 17; v++) {
            e_suggestvariant_t &variant = record.variants[v];
            suggestvariant_t &ps2variant = ps2record.variants[v];
            if(!variant.islinked) continue;

            suggestvariant_t &linkedto = ps2record.variants[variant.linknum];
            ps2variant.numlines = linkedto.numlines;
            ps2variant.ptr_lines = linkedto.ptr_lines;
        }

        ps2record.lengthinsubdots = record.lengthinsubdots;
        ps2record.soundboardid = record.soundboardid;
        memcpy(ps2record.vs_data, record.vs_data, sizeof(ps2record.vs_data));

        updateRecordScCommands(0, ModeSize - 1, record.address, uploadPos, ModeCommands);

        UPLOAD(uploadPos, ps2record);
    }
 
    for(int i = 0; i < ModeSize; i++) {
        Modes[i].ptr_scenecommands = uploadPos;
        Modes[i].count_scenecommands = ModeCommands[i].size();
        for(int k = 0; k < ModeCommands[i].size(); k++) {UPLOAD(uploadPos, ModeCommands[i][k]);}
        UPLOAD_NOMOVE(StageInfo.stagemodelistbase + (i * sizeof(scenemode_t)), Modes[i]);
    }
    
    if (uploadPos > StageInfo.stagemodelistbase)
    {
        return false; // Upload Error: Too much data, corrupted stage info
    }

    return true;
}

#undef UPLOAD_SIZE
#undef DOWNLOAD_SIZE

bool olmupload(QString filename) {
    std::ifstream file(filename.toUtf8(), std::ios::binary);
    if (!file) return false;

    file.seekg(0, std::ios::end);
    uint32_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[fileSize];
    if (!file.read(buffer, fileSize)) { delete[] buffer; return false; }
    bool result = pcsx2reader::write(OLM_LINK_ADDRESS, buffer, fileSize);

    delete[] buffer;
    return result;
}
