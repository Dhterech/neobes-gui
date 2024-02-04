#ifndef NEOBES_H
#define NEOBES_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QMimeData>

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

    // Other functions
    void ALinkVariant(bool linkAll);

private:
    Ui::neobes *ui;
    int displaySaveDlg();
    void importStageInfo();
    void updateLog();

    void drawButtonProperties();
    void drawInfo();
    void drawCommands();
    void drawEditorGUI();
    void drawMenuGUI();

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    /* Editor Keyboard Input */
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // NEOBES_H
