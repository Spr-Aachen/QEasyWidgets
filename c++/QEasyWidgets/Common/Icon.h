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


/**
 * Custom icon engine for SVG icons
 */
class IconEngine : public QIconEngine {
public:
    IconEngine(const QIcon *icon = nullptr);
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
 * Icon base enumeration
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
    Copy,
    Scissors,
    Clipboard,
    Download,
    Send,
    Play,
    Pause,
    Stop,
    Dash,
    FullScreen,
    FullScreen_Exit,
    X
};


/**
 * Get icon name string
 */
QString iconName(IconBase icon);


/**
 * Create QIcon from IconBase
 */
QIcon createIcon(IconBase icon, const QString &theme = QString());


/**
 * Draw icon on painter
 */
void drawIcon(IconBase icon, QPainter *painter, const QRect &rect, const QString &theme = QString());


/**
 * Draw icon on painter (QIcon version)
 */
void drawIcon(const QIcon &icon, QPainter *painter, const QRect &rect);


/**
 * Convert to QIcon
 */
QIcon toQIcon(IconBase icon);
QIcon toQIcon(const QString &iconPath);


#endif // QEASYWIDGETS_ICON_H