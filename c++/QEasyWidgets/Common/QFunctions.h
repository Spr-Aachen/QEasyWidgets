#ifndef QEASYWIDGETS_QFUNCTIONS_H
#define QEASYWIDGETS_QFUNCTIONS_H

#include <QWidget>
#include <QStackedWidget>
#include <QSize>
#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QFont>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QUrl>
#include <QDesktopServices>
#include <QSettings>
#include <QScreen>
#include <QApplication>
#include <QCursor>
#include <functional>


/**
 * Find child widget of specific type
 */
template<typename T>
T* findChild(QWidget *parent) {
    if (!parent) return nullptr;

    const QObjectList &children = parent->children();
    for (QObject *child : children) {
        T *typedChild = qobject_cast<T*>(child);
        if (typedChild) {
            return typedChild;
        }
    }
    return nullptr;
}


/**
 * Find parent widget of specific type
 */
template<typename T>
T* findParent(QWidget *child) {
    if (!child) return nullptr;

    QObject *parent = child->parent();
    while (parent) {
        T *typedParent = qobject_cast<T*>(parent);
        if (typedParent) {
            return typedParent;
        }
        parent = parent->parent();
    }
    return nullptr;
}


/**
 * Get widget width
 */
inline int getWidth(QWidget *widget) {
    if (!widget) return 0;
    return (widget->size() == QSize(100, 30)) ? widget->geometry().width() : widget->width();
}


/**
 * Get widget height
 */
inline int getHeight(QWidget *widget) {
    if (!widget) return 0;
    return (widget->size() == QSize(100, 30)) ? widget->geometry().height() : widget->height();
}


/**
 * Remove all sub-widgets from stacked widget
 */
void removeSubWidgets(QStackedWidget *widget);


/**
 * Set retain size when hidden
 */
void setRetainSizeWhenHidden(QWidget *widget, bool retainSize = true);


/**
 * Set drop shadow effect
 */
void setDropShadowEffect(QWidget *widget, qreal radius = 3.0, const QColor &color = Qt::gray);


/**
 * Set animation parameters
 */
void setAnimation(QPropertyAnimation *animation, const QVariant &startValue, const QVariant &endValue, int duration);


/**
 * Set widget position animation
 */
QPropertyAnimation* setWidgetPosAnimation(QWidget *widget, int duration = 99);


/**
 * Set widget size animation
 */
QParallelAnimationGroup* setWidgetSizeAnimation(QWidget *widget, int targetWidth = -1, int targetHeight = -1, int duration = 210);


/**
 * Set widget opacity animation
 */
QPropertyAnimation* setWidgetOpacityAnimation(QWidget *widget, qreal originalOpacity, qreal targetOpacity, int duration = 99);


/**
 * Set font for widget
 */
void setFont(QWidget *widget, int fontSize = 12, QFont::Weight weight = QFont::Normal, const QStringList &families = QStringList("Microsoft YaHei"));


/**
 * Set text for widget
 */
void setText(QWidget *widget, const QString &text, bool setHtml = true, bool setPlaceholderText = false, const QString &placeholderText = QString());


/**
 * Get text from widget
 */
QString getText(QWidget *widget, bool getHtml = false, bool getPlaceholderText = false);


/**
 * File dialog modes
 */
enum class FileDialogMode {
    SelectFolder,
    SelectFile,
    SaveFile
};


/**
 * Get file dialog
 */
QString getFileDialog(FileDialogMode mode, const QString &fileType = QString(), const QString &directory = QString());


/**
 * Set context menu with actions and their callbacks
 * @param parent Parent widget
 * @param contextMenu Menu to configure
 * @param actions Map of action names to their callback functions
 */
void setContextMenu(QWidget *parent, QMenu *contextMenu, const QMap<QString, std::function<void()>> &actions);


/**
 * Show context menu at position
 * @param parent Parent widget
 * @param contextMenu Menu to show
 * @param actions Map of action names to their callback functions
 * @param position Position to show menu
 */
void showContextMenu(QWidget *parent, QMenu *contextMenu, const QMap<QString, std::function<void()>> &actions, const QPoint &position);


/**
 * Open URL
 */
void openURL(const QString &url, bool createIfNotExist = false);
void openURL(const QStringList &urls, bool createIfNotExist = false);


/**
 * Save/restore layout
 */
void saveLayout(QWidget *widget, QSettings *settings);
void restoreLayout(QWidget *widget, QSettings *settings);


/**
 * Get current screen
 */
QScreen* getCurrentScreen();


/**
 * Get screen geometry
 */
QRect getScreenGeometry(QScreen *screen = nullptr, bool getAvailableGeometry = true);


#endif // QEASYWIDGETS_QFUNCTIONS_H