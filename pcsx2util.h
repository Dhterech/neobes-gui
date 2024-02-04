#ifndef BES_PCSX2UTIL_H
#define BES_PCSX2UTIL_H

#include "neosound.h"
#include "suggest.h"
#include "stageinfo.h"

#define HD_SIGNATURE 0x53434549

uint32_t findhdbase(uint32_t resource_list_base);
uint32_t findbdbase(uint32_t resource_list_base);
int getnumhd(uint32_t hdbase);

void pcsx2DwnlRecord(uint32_t record_addr, e_suggestrecord_t &editor_record);
void pcsx2ParseComRecords();

void pcsx2GetComBuffers();
void pcxs2GetModelist(uint32_t stagemode_start, int count);

void pcsx2GetSoundboards(uint32_t hdlistbase, uint32_t bdlistbase, uint32_t count);
void pcsx2GetKeytables(uint32_t keylistbase, int count, int sbbase);

bool pcsx2upload();
bool olmupload(QString filename);

void getProjectRecordAddresses();

//char pcsx2GetOLM(int size);

#endif // BES_PCSX2UTIL_H
