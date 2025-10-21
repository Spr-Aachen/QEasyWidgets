#ifndef QEASYWIDGETS_CHECKBOX_H
#define QEASYWIDGETS_CHECKBOX_H

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>


namespace QEW {

/**
 * @brief Toggle indicator widget for CheckBox (internal use only)
 */
class Indicator : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(float ellipseCordX READ ellipseCordX WRITE setEllipseCordX)

public:
    explicit Indicator(QWidget *parent = nullptr);
    ~Indicator() override = default;

    bool isChecked() const;
    void setChecked(bool checked);
    void toggle();

    float ellipseCordX() const;
    void setEllipseCordX(float x);
    void setDown(bool isDown);
    void setHover(bool isHover);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void init();

    bool m_isChecked;
    bool m_isDown;
    bool m_isHover;
    float m_ellipseCordX;
    float m_ellipseCordX_Default;
    int m_ellipseLength;
    int m_width;
    int m_height;
    QPropertyAnimation *m_ellipseAnimation;
};

/**
 * @brief Enhanced checkbox with custom indicator and theme support
 */
class CheckBoxBase : public QWidget
{
    Q_OBJECT

public:
    explicit CheckBoxBase(QWidget *parent = nullptr);
    explicit CheckBoxBase(const QString &text, QWidget *parent = nullptr);
    ~CheckBoxBase() override = default;

    QString text() const;
    void setText(const QString &text);

    bool isChecked() const;
    void setChecked(bool checked);

    int spacing() const;
    void setSpacing(int spacing);

signals:
    void toggled(bool checked);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void init();

    Indicator *m_indicator;
    QLabel *m_label;
    QHBoxLayout *m_layout;
    int m_spacing;
};

} // namespace QEW

#endif // QEASYWIDGETS_CHECKBOX_H