#ifndef QEASYWIDGETS_THEME_H
#define QEASYWIDGETS_THEME_H

#include <QObject>
#include <QColor>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>


namespace QEW {

// Forward declarations
class BackgroundColorAnimationBase;

/**
 * @brief Theme enumeration
 */
enum Theme {
    LIGHT,
    DARK,
    AUTO
};

/**
 * @brief Convert theme to string
 */
inline QString ThemeString(Theme theme) {
    switch (theme) {
        case LIGHT: return "light";
        case DARK: return "dark";
        case AUTO: return "auto";
        default: return "light";
    }
}

/**
 * @brief Theme color enumeration
 */
enum class ThemeColor {
    Default,
    Light,
    Dark
};

/**
 * @brief Get color for theme color type
 */
inline QColor getThemeColor(ThemeColor type) {
    switch (type) {
        case ThemeColor::Light:
            return QColor(246, 246, 246);
        case ThemeColor::Dark:
            return QColor(24, 24, 24);
        case ThemeColor::Default:
        default:
            return QColor(120, 180, 240, 123);
    }
}

/**
 * @brief Detect system theme
 */
Theme detectSystemTheme();

/**
 * @brief Get current theme
 */
Theme currentTheme();

/**
 * @brief Set current theme
 */
void setCurrentTheme(Theme theme);

/**
 * @brief Check if current theme is dark
 */
bool isDarkTheme();

/**
 * @brief Get current color based on theme
 */
QColor currentColor();

/**
 * @brief Background color animation base class
 */
class BackgroundColorAnimationBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit BackgroundColorAnimationBase(QWidget *widget = nullptr);
    ~BackgroundColorAnimationBase() override = default;

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    void setCustomBackgroundColor(const QColor &lightColor, const QColor &darkColor);
    void updateBackgroundColor();

protected:
    virtual QColor normalBackgroundColor() const;
    virtual QColor hoverBackgroundColor() const;
    virtual QColor pressedBackgroundColor() const;
    virtual QColor focusInBackgroundColor() const;
    virtual QColor disabledBackgroundColor() const;

    QWidget *m_widget;
    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;
    QPropertyAnimation *m_bgColorAnim;
    bool m_isHover;
    bool m_isPressed;
};

} // namespace QEW

#endif // QEASYWIDGETS_THEME_H