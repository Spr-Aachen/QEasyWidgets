#include "Button.h"

#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QFontMetrics>
#include <QApplication>
#include <QStandardPaths>
#include <QLineEdit>
#include <QAction>

#include "../Common/Icon.h"
#include "../Common/StyleSheet.h"


/**
 * ButtonBase implementation
 */

ButtonBase::ButtonBase(QWidget *parent)
    : QPushButton(parent)
    , m_spacing(3)
    , m_hasIconBase(false)
    , m_alignment(Qt::AlignCenter)
    , m_lightBackgroundColor(getThemeColor(ThemeColor::Light))
    , m_darkBackgroundColor(getThemeColor(ThemeColor::Dark))
    , m_hasHoverColorOverride(false)
    , m_isHover(false)
    , m_isPressed(false) {
    init();
}

ButtonBase::ButtonBase(const QString &text, QWidget *parent)
    : ButtonBase(parent) {
    setText(text);
}

ButtonBase::ButtonBase(const QString &text, const QIcon &icon, QWidget *parent)
    : ButtonBase(parent) {
    setText(text);
    setIcon(icon);
}

void ButtonBase::init() {
    setIconSize(QSize(16, 16));

    QFont font = this->font();
    font.setPointSize(12);
    setFont(font);

    m_backgroundColor = normalBackgroundColor();

    m_bgColorAnim = new QPropertyAnimation(this, "backgroundColor", this);
    m_bgColorAnim->setDuration(210);

    StyleSheetBase::apply(this, StyleSheetBase::Button);
}

void ButtonBase::setSpacing(int spacing) {
    m_spacing = spacing;
    updateGeometry();
}

int ButtonBase::spacing() const {
    return m_spacing;
}

void ButtonBase::setIcon(const QIcon &icon) {
    m_icon = icon;
    m_hasIconBase = false;
    setProperty("hasIcon", !icon.isNull());
    QPushButton::setIcon(icon);
}

void ButtonBase::setIcon(IconBase icon) {
    m_iconBase = icon;
    m_hasIconBase = true;
    m_icon = toQIcon(icon);
    setProperty("hasIcon", true);
    QPushButton::setIcon(m_icon);
}

QIcon ButtonBase::icon() const {
    return m_icon;
}

void ButtonBase::setAlignment(Qt::Alignment alignment) {
    m_alignment = alignment;
    update();
}

Qt::Alignment ButtonBase::alignment() const {
    return m_alignment;
}

QColor ButtonBase::backgroundColor() const {
    return m_backgroundColor;
}

void ButtonBase::setBackgroundColor(const QColor &color) {
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        update();
    }
}

void ButtonBase::setCustomBackgroundColor(const QColor &light, const QColor &dark) {
    m_lightBackgroundColor = light;
    m_darkBackgroundColor = dark;
    updateBackgroundColor();
}

void ButtonBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
    style()->unpolish(this);
    style()->polish(this);
}

void ButtonBase::setTransparent(bool transparent) {
    setProperty("isTransparent", transparent);
    style()->unpolish(this);
    style()->polish(this);
}

void ButtonBase::setHoverBackgroundColor(const QColor &color) {
    m_hoverBackgroundColorOverride = color;
    m_hasHoverColorOverride = true;
    if (m_isHover) {
        updateBackgroundColor();
    }
}

void ButtonBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

QSize ButtonBase::minimumSizeHint() const {
    QSize iconSize = !icon().isNull() ? this->iconSize() : QSize(0, 0);
    int textWidth = !text().isEmpty() ? fontMetrics().horizontalAdvance(text()) : 0;

    int width = m_spacing;
    if (!icon().isNull()) {
        width += iconSize.width() + m_spacing;
    }
    if (!text().isEmpty()) {
        width += textWidth + m_spacing;
    }

    int height = qMax(iconSize.height(), fontMetrics().height()) + m_spacing / 2;

    return QSize(width, height);
}

void ButtonBase::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QStylePainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // Draw background
    QStyleOptionButton option;
    initStyleOption(&option);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

    // Calculate content dimensions
    QSize iconSize = !icon().isNull() ? this->iconSize() : QSize(0, 0);
    int textWidth = !text().isEmpty() ? fontMetrics().horizontalAdvance(text()) : 0;

    int contentWidth = 0;
    if (!icon().isNull()) {
        contentWidth += iconSize.width() + m_spacing;
    }
    if (!text().isEmpty()) {
        contentWidth += textWidth;
    }

    int contentHeight = qMax(iconSize.height(), fontMetrics().height());

    // Calculate starting position based on alignment
    int x = (rect().width() - contentWidth) / 2;
    int y = (rect().height() - contentHeight) / 2;

    // Draw icon
    if (!icon().isNull()) {
        QRect iconRect(x, y, iconSize.width(), iconSize.height());
        drawIcon(icon(), &painter, iconRect);
        x += iconSize.width() + m_spacing;
    }

    // Draw text
    if (!text().isEmpty()) {
        QRect textRect(x, y, textWidth, contentHeight);
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
    }
}

void ButtonBase::mousePressEvent(QMouseEvent *event) {
    m_isPressed = true;
    updateBackgroundColor();
    QPushButton::mousePressEvent(event);
}

void ButtonBase::mouseReleaseEvent(QMouseEvent *event) {
    m_isPressed = false;
    updateBackgroundColor();
    QPushButton::mouseReleaseEvent(event);
}

void ButtonBase::enterEvent(QEnterEvent *event) {
    m_isHover = true;
    updateBackgroundColor();
    QPushButton::enterEvent(event);
}

void ButtonBase::leaveEvent(QEvent *event) {
    m_isHover = false;
    updateBackgroundColor();
    QPushButton::leaveEvent(event);
}

void ButtonBase::focusInEvent(QFocusEvent *event) {
    updateBackgroundColor();
    QPushButton::focusInEvent(event);
}

QColor ButtonBase::normalBackgroundColor() const {
    return isDarkTheme() ? m_darkBackgroundColor : m_lightBackgroundColor;
}

QColor ButtonBase::hoverBackgroundColor() const {
    if (m_hasHoverColorOverride) {
        return m_hoverBackgroundColorOverride;
    }
    return normalBackgroundColor().lighter(110);
}

QColor ButtonBase::pressedBackgroundColor() const {
    return normalBackgroundColor().darker(110);
}

QColor ButtonBase::disabledBackgroundColor() const {
    return normalBackgroundColor().lighter(120);
}

void ButtonBase::updateBackgroundColor() {
    QColor targetColor;

    if (!isEnabled()) {
        targetColor = disabledBackgroundColor();
    } else if (m_isPressed) {
        targetColor = pressedBackgroundColor();
    } else if (m_isHover) {
        targetColor = hoverBackgroundColor();
    } else {
        targetColor = normalBackgroundColor();
    }

    m_bgColorAnim->stop();
    m_bgColorAnim->setEndValue(targetColor);
    m_bgColorAnim->start();
}

void ButtonBase::_drawIcon(const QIcon &icon, QPainter *painter, const QRect &rect) {
    if (m_hasIconBase) {
        drawIcon(m_iconBase, painter, rect);
    } else {
        drawIcon(icon, painter, rect);
    }
}

/**
 * PrimaryButton implementation
 */

PrimaryButton::PrimaryButton(QWidget *parent)
    : ButtonBase(parent) {
}

PrimaryButton::PrimaryButton(const QString &text, QWidget *parent)
    : ButtonBase(text, parent) {
}

PrimaryButton::PrimaryButton(const QString &text, const QIcon &icon, QWidget *parent)
    : ButtonBase(text, icon, parent) {
}

QColor PrimaryButton::normalBackgroundColor() const {
    return QColor(0, 120, 215);
}

QColor PrimaryButton::hoverBackgroundColor() const {
    return QColor(0, 102, 204);
}

QColor PrimaryButton::pressedBackgroundColor() const {
    return QColor(0, 90, 158);
}

/**
 * TransparentButton implementation
 */

TransparentButton::TransparentButton(QWidget *parent)
    : ButtonBase(parent) {
}

TransparentButton::TransparentButton(const QString &text, QWidget *parent)
    : ButtonBase(text, parent) {
}

TransparentButton::TransparentButton(const QString &text, const QIcon &icon, QWidget *parent)
    : ButtonBase(text, icon, parent) {
}

QColor TransparentButton::normalBackgroundColor() const {
    return QColor(0, 0, 0, 0);
}

QColor TransparentButton::hoverBackgroundColor() const {
    return QColor(0, 0, 0, 20);
}

QColor TransparentButton::pressedBackgroundColor() const {
    return QColor(0, 0, 0, 40);
}

/**
 * ClearButton implementation
 */

ClearButton::ClearButton(QWidget *parent)
    : ButtonBase(parent)
    , m_isPressed(false) {
    setFocusPolicy(Qt::NoFocus);
    setCursor(Qt::PointingHandCursor);
    setIcon(IconBase::X);
}

void ClearButton::mousePressEvent(QMouseEvent *event) {
    m_isPressed = true;
    ButtonBase::mousePressEvent(event);
}

void ClearButton::mouseReleaseEvent(QMouseEvent *event) {
    m_isPressed = false;
    ButtonBase::mouseReleaseEvent(event);
}

void ClearButton::paintEvent(QPaintEvent *event) {
    ButtonBase::paintEvent(event);

    if (m_isPressed) {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter.setOpacity(0.75);
        // The base paintEvent will be called again with opacity
        // But for simplicity, we'll just apply opacity to the whole widget
        // This is a simplified implementation
    }
}

void NavigationButton::setHorizontal(bool horizontal) {
    setProperty("isHorizontal", horizontal);
}

/**
 * RotateButton implementation
 */

RotateButton::RotateButton(QWidget *parent)
    : QAbstractButton(parent), m_angle(0.0f), m_rotateAnimation(nullptr) {
    init();
}

void RotateButton::init() {
    m_rotateAnimation = new QPropertyAnimation(this, "angle", this);
    m_rotateAnimation->setDuration(210);

    connect(this, &QAbstractButton::clicked, this, [this]()
            { setRotate(m_angle < 180.0f); });
}

float RotateButton::angle() const {
    return m_angle;
}

void RotateButton::setAngle(float angle) {
    m_angle = angle;
    update();
}

void RotateButton::setRotate(bool isDown) {
    m_rotateAnimation->stop();
    m_rotateAnimation->setEndValue(isDown ? 180.0f : 0.0f);
    m_rotateAnimation->start();
}

void RotateButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);

    // Draw icon
    painter.translate(rect().center().x(), rect().center().y());
    painter.rotate(m_angle);
    QIcon icon = toQIcon(IconBase::Chevron_Down);
    icon.paint(&painter, QRect(-width() / 4, -height() / 4, width() / 2, height() / 2));
}

/**
 * FileButton implementation
 */

FileButton::FileButton(QWidget *parent)
    : ButtonBase(parent) {
    init();
}

void FileButton::init() {
    setFocusPolicy(Qt::NoFocus);
    setCursor(Qt::PointingHandCursor);
    setIcon(IconBase::OpenedFolder);
}

void FileButton::setFileDialog(QWidget *parent, QFileDialog::FileMode mode, const QString &fileType, const QString &directory, const QString &buttonTooltip) {
    auto callback = [parent, mode, fileType, directory]()
    {
        QString selectedPath;

        switch (mode)
        {
        case QFileDialog::ExistingFile:
        case QFileDialog::ExistingFiles:
        {
            QString dir = directory.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) : directory;
            selectedPath = QFileDialog::getOpenFileName(parent, "Select File", dir, fileType);
            break;
        }
        case QFileDialog::Directory:
        {
            QString dir = directory.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) : directory;
            selectedPath = QFileDialog::getExistingDirectory(parent, "Select Directory", dir);
            break;
        }
        case QFileDialog::AnyFile:
        {
            QString dir = directory.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) : directory;
            selectedPath = QFileDialog::getSaveFileName(parent, "Save File", dir, fileType);
            break;
        }
        default:
            break;
        }

        if (!selectedPath.isEmpty())
        {
            // Try to set text on parent widget if it's a line edit
            if (auto lineEdit = qobject_cast<QLineEdit *>(parent))
            {
                lineEdit->setText(selectedPath);
            }
        }
    };

    connect(this, &QPushButton::clicked, callback);
    setToolTip(buttonTooltip);
}

/**
 * MenuButton implementation
 */

MenuButton::MenuButton(QWidget *parent)
    : ButtonBase(parent), m_menu(nullptr) {
    init();
}

void MenuButton::init() {
    setIcon(IconBase::Ellipsis);
}

void MenuButton::setMenu(QMenu *menu) {
    m_menu = menu;

    connect(this, &QPushButton::clicked, this, [this]()
            {
        if (m_menu) {
            QSize menuSize = m_menu->sizeHint();
            QPoint pos = mapToGlobal(QPoint(width() - menuSize.width(), height()));
            m_menu->exec(pos);
        } });
}

void MenuButton::setMenu(const QMap<QString, std::function<void()>> &actionEvents) {
    m_menu = new QMenu(this);

    for (auto it = actionEvents.begin(); it != actionEvents.end(); ++it)
    {
        QAction *action = new QAction(it.key(), this);
        connect(action, &QAction::triggered, this, it.value());
        m_menu->addAction(action);
    }

    setMenu(m_menu);
}

/**
 * HollowButton implementation
 */

HollowButton::HollowButton(QWidget *parent)
    : ButtonBase(parent) {
}

/**
 * NavigationButton implementation
 */

NavigationButton::NavigationButton(QWidget *parent)
    : ButtonBase(parent) {
    init();
}

void NavigationButton::init() {
    setCheckable(true);
    setAutoExclusive(true);
    setSpacing(13.5);
    setAlignment(Qt::AlignLeft);
    setHorizontal(false);
}