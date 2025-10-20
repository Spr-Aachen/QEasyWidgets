#ifndef QEASYWIDGETS_MENU_H
#define QEASYWIDGETS_MENU_H

#include <QMenu>


namespace QEW {

/**
 * @brief Enhanced menu with theme support
 */
class MenuBase : public QMenu
{
    Q_OBJECT

public:
    explicit MenuBase(QWidget *parent = nullptr);
    ~MenuBase() override = default;

    void clearDefaultStyleSheet();

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_MENU_H
