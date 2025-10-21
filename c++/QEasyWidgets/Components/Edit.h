#ifndef QEASYWIDGETS_EDIT_H
#define QEASYWIDGETS_EDIT_H

#include <QLineEdit>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QToolTip>

#include "Button.h"


namespace QEW {

/**
 * @brief Enhanced line edit with additional features
 */
class LineEditBase : public QLineEdit
{
    Q_OBJECT

public:
    explicit LineEditBase(QWidget *parent = nullptr);
    explicit LineEditBase(const QString &text, QWidget *parent = nullptr);
    ~LineEditBase() override = default;

    void setClearButtonEnabled(bool enable);
    bool isClearButtonEnabled() const;

    void setFileButtonEnabled(bool enable);
    bool isFileButtonEnabled() const;

    void setFileDialog(QFileDialog::FileMode mode,
                      const QString &fileType = QString(),
                      const QString &directory = QString(),
                      const QString &buttonTooltip = "Browse");

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);

    void clearDefaultStyleSheet();

    void alert(bool enable, const QString &content = QString());

signals:
    void cursorPositionChanged(int x, int y);
    void focusedIn();
    void focusedOut();
    void interacted();
    void rectChanged(const QRect &rect);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void init();

    ClearButton *m_clearButton;
    FileButton *m_fileButton;
    QSpacerItem *m_spacer;
    QHBoxLayout *m_layout;
    QToolTip *m_toolTip;
    bool m_isClearButtonEnabled;
    bool m_isFileButtonEnabled;
    bool m_isAlerted;
};

/**
 * @brief Enhanced text edit with additional features
 */
class TextEditBase : public QTextEdit
{
    Q_OBJECT

public:
    explicit TextEditBase(QWidget *parent = nullptr);
    explicit TextEditBase(const QString &text, QWidget *parent = nullptr);
    ~TextEditBase() override = default;

    void blockKeyEnter(bool block);
    bool isKeyEnterBlocked() const;

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);

    void clearDefaultStyleSheet();

signals:
    void keyEnterPressed();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void init();

    bool m_keyEnterBlocked;
};

} // namespace QEW

#endif // QEASYWIDGETS_EDIT_H
