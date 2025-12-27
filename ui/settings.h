#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"

#include <QDialog>
#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QDialog // Changed from QWidget to QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private slots:
    void loadSettings();
    void saveSettings();
    void refreshHistory();
    void onClearHistory();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
