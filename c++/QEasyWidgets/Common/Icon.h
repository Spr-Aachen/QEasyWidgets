#ifndef QEASYWIDGETS_ICON_H
#define QEASYWIDGETS_ICON_H

#include <QIcon>
#include <QIconEngine>
#include <QPainter>
#include <QRect>
#include <QSize>
#include <QPixmap>
#include <QSvgRenderer>
#include <QString>


namespace QEW {

/**
 * @brief Custom icon engine for SVG icons
 */
class IconEngine : public QIconEngine
{
public:
    IconEngine();
    ~IconEngine() override = default;

    void loadSVG(const QString &svgString);
    
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine *clone() const override;

private:
    bool m_isIconSVG;
    QByteArray m_iconData;
};

/**
 * @brief Icon base enumeration
 */
enum class IconBase {
    Arrow_Clockwise,
    Arrow_Repeat,
    Chevron_Left,
    CompactChevron_Left,
    Chevron_Right,
    CompactChevron_Right,
    Chevron_Up,
    Chevron_Down,
    Ellipsis,
    OpenedFolder,
    Clipboard,
    Download,
    Play,
    Pause,
    Send,
    Stop,
    Dash,
    Window_FullScreen,
    Window_Stack,
    FullScreen,
    FullScreen_Exit,
    X
};

/**
 * @brief Get icon name string
 */
QString iconName(IconBase icon);

/**
 * @brief Create QIcon from IconBase
 */
QIcon createIcon(IconBase icon, const QString &theme = QString());

/**
 * @brief Draw icon on painter
 */
void drawIcon(IconBase icon, QPainter *painter, const QRect &rect, const QString &theme = QString());

/**
 * @brief Draw icon on painter (QIcon version)
 */
void drawIcon(const QIcon &icon, QPainter *painter, const QRect &rect);

/**
 * @brief Convert to QIcon
 */
QIcon toQIcon(IconBase icon);
QIcon toQIcon(const QString &iconPath);

} // namespace QEW

#endif // QEASYWIDGETS_ICON_H