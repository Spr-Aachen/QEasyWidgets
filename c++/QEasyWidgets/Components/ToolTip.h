#ifndef QEASYWIDGETS_TOOLTIP_H
#define QEASYWIDGETS_TOOLTIP_H

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>
#include <QEvent>
#include <QObject>
#include <QPropertyAnimation>

#include "../Common/Config.h"
#include "../Common/StyleSheet.h"
#include "../Common/QFunctions.h"


/**
 * Base class for toolTip components
 */
class ToolTipBase : public QFrame {
    Q_OBJECT

public:
    explicit ToolTipBase(QWidget *parent = nullptr, const QString &text = QString());
    ~ToolTipBase() override = default;

    QString text() const;
    void setText(const QString &text);
    
    int duration() const;
    void setDuration(int duration);

    void showText(const QPoint &pos, const QString &text);
    void showText(int x, int y, const QString &text);
    void hideText();
    
    void adjustPos(QWidget *widget, Position position);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    QString m_text;
    int m_duration;
    QTimer *m_timer;
    QLabel *m_label;
    QPropertyAnimation *m_opacityAnim;
};


/**
 * Event filter for toolTip functionality
 */
class ToolTipEventFilter : public QObject {
    Q_OBJECT

public:
    explicit ToolTipEventFilter(QWidget *parent = nullptr, Position position = Position::Top, int delay = 333);
    explicit ToolTipEventFilter(QWidget *parent, ToolTipBase *toolTip, int delay = 333);
    ~ToolTipEventFilter() override = default;

    bool eventFilter(QObject *obj, QEvent *event) override;
    
    void setToolTipDelay(int delay);

private slots:
    void showToolTip();
    void hideToolTip();

private:
    void init(QWidget *parent, int delay);
    
    int m_showDelay;
    bool m_isEnter;
    Position m_position;
    QTimer *m_timer;
    ToolTipBase *m_toolTip;
};


#endif // QEASYWIDGETS_TOOLTIP_H