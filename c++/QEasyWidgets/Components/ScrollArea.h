#ifndef QEASYWIDGETS_SCROLLAREA_H
#define QEASYWIDGETS_SCROLLAREA_H

#include <QScrollArea>

#include "ScrollBar.h"


namespace QEW {

/**
 * @brief Enhanced scroll area with theme support and custom scroll bars
 */
class ScrollAreaBase : public QScrollArea
{
    Q_OBJECT

public:
    explicit ScrollAreaBase(QWidget *parent = nullptr);
    ~ScrollAreaBase() override = default;

    ScrollBar* verticalScrollBar() const { return m_verticalScrollBar; }
    ScrollBar* horizontalScrollBar() const { return m_horizontalScrollBar; }

private:
    void init();

    ScrollBar *m_verticalScrollBar;
    ScrollBar *m_horizontalScrollBar;
};

/**
 * @brief Vertical scroll area with custom scroll bar
 */
class VerticalScrollArea : public ScrollAreaBase
{
    Q_OBJECT

public:
    explicit VerticalScrollArea(QWidget *parent = nullptr);
    ~VerticalScrollArea() override = default;
};

/**
 * @brief Horizontal scroll area with custom scroll bar
 */
class HorizontalScrollArea : public ScrollAreaBase
{
    Q_OBJECT

public:
    explicit HorizontalScrollArea(QWidget *parent = nullptr);
    ~HorizontalScrollArea() override = default;
};

} // namespace QEW

#endif // QEASYWIDGETS_SCROLLAREA_H
