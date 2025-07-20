#ifndef NEOBES_H
#define NEOBES_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QMimeData>

#include "audio/audioplayer.h"
#include "io/emuinterfacer.h"
#include "io/ptr2reader.h"
#include "config.h"
#include "neodata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class neobes; }
QT_END_NAMESPACE

class neobes : public QMainWindow
{
    Q_OBJECT

public:
    neobes(QWidget *parent = nullptr);
    ~neobes();

private slots:
    void changeMenu();
    void handleDisplayClick();
    void handleButtonKeys(int buttonId);
    void handleWasdKeys(bool lr, int inc);
    void handlePrecKeys(int inc);
    void handleArrowKeys(bool lr, int inc);
    void AButtonCopy();
    void AButtonCut();
    void AButtonDelete();
    void AButtonPaste();

    // Main functions
    void ASaveProject();
    void ALoadProject();

    void AUploadOLM();
    void ADownloadOLM();

    void AUploadEmu();
    void ADownloadEmu();

    void ASelectRegion();
    void AAboutGUI();
    void drawEditorGUI();

    void updateButtonProperties(int row, int column);
    void updateCommandProperties();

    // Other functions
    void ALinkVariant(bool linkAll);
    void ASetSoundboard();
    void APlayVariant(bool ticker);

private:
    Ui::neobes *ui;
    int displaySaveDlg();
    void importStageInfo();
    void updateLog();

    void drawButtonProperties();
    void drawLineProperties();
    void drawInfo();
    void drawCommands();
    void drawMenuGUI();

    void setupCommandCB();

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    /* Editor Keyboard Input */
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // NEOBES_H
