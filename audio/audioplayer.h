#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QVector>
#include <algorithm>

#include "audio/soundmanager.h"
#include "neodata.h"

// AudioPlayer class definition
class AudioPlayer : public QWidget {
    Q_OBJECT

public:
    AudioPlayer(QWidget *parent = nullptr) : QWidget(parent), timer(new QTimer(this)), currentSoundIndex(0) {
        connect(timer, &QTimer::timeout, this, &AudioPlayer::playNextSound);
    }

    void initialize() { initSound(); loadTicker(); }
    void cleanup() { cleanupSound(); cleanupTicker(); }

    void loadSoundDB(int id) {
        if(lastSBLoaded == id || id >= Soundboards.size()) return;
        soundenv.load(id == -1 ? Soundboards[Soundboards.size() - 1] : Soundboards[id]);
        lastSBLoaded = id;
    }

    void playSound(int soundId) {
        soundenv.play(soundId);
    }

    // Function to load the audio variant and start playback
    void playVariant(const e_suggestvariant_t &variant, int length, double bpm, bool ticker) {
        tickerSet = ticker;
        tickerInterval = 60/bpm;
        nextTickerTime = 0;

        tokens.clear();
        double spsd = bpmToSpsd(bpm);
        lineEnd = spsd * length;

        // Prepare tokens from the variant
        for (const e_suggestline_t &line : variant.lines) {
            for (const suggestbutton_t &button : line.buttons) {
                for (const soundentry_t &se : button.sounds) {
                    if (se.soundid == -1) continue;
                    playtoken_t token;
                    token.when = (spsd * double(button.timestamp + line.timestamp_start)) +
                                 (double(se.relativetime) / double(GAME_FRAMERATE[CurrentRegion]));
                    token.soundid = se.soundid;
                    tokens.push_back(token);
                }
            }
        }

        std::sort(tokens.begin(), tokens.end(), tokenSorter);

        currentSoundIndex = 0;
        resetTimer();  // Reset the timer when playback starts
        timer->start(16); // Update every ~16 ms (for ~60 FPS)
    }

protected:
    // Override key press event to stop playback
    void keyPressEvent(QKeyEvent *event) override {
        timer->stop();
        soundenv.stopAll();
        this->close();
    }

private slots:
    // Play the next sound based on the current time and the tokens
    void playNextSound() {
        double rtime = getDeltaTime();  // Get the time since playback started

        // Play ticker
        if (rtime >= nextTickerTime && tickerSet) {
            playTicker();
            nextTickerTime += tickerInterval;
        }

        // Play the next sound if enough time has passed
        while (currentSoundIndex < tokens.size() && rtime >= tokens[currentSoundIndex].when) {
            playSound(tokens[currentSoundIndex].soundid);
            currentSoundIndex++;
        }

        // Stop playback if all sounds are played
        if (rtime >= lineEnd) {
            timer->stop();
            soundenv.stopAll();
        }
    }

private:
    // TODO: Cloned from neodata. Implement configuration
    const int GAME_FRAMERATE[3] = {60, 50, 60};

    // Helper functions
    struct playtoken_t { double when; int soundid; };
    double bpmToSpsd(double bpm) { return (15.0 / bpm) / 24.0; }
    static bool tokenSorter(playtoken_t a, playtoken_t b) { return a.when < b.when; }

    // Other things
    QTimer *timer;
    QVector<playtoken_t> tokens;
    int currentSoundIndex = 0;

    QElapsedTimer playTimer;

    bool tickerSet = false;
    double tickerInterval = 0;
    double nextTickerTime = 0;
    double lineEnd = 0;

    soundenv_t soundenv;
    int lastSBLoaded = -1;

    // Function to get the elapsed time since playback started
    double getDeltaTime() {
        if (!playTimer.isValid()) {
            playTimer.start();
        }
        qint64 nanoseconds = playTimer.nsecsElapsed();
        return static_cast<double>(nanoseconds) / 1e9; // nanosecond -> second
    }

    // Reset the timer at the start of playback
    void resetTimer() {
        playTimer.restart();
    }
};

#endif // AUDIOPLAYER_H
