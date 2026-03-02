#include "Browser.h"

#include <QFile>
#include <QTextStream>
#include <QContextMenuEvent>
#include <QFileInfo>
#include <QUrl>

#include "../Common/StyleSheet.h"
#include "Menu.h"


/**
 * TextBrowserBase implementation
 */

TextBrowserBase::TextBrowserBase(QWidget *parent)
    : SizableWidget<QTextBrowser>(parent)
    , m_delegate(nullptr) {
    init();
}

void TextBrowserBase::init() {
    m_delegate = new ScrollDelegate(this);
    document()->setDocumentMargin(0);
    document()->setDefaultStyleSheet("img { width: 100%; }");
    StyleSheetBase::apply(this, StyleSheetBase::Browser);
}

void TextBrowserBase::updateGeometries() {
    SizableWidget<QTextBrowser>::updateGeometries();
    if (m_delegate) {
        m_delegate->updateViewportMargins();
        // Ensure document width matches viewport to prevent horizontal scrollbars with img { width: 100%; }
        document()->setTextWidth(viewport()->width());
    }
}

void TextBrowserBase::contextMenuEvent(QContextMenuEvent *event) {
    MenuBase menu(this);
    menu.exec(event->globalPos());
}

void TextBrowserBase::loadMarkdown(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString markdown = in.readAll();
        QFileInfo fileInfo(filePath);
        document()->setBaseUrl(QUrl::fromLocalFile(fileInfo.absolutePath() + "/"));
        setMarkdown(markdown);
        file.close();
    }
}

void TextBrowserBase::loadHtml(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString html = in.readAll();
        QFileInfo fileInfo(filePath);
        document()->setBaseUrl(QUrl::fromLocalFile(fileInfo.absolutePath() + "/"));
        setHtml(html);
        file.close();
    }
}

void TextBrowserBase::setMaximumLines(int maxLines) {
    QTextDocument *document = this->document();
    document->setMaximumBlockCount(maxLines);
}

void TextBrowserBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
}

void TextBrowserBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}