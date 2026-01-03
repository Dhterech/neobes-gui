#ifndef MENUGUI_H
#define MENUGUI_H

#include <QMainWindow>
#include <QWidget>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class menugui; }
QT_END_NAMESPACE

class menugui : public QWidget
{
    Q_OBJECT

public:
    explicit menugui(QWidget *parent = nullptr);
    ~menugui();

public slots:
    void setupRecentFiles();

signals:
    // buttons
    void ADownloadEmu();
    void ALoadProject();
    void ASettingsGUI();
    void AAboutGUI();
    // loadsig
    void loadFileFromRecents(const QString &filePath);

private:
    Ui::menugui *ui;
    void onTreeViewClicked(const QModelIndex &index);
    QStandardItemModel* model;
};

#endif // MENUGUI_H
