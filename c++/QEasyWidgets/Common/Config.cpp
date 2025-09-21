#include "Config.h"

#include <QStandardPaths>
#include <QDir>


// Status class static members
const QString Status::Loading = "Loading";


// ChatRole class static members
const QString ChatRole::User = "User";
const QString ChatRole::Contact = "Contact";


QConfig::QConfig(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
    , m_theme(LIGHT)
    , m_themeColor(120, 180, 240, 123) {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation); // Default config path
    QDir().mkpath(configDir);
    m_configPath = configDir + "/qeasywidgets.ini";
    
    initConfig();
}


QConfig::QConfig(const QString &configPath, QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
    , m_theme(LIGHT)
    , m_themeColor(120, 180, 240, 123)
    , m_configPath(configPath) {
    initConfig();
}


void QConfig::initConfig() {
    if (m_settings) {
        delete m_settings;
    }
    
    m_settings = new QSettings(m_configPath, QSettings::IniFormat, this);
    load();
}


Theme QConfig::theme() const {
    return m_theme;
}


void QConfig::setTheme(Theme theme) {
    if (m_theme != theme) {
        m_theme = theme;
        save();
        emit themeChanged(theme);
    }
}


QColor QConfig::themeColor() const {
    return m_themeColor;
}


void QConfig::setThemeColor(const QColor &color) {
    if (m_themeColor != color) {
        m_themeColor = color;
        save();
        emit themeColorChanged(color);
    }
}


void QConfig::save() {
    if (!m_settings) {
        return;
    }
    
    m_settings->beginGroup("Theme");
    m_settings->setValue("theme", static_cast<int>(m_theme));
    m_settings->setValue("themeColor", m_themeColor.name(QColor::HexArgb));
    m_settings->endGroup();
    
    m_settings->sync();
}


void QConfig::load() {
    if (!m_settings) {
        return;
    }
    
    m_settings->beginGroup("Theme");
    m_theme = static_cast<Theme>(m_settings->value("theme", static_cast<int>(LIGHT)).toInt());
    m_themeColor = QColor(m_settings->value("themeColor", "#7878F07B").toString());
    m_settings->endGroup();
}