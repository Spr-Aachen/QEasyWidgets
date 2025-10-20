#ifndef QEASYWIDGETS_GROUPBOX_H
#define QEASYWIDGETS_GROUPBOX_H

#include <QGroupBox>
#include "Widget.h"


namespace QEW {

/**
 * @brief Enhanced group box with theme support and expand/collapse functionality
 */
class GroupBoxBase : public SizableWidget<QGroupBox>
{
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit GroupBoxBase(QWidget *parent = nullptr);
    explicit GroupBoxBase(const QString &title, QWidget *parent = nullptr);
    ~GroupBoxBase() override = default;

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);

    void clearDefaultStyleSheet();

public slots:
    void expand();
    void collapse();

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_GROUPBOX_H
