#ifndef BES_SS_H
#define BES_SS_H

#include "types.h"
//#include <Windows.h>
//#include <dsound.h>

//extern LPDIRECTSOUND dsnd;
//extern LPDIRECTSOUNDBUFFER ticksnd;

struct e_sound_t {
    uint32_t bdoffset;
    uint16_t frequency;
    uint16_t volume;
};

struct ptrkey_t {
    uint16_t program;
    uint16_t key;
    uint16_t vol;
};

struct e_soundboard_t {
    std::vector<e_sound_t> sounds;
    std::vector<ptrkey_t> keys;
    std::array<std::array<uint8_t, 128>, 128> prog;
    struct {
        uint32_t len = 0;
        char *bytes = nullptr;
    } bd;
    ~e_soundboard_t();
    void clear();
};

struct sound_t {
    char *sndbuf = NULL;
    //IDirectSoundBuffer *sndbuf = NULL;
    ~sound_t();
    void clear();
    void load(const e_soundboard_t &sb, const e_sound_t &snd);
};

struct soundenv_t {
    uint32_t soundboardid;
    std::vector<sound_t> sounds;
    std::vector<ptrkey_t> keys;
    std::array<std::array<uint8_t, 128>, 128> prog;
    std::array<uint16_t, 128> lastkey;
    void clear();
    void load(const e_soundboard_t &sb);
    void play(int key);
    void stopAll();
};

void loadticker();
void playticker();
void initsound(char *HWND);

#endif // BES_SS_H
