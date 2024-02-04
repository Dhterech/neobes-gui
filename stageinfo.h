#ifndef BES_STAGEINFO_H
#define BES_STAGEINFO_H

#include "types.h"

struct currentstage_t {
    QString name;
    double bpm;
    uint32_t stagemodelistbase;
    uint32_t keytablebase;
    uint32_t buttondatabase;
    uint32_t buttondataend;
};

struct regioninfo_t {
    uint32_t stagemodelistbase;
    uint32_t keytablebase;
    uint32_t keytablesize;
};

struct stageinfo_t {
    const QString name;
    double bpm;
    regioninfo_t regions[3];
};

extern stageinfo_t stages[18];

#endif // BES_STAGEINFO_H
