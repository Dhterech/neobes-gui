#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);

    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &Settings::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(ui->btnClearHistory, &QPushButton::clicked, this, &Settings::onClearHistory);

    connect(&SettingsManager::instance(), &SettingsManager::historyChanged, this, &Settings::refreshHistory);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::loadSettings()
{
    auto& settings = SettingsManager::instance();

    // HUD Tab
    ui->chkCenterVisual->setChecked(settings.hudCenterVisual());
    ui->chkFirstRecVar->setChecked(settings.hudFirstRecVarOnLoad());
    ui->chkEscapeLines->setChecked(settings.hudEscapeLines());
    ui->spinIconSize->setValue(settings.hudIconSize());

    // Behaviour Tab
    ui->chkKeepSubtitles->setChecked(settings.bhvKeepSubtitles());

    // History Tab
    refreshHistory();
}

void Settings::saveSettings()
{
    auto& settings = SettingsManager::instance();

    // HUD Tab
    settings.setHudCenterVisual(ui->chkCenterVisual->isChecked());
    settings.setHudFirstRecVarOnLoad(ui->chkFirstRecVar->isChecked());
    settings.setHudEscapeLines(ui->chkEscapeLines->isChecked());
    settings.setHudIconSize(ui->spinIconSize->value());

    // Behaviour Tab
    settings.setBhvKeepSubtitles(ui->chkKeepSubtitles->isChecked());

    settings.save();
    this->accept();
}

void Settings::refreshHistory()
{
    ui->listHistory->clear();
    QList<FileHistory> history = SettingsManager::instance().getHistory();

    for (const auto& item : history) {
        QString info = QString("%1\nStage: %2 | Date: %3")
                           .arg(item.fileName)
                           .arg(item.stage)
                           .arg(item.fileDate);
        ui->listHistory->addItem(info);
    }
}

void Settings::onClearHistory()
{
    SettingsManager::instance().clearHistory();
}
