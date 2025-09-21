#include "Theme.h"
#include "Signals.h"

#include <QApplication>
#include <QWidget>
#include <QPalette>
#include <QSettings>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


// Detect system theme based on OS
Theme detectSystemTheme() {
    #ifdef Q_OS_WIN
        // Windows 10/11 theme detection via registry
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
        int appsUseLightTheme = settings.value("AppsUseLightTheme", 1).toInt();
        return (appsUseLightTheme == 0) ? DARK : LIGHT;
    #elif defined(Q_OS_MACOS)
        // macOS theme detection would go here
        // For now, use palette-based detection
        QPalette palette = QApplication::palette();
        return (palette.color(QPalette::Window).lightness() < 128) ? DARK : LIGHT;
    #else
        // Linux and other platforms - use palette-based detection
        if (QApplication::instance()) {
            QPalette palette = QApplication::palette();
            return (palette.color(QPalette::Window).lightness() < 128) ? DARK : LIGHT;
        }
        return LIGHT;
    #endif
}


// Static variable to store current theme
static Theme g_currentTheme = detectSystemTheme();


Theme currentTheme() {
    return g_currentTheme;
}


void setCurrentTheme(Theme theme) {
    if (g_currentTheme != theme) {
        g_currentTheme = theme;
        // Emit global theme change signal
        if (componentsSignals) {
            emit componentsSignals->setTheme(ThemeString(theme));
        }
    }
}


bool isDarkTheme() {
    return g_currentTheme == DARK;
}


QColor currentColor() {
    return isDarkTheme() ? getThemeColor(ThemeColor::Dark) : getThemeColor(ThemeColor::Light);
}


BackgroundColorAnimationBase::BackgroundColorAnimationBase(QWidget *widget)
    : QObject(widget)
    , m_widget(widget)
    , m_lightBackgroundColor(getThemeColor(ThemeColor::Light))
    , m_darkBackgroundColor(getThemeColor(ThemeColor::Dark))
    , m_bgColorAnim(nullptr)
    , m_isHover(false)
    , m_isPressed(false) {
    if (m_widget) {
        // Create property animation for background color
        m_bgColorAnim = new QPropertyAnimation(this, "backgroundColor", this);
        m_bgColorAnim->setDuration(210);

        // Connect to global theme change signal
        if (componentsSignals) {
            connect(componentsSignals, &CustomSignals::setTheme, this, [this](const QString &) {
                updateBackgroundColor();
            });
        }
    }
}


QColor BackgroundColorAnimationBase::backgroundColor() const {
    if (m_widget) {
        // Get current background color from widget
        // This is a simplified implementation
        return m_widget->palette().color(QPalette::Window);
    }
    return QColor();
}


void BackgroundColorAnimationBase::setBackgroundColor(const QColor &color) {
    if (m_widget) {
        QPalette palette = m_widget->palette();
        palette.setColor(QPalette::Window, color);
        m_widget->setPalette(palette);
        m_widget->update();
    }
}


void BackgroundColorAnimationBase::setCustomBackgroundColor(const QColor &lightColor, const QColor &darkColor) {
    m_lightBackgroundColor = lightColor;
    m_darkBackgroundColor = darkColor;
    updateBackgroundColor();
}


void BackgroundColorAnimationBase::updateBackgroundColor() {
    if (!m_widget) return;

    QColor targetColor;
    if (!m_widget->isEnabled()) {
        targetColor = disabledBackgroundColor();
    } else if (m_widget->hasFocus()) {
        targetColor = focusInBackgroundColor();
    } else if (m_isPressed) {
        targetColor = pressedBackgroundColor();
    } else if (m_isHover) {
        targetColor = hoverBackgroundColor();
    } else {
        targetColor = normalBackgroundColor();
    }

    if (m_bgColorAnim) {
        m_bgColorAnim->stop();
        m_bgColorAnim->setEndValue(targetColor);
        m_bgColorAnim->start();
    } else {
        setBackgroundColor(targetColor);
    }
}


QColor BackgroundColorAnimationBase::normalBackgroundColor() const {
    return isDarkTheme() ? m_darkBackgroundColor : m_lightBackgroundColor;
}


QColor BackgroundColorAnimationBase::hoverBackgroundColor() const {
    return normalBackgroundColor();
}


QColor BackgroundColorAnimationBase::pressedBackgroundColor() const {
    return normalBackgroundColor();
}


QColor BackgroundColorAnimationBase::focusInBackgroundColor() const {
    return normalBackgroundColor();
}


QColor BackgroundColorAnimationBase::disabledBackgroundColor() const {
    return normalBackgroundColor();
}