#include "neobes.h"
#include "pcsx2reader.h"
#include "pcsx2util.h"
#include "config.h"
#include "neodata.h"
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

QString ownerNames[] = {
    "None",
    "???",
    "Teacher",
    "PaRappa",
    "SFX",
    "Scene"
};

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
bool hasEdited;

suggestbutton_t copiedButton;

neobes::neobes(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::neobes)
{
    ui->setupUi(this);
    pcsx2reader::SetupIPC();
    neodata::Log("NeoBESMS Launched");

    updateLog();
    drawMenuGUI();
    this->setFocusPolicy(Qt::StrongFocus); // may help may not

    ui->variantInput->setMaximum(16);
    //for(int i = 0; i < 4; i++) ui->comTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);

    connect(ui->selregBtn, &QPushButton::clicked, this, &neobes::ASelectRegion);

    connect(ui->actionLoad, &QAction::triggered, this, &neobes::ALoadProject);
    connect(ui->actionSave, &QAction::triggered, this, &neobes::ASaveProject);

    connect(ui->actionUploadOLM, &QAction::triggered, this, &neobes::AUploadOLM);
    connect(ui->actionDownloadOLM, &QAction::triggered, this, &neobes::ADownloadOLM);

    connect(ui->actionDownloadEmu, &QAction::triggered, this, &neobes::ADownloadEmu);

    connect(ui->actionLink, &QAction::triggered, this, [=, this]() {neobes::ALinkVariant(false);});
    connect(ui->actionLink_all_variants, &QAction::triggered, this, [=, this]() {neobes::ALinkVariant(true);});
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

    switch(neodata::SaveToEmu()) {
    case 0:
        updateLog();
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
    int size = QInputDialog::getInt(this, tr("Type the OLM file size"), tr("Type file size"), 0, 0, 50000); // FIXME: IPC LIMIT Size
    if(size == -1) return; // User Cancel

    neodata::DownloadOLMFromEmu(fileName, size);
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
    CurrentRegion = QInputDialog::getInt(this, tr("Region select"), tr("Type the region (0 = NTSC-U, 1 = PAL, 2 = NTSC-J"), CurrentRegion, 0, 2);
}

/* GUI Core Methods */

void neobes::drawMenuGUI() {
    ui->stackedWidget->setCurrentIndex(0);
}

QString drawRecord(int owner, int start, int length, int interval, int cursorPos) {
    QString drawnRec;
    suggestbutton_t button;

    QString cursor = "<b style=\"background-color: blue\">%1</b>";
    QString iconStart = "<img src=\"%1\" width=24>";

    e_suggestvariant_t &variant = Records[CurrentRecord].variants[CurrentVariant];
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
    if(Records[CurrentRecord].variants[CurrentVariant].getButFromSubdot(owners[cursorowner], (cursorpos * 24) + precpos, button)) {
        ui->tableWidget->setEnabled(true);

        for(int i = 0; i < 4; i++) {
            QTableWidgetItem *sdid = new QTableWidgetItem(QString::number(button.sounds[i].soundid));
            QTableWidgetItem *anim = new QTableWidgetItem(QString::number(button.sounds[i].animationid));
            QTableWidgetItem *time = new QTableWidgetItem(QString::number(button.sounds[i].relativetime));

            ui->tableWidget->setItem(i,0,sdid);
            ui->tableWidget->setItem(i,1,anim);
            ui->tableWidget->setItem(i,2,time);
        }
    } else {
        ui->tableWidget->setEnabled(false);
    }

}

void neobes::drawInfo() {
    int usedSize = neodata::CalcAvailableStorage();
    int avaiSize = StageInfo.buttondataend - StageInfo.buttondatabase + 1;

    ui->infoSize->setText(QString::number(usedSize) + "/" + QString::number(avaiSize));

    ui->sizeProgressBar->setRange(0, avaiSize);
    ui->sizeProgressBar->setValue(usedSize);
    QPalette palette = ui->sizeProgressBar->palette();
    palette.setColor(QPalette::Highlight, QColor(avaiSize < usedSize ? "#AA0000" : "#00AA00"));
    ui->sizeProgressBar->setPalette(palette);

    ui->variantInput->setValue(CurrentVariant);
    ui->recordInput->setValue(CurrentRecord);
    ui->recordInput->setMaximum(Records.size());
}

void neobes::drawEditorGUI() {
    ui->stackedWidget->setCurrentIndex(1);
    QString textGUI = "<br>";
    bool once = false;
    updateLog();

    for(int curOwner = 0; curOwner < numowners; curOwner++) {
        textGUI += "<h2>" + ownerNames[curOwner+2] + "</h2>"; // FIXME: Calculate properly
        textGUI += "<br>" + drawRecord(owners[curOwner], 0, Records[CurrentRecord].lengthinsubdots, 24, curOwner == cursorowner ? cursorpos : -1) + "<br>";
    }
    if(centerEditorLines) textGUI = QString("<center>%1</center><br>").arg(textGUI);

    textGUI += "<h3>Precision</h3><br>" + drawRecord(owners[cursorowner], cursorpos * 24, 24, 1, cursorpos * 24 + precpos);

    drawInfo();
    if(!once) { drawCommands(); once = true;}
    drawButtonProperties();
    ui->textBrowser->setText(textGUI);
    isMenu = false;
}

#include "intcommand.h"
int currentCommandSelect = 1;
void neobes::drawCommands() {
    int curRow = 0;

    for(commandbuffer_t &command : ModeCommands[currentCommandSelect]) {
        GUICommand gui = intcommand::ConvertToGUI(command);
        QTableWidgetItem *ctyp = new QTableWidgetItem(gui.commandType);
        QTableWidgetItem *arg1 = new QTableWidgetItem(gui.arg1);
        QTableWidgetItem *arg2 = new QTableWidgetItem(gui.arg2);
        QTableWidgetItem *arg3 = new QTableWidgetItem(gui.arg3);
        QTableWidgetItem *arg4 = new QTableWidgetItem(gui.arg4);
        ui->comTable->insertRow(curRow);
        ui->comTable->setItem(curRow, 0, ctyp);
        ui->comTable->setItem(curRow, 1, arg1);
        ui->comTable->setItem(curRow, 2, arg2);
        ui->comTable->setItem(curRow, 3, arg3);
        ui->comTable->setItem(curRow, 4, arg4);
        curRow++;
    }
}

/* GUI Functions */

void neobes::handleArrowKeys(bool lr, int inc) {
    if(lr && CurrentRecord+inc >= 0 && CurrentRecord+inc < Records.size()) CurrentRecord += inc;
    else if(!lr && CurrentVariant+inc >= 0 && CurrentVariant+inc < 17) CurrentVariant += inc;
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
    else Records[CurrentRecord].variants[CurrentVariant].createButton((cursorpos * 24) + precpos, owners[cursorowner], buttonId);
}

// TODO: Expose variables for managing without cursor?
void neobes::AButtonCopy() {
    suggestbutton_t button; // TODO: Test done without copying it
    if(Records[CurrentRecord].variants[CurrentVariant].getButFromSubdot(owners[cursorowner], cursorpos + precpos, button)) copiedButton = button;
}

void neobes::AButtonCut() { // TODO: #define?
    AButtonCopy();
    AButtonDelete();
}

void neobes::AButtonDelete() {
    Records[CurrentRecord].variants[CurrentVariant].deleteButton((cursorpos * 24) + precpos, owners[cursorowner]);
}

void neobes::AButtonPaste() { // TODO: Check better solution for Paste
    suggestbutton_t *pbutton;
    AButtonDelete();
    Records[CurrentRecord].variants[CurrentVariant].createButton((cursorpos * 24) + precpos, owners[cursorowner], copiedButton.buttonid);

    if(Records[CurrentRecord].variants[CurrentVariant].getButRefFromSubdot(owners[cursorowner], cursorpos + precpos, &pbutton)) { // FIXME: Redundant
        for(int s = 0; s < 4; s++) pbutton->sounds[s] = copiedButton.sounds[s];
    }
}

void neobes::ALinkVariant(bool linkAll)
{
    int linkId = QInputDialog::getInt(this, tr("Type the record to link to"), tr("Type the record to link to"), 0, 0, Records.size());
    if(linkId == -1) return; // User Cancel

    if(linkId == CurrentVariant) linkId = -1; // Unlink
    else if(Records[CurrentRecord].variants[linkId].islinked) {
        QMessageBox::warning(this, "Error on link", "You can't link a linked variant");
        neodata::Log("Can't link linked variant!");
        updateLog();
    }

    if(linkAll) {
        for(e_suggestvariant_t &variant : Records[CurrentRecord].variants) {
            variant.setLink(linkId);
        }
    } else {
        Records[CurrentRecord].variants[CurrentVariant].setLink(linkId);
    }
}
