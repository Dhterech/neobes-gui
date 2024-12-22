#include "audio/soundsystem.h"
#include "lib/audio/adpcm.h"
#include "lib/audio/bdutil.h"

#include <cstdio>  // For file operations (fopen, fread, fclose)
#include <cstdlib> // For malloc, free
#include <vector>

ALCdevice* device = nullptr;
ALCcontext* context = nullptr;
TickerSound tickerSound;

void initSound() {
    device = alcOpenDevice(nullptr); // Open default device
    if (!device) {
        fprintf(stderr, "Failed to open OpenAL device.\n");
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context) {
        fprintf(stderr, "Failed to create OpenAL context.\n");
        alcCloseDevice(device);
        device = nullptr;
        return;
    }

    if (!alcMakeContextCurrent(context)) {
        fprintf(stderr, "Failed to make OpenAL context current.\n");
        alcDestroyContext(context);
        alcCloseDevice(device);
        context = nullptr;
        device = nullptr;
        return;
    }
}

void cleanupSound() {
    cleanupTicker();

    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr;
    }
    if (device) {
        alcCloseDevice(device);
        device = nullptr;
    }
}

void playTicker() {
    if (tickerSound.source == AL_NONE) return;

    ALint state;
    alGetSourcei(tickerSound.source, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) {
        alSourceStop(tickerSound.source);
    }

    alSourcePlay(tickerSound.source);
}

void cleanupTicker() {
    if (tickerSound.source != AL_NONE) {
        alDeleteSources(1, &tickerSound.source);
        tickerSound.source = AL_NONE;
    }
    if (tickerSound.buffer != AL_NONE) {
        alDeleteBuffers(1, &tickerSound.buffer);
        tickerSound.buffer = AL_NONE;
    }
}

void e_soundboard_t::clear() {
    this->sounds.clear();
    this->keys.clear();
    if (this->bd.bytes != nullptr) {
        free(this->bd.bytes);  // Free memory when clearing
        this->bd.bytes = nullptr;
    }
    this->bd.len = 0;
}

e_soundboard_t::~e_soundboard_t() {
    this->clear();
}

void sound_t::clear() {
    if (this->source != AL_NONE) {
        alDeleteSources(1, &this->source);
        this->source = AL_NONE;
    }
    if (this->buffer != AL_NONE) {
        alDeleteBuffers(1, &this->buffer);
        this->buffer = AL_NONE;
    }
}

sound_t::~sound_t() {
    this->clear();
}

void sound_t::load(const e_soundboard_t &sb, const e_sound_t &snd) {
    char *bd = sb.bd.bytes + snd.bdoffset;
    decode_hist1 = 0;
    decode_hist2 = 0;

    int bdlen = getbdlen(bd);
    int nsamples = getnsamplesfrombdlen(bdlen);
    int rawbuflen = nsamples * sizeof(int16_t);
    std::vector<int16_t> rawbuf(nsamples);

    for(int i = 0; i < nsamples; i += 28) {
        decode_psx((uint8_t*)bd, rawbuf.data() + i, 1, i, 28);
    }

    alGenBuffers(1, &this->buffer);
    alBufferData(this->buffer, AL_FORMAT_MONO16, rawbuf.data(), rawbuflen, snd.frequency);

    alGenSources(1, &this->source);
    alSourcei(this->source, AL_BUFFER, this->buffer);
}

void soundenv_t::clear() {
    this->sounds.clear();
    this->keys.clear();
    for(uint16_t &key : lastkey) {
        key = uint16_t(~0);
    }
}

void soundenv_t::load(const e_soundboard_t &sb) {
    this->clear();
    int nsounds = sb.sounds.size();
    this->sounds.resize(nsounds);

    for (int i = 0; i < nsounds; i++) {
        this->sounds[i].load(sb, sb.sounds[i]);
    }

    this->keys = sb.keys;
    this->prog = sb.prog;
}

void soundenv_t::play(int keyid) {
    try {
        if (keyid >= this->keys.size() || keyid < 0) return;
        uint16_t prog = this->keys[keyid].program;
        uint16_t key = this->keys[keyid].key;
        uint16_t lastkey = this->lastkey[prog];
        uint16_t soundid = this->prog[prog][key];

        if (lastkey != uint16_t(~0)) {
            alSourceStop(this->sounds[this->prog[prog][lastkey]].source);
        }
        this->lastkey[prog] = key;

        alSourcePlay(this->sounds[soundid].source);
    } catch (const std::exception &e) {
        fprintf(stderr, "Exception in soundenv_t::play: %s\n", e.what());
    } catch (...) {
        fprintf(stderr, "Unknown exception in soundenv_t::play.\n");
    }
}

void soundenv_t::stopAll() {
    for (auto &sound : this->sounds) {
        if (sound.source != AL_NONE) {
            alSourceStop(sound.source);
        }
    }

    for (auto &key : lastkey) {
        key = uint16_t(~0);
    }
}

void loadTicker() {
    FILE* file = fopen("tick.raw", "rb");
    if (!file) return;

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::vector<char> data(fileSize);
    fread(data.data(), 1, fileSize, file);
    fclose(file);

    alGenBuffers(1, &tickerSound.buffer);
    alBufferData(tickerSound.buffer, AL_FORMAT_MONO16, data.data(), fileSize, 24000);

    alGenSources(1, &tickerSound.source);
    alSourcei(tickerSound.source, AL_BUFFER, tickerSound.buffer);
}
