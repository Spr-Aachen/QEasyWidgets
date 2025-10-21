#include "Label.h"

#include "../Common/StyleSheet.h"


/**
 * LabelBase implementation
 */

LabelBase::LabelBase(QWidget *parent)
    : SizableWidget<QLabel>(parent)
    , m_lightTextColor(Qt::black)
    , m_darkTextColor(Qt::white) {
    StyleSheetBase::apply(this, StyleSheetBase::Label);
    updateTextColor();
}

LabelBase::LabelBase(const QString &text, QWidget *parent)
    : LabelBase(parent) {
    setText(text);
}

void LabelBase::setCustomTextColor(const QColor &light, const QColor &dark) {
    m_lightTextColor = light;
    m_darkTextColor = dark;
    updateTextColor();
}

void LabelBase::updateTextColor() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, isDarkTheme() ? m_darkTextColor : m_lightTextColor);
    setPalette(palette);
}