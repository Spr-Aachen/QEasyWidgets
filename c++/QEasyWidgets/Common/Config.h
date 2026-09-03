#ifndef QEASYWIDGETS_CONFIG_H
#define QEASYWIDGETS_CONFIG_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>


/**
 * Direction enumeration
 */
enum class Direction {
    Up = 1,
    Down = 2
};


/**
 * Position enumeration
 */
enum class Position {
    Top = 1,
    Bottom = 2,
    Left = 3,
    Right = 4,
    TopLeft = 5,
    TopRight = 6,
    BottomLeft = 7,
    BottomRight = 8
};


/**
 * Status enumeration
 */
enum class Status {
    Loading = 1
};


/**
 * Chat role enumeration
 */
enum class ChatRole {
    User = 1,
    Contact = 2
};


/**
 * Theme enumeration
 */
enum class Theme {
    Light,
    Dark,
    Auto
};


/**
 * Configuration item
 */
class ConfigItem : public QObject {
    Q_OBJECT

public:
    ConfigItem(const QString &group, const QString &name, const QVariant &defaultValue, QObject *parent = nullptr)
        : QObject(parent), m_group(group), m_name(name), m_value(defaultValue), m_defaultValue(defaultValue) {}

    QString key() const { return m_name.isEmpty() ? m_group : m_group + "." + m_name; }
    QString group() const { return m_group; }
    QString name() const { return m_name; }
    QVariant value() const { return m_value; }
    void setValue(const QVariant &value) {
        if (m_value != value) {
            m_value = value;
            emit valueChanged(value);
        }
    }

signals:
    void valueChanged(const QVariant &value);

private:
    QString m_group;
    QString m_name;
    QVariant m_value;
    QVariant m_defaultValue;
};


/**
 * Configuration manager for QEasyWidgets
 */
class QConfig : public QObject {
    Q_OBJECT

public:
    explicit QConfig(QObject *parent = nullptr);
    ~QConfig() override = default;

    // Theme management
    Theme theme() const;
    Theme themeMode() const { return m_themeMode; }
    void setTheme(Theme theme);

    // Save configuration
    void save();

    // Load configuration
    void load();

signals:
    void themeChanged(Theme theme);

private:
    QString m_configPath;
    Theme m_themeMode;
    
public:
    ConfigItem *themeItem;
};

extern QConfig *qconfig;

#endif // QEASYWIDGETS_CONFIG_H