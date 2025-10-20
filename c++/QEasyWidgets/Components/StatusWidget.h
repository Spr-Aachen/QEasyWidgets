#ifndef QEASYWIDGETS_STATUSWIDGET_H
#define QEASYWIDGETS_STATUSWIDGET_H

#include <QWidget>
#include <QTimer>


namespace QEW {

/**
 * @brief Loading status widget with animated dots
 */
class StatusWidgetBase : public QWidget
{
    Q_OBJECT

public:
    explicit StatusWidgetBase(QWidget *parent = nullptr);
    ~StatusWidgetBase() override = default;

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

} // namespace QEW

#endif // QEASYWIDGETS_STATUSWIDGET_H
