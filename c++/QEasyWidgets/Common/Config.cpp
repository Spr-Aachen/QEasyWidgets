#include "Config.h"
#include "Theme.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>


QConfig::QConfig(QObject *parent)
    : QObject(parent), m_themeMode(Theme::Auto) {
    m_configPath = QDir::currentPath() + "/config/config.json";
    
    themeItem = new ConfigItem("General", "ThemeMode", "auto", this);
    
    load();
}

Theme QConfig::theme() const {
    if (m_themeMode == Theme::Auto) {
        return detectSystemTheme();
    }
    return m_themeMode;
}

void QConfig::setTheme(Theme theme) {
    if (m_themeMode != theme) {
        m_themeMode = theme;
        
        QString themeStr;
        if (theme == Theme::Light) themeStr = "light";
        else if (theme == Theme::Dark) themeStr = "dark";
        else themeStr = "auto";
        
        themeItem->setValue(themeStr);
        save();
        emit themeChanged(this->theme());
    }
}

void QConfig::save() {
    QDir().mkpath(QFileInfo(m_configPath).absolutePath());
    
    QJsonObject generalObj;
    QString themeStr;
    if (m_themeMode == Theme::Light) themeStr = "light";
    else if (m_themeMode == Theme::Dark) themeStr = "dark";
    else themeStr = "auto";
    
    generalObj["ThemeMode"] = themeStr;
    
    QJsonObject root;
    root["General"] = generalObj;
    
    QJsonDocument doc(root);
    QFile file(m_configPath);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        file.write(doc.toJson());
        file.close();
    }
}

void QConfig::load() {
    QFile file(m_configPath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull()) {
        QJsonObject root = doc.object();
        QJsonObject general = root["General"].toObject();
        QString themeStr = general["ThemeMode"].toString("auto");
        
        if (themeStr == "light") m_themeMode = Theme::Light;
        else if (themeStr == "dark") m_themeMode = Theme::Dark;
        else m_themeMode = Theme::Auto;
        
        themeItem->setValue(themeStr);
    }
}

QConfig *qconfig = new QConfig();