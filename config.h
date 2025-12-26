#ifndef BES_CONFIG_H
#define BES_CONFIG_H

#include "types.h"

#include <QString>
#include <QObject>
#include <QSettings>

struct FileHistory {
    QString fileName;
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

    bool bhvKeepSubtitles() const { return m_settings->value("bhvKeepSubtitles", false).toBool(); }

    /* File History */
    QList<FileHistory> getHistory() {
        QList<FileHistory> list;
        int size = m_settings->beginReadArray("recentFiles");
        for (int i = 0; i < size; ++i) {
            m_settings->setArrayIndex(i);
            list.append({
                m_settings->value("fileName").toString(),
                m_settings->value("stage").toString(),
                m_settings->value("fileDate").toString(),
                m_settings->value("lastUsed").toString()
            });
        }
        m_settings->endArray();
        return list;
    }

    void addFileToHistory(FileHistory item) {
        QList<FileHistory> list = getHistory();

        list.removeAll(item);
        list.prepend(item);

        while (list.size() > 10) {
            list.removeLast();
        }

        m_settings->beginWriteArray("recentFiles");
        for (int i = 0; i < list.size(); ++i) {
            m_settings->setArrayIndex(i);
            m_settings->setValue("fileName", list[i].fileName);
            m_settings->setValue("stage", list[i].stage);
            m_settings->setValue("fileDate", list[i].fileDate);
            m_settings->setValue("lastUsed", list[i].lastUsed);
        }
        m_settings->endArray();

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
        m_settings = new QSettings("Zungle", "NeoBES");
    }
    QSettings *m_settings;
};

#endif // BES_CONFIG_H
