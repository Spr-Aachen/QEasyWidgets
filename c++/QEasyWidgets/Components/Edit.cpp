#include "Edit.h"

#include <QMimeData>
#include <QMouseEvent>
#include <QKeyEvent>

#include "../Common/StyleSheet.h"


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
    if (m_clearButton) {
        m_clearButton->setVisible(enable && hasFocus());
    }
}

bool LineEditBase::isClearButtonEnabled() const {
    return m_isClearButtonEnabled;
}

void LineEditBase::setFileButtonEnabled(bool enable) {
    m_isFileButtonEnabled = enable;
    if (m_fileButton) {
        m_fileButton->setVisible(enable);
    }
}

bool LineEditBase::isFileButtonEnabled() const {
    return m_isFileButtonEnabled;
}

void LineEditBase::setFileDialog(QFileDialog::FileMode mode, const QString &fileType, const QString &directory, const QString &buttonTooltip) {
    if (!m_fileButton) {
        // Create file button if it doesn't exist
        m_fileButton = new FileButton(this);
        m_fileButton->setBorderless(true);
        m_fileButton->setTransparent(true);
        connect(m_fileButton, &QPushButton::clicked, this, [this]() {
            emit interacted();
        });
        m_layout->addWidget(m_fileButton, 0, Qt::AlignRight);
    }

    m_fileButton->setFileDialog(this, mode, fileType, directory, buttonTooltip);
    setFileButtonEnabled(true);
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
    if (enable && !content.isEmpty()) {
        m_toolTip->showText(mapToGlobal(QPoint(0, -height())), content);
    } else {
        m_toolTip->hideText();
    }
    // Force stylesheet update
    setStyleSheet(styleSheet());
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
    , m_keyEnterBlocked(false) {
    init();
}

TextEditBase::TextEditBase(const QString &text, QWidget *parent)
    : TextEditBase(parent) {
    setText(text);
}

void TextEditBase::blockKeyEnter(bool block) {
    m_keyEnterBlocked = block;
}

bool TextEditBase::isKeyEnterBlocked() const {
    return m_keyEnterBlocked;
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

void TextEditBase::keyPressEvent(QKeyEvent *event) {
    QTextEdit::keyPressEvent(event);
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) &&
        !(event->modifiers() & Qt::ShiftModifier) && !m_keyEnterBlocked) {
        emit keyEnterPressed();
    }
}

void TextEditBase::init() {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    StyleSheetBase::apply(this, StyleSheetBase::Edit);
}