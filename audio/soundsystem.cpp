#include "audio/soundsystem.h"
#include "lib/audio/adpcm.h"
#include "lib/audio/bdutil.h"

#include <QFile>
#include <QByteArray>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>

#define MINIAUDIO_IMPLEMENTATION
#include "audio/miniaudio.h"

ma_engine audioEngine;
TickerSound tickerSound;
bool isAudioInitialized = false;

void initSound() {
    if (isAudioInitialized) return;

    ma_engine_config config = ma_engine_config_init();

    if (ma_engine_init(&config, &audioEngine) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialize Miniaudio engine.\n");
        return;
    }

    isAudioInitialized = true;
}

void cleanupSound() {
    cleanupTicker();

    if (isAudioInitialized) {
        ma_engine_uninit(&audioEngine);
        isAudioInitialized = false;
    }
}

void playTicker() {
    if (!tickerSound.initialized) return;

    if (ma_sound_is_playing(&tickerSound.sound)) {
        ma_sound_stop(&tickerSound.sound);
        ma_sound_seek_to_pcm_frame(&tickerSound.sound, 0);
    }

    ma_sound_start(&tickerSound.sound);
}

void cleanupTicker() {
    if (tickerSound.initialized) {
        ma_sound_uninit(&tickerSound.sound);
        ma_audio_buffer_uninit(&tickerSound.audioBuffer);
        tickerSound.pcmData.clear();
        tickerSound.initialized = false;
    }
}


void e_soundboard_t::clear() {
    this->sounds.clear();
    this->keys.clear();
    if (this->bd.bytes != nullptr) {
        free(this->bd.bytes);
        this->bd.bytes = nullptr;
    }
    this->bd.len = 0;
}

e_soundboard_t::~e_soundboard_t() {
    this->clear();
}

void sound_t::clear() {
    if (this->initialized) {
        ma_sound_uninit(&this->sound);
        ma_audio_buffer_uninit(&this->audioBuffer);
        this->pcmData.clear();
        this->initialized = false;
    }
}

sound_t::~sound_t() {
    this->clear();
}

void sound_t::load(const e_soundboard_t &sb, const e_sound_t &snd) {
    if (!isAudioInitialized) return;

    char *bd = sb.bd.bytes + snd.bdoffset;
    if (!bd) return;

    decode_hist1 = 0;
    decode_hist2 = 0;

    int bdlen = getbdlen(bd);
    if (bdlen <= 0) return;

    int nsamples = getnsamplesfrombdlen(bdlen);
    if (nsamples <= 0) return;

    this->pcmData.resize(nsamples);

    for(int i = 0; i < nsamples; i += 28) {
        decode_psx((uint8_t*)bd, this->pcmData.data() + i, 1, i, 28);
    }

    ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(
        ma_format_s16,
        1,
        nsamples,
        this->pcmData.data(),
        NULL
    );

    if (ma_audio_buffer_init(&bufferConfig, &this->audioBuffer) != MA_SUCCESS) {
        fprintf(stderr, "Failed to init audio buffer.\n");
        return;
    }

    if (ma_sound_init_from_data_source(&audioEngine, &this->audioBuffer, 0, NULL, &this->sound) != MA_SUCCESS) {
        fprintf(stderr, "Failed to init sound.\n");
        ma_audio_buffer_uninit(&this->audioBuffer);
        return;
    }

    float pitch = (float)snd.frequency / (float)ma_engine_get_sample_rate(&audioEngine);
    ma_sound_set_pitch(&this->sound, pitch);

    this->initialized = true;
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
        this->sounds[i] = std::make_shared<sound_t>();
        this->sounds[i]->load(sb, sb.sounds[i]);
    }

    this->keys = sb.keys;
    this->prog = sb.prog;
}

void soundenv_t::play(int keyid) {
    try {
        if (keyid >= (int)this->keys.size() || keyid < 0) return;

        uint16_t prog = this->keys[keyid].program;
        uint16_t key = this->keys[keyid].key;
        uint16_t lastkeyIdx = this->lastkey[prog];
        uint16_t soundid = this->prog[prog][key];

        if (lastkeyIdx != uint16_t(~0)) {
            uint16_t lastSoundId = this->prog[prog][lastkeyIdx];
            if(lastSoundId < this->sounds.size() && this->sounds[lastSoundId] && this->sounds[lastSoundId]->initialized) {
                ma_sound_stop(&this->sounds[lastSoundId]->sound);
            }
        }

        this->lastkey[prog] = key;

        if(soundid < this->sounds.size() && this->sounds[soundid] && this->sounds[soundid]->initialized) {
            ma_sound_seek_to_pcm_frame(&this->sounds[soundid]->sound, 0);
            ma_sound_start(&this->sounds[soundid]->sound);
        }

    } catch (const std::exception &e) {
        fprintf(stderr, "Exception in soundenv_t::play: %s\n", e.what());
    }
}

void soundenv_t::stopAll() {
    for (auto &soundPtr : this->sounds) {
        if (soundPtr && soundPtr->initialized) {
            ma_sound_stop(&soundPtr->sound);
        }
    }

    for (auto &key : lastkey) {
        key = uint16_t(~0);
    }
}

void loadTicker()
{
    if (!isAudioInitialized) return;

    QFile file(":/res/tick.raw");
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty())
        return;

    int sampleRate = 24000;
    int channels = 1;
    int frameCount = data.size() / 2;

    if (frameCount <= 0) return;

    tickerSound.pcmData.resize(frameCount);
    memcpy(tickerSound.pcmData.data(), data.constData(), data.size());

    ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(
        ma_format_s16,
        channels,
        frameCount,
        tickerSound.pcmData.data(),
                                                                      NULL
    );

    if (ma_audio_buffer_init(&bufferConfig, &tickerSound.audioBuffer) != MA_SUCCESS) {
        return;
    }

    if (ma_sound_init_from_data_source(&audioEngine, &tickerSound.audioBuffer, 0, NULL, &tickerSound.sound) != MA_SUCCESS) {
        ma_audio_buffer_uninit(&tickerSound.audioBuffer);
        return;
    }

    float pitch = (float)sampleRate / (float)ma_engine_get_sample_rate(&audioEngine);
    ma_sound_set_pitch(&tickerSound.sound, pitch);

    tickerSound.initialized = true;
}
