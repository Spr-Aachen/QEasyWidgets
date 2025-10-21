#ifndef QEASYWIDGETS_TOOLBOX_H
#define QEASYWIDGETS_TOOLBOX_H

#include <QToolBox>


namespace QEW {

/**
 * @brief Enhanced tool box with theme support
 */
class ToolBoxBase : public QToolBox
{
    Q_OBJECT

public:
    explicit ToolBoxBase(QWidget *parent = nullptr);
    ~ToolBoxBase() override = default;

    void clearDefaultStyleSheet();

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_TOOLBOX_H
