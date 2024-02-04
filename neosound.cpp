//Sound System
#include "neosound.h"
#include "adpcm.h"
#include "bdutil.h"

//LPDIRECTSOUND dsnd;
//LPDIRECTSOUNDBUFFER ticksnd = NULL;

void initsound(char *hwnd) {
    //DirectSoundCreate(NULL, &dsnd, NULL);
    //dsnd->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
    //dsnd->SetSpeakerConfig(DSSPEAKER_STEREO);
}

void playticker() {
    //if(ticksnd == NULL) return;
    //ticksnd->Stop();
    //ticksnd->SetCurrentPosition(0);
    //ticksnd->Play(0,0,0);
}

void e_soundboard_t::clear() {
    this->sounds.clear();
    if(this->bd.bytes != nullptr) free(this->bd.bytes);
    this->bd.bytes = nullptr;
}

e_soundboard_t::~e_soundboard_t() {
    this->clear();
}

void sound_t::clear() {
    //if(this->sndbuf != NULL) this->sndbuf->Release();
    this->sndbuf = NULL;
}

void sound_t::load(const e_soundboard_t &sb, const e_sound_t &snd) {
    char *bd = sb.bd.bytes + snd.bdoffset;
    decode_hist1 = 0;
    decode_hist2 = 0;

    int bdlen = getbdlen(bd);
    int nsamples = getnsamplesfrombdlen(bdlen);
    int rawbuflen = nsamples * 2;

    int16_t *rawbuf = (int16_t*)malloc(rawbuflen);

    for(int i = 0; i < nsamples; i += 28) {
        decode_psx((uint8_t*)bd, rawbuf + i, 1, i, 28);
    }

    /*WAVEFORMATEX wfx;
    wfx.cbSize = 0;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = snd.frequency;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.wBitsPerSample / 8) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.wFormatTag = WAVE_FORMAT_PCM;

    DSBUFFERDESC desc;
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwBufferBytes = rawbuflen;
    desc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
    desc.dwReserved = 0;
    desc.guid3DAlgorithm = DS3DALG_DEFAULT;
    desc.lpwfxFormat = &wfx;

    IDirectSoundBuffer *sndbuf;

    dsnd->CreateSoundBuffer(&desc, &sndbuf, NULL);

    //TODO: Check Result

    sndbuf->QueryInterface(IID_IDirectSoundBuffer, (void**)(&this->sndbuf));

    LPVOID audbuf1;
    DWORD audbuf1len;
    LPVOID audbuf2;
    DWORD audbuf2len;
    this->sndbuf->Lock(0, rawbuflen, &audbuf1, &audbuf1len, &audbuf2, &audbuf2len, 0);

    memcpy(audbuf1, rawbuf, rawbuflen);
    this->sndbuf->Unlock(audbuf1, audbuf1len, audbuf2, audbuf2len);

    this->sndbuf->SetFrequency(snd.frequency);
    //this->sndbuf->SetVolume(10000);

    sndbuf->Release();*/
    free(rawbuf);
}

sound_t::~sound_t() {
    this->clear();
}

void soundenv_t::clear() {
    this->sounds.clear();
    this->keys.clear();
    for(uint16_t &key : lastkey) {key = ~0;}
}

void soundenv_t::load(const e_soundboard_t &sb) {
    this->clear();
    int nsounds = sb.sounds.size();
    this->sounds.resize(nsounds);

    for(int i = 0; i < nsounds; i++) {this->sounds[i].load(sb, sb.sounds[i]);}

    this->keys = sb.keys;
    this->prog = sb.prog;
}

void soundenv_t::play(int keyid) {
    try{ 
        if(keyid >= this->keys.size() || keyid < 0) return;
        uint16_t prog = this->keys[keyid].program;
        uint16_t key = this->keys[keyid].key;
        uint16_t lastkey = this->lastkey[prog];
        uint16_t soundid = this->prog[prog][key];

        //if(lastkey != uint16_t(~0)) {this->sounds[this->prog[prog][lastkey]].sndbuf->Stop();}
        this->lastkey[prog] = key;

        //this->sounds[soundid].sndbuf->SetCurrentPosition(0);
        //this->sounds[soundid].sndbuf->SetVolume(10000);
        //this->sounds[soundid].sndbuf->Play(0,0,0);
    } catch(...) { return; }
}

void soundenv_t::stopAll() {
    try{
        for(int i = 0; i < this->sounds.size(); i++) {
            uint16_t prog = this->keys[i].program;
            uint16_t lastkey = (this-> lastkey.size() != 0 ? this->lastkey[prog] : 0);
            uint16_t key = this->keys[i].key;
            
            if(prog > this->prog.size()) return;
            //if(lastkey != uint16_t(~0)) {this->sounds[this->prog[prog][lastkey]].sndbuf->Stop();}
            this->lastkey[prog] = key;
        }
    } catch(...) { return; }
}

void loadticker() {
    /*HANDLE tickfile = CreateFileW(
        L"tick.raw",
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    LARGE_INTEGER len;

    if(tickfile == INVALID_HANDLE_VALUE) {ticksnd = NULL; return;}

    GetFileSizeEx(tickfile, &len);

    WAVEFORMATEX wfx;
    wfx.cbSize = 0;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = 24000;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.wBitsPerSample / 8) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.wFormatTag = WAVE_FORMAT_PCM;

    DSBUFFERDESC desc;
    desc.dwBufferBytes = DWORD(len.LowPart);
    desc.guid3DAlgorithm = GUID_NULL;
    desc.dwReserved = 0;
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
    desc.lpwfxFormat = &wfx;

    IDirectSoundBuffer *tmp;
    dsnd->CreateSoundBuffer(&desc, &tmp, NULL);
    tmp->QueryInterface(IID_IDirectSoundBuffer, (void**)&ticksnd);

    LPVOID audbuf1; DWORD audbuf1len;
    LPVOID audbuf2; DWORD audbuf2len;
    ticksnd->Lock(0, len.LowPart, &audbuf1, &audbuf1len, &audbuf2, &audbuf2len, 0);

    DWORD readin;
    ReadFile(tickfile, audbuf1, len.LowPart, &readin, NULL);

    ticksnd->Unlock(audbuf1, audbuf1len, audbuf2, audbuf2len);
    ticksnd->SetFrequency(24000);
    ticksnd->SetVolume(10000);

    tmp->Release();
    return;*/
}
