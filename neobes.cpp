#include "neobes.h"
#include "ui_neobes.h"
#include <QFileDialog>
#include <QInputDialog>

#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>

bool isMenu = true;
int owners[3] {0x2, 0x4, 0x8};
int numowners = 3;

int cursorpos = 0;
int cursorowner = 0;
int precpos = 0;

int ownerSetting[3] = {0x2, 0x4, 0x8};

struct ownerEntry {
    QString name;
    int value;
};

ownerEntry ownerNames[] = {
    {"Unknown", 0},
    {"None", 0x1},
    {"Teacher", 0x2},
    {"PaRappa", 0x4},
    {"SFX", 0x8},
    {"Scene", 0x10}
};

QString getOwnerName(int hexValue) {
    for (const auto& entry : ownerNames) {
        if (entry.value == hexValue) {
            return entry.name;
        }
    }
    return "Unknown";
}

QString buttonsUIResNot[] {
    ":/editor/buttons/res/editor/buttons/dot.png",
    ":/editor/buttons/res/editor/buttons/star.png",
    ":/editor/buttons/res/editor/buttons/udot.png",
    ":/editor/buttons/res/editor/buttons/ustar.png",
};

QString buttonsUIRes[] {
    ":/editor/buttons/res/editor/buttons/unknown.png",
    ":/editor/buttons/res/editor/buttons/triangle.png",
    ":/editor/buttons/res/editor/buttons/circle.png",
    ":/editor/buttons/res/editor/buttons/cross.png",
    ":/editor/buttons/res/editor/buttons/square.png",
    ":/editor/buttons/res/editor/buttons/left.png",
    ":/editor/buttons/res/editor/buttons/right.png",
    ":/editor/buttons/res/editor/buttons/shooriken.png",
};

QString logged;
int CurrentRecord;
int CurrentVariant;
int MentionedVariant;
int SelectedLine;
bool hasEdited;

suggestbutton_t copiedButton;

AudioPlayer *audio;

QString accentColor;

neobes::neobes(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::neobes)
{
    ui->setupUi(this);
    pcsx2reader::SetupIPC();
    neodata::Log("NeoBES Launched");

    updateLog();
    drawMenuGUI();
    this->setFocusPolicy(Qt::StrongFocus);

    ui->variantInput->setMaximum(16);

    connect(ui->actionChangeMenu, &QAction::triggered, this, &neobes::changeMenu);
    connect(ui->actionSettings, &QAction::triggered, this, &neobes::ASettingsGUI);

    connect(ui->actionLoad, &QAction::triggered, this, &neobes::ALoadProject);
    connect(ui->actionSave, &QAction::triggered, this, &neobes::ASaveProject);

    connect(ui->actionUploadOLM, &QAction::triggered, this, &neobes::AUploadOLM);
    connect(ui->actionDownloadOLM, &QAction::triggered, this, &neobes::ADownloadOLM);

    connect(ui->actionAbout, &QAction::triggered, this, &neobes::AAboutGUI);

    connect(ui->actionDownloadEmu, &QAction::triggered, this, &neobes::ADownloadEmu);
    connect(ui->actionUploadEmu, &QAction::triggered, this, &neobes::AUploadEmu);

    /* Editing */
    connect(ui->actionLink, &QAction::triggered, this, [=, this]() {neobes::ALinkVariant(false);});
    connect(ui->actionLinkAll, &QAction::triggered, this, [=, this]() {neobes::ALinkVariant(true);});
    connect(ui->actionPlayRecord, &QAction::triggered, this, [=, this](){neobes::APlayVariant(false);});
    connect(ui->actionPlayRecordTicker, &QAction::triggered, this, [=, this](){neobes::APlayVariant(true);});

    connect(ui->actionSetRecSB, &QAction::triggered, this, &neobes::ASetSoundboard);

    connect(ui->recordInput, QOverload<int>::of(&QSpinBox::valueChanged), this, [=, this]() {
        CurrentRecord = ui->recordInput->value();
        neobes::drawEditorGUI();
    });
    connect(ui->variantInput, QOverload<int>::of(&QSpinBox::valueChanged), this, [=, this]() {
        CurrentVariant = ui->variantInput->value();
        neobes::drawEditorGUI();
    });

    connect(ui->butProperty, &QTableWidget::cellChanged, this, &neobes::updateButtonProperties);
    connect(ui->lineOptions, &QTableWidget::cellChanged, this, &neobes::updateLineProperties);
    connect(ui->comSelector, &QComboBox::currentIndexChanged, this, [=, this]() {
        neobes::drawCommands();
    });
    connect(ui->comTable, &QTableWidget::cellChanged, this, &neobes::updateCommandProperties);
    connect(ui->addCom, &QPushButton::clicked, this, &neobes::ACommandCreate);
    connect(ui->remCom, &QPushButton::clicked, this, &neobes::ACommandDelete);

    /* Init audio */
    audio = new AudioPlayer();
    audio->initialize();

    // Setup accent color
    accentColor = QApplication::palette().color(QPalette::Highlight).name();
}

/* Editor Keyboard Input */
void neobes::keyPressEvent(QKeyEvent *event)
{
    if(ui->stackedWidget->currentIndex() == 1) { // Only get keys when in editor
        switch(event->key()) {
        /* Moving cursor */
        case Qt::Key_W: handleWasdKeys(false, -1); break;
        case Qt::Key_S: handleWasdKeys(false, +1); break;
        case Qt::Key_A: handleWasdKeys(true, -1); break;
        case Qt::Key_D: handleWasdKeys(true, +1); break;
        case Qt::Key_Q: handlePrecKeys(-1); break;
        case Qt::Key_E: handlePrecKeys(+1); break;
        /* Moving between Records/Variants */
        case Qt::Key_Up:    handleArrowKeys(false, -1); break;
        case Qt::Key_Down:  handleArrowKeys(false, +1); break;
        case Qt::Key_Left:  handleArrowKeys(true, -1); break;
        case Qt::Key_Right: handleArrowKeys(true, +1); break;
        /* Managing Buttons */
        case Qt::Key_0: handleButtonKeys(0); break;
        case Qt::Key_1: handleButtonKeys(1); break;
        case Qt::Key_2: handleButtonKeys(2); break;
        case Qt::Key_3: handleButtonKeys(3); break;
        case Qt::Key_4: handleButtonKeys(4); break;
        case Qt::Key_5: handleButtonKeys(5); break;
        case Qt::Key_6: handleButtonKeys(6); break;
        case Qt::Key_7: handleButtonKeys(7); break;
        case Qt::Key_X: AButtonCut(); break;
        case Qt::Key_C: AButtonCopy(); break;
        case Qt::Key_V: AButtonPaste(); break;
        /* Managing Lines */
        case Qt::Key_Space: ALineCreate(); break;
        case Qt::Key_Backspace: ALineDelete(); break;
        /* Don't draw when not needed */
        default: return;
        }

        drawEditorGUI();
    }
}

/* Closing NeoBES */

void neobes::closeEvent(QCloseEvent *event) {
    if(hasEdited) {
        int wantsToGo = displaySaveDlg();
        if(!wantsToGo) event->ignore();
    }
}

int neobes::displaySaveDlg() {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Unsaved Changes");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("The project file has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);

    switch(msgBox.exec()) {
    case QMessageBox::Save:
        return ASaveProject();
    case QMessageBox::Discard:
        return true;
    default:
        return false;
    }
}

/* Drag and Drop */

void neobes::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

void neobes::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void neobes::dragLeaveEvent(QDragLeaveEvent *event) {
    event->accept();
}

void neobes::dropEvent(QDropEvent *event) {
    const QMimeData *mime(event->mimeData());
    QString fileName = QDir::toNativeSeparators(QUrl(mime->urls().at(0).toString()).toLocalFile());

    neodata::Log("Loading from drag & drop: " + fileName);
    int result = neodata::LoadFromBes(fileName);
    if(result == 0) { drawEditorGUI(); neodata::Log("Loaded project file successfully."); }
    else QMessageBox::critical(this, "Error on project load", strerror(result));
}

neobes::~neobes()
{
    delete ui;
}

void neobes::updateLog() {
    ui->textLog->setPlainText(logHistory);
}

/* LOAD PROJECT / SAVE PROJECT */

void neobes::ALoadProject()
{
    if(hasEdited) {
        int wantsToGo = displaySaveDlg();
        if(!wantsToGo) return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open BES Project"), "", tr("Any File (*)"));
    if(fileName.isEmpty()) return;
    neodata::Log("Loading project from file: " + fileName);

    int result = neodata::LoadFromBes(fileName);
    if(result == 0) {
        hasEdited = false;
        afterProjLoad();
        drawEditorGUI();
        neodata::Log("Loaded project file successfully.");
    }
    else QMessageBox::critical(this, "Error on project load", strerror(result));

    updateLog();
}

int neobes::ASaveProject()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save BES Project"), "", tr("Any File (*)"));
    if(fileName.isEmpty()) return 0;
    neodata::Log("Saving Project to: " + fileName);

    int result = neodata::SaveToBes(fileName);
    if(result == 0) {neodata::Log("Saved project file successfully.");}
    else {
        QMessageBox::critical(this, "Error on project save", strerror(result));
        neodata::Log("Could not save project." + QString(strerror(result)));
        return 0;
    }

    hasEdited = false;
    updateLog();
    return 1;
}

/* DOWNLOAD EMU / UPLOAD EMU */

void neobes::ADownloadEmu()
{
    if(hasEdited) {
        int wantsToGo = displaySaveDlg();
        if(!wantsToGo) return;
    }

    neodata::Log("Downloading from PCSX2...");

    switch(neodata::LoadFromEmu()) {
    case 0:
        hasEdited = false;
        afterProjLoad();
        drawEditorGUI();
        break;
    case 1:
        QMessageBox::critical(this, "Error on download", "PCSX2 wasn't found! Please verify that you're using PCSX2 with PINE enabled.");
        break;
    case 2:
        QMessageBox::critical(this, "Error on download", "NeoBES caused an error while loading from PCSX2.");
        break;
    case 3:
        QMessageBox::critical(this, "Error on download", "Can't confirm a compatible stage! Please verify game region and if is paused on a actual stage.");
        break;
    case 5:
        QMessageBox::critical(this, "Error on download", "This is not a compatible game. Please use a retail copy of PaRappa the Rapper 2 (US/JP/PAL).");
        break;
    }
}

void neobes::AUploadEmu()
{
    neodata::Log("Uploading to PCSX2...");

    int usedSize = neodata::CalcAvailableStorage();
    int avaiSize = StageInfo.buttondataend - StageInfo.buttondatabase + 1;
    if(usedSize > avaiSize) {
        updateLog();
        QMessageBox::information(this, "Error on upload", "The size of the project is bigger than the available space on the game's memory.");
        return;
    }

    switch(neodata::SaveToEmu()) {
    case 0:
        updateLog();
        QMessageBox::information(this, "Uploaded Successfully", "The project file was uploaded successfully into PCSX2.");
        break;
    case 1:
        QMessageBox::critical(this, "Error on upload", "PCSX2 wasn't found! Please verify that you're using PCSX2 with PINE enabled.");
        break;
    case 3:
        QMessageBox::critical(this, "Error on upload", "This project is for another stage! Please verify compatibility and if is paused on the correct stage.");
        break;
    case 5:
        QMessageBox::critical(this, "Error on upload", "This is not a compatible game. Please use a retail copy of PaRappa the Rapper 2 (US/JP/PAL).");
        break;
    default:
        QMessageBox::critical(this, "Error on upload", "NeoBES caused an error while uploading to PCSX2.");
        break;
    }
}

/* DOWNLOAD OLM / UPLOAD OLM */

void neobes::ADownloadOLM()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Downloaded OLM"), "", tr("Overlay Module (*.OLM)"));
    if(fileName.isEmpty()) return; // User Cancel
    int size = QInputDialog::getInt(this, tr("Type the OLM file size"), tr("Type file size"), 0, 0, 52428800);
    if(size == -1) return; // User Cancel

    switch(neodata::DownloadOLMFromEmu(fileName, size)) {
    case 1:
        QMessageBox::critical(this, "Error on download", "NeoBES caused an error while downloading to PCSX2.");
        break;
    case 0:
        QMessageBox::information(this, "Success on download", "The OLM File was downloaded successfully.");
        break;
    case 2:
        QMessageBox::critical(this, "Error on upload", "PCSX2 wasn't found! Please verify that you're using PCSX2 with PINE enabled.");
        break;
    };
}

void neobes::AUploadOLM()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load OLM to Upload"), "", tr("Overlay Module (*.OLM)"));
    if(fileName.isEmpty()) return; // User Cancel
    switch(neodata::UploadOLMToEmu(fileName)) {
    case false:
        QMessageBox::critical(this, "Error on upload", "NeoBES caused an error while uploading to PCSX2.");
        break;
    case true:
        QMessageBox::information(this, "Success on upload", "The OLM File was uploaded successfully.");
        break;
    case 2:
        QMessageBox::critical(this, "Error on upload", "PCSX2 wasn't found! Please verify that you're using PCSX2 with PINE enabled.");
        break;
    };
}

/* SETTINGS */

void neobes::ASettingsGUI() {
    Settings *settings = new Settings();
    settings->exec();
}

/* ABOUT */

#include "about.h"

void neobes::AAboutGUI() {
    About *about = new About();
    about->show();
}

/* GUI Core Methods */

void neobes::changeMenu() {
    if(Records.size() != 0) {
        isMenu = !isMenu;
        ui->actionChangeMenu->setEnabled(true);

        if(isMenu) drawMenuGUI();
        else drawEditorGUI();
    }
}

void neobes::drawMenuGUI() {
    ui->stackedWidget->setCurrentIndex(0);
    ui->actionChangeMenu->setText("Go to Editor");
}

void neobes::afterProjLoad() {
    QString stageNumber = "";
    if(CurrentStage < 9) {
        stageNumber = QString::number(CurrentStage + 1);
    } else {
        stageNumber = QString::number(CurrentStage - 10 + 1) + "-VS";
    }
    QWidget::setWindowTitle("NeoBES - S" + stageNumber + ": " + projFileName);

    CurrentRecord = 0;
    CurrentVariant = 0;
}

QString drawRecord(int owner, int start, int length, int interval, int cursorPos) {
    QString drawnRec;
    suggestbutton_t button;

    QString cursor = "<b style=\"background-color: " + accentColor + "\">%1</b>";
    QString iconStart = "<img src=\"%1\" width=" + QString::number(SettingsManager::instance().hudIconSize()) + ">";
    bool escapeEditorLines = SettingsManager::instance().hudEscapeLines();

    e_suggestvariant_t &variant = Records[CurrentRecord].variants[MentionedVariant];
    for (int dot = start; dot < start + length; dot += interval) {
        QString drawnIcon = "";

        for(int subcount = 0; subcount < interval; subcount++) {
            if(!variant.getButFromSubdot(owner, dot + subcount, button)) continue;
            drawnIcon = iconStart.arg(buttonsUIRes[button.buttonid]);
        }

        if(drawnIcon.isEmpty()) {
            int count = dot / interval;

            int isStar = count % 4 == 0;
            int isOwned = variant.isDotOwned(interval == 1 ? cursorpos : count, owner) ? 0 : 2;
            drawnIcon = iconStart.arg(buttonsUIResNot[isStar+isOwned]);
        }

        if(dot / interval % 32 == 0 && interval != 0 && interval != 1 && escapeEditorLines) drawnRec += "<br>";

        if(dot / interval == cursorPos) drawnIcon = cursor.arg(drawnIcon); // Add cursor
        drawnRec += drawnIcon;
    }

    isMenu = true;
    return drawnRec;
}

void neobes::drawButtonProperties() {
    suggestbutton_t button;
    ui->butProperty->blockSignals(true);
    if(Records[CurrentRecord].variants[MentionedVariant].getButFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos, button)) {
        ui->butProperty->setEnabled(true);

        for(int i = 0; i < 4; i++) {
            QTableWidgetItem *sdid = new QTableWidgetItem(QString::number((int16_t)button.sounds[i].soundid));
            QTableWidgetItem *anim = new QTableWidgetItem(QString::number((int16_t)button.sounds[i].animationid));
            QTableWidgetItem *time = new QTableWidgetItem(QString::number((int32_t)button.sounds[i].relativetime));

            ui->butProperty->setItem(i,0,sdid);
            ui->butProperty->setItem(i,1,anim);
            ui->butProperty->setItem(i,2,time);
        }
    } else {
        ui->butProperty->setEnabled(false);
    }
    ui->butProperty->blockSignals(false);
}

void neobes::updateButtonProperties(int row, int column) {
    suggestbutton_t *button;
    Records[CurrentRecord].variants[MentionedVariant].getButRefFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos, &button);
    QString valueChanged = ui->butProperty->item(row, column)->text();

    if (column == 0) { // Sound
        uint16_t soundId = (uint16_t)valueChanged.toInt();

        button->sounds[row].soundid = soundId;
        audio->playSound(soundId);
    }
    if (column == 1) { // Animation
        uint16_t animId = (uint16_t)valueChanged.toInt();

        button->sounds[row].animationid = animId;
    }
    if (column == 2) { // Timing
        uint32_t timing = (uint32_t)valueChanged.toInt();

        button->sounds[row].relativetime = timing;
    }
}

void neobes::drawLineProperties() {
    int count = 0;
    ui->lineOptions->setRowCount(Records[CurrentRecord].variants[MentionedVariant].lines.size());
    for(const e_suggestline_t &line : Records[CurrentRecord].variants[MentionedVariant].lines) {
        QTableWidgetItem *coolTres = new QTableWidgetItem(QString::number((int32_t)line.coolmodethreshold));
        QTableWidgetItem *ownerNam = new QTableWidgetItem(getOwnerName(line.owner));
        QTableWidgetItem *subtitle = new QTableWidgetItem("Placeholder");
        ui->lineOptions->setItem(count,0,coolTres);
        ui->lineOptions->setItem(count,1,ownerNam);
        ui->lineOptions->setItem(count,2,subtitle);
        count++;
    }
}

void neobes::updateLineProperties(int row, int column) {
    hasEdited = true;
    uint32_t coolTres = (uint32_t)ui->lineOptions->item(row,0)->text().toInt();
    Records[CurrentRecord].variants[MentionedVariant].lines[row].coolmodethreshold = coolTres;
}

void neobes::drawInfo() {
    int usedSize = neodata::CalcAvailableStorage();
    int avaiSize = StageInfo.buttondataend - StageInfo.buttondatabase + 1;
    int displaySize = avaiSize < usedSize ? avaiSize : usedSize;

    ui->infoSize->setText(QString::number(usedSize) + "/" + QString::number(avaiSize));

    ui->sizeProgressBar->setRange(0, avaiSize);
    ui->sizeProgressBar->setValue(displaySize);
    QPalette palette = ui->sizeProgressBar->palette();
    palette.setColor(QPalette::Accent, QColor(avaiSize < usedSize ? "#AA0000" : accentColor));
    ui->sizeProgressBar->setPalette(palette);

    ui->variantInput->setValue(CurrentVariant);
    ui->recordInput->setValue(CurrentRecord);
    ui->recordInput->setMaximum(Records.size() - 1);
}

void neobes::drawEditorGUI() {
    ui->stackedWidget->setCurrentIndex(1);
    ui->actionChangeMenu->setText("Go to Menu");
    drawCommands();
    QString textGUI = Records[CurrentRecord].variants[CurrentVariant].islinked ? QString("Linked to %1<br>").arg(Records[CurrentRecord].variants[CurrentVariant].linknum) : "<br>";
    updateLog();

    for(int curOwner = 0; curOwner < numowners; curOwner++) {
        textGUI += "<h2>" + ownerNames[curOwner+2].name + "</h2>"; // FIXME: Calculate properly
        textGUI += "" + drawRecord(owners[curOwner], 0, Records[CurrentRecord].lengthinsubdots, 24, curOwner == cursorowner ? cursorpos : -1) + "<br>";
    }
    if(SettingsManager::instance().hudCenterVisual()) textGUI = QString("<center>%1</center><br>").arg(textGUI);

    textGUI += "<h3>Precision</h3><br>" + drawRecord(owners[cursorowner], cursorpos * 24, 24, 1, cursorpos * 24 + precpos);

    drawInfo();
    drawButtonProperties();
    drawLineProperties();
    ui->display->setText(textGUI);

    isMenu = false;
}

#include "intcommand.h"

bool once = false;
void neobes::setupCommandCB() {
    ui->comSelector->blockSignals(true);
    if(VSMode) {

    } else {
        ui->comSelector->addItem("Cool", 0);
        ui->comSelector->addItem("Good", 1);
        ui->comSelector->addItem("Bad", 2);
        ui->comSelector->addItem("Awful", 3);
    }
    ui->comSelector->blockSignals(false);
}

void neobes::drawCommands() { // NOTE TO JOO: I left it as it is due to extreme delay when loading :)
    if(!once) {setupCommandCB(); once = true;}
    int currentCommandSelect = ui->comSelector->currentIndex();
    int curRow = 0;

    ui->comTable->blockSignals(true);
    int size = ModeCommands[currentCommandSelect].size() > 0 ? ModeCommands[currentCommandSelect].size() : 0;
    ui->comTable->setRowCount(size);
    for(commandbuffer_t &command : ModeCommands[currentCommandSelect]) {
        GUICommand gui = intcommand::ConvertToGUI(command);
        QTableWidgetItem *ctyp = new QTableWidgetItem(gui.commandType);
        QTableWidgetItem *arg1 = new QTableWidgetItem(gui.arg1);
        QTableWidgetItem *arg2 = new QTableWidgetItem(gui.arg2);
        QTableWidgetItem *arg3 = new QTableWidgetItem(gui.arg3);
        QTableWidgetItem *arg4 = new QTableWidgetItem(gui.arg4);
        ui->comTable->setItem(curRow, 0, ctyp);
        ui->comTable->setItem(curRow, 1, arg1);
        ui->comTable->setItem(curRow, 2, arg2);
        ui->comTable->setItem(curRow, 3, arg3);
        ui->comTable->setItem(curRow, 4, arg4);
        curRow++;
    }
    ui->comTable->blockSignals(false);
}

void neobes::updateCommandProperties() {
    hasEdited = true;
    int curComSel = ui->comSelector->currentIndex();
    int curCol = ui->comTable->currentColumn();
    int curRow = ui->comTable->currentRow();

    QString editedText = ui->comTable->item(curRow, curCol)->text();
    commandbuffer_t tmpCommand = ModeCommands[curComSel][curRow];

    int newInput = intcommand::ConvertToNormal(editedText, tmpCommand, curRow, curCol);

    switch(curCol) {
    case 0:
        ModeCommands[curComSel][curRow].cmd_id = newInput;
        break;
    case 1:
        ModeCommands[curComSel][curRow].arg1 = newInput;
        break;
    case 2:
        ModeCommands[curComSel][curRow].arg2 = newInput;
        break;
    case 3:
        ModeCommands[curComSel][curRow].arg3 = newInput;
        break;
    case 4:
        ModeCommands[curComSel][curRow].arg4 = newInput;
        break;
    }

    ui->comTable->blockSignals(true);
    GUICommand gui = intcommand::ConvertToGUI(ModeCommands[curComSel][curRow]);
    QTableWidgetItem *ctyp = new QTableWidgetItem(gui.commandType);
    QTableWidgetItem *arg1 = new QTableWidgetItem(gui.arg1);
    QTableWidgetItem *arg2 = new QTableWidgetItem(gui.arg2);
    QTableWidgetItem *arg3 = new QTableWidgetItem(gui.arg3);
    QTableWidgetItem *arg4 = new QTableWidgetItem(gui.arg4);
    ui->comTable->setItem(curRow, 0, ctyp);
    ui->comTable->setItem(curRow, 1, arg1);
    ui->comTable->setItem(curRow, 2, arg2);
    ui->comTable->setItem(curRow, 3, arg3);
    ui->comTable->setItem(curRow, 4, arg4);
    ui->comTable->blockSignals(false);
}

/* GUI Functions */

void neobes::handleDisplayClick() {
    int testPos = ui->display->textCursor().position();
    neodata::Log(QString::number(testPos));
}

void neobes::handleArrowKeys(bool lr, int inc) {
    if(lr && CurrentRecord+inc >= 0 && CurrentRecord+inc < Records.size()) {
        CurrentRecord += inc;
        audio->loadSoundDB(Records[CurrentRecord].soundboardid - 1);
    }
    else if(!lr && CurrentVariant+inc >= 0 && CurrentVariant+inc < 17) {
        CurrentVariant += inc;
        MentionedVariant = Records[CurrentRecord].variants[CurrentVariant].islinked ? Records[CurrentRecord].variants[CurrentVariant].linknum : CurrentVariant;
    }
}

void neobes::handleWasdKeys(bool lr, int inc) {
    if(lr) {
        if(cursorpos+inc >= 0 && cursorpos+inc < Records[CurrentRecord].lengthinsubdots / 24) cursorpos += inc;
        else {
            if(cursorpos+inc < 0) cursorpos = (Records[CurrentRecord].lengthinsubdots / 24) - 1;

            if(cursorpos+inc >= Records[CurrentRecord].lengthinsubdots / 24) cursorpos = 0;
        }
    }
    else {
        if(cursorowner+inc >= 0 && cursorowner+inc < numowners) cursorowner += inc;
        else {
            if(cursorowner+inc < 0) cursorowner = numowners - 1;

            if(cursorowner+inc >= numowners) cursorowner = 0;
        }
    }
}

void neobes::handlePrecKeys(int inc) {
    if(precpos+inc >= 0 && precpos+inc < 24) precpos += inc;
}

void neobes::handleButtonKeys(int buttonId) {
    hasEdited = true;
    if(buttonId == 0) AButtonDelete();
    else Records[CurrentRecord].variants[MentionedVariant].createButton((cursorpos * 24) + precpos, owners[cursorowner], buttonId);
}

// TODO: Expose variables for managing with cursor?
void neobes::AButtonCopy() {
    suggestbutton_t button; // TODO: Test done without copying it
    if(Records[CurrentRecord].variants[MentionedVariant].getButFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos, button)) copiedButton = button;
}

void neobes::AButtonCut() {
    AButtonCopy();
    AButtonDelete();
}

void neobes::AButtonDelete() {
    hasEdited = true;
    Records[CurrentRecord].variants[MentionedVariant].deleteButton((cursorpos * 24) + precpos, owners[cursorowner]);
}

void neobes::AButtonPaste() { // TODO: Check better solution for Paste
    suggestbutton_t *pbutton;

    AButtonDelete();
    Records[CurrentRecord].variants[MentionedVariant].createButton((cursorpos * 24) + precpos, owners[cursorowner], copiedButton.buttonid);

    if(Records[CurrentRecord].variants[MentionedVariant].getButRefFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos, &pbutton)) {
        for(int s = 0; s < 4; s++) pbutton->sounds[s] = copiedButton.sounds[s];
    }
}

void neobes::ALineCreate() {
    hasEdited = true;
    Records[CurrentRecord].variants[MentionedVariant].createLine((cursorpos * 24) + precpos, (cursorpos * 24) + precpos + 24, owners[cursorowner]);
}

void neobes::ALineDelete() {
    hasEdited = true;
    Records[CurrentRecord].variants[MentionedVariant].deleteLine((cursorpos * 24) + precpos, owners[cursorowner]);
}

void neobes::ACommandCreate() {
    hasEdited = true;
    int curComSel = ui->comSelector->currentIndex();
    int curRow = ui->comTable->currentRow();
    commandbuffer_t newCom;
    newCom.cmd_id = 0;
    newCom.arg1 = 0;
    newCom.arg2 = 0;
    newCom.arg3 = 0;
    newCom.arg4 = 0;

    ModeCommands[curComSel].insert(ModeCommands[curComSel].begin() + curRow, newCom);
    drawCommands();
}

void neobes::ACommandDelete() {
    hasEdited = true;
    int curComSel = ui->comSelector->currentIndex();
    int curRow = ui->comTable->currentRow();

    ModeCommands[curComSel].erase(ModeCommands[curComSel].begin() + curRow);
    drawCommands();
}

void neobes::ALinkVariant(bool linkAll)
{
    int linkId = QInputDialog::getInt(this, tr("Link Variant"), tr("Type the variant to link to"), 0, 0, 16);
    if(linkId == -1) return; // User Cancel

    if(linkAll) {
        for(int i = 0; i < 17; i++) {
            hasEdited = true;
            if(i != linkId) Records[CurrentRecord].variants[i].setLink(linkId);
        }
    } else {
        if(linkId == CurrentVariant) {
            linkId = -1; // Unlink
        }
        else if(Records[CurrentRecord].variants[linkId].islinked) {
            QMessageBox::warning(this, "Link Variant", "You can't link a linked variant!");
            neodata::Log("Can't link linked variant!");
            updateLog();
            return;
        }

        hasEdited = true;
        Records[CurrentRecord].variants[CurrentVariant].setLink(linkId);
    }

    drawEditorGUI();
}

void neobes::ASetSoundboard()
{
    hasEdited = true;
    int sbId = QInputDialog::getInt(this, tr("Soundboard Change"), tr("Type the soundboard id to change"), 0, 0, Records.size());
    if(sbId == -1) return; // User Cancel

    if(sbId >= Soundboards.size()) {
        QMessageBox::critical(this, "Soundboard Change", "Invalid soundboard number!");
        neodata::Log("Invalid soundboard number!");
        updateLog();
        return;
    }

    Records[CurrentRecord].soundboardid = sbId;
}

void neobes::APlayVariant(bool ticker) {
    audio->playVariant(Records[CurrentRecord].variants[MentionedVariant], Records[CurrentRecord].lengthinsubdots, StageInfo.bpm, ticker);
}
