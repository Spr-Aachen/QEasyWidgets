#include "Dialog.h"

#include <QApplication>
#include <QSize>
#include <functional>

#include "../Components/Button.h"


/**
 * DialogBase implementation
 */

DialogBase::DialogBase(QWidget *parent, Qt::WindowFlags flags)
    : FramelessWindowBase(parent, flags)
    , m_mainLayout(nullptr)
    , m_eventLoop(nullptr)
    , m_result(0) {
    setFrameless(false); // Dialogs don't stretch
    setupUI();
    
    // Connect to parent window mask
    QWidget *parentWindow = parent;
    while (parentWindow && !qobject_cast<FramelessWindowBase*>(parentWindow)) {
        parentWindow = parentWindow->parentWidget();
        if (parentWindow) parentWindow = parentWindow->window();
    }
    
    if (!parentWindow && parent) {
        parentWindow = parent->window();
    }
    
    if (auto fwParent = qobject_cast<FramelessWindowBase*>(parentWindow)) {
        connect(this, &FramelessWindowBase::showed, fwParent, [fwParent]() { fwParent->showMask(true); });
        connect(this, &FramelessWindowBase::closed, fwParent, [fwParent]() { fwParent->showMask(false); });
    }
}

void DialogBase::setupUI() {
    FramelessWindowBase::setupUI();

    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    layout()->addLayout(m_mainLayout);

    // Hide minimize and maximize buttons for dialogs
    if (titleBar()) {
        if (titleBar()->minimizeButton()) {
            titleBar()->minimizeButton()->hide();
            titleBar()->minimizeButton()->deleteLater();
        }
        if (titleBar()->maximizeButton()) {
            titleBar()->maximizeButton()->hide();
            titleBar()->maximizeButton()->deleteLater();
        }
    }
}

int DialogBase::exec() {
    if (m_eventLoop) {
        return m_result;
    }

    m_eventLoop = new QEventLoop(this);
    show();  // This will emit showed() signal via showEvent
    m_result = m_eventLoop->exec();
    delete m_eventLoop;
    m_eventLoop = nullptr;

    emit finished(m_result);
    // closed() signal is emitted by closeEvent

    return m_result;
}

void DialogBase::accept() {
    m_result = QDialog::Accepted;
    if (m_eventLoop) {
        m_eventLoop->exit(m_result);
    }
    hide();
    emit accepted();
}

void DialogBase::reject() {
    m_result = QDialog::Rejected;
    if (m_eventLoop) {
        m_eventLoop->exit(m_result);
    }
    hide();
    emit rejected();
}

void DialogBase::closeEvent(QCloseEvent *event) {
    if (m_eventLoop) {
        reject();
    }
    FramelessWindowBase::closeEvent(event);
}

void DialogBase::mouseDoubleClickEvent(QMouseEvent *event) {
    // Disable double-click maximize for dialogs
    Q_UNUSED(event);
    return;
}

// MessageBoxBase implementation
MessageBoxBase::MessageBoxBase(QWidget *parent, int minWidth, int minHeight)
    : DialogBase(parent)
    , m_iconLabel(nullptr)
    , m_textLabel(nullptr)
    , m_buttonBox(nullptr)
    , m_detailedTextBrowser(nullptr)
    , m_clickedButton(QMessageBox::NoButton) {
    setMinimumSize(minWidth, minHeight);
    
    // Create icon and text layout
    m_iconLabel = new QLabel(this);
    m_iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_iconLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    
    m_textLabel = new QLabel(this);
    m_textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_textLabel->setWordWrap(true);
    m_textLabel->setAlignment(Qt::AlignCenter);
    
    QHBoxLayout *labelLayout = new QHBoxLayout();
    labelLayout->setContentsMargins(0, 0, 0, 0);
    labelLayout->setSpacing(12);
    labelLayout->addWidget(m_iconLabel, 0);
    labelLayout->addWidget(m_textLabel, 1);
    
    // Create button box
    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_buttonBox->setOrientation(Qt::Horizontal);
    m_buttonBox->setStyleSheet("padding: 6px 18px 6px 18px;");
    
    connect(m_buttonBox, &QDialogButtonBox::clicked, this, &MessageBoxBase::onButtonClicked);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &DialogBase::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &DialogBase::reject);
    
    // Add to main layout
    m_mainLayout->setContentsMargins(21, 12, 21, 12);
    m_mainLayout->setSpacing(12);
    m_mainLayout->addLayout(labelLayout);
    m_mainLayout->addWidget(m_buttonBox);
}

void MessageBoxBase::setStandardButtons(QMessageBox::StandardButtons buttons) {
    QDialogButtonBox::StandardButtons dialogButtons = QDialogButtonBox::NoButton;
    
    if (buttons & QMessageBox::Ok) dialogButtons |= QDialogButtonBox::Ok;
    if (buttons & QMessageBox::Cancel) dialogButtons |= QDialogButtonBox::Cancel;
    if (buttons & QMessageBox::Yes) dialogButtons |= QDialogButtonBox::Yes;
    if (buttons & QMessageBox::No) dialogButtons |= QDialogButtonBox::No;
    if (buttons & QMessageBox::Retry) dialogButtons |= QDialogButtonBox::Retry;
    if (buttons & QMessageBox::Ignore) dialogButtons |= QDialogButtonBox::Ignore;
    if (buttons & QMessageBox::Open) dialogButtons |= QDialogButtonBox::Open;
    if (buttons & QMessageBox::Close) dialogButtons |= QDialogButtonBox::Close;
    if (buttons & QMessageBox::Save) dialogButtons |= QDialogButtonBox::Save;
    if (buttons & QMessageBox::Discard) dialogButtons |= QDialogButtonBox::Discard;
    if (buttons & QMessageBox::Apply) dialogButtons |= QDialogButtonBox::Apply;
    if (buttons & QMessageBox::RestoreDefaults) dialogButtons |= QDialogButtonBox::RestoreDefaults;
    
    m_buttonBox->setStandardButtons(dialogButtons);
}

void MessageBoxBase::setIcon(QMessageBox::Icon icon) {
    QStyle::StandardPixmap pixmap;
    switch (icon) {
        case QMessageBox::Question:
            pixmap = QStyle::SP_MessageBoxQuestion;
            break;
        case QMessageBox::Information:
            pixmap = QStyle::SP_MessageBoxInformation;
            break;
        case QMessageBox::Warning:
            pixmap = QStyle::SP_MessageBoxWarning;
            break;
        case QMessageBox::Critical:
            pixmap = QStyle::SP_MessageBoxCritical;
            break;
        default:
            m_iconLabel->clear();
            return;
    }
    
    int length = qMin(width(), height()) / 6;
    if (length < 32) length = 32;
    
    QIcon standardIcon = QApplication::style()->standardIcon(pixmap);
    QPixmap iconPixmap = standardIcon.pixmap(QSize(length, length));
    m_iconLabel->setPixmap(iconPixmap);
}

void MessageBoxBase::setIcon(const QIcon &icon) {
    int length = qMin(width(), height()) / 6;
    if (length < 32) length = 32;
    m_iconLabel->setPixmap(icon.pixmap(QSize(length, length)));
}

void MessageBoxBase::setIcon(const QPixmap &pixmap) {
    m_iconLabel->setPixmap(pixmap);
}

void MessageBoxBase::setWindowIcon(QMessageBox::Icon icon) {
    QStyle::StandardPixmap pixmap;
    switch (icon) {
        case QMessageBox::Question:
            pixmap = QStyle::SP_MessageBoxQuestion;
            break;
        case QMessageBox::Information:
            pixmap = QStyle::SP_MessageBoxInformation;
            break;
        case QMessageBox::Warning:
            pixmap = QStyle::SP_MessageBoxWarning;
            break;
        case QMessageBox::Critical:
            pixmap = QStyle::SP_MessageBoxCritical;
            break;
        default:
            return;
    }
    
    QIcon standardIcon = QApplication::style()->standardIcon(pixmap);
    QWidget::setWindowIcon(standardIcon);
}

void MessageBoxBase::setText(const QString &text, float textSize, int textWeight) {
    QString html = QString("<div style='text-align: center; font-size: %1pt; font-weight: %2;'>%3</div>").arg(textSize).arg(textWeight).arg(text);
    m_textLabel->setText(html);
}

void MessageBoxBase::setDetailedText(const QString &text) {
    if (!m_detailedTextBrowser) {
        m_detailedTextBrowser = new QTextBrowser(this);
        m_detailedTextBrowser->setReadOnly(true);
        int buttonIndex = m_mainLayout->indexOf(m_buttonBox);
        m_mainLayout->insertWidget(buttonIndex, m_detailedTextBrowser, 1);
    }
    m_detailedTextBrowser->setMarkdown(text);
}

QTextBrowser *MessageBoxBase::detailedTextBrowser() {
    if (!m_detailedTextBrowser) {
        m_detailedTextBrowser = new QTextBrowser(this);
        m_detailedTextBrowser->setReadOnly(true);
        int buttonIndex = m_mainLayout->indexOf(m_buttonBox);
        m_mainLayout->insertWidget(buttonIndex, m_detailedTextBrowser, 1);
    }
    return m_detailedTextBrowser;
}

QPair<QString, bool> MessageBoxBase::getText(
    const QString &title,
    const QString &label,
    QLineEdit::EchoMode echo,
    const QString &text,
    Qt::WindowFlags flags,
    Qt::InputMethodHints inputMethodHints
) {
    setWindowFlags(flags);
    setText(title);
    
    QLabel *inputLabel = new QLabel(label, this);
    inputLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QLineEdit *inputArea = new QLineEdit(this);
    inputArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    inputArea->setEchoMode(echo);
    inputArea->setText(text);
    inputArea->setInputMethodHints(inputMethodHints);
    
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(12);
    inputLayout->addWidget(inputLabel, 0);
    inputLayout->addWidget(inputArea, 1);
    
    m_mainLayout->insertLayout(1, inputLayout, 1);
    
    setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int result = exec();
    
    return QPair<QString, bool>(inputArea->text(), result == QMessageBox::Ok);
}

QMessageBox::StandardButton MessageBoxBase::pop(
    QWidget *windowToMask,
    QMessageBox::Icon messageType,
    const QString &windowTitle,
    const QString &text,
    const QString &detailedText,
    QMessageBox::StandardButtons buttons,
    const QMap<QMessageBox::StandardButton, std::function<void()>> &buttonEvents
) {
    MessageBoxBase *msgBox = new MessageBoxBase(windowToMask);
    
    msgBox->setIcon(messageType);
    msgBox->setWindowTitle(windowTitle);
    msgBox->setText(text);
    if (!detailedText.isEmpty()) {
        msgBox->setDetailedText(detailedText);
    }
    msgBox->setStandardButtons(buttons);
    
    int result = msgBox->exec();
    QMessageBox::StandardButton clickedBtn = msgBox->clickedButton();
    
    if (buttonEvents.contains(clickedBtn)) {
        buttonEvents[clickedBtn]();
    }
    
    msgBox->deleteLater();
    return clickedBtn;
}

int MessageBoxBase::exec() {
    int result = DialogBase::exec();
    return static_cast<int>(m_clickedButton);
}

void MessageBoxBase::onButtonClicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton dialogBtn = m_buttonBox->standardButton(button);
    
    m_clickedButton = QMessageBox::NoButton;
    if (dialogBtn & QDialogButtonBox::Ok) m_clickedButton = QMessageBox::Ok;
    else if (dialogBtn & QDialogButtonBox::Cancel) m_clickedButton = QMessageBox::Cancel;
    else if (dialogBtn & QDialogButtonBox::Yes) m_clickedButton = QMessageBox::Yes;
    else if (dialogBtn & QDialogButtonBox::No) m_clickedButton = QMessageBox::No;
    else if (dialogBtn & QDialogButtonBox::Retry) m_clickedButton = QMessageBox::Retry;
    else if (dialogBtn & QDialogButtonBox::Ignore) m_clickedButton = QMessageBox::Ignore;
    else if (dialogBtn & QDialogButtonBox::Open) m_clickedButton = QMessageBox::Open;
    else if (dialogBtn & QDialogButtonBox::Close) m_clickedButton = QMessageBox::Close;
    else if (dialogBtn & QDialogButtonBox::Save) m_clickedButton = QMessageBox::Save;
    else if (dialogBtn & QDialogButtonBox::Discard) m_clickedButton = QMessageBox::Discard;
    else if (dialogBtn & QDialogButtonBox::Apply) m_clickedButton = QMessageBox::Apply;
    else if (dialogBtn & QDialogButtonBox::RestoreDefaults) m_clickedButton = QMessageBox::RestoreDefaults;
}

/**
 * InputDialogBase implementation
 */

InputDialogBase::InputDialogBase(QWidget *parent, int minWidth, int minHeight)
    : MessageBoxBase(parent, minWidth, minHeight) {
}

QPair<QString, bool> InputDialogBase::getText(
    QWidget *parent,
    const QString &title,
    const QString &label,
    QLineEdit::EchoMode echo,
    const QString &text,
    Qt::WindowFlags flags,
    Qt::InputMethodHints inputMethodHints
) {
    MessageBoxBase msgBox(parent, 420, 210);
    return msgBox.getText(title, label, echo, text, flags, inputMethodHints);
}