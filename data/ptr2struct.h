#ifndef BES_SUGGEST_H
#define BES_SUGGEST_H

#include "types.h"

enum class PLAYER_CODE : uint32_t {
    PCODE_NONE = 1,
    PCODE_TEACHER = 2,
    PCODE_PARA = 4,
    PCODE_BOXY = 8,
    PCODE_MOVE = 16,
    PCODE_ALL = 31
};

inline bool has_player(PLAYER_CODE mask, PLAYER_CODE flag) {
    return (static_cast<uint32_t>(mask) & static_cast<uint32_t>(flag)) != 0;
}

enum class TAPSCODE_ENUM : uint32_t {
    TAPSCODE_NORMAL,
    TAPSCODE_QUESTION,
    TAPSCODE_ANSWER_F,
    TAPSCODE_ANSWER,
    TAPSCODE_MAX
};

struct soundentry_t {                        // Original Struct Name: TAPCT
    int32_t relativetime;                    // frame
    int32_t animationid;                     // actor
    int16_t soundid;                         // sound
    uint16_t reserved;                       // (padding)
};

struct suggestbutton_t {                     // Original Struct Name: TAPDAT
    int32_t timestamp;                       // time
    int16_t buttonid;                        // KeyIndex
    uint16_t reserved;                       // (padding)
    soundentry_t sounds[4];                  // tapct
};

template<size_t subcount>
struct suggestline_base {                    // Original Struct Name: TAPSET
    PLAYER_CODE owner;                       // player_code
    int32_t buttoncount;                     // tapdat_size
    uint32_t ptr_buttons;                    // tapdat_pp

    int32_t oopscount;                       // tapdatNG_size
    uint32_t ptr_oops;                       // tapdatNG_pp

    int32_t timestamp_start;                 // taptimeStart
    int32_t timestamp_end;                   // taptimeEnd

    int32_t coolmodethreshold;               // coolup

    uint32_t chan[2];                        // chan[2]
    uint32_t ptr_subtitles[subcount];        // tapsubt[2 NTSC / 5 PAL]

    TAPSCODE_ENUM type;                      // tapscode
};

using suggestline_t = suggestline_base<2>;
using suggestline_t_pal = suggestline_base<5>;

struct e_suggestline_t {
    PLAYER_CODE owner;
    std::vector<suggestbutton_t> buttons;

    int32_t oopscount;
    uint32_t ptr_oops;

    int32_t timestamp_start;
    int32_t timestamp_end;

    int32_t coolmodethreshold;

    uint32_t chan[2];
    uint32_t ptr_subtitles[5];

    TAPSCODE_ENUM type;

    bool containssubdot(uint32_t subdot);
};

struct e_suggestvariant_t { // different
    bool islinked = false;
    int linknum;
    std::vector<e_suggestline_t> lines;

    void createButton(uint32_t subdot, PLAYER_CODE owner, int buttonid);
    void deleteButton(uint32_t subdot, PLAYER_CODE owner);
    void createLine(uint32_t subdot_start, uint32_t subdot_end, PLAYER_CODE owner);
    void deleteLine(int subdot, PLAYER_CODE owner);
    int setLink(int linkId);

    bool getLineRefFromSubdot(PLAYER_CODE owner, uint32_t subdot, e_suggestline_t **line);
    bool getButFromSubdot(PLAYER_CODE owner, uint32_t subdot, suggestbutton_t &button);
    bool getButRefFromSubdot(PLAYER_CODE owner, uint32_t subdot, suggestbutton_t **button);
    bool isDotOwned(int dot, PLAYER_CODE owner);
};

struct suggestvariant_t {              // Original Struct Name: TAPSTR
    int32_t numlines;                  // tapset_size
    uint32_t ptr_lines;                // tapset_pp
};

struct e_suggestrecord_t {
    uint32_t type;
    uint32_t address;
    uint32_t soundboardid;
    uint32_t lengthinsubdots;
    e_suggestvariant_t variants[17];
    int32_t vs_data[17];
};

struct suggestrecord_t {              // Original Struct Name: SCRDAT
    int32_t soundboardid;             // sndrec_num
    int32_t lengthinsubdots;          // score_lng
    suggestvariant_t variants[17];    // tapstr
    int32_t vs_data[17];              // drawofs
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
    uint8_t reserved;                // (padding)
    uint32_t count_scenecommands;    // scrrec_num
    uint32_t ptr_scenecommands;      // scrrec_pp
    uint32_t reserved_timer1;        // lineTime
    uint32_t reserved_timer2;        // lineTimeFrame
    uint32_t subtitle_scene_id;      // subtLine
    uint32_t scene_id;               // drawLine
    int32_t scr_chan_auto_size;      // scr_chan_auto_size
    uint32_t scr_chan_auto_pp;       // scr_chan_auto_pp
    int32_t offset_in_ms;            // ofsCdtime
    float bpm;                       // tempo
};

#endif // BES_SUGGEST_H

