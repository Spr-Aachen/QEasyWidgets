#ifndef QEASYWIDGETS_TOOLBOX_H
#define QEASYWIDGETS_TOOLBOX_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QList>
#include <QPropertyAnimation>

#include "Frame.h"
#include "Button.h"


// Forward declarations
class ToolPage;
class Folder;


/**
 * Folder header for ToolPage
 */
class Folder : public QLabel {
    Q_OBJECT

public:
    explicit Folder(QWidget *parent = nullptr);
    ~Folder() override = default;

    bool isEntered() const { return m_isEntered; }
    void setHoverColor(const QColor &color) { m_hoverColor = color; }

signals:
    void clicked();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void init();

    static constexpr int FOLDER_HEIGHT = 30;
    static constexpr int FOLDER_MARGIN = 6;

    bool m_isEntered = false;
    QColor m_hoverColor;
    RotateButton *m_folderButton = nullptr;
};


/**
 * Single page/item in ToolBox
 */
class ToolPage : public WidgetBase {
    Q_OBJECT

public:
    explicit ToolPage(QWidget *parent = nullptr);
    ~ToolPage() override = default;

    void addWidget(QWidget *widget);
    void setText(const QString &text);
    QString text() const;
    bool isExpanded() const { return m_isExpanded; }

    void expand();
    void collapse();
    void updateHeight(QWidget *addedWidget = nullptr);

signals:
    void resized(QWidget *widget = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void init();

    bool m_isExpanded = true;
    Folder *m_folder = nullptr;
    WidgetBase *m_widget = nullptr;
};


/**
 * Enhanced tool box with theme support and multiple collapsible pages
 */
class ToolBoxBase : public FrameBase {
    Q_OBJECT

public:
    explicit ToolBoxBase(QWidget *parent = nullptr);
    ~ToolBoxBase() override = default;

    // Item management
    void addItem(QWidget *widget, const QString &text);
    ToolPage *widget(int index) const;
    void setItemText(int index, const QString &text);

    // Current index management
    void setCurrentIndex(int index);
    int currentIndex() const;
    int indexOf(QWidget *widget) const;

    // Style properties
    void setBorderless(bool borderless);
    void setTransparent(bool transparent);
    void clearDefaultStyleSheet();

private:
    void init();
    void updateHeight();

    QList<ToolPage *> m_toolPages;
    int m_currentIndex = -1;
};


#endif // QEASYWIDGETS_TOOLBOX_H