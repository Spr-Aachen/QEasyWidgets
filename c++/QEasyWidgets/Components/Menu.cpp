#include "Menu.h"

#include "../Common/StyleSheet.h"
#include "../Common/QFunctions.h"
#include "../Common/Theme.h"
#include "ScrollArea.h"

#include <QScreen>
#include <QHoverEvent>
#include <QDebug>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QWindow>
#include <QShortcut>
#include <algorithm>


// Font for menu items
static QFont getMenuFont() {
    QFont font;
    font.setPixelSize(12);
    return font;
}

/**
 * MenuItemDelegate implementation
 */

MenuItemDelegate::MenuItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {
}

bool MenuItemDelegate::isSeparator(const QModelIndex &index) const {
    return index.model()->data(index, Qt::DecorationRole).toString() == "seperator";
}

void MenuItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!isSeparator(index)) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    // Draw separator
    painter->save();
    int c = isDarkTheme() ? 255 : 0;
    QPen pen(QColor(c, c, c, 25), 1);
    pen.setCosmetic(true);
    painter->setPen(pen);
    QRect rect = option.rect;
    painter->drawLine(0, rect.y() + 4, rect.width() + 12, rect.y() + 4);
    painter->restore();
    // Draw shortcut key
    QAction *action = index.data(Qt::UserRole).value<QAction*>();
    if (!action || action->shortcut().isEmpty()) {
        return;
    }
    painter->save();
    if (!(option.state & QStyle::State_Enabled)) {
        painter->setOpacity(isDarkTheme() ? 0.5 : 0.6);
    }
    painter->setPen(isDarkTheme() ? QColor(255, 255, 255, 200) : QColor(0, 0, 0, 153));
    QFont font = getMenuFont();
    QFontMetrics fm(font);
    QString shortcut = action->shortcut().toString(QKeySequence::NativeText);
    int sw = fm.boundingRect(shortcut).width();
    painter->translate(option.rect.width() - sw - 20, 0);
    QRectF textRect(0, option.rect.y(), sw, option.rect.height());
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, shortcut);
    painter->restore();
}

/**
 * MenuAnimationManager implementation
 */

MenuAnimationManager::MenuAnimationManager(MenuBase *menu)
    : QObject(menu), m_menu(menu) {
    m_anim = new QPropertyAnimation(menu, "pos", menu);
    m_anim->setDuration(250);
    m_anim->setEasingCurve(QEasingCurve::OutQuad);
    connect(m_anim, &QPropertyAnimation::valueChanged, this, &MenuAnimationManager::onValueChanged);
    connect(m_anim, &QPropertyAnimation::valueChanged, this, &MenuAnimationManager::updateMenuViewport);
}

QSize MenuAnimationManager::menuSize() const {
    QMargins m = m_menu->layout()->contentsMargins();
    int w = m_menu->m_view->width() + m.left() + m.right() + 120;
    int h = m_menu->m_view->height() + m.top() + m.bottom() + 20;
    return QSize(w, h);
}

void MenuAnimationManager::onValueChanged(const QVariant &value) {
    Q_UNUSED(value);
}

void MenuAnimationManager::updateMenuViewport() {
    m_menu->m_view->viewport()->update();
    m_menu->m_view->setAttribute(Qt::WA_UnderMouse, true);
    // Simulate hover event
    QHoverEvent e(QEvent::HoverEnter, QPointF(), QPointF(1, 1));
    QApplication::sendEvent(m_menu->m_view, &e);
}

QSize MenuAnimationManager::availableViewSize(const QPoint &pos) {
    Q_UNUSED(pos);
    QRect ss = getScreenGeometry(getCurrentScreen());
    int w = ss.width() - 100;
    int h = ss.height() - 100;
    return QSize(w, h);
}

void MenuAnimationManager::exec(const QPoint &pos) {
    QRect rect = getScreenGeometry(getCurrentScreen());
    int x = std::min(pos.x() - m_menu->layout()->contentsMargins().left(), rect.right() - (m_menu->width() + 5));
    int y = std::min(pos.y() - 4, rect.bottom() - (m_menu->height()) + 10);
    m_menu->move(x, y);
}

MenuAnimationManager* MenuAnimationManager::make(MenuBase *menu, Direction animType) {
    switch (animType) {
        case Direction::Down:
            return new DropDownMenuAnimationManager(menu);
        case Direction::Up:
            return new PullUpMenuAnimationManager(menu);
        default:
            return new MenuAnimationManager(menu);
    }
}

/**
 * DropDownMenuAnimationManager implementation
 */

void DropDownMenuAnimationManager::onValueChanged(const QVariant &value) {
    Q_UNUSED(value);
    int y = m_anim->endValue().toPoint().y() - m_anim->currentValue().toPoint().y();
    QSize s = menuSize();
    m_menu->setMask(QRegion(0, y, s.width(), s.height()));
}

QSize DropDownMenuAnimationManager::availableViewSize(const QPoint &pos) {
    QRect ss = getScreenGeometry(getCurrentScreen());
    return QSize(ss.width() - 100, std::max(ss.bottom() - pos.y() - 10, 1));
}

void DropDownMenuAnimationManager::exec(const QPoint &pos) {
    QRect rect = getScreenGeometry(getCurrentScreen());
    int x = std::min(pos.x() - m_menu->layout()->contentsMargins().left(), rect.right() - (m_menu->width() + 5));
    int y = std::min(pos.y() - 4, rect.bottom() - (m_menu->height()) + 10);
    QPoint newPos(x, y);
    int h = m_menu->height() + 5;
    m_anim->setStartValue(newPos - QPoint(0, h/2));
    m_anim->setEndValue(newPos);
    m_anim->start();
}

/**
 * PullUpMenuAnimationManager implementation
 */

void PullUpMenuAnimationManager::onValueChanged(const QVariant &value) {
    Q_UNUSED(value);
    int y = m_anim->endValue().toPoint().y() - m_anim->currentValue().toPoint().y();
    QSize s = menuSize();
    m_menu->setMask(QRegion(0, y, s.width(), s.height() - 28));
}

QSize PullUpMenuAnimationManager::availableViewSize(const QPoint &pos) {
    QRect ss = getScreenGeometry(getCurrentScreen());
    return QSize(ss.width() - 100, std::max(pos.y() - ss.top() - 28, 1));
}

void PullUpMenuAnimationManager::exec(const QPoint &pos) {
    QRect rect = getScreenGeometry(getCurrentScreen());
    int x = std::min(pos.x() - m_menu->layout()->contentsMargins().left(), rect.right() - (m_menu->width() + 5));
    int y = std::max(pos.y() - (m_menu->height()) + 10, rect.top() + 4);
    QPoint newPos(x, y);
    int h = m_menu->height() + 5;
    m_anim->setStartValue(newPos + QPoint(0, h/2));
    m_anim->setEndValue(newPos);
    m_anim->start();
}

/**
 * MenuActionListWidget implementation
 */

MenuActionListWidget::MenuActionListWidget(QWidget *parent)
    : QListWidget(parent), m_itemHeight(28), m_maxVisibleItems(-1) {
    setViewportMargins(0, 6, 0, 6);
    setTextElideMode(Qt::ElideNone);
    setDragEnabled(false);
    setMouseTracking(true);
    setVerticalScrollMode(ScrollMode::ScrollPerPixel);
    setIconSize(QSize(14, 14));
    setItemDelegate(new MenuItemDelegate(this));

    new ScrollDelegate(this);

    setStyleSheet("MenuActionListWidget{font: 14px \"Segoe UI\", \"Microsoft YaHei\", \"PingFang SC\"}");
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MenuActionListWidget::adjustSize(const QPoint &pos, Direction animType) {
    QSize size;
    for (int i = 0; i < count(); ++i) {
        QSize s = item(i)->sizeHint();
        size.setWidth(std::max({s.width(), size.width(), 1}));
        size.setHeight(std::max(1, size.height() + s.height()));
    }
    // Adjust viewport height
    MenuBase* menu = qobject_cast<MenuBase*>(parent());
    MenuAnimationManager* manager = MenuAnimationManager::make(menu, animType);
    QSize availSize = manager->availableViewSize(pos);
    delete manager; // Clean up temporary manager
    viewport()->adjustSize();
    // Adjust the height of list widget
    QMargins m = viewportMargins();
    size += QSize(m.left() + m.right() + 2, m.top() + m.bottom());
    size.setHeight(std::min(availSize.height(), size.height() + 3));
    size.setWidth(std::max(std::min(availSize.width(), size.width()), minimumWidth()));
    if (maxVisibleItems() > 0) {
        size.setHeight(std::min(size.height(), maxVisibleItems() * m_itemHeight + m.top() + m.bottom() + 3));
    }
    setFixedSize(size);
}

void MenuActionListWidget::insertItem(int row, QListWidgetItem *item) {
    QListWidget::insertItem(row, item);
    adjustSize();
}

void MenuActionListWidget::addItem(QListWidgetItem *item) {
    QListWidget::addItem(item);
    adjustSize();
}

QListWidgetItem *MenuActionListWidget::takeItem(int row) {
    QListWidgetItem *item = QListWidget::takeItem(row);
    adjustSize();
    return item;
}

int MenuActionListWidget::maxVisibleItems() const {
    return m_maxVisibleItems;
}

void MenuActionListWidget::setMaxVisibleItems(int num) {
    m_maxVisibleItems = num;
    adjustSize();
}

void MenuActionListWidget::setItemHeight(int height) {
    if (height == m_itemHeight) return;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *itm = item(i);
        if (!itemWidget(itm)) {
            itm->setSizeHint(QSize(itm->sizeHint().width(), height));
        }
    }
    m_itemHeight = height;
    adjustSize();
}

int MenuActionListWidget::itemsHeight() const {
    int N = (maxVisibleItems() < 0) ? count() : std::min(maxVisibleItems(), count());
    int h = 0;
    for (int i = 0; i < N; ++i) {
        h += item(i)->sizeHint().height();
    }
    QMargins m = viewportMargins();
    return h + m.top() + m.bottom();
}

int MenuActionListWidget::heightForAnimation(const QPoint &pos, Direction animType) {
    int ih = itemsHeight();
    MenuBase* menu = qobject_cast<MenuBase*>(parent());
    MenuAnimationManager* manager = MenuAnimationManager::make(menu, animType);
    int sh = manager->availableViewSize(pos).height();
    delete manager;
    return std::min(ih, sh);
}

/**
 * SubMenuItemWidget implementation
 */

SubMenuItemWidget::SubMenuItemWidget(MenuBase *menu, QListWidgetItem *item, QWidget *parent)
    : QWidget(parent), m_menu(menu), m_item(item) {
}

void SubMenuItemWidget::enterEvent(QEnterEvent *event) {
    QWidget::enterEvent(event);
    emit showMenuSig(m_item);
}

void SubMenuItemWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    // Draw right arrow
    drawIcon(IconBase::Chevron_Right, &painter, QRectF(width() - 10, height() / 2.0 - 4.5, 9, 9).toRect());
}

/**
 * MenuBase implementation
 */

void MenuBase::init() {
    m_isSubMenu = false;
    m_parentMenu = nullptr;
    m_menuItem = nullptr;
    m_lastHoverItem = nullptr;
    m_lastHoverSubMenuItem = nullptr;
    m_isHideBySystem = true;
    m_itemHeight = 28;

    m_hBoxLayout = new QHBoxLayout(this);
    m_view = new MenuActionListWidget(this);
    m_timer = new QTimer(this);

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    m_timer->setSingleShot(true);
    m_timer->setInterval(400);
    connect(m_timer, &QTimer::timeout, this, &MenuBase::onShowMenuTimeOut);

    setShadowEffect();
    m_hBoxLayout->addWidget(m_view, 1, Qt::AlignCenter);
    m_hBoxLayout->setContentsMargins(12, 8, 12, 20);

    connect(m_view, &QListWidget::itemClicked, this, &MenuBase::onItemClicked);
    connect(m_view, &QListWidget::itemEntered, this, &MenuBase::onItemEntered);

    StyleSheetBase::apply(this, StyleSheetBase::Menu);
}

MenuBase::MenuBase(QWidget *parent)
    : QMenu(parent) {
    init();
}

MenuBase::MenuBase(const QString &title, QWidget *parent)
    : QMenu(title, parent), m_title(title) {
    init();
}

void MenuBase::setMaxVisibleItems(int num) {
    m_view->setMaxVisibleItems(num);
    adjustSize();
}

void MenuBase::setItemHeight(int height) {
    if (height == m_itemHeight) return;
    m_itemHeight = height;
    m_view->setItemHeight(height);
}

void MenuBase::setShadowEffect(int blurRadius, const QPoint &offset, const QColor &color) {
    m_shadowEffect = new QGraphicsDropShadowEffect(m_view);
    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(offset);
    m_shadowEffect->setColor(color);
    m_view->setGraphicsEffect(nullptr);
    m_view->setGraphicsEffect(m_shadowEffect);
}

void MenuBase::_setParentMenu(MenuBase *parent, QListWidgetItem *item) {
    m_parentMenu = parent;
    m_menuItem = item;
    m_isSubMenu = (parent != nullptr);
}

void MenuBase::adjustSize() {
    QMargins m = layout()->contentsMargins();
    int w = m_view->width() + m.left() + m.right();
    int h = m_view->height() + m.top() + m.bottom();
    setFixedSize(w, h);
}

QString MenuBase::title() const {
    return m_title;
}

QIcon MenuBase::icon() const {
    return m_icon;
}

void MenuBase::setIcon(const QIcon &icon) {
    m_icon = icon;
}

void MenuBase::setIcon(IconBase icon) {
    m_icon = createIcon(icon);
}

bool MenuBase::hasItemIcon() const {
    for (QAction *action : m_actions) {
        if (!action->icon().isNull()) return true;
    }
    for (MenuBase *menu : m_subMenus) {
        if (!menu->icon().isNull()) return true;
    }
    return false;
}

QIcon MenuBase::createItemIcon(QAction *action) {
    bool hasIcon = hasItemIcon();
    QIcon icon = action->icon(); // Assuming QIconEngine logic is handled or standard QIcon is enough
    if (hasIcon && icon.isNull()) {
        QPixmap pixmap(m_view->iconSize());
        pixmap.fill(Qt::transparent);
        icon = QIcon(pixmap);
    } else if (!hasIcon) {
        icon = QIcon();
    }
    return icon;
}

int MenuBase::longestShortcutWidth() const {
    int width = 0;
    QFont font = getMenuFont();
    QFontMetrics fm(font);
    for (QAction *action : m_actions) {
        width = std::max(width, fm.boundingRect(action->shortcut().toString()).width());
    }
    return width;
}

int MenuBase::adjustItemText(QListWidgetItem *item, QAction *action) {
    int sw = 0;
    // Leave space for shortcut key if using custom delegate
    if (qobject_cast<MenuItemDelegate*>(m_view->itemDelegate())) {
        sw = longestShortcutWidth();
        if (sw) sw += 22;
    }
    // Adjust the width of item text
    int w = 0;
    if (!hasItemIcon()) {
        item->setText(action->text());
        w = 40 + m_view->fontMetrics().boundingRect(action->text()).width() + sw;
    } else {
        // Add a blank character to increase space between icon and text
        item->setText(" " + action->text());
        int space = 4 - m_view->fontMetrics().boundingRect(" ").width();
        w = 60 + m_view->fontMetrics().boundingRect(item->text()).width() + sw + space;
    }
    item->setSizeHint(QSize(w, m_itemHeight));
    return w;
}

void MenuBase::onActionChanged() {
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) return;
    QListWidgetItem *item = m_actionToItem.value(action);
    if (!item) return;
    item->setIcon(createItemIcon(action));
    adjustItemText(item, action);
    if (action->isEnabled()) {
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    } else {
        item->setFlags(Qt::NoItemFlags);
    }
    m_view->adjustSize();
    adjustSize();
}

QListWidgetItem* MenuBase::createActionItem(QAction *action, QAction *before) {
    if (!before) {
        m_actions.append(action);
        QMenu::addAction(action);
    } else if (m_actions.contains(before)) {
        int index = m_actions.indexOf(before);
        m_actions.insert(index, action);
        QMenu::insertAction(before, action);
    } else {
        // Handle error or fallback
        m_actions.append(action);
        QMenu::addAction(action);
    }
    QListWidgetItem *item = new QListWidgetItem(createItemIcon(action), action->text());
    adjustItemText(item, action);
    if (!action->isEnabled()) {
        // Disable item
        item->setFlags(Qt::NoItemFlags);
    }
    QVariant v;
    v.setValue(action);
    item->setData(Qt::UserRole, v);
    m_actionToItem[action] = item;
    m_itemToAction[item] = action;
    connect(action, &QAction::changed, this, &MenuBase::onActionChanged);
    return item;
}

void MenuBase::addAction(QAction *action) {
    QListWidgetItem *item = createActionItem(action);
    m_view->addItem(item);
    adjustSize();
}

void MenuBase::addActions(const QList<QAction*> &actions) {
    for (QAction *action : actions) {
        addAction(action);
    }
}

void MenuBase::insertAction(QAction *before, QAction *action) {
    if (!m_actions.contains(before)) return;
    
    QListWidgetItem *beforeItem = m_actionToItem.value(before);
    if (!beforeItem) return;
    
    int index = m_view->row(beforeItem);
    QListWidgetItem *item = createActionItem(action, before);
    m_view->insertItem(index, item);
    adjustSize();
}

void MenuBase::insertActions(QAction *before, const QList<QAction*> &actions) {
    for (QAction *action : actions) {
        insertAction(before, action);
    }
}

void MenuBase::removeAction(QAction *action) {
    if (!m_actions.contains(action)) return;
    QListWidgetItem *item = m_actionToItem.value(action);
    m_actions.removeOne(action);
    m_actionToItem.remove(action);
    m_itemToAction.remove(item);
    if (item) {
        m_view->takeItem(m_view->row(item));
        item->setData(Qt::UserRole, QVariant());
        delete item;
    }
    QMenu::removeAction(action);
}

void MenuBase::clear() {
    for (int i = m_actions.size() - 1; i >= 0; --i) {
        removeAction(m_actions[i]);
    }
}

void MenuBase::addWidget(QWidget *widget, bool selectable, const std::function<void()> &onClick) {
    QAction *action = new QAction(this);
    // Store selectable state property if needed, but here we control via flags
    QListWidgetItem *item = createActionItem(action);
    item->setSizeHint(widget->size());
    m_view->addItem(item);
    m_view->setItemWidget(item, widget);
    if (!selectable) {
        item->setFlags(Qt::NoItemFlags);
    }
    if (onClick) {
        connect(action, &QAction::triggered, this, [onClick](bool){ onClick(); });
    }
    adjustSize();
}

void MenuBase::onSubMenuShowSignal(QListWidgetItem *item) {
    m_lastHoverItem = item;
    m_lastHoverSubMenuItem = item;
    // Delay to anti-shake
    m_timer->stop();
    m_timer->start();
}

QPair<QListWidgetItem*, SubMenuItemWidget*> MenuBase::createSubMenuItem(MenuBase *menu) {
    m_subMenus.append(menu);
    QListWidgetItem *item = new QListWidgetItem(createItemIcon(menu), menu->title());
    int w = 0;
    if (!hasItemIcon()) {
        w = 60 + m_view->fontMetrics().boundingRect(menu->title()).width();
    } else {
        // Add a blank character to increase space between icon and text
        item->setText(" " + item->text());
        w = 72 + m_view->fontMetrics().boundingRect(item->text()).width();
    }
    // Add submenu item
    menu->_setParentMenu(this, item);
    item->setSizeHint(QSize(w, m_itemHeight));
    SubMenuItemWidget *wWidget = new SubMenuItemWidget(menu, item, this);
    connect(wWidget, &SubMenuItemWidget::showMenuSig, this, &MenuBase::onSubMenuShowSignal);
    wWidget->resize(item->sizeHint());
    return qMakePair(item, wWidget);
}

void MenuBase::addMenu(MenuBase *menu) {
    QPair<QListWidgetItem*, SubMenuItemWidget*> pair = createSubMenuItem(menu);
    m_view->addItem(pair.first);
    m_view->setItemWidget(pair.first, pair.second);
    adjustSize();
}

void MenuBase::insertMenu(QAction *before, MenuBase *menu) {
    if (!m_actions.contains(before)) return;
    QPair<QListWidgetItem*, SubMenuItemWidget*> pair = createSubMenuItem(menu);
    QListWidgetItem *beforeItem = m_actionToItem.value(before);
    m_view->insertItem(m_view->row(beforeItem), pair.first);
    m_view->setItemWidget(pair.first, pair.second);
    adjustSize();
}

void MenuBase::onShowMenuTimeOut() {
    if (!m_lastHoverSubMenuItem || m_lastHoverItem != m_lastHoverSubMenuItem) return;
    SubMenuItemWidget *w = qobject_cast<SubMenuItemWidget*>(m_view->itemWidget(m_lastHoverSubMenuItem));
    if (!w) return;
    if (w->m_menu->m_parentMenu && w->m_menu->m_parentMenu->isHidden()) return;
    QPoint pos = w->mapToGlobal(QPoint(w->width() + 5, -5));
    w->m_menu->exec(pos);
}

void MenuBase::addSeparator() {
    QMargins m = m_view->viewportMargins();
    int w = m_view->width() - m.left() - m.right();
    QListWidgetItem *item = new QListWidgetItem();
    item->setFlags(Qt::NoItemFlags);
    item->setSizeHint(QSize(w, 9));
    m_view->addItem(item);
    item->setData(Qt.DecorationRole, "seperator");
    adjustSize();
}

void MenuBase::closeEvent(QCloseEvent *event) {
    event->accept();
    // closedSignal emit if needed
    m_view->clearSelection();
}

void MenuBase::hideMenu(bool isHideBySystem) {
    m_isHideBySystem = isHideBySystem;
    m_view->clearSelection();
    if (m_isSubMenu) {
        hide();
    } else {
        close();
    }
}

void MenuBase::closeParentMenu() {
    MenuBase *menu = this;
    while (menu) {
        menu->close();
        menu = menu->m_parentMenu;
    }
}

void MenuBase::hideEvent(QHideEvent *event) {
    if (m_isHideBySystem && m_isSubMenu) {
        closeParentMenu();
    }
    m_isHideBySystem = true;
    event->accept();
}

void MenuBase::mousePressEvent(QMouseEvent *event) {
    QWidget *widget = childAt(event->pos());
    if (widget != m_view && !m_view->isAncestorOf(widget)) {
        hideMenu(true);
    }
}

void MenuBase::mouseMoveEvent(QMouseEvent *event) {
    if (!m_isSubMenu) {
        return;
    }
    // Hide submenu when mouse moves out of submenu item
    QPoint globalPos = event->globalPos();
    MenuActionListWidget *parentView = m_parentMenu->m_view;
    // Get the rect of menu item
    QMargins margin = parentView->viewportMargins();
    QRect rect = parentView->visualItemRect(m_menuItem).translated(parentView->mapToGlobal(QPoint()));
    rect = rect.translated(margin.left(), margin.top() + 2);
    if (m_parentMenu->geometry().contains(globalPos) && !rect.contains(globalPos) && 
        !geometry().contains(globalPos)) {
        parentView->clearSelection();
        hideMenu(false);
    }
}

void MenuBase::onItemClicked(QListWidgetItem *item) {
    QAction *action = m_itemToAction.value(item);
    if (action) {
        action->trigger();
        close();
    }
}

void MenuBase::onItemEntered(QListWidgetItem *item) {
    m_lastHoverItem = item;
    // Check if this item contains a submenu (check if it has a SubMenuItemWidget)
    QWidget *widget = m_view->itemWidget(item);
    if (widget && qobject_cast<SubMenuItemWidget*>(widget)) {
        onSubMenuShowSignal(item);
    }
}

void MenuBase::adjustPosition() {
    QMargins margins = layout()->contentsMargins();
    QRect screenRect = getScreenGeometry(getCurrentScreen());
    int w = layout()->sizeHint().width() + 5;
    int h = layout()->sizeHint().height();
    int x = std::min(this->x() - margins.left(), screenRect.right() - w);
    int y = this->y();
    if (y > screenRect.bottom() - h) {
        y = this->y() - h + margins.bottom();
    }
    move(x, y);
}

void MenuBase::exec(const QPoint &pos, Direction animType) {
    m_view->adjustSize(pos, animType);
    adjustSize();
    MenuAnimationManager *manager = MenuAnimationManager::make(this, animType);
    manager->exec(pos);
    delete manager;
    show();
}

void MenuBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}