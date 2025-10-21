#ifndef QEASYWIDGETS_WIDGET_H
#define QEASYWIDGETS_WIDGET_H

#include <QWidget>


/**
 * Mixin class providing size properties for widgets
 */
template<typename BaseWidget>
class SizableWidget : public BaseWidget {
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
 * Enhanced widget with theme support and size properties
 */
class WidgetBase : public SizableWidget<QWidget> {
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


#endif // QEASYWIDGETS_WIDGET_H