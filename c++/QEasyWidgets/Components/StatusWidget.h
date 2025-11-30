#ifndef QEASYWIDGETS_STATUSWIDGET_H
#define QEASYWIDGETS_STATUSWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QColor>
#include <QPointF>
#include <QList>

#include "../Common/Config.h"


/**
 * Forward declaration
 */
class QGridLayout;
class QSize;
class QString;
class QPaintEvent;
class QResizeEvent;


/**
 * Loading status indicator widget
 */
class LoadingStatus : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingStatus(QWidget *parent = nullptr);
    ~LoadingStatus() override = default;

    void setDotCount(int count);
    void setDotColor(const QColor &color);
    void setInterval(int interval);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateGeometry();

    int m_dotCount;
    QColor m_dotColor;
    int m_interval;
    QTimer *m_timer;
    int m_currentStep;

    QList<QPointF> m_locations;
    QList<qreal> m_radii;
};


/**
 * Status widget base with indicator support
 */
class StatusWidgetBase : public QWidget
{
    Q_OBJECT

public:
    explicit StatusWidgetBase(const Status status = Status::Loading, QWidget *parent = nullptr);
    explicit StatusWidgetBase(const Status status, const QSize &size, QWidget *parent = nullptr);
    ~StatusWidgetBase() override = default;

    void setStatus(const Status status);

private:
    QWidget *m_currentStatus = nullptr;
    QGridLayout *m_layout = nullptr;
};


#endif // QEASYWIDGETS_STATUSWIDGET_H