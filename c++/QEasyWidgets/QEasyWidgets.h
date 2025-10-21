#ifndef QEASYWIDGETS_H
#define QEASYWIDGETS_H

#include "Common/Theme.h"
#include "Common/Config.h"
#include "Common/StyleSheet.h"

#include <QObject>
#include <QColor>
#include <QScopedPointer>


/**
 * @brief QEasyWidgets library manager, singleton
 * Manages theme, configuration, and global settings
 */
class QEasyWidgets : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(QEasyWidgets)
    QEasyWidgets(QEasyWidgets &&) = delete;
    QEasyWidgets &operator=(QEasyWidgets &&) = delete;

public:
    static QEasyWidgets &instance()
    {
        static QEasyWidgets single;
        return single;
    }

    /**
     * @brief Set custom configuration file path
     */
    void setConfigPath(const QString &path);

    /************************** Theme Management **************************/

    /**
     * @brief Get current theme
     */
    Theme theme() const;
    
    /**
     * @brief Get theme name as string
     */
    QString themeName() const;
    
    /**
     * @brief Set theme
     */
    void setTheme(const Theme &theme);
    
    /**
     * @brief Get theme color
     */
    QColor themeColor() const;
    
    /**
     * @brief Set theme color
     */
    void setThemeColor(const QColor &themeColor);
    
    /**
     * @brief Check if current theme is dark
     */
    bool isDarkTheme() const;

    /************************** Configuration **************************/

    /**
     * @brief Get configuration object
     */
    QConfig *config() const;

signals:
    /**
     * @brief Emitted when application needs to restart
     */
    void appRestartRequested();
    
    /**
     * @brief Emitted when theme changes
     */
    void themeChanged(Theme theme);
    
    /**
     * @brief Emitted when theme color changes
     */
    void themeColorChanged(const QColor &color);

private:
    QEasyWidgets();
    ~QEasyWidgets() override = default;

    Theme m_theme;
    QColor m_themeColor;
    QScopedPointer<QConfig> m_config;
};

// Convenience macro for accessing singleton
#define QEWIns (QEasyWidgets::instance())

#endif // QEASYWIDGETS_H