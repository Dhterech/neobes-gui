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
    void handleEditorReady(const QString &title);
    void enableDestructiveActions();
    void disableDestructiveActions();

private slots:
    void toggleView();

    void ASettingsGUI();
    void AAboutGUI();

private:
    void createActions();
    void createMenus();
    void setDestructive(bool destructive);

    QStackedWidget *stackedWidget;
    menugui *menuWidget;
    editorgui *editorWidget;

    // Actions
    QAction *actionLoad, *actionSave, *actionExit;
    QAction *actionUploadOLM, *actionDownloadOLM;
    QAction *actionUploadEmu, *actionDownloadEmu;
    QAction *actionLink, *actionLinkAll;
    QAction *actionSetRecSB, *actionPlayRecord, *actionPlayRecordTicker;
    QAction *actionChangeMenu, *actionSettings, *actionAbout;
};

#endif // GUIMANAGER_H
