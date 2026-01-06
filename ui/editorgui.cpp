#include "editorgui.h"
#include "ui/ui_editorgui.h"

#include <QFileDialog>
#include <QInputDialog>

#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>

bool isEditorActive = false;
bool oldProjectLimit = false;

int cursorpos = 0;
int cursorowner = 0;
int precpos = 0;
QString fileName;

int numowners = 3;
PLAYER_CODE owners[3] = {PLAYER_CODE::PCODE_TEACHER, PLAYER_CODE::PCODE_PARA, PLAYER_CODE::PCODE_BOXY};

static const QMap<PLAYER_CODE, QString> ownerNames = {
    {PLAYER_CODE::PCODE_NONE,    "None"},
    {PLAYER_CODE::PCODE_TEACHER, "Teacher"},
    {PLAYER_CODE::PCODE_PARA,    "PaRappa"},
    {PLAYER_CODE::PCODE_BOXY,    "SFX"},
    {PLAYER_CODE::PCODE_MOVE,    "Scene"}
};


QString getOwnerName(PLAYER_CODE code) {
    return ownerNames.value(code, "Unknown");
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

suggestbutton_t *copiedButton;

AudioPlayer *audio;

QString accentColor;

editorgui::editorgui(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::editorgui)
{
    ui->setupUi(this);
    pcsx2reader::SetupIPC();

    this->setFocusPolicy(Qt::StrongFocus);
    ui->variantInput->setMaximum(16);

    connect(ui->recordInput, QOverload<int>::of(&QSpinBox::valueChanged), this, [=, this]() {
        CurrentRecord = ui->recordInput->value();
        editorgui::drawEditorGUI();
    });
    connect(ui->variantInput, QOverload<int>::of(&QSpinBox::valueChanged), this, [=, this]() {
        CurrentVariant = ui->variantInput->value();
        editorgui::drawEditorGUI();
    });

    connect(ui->butProperty, &QTableWidget::cellChanged, this, &editorgui::updateButtonProperties);
    connect(ui->lineOptions, &QTableWidget::cellChanged, this, &editorgui::updateLineProperties);
    connect(ui->comSelector, &QComboBox::currentIndexChanged, this, [=, this]() {
        editorgui::drawCommands();
    });
    connect(ui->comTable, &QTableWidget::cellChanged, this, &editorgui::updateCommandProperties);
    connect(ui->addCom, &QPushButton::clicked, this, &editorgui::ACommandCreate);
    connect(ui->remCom, &QPushButton::clicked, this, &editorgui::ACommandDelete);

    /* Init audio */
    audio = new AudioPlayer();
    audio->initialize();

    // Setup accent color
    accentColor = QApplication::palette().color(QPalette::Highlight).name();
}

editorgui::~editorgui()
{
    delete ui;
}

void editorgui::updateLog() {
    ui->textLog->setPlainText(logHistory);
}

void editorgui::toggleActive() {
    isEditorActive = !isEditorActive;
}

void editorgui::closeEvent(QCloseEvent *event) {
    if(hasEdited) {
        int wantsToGo = displaySaveDlg();
        if(!wantsToGo) event->ignore();
    }
}

int editorgui::displaySaveDlg() {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Unsaved Changes");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("The project file has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);

    switch(msgBox.exec()) {
    case QMessageBox::Save:
        return ASaveAsProject();
    case QMessageBox::Discard:
        return true;
    default:
        return false;
    }
}

/* KEYBINDINGS */
void editorgui::keyPressEvent(QKeyEvent *event)
{
    if(isEditorActive) { // Only get keys when in editor
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
        case Qt::Key_Comma: ALineLeftExpand(cursorpos * 24, owners[cursorowner]); break;
        case Qt::Key_Period: ALineRightExpand(cursorpos * 24, owners[cursorowner]); break;
        case Qt::Key_N: ALineMoveLeftLine(cursorpos * 24, owners[cursorowner]); break;
        case Qt::Key_M: ALineMoveRightLine(cursorpos * 24, owners[cursorowner]); break;

        /* Don't draw when not needed */
        default: return;
        }

        drawEditorGUI();
    }
}

/* LOAD PROJECT / SAVE PROJECT */

void editorgui::loadProject(QString tmpFileName)
{
    if(hasEdited) {
        int wantsToGo = displaySaveDlg();
        if(!wantsToGo) return;
    }

    if(tmpFileName.isEmpty()) return;
    neodata::Log("Loading project from file: " + tmpFileName);

    int result = neodata::LoadFromBes(tmpFileName);
    if(result == 0 || result == 254) {
        hasEdited = false;
        emit updateMenuHistoryFile();
        afterProjLoad();
        drawEditorGUI();

        if (result == 254) {
            neodata::Log("Project file is a old BESMS project.");
            QMessageBox::warning(this, "Old Project", "This is a old project made in other besms tools that doesn't have all the data of the stage. This was responsible for errors or crashes where there wouldn't be in the original game.\n\nThe necessary data will be added if you use Upload to PCSX2. This won't affect other tools.");
            oldProjectLimit = true;
        }

        neodata::Log("Loaded project file successfully.");
    }
    else QMessageBox::critical(this, "Error on project load", strerror(result));

    updateLog();
}

void editorgui::ALoadProject()
{
    loadProject(QFileDialog::getOpenFileName(this, tr("Open BES Project"), "", tr("Any File (*)")));
}

int editorgui::ASaveProject()
{
    if(fileName.isEmpty()) return editorgui::ASaveAsProject();
    neodata::Log("Saving Project to: " + fileName);

    int result = neodata::SaveToBes(fileName);
    if(result == 0) {
        neodata::Log("Saved project file successfully.");
        hasEdited = false;
    }
    else {
        QMessageBox::critical(this, "Error on project save", strerror(result));
        neodata::Log("Could not save project." + QString(strerror(result)));
        return 0;
    }

    updateLog();
    return 1;
}

int editorgui::ASaveAsProject()
{
    QString tmpFileName = QFileDialog::getSaveFileName(this, tr("Save BES Project"), "", tr("Any File (*)"));
    if(tmpFileName.isEmpty()) return 0;
    neodata::Log("Saving Project to: " + tmpFileName);

    int result = neodata::SaveToBes(tmpFileName);
    if(result == 0) {
        neodata::Log("Saved project file successfully.");
        setProjectWindowName();
    } else {
        QMessageBox::critical(this, "Error on project save", strerror(result));
        neodata::Log("Could not save project." + QString(strerror(result)));
        return 0;
    }

    hasEdited = false;
    updateLog();
    return 1;
}

/* DOWNLOAD EMU / UPLOAD EMU */

void editorgui::ADownloadEmu()
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

void editorgui::AUploadEmu()
{
    neodata::Log("Uploading to PCSX2...");

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
    case 6:
        QMessageBox::information(this, "Error on upload", "The size of the project is bigger than the available space on the game's memory.");
        break;
    default:
        QMessageBox::critical(this, "Error on upload", "NeoBES caused an error while uploading to PCSX2.");
        break;
    }
}

/* DOWNLOAD OLM / UPLOAD OLM */

void editorgui::ADownloadOLM()
{
    QString tmpFileName = QFileDialog::getSaveFileName(this, tr("Save Downloaded OLM"), "", tr("Overlay Module (*.OLM)"));
    if(tmpFileName.isEmpty()) return; // User Cancel

    bool userConfirmation;
    int size = QInputDialog::getInt(this, tr("Type the OLM file size"), tr("Type file size"), 0, 0, 52428800, 1, &userConfirmation);
    if(!userConfirmation) return; // User Cancel 2

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

void editorgui::AUploadOLM()
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

/* GUI Core Methods */

void editorgui::setProjectWindowName() {
    QString stageNumber = "";
    if(CurrentStage < 9) {
        stageNumber = QString::number(CurrentStage + 1);
    } else {
        stageNumber = QString::number(CurrentStage - 10 + 1) + "-VS";
    }
    emit setWindowName("NeoBES - S" + stageNumber + ": " + ProjectInfo.prettyFileName);
}

void editorgui::afterProjLoad() {
    setProjectWindowName();
    emit setDestructive(true);
    if(!isEditorActive) emit editorReady();

    CurrentRecord = 0;
    CurrentVariant = 0;
}

QString drawLine(int start, int end, int interval, PLAYER_CODE owner, int skip, int cursorPos, bool escapeEditorLines) {
    e_suggestvariant_t &variant = Records[CurrentRecord].variants[MentionedVariant];
    QString iconStart = "<img src=\"%1\" width=" + QString::number(SettingsManager::instance().hudIconSize()) + ">";
    QString cursor = "<b style=\"background-color: " + accentColor + "\">%1</b>";
    QString out;

    for (int dot = start; dot < end; dot += interval) {
        QString drawnIcon = "";

        for(int subcount = 0; subcount < interval; subcount++) {
            if(suggestbutton_t *button = variant.getButtonRefFromSubdot(owner, dot + subcount)) {
                drawnIcon = iconStart.arg(buttonsUIRes[button->buttonid]);
            }
        }

        if(drawnIcon.isEmpty()) {
            int count = dot / interval;

            int isStar = count % 4 == 0;
            int isOwned = variant.isDotOwned(interval == 1 ? cursorpos : count, owner) ? 0 : 2;
            drawnIcon = iconStart.arg(buttonsUIResNot[isStar+isOwned]);
        }

        if(dot / interval % 32 == 0 && interval != 0 && interval != 1 && escapeEditorLines) out += "<br>";

        if(dot / interval == cursorPos) drawnIcon = cursor.arg(drawnIcon); // Add cursor
        out += drawnIcon;
    }

    return out;
}

QString drawRecord(PLAYER_CODE owner, int start, int length, int interval, int cursorPos) {
    bool escapeEditorLines = SettingsManager::instance().hudEscapeLines();
    QString drawnRec;

    drawnRec = drawLine(start, start + length, interval, owner, interval, cursorPos, escapeEditorLines);

    return drawnRec;
}

void editorgui::drawButtonProperties() {
    suggestbutton_t *button = Records[CurrentRecord].variants[MentionedVariant].getButtonRefFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos);
    ui->butProperty->blockSignals(true);
    if(button) {
        ui->butProperty->setEnabled(true);

        for(int i = 0; i < 4; i++) {
            QTableWidgetItem *sdid = new QTableWidgetItem(QString::number((int16_t)button->sounds[i].soundid));
            QTableWidgetItem *anim = new QTableWidgetItem(QString::number((int16_t)button->sounds[i].animationid));
            QTableWidgetItem *time = new QTableWidgetItem(QString::number((int32_t)button->sounds[i].relativetime));

            ui->butProperty->setItem(i,0,sdid);
            ui->butProperty->setItem(i,1,anim);
            ui->butProperty->setItem(i,2,time);
        }
    } else {
        ui->butProperty->setEnabled(false);
    }
    ui->butProperty->blockSignals(false);
}

void editorgui::updateButtonProperties(int row, int column) {
    suggestbutton_t *button = Records[CurrentRecord].variants[MentionedVariant].getButtonRefFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos);
    QString valueChanged = ui->butProperty->item(row, column)->text();

    if (column == 0) { // Sound
        int16_t soundId = (int16_t)valueChanged.toInt();

        button->sounds[row].soundid = soundId;
        audio->playSound(soundId);
    }
    if (column == 1) { // Animation
        int32_t animId = (int32_t)valueChanged.toInt();

        button->sounds[row].animationid = animId;
    }
    if (column == 2) { // Timing
        int32_t timing = (int32_t)valueChanged.toInt();

        button->sounds[row].relativetime = timing;
    }
}

void editorgui::drawLineProperties() {
    int count = 0;
    bool isSelected = false;
    ui->lineOptions->setRowCount(Records[CurrentRecord].variants[MentionedVariant].lines.size());
    for(const e_suggestline_t &line : Records[CurrentRecord].variants[MentionedVariant].lines) {
        QTableWidgetItem *coolTres = new QTableWidgetItem(QString::number((int32_t)line.coolmodethreshold));
        QTableWidgetItem *ownerNam = new QTableWidgetItem(getOwnerName(line.owner));

        suggestbutton_t button;
        QString drawn = drawLine(line.timestamp_start, line.timestamp_end, 24, line.owner, 24, -1, false);
        QLabel *lineLabel = new QLabel(drawn);

        ui->lineOptions->setItem(count,0,coolTres);
        ui->lineOptions->setItem(count,1,ownerNam);
        ui->lineOptions->setCellWidget(count,2,lineLabel);
        count++;

        if(owners[cursorowner] == line.owner && (cursorpos * 24 >= line.timestamp_start && cursorpos * 24 < line.timestamp_end)) {
            isSelected = true;
            ui->lineOptions->selectRow(count-1);
        }
    }

    if(!isSelected) ui->lineOptions->clearSelection();
}

void editorgui::updateLineProperties(int row, int column) {
    hasEdited = true;
    int32_t coolTres = ui->lineOptions->item(row,0)->text().toInt();
    Records[CurrentRecord].variants[MentionedVariant].lines[row].coolmodethreshold = coolTres;
}

void editorgui::drawInfo() {
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

void editorgui::drawEditorGUI() {
    drawCommands();
    QString textGUI = Records[CurrentRecord].variants[CurrentVariant].islinked ? QString("Linked to %1<br>").arg(Records[CurrentRecord].variants[CurrentVariant].linknum) : "<br>";
    MentionedVariant = Records[CurrentRecord].variants[CurrentVariant].islinked ? Records[CurrentRecord].variants[CurrentVariant].linknum : CurrentVariant;
    audio->loadSoundDB(Records[CurrentRecord].soundboardid - 1);
    updateLog();

    for(int curOwner = 0; curOwner < numowners; curOwner++) {
        textGUI += "<h2>" + getOwnerName(owners[curOwner]) + "</h2>";
        textGUI += "" + drawRecord(owners[curOwner], 0, Records[CurrentRecord].lengthinsubdots, 24, curOwner == cursorowner ? cursorpos : -1) + "<br>";
    }
    if(SettingsManager::instance().hudCenterVisual()) textGUI = QString("<center>%1</center><br>").arg(textGUI);

    textGUI += "<h3>Precision</h3><br>" + drawRecord(owners[cursorowner], cursorpos * 24, 24, 1, cursorpos * 24 + precpos);

    drawInfo();
    drawButtonProperties();
    drawLineProperties();
    ui->display->setText(textGUI);
}

#include "job.h"

bool once = false;
void editorgui::setupCommandCB() {

    QString commandListNames[27] = {"Cool", "Good", "Bad", "Awful", "Trans. to Cool", "Trans. from Cool", "Trans. to Better", "Trans. to Worse", "Game Over Scene", "Unknown"};
    int commCount = 0;

    ui->comSelector->blockSignals(true);
    if(VSMode) {

    } else {
        for(int i = 0; i < ModeSize; i++) {
            int commName = commCount > 9 ? 9 : commCount;
            ui->comSelector->addItem(QString::number(commCount) + ": " + commandListNames[commName], commCount);
            commCount++;
        }
    }
    ui->comSelector->blockSignals(false);
}

void editorgui::drawCommands() { // NOTE TO JOO: I left it as it is due to extreme delay when loading :)
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

void editorgui::updateCommandProperties() {
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

void editorgui::handleDisplayClick() {
    int testPos = ui->display->textCursor().position();
    neodata::Log(QString::number(testPos));
}

void editorgui::handleArrowKeys(bool lr, int inc) {
    if(lr && CurrentRecord+inc >= 0 && CurrentRecord+inc < Records.size()) {
        CurrentRecord += inc;
    }
    else if(!lr && CurrentVariant+inc >= 0 && CurrentVariant+inc < 17) {
        CurrentVariant += inc;
    }
}

void editorgui::handleWasdKeys(bool lr, int inc) {
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

void editorgui::handlePrecKeys(int inc) {
    if(precpos+inc >= 0 && precpos+inc < 24) precpos += inc;
}

void editorgui::handleButtonKeys(int buttonId) {
    hasEdited = true;
    if(buttonId == 0) AButtonDelete();
    else Records[CurrentRecord].variants[MentionedVariant].createButton((cursorpos * 24) + precpos, owners[cursorowner], buttonId);
}

// TODO: Expose variables for managing with cursor?
void editorgui::AButtonCopy() {
    suggestbutton_t *button = Records[CurrentRecord].variants[MentionedVariant].getButtonRefFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos);
    if(button) copiedButton = button;
}

void editorgui::AButtonCut() {
    AButtonCopy();
    AButtonDelete();
}

void editorgui::AButtonDelete() {
    hasEdited = true;
    Records[CurrentRecord].variants[MentionedVariant].deleteButton((cursorpos * 24) + precpos, owners[cursorowner]);
}

void editorgui::AButtonPaste() {
    AButtonDelete();
    Records[CurrentRecord].variants[MentionedVariant].createButton((cursorpos * 24) + precpos, owners[cursorowner], copiedButton->buttonid);

    suggestbutton_t *pbutton = Records[CurrentRecord].variants[MentionedVariant].getButtonRefFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos);
    if(pbutton) {
        for(int s = 0; s < 4; s++) pbutton->sounds[s] = copiedButton->sounds[s];
    }
}

void editorgui::ALineCreate() {
    hasEdited = true;
    Records[CurrentRecord].variants[MentionedVariant].createLine(cursorpos * 24, (cursorpos * 24) + 24, owners[cursorowner]);
}

void editorgui::ALineDelete() {
    hasEdited = true;
    Records[CurrentRecord].variants[MentionedVariant].deleteLine(cursorpos * 24, owners[cursorowner]);
}

void editorgui::ACommandCreate() {
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

void editorgui::ACommandDelete() {
    hasEdited = true;
    int curComSel = ui->comSelector->currentIndex();
    int curRow = ui->comTable->currentRow();

    ModeCommands[curComSel].erase(ModeCommands[curComSel].begin() + curRow);
    drawCommands();
}

void editorgui::ALinkVariant(bool linkAll)
{
    bool userConfirmation;
    int linkId = QInputDialog::getInt(this, tr("Link Variant"), tr("Type the variant to link to"), 0, 0, 16, 1, &userConfirmation);
    if(!userConfirmation) return;

    if(linkId == CurrentVariant && !linkAll) {
        linkId = -1; // Unlink
    }
    else if(Records[CurrentRecord].variants[linkId].islinked) {
        QMessageBox::warning(this, "Link Variant", "You can't link a linked variant!");
        neodata::Log("Can't link linked variant!");
        updateLog();
        return;
    }

    hasEdited = true;
    if(linkAll) {
        for(int i = 0; i < 17; i++) {
            if(i != linkId) Records[CurrentRecord].variants[i].setLink(linkId);
        }
    } else {
        Records[CurrentRecord].variants[CurrentVariant].setLink(linkId);
    }

    drawEditorGUI();
}

void editorgui::ASetSoundboard()
{
    bool userConfirmation;
    int sbId = QInputDialog::getInt(this, tr("Soundboard Change"), tr("Type the soundboard id to change"), 0, 0, Records.size(), 1, &userConfirmation);
    if(!userConfirmation) return;

    if(sbId >= Soundboards.size()) {
        QMessageBox::critical(this, "Soundboard Change", "Invalid soundboard number!");
        neodata::Log("Invalid soundboard number!");
        updateLog();
        return;
    }

    hasEdited = true;
    Records[CurrentRecord].soundboardid = sbId;
    audio->loadSoundDB(Records[CurrentRecord].soundboardid - 1);
}

void editorgui::APlayVariant(bool ticker) {
    audio->playVariant(Records[CurrentRecord].variants[MentionedVariant], Records[CurrentRecord].lengthinsubdots, StageInfo.bpm, ticker);
}

/* Line Resize */

void editorgui::ALineLeftExpand(uint32_t subdot, PLAYER_CODE owner) {
    Records[CurrentRecord].variants[CurrentVariant].resizeMoveLine(subdot, owner, true, false);
}

void editorgui::ALineRightExpand(uint32_t subdot, PLAYER_CODE owner) {
    Records[CurrentRecord].variants[CurrentVariant].resizeMoveLine(subdot, owner, false, false);
}

void editorgui::ALineMoveLeftLine(uint32_t subdot, PLAYER_CODE owner) {
    Records[CurrentRecord].variants[CurrentVariant].resizeMoveLine(subdot, owner, true, true);
}

void editorgui::ALineMoveRightLine(uint32_t subdot, PLAYER_CODE owner) {
    Records[CurrentRecord].variants[CurrentVariant].resizeMoveLine(subdot, owner, false, true);
}

