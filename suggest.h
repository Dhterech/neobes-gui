#ifndef BES_SUGGEST_H
#define BES_SUGGEST_H

#include "types.h"

struct soundentry_t {
    int32_t relativetime;
    int16_t animationid;
    uint16_t unk;
    int16_t soundid;
    uint16_t always_zero;
};

struct suggestbutton_t {
    uint32_t timestamp;
    uint32_t buttonid;
    soundentry_t sounds[4];
};

struct suggestline_t {
    uint32_t owner;
    uint32_t buttoncount;
    uint32_t ptr_buttons;

    uint32_t oopscount;
    uint32_t ptr_oops;

    uint32_t timestamp_start;
    uint32_t timestamp_end;

    uint32_t coolmodethreshold;

    uint32_t localisations[4];
    uint32_t vs_count;
};

struct suggestline_t_pal {
    uint32_t owner;
    uint32_t buttoncount;
    uint32_t ptr_buttons;

    uint32_t oopscount;
    uint32_t ptr_oops;

    uint32_t timestamp_start;
    uint32_t timestamp_end;

    uint32_t coolmodethreshold;

    uint32_t localisations[7];
    uint32_t vs_count;
};

struct e_suggestline_t {
    uint32_t owner;
    std::vector<suggestbutton_t> buttons;

    uint32_t oopscount;
    uint32_t ptr_oops;

    uint32_t timestamp_start;
    uint32_t timestamp_end;

    uint32_t coolmodethreshold;

    uint32_t localisations[7];
    uint32_t vs_count;

    bool containssubdot(uint32_t subdot);
};

struct e_suggestvariant_t {
    bool islinked = false;
    int linknum;
    std::vector<e_suggestline_t> lines;

    void createButton(uint32_t subdot, uint32_t owner, int buttonid);
    void deleteButton(uint32_t subdot, uint32_t owner);
    int setLink(int linkId);

    bool getButFromSubdot(uint32_t owner, uint32_t subdot, suggestbutton_t &button);
    bool getButRefFromSubdot(uint32_t owner, uint32_t subdot, suggestbutton_t **button);
    bool isDotOwned(int dot, uint32_t owner);
};

struct suggestvariant_t {
    uint32_t numlines;
    uint32_t ptr_lines;
};

struct e_suggestrecord_t {
    uint32_t type;
    uint32_t address;
    uint32_t soundboardid;
    uint32_t lengthinsubdots;
    e_suggestvariant_t variants[17];
    char vs_data[0x48];
};

struct suggestrecord_t {
    uint32_t soundboardid;
    uint32_t lengthinsubdots;
    suggestvariant_t variants[17];
    char vs_data[0x48];
};

#define SCENECMD_SETCURSOR 0
#define SCENECMD_SETRECORD 1
#define SCENECMD_SETGRADED 2
#define SCENECMD_ACTIVATE 9

struct commandbuffer_t {
    uint16_t cmd_id;
    uint16_t arg1;
    uint32_t arg2;
    uint32_t arg3;
    uint32_t arg4;
};

struct scenemode_t {
    uint8_t music;
    uint8_t left_channel;
    uint8_t right_channel;
    uint8_t always_zero; /* Maybe? */
    uint32_t count_scenecommands;
    uint32_t ptr_scenecommands;
    uint32_t reserved_timer1; /* Used by game engine */
    uint32_t reserved_timer2; /* Used by game engine */
    uint32_t subtitle_scene_id;
    uint32_t scene_id;
    uint32_t unk[2]; /* Used by versus mode */
    uint32_t offset_in_ms;
    float bpm;
};

#endif // BES_SUGGEST_H

