#ifndef QEASYWIDGETS_BUTTON_H
#define QEASYWIDGETS_BUTTON_H

#include <QPushButton>
#include <QAbstractButton>
#include <QIcon>
#include <QColor>
#include <QPropertyAnimation>
#include <QFileDialog>
#include <QMenu>
#include <functional>

#include "../Common/Icon.h"
#include "../Common/Theme.h"


/**
 * Base button class with theme support
 */
class ButtonBase : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit ButtonBase(QWidget *parent = nullptr);
    explicit ButtonBase(const QString &text, QWidget *parent = nullptr);
    explicit ButtonBase(const QString &text, const QIcon &icon, QWidget *parent = nullptr);
    ~ButtonBase() override = default;

    // Spacing between icon and text
    void setSpacing(int spacing);
    int spacing() const;

    // Icon management
    void setIcon(const QIcon &icon);
    void setIcon(IconBase icon);
    QIcon icon() const;

    // Alignment
    void setAlignment(Qt::Alignment alignment);
    Qt::Alignment alignment() const;

    // Background color
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);
    
    // Custom background colors for light/dark themes
    void setCustomBackgroundColor(const QColor &light, const QColor &dark);
    
    // Property methods
    void setBorderless(bool borderless);
    void setTransparent(bool transparent);
    void setHoverBackgroundColor(const QColor &color);
    void clearDefaultStyleSheet();

    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    
    virtual QColor normalBackgroundColor() const;
    virtual QColor hoverBackgroundColor() const;
    virtual QColor pressedBackgroundColor() const;
    virtual QColor disabledBackgroundColor() const;
    
    void updateBackgroundColor();
    void _drawIcon(const QIcon &icon, QPainter *painter, const QRect &rect);

private:
    void init();
    
    int m_spacing;
    QIcon m_icon;
    IconBase m_iconBase;
    bool m_hasIconBase;
    Qt::Alignment m_alignment;
    
    QColor m_backgroundColor;
    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;
    QColor m_hoverBackgroundColorOverride;
    bool m_hasHoverColorOverride;
    
    QPropertyAnimation *m_bgColorAnim;
    
    bool m_isHover;
    bool m_isPressed;
};


/**
 * Primary button with accent color
 */
class PrimaryButton : public ButtonBase {
    Q_OBJECT

public:
    explicit PrimaryButton(QWidget *parent = nullptr);
    explicit PrimaryButton(const QString &text, QWidget *parent = nullptr);
    explicit PrimaryButton(const QString &text, const QIcon &icon, QWidget *parent = nullptr);

protected:
    QColor normalBackgroundColor() const override;
    QColor hoverBackgroundColor() const override;
    QColor pressedBackgroundColor() const override;
};


/**
 * Transparent button
 */
class TransparentButton : public ButtonBase {
    Q_OBJECT

public:
    explicit TransparentButton(QWidget *parent = nullptr);
    explicit TransparentButton(const QString &text, QWidget *parent = nullptr);
    explicit TransparentButton(const QString &text, const QIcon &icon, QWidget *parent = nullptr);

protected:
    QColor normalBackgroundColor() const override;
    QColor hoverBackgroundColor() const override;
    QColor pressedBackgroundColor() const override;
};


/**
 * Clear button with X icon
 */
class ClearButton : public ButtonBase {
    Q_OBJECT

public:
    explicit ClearButton(QWidget *parent = nullptr);
    ~ClearButton() override = default;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_isPressed;
};


/**
 * Rotate button with animation
 */
class RotateButton : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(float angle READ angle WRITE setAngle)

public:
    explicit RotateButton(QWidget *parent = nullptr);
    ~RotateButton() override = default;

    float angle() const;
    void setAngle(float angle);

    void setRotate(bool isDown);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void init();

    float m_angle;
    QPropertyAnimation *m_rotateAnimation;
};


/**
 * File button for opening file dialogs
 */
class FileButton : public ButtonBase {
    Q_OBJECT

public:
    explicit FileButton(QWidget *parent = nullptr);
    ~FileButton() override = default;

    void setFileDialog(
        QWidget *parent, QFileDialog::FileMode mode,
        const QString &fileType = QString(),
        const QString &directory = QString(),
        const QString &buttonTooltip = "Browse"
    );

private:
    void init();
};


/**
 * Menu button that shows a dropdown menu
 */
class MenuButton : public ButtonBase {
    Q_OBJECT

public:
    explicit MenuButton(QWidget *parent = nullptr);
    ~MenuButton() override = default;

    void setMenu(QMenu *menu);
    void setMenu(const QMap<QString, std::function<void()>> &actionEvents);

private:
    void init();
    QMenu *m_menu;
};


/**
 * Hollow button (transparent background)
 */
class HollowButton : public ButtonBase {
    Q_OBJECT

public:
    explicit HollowButton(QWidget *parent = nullptr);
    ~HollowButton() override = default;
};


/**
 * Navigation button with exclusive selection
 */
class NavigationButton : public ButtonBase {
    Q_OBJECT

public:
    explicit NavigationButton(QWidget *parent = nullptr);
    ~NavigationButton() override = default;

    void setHorizontal(bool horizontal);

private:
    void init();
};


#endif // QEASYWIDGETS_BUTTON_H