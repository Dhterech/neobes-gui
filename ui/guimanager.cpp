#include "guimanager.h"

#include "about.h"
#include "settings.h"

#include <QMessageBox>
#include <qdir.h>
#include <qmimedata.h>

guimanager::guimanager(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("NeoBES");
    setWindowIcon(QIcon(":/res/neobes.ico"));

    menuWidget = new menugui(this);
    editorWidget = new editorgui(this);

    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(menuWidget);
    stackedWidget->addWidget(editorWidget);
    setCentralWidget(stackedWidget);

    setStatusBar(new QStatusBar(this));
    createActions();
    createMenus();

    // Other Windows Actions
    connect(actionAbout, &QAction::triggered, this, &guimanager::AAboutGUI);
    connect(actionSettings, &QAction::triggered, this, &guimanager::ASettingsGUI);
    connect(actionChangeMenu, &QAction::triggered, this, &guimanager::toggleView);

    // Menu Signals
    connect(menuWidget, &menugui::loadFileFromRecents, this, &guimanager::loadFromFile);

    // Menu Actions
    connect(menuWidget, &menugui::AAboutGUI, this, &guimanager::AAboutGUI);
    connect(menuWidget, &menugui::ASettingsGUI, this, &guimanager::ASettingsGUI);
    connect(menuWidget, &menugui::ALoadProject, editorWidget, &editorgui::ALoadProject);
    connect(menuWidget, &menugui::ADownloadEmu, editorWidget, &editorgui::ADownloadEmu);

    // Editor Signals
    connect(editorWidget, &editorgui::editorReady, this, &guimanager::handleEditorReady);
    connect(editorWidget, &editorgui::setWindowName, this, &guimanager::handleSetWindowName);
    connect(editorWidget, &editorgui::setDestructive, this, &guimanager::setDestructiveActions);
    connect(this, &guimanager::loadFromFile, editorWidget, &editorgui::loadProject);

    // Editor Oriented Actions
    connect(actionLoad, &QAction::triggered, editorWidget, &editorgui::ALoadProject);
    connect(actionSave, &QAction::triggered, editorWidget, &editorgui::ASaveProject);
    connect(actionSaveAs, &QAction::triggered, editorWidget, &editorgui::ASaveAsProject);

    connect(actionUploadEmu, &QAction::triggered, editorWidget, &editorgui::AUploadEmu);
    connect(actionUploadOLM, &QAction::triggered, editorWidget, &editorgui::AUploadOLM);
    connect(actionDownloadEmu, &QAction::triggered, editorWidget, &editorgui::ADownloadEmu);
    connect(actionDownloadOLM, &QAction::triggered, editorWidget, &editorgui::ADownloadOLM);

    // Editor Oriented Actions (Multiple)
    connect(actionLink, &QAction::triggered, editorWidget, [=, this]() {editorWidget->ALinkVariant(false);});
    connect(actionLinkAll, &QAction::triggered, editorWidget, [=, this]() {editorWidget->ALinkVariant(true);});
    connect(actionPlayRecord, &QAction::triggered, editorWidget, [=, this](){editorWidget->APlayVariant(false);});
    connect(actionPlayRecordTicker, &QAction::triggered, editorWidget, [=, this](){editorWidget->APlayVariant(true);});

    // Disable actions that should only be enabled when project loaded;
    connect(editorWidget, &editorgui::updateMenuHistoryFile, menuWidget, &menugui::setupRecentFiles);
    setDestructiveActions(false);
}

/* MENU BAR */

void guimanager::createActions() {
    // Load
    actionLoad = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "&Load", this);
    actionLoad->setShortcut(QKeySequence("F2"));

    // Save
    actionSave = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "&Save", this);
    actionSave->setShortcut(QKeySequence("Ctrl+S"));

    actionSaveAs = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "&Save As...", this);
    actionSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));

    // Emulator Actions
    actionUploadEmu = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend), "Upload to &PCSX2", this);
    actionUploadEmu->setShortcut(QKeySequence("F5"));

    actionDownloadEmu = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "Download &from PCSX2", this);
    actionDownloadEmu->setShortcut(QKeySequence("F6"));

    // OLM Actions
    actionUploadOLM = new QAction("&Upload OLM", this);
    actionUploadOLM->setShortcut(QKeySequence("F7"));

    actionDownloadOLM = new QAction("&Download OLM", this);
    actionDownloadOLM->setShortcut(QKeySequence("F8"));

    // Linking
    actionLink = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::InsertLink), "&Link", this);
    actionLink->setShortcut(QKeySequence("L"));

    actionLinkAll = new QAction("Link all &variants", this);
    actionLinkAll->setShortcut(QKeySequence("Shift+L"));

    // Audio / Audio Editing
    actionSetRecSB = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeHigh), "&Set Record Soundboard", this);
    actionSetRecSB->setShortcut(QKeySequence("B"));

    actionPlayRecord = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart), "Play Record (No Ticker)", this);
    actionPlayRecord->setShortcut(QKeySequence("Shift+P"));

    actionPlayRecordTicker = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart), "&Play Record", this);
    actionPlayRecordTicker->setShortcut(QKeySequence("P"));

    // System
    actionChangeMenu = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaylistRepeat), "Show Menu", this);
    actionChangeMenu->setToolTip("Toggles between Menu and Editor interface.");
    actionChangeMenu->setShortcut(QKeySequence("Esc"));

    actionSettings = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties), "Settings", this);

    actionAbout = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "&About NeoBES", this);
    actionAbout->setShortcut(QKeySequence("F1"));

    actionExit = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit), "Exit", this);
}

void guimanager::createMenus() {
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(actionLoad);
    fileMenu->addAction(actionSave);
    fileMenu->addAction(actionSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(actionSettings);
    fileMenu->addAction(actionExit);

    // Project/Transfer Menu
    QMenu *transferMenu = menuBar()->addMenu("&Transfer");
    transferMenu->addAction(actionUploadEmu);
    transferMenu->addAction(actionDownloadEmu);
    transferMenu->addSeparator();
    transferMenu->addAction(actionUploadOLM);
    transferMenu->addAction(actionDownloadOLM);

    // Tools Menu
    QMenu *toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction(actionLink);
    toolsMenu->addAction(actionLinkAll);
    toolsMenu->addSeparator();
    toolsMenu->addAction(actionPlayRecordTicker);
    toolsMenu->addAction(actionPlayRecord);
    toolsMenu->addAction(actionSetRecSB);

    // View Menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(actionChangeMenu);

    // Help Menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(actionAbout);
}

void guimanager::toggleView() {
    int nextIndex = (stackedWidget->currentIndex() == 0) ? 1 : 0;
    stackedWidget->setCurrentIndex(nextIndex);
    actionChangeMenu->setText(nextIndex == 0 ? "Show Editor" : "Show Menu");
}

void guimanager::setDestructiveActions(const bool &destructive) {
    actionSave->setEnabled(destructive);
    actionSaveAs->setEnabled(destructive);
    actionUploadEmu->setEnabled(destructive);
    actionUploadOLM->setEnabled(destructive);
    actionLink->setEnabled(destructive);
    actionLinkAll->setEnabled(destructive);
    actionPlayRecord->setEnabled(destructive);
    actionPlayRecordTicker->setEnabled(destructive);
    actionSetRecSB->setEnabled(destructive);
    actionChangeMenu->setEnabled(destructive);
}

/* Launch Interfaces */

void guimanager::ASettingsGUI() {
    Settings *settings = new Settings();
    settings->exec();
}

void guimanager::AAboutGUI() {
    About *about = new About();
    about->show();
}

/* Other things */

void guimanager::handleEditorReady() {
    stackedWidget->setCurrentIndex(1);
    editorWidget->toggleActive();
}

void guimanager::handleSetWindowName(const QString &title) {
    this->setWindowTitle(title);
}

/* Drag and Drop */

void guimanager::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

void guimanager::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void guimanager::dragLeaveEvent(QDragLeaveEvent *event) {
    event->accept();
}

void guimanager::dropEvent(QDropEvent *event) {
    const QMimeData *mime(event->mimeData());
    QString fileName = QDir::toNativeSeparators(QUrl(mime->urls().at(0).toString()).toLocalFile());
    neodata::Log("LOAD FROM FILE DROP");

    emit loadFromFile(fileName);
}
