#include "ScrollArea.h"

#include "../Common/StyleSheet.h"


namespace QEW {

ScrollAreaBase::ScrollAreaBase(QWidget *parent)
    : QScrollArea(parent)
    , m_verticalScrollBar(nullptr)
    , m_horizontalScrollBar(nullptr)
{
    init();
}

void ScrollAreaBase::init()
{
    setWidgetResizable(true);
    
    // Create custom scroll bars
    m_verticalScrollBar = new ScrollBar(Qt::Vertical, this);
    m_horizontalScrollBar = new ScrollBar(Qt::Horizontal, this);
    
    StyleSheetBase::apply(this, StyleSheetBase::ScrollArea);
}

// VerticalScrollArea implementation
VerticalScrollArea::VerticalScrollArea(QWidget *parent)
    : ScrollAreaBase(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

// HorizontalScrollArea implementation
HorizontalScrollArea::HorizontalScrollArea(QWidget *parent)
    : ScrollAreaBase(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

} // namespace QEW
