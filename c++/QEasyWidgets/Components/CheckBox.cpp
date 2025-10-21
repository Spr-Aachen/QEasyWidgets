#include "CheckBox.h"

#include <QFont>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>

#include "../Common/StyleSheet.h"
#include "../Common/Theme.h"


namespace QEW {

// Indicator implementation
Indicator::Indicator(QWidget *parent)
    : QPushButton(parent)
    , m_isChecked(false)
    , m_isDown(false)
    , m_isHover(false)
    , m_ellipseCordX(0.0f)
    , m_ellipseCordX_Default(6.0f)
    , m_ellipseLength(12)
    , m_width(48)
    , m_height(24)
    , m_ellipseAnimation(nullptr)
{
    init();
}

void Indicator::init()
{
    setCheckable(true);
    setFixedSize(m_width, m_height);

    m_ellipseCordX = m_ellipseCordX_Default;

    m_ellipseAnimation = new QPropertyAnimation(this, "ellipseCordX", this);
    m_ellipseAnimation->setDuration(123);

    connect(this, &QPushButton::toggled, this, [this](bool checked) {
        m_ellipseAnimation->stop();
        float endValue = checked ? (m_width - m_ellipseLength - m_ellipseCordX_Default) : m_ellipseCordX_Default;
        m_ellipseAnimation->setEndValue(endValue);
        m_ellipseAnimation->start();
    });

    StyleSheetBase::apply(this, StyleSheetBase::CheckBox);
}

bool Indicator::isChecked() const
{
    return m_isChecked;
}

void Indicator::setChecked(bool checked)
{
    if (m_isChecked != checked) {
        m_isChecked = checked;
        QPushButton::setChecked(checked);
        emit toggled(checked);
        update();
    }
}

void Indicator::toggle()
{
    setChecked(!m_isChecked);
}

float Indicator::ellipseCordX() const
{
    return m_ellipseCordX;
}

void Indicator::setEllipseCordX(float x)
{
    m_ellipseCordX = qMax(x, m_ellipseCordX_Default);
    update();
}

void Indicator::setDown(bool isDown)
{
    m_isDown = isDown;
    QPushButton::setDown(isDown);
}

void Indicator::setHover(bool isHover)
{
    m_isHover = isHover;
    update();
}

void Indicator::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);
    emit toggled(m_isChecked);
}

void Indicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Draw background
    float radius = m_height / 2.0f;
    QColor backgroundColor = m_isChecked ? getThemeColor(ThemeColor::Default) : currentColor();
    painter.setPen(backgroundColor);
    painter.setBrush(m_isChecked ? getThemeColor(ThemeColor::Default) : (isDarkTheme() ? QColor(Qt::white) : QColor(Qt::black)));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), radius, radius);

    // Draw circle
    painter.setPen(Qt::NoPen);
    QColor circleColor = m_isChecked ? (isDarkTheme() ? QColor(Qt::black) : QColor(Qt::white)) : currentColor();
    painter.setBrush(circleColor);
    painter.drawEllipse(QRectF(m_ellipseCordX, (m_height - m_ellipseLength) / 2.0f, m_ellipseLength, m_ellipseLength));
}

// CheckBoxBase implementation

CheckBoxBase::CheckBoxBase(QWidget *parent)
    : QWidget(parent)
    , m_indicator(nullptr)
    , m_label(nullptr)
    , m_layout(nullptr)
    , m_spacing(12)
{
    init();
}

CheckBoxBase::CheckBoxBase(const QString &text, QWidget *parent)
    : CheckBoxBase(parent)
{
    setText(text);
}

void CheckBoxBase::init()
{
    QFont font = this->font();
    font.setPointSize(15);
    setFont(font);

    m_indicator = new Indicator(this);
    connect(m_indicator, &Indicator::toggled, this, &CheckBoxBase::toggled);

    m_label = new QLabel(this);

    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(3, 0, 0, 0);
    m_layout->setSpacing(m_spacing);
    m_layout->addWidget(m_indicator);
    m_layout->addWidget(m_label);
    m_layout->setAlignment(Qt::AlignLeft);

    setMinimumSize(m_layout->totalSizeHint());

    StyleSheetBase::apply(this, StyleSheetBase::CheckBox);
}

QString CheckBoxBase::text() const
{
    return m_label ? m_label->text() : QString();
}

void CheckBoxBase::setText(const QString &text)
{
    if (m_label) {
        m_label->setText(text);
        adjustSize();
    }
}

bool CheckBoxBase::isChecked() const
{
    return m_indicator ? m_indicator->isChecked() : false;
}

void CheckBoxBase::setChecked(bool checked)
{
    if (m_indicator) {
        m_indicator->setChecked(checked);
    }
}

int CheckBoxBase::spacing() const
{
    return m_spacing;
}

void CheckBoxBase::setSpacing(int spacing)
{
    m_spacing = spacing;
    if (m_layout) {
        m_layout->setSpacing(spacing);
        update();
    }
}

bool CheckBoxBase::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this && isEnabled()) {
        if (event->type() == QEvent::MouseButtonPress) {
            m_indicator->setDown(true);
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_indicator->setDown(false);
            m_indicator->toggle();
        } else if (event->type() == QEvent::Enter) {
            m_indicator->setHover(true);
        } else if (event->type() == QEvent::Leave) {
            m_indicator->setHover(false);
        }
    }
    return QWidget::eventFilter(watched, event);
}

} // namespace QEW