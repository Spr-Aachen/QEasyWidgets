#include "StatusWidget.h"

#include <QPainter>
#include <QtMath>
#include <QGridLayout>
#include <QSize>
#include <QString>
#include <QPaintEvent>
#include <QResizeEvent>


/**
 * LoadingStatus implementation
 */

LoadingStatus::LoadingStatus(QWidget *parent)
    : QWidget(parent)
    , m_dotCount(12)
    , m_dotColor(48, 177, 222)
    , m_interval(50)
    , m_timer(nullptr)
    , m_currentStep(0) {
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_timer = new QTimer(this);
    m_timer->setInterval(m_interval);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_currentStep = (m_currentStep + 1) % m_dotCount;
        update();
    });
    m_timer->start();

    updateGeometry();
}

void LoadingStatus::setDotCount(int count) {
    m_dotCount = count;
    updateGeometry();
    update();
}

void LoadingStatus::setDotColor(const QColor &color) {
    m_dotColor = color;
    update();
}

void LoadingStatus::setInterval(int interval) {
    m_interval = interval;
    if (m_timer) {
        m_timer->setInterval(m_interval);
    }
}

void LoadingStatus::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    qreal squareWidth = qMin(width(), height());
    qreal maxDiameter = squareWidth / 6.0;
    qreal minDiameter = maxDiameter - squareWidth / 12.0;

    for (int i = 0; i < m_dotCount; ++i) {
        int step = (i + m_currentStep) % m_dotCount;
        qreal radius = maxDiameter / 2.0 - step * (maxDiameter - minDiameter) / (m_dotCount - 1) / 2.0;

        QColor color = m_dotColor;
        color.setAlphaF(1.0 - static_cast<qreal>(step) / m_dotCount);

        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawEllipse(m_locations[i], radius, radius);
    }
}

void LoadingStatus::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateGeometry();
}

void LoadingStatus::updateGeometry() {
    qreal squareWidth = qMin(width(), height());
    qreal maxDiameter = squareWidth / 6.0;
    qreal minDiameter = maxDiameter - squareWidth / 12.0;
    qreal half = squareWidth / 2.0;
    qreal centerDistance = half - maxDiameter / 2.0 - 1.0;
    qreal angleGap = 360.0 / m_dotCount;

    m_locations.clear();
    m_radii.clear();

    for (int i = 0; i < m_dotCount; ++i) {
        qreal radian = qDegreesToRadians(-angleGap * i);
        QPointF location(half + centerDistance * qCos(radian), half - centerDistance * qSin(radian));
        m_locations.append(location);
        m_radii.append(maxDiameter / 2.0 - i * (maxDiameter - minDiameter) / (m_dotCount - 1) / 2.0);
    }
}

StatusWidgetBase::StatusWidgetBase(const QString &status, const QSize &size, QWidget *parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    setStatus(status);

    setFixedSize(size);
}

StatusWidgetBase::StatusWidgetBase(const QString &status, QWidget *parent)
    : StatusWidgetBase(status, QSize(24, 24), parent) {
}

void StatusWidgetBase::setStatus(const QString &status) {
    if (m_currentStatus) {
        m_layout->removeWidget(m_currentStatus);
        m_currentStatus->deleteLater();
        m_currentStatus = nullptr;
    }

    if (status == Status::Loading) {
        m_currentStatus = new LoadingStatus(this);
    }

    if (m_currentStatus) {
        m_layout->addWidget(m_currentStatus);
    }
}