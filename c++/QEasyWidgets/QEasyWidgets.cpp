#include "QEasyWidgets.h"

#include <QApplication>


QEasyWidgets::QEasyWidgets()
    : QObject(nullptr)
    , m_theme(LIGHT)
    , m_themeColor(120, 180, 240, 123)
{
    m_config.reset(new QConfig(this));
    m_theme = m_config->theme();
    m_themeColor = m_config->themeColor();

    // Connect config signals
    connect(m_config.data(), &QConfig::themeChanged,
            this, &QEasyWidgets::themeChanged);
    connect(m_config.data(), &QConfig::themeColorChanged,
            this, &QEasyWidgets::themeColorChanged);
}

void QEasyWidgets::setConfigPath(const QString &path)
{
    m_config.reset(new QConfig(path, this));
    m_theme = m_config->theme();
    m_themeColor = m_config->themeColor();

    // Reconnect signals
    connect(m_config.data(), &QConfig::themeChanged,
            this, &QEasyWidgets::themeChanged);
    connect(m_config.data(), &QConfig::themeColorChanged,
            this, &QEasyWidgets::themeColorChanged);
}

Theme QEasyWidgets::theme() const
{
    return m_theme;
}

QString QEasyWidgets::themeName() const
{
    return ThemeString(m_theme);
}

void QEasyWidgets::setTheme(const Theme &theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        m_config->setTheme(theme);
        emit themeChanged(theme);
    }
}

QColor QEasyWidgets::themeColor() const
{
    return m_themeColor;
}

void QEasyWidgets::setThemeColor(const QColor &themeColor)
{
    if (m_themeColor != themeColor) {
        m_themeColor = themeColor;
        m_config->setThemeColor(themeColor);
        emit themeColorChanged(themeColor);
    }
}

bool QEasyWidgets::isDarkTheme() const
{
    return m_theme == DARK;
}

QConfig *QEasyWidgets::config() const
{
    return m_config.data();
}