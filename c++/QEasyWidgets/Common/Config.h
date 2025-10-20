#ifndef QEASYWIDGETS_CONFIG_H
#define QEASYWIDGETS_CONFIG_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QString>

#include "Theme.h"


namespace QEW {

/**
 * @brief Status enumeration
 */
class Status {
public:
    static const QString Loading;
};

/**
 * @brief Chat role enumeration
 */
class ChatRole {
public:
    static const QString User;
    static const QString Contact;
};

/**
 * @brief Configuration manager for QEasyWidgets
 */
class QConfig : public QObject
{
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

} // namespace QEW

#endif // QEASYWIDGETS_CONFIG_H