#ifndef EDITORGUI_H
#define EDITORGUI_H

#include <QMainWindow>
#include <QWidget>

#include "audio/audioplayer.h"
#include "io/emuinterfacer.h"
#include "io/ptr2reader.h"
#include "config.h"
#include "settings.h"
#include "neodata.h"


QT_BEGIN_NAMESPACE
namespace Ui { class editorgui; }
QT_END_NAMESPACE

class editorgui : public QWidget
{
    Q_OBJECT

public:
    explicit editorgui(QWidget *parent = nullptr);
    ~editorgui();
    void toggleActive();

signals:
    void editorReady();
    void setWindowName(const QString &title);
    void setDestructive(const bool &state);
    void setOldPatching(const bool &state);

public slots:
    void loadProject(QString file);
    // Main functions
    int ASaveProject();
    int ASaveAsProject();
    void ALoadProject();

    void AUploadOLM();
    void ADownloadOLM();

    void AUploadEmu();
    void ADownloadEmu();
    // Other functions
    void ALinkVariant(bool linkAll);
    void ASetSoundboard();
    void APlayVariant(bool ticker);
    void APatchOldProj();

private slots:
    // Internal
    void handleDisplayClick();
    void handleButtonKeys(int buttonId);
    void handleWasdKeys(bool lr, int inc);
    void handlePrecKeys(int inc);
    void handleArrowKeys(bool lr, int inc);
    void AButtonCopy();
    void AButtonCut();
    void AButtonDelete();
    void AButtonPaste();
    void ALineCreate();
    void ALineDelete();

    void ACommandCreate();
    void ACommandDelete();

    void drawEditorGUI();

    void updateButtonProperties(int row, int column);
    void updateLineProperties(int row, int column);
    void updateCommandProperties();

private:
    Ui::editorgui *ui;
    int displaySaveDlg();
    void importStageInfo();
    void afterProjLoad();
    void updateLog();

    void drawButtonProperties();
    void drawLineProperties();
    void drawInfo();
    void drawCommands();
    void drawMenuGUI();

    void setupCommandCB();
    void setProjectWindowName();

protected:
    void closeEvent(QCloseEvent *event) override;

    /* Editor Keyboard Input */
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // EDITORGUI_H
