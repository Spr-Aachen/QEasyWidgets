#include "Icon.h"
#include "Theme.h"

#include <QFile>
#include <QDomDocument>
#include <QImage>
#include <Qt>


IconEngine::IconEngine(const QIcon *icon)
    : m_isIconSVG(false) {
    Q_UNUSED(icon);
}


void IconEngine::loadSVG(const QString &svgString) {
    m_isIconSVG = true;
    m_iconData = svgString.toUtf8();
}


void IconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode);
    Q_UNUSED(state);
    
    if (m_isIconSVG) {
        QSvgRenderer renderer(m_iconData);
        renderer.render(painter, QRectF(rect));
    }
}


QPixmap IconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) {
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);
    
    QPainter painter(&pixmap);
    QRect rect(0, 0, size.width(), size.height());
    paint(&painter, rect, mode, state);
    
    return pixmap;
}


QIconEngine *IconEngine::clone() const {
    IconEngine *engine = new IconEngine();
    engine->m_isIconSVG = m_isIconSVG;
    engine->m_iconData = m_iconData;
    return engine;
}


QString iconName(IconBase icon) {
    switch (icon) {
        case IconBase::Arrow_Clockwise: return "Arrow-Clockwise";
        case IconBase::Arrow_Repeat: return "Arrow-Repeat";
        case IconBase::Chevron_Left: return "Chevron-Left";
        case IconBase::CompactChevron_Left: return "CompactChevron-Left";
        case IconBase::Chevron_Right: return "Chevron-Right";
        case IconBase::CompactChevron_Right: return "CompactChevron-Right";
        case IconBase::Chevron_Up: return "Chevron-Up";
        case IconBase::Chevron_Down: return "Chevron-Down";
        case IconBase::Ellipsis: return "Ellipsis";
        case IconBase::OpenedFolder: return "OpenedFolder";
        case IconBase::Copy: return "Copy";
        case IconBase::Scissors: return "Scissors";
        case IconBase::Clipboard: return "Clipboard";
        case IconBase::Download: return "Download";
        case IconBase::Send: return "Send";
        case IconBase::Play: return "Play";
        case IconBase::Pause: return "Pause";
        case IconBase::Stop: return "Stop";
        case IconBase::Dash: return "Dash";
        case IconBase::FullScreen: return "FullScreen";
        case IconBase::FullScreen_Exit: return "FullScreen-Exit";
        case IconBase::X: return "X";
        default: return "";
    }
}


QIcon createIcon(IconBase icon, const QString &theme) {
    QString themeName = theme.isEmpty() ? ThemeString(currentTheme()) : theme;
    QString iconPath = QString(":/Icons/icons/%1/%2.svg").arg(themeName).arg(iconName(icon));
    
    QFile file(iconPath);
    if (!file.open(QFile::ReadOnly)) {
        return QIcon();
    }
    
    QDomDocument domDocument;
    domDocument.setContent(file.readAll());
    file.close();
    
    IconEngine *engine = new IconEngine();
    engine->loadSVG(domDocument.toString());
    
    return QIcon(engine);
}


void drawIcon(IconBase icon, QPainter *painter, const QRect &rect, const QString &theme) {
    QString themeName = theme.isEmpty() ? ThemeString(currentTheme()) : theme;
    QString iconPath = QString(":/Icons/icons/%1/%2.svg").arg(themeName).arg(iconName(icon));
    
    QSvgRenderer renderer(iconPath);
    renderer.render(painter, QRectF(rect));
}


void drawIcon(const QIcon &icon, QPainter *painter, const QRect &rect) {
    icon.paint(painter, rect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
}


QIcon toQIcon(IconBase icon) {
    return createIcon(icon);
}


QIcon toQIcon(const QString &iconPath) {
    return QIcon(iconPath);
}