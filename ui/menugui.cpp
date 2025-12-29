#include "menugui.h"
#include "ui_menugui.h"

#include <QPushButton>

menugui::menugui(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::menugui)
{
    ui->setupUi(this);

    // Connect buttons

    connect(ui->loadbesBtn, &QPushButton::clicked, this, &menugui::ALoadProject);
    connect(ui->loademuBtn, &QPushButton::clicked, this, &menugui::ADownloadEmu);
    connect(ui->settingsBtn, &QPushButton::clicked, this, &menugui::ASettingsGUI);
    connect(ui->infobackBtn, &QPushButton::clicked, this, &menugui::AAboutGUI);
}

menugui::~menugui()
{
    delete ui;
}
