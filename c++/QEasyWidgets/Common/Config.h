#ifndef QEASYWIDGETS_CONFIG_H
#define QEASYWIDGETS_CONFIG_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QString>

#include "Theme.h"


/**
 * Position enumeration
 */
enum class Position {
    Top = 0,
    Bottom = 1,
    Left = 2,
    Right = 3,
    TopLeft = 4,
    TopRight = 5,
    BottomLeft = 6,
    BottomRight = 7
};


/**
 * Status enumeration
 */
enum class Status {
    Loading = 0
};


/**
 * Chat role enumeration
 */
enum class ChatRole {
    User = 0,
    Contact = 1
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