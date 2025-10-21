#ifndef QEASYWIDGETS_LABEL_H
#define QEASYWIDGETS_LABEL_H

#include <QLabel>
#include <QColor>

#include "Widget.h"


/**
 * Enhanced label with theme support
 */
class LabelBase : public SizableWidget<QLabel> {
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit LabelBase(QWidget *parent = nullptr);
    explicit LabelBase(const QString &text, QWidget *parent = nullptr);
    ~LabelBase() override = default;

    void setCustomTextColor(const QColor &light, const QColor &dark);

protected:
    void updateTextColor();

private:
    QColor m_lightTextColor;
    QColor m_darkTextColor;
};


#endif // QEASYWIDGETS_LABEL_H