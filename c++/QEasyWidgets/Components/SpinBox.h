#ifndef QEASYWIDGETS_SPINBOX_H
#define QEASYWIDGETS_SPINBOX_H

#include <QSpinBox>
#include <QDoubleSpinBox>


namespace QEW {

/**
 * @brief Enhanced spin box with theme support
 */
class SpinBoxBase : public QSpinBox
{
    Q_OBJECT

public:
    explicit SpinBoxBase(QWidget *parent = nullptr);
    ~SpinBoxBase() override = default;

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);

    void clearDefaultStyleSheet();

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    void init();
};

/**
 * @brief Enhanced double spin box with theme support
 */
class DoubleSpinBoxBase : public QDoubleSpinBox
{
    Q_OBJECT

public:
    explicit DoubleSpinBoxBase(QWidget *parent = nullptr);
    ~DoubleSpinBoxBase() override = default;

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);

    void clearDefaultStyleSheet();

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_SPINBOX_H
