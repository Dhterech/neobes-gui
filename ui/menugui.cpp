#include "menugui.h"
#include "ui_menugui.h"
#include "config.h"

#include <QPushButton>

menugui::menugui(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::menugui)
{
    ui->setupUi(this);

    // Connect buttons

    connect(ui->loadbesBtn, &QPushButton::clicked, this, &menugui::ALoadProject);
    connect(ui->loademuBtn, &QPushButton::clicked, this, &menugui::ADownloadEmu);
    connect(ui->settingsBtn, &QPushButton::clicked, this, &menugui::ASettingsGUI);
    connect(ui->infobackBtn, &QPushButton::clicked, this, &menugui::AAboutGUI);

    // Load Recent Files
    connect(ui->recentFilesView, &QTreeView::doubleClicked, this, &menugui::onTreeViewClicked);

    model = new QStandardItemModel(0, 4, ui->recentFilesView);
    setupRecentFiles();
}

menugui::~menugui()
{
    delete ui;
}

#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QHeaderView>

void menugui::setupRecentFiles() {
    model->clear();
    model->setHorizontalHeaderLabels({"Filename", "Stage", "Created", "Last Used"});
    QFileIconProvider iconProvider;

    for (const FileHistory &entry : SettingsManager::instance().getHistory()) {
        QList<QStandardItem*> row;

        QIcon fileIcon = iconProvider.icon(QFileInfo(entry.fileName));
        QStandardItem* nameItem = new QStandardItem(fileIcon, entry.prettyFileName);
        QStandardItem* stageItem = new QStandardItem(entry.stage);
        QStandardItem* createdItem = new QStandardItem(entry.fileDate);
        QStandardItem* usedItem = new QStandardItem(entry.lastUsed);

        row << nameItem << stageItem << createdItem << usedItem;
        model->appendRow(row);
    }

    ui->recentFilesView->setModel(model);

    ui->recentFilesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->recentFilesView->setAlternatingRowColors(true);
    ui->recentFilesView->setRootIsDecorated(false);

    ui->recentFilesView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void menugui::onTreeViewClicked(const QModelIndex &index) {
    if (!index.isValid()) return;

    int aIndex = index.row();
    QString absolutePath = SettingsManager::instance().getHistory().at(aIndex).fileName;

    if (!absolutePath.isEmpty()) {
        emit loadFileFromRecents(absolutePath);
    }
}
