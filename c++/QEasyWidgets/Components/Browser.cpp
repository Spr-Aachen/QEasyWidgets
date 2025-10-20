#include "Browser.h"

#include <QFile>
#include <QTextStream>

#include "../Common/StyleSheet.h"
#include "Menu.h"


namespace QEW {

TextBrowserBase::TextBrowserBase(QWidget *parent)
    : SizableWidget<QTextBrowser>(parent)
{
    init();
}

void TextBrowserBase::init()
{
    StyleSheetBase::apply(this, StyleSheetBase::Browser);
}

void TextBrowserBase::loadMarkdown(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString markdown = in.readAll();
        setMarkdown(markdown);
        file.close();
    }
}

void TextBrowserBase::loadHtml(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString html = in.readAll();
        setHtml(html);
        file.close();
    }
}

void TextBrowserBase::setBorderless(bool borderless)
{
    setProperty("isBorderless", borderless);
}

void TextBrowserBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

void TextBrowserBase::contextMenuEvent(QContextMenuEvent *event)
{
    MenuBase menu(this);
    menu.exec(event->globalPos());
}

} // namespace QEW
