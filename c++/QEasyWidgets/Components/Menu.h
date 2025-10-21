#ifndef QEASYWIDGETS_MENU_H
#define QEASYWIDGETS_MENU_H

#include <QMenu>


/**
 * Enhanced menu with theme support
 */
class MenuBase : public QMenu {
    Q_OBJECT

public:
    explicit MenuBase(QWidget *parent = nullptr);
    ~MenuBase() override = default;

    void clearDefaultStyleSheet();

private:
    void init();
};


#endif // QEASYWIDGETS_MENU_H