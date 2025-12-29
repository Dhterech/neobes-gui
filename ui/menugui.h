#ifndef MENUGUI_H
#define MENUGUI_H

#include <QMainWindow>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class menugui; }
QT_END_NAMESPACE

class menugui : public QWidget
{
    Q_OBJECT

public:
    explicit menugui(QWidget *parent = nullptr);
    ~menugui();

public: signals:
    // buttons
    void ADownloadEmu();
    void ALoadProject();
    void ASettingsGUI();
    void AAboutGUI();

private:
    Ui::menugui *ui;
};

#endif // MENUGUI_H
