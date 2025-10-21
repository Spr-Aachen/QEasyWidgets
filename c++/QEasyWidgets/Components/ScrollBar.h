#ifndef QEASYWIDGETS_SCROLLBAR_H
#define QEASYWIDGETS_SCROLLBAR_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QAbstractScrollArea>
#include <QToolButton>

#include "../Common/Icon.h"


namespace QEW {

/**
 * @brief Arrow button for scroll bar
 */
class ArrowButton : public QToolButton
{
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
 * @brief Scroll bar groove (background)
 */
class ScrollBarGroove : public QWidget
{
    Q_OBJECT

public:
    explicit ScrollBarGroove(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~ScrollBarGroove() override = default;

    void fadeIn();
    void fadeOut();

    ArrowButton* upButton() const { return m_upButton; }
    ArrowButton* downButton() const { return m_downButton; }

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
 * @brief Scroll bar handle (thumb)
 */
class ScrollBarHandle : public QWidget
{
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
 * @brief Custom fluent-style scroll bar
 */
class ScrollBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValueProperty NOTIFY valueChanged)

public:
    explicit ScrollBar(Qt::Orientation orientation, QAbstractScrollArea *parent = nullptr);
    ~ScrollBar() override = default;

    int minimum() const { return m_minimum; }
    int maximum() const { return m_maximum; }
    int value() const { return m_value; }
    int pageStep() const { return m_pageStep; }
    int singleStep() const { return m_singleStep; }

    void setMinimum(int min);
    void setMaximum(int max);
    void setRange(int min, int max);
    void setValue(int value);
    void setValueImmediately(int value);
    void setPageStep(int step);
    void setSingleStep(int step);

signals:
    void valueChanged(int value);
    void rangeChanged(int min, int max);

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
    bool m_isEnter;
    bool m_isExpanded;
    
    QPropertyAnimation *m_animation;
    QTimer *m_timer;
};

} // namespace QEW

#endif // QEASYWIDGETS_SCROLLBAR_H
