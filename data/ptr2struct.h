#ifndef BES_SUGGEST_H
#define BES_SUGGEST_H

#include "types.h"

struct soundentry_t {                        // Original Struct Name: TAPCT?
    uint32_t relativetime;                   // frame
    uint16_t animationid;                    // actor
    uint16_t unk;                            // (actor)
    uint16_t soundid;                        // sound
    uint16_t always_zero;                    // undefined (?)
};

struct suggestbutton_t {                     // Original Struct Name: TAPDAT
    uint32_t timestamp;                      // time
    uint32_t buttonid;                       // KeyIndex
    soundentry_t sounds[4];                  // tapct
};

struct suggestline_t {                       // Original Struct Name: TAPSET
    uint32_t owner;                          // player_code
    uint32_t buttoncount;                    // tapdat_size
    uint32_t ptr_buttons;                    // tapdat_pp

    uint32_t oopscount;                      // tapdatNG_size
    uint32_t ptr_oops;                       // tapdatNG_pp

    uint32_t timestamp_start;                // taptimeStart
    uint32_t timestamp_end;                  // taptimeEnd

    uint32_t coolmodethreshold;              // coolup

    uint32_t localisations[4];               // chan[2], tapsubt[2]
    uint32_t vs_count;                       // tapscode
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

struct e_suggestvariant_t { // different
    bool islinked = false;
    int linknum;
    std::vector<e_suggestline_t> lines;

    void createButton(uint32_t subdot, uint32_t owner, int buttonid);
    void deleteButton(uint32_t subdot, uint32_t owner);
    void createLine(uint32_t subdot_start, uint32_t subdot_end, int owner);
    void deleteLine(int subdot, int owner);
    int setLink(int linkId);

    bool getLineRefFromSubdot(int owner, uint32_t subdot, e_suggestline_t **line);
    bool getButFromSubdot(uint32_t owner, uint32_t subdot, suggestbutton_t &button);
    bool getButRefFromSubdot(uint32_t owner, uint32_t subdot, suggestbutton_t **button);
    bool isDotOwned(int dot, uint32_t owner);
};

struct suggestvariant_t {              // Original Struct Name: TAPSTR
    uint32_t numlines;                 // tapset_size
    uint32_t ptr_lines;                // tapset_pp
};

struct e_suggestrecord_t {
    uint32_t type;
    uint32_t address;
    uint32_t soundboardid;
    uint32_t lengthinsubdots;
    e_suggestvariant_t variants[17];
    char vs_data[0x48];
};

struct suggestrecord_t {              // Original Struct Name: SCRDAT
    uint32_t soundboardid;            // sndrec_num
    uint32_t lengthinsubdots;         // score_lng
    suggestvariant_t variants[17];    // tapstr
    char vs_data[0x48];               // drawofs
};

#define SCENECMD_SETCURSOR 0
#define SCENECMD_SETRECORD 1
#define SCENECMD_SETGRADED 2
#define SCENECMD_ACTIVATE 9

struct commandbuffer_t {             // Original Struct Name: SCRREC
    uint16_t cmd_id;                 // job
    uint16_t arg1;                   // sub
    uint32_t arg2;                   // jobd1
    uint32_t arg3;                   // jobd2
    uint32_t arg4;                   // data
};

struct scenemode_t {                 // Original Struct Name: SCR_CTRL
    uint8_t music;                   // gtime_type
    uint8_t left_channel;            // cdChan
    uint8_t right_channel;           // cdChan
    uint8_t always_zero;             // undefined (?)
    uint32_t count_scenecommands;    // scrrec_num
    uint32_t ptr_scenecommands;      // scrrec_pp
    uint32_t reserved_timer1;        // lineTime
    uint32_t reserved_timer2;        // lineTimeFrame
    uint32_t subtitle_scene_id;      // subtLine
    uint32_t scene_id;               // drawLine
    uint32_t unk[2];                 // scr_chan_auto_size // scr_chan_auto_pp
    uint32_t offset_in_ms;           // ofsCdtime
    float bpm;                       // tempo
};

#endif // BES_SUGGEST_H

