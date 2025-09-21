#include "QFunctions.h"

#include <QGraphicsOpacityEffect>
#include <QEasingCurve>
#include <QDir>
#include <QDebug>
#include <QMainWindow>
#include <QDockWidget>


void removeSubWidgets(QStackedWidget *widget) {
    if (!widget) return;

    while (widget->count() > 0) {
        QWidget *w = widget->widget(0);
        widget->removeWidget(w);
        w->deleteLater();
    }
}


void setRetainSizeWhenHidden(QWidget *widget, bool retainSize) {
    if (!widget) return;

    QSizePolicy policy = widget->sizePolicy();
    policy.setRetainSizeWhenHidden(retainSize);
    widget->setSizePolicy(policy);
}


void setDropShadowEffect(QWidget *widget, qreal radius, const QColor &color) {
    if (!widget) return;

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(widget);
    effect->setOffset(0, 0);
    effect->setBlurRadius(radius);
    effect->setColor(color);
    widget->setGraphicsEffect(effect);
}


void setAnimation(QPropertyAnimation *animation, const QVariant &startValue, const QVariant &endValue, int duration) {
    if (!animation) return;

    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::InOutQuart);
}


QPropertyAnimation* setWidgetPosAnimation(QWidget *widget, int duration) {
    if (!widget) return nullptr;

    QRect originalGeometry = widget->geometry();
    QRect alteredGeometry(
        originalGeometry.left(),
        originalGeometry.top() + originalGeometry.height() / duration,
        originalGeometry.width(),
        originalGeometry.height()
    );

    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry", widget);
    setAnimation(animation, originalGeometry, alteredGeometry, duration);

    return animation;
}


QParallelAnimationGroup* setWidgetSizeAnimation(QWidget *widget, int targetWidth, int targetHeight, int duration) {
    if (!widget) return nullptr;

    int currentWidth = getWidth(widget);
    int currentHeight = getHeight(widget);

    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup(widget);

    if (widget->property("currentWidth").isValid() && widget->property("currentHeight").isValid()) {
        if (targetWidth >= 0) {
            QPropertyAnimation *widthAnim = new QPropertyAnimation(widget, "currentWidth", widget);
            setAnimation(widthAnim, currentWidth, targetWidth, duration);
            animationGroup->addAnimation(widthAnim);
        }
        if (targetHeight >= 0) {
            QPropertyAnimation *heightAnim = new QPropertyAnimation(widget, "currentHeight", widget);
            setAnimation(heightAnim, currentHeight, targetHeight, duration);
            animationGroup->addAnimation(heightAnim);
        }
    } else {
        if (targetWidth >= 0) {
            QPropertyAnimation *minWidthAnim = new QPropertyAnimation(widget, "minimumWidth", widget);
            QPropertyAnimation *maxWidthAnim = new QPropertyAnimation(widget, "maximumWidth", widget);
            setAnimation(minWidthAnim, currentWidth, targetWidth, duration);
            setAnimation(maxWidthAnim, currentWidth, targetWidth, duration);
            animationGroup->addAnimation(minWidthAnim);
            animationGroup->addAnimation(maxWidthAnim);
        }
        if (targetHeight >= 0) {
            QPropertyAnimation *minHeightAnim = new QPropertyAnimation(widget, "minimumHeight", widget);
            QPropertyAnimation *maxHeightAnim = new QPropertyAnimation(widget, "maximumHeight", widget);
            setAnimation(minHeightAnim, currentHeight, targetHeight, duration);
            setAnimation(maxHeightAnim, currentHeight, targetHeight, duration);
            animationGroup->addAnimation(minHeightAnim);
            animationGroup->addAnimation(maxHeightAnim);
        }
    }

    return animationGroup;
}


QPropertyAnimation* setWidgetOpacityAnimation(QWidget *widget, qreal originalOpacity, qreal targetOpacity, int duration) {
    if (!widget) return nullptr;

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity", widget);
    setAnimation(animation, originalOpacity, targetOpacity, duration);

    return animation;
}


void setFont(QWidget *widget, int fontSize, QFont::Weight weight, const QStringList &families) {
    if (!widget) return;

    QFont font;
    font.setFamilies(families);
    font.setPixelSize(fontSize);
    font.setWeight(weight);
    widget->setFont(font);
}


void setText(QWidget *widget, const QString &text, bool setHtml, bool setPlaceholderText, const QString &placeholderText) {
    if (!widget) return;

    // Try different text setting methods
    if (widget->metaObject()->indexOfMethod("setText(QString)") >= 0) {
        QMetaObject::invokeMethod(widget, "setText", Q_ARG(QString, text));
    }
    if (widget->metaObject()->indexOfMethod("setPlainText(QString)") >= 0) {
        QMetaObject::invokeMethod(widget, "setPlainText", Q_ARG(QString, text));
    }
    if (setHtml && widget->metaObject()->indexOfMethod("setHtml(QString)") >= 0) {
        QMetaObject::invokeMethod(widget, "setHtml", Q_ARG(QString, text));
    }
    if (setPlaceholderText && widget->metaObject()->indexOfMethod("setPlaceholderText(QString)") >= 0) {
        QString phText = placeholderText.isEmpty() ? text : placeholderText;
        QMetaObject::invokeMethod(widget, "setPlaceholderText", Q_ARG(QString, phText));
    }
}


QString getText(QWidget *widget, bool getHtml, bool getPlaceholderText) {
    if (!widget) return QString();

    QString text;

    // Try different text getting methods
    if (widget->metaObject()->indexOfMethod("text()") >= 0) {
        QMetaObject::invokeMethod(widget, "text", Q_RETURN_ARG(QString, text));
    } else if (widget->metaObject()->indexOfMethod("toPlainText()") >= 0) {
        QMetaObject::invokeMethod(widget, "toPlainText", Q_RETURN_ARG(QString, text));
    }

    if (getHtml && widget->metaObject()->indexOfMethod("toHtml()") >= 0) {
        QMetaObject::invokeMethod(widget, "toHtml", Q_RETURN_ARG(QString, text));
    }
    if (getPlaceholderText && widget->metaObject()->indexOfMethod("placeholderText()") >= 0) {
        QString phText;
        QMetaObject::invokeMethod(widget, "placeholderText", Q_RETURN_ARG(QString, phText));
        if (!phText.isEmpty() && text.trimmed().isEmpty()) {
            text = phText;
        }
    }

    return text;
}


QString getFileDialog(FileDialogMode mode, const QString &fileType, const QString &directory) {
    QString result;
    QString startDir = directory.isEmpty() ? QDir::currentPath() : directory;

    if (!QDir(startDir).exists()) {
        QDir().mkpath(startDir);
    }

    switch (mode) {
        case FileDialogMode::SelectFolder:
            result = QFileDialog::getExistingDirectory(nullptr, "选择文件夹 | SelectFolder", startDir);
            break;
        case FileDialogMode::SelectFile: {
            QString filter = fileType.isEmpty() ? "任意类型 (*.*)" : fileType;
            result = QFileDialog::getOpenFileName(nullptr, "选择文件 | SelectFile", startDir, filter);
            break;
        }
        case FileDialogMode::SaveFile: {
            QString filter = fileType.isEmpty() ? "任意类型 (*.*)" : fileType;
            result = QFileDialog::getSaveFileName(nullptr, "保存文件 | SaveFile", startDir, filter);
            break;
        }
    }

    return result;
}


void setContextMenu(QWidget *parent, QMenu *contextMenu, const QMap<QString, std::function<void()>> &actions) {
    if (!parent || !contextMenu) return;

    for (auto it = actions.begin(); it != actions.end(); ++it) {
        QAction *action = new QAction(it.key(), parent);
        // Connect using lambda to call the std::function
        QObject::connect(action, &QAction::triggered, parent, it.value());
        contextMenu->addAction(action);
    }
}


void showContextMenu(QWidget *parent, QMenu *contextMenu, const QMap<QString, std::function<void()>> &actions, const QPoint &position) {
    setContextMenu(parent, contextMenu, actions);
    contextMenu->exec(position);
}


void openURL(const QString &url, bool createIfNotExist) {
    QUrl qurl = QUrl::fromLocalFile(url);
    if (qurl.isValid()) {
        if (createIfNotExist) {
            QDir().mkpath(url);
        }
        bool success = QDesktopServices::openUrl(qurl);
        if (!success) {
            qDebug() << "Failed to open URL:" << url;
        }
    } else {
        qDebug() << "Invalid URL:" << url;
    }
}


void openURL(const QStringList &urls, bool createIfNotExist) {
    for (const QString &url : urls) {
        openURL(url, createIfNotExist);
    }
}


void saveLayout(QWidget *widget, QSettings *settings) {
    if (!widget || !settings) return;

    settings->setValue("layout/geometry", widget->saveGeometry());
    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(widget)) {
        settings->setValue("layout/state", mainWindow->saveState());
    }
    settings->sync();
}


void restoreLayout(QWidget *widget, QSettings *settings) {
    if (!widget || !settings) return;

    widget->restoreGeometry(settings->value("layout/geometry").toByteArray());
    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(widget)) {
        mainWindow->restoreState(settings->value("layout/state").toByteArray());
        // Restore dock widgets
        QList<QDockWidget*> dockWidgets = mainWindow->findChildren<QDockWidget*>();
        for (QDockWidget *dock : dockWidgets) {
            mainWindow->restoreDockWidget(dock);
        }
    }
}


QScreen* getCurrentScreen() {
    QPoint cursorPos = QCursor::pos();
    QList<QScreen*> screens = QApplication::screens();
    for (QScreen *screen : screens) {
        if (screen->geometry().contains(cursorPos)) {
            return screen;
        }
    }
    return QApplication::primaryScreen();
}


QRect getScreenGeometry(QScreen *screen, bool getAvailableGeometry) {
    if (!screen) {
        screen = getCurrentScreen();
        if (!screen) {
            screen = QApplication::primaryScreen();
        }
    }

    if (!screen) return QRect();

    return getAvailableGeometry ? screen->availableGeometry() : screen->geometry();
}