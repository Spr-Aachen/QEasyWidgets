#ifndef QEASYWIDGETS_SCROLLAREA_H
#define QEASYWIDGETS_SCROLLAREA_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QAbstractScrollArea>
#include <QToolButton>
#include <QScrollArea>

#include "../Common/Icon.h"


/**
 * Arrow button for scroll bar
 */
class ArrowButton : public QToolButton {
    Q_OBJECT

public:
    explicit ArrowButton(IconBase icon, QWidget *parent = nullptr);
    ~ArrowButton() override = default;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    IconBase m_icon;
};


/**
 * Scroll bar groove (background)
 */
class ScrollBarGroove : public QWidget {
    Q_OBJECT

public:
    explicit ScrollBarGroove(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~ScrollBarGroove() override = default;

    void fadeIn();
    void fadeOut();

    ArrowButton* upButton() const {
        return m_upButton;
    }
    ArrowButton* downButton() const {
        return m_downButton;
    }
    QPropertyAnimation* opacityAnimation() const {
        return m_opacityAnimation;
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void init();

    Qt::Orientation m_orientation;
    ArrowButton *m_upButton;
    ArrowButton *m_downButton;
    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation *m_opacityAnimation;
};


/**
 * Scroll bar handle (thumb)
 */
class ScrollBarHandle : public QWidget {
    Q_OBJECT

public:
    explicit ScrollBarHandle(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~ScrollBarHandle() override = default;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Qt::Orientation m_orientation;
};


/**
 * Custom fluent-style scroll bar
 */
class ScrollBar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValueProperty NOTIFY valueChanged)

public:
    explicit ScrollBar(Qt::Orientation orientation, QAbstractScrollArea *parent = nullptr);
    ~ScrollBar() override = default;

    int minimum() const {
        return m_minimum;
    }
    int maximum() const {
        return m_maximum;
    }
    int value() const {
        return m_value;
    }
    int pageStep() const {
        return m_pageStep;
    }
    int singleStep() const {
        return m_singleStep;
    }

    void setMinimum(int min);
    void setMaximum(int max);
    void setRange(int min, int max);
    void setValue(int value);
    void setValueImmediately(int value);
    void setPageStep(int step);
    void setSingleStep(int step);
    void setAlwaysOff(bool alwaysOff);
    bool isAlwaysOff() const {
        return m_isAlwaysOff;
    }

    // Slider signals and control
    bool isSliderDown() const;
    void setSliderDown(bool isDown);

signals:
    void valueChanged(int value);
    void rangeChanged(int min, int max);
    void sliderPressed();
    void sliderReleased();
    void sliderMoved();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onPageUp();
    void onPageDown();
    void onParentValueChanged(int value);
    void onOpacityChanged();

private:
    void init();
    void adjustHandleSize();
    void adjustHandlePos();
    void adjustPos(const QSize &size);
    void expand();
    void collapse();
    void jumpToPosition(const QPoint &pos);
    
    int grooveLength() const;
    int slideLength() const;
    
    void setValueProperty(int value);

    Qt::Orientation m_orientation;
    QAbstractScrollArea *m_scrollArea;
    
    ScrollBarGroove *m_groove;
    ScrollBarHandle *m_handle;
    
    int m_minimum;
    int m_maximum;
    int m_value;
    int m_pageStep;
    int m_singleStep;
    int m_padding;
    
    QPoint m_pressedPos;
    bool m_isPressed;
    // Track whether the handle itself was pressed for dragging
    bool m_handlePressed;
    int m_pressedValue;
    bool m_isEnter;
    bool m_isExpanded;
    
    QPropertyAnimation *m_animation;
    QTimer *m_timer;
    bool m_isAlwaysOff;
};


/**
 * Scroll delegate to manage custom scroll bars
 */
class ScrollDelegate : public QObject {
    Q_OBJECT

public:
    explicit ScrollDelegate(QAbstractScrollArea *parentArea);
    ~ScrollDelegate() override = default;

    ScrollBar* verticalScrollBar() const;
    ScrollBar* horizontalScrollBar() const;

    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QAbstractScrollArea *m_parentArea;
    ScrollBar *m_vScrollBar;
    ScrollBar *m_hScrollBar;
};


/**
 * Enhanced scroll area with theme support and custom scroll bars
 */
class ScrollAreaBase : public QScrollArea {
    Q_OBJECT

public:
    explicit ScrollAreaBase(QWidget *parent = nullptr);
    ~ScrollAreaBase() override = default;

    ScrollBar* verticalScrollBar() const {
        return m_verticalScrollBar;
    }
    ScrollBar* horizontalScrollBar() const {
        return m_horizontalScrollBar;
    }

    // Emit when viewport size changes
    Q_SIGNAL void viewportSizeChanged(const QSize &size);

    // Control scrollbar policy: force native scrollbars off and manage custom ones
    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);

    // Widget helpers
    void setWidget(QWidget *widget);
    void resizeEvent(QResizeEvent *event) override;

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);
    void clearDefaultStyleSheet();

private:
    void init();

    ScrollBar *m_verticalScrollBar;
    ScrollBar *m_horizontalScrollBar;
    ScrollDelegate *m_delegate;
};


/**
 * Vertical scroll area with custom scroll bar
 */
class VerticalScrollArea : public ScrollAreaBase {
    Q_OBJECT

public:
    explicit VerticalScrollArea(QWidget *parent = nullptr);
    ~VerticalScrollArea() override = default;
};


/**
 * Horizontal scroll area with custom scroll bar
 */
class HorizontalScrollArea : public ScrollAreaBase {
    Q_OBJECT

public:
    explicit HorizontalScrollArea(QWidget *parent = nullptr);
    ~HorizontalScrollArea() override = default;
};


#endif // QEASYWIDGETS_SCROLLAREA_H