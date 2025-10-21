#ifndef QEASYWIDGETS_WIDGET_H
#define QEASYWIDGETS_WIDGET_H

#include <QWidget>


namespace QEW {

/**
 * @brief Mixin class providing size properties for widgets
 * 
 * This template class adds currentWidth and currentHeight properties
 * to any Qt widget class, similar to Python's SizableWidget mixin.
 */
template<typename BaseWidget>
class SizableWidget : public BaseWidget
{
public:
    using BaseWidget::BaseWidget;  // Inherit constructors

    int currentWidth() const {
        return BaseWidget::width();
    }

    void setCurrentWidth(int width) {
        BaseWidget::setFixedWidth(width);
    }

    int currentHeight() const {
        return BaseWidget::height();
    }

    void setCurrentHeight(int height) {
        BaseWidget::setFixedHeight(height);
    }
};

/**
 * @brief Enhanced widget with theme support and size properties
 */
class WidgetBase : public SizableWidget<QWidget>
{
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit WidgetBase(QWidget *parent = nullptr);
    ~WidgetBase() override = default;

signals:
    void resized();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

} // namespace QEW

#endif // QEASYWIDGETS_WIDGET_H
