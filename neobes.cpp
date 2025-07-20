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

/* Configs */
bool escapeEditorLines = true;
bool centerEditorLines = true;
int ownerSetting[3] = {0x2, 0x4, 0x8};

/*QString ownerNames[] = {
    "None",
    "???", // 0x1?
    "Teacher", //0x2
    "PaRappa", //0x4
    "SFX", // 0x8
    "Scene" // 0x10?
};*/

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
    this->setFocusPolicy(Qt::StrongFocus); // may help may not

    ui->variantInput->setMaximum(16);

    connect(ui->selregBtn, &QPushButton::clicked, this, &neobes::ASelectRegion);

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

    connect(ui->comSelector, &QComboBox::currentIndexChanged, this, [=, this]() {
        neobes::drawCommands();
    });
    connect(ui->comTable, &QTableWidget::cellChanged, this, &neobes::updateCommandProperties);

    /* Init audio */
    audio = new AudioPlayer();
    audio->initialize();

    // Setup accent color
    accentColor = QApplication::palette().color(QPalette::Highlight).name();
}

/* Editor Keyboard Input */
void neobes::keyPressEvent(QKeyEvent *event)
{    
    // Escape function
    if(event->key() == Qt::Key_Escape && Records.size() != 0) {
        isMenu = !isMenu;

        if(isMenu) drawMenuGUI();
        else drawEditorGUI();
        return;
    }

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
        /* Don't draw when not needed */
        default: return;
        }

        drawEditorGUI();
    }
}

/* Closing NeoBES */

void neobes::closeEvent(QCloseEvent *event) {
    if(hasEdited) {
        int result = displaySaveDlg();
        switch(result) {
        case QMessageBox::Save:
            ASaveProject();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        }
    }
}

int neobes::displaySaveDlg() {
    QMessageBox msgBox;
    msgBox.setText("The Project file has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    return msgBox.exec();
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open BES Project"), "", tr("Any File (*)"));
    if(fileName == "") return;
    neodata::Log("Loading project from file: " + fileName);

    int result = neodata::LoadFromBes(fileName);
    if(result == 0) { drawEditorGUI(); neodata::Log("Loaded project file successfully."); }
    else QMessageBox::critical(this, "Error on project load", strerror(result));

    updateLog();
}

void neobes::ASaveProject()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save BES Project"), "", tr("Any File (*)"));
    if(fileName == "") return;
    neodata::Log("Saving Project to: " + fileName);

    int result = neodata::SaveToBes(fileName);
    if(result == 0) {neodata::Log("Saved project file successfully.");}
    else QMessageBox::critical(this, "Error on project save", strerror(result));

    updateLog();
}

/* DOWNLOAD EMU / UPLOAD EMU */

void neobes::ADownloadEmu()
{
    neodata::Log("Downloading from PCSX2...");

    switch(neodata::LoadFromEmu()) {
    case 0:
        drawEditorGUI();
        break;
    case 1:
        QMessageBox::critical(this, "Error on download", "PCSX2 wasn't found! Please verify that you're using PCSX2 Nightly with PINE enabled.");
        break;
    case 2:
        QMessageBox::critical(this, "Error on download", "NeoBES caused an error while loading from PCSX2.");
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
        QMessageBox::critical(this, "Error on upload", "PCSX2 wasn't found! Please verify that you're using PCSX2 Nightly with PINE enabled.");
        break;
    case 2:
        QMessageBox::critical(this, "Error on upload", "NeoBES caused an error while uploading to PCSX2.");
        break;
    }
}

/* DOWNLOAD OLM / UPLOAD OLM */

void neobes::ADownloadOLM()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Downloaded OLM"), "", tr("Overlay Module (*.OLM)"));
    if(fileName == "") return; // User Cancel
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
        QMessageBox::critical(this, "Error on upload", "PCSX2 wasn't found! Please verify that you're using PCSX2 Nightly with PINE enabled.");
        break;
    };
}

void neobes::AUploadOLM()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load OLM to Upload"), "", tr("Overlay Module (*.OLM)"));
    if(fileName == "") return; // User Cancel
    switch(neodata::UploadOLMToEmu(fileName)) {
    case false:
        QMessageBox::critical(this, "Error on upload", "NeoBES caused an error while uploading to PCSX2.");
        break;
    case true:
        QMessageBox::information(this, "Success on upload", "The OLM File was uploaded successfully.");
        break;
    case 2:
        QMessageBox::critical(this, "Error on upload", "PCSX2 wasn't found! Please verify that you're using PCSX2 Nightly with PINE enabled.");
        break;
    };
}

/* REGION SELECT */

void neobes::ASelectRegion() {
    QInputDialog regionPicker;
    QStringList regionList;
    regionList << "NTSC-U" << "PAL" << "NTSC-J";

    regionPicker.setOptions(QInputDialog::UseListViewForComboBoxItems);
    regionPicker.setComboBoxItems(regionList);
    regionPicker.setWindowTitle("Region select");
    regionPicker.setLabelText("Select the region");
    CurrentRegion = regionPicker.exec();
}

/* ABOUT */

#include "about.h"

void neobes::AAboutGUI() {
    About *about = new About();
    about->show();
}

/* GUI Core Methods */

void neobes::drawMenuGUI() {
    ui->stackedWidget->setCurrentIndex(0);
}

QString drawRecord(int owner, int start, int length, int interval, int cursorPos) {
    QString drawnRec;
    suggestbutton_t button;

    QString cursor = "<b style=\"background-color: " + accentColor + "\">%1</b>";
    QString iconStart = "<img src=\"%1\" width=24>";

    e_suggestvariant_t &variant = Records[CurrentRecord].variants[MentionedVariant];
    for (int dot = start; dot < start + length; dot += interval) {
        QString drawnIcon = "";

        for(int subcount = 0; subcount < interval; subcount++) {
            if(!variant.getButFromSubdot(owner, dot + subcount, button)) continue;
            drawnIcon = iconStart.arg(buttonsUIRes[button.buttonid]);
        }

        if(drawnIcon == "") {
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
    for(e_suggestline_t line : Records[CurrentRecord].variants[MentionedVariant].lines) {
        QTableWidgetItem *coolTres = new QTableWidgetItem(QString::number((int32_t)line.coolmodethreshold));
        QTableWidgetItem *ownerNam = new QTableWidgetItem(getOwnerName(line.owner));
        QTableWidgetItem *subtitle = new QTableWidgetItem("Placeholder");
        ui->lineOptions->setItem(count,0,coolTres);
        ui->lineOptions->setItem(count,1,ownerNam);
        ui->lineOptions->setItem(count,2,subtitle);
        count++;
    }
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
    drawCommands();
    QString textGUI = Records[CurrentRecord].variants[CurrentVariant].islinked ? QString("Linked to %1<br>").arg(Records[CurrentRecord].variants[CurrentVariant].linknum) : "<br>";
    updateLog();

    for(int curOwner = 0; curOwner < numowners; curOwner++) {
        textGUI += "<h2>" + ownerNames[curOwner+2].name + "</h2>"; // FIXME: Calculate properly
        textGUI += "" + drawRecord(owners[curOwner], 0, Records[CurrentRecord].lengthinsubdots, 24, curOwner == cursorowner ? cursorpos : -1) + "<br>";
    }
    if(centerEditorLines) textGUI = QString("<center>%1</center><br>").arg(textGUI);

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
    ui->comTable->setRowCount(ModeCommands[currentCommandSelect].size());
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
    int currentCommandSelect = ui->comSelector->currentIndex();
    int currentColumn = ui->comTable->currentColumn();
    int currentRow = ui->comTable->currentRow();

    GUICommand guiCom;
    guiCom.commandType = ui->comTable->item(currentRow, 0)->text();
    guiCom.arg1 = ui->comTable->item(currentRow, 1)->text();
    guiCom.arg2 = ui->comTable->item(currentRow, 2)->text();
    guiCom.arg3 = ui->comTable->item(currentRow, 3)->text();
    guiCom.arg4 = ui->comTable->item(currentRow, 4)->text();

    commandbuffer_t newCommand = intcommand::ConvertToNormal(guiCom);

    switch(currentColumn) {
    case 0:
        ModeCommands[currentCommandSelect][currentRow].cmd_id = newCommand.cmd_id;
        break;
    case 1:
        ModeCommands[currentCommandSelect][currentRow].arg1 = newCommand.arg1;
        break;
    case 2:
        ModeCommands[currentCommandSelect][currentRow].arg1 = newCommand.arg2;
        break;
    case 3:
        ModeCommands[currentCommandSelect][currentRow].arg1 = newCommand.arg3;
        break;
    case 4:
        ModeCommands[currentCommandSelect][currentRow].arg1 = newCommand.arg4;
        break;
    }
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

void neobes::ALinkVariant(bool linkAll)
{
    int linkId = QInputDialog::getInt(this, tr("Link Variant"), tr("Type the variant to link to"), 0, 0, Records.size());
    if(linkId == -1) return; // User Cancel

    if(linkAll) {
        for(int i = 0; i < 17; i++) {
            if(i != CurrentVariant) Records[CurrentRecord].variants[i].setLink(linkId);
        }
    } else {
        if(linkId == CurrentVariant) {
            linkId = -1; // Unlink
        }
        else if(Records[CurrentRecord].variants[linkId].islinked) {
            QMessageBox::warning(this, "Link Variant", "You can't link a linked variant!");
            neodata::Log("Can't link linked variant!");
            updateLog();
        }

        Records[CurrentRecord].variants[MentionedVariant].setLink(linkId);
    }

    drawEditorGUI();
}

void neobes::ASetSoundboard()
{
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
    audio->playVariant(Records[CurrentRecord].variants[MentionedVariant], StageInfo.bpm, ticker);
}
