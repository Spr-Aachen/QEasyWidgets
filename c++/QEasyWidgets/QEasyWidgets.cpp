#include "QEasyWidgets.h"

#include <QApplication>


QEasyWidgets::QEasyWidgets()
    : QObject(nullptr)
    , m_theme(QEW::LIGHT)
    , m_themeColor(120, 180, 240, 123)
{
    m_config.reset(new QEW::QConfig(this));
    m_theme = m_config->theme();
    m_themeColor = m_config->themeColor();

    // Connect config signals
    connect(m_config.data(), &QEW::QConfig::themeChanged,
            this, &QEasyWidgets::themeChanged);
    connect(m_config.data(), &QEW::QConfig::themeColorChanged,
            this, &QEasyWidgets::themeColorChanged);
}

void QEasyWidgets::setConfigPath(const QString &path)
{
    m_config.reset(new QEW::QConfig(path, this));
    m_theme = m_config->theme();
    m_themeColor = m_config->themeColor();

    // Reconnect signals
    connect(m_config.data(), &QEW::QConfig::themeChanged,
            this, &QEasyWidgets::themeChanged);
    connect(m_config.data(), &QEW::QConfig::themeColorChanged,
            this, &QEasyWidgets::themeColorChanged);
}

QEW::Theme QEasyWidgets::theme() const
{
    return m_theme;
}

QString QEasyWidgets::themeName() const
{
    return QEW::ThemeString(m_theme);
}

void QEasyWidgets::setTheme(const QEW::Theme &theme)
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
    return m_theme == QEW::DARK;
}

QEW::QConfig *QEasyWidgets::config() const
{
    return m_config.data();
}