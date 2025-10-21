#include "GroupBox.h"

#include <QFont>
#include <QPropertyAnimation>

#include "../Common/StyleSheet.h"
#include "../Common/QFunctions.h"


/**
 * GroupBoxBase implementation
 */

GroupBoxBase::GroupBoxBase(QWidget *parent)
    : SizableWidget<QGroupBox>(parent) {
    init();
}

GroupBoxBase::GroupBoxBase(const QString &title, QWidget *parent)
    : SizableWidget<QGroupBox>(title, parent) {
    init();
}

void GroupBoxBase::init() {
    setCheckable(true);
    connect(this, &QGroupBox::toggled, this, [this](bool checked) {
        if (checked) {
            expand();
        } else {
            collapse();
        }
    });

    QFont font = this->font();
    font.setPointSize(15);
    setFont(font);

    StyleSheetBase::apply(this, StyleSheetBase::GroupBox);
}

void GroupBoxBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
}

void GroupBoxBase::setTransparent(bool transparent) {
    setProperty("isTransparent", transparent);
}

void GroupBoxBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

void GroupBoxBase::expand() {
    QParallelAnimationGroup *animation = setWidgetSizeAnimation(this, -1, sizeHint().height());
    if (animation) {
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }
}

void GroupBoxBase::collapse() {
    int titleHeight = fontMetrics().height() + 3;
    QParallelAnimationGroup *animation = setWidgetSizeAnimation(this, -1, titleHeight);
    if (animation) {
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }
}