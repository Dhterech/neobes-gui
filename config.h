#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

#include <QString>
#include <QObject>
#include <QSettings>

struct FileHistory {
    QString fileName;
    QString prettyFileName;
    QString stage;
    QString fileDate;
    QString lastUsed;

    // If a file is already in the list
    bool operator==(const FileHistory& other) const {
        return fileName == other.fileName;
    }
};

class SettingsManager : public QObject {
    Q_OBJECT
public:
    static SettingsManager& instance() {
        static SettingsManager _instance;
        return _instance;
    }

    void save() { m_settings->sync(); }

    /* HUD */
    void setHudCenterVisual(bool centerVisual) { m_settings->setValue("hudCenterVisual", centerVisual); }
    void setHudFirstRecVarOnLoad(bool firstRecVarReset) { m_settings->setValue("hudFirstRecVarOnLoad", firstRecVarReset); }
    void setHudEscapeLines(bool escapeLines) { m_settings->setValue("hudEscapeLines", escapeLines); }
    void setHudIconSize(int iconSize) { m_settings->setValue("hudIconSize", iconSize); }

    bool hudCenterVisual() const { return m_settings->value("hudCenterVisual", true).toBool(); }
    bool hudFirstRecVarOnLoad() const { return m_settings->value("hudFirstRecVarOnLoad", true).toBool(); }
    bool hudEscapeLines() const { return m_settings->value("hudEscapeLines", true).toBool(); }
    int  hudIconSize() const { return m_settings->value("hudIconSize", 24).toInt(); }

    /* Behaviour */
    void setBhvKeepSubtitles(bool keepSubs) { m_settings->setValue("bhvKeepSubtitles", keepSubs); }
    void setBhvDebugJP(bool forceRegion) { m_settings->setValue("bhvJul12Region", forceRegion); }

    bool bhvKeepSubtitles() const { return m_settings->value("bhvKeepSubtitles", false).toBool(); }
    bool bhvDebugJP() const { return m_settings->value("bhvJul12Region", false).toBool(); }

    /* File History */
    QList<FileHistory> getHistory() {
        QList<FileHistory> list;
        int size = m_recFiles->beginReadArray("recentFiles");
        for (int i = 0; i < size; ++i) {
            m_recFiles->setArrayIndex(i);
            list.append({
                m_recFiles->value("fileName").toString(),
                m_recFiles->value("prettyFileName").toString(),
                m_recFiles->value("stage").toString(),
                m_recFiles->value("fileDate").toString(),
                m_recFiles->value("lastUsed").toString()
            });
        }
        m_recFiles->endArray();
        return list;
    }

    void addFileToHistory(FileHistory item) {
        QList<FileHistory> list = getHistory();

        list.removeAll(item);
        list.prepend(item);

        while (list.size() > 10) {
            list.removeLast();
        }

        m_recFiles->beginWriteArray("recentFiles");
        for (int i = 0; i < list.size(); ++i) {
            m_recFiles->setArrayIndex(i);
            m_recFiles->setValue("fileName", list[i].fileName);
            m_recFiles->setValue("prettyFileName", list[i].prettyFileName);
            m_recFiles->setValue("stage", list[i].stage);
            m_recFiles->setValue("fileDate", list[i].fileDate);
            m_recFiles->setValue("lastUsed", list[i].lastUsed);
        }
        m_recFiles->endArray();

        emit historyChanged();
    }

    void clearHistory() {
        m_settings->remove("recentFiles");

        emit historyChanged();
    }

signals:
    void historyChanged();

private:
    explicit SettingsManager() {
        m_settings = new QSettings("NeoBES", "config");
        m_recFiles = new QSettings("NeoBES", "history");
    }
    QSettings *m_settings;
    QSettings *m_recFiles;
};

#endif // CONFIG_H
