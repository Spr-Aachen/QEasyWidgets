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


namespace QEW {

/**
 * @brief Find child widget of specific type
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
 * @brief Find parent widget of specific type
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
 * @brief Get widget width
 */
inline int getWidth(QWidget *widget) {
    if (!widget) return 0;
    return (widget->size() == QSize(100, 30)) ? widget->geometry().width() : widget->width();
}

/**
 * @brief Get widget height
 */
inline int getHeight(QWidget *widget) {
    if (!widget) return 0;
    return (widget->size() == QSize(100, 30)) ? widget->geometry().height() : widget->height();
}

/**
 * @brief Remove all sub-widgets from stacked widget
 */
void removeSubWidgets(QStackedWidget *widget);

/**
 * @brief Set retain size when hidden
 */
void setRetainSizeWhenHidden(QWidget *widget, bool retainSize = true);

/**
 * @brief Set drop shadow effect
 */
void setDropShadowEffect(QWidget *widget, qreal radius = 3.0, const QColor &color = Qt::gray);

/**
 * @brief Set animation parameters
 */
void setAnimation(QPropertyAnimation *animation, const QVariant &startValue, const QVariant &endValue, int duration);

/**
 * @brief Set widget position animation
 */
QPropertyAnimation* setWidgetPosAnimation(QWidget *widget, int duration = 99);

/**
 * @brief Set widget size animation
 */
QParallelAnimationGroup* setWidgetSizeAnimation(QWidget *widget, int targetWidth = -1, int targetHeight = -1, int duration = 210);

/**
 * @brief Set widget opacity animation
 */
QPropertyAnimation* setWidgetOpacityAnimation(QWidget *widget, qreal originalOpacity, qreal targetOpacity, int duration = 99);

/**
 * @brief Set font for widget
 */
void setFont(QWidget *widget, int fontSize = 12, QFont::Weight weight = QFont::Normal, const QStringList &families = QStringList("Microsoft YaHei"));

/**
 * @brief Set text for widget
 */
void setText(QWidget *widget, const QString &text, bool setHtml = true, bool setPlaceholderText = false, const QString &placeholderText = QString());

/**
 * @brief Get text from widget
 */
QString getText(QWidget *widget, bool getHtml = false, bool getPlaceholderText = false);

/**
 * @brief File dialog modes
 */
enum class FileDialogMode {
    SelectFolder,
    SelectFile,
    SaveFile
};

/**
 * @brief Get file dialog
 */
QString getFileDialog(FileDialogMode mode, const QString &fileType = QString(), const QString &directory = QString());

/**
 * @brief Set context menu with actions and their callbacks
 * @param parent Parent widget
 * @param contextMenu Menu to configure
 * @param actions Map of action names to their callback functions
 */
void setContextMenu(QWidget *parent, QMenu *contextMenu, const QMap<QString, std::function<void()>> &actions);

/**
 * @brief Show context menu at position
 * @param parent Parent widget
 * @param contextMenu Menu to show
 * @param actions Map of action names to their callback functions
 * @param position Position to show menu
 */
void showContextMenu(QWidget *parent, QMenu *contextMenu, const QMap<QString, std::function<void()>> &actions, const QPoint &position);

/**
 * @brief Open URL
 */
void openURL(const QString &url, bool createIfNotExist = false);
void openURL(const QStringList &urls, bool createIfNotExist = false);

/**
 * @brief Save/restore layout
 */
void saveLayout(QWidget *widget, QSettings *settings);
void restoreLayout(QWidget *widget, QSettings *settings);

/**
 * @brief Get current screen
 */
QScreen* getCurrentScreen();

/**
 * @brief Get screen geometry
 */
QRect getScreenGeometry(QScreen *screen = nullptr, bool getAvailableGeometry = true);

} // namespace QEW

#endif // QEASYWIDGETS_QFUNCTIONS_H
