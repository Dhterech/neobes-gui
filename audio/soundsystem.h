#ifndef BES_SS_H
#define BES_SS_H

#include <vector>
#include <array>
#include <cstdint>
#include <memory>

#include "audio/miniaudio.h"

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

    e_soundboard_t() = default;
    ~e_soundboard_t();
    void clear();

    e_soundboard_t(const e_soundboard_t&) = delete;
    e_soundboard_t& operator=(const e_soundboard_t&) = delete;

    e_soundboard_t(e_soundboard_t&& other) noexcept {
        sounds = std::move(other.sounds);
        keys = std::move(other.keys);
        prog = other.prog;
        bd = other.bd;
        other.bd.bytes = nullptr;
        other.bd.len = 0;
    }

    e_soundboard_t& operator=(e_soundboard_t&& other) noexcept {
        if (this != &other) {
            clear();
            sounds = std::move(other.sounds);
            keys = std::move(other.keys);
            prog = other.prog;
            bd = other.bd;
            other.bd.bytes = nullptr;
            other.bd.len = 0;
        }
        return *this;
    }
};

struct sound_t {
    ma_audio_buffer audioBuffer;
    ma_sound sound;
    std::vector<int16_t> pcmData;
    bool initialized = false;

    ~sound_t();
    void clear();
    void load(const e_soundboard_t &sb, const e_sound_t &snd);
};

struct soundenv_t {
    uint32_t soundboardid;
    std::vector<std::shared_ptr<sound_t>> sounds;
    std::vector<ptrkey_t> keys;
    std::array<std::array<uint8_t, 128>, 128> prog;
    std::array<uint16_t, 128> lastkey;

    void clear();
    void load(const e_soundboard_t &sb);
    void play(int key);
    void stopAll();
};

struct TickerSound {
    ma_audio_buffer audioBuffer;
    ma_sound sound;
    std::vector<int16_t> pcmData;
    bool initialized = false;
};

extern TickerSound tickerSound;
extern ma_engine audioEngine;

void loadTicker();
void playTicker();
void cleanupTicker();

void initSound();
void cleanupSound();

#endif // BES_SS_H
