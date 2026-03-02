#include "Edit.h"

#include <QMimeData>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>

#include "../Common/StyleSheet.h"
#include "Menu.h"


/**
 * LineEditBase implementation
 */

LineEditBase::LineEditBase(QWidget *parent)
    : QLineEdit(parent)
    , m_clearButton(nullptr)
    , m_fileButton(nullptr)
    , m_spacer(nullptr)
    , m_layout(nullptr)
    , m_toolTip(nullptr)
    , m_isClearButtonEnabled(false)
    , m_isFileButtonEnabled(false)
    , m_isAlerted(false) {
    init();
}

LineEditBase::LineEditBase(const QString &text, QWidget *parent)
    : LineEditBase(parent) {
    setText(text);
}

void LineEditBase::setClearButtonEnabled(bool enable) {
    m_isClearButtonEnabled = enable;
    if (!m_clearButton && enable) {
        m_clearButton = new ClearButton(this);
        m_clearButton->setBorderless(true);
        m_clearButton->setTransparent(true);
        connect(m_clearButton, &QPushButton::clicked, this, &QLineEdit::clear);
        connect(m_clearButton, &QPushButton::clicked, this, [this]() { emit interacted(); });
        
        int index = m_layout->indexOf(m_spacer);
        m_layout->insertWidget(index + 1, m_clearButton, 0, Qt::AlignRight);
        
        int rightMargin = m_clearButton->minimumSizeHint().width() + textMargins().right() + 3;
        setTextMargins(0, 0, rightMargin, 0);
    }
    
    if (m_clearButton) {
        m_clearButton->setVisible(enable && hasFocus());
    }
}

bool LineEditBase::isClearButtonEnabled() const {
    return m_isClearButtonEnabled;
}

void LineEditBase::setFileButtonEnabled(bool enable) {
    m_isFileButtonEnabled = enable;
    if (!m_fileButton && enable) {
        m_fileButton = new FileButton(this);
        m_fileButton->setBorderless(true);
        m_fileButton->setTransparent(true);
        connect(m_fileButton, &QPushButton::clicked, this, [this]() {
            emit interacted();
        });
        m_layout->addWidget(m_fileButton, 0, Qt::AlignRight);
        
        int rightMargin = m_fileButton->minimumSizeHint().width() + textMargins().right() + 3;
        setTextMargins(0, 0, rightMargin, 0);
    }
    
    if (m_fileButton) {
        m_fileButton->setVisible(enable);
    }
}

bool LineEditBase::isFileButtonEnabled() const {
    return m_isFileButtonEnabled;
}

void LineEditBase::setFileDialog(QFileDialog::FileMode mode, const QString &fileType, const QString &directory, const QString &buttonTooltip) {
    setFileButtonEnabled(true);
    m_fileButton->setFileDialog(this, mode, fileType, directory, buttonTooltip);
}

void LineEditBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
    style()->unpolish(this);
    style()->polish(this);
}

void LineEditBase::setTransparent(bool transparent) {
    setProperty("isTransparent", transparent);
    style()->unpolish(this);
    style()->polish(this);
}

void LineEditBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

void LineEditBase::alert(bool enable, const QString &content) {
    m_isAlerted = enable;
    if (enable) {
        showToolTip(content);
    } else {
        hideToolTip();
    }
    // Force stylesheet update to apply alert style
    style()->unpolish(this);
    style()->polish(this);
}

void LineEditBase::showToolTip(const QString &content) {
    if (!m_toolTip->isVisible() && !content.isEmpty()) {
        m_toolTip->showText(mapToGlobal(QPoint(0, -height())), content);
    }
}

void LineEditBase::hideToolTip() {
    if (m_toolTip->isVisible()) {
        m_toolTip->hideText();
    }
}

void LineEditBase::contextMenuEvent(QContextMenuEvent *event) {
    LineEditMenu *menu = new LineEditMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->exec(event->globalPos());
}

void LineEditBase::mouseMoveEvent(QMouseEvent *event) {
    QLineEdit::mouseMoveEvent(event);
    emit cursorPositionChanged(event->position().x(), event->position().y());
    emit interacted();
}

void LineEditBase::moveEvent(QMoveEvent *event) {
    QLineEdit::moveEvent(event);
    emit rectChanged(rect());
}

void LineEditBase::resizeEvent(QResizeEvent *event) {
    QLineEdit::resizeEvent(event);
    emit rectChanged(rect());
}

void LineEditBase::focusInEvent(QFocusEvent *event) {
    QLineEdit::focusInEvent(event);
    emit focusedIn();
    if (m_clearButton && m_isClearButtonEnabled) {
        m_clearButton->show();
    }
}

void LineEditBase::focusOutEvent(QFocusEvent *event) {
    QLineEdit::focusOutEvent(event);
    emit focusedOut();
    if (m_clearButton && m_isClearButtonEnabled) {
        m_clearButton->hide();
    }
}

void LineEditBase::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
    QLineEdit::dragEnterEvent(event);
}

void LineEditBase::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QStringList paths;
            for (const QUrl &url : urls) {
                paths.append(url.toLocalFile());
            }
            setText(paths.join(", "));
        }
    }
    QLineEdit::dropEvent(event);
}

void LineEditBase::init() {
    installEventFilter(this);

    connect(this, &QLineEdit::textChanged, this, [this]() {
        emit interacted();
        setClearButtonEnabled(!text().isEmpty());
    });

    m_toolTip = new ToolTipBase(this);
    installEventFilter(new ToolTipEventFilter(this, m_toolTip));

    m_layout = new QHBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addItem(m_spacer);

    // Initialize buttons as nullptr, they will be created lazily if needed
    // or we can create them here if we want to follow Python's logic of property getters
    
    StyleSheetBase::apply(this, StyleSheetBase::Edit);
}

QSize LineEditBase::sizeHint() const {
    QSize hint = QLineEdit::sizeHint();
    if (width() > 0) {
        hint.setWidth(qMax(hint.width(), width()));
    }
    return hint;
}

/**
 * TextEditBase implementation
 */

TextEditBase::TextEditBase(QWidget *parent)
    : QTextEdit(parent)
    , m_keyEnterBlocked(false)
    , m_delegate(nullptr) {
    init();
}

TextEditBase::TextEditBase(const QString &text, QWidget *parent)
    : TextEditBase(parent) {
    setText(text);
}

void TextEditBase::init() {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    m_delegate = new ScrollDelegate(this);

    document()->setDocumentMargin(0);

    StyleSheetBase::apply(this, StyleSheetBase::Edit);
}

void TextEditBase::updateGeometries() {
    QTextEdit::updateGeometries();
    if (m_delegate) {
        m_delegate->updateViewportMargins();
        // Ensure document width matches viewport to prevent horizontal scrollbars with img { width: 100%; }
        document()->setTextWidth(viewport()->width());
    }
}

void TextEditBase::contextMenuEvent(QContextMenuEvent *event) {
    TextEditMenu *menu = new TextEditMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->exec(event->globalPos());
}

void TextEditBase::keyPressEvent(QKeyEvent *event) {
    QTextEdit::keyPressEvent(event);
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) &&
        !(event->modifiers() & Qt::ShiftModifier) && !m_keyEnterBlocked) {
        emit keyEnterPressed();
    }
}

void TextEditBase::blockKeyEnter(bool block) {
    m_keyEnterBlocked = block;
}

bool TextEditBase::isKeyEnterBlocked() const {
    return m_keyEnterBlocked;
}

void TextEditBase::setMaximumLines(int maxLines) {
    document()->setMaximumBlockCount(maxLines);
}

void TextEditBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
    style()->unpolish(this);
    style()->polish(this);
}

void TextEditBase::setTransparent(bool transparent) {
    setProperty("isTransparent", transparent);
    style()->unpolish(this);
    style()->polish(this);
}

void TextEditBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}