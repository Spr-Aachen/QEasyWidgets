#ifndef QEASYWIDGETS_BROWSER_H
#define QEASYWIDGETS_BROWSER_H

#include <QTextBrowser>
#include "Widget.h"


namespace QEW {

/**
 * @brief Enhanced text browser with theme support
 */
class TextBrowserBase : public SizableWidget<QTextBrowser>
{
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit TextBrowserBase(QWidget *parent = nullptr);
    ~TextBrowserBase() override = default;

    void loadMarkdown(const QString &filePath);
    void loadHtml(const QString &filePath);

    void setBorderless(bool borderless);

    void clearDefaultStyleSheet();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_BROWSER_H
