#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <QMenu>

#include <ui/editorgui.h>
#include <ui/menugui.h>


class guimanager : public QMainWindow {
    Q_OBJECT
public:
    guimanager(QWidget *parent = nullptr);

public slots:
    void handleEditorReady();
    void handleSetWindowName(const QString &title);
    void setDestructiveActions(const bool &state);

signals:
    void loadFromFile(const QString &filePath);

private slots:
    void toggleView();

    void ASettingsGUI();
    void AAboutGUI();

private:
    void createActions();
    void createMenus();

    QStackedWidget *stackedWidget;
    menugui *menuWidget;
    editorgui *editorWidget;

    // Actions
    QAction *actionLoad, *actionSave, *actionSaveAs, *actionExit;
    QAction *actionUploadOLM, *actionDownloadOLM;
    QAction *actionUploadEmu, *actionDownloadEmu;
    QAction *actionLink, *actionLinkAll;
    QAction *actionSetRecSB, *actionPlayRecord, *actionPlayRecordTicker, *actionPatchOldProj;
    QAction *actionChangeMenu, *actionSettings, *actionAbout;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // GUIMANAGER_H
