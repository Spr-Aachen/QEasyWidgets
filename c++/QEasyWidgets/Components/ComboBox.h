#ifndef QEASYWIDGETS_COMBOBOX_H
#define QEASYWIDGETS_COMBOBOX_H

#include <QComboBox>


/**
 * Forward declaration
 */
class QWheelEvent;


/**
 * Enhanced combo box with theme support
 */
class ComboBoxBase : public QComboBox {
    Q_OBJECT

public:
    explicit ComboBoxBase(QWidget *parent = nullptr);
    ~ComboBoxBase() override = default;

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);

    void clearDefaultStyleSheet();

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    void init();
};


#endif // QEASYWIDGETS_COMBOBOX_H