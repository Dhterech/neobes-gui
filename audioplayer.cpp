#include "audioplayer.h"
#include "ui_audioplayer.h"

AudioPlayer::AudioPlayer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AudioPlayer)
{
    ui->setupUi(this);
}

AudioPlayer::~AudioPlayer()
{
    delete ui;
}
