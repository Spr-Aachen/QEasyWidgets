#include "ScrollArea.h"

#include <QPainter>
#include <QScrollBar>
#include <QTimer>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWheelEvent>

#include "../Common/Theme.h"
#include "../Common/Icon.h"
#include "../Common/StyleSheet.h"


/**
 * ArrowButton implementation
 */

ArrowButton::ArrowButton(IconBase icon, QWidget *parent)
    : QToolButton(parent)
    , m_icon(icon) {
    setFixedSize(10, 10);
}

void ArrowButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    
    int length = isDown() ? 6 : 9;
    qreal cord = (width() - length) / 2.0;
    
    drawIcon(m_icon, &painter, QRect(static_cast<int>(cord), static_cast<int>(cord), length, length));
}

/**
 * ScrollBarGroove implementation
 */

ScrollBarGroove::ScrollBarGroove(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
    , m_orientation(orientation)
    , m_upButton(nullptr)
    , m_downButton(nullptr)
    , m_opacityEffect(nullptr)
    , m_opacityAnimation(nullptr) {
    init();
}

void ScrollBarGroove::init() {
    if (m_orientation == Qt::Vertical) {
        setFixedWidth(12);
        m_upButton = new ArrowButton(IconBase::Chevron_Up, this);
        m_downButton = new ArrowButton(IconBase::Chevron_Down, this);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(m_upButton, 0, Qt::AlignHCenter);
        layout->addStretch(1);
        layout->addWidget(m_downButton, 0, Qt::AlignHCenter);
        layout->setContentsMargins(0, 3, 0, 3);
    } else {
        setFixedHeight(12);
        m_upButton = new ArrowButton(IconBase::Chevron_Left, this);
        m_downButton = new ArrowButton(IconBase::Chevron_Right, this);
        
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(m_upButton, 0, Qt::AlignVCenter);
        layout->addStretch(1);
        layout->addWidget(m_downButton, 0, Qt::AlignVCenter);
        layout->setContentsMargins(3, 0, 3, 0);
    }
    
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    setGraphicsEffect(m_opacityEffect);
    m_opacityEffect->setOpacity(0);
}

void ScrollBarGroove::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    
    if (isDarkTheme()) {
        painter.setBrush(QColor(48, 48, 48, 234));
    } else {
        painter.setBrush(QColor(246, 246, 246, 234));
    }
    
    painter.drawRoundedRect(rect(), 6, 6);
}

void ScrollBarGroove::fadeIn() {
    m_opacityAnimation->stop();
    m_opacityAnimation->setEndValue(1.0);
    m_opacityAnimation->setDuration(150);
    m_opacityAnimation->start();
}

void ScrollBarGroove::fadeOut() {
    m_opacityAnimation->stop();
    m_opacityAnimation->setEndValue(0.0);
    m_opacityAnimation->setDuration(150);
    m_opacityAnimation->start();
}

/**
 * ScrollBar implementation
 */

ScrollBarHandle::ScrollBarHandle(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
    , m_orientation(orientation) {
    if (m_orientation == Qt::Vertical) {
        setFixedWidth(3);
    } else {
        setFixedHeight(3);
    }
}

void ScrollBarHandle::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    
    qreal radius = m_orientation == Qt::Vertical ? width() / 2.0 : height() / 2.0;
    QColor color = isDarkTheme() ? QColor(255, 255, 255, 123) : QColor(0, 0, 0, 123);
    painter.setBrush(color);
    painter.drawRoundedRect(rect(), radius, radius);
}

/**
 * ScrollBar implementation
 */

ScrollBar::ScrollBar(Qt::Orientation orientation, QAbstractScrollArea *parent)
    : QWidget(parent)
    , m_orientation(orientation)
    , m_scrollArea(parent)
    , m_groove(nullptr)
    , m_handle(nullptr)
    , m_minimum(0)
    , m_maximum(0)
    , m_value(0)
    , m_pageStep(30)
    , m_singleStep(1)
    , m_padding(15)
    , m_isPressed(false)
    , m_isEnter(false)
    , m_isExpanded(false)
    , m_handlePressed(false)
    , m_pressedValue(0)
    , m_isAlwaysOff(false)
    , m_animation(nullptr)
    , m_timer(nullptr) {
    init();
}

void ScrollBar::setAlwaysOff(bool alwaysOff) {
    m_isAlwaysOff = alwaysOff;
    setVisible(!m_isAlwaysOff && m_maximum > 0);
}

void ScrollBar::init() {
    parent()->installEventFilter(this);
    
    m_timer = new QTimer(this);
    
    // Get parent scroll bar and connect signals
    QScrollBar *parentScrollBar = nullptr;
    if (m_orientation == Qt::Vertical) {
        parentScrollBar = m_scrollArea->verticalScrollBar();
        m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        parentScrollBar = m_scrollArea->horizontalScrollBar();
        m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    
    if (parentScrollBar) {
        connect(parentScrollBar, &QScrollBar::rangeChanged, this, &ScrollBar::setRange);
        connect(parentScrollBar, &QScrollBar::valueChanged, this, &ScrollBar::onParentValueChanged);
        connect(this, &ScrollBar::valueChanged, parentScrollBar, &QScrollBar::setValue);
        
        setRange(parentScrollBar->minimum(), parentScrollBar->maximum());
    }
    
    // Create groove and handle
    m_groove = new ScrollBarGroove(m_orientation, this);
    connect(m_groove->upButton(), &QToolButton::clicked, this, &ScrollBar::onPageUp);
    connect(m_groove->downButton(), &QToolButton::clicked, this, &ScrollBar::onPageDown);
    
    m_handle = new ScrollBarHandle(m_orientation, this);
    
    // Setup animation
    m_animation = new QPropertyAnimation(this, "value", this);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    m_animation->setDuration(369);
    
    // react to groove opacity animation so we can update positions while fading
    if (m_groove && m_groove->opacityAnimation()) {
        connect(m_groove->opacityAnimation(), QOverload<const QVariant &>::of(&QPropertyAnimation::valueChanged),
                this, &ScrollBar::onOpacityChanged);
    }
    
    // Initial positioning
    adjustPos(m_scrollArea->size());
    setVisible(m_maximum > 0);
}

void ScrollBar::setMinimum(int min) {
    if (min == m_minimum) return;
    m_minimum = min;
    emit rangeChanged(m_minimum, m_maximum);
}

void ScrollBar::setMaximum(int max) {
    if (max == m_maximum) return;
    m_maximum = max;
    emit rangeChanged(m_minimum, m_maximum);
}

void ScrollBar::setRange(int min, int max) {
    if (min > max || (min == m_minimum && max == m_maximum)) return;
    
    m_minimum = min;
    m_maximum = max;
    
    adjustHandleSize();
    adjustHandlePos();
    setVisible(max > 0);
    
    emit rangeChanged(min, max);
}

void ScrollBar::setValue(int value) {
    if (value == m_value) return;
    
    int distance = qAbs(value - m_value);
    int duration = qMin(369, qMax(369 / 2, distance * 3));
    
    m_animation->stop();
    m_animation->setDuration(duration);
    m_animation->setStartValue(m_value);
    m_animation->setEndValue(value);
    m_animation->start();
}

void ScrollBar::setValueImmediately(int value) {
    m_animation->stop();
    if (value == m_value) return;
    
    value = qBound(m_minimum, value, m_maximum);
    m_value = value;
    emit valueChanged(value);
    adjustHandlePos();
}

void ScrollBar::setValueProperty(int value) {
    if (value == m_value) return;
    
    value = qBound(m_minimum, value, m_maximum);
    m_value = value;
    emit valueChanged(value);
    adjustHandlePos();
}

void ScrollBar::setPageStep(int step) {
    if (step >= 1) {
        m_pageStep = step;
    }
}

void ScrollBar::setSingleStep(int step) {
    if (step >= 1) {
        m_singleStep = step;
    }
}

bool ScrollBar::isSliderDown() const {
    return m_handlePressed;
}

void ScrollBar::setSliderDown(bool isDown) {
    m_isPressed = isDown;
    m_handlePressed = isDown;
    if (isDown) emit sliderPressed(); else emit sliderReleased();
}

void ScrollBar::adjustHandleSize() {
    if (m_orientation == Qt::Vertical) {
        int total = m_maximum - m_minimum + m_scrollArea->height();
        int s = grooveLength() * m_scrollArea->height() / qMax(total, 1);
        m_handle->setFixedHeight(qMax(30, s));
    } else {
        int total = m_maximum - m_minimum + m_scrollArea->width();
        int s = grooveLength() * m_scrollArea->width() / qMax(total, 1);
        m_handle->setFixedWidth(qMax(30, s));
    }
}

void ScrollBar::adjustHandlePos() {
    int total = qMax(m_maximum - m_minimum, 1);
    int delta = m_value * slideLength() / total;
    
    if (m_orientation == Qt::Vertical) {
        int x = width() - m_handle->width() - 3;
        m_handle->move(x, m_padding + delta);
    } else {
        int y = height() - m_handle->height() - 3;
        m_handle->move(m_padding + delta, y);
    }
}

void ScrollBar::adjustPos(const QSize &size) {
    if (m_orientation == Qt::Vertical) {
        resize(12, size.height() - 2);
        move(size.width() - 13, 1);
    } else {
        resize(size.width() - 2, 12);
        move(1, size.height() - 13);
    }
}

int ScrollBar::grooveLength() const {
    if (m_orientation == Qt::Vertical) {
        return height() - 2 * m_padding;
    }
    return width() - 2 * m_padding;
}

int ScrollBar::slideLength() const {
    if (m_orientation == Qt::Vertical) {
        return grooveLength() - m_handle->height();
    }
    return grooveLength() - m_handle->width();
}

void ScrollBar::expand() {
    if (m_isExpanded || !m_isEnter) return;
    m_isExpanded = true;
    m_groove->fadeIn();
}

void ScrollBar::collapse() {
    if (!m_isExpanded || m_isEnter) return;
    m_isExpanded = false;
    m_groove->fadeOut();
}

void ScrollBar::jumpToPosition(const QPoint &pos) {
    int value = 0;
    
    if (m_orientation == Qt::Vertical) {
        int posY = qBound(0, pos.y() - m_padding, grooveLength());
        if (posY > m_handle->y() + m_handle->height() / 2) {
            posY -= m_handle->height();
        }
        value = posY * m_maximum / qMax(slideLength(), 1);
    } else {
        int posX = qBound(0, pos.x() - m_padding, grooveLength());
        if (posX > m_handle->x() + m_handle->width() / 2) {
            posX -= m_handle->width();
        }
        value = posX * m_maximum / qMax(slideLength(), 1);
    }
    
    setValue(value);
}

void ScrollBar::onPageUp() {
    setValue(m_value - m_pageStep);
}

void ScrollBar::onPageDown() {
    setValue(m_value + m_pageStep);
}

void ScrollBar::onParentValueChanged(int value) {
    setValueProperty(value);
}

void ScrollBar::onOpacityChanged() {
    // When the groove fades in/out, make sure the handle position is adjusted
    adjustHandlePos();
}

void ScrollBar::enterEvent(QEnterEvent *event) {
    m_isEnter = true;
    m_timer->stop();
    QTimer::singleShot(210, this, &ScrollBar::expand);
    QWidget::enterEvent(event);
}

void ScrollBar::leaveEvent(QEvent *event) {
    m_isEnter = false;
    m_timer->stop();
    QTimer::singleShot(210, this, &ScrollBar::collapse);
    QWidget::leaveEvent(event);
}

void ScrollBar::mousePressEvent(QMouseEvent *event) {
    m_animation->stop();
    m_isPressed = true;
    m_pressedPos = event->pos();
    // detect whether the handle itself was pressed
    m_handlePressed = (childAt(event->pos()) == m_handle);

    if (!m_handlePressed && (m_padding <= event->pos().x() && event->pos().x() <= width() - m_padding || m_padding <= event->pos().y() && event->pos().y() <= height() - m_padding)) {
        // click on groove -> jump (animated)
        jumpToPosition(event->pos());
    } else if (m_handlePressed) {
        // prepare for dragging
        m_pressedValue = m_value;
        emit sliderPressed();
    }

    QWidget::mousePressEvent(event);
}

void ScrollBar::mouseReleaseEvent(QMouseEvent *event) {
    m_isPressed = false;
    if (m_handlePressed) {
        emit sliderReleased();
        m_handlePressed = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void ScrollBar::mouseMoveEvent(QMouseEvent *event) {
    if (!m_isPressed || !m_handlePressed) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    // dragging: compute delta in value units from pixel movement
    int total = qMax(m_maximum - m_minimum, 1);
    int deltaPixels = (m_orientation == Qt::Vertical) ? (event->pos().y() - m_pressedPos.y()) : (event->pos().x() - m_pressedPos.x());
    int slideLen = qMax(1, slideLength());
    int deltaValue = deltaPixels * total / slideLen;

    int newValue = m_pressedValue + deltaValue;
    newValue = qMax(m_minimum, qMin(m_maximum, newValue));
    setValueImmediately(newValue);
    emit sliderMoved();

    QWidget::mouseMoveEvent(event);
}

void ScrollBar::resizeEvent(QResizeEvent *event) {
    m_groove->resize(size());
    QWidget::resizeEvent(event);
}

bool ScrollBar::eventFilter(QObject *watched, QEvent *event) {
    if (watched != parent()) {
        return QWidget::eventFilter(watched, event);
    }
    
    if (event->type() == QEvent::Resize) {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        adjustPos(resizeEvent->size());
    }
    
    return QWidget::eventFilter(watched, event);
}

/**
 * ScrollDelegate implementation
 */

ScrollDelegate::ScrollDelegate(QAbstractScrollArea *parentArea)
    : QObject(parentArea)
    , m_parentArea(parentArea)
    , m_vScrollBar(nullptr)
    , m_hScrollBar(nullptr) {
    // create custom scrollbars with the scroll area as parent
    m_vScrollBar = new ScrollBar(Qt::Vertical, m_parentArea);
    m_hScrollBar = new ScrollBar(Qt::Horizontal, m_parentArea);

    // forward wheel events from viewport to our scrollbars
    if (m_parentArea->viewport()) {
        m_parentArea->viewport()->installEventFilter(this);
    }
}

ScrollBar* ScrollDelegate::verticalScrollBar() const {
    return m_vScrollBar;
}

ScrollBar* ScrollDelegate::horizontalScrollBar() const {
    return m_hScrollBar; 
}

void ScrollDelegate::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    if (m_vScrollBar) m_vScrollBar->setAlwaysOff(policy == Qt::ScrollBarAlwaysOff);
}

void ScrollDelegate::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    if (m_hScrollBar) m_hScrollBar->setAlwaysOff(policy == Qt::ScrollBarAlwaysOff);
}

bool ScrollDelegate::eventFilter(QObject *obj, QEvent *event) {
    if (obj != m_parentArea->viewport()) {
        return QObject::eventFilter(obj, event);
    }

    if (event->type() == QEvent::Wheel) {
        QWheelEvent *we = static_cast<QWheelEvent*>(event);
        int delta = we->angleDelta().y() ? we->angleDelta().y() : we->angleDelta().x();
        if (delta == 0) return QObject::eventFilter(obj, event);

        // prefer vertical scrolling when possible
        if (m_vScrollBar && m_vScrollBar->isVisible()) {
            int newValue = m_vScrollBar->value() - delta;
            newValue = qMax(m_vScrollBar->minimum(), qMin(m_vScrollBar->maximum(), newValue));
            m_vScrollBar->setValue(newValue);
            return true;
        }

        if (m_hScrollBar && m_hScrollBar->isVisible()) {
            int newValue = m_hScrollBar->value() - delta;
            newValue = qMax(m_hScrollBar->minimum(), qMin(m_hScrollBar->maximum(), newValue));
            m_hScrollBar->setValue(newValue);
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

/**
 * ScrollAreaBase implementation
 */

ScrollAreaBase::ScrollAreaBase(QWidget *parent)
    : QScrollArea(parent)
    , m_verticalScrollBar(nullptr)
    , m_horizontalScrollBar(nullptr) {
    init();
}

void ScrollAreaBase::init() {
    setWidgetResizable(true);
    
    // Create scroll delegate which owns custom scroll bars and handles wheel events
    m_delegate = new ScrollDelegate(this);
    m_verticalScrollBar = m_delegate->verticalScrollBar();
    m_horizontalScrollBar = m_delegate->horizontalScrollBar();

    StyleSheetBase::apply(this, StyleSheetBase::ScrollArea);
}

/**
 * VerticalScrollArea implementation
 */

VerticalScrollArea::VerticalScrollArea(QWidget *parent)
    : ScrollAreaBase(parent) {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

/**
 * HorizontalScrollArea implementation
 */

HorizontalScrollArea::HorizontalScrollArea(QWidget *parent)
    : ScrollAreaBase(parent) {
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

// ScrollAreaBase helper implementations
void ScrollAreaBase::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    // Always disable native scrollbars on the QAbstractScrollArea and manage our custom one
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (m_delegate) m_delegate->setVerticalScrollBarPolicy(policy);
}

void ScrollAreaBase::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (m_delegate) m_delegate->setHorizontalScrollBarPolicy(policy);
}

void ScrollAreaBase::setWidget(QWidget *widget) {
    // delete previous widget
    if (this->widget() != nullptr) {
        this->widget()->deleteLater();
    }
    QScrollArea::setWidget(widget);
}

void ScrollAreaBase::resizeEvent(QResizeEvent *event) {
    emit viewportSizeChanged(viewport()->size());
    QScrollArea::resizeEvent(event);
}

void ScrollAreaBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
}

void ScrollAreaBase::setTransparent(bool transparent) {
    setProperty("isTransparent", transparent);
}

void ScrollAreaBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}