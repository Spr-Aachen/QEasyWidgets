#include "ComboBox.h"

#include <QWheelEvent>

#include "../Common/StyleSheet.h"


/**
 * ComboBoxBase implementation
 */

ComboBoxBase::ComboBoxBase(QWidget *parent)
    : QComboBox(parent) {
    init();
}

void ComboBoxBase::init() {
    setFocusPolicy(Qt::StrongFocus);
    StyleSheetBase::apply(this, StyleSheetBase::ComboBox);
}

void ComboBoxBase::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void ComboBoxBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
}

void ComboBoxBase::setTransparent(bool transparent) {
    setProperty("isTransparent", transparent);
}

void ComboBoxBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}