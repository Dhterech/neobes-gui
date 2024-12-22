#ifndef BES_CONFIG_H
#define BES_CONFIG_H

#define DEFAULT_REGION 0     // 0 NTSC-U, 1 PAL, 2 NTSC-J
#define DEFAULT_SUBS   false // Keep subtitles (breaks saved projects, dont use)

#include "types.h"
#include <QString>

struct fileHistory {
    QString fileName;
    QString stage;
    QString fileDate;
    QString lastUsed;
};

struct currentConfiguration {
    // Actual config
    int defaultRegion = 0;  // 0 NTSC-U, 1 PAL, 2 NTSC-J
    bool keepSubtitles = false;

    // UI
    bool centerVisual = true; // Centralize interface
    int iconSize = 24;        // GUI Icons size

    // History
    std::vector<fileHistory> recentFiles;
    QString lastOpenBESProjFolder;
};

/*namespace neoconfig {
    currentConfiguration currentConfig;
}*/

#endif // BES_CONFIG_H
