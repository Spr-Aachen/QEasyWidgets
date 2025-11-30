#include "ToolTip.h"

#include <QApplication>
#include <QScreen>
#include <QCursor>


ToolTipBase::ToolTipBase(QWidget *parent, const QString &text)
    : QFrame(parent)
    , m_text(text)
    , m_duration(333)
    , m_timer(nullptr)
    , m_container(nullptr)
    , m_containerLayout(nullptr)
    , m_label(nullptr)
    , m_opacityAnim(nullptr) {
    
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ToolTipBase::hide);
    
    // Set layout
    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(12, 8, 12, 12);
    
    // Set container
    m_container = createContainer();
    m_containerLayout = new QHBoxLayout(m_container);
    layout()->addWidget(m_container);
    m_label = new QLabel(text, this);
    m_containerLayout->addWidget(m_label);
    m_containerLayout->setContentsMargins(8, 6, 8, 6);
    
    // Add shadow
    setDropShadowEffect(this, 21.0, QColor(0, 0, 0, 60), 0.0, 3.0);
    
    // Add opacity effect
    m_opacityAnim = setOpacityEffect(this);
    
    // Set style
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setQSS();
}


QFrame* ToolTipBase::createContainer() {
    return new QFrame(this);
}


void ToolTipBase::setQSS() {
    m_container->setObjectName("container");
    m_label->setObjectName("contentLabel");
    StyleSheetBase::apply(this, StyleSheetBase::ToolTip);
    m_label->adjustSize();
    adjustSize();
}


void ToolTipBase::showEvent(QShowEvent *event) {
    m_opacityAnim->setStartValue(0);
    m_opacityAnim->setEndValue(1);
    m_opacityAnim->start();
    m_timer->stop();
    if (duration() > 0) {
        m_timer->start(m_duration + m_opacityAnim->duration());
    }
    QFrame::showEvent(event);
}


void ToolTipBase::hideEvent(QHideEvent *event) {
    m_timer->stop();
    QFrame::hideEvent(event);
}


QString ToolTipBase::text() const {
    return m_text;
}


void ToolTipBase::setText(const QString &text) {
    m_text = text;
    m_label->setText(text);
    m_container->adjustSize();
    adjustSize();
}


int ToolTipBase::duration() const {
    return m_duration;
}


void ToolTipBase::setDuration(int duration) {
    m_duration = duration;
}


void ToolTipBase::showText(const QPoint &pos, const QString &text) {
    move(pos);
    setText(text);
    show();
}


void ToolTipBase::showText(int x, int y, const QString &text) {
    move(x, y);
    setText(text);
    show();
}


void ToolTipBase::hideText() {
    hide();
}


void ToolTipBase::adjustPos(QWidget *widget, Position position) {
    QPoint pos;
    int x = 0, y = 0;
    
    switch (position) {
        case Position::Top:
            pos = widget->mapToGlobal(QPoint());
            x = pos.x() + widget->width() / 2 - width() / 2;
            y = pos.y() - height();
            break;
        case Position::Bottom:
            pos = widget->mapToGlobal(QPoint());
            x = pos.x() + widget->width() / 2 - width() / 2;
            y = pos.y() + widget->height();
            break;
        case Position::Left:
            pos = widget->mapToGlobal(QPoint());
            x = pos.x() - width();
            y = pos.y() + (widget->height() - height()) / 2;
            break;
        case Position::Right:
            pos = widget->mapToGlobal(QPoint());
            x = pos.x() + widget->width();
            y = pos.y() + (widget->height() - height()) / 2;
            break;
        case Position::TopLeft:
            pos = widget->mapToGlobal(QPoint());
            x = pos.x() - layout()->contentsMargins().left();
            y = pos.y() - height();
            break;
        case Position::TopRight:
            pos = widget->mapToGlobal(QPoint());
            x = pos.x() + widget->width() - width() + layout()->contentsMargins().right();
            y = pos.y() - height();
            break;
        case Position::BottomLeft:
            pos = widget->mapToGlobal(QPoint());
            x = pos.x() - layout()->contentsMargins().left();
            y = pos.y() + widget->height();
            break;
        case Position::BottomRight:
            pos = widget->mapToGlobal(QPoint());
            x = pos.x() + widget->width() - width() + layout()->contentsMargins().right();
            y = pos.y() + widget->height();
            break;
    }
    
    QRect screenRect = getScreenGeometry(getCurrentScreen());
    x = qMax(screenRect.left(), qMin(x, screenRect.right() - width() - 4));
    y = qMax(screenRect.top(), qMin(y, screenRect.bottom() - height() - 4));
    move(x, y);
}


// ToolTipEventFilter implementation
ToolTipEventFilter::ToolTipEventFilter(QWidget *parent, Position position, int delay)
    : QObject(parent)
    , m_showDelay(delay)
    , m_isEnter(false)
    , m_position(position)
    , m_timer(nullptr)
    , m_toolTip(nullptr) {
    init(parent, delay);
    m_position = position;
    m_toolTip = new ToolTipBase(parent->window(), parent->toolTip());
}


ToolTipEventFilter::ToolTipEventFilter(QWidget *parent, ToolTipBase *toolTip, int delay)
    : QObject(parent)
    , m_showDelay(delay)
    , m_isEnter(false)
    , m_timer(nullptr)
    , m_toolTip(nullptr) {
    init(parent, delay);
    m_toolTip = toolTip ? toolTip : new ToolTipBase(parent->window(), parent->toolTip());
}


void ToolTipEventFilter::init(QWidget *parent, int delay) {
    m_showDelay = delay;
    m_isEnter = false;
    
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ToolTipEventFilter::showToolTip);
}


bool ToolTipEventFilter::eventFilter(QObject *obj, QEvent *event) {
    Q_UNUSED(obj)
    
    switch (event->type()) {
        case QEvent::ToolTip:
            return true;
        case QEvent::Hide:
        case QEvent::Leave:
            hideToolTip();
            break;
        case QEvent::Enter:
            m_isEnter = true;
            if (QWidget *parent = qobject_cast<QWidget*>(this->parent())) {
                if (!parent->toolTip().isEmpty() && parent->isEnabled()) {
                    int t = parent->toolTipDuration() > 0 ? parent->toolTipDuration() : -1;
                    m_toolTip->setDuration(t);
                    m_timer->start(m_showDelay);
                }
            }
            break;
        case QEvent::MouseButtonPress:
            hideToolTip();
            break;
        default:
            break;
    }
    
    return QObject::eventFilter(obj, event);
}


void ToolTipEventFilter::showToolTip() {
    if (!m_isEnter) return;
    
    if (QWidget *parent = qobject_cast<QWidget*>(this->parent())) {
        m_toolTip->setText(parent->toolTip());
        m_toolTip->adjustPos(parent, m_position);
        m_toolTip->show();
    }
}


void ToolTipEventFilter::hideToolTip() {
    m_isEnter = false;
    m_timer->stop();
    if (m_toolTip) {
        m_toolTip->hide();
    }
}


void ToolTipEventFilter::setToolTipDelay(int delay) {
    m_showDelay = delay;
}