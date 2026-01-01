#ifndef PTR2READER_H
#define PTR2READER_H

#include "data/ptr2struct.h"
#include "data/stageinfo.h"

#define HD_SIGNATURE 0x53434549

uint32_t findhdbase(uint32_t resource_list_base);
uint32_t findbdbase(uint32_t resource_list_base);
int getnumhd(uint32_t hdbase);

void pcsx2DwnlRecord(uint32_t record_addr, e_suggestrecord_t &editor_record);
void pcsx2ParseComRecords(int scene_start, int scene_end);

void pcsx2GetComBuffers();
void pcxs2GetModelist(uint32_t stagemode_start, int count);

void pcsx2GetSoundboards(uint32_t hdlistbase, uint32_t bdlistbase, uint32_t count);
void pcsx2GetKeytables(uint32_t keylistbase, int count, int sbbase);


template<size_t N> void uploadEditorLine(uint32_t linebase, uint32_t buttonbase, const e_suggestline_t& line);
//bool pcsx2upload();
bool pcsx2upload(std::vector<e_suggestrecord_t> Records, std::vector<scenemode_t> Modes, std::vector<std::vector<commandbuffer_t>> ModeCommands, currentstage_t StageInfo, bool VSMode, bool PALMode, bool SUBMode, int OopsSize, uint32_t ModeSize);
bool olmupload(QString filename);

void getProjectRecordAddresses();

//char pcsx2GetOLM(int size);
#endif // PTR2READER_H
