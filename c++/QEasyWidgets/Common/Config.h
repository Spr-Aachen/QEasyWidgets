#ifndef QEASYWIDGETS_CONFIG_H
#define QEASYWIDGETS_CONFIG_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QString>

#include "Theme.h"


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
 * Configuration manager for QEasyWidgets
 */
class QConfig : public QObject {
    Q_OBJECT

public:
    explicit QConfig(QObject *parent = nullptr);
    explicit QConfig(const QString &configPath, QObject *parent = nullptr);
    ~QConfig() override = default;

    // Theme management
    Theme theme() const;
    void setTheme(Theme theme);

    // Theme color management
    QColor themeColor() const;
    void setThemeColor(const QColor &color);

    // Save configuration
    void save();

    // Load configuration
    void load();

signals:
    void themeChanged(Theme theme);
    void themeColorChanged(const QColor &color);

private:
    void initConfig();

    QSettings *m_settings;
    Theme m_theme;
    QColor m_themeColor;
    QString m_configPath;
};


#endif // QEASYWIDGETS_CONFIG_H