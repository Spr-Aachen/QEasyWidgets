#ifndef QEASYWIDGETS_MENU_H
#define QEASYWIDGETS_MENU_H

#include <QMenu>
#include <QListWidget>
#include <QStyledItemDelegate>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QPainter>
#include <QEvent>
#include <QAction>
#include <QApplication>
#include <QEnterEvent>
#include <QHash>
#include <QPair>
#include <functional>

#include "../Common/Config.h"
#include "../Common/Icon.h"


class MenuBase;

/**
 * Menu item delegate
 */
class MenuItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit MenuItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    bool isSeparator(const QModelIndex &index) const;
};

/**
 * Menu animation manager
 */
class MenuAnimationManager : public QObject {
    Q_OBJECT

public:
    explicit MenuAnimationManager(MenuBase *menu);
    virtual ~MenuAnimationManager() = default;

    virtual void exec(const QPoint &pos);
    virtual QSize availableViewSize(const QPoint &pos);

    static MenuAnimationManager* make(MenuBase *menu, Direction animType);

protected:
    MenuBase *m_menu;
    QPropertyAnimation *m_anim;

    QSize menuSize() const;

protected slots:
    virtual void onValueChanged(const QVariant &value);
    void updateMenuViewport();
};

/**
 * Drop down menu animation manager
 */
class DropDownMenuAnimationManager : public MenuAnimationManager {
    Q_OBJECT

public:
    using MenuAnimationManager::MenuAnimationManager;
    void exec(const QPoint &pos) override;
    QSize availableViewSize(const QPoint &pos) override;

protected slots:
    void onValueChanged(const QVariant &value) override;
};

/**
 * Pull up menu animation manager
 */
class PullUpMenuAnimationManager : public MenuAnimationManager {
    Q_OBJECT

public:
    using MenuAnimationManager::MenuAnimationManager;
    void exec(const QPoint &pos) override;
    QSize availableViewSize(const QPoint &pos) override;

protected slots:
    void onValueChanged(const QVariant &value) override;
};

/**
 * Menu action list widget
 */
class MenuActionListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit MenuActionListWidget(QWidget *parent = nullptr);

    void adjustSize(const QPoint &pos = QPoint(), Direction animType = Direction::Down);
    
    // Override QListWidget methods to trigger adjustSize
    void insertItem(int row, QListWidgetItem *item);
    void addItem(QListWidgetItem *item);
    QListWidgetItem *takeItem(int row);

    int maxVisibleItems() const;
    void setMaxVisibleItems(int num);

    void setItemHeight(int height);
    int itemsHeight() const;
    int heightForAnimation(const QPoint &pos, Direction animType);

private:
    int m_itemHeight;
    int m_maxVisibleItems;
};

/**
 * Sub menu item
 */
class SubMenuItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit SubMenuItemWidget(MenuBase *menu, QListWidgetItem *item, QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void showMenuSig(QListWidgetItem *item);

public:
    MenuBase *m_menu;
    QListWidgetItem *m_item;
};

/**
 * Enhanced menu with theme support
 */
class MenuBase : public QMenu {
    Q_OBJECT

public:
    explicit MenuBase(QWidget *parent = nullptr);
    explicit MenuBase(const QString &title, QWidget *parent = nullptr);
    ~MenuBase() override = default;

    void exec(const QPoint &pos, Direction animType = Direction::Down);
    
    void setMaxVisibleItems(int num);
    void setItemHeight(int height);
    
    QString title() const;
    QIcon icon() const;
    void setIcon(const QIcon &icon);
    void setIcon(IconBase icon);

    void addAction(QAction *action);
    void addActions(const QList<QAction*> &actions);
    void insertAction(QAction *before, QAction *action);
    void insertActions(QAction *before, const QList<QAction*> &actions);
    void removeAction(QAction *action);
    void clear();

    void addWidget(QWidget *widget, bool selectable = true, const std::function<void()> &onClick = nullptr);
    void addMenu(MenuBase *menu);
    void insertMenu(QAction *before, MenuBase *menu);
    void addSeparator();

    // Helper for submenus
    void _setParentMenu(MenuBase *parent, QListWidgetItem *item);

    // Accessors for animation manager
    friend class MenuAnimationManager;
    friend class DropDownMenuAnimationManager;
    friend class PullUpMenuAnimationManager;
    friend class MenuActionListWidget;
    friend class SubMenuItemWidget;

    void clearDefaultStyleSheet();


    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void init();
    void setShadowEffect(int blurRadius = 30, const QPoint &offset = QPoint(0, 8), const QColor &color = QColor(0, 0, 0, 30));
    
    void adjustPosition();
    void hideMenu(bool isHideBySystem = false);
    void closeParentMenu();
    
    bool hasItemIcon() const;
    QIcon createItemIcon(QAction *action);
    int longestShortcutWidth() const;
    int adjustItemText(QListWidgetItem *item, QAction *action);
    QListWidgetItem* createActionItem(QAction *action, QAction *before = nullptr);
    QPair<QListWidgetItem*, SubMenuItemWidget*> createSubMenuItem(MenuBase *menu);

private slots:
    void onActionChanged();
    void onItemClicked(QListWidgetItem *item);
    void onItemEntered(QListWidgetItem *item);
    void onShowMenuTimeOut();
    void onSubMenuShowSignal(QListWidgetItem *item);

private:
    QString m_title;
    QIcon m_icon;
    QList<QAction*> m_actions;
    QList<MenuBase*> m_subMenus;

    bool m_isSubMenu;
    MenuBase *m_parentMenu;
    QListWidgetItem *m_menuItem;
    QListWidgetItem *m_lastHoverItem;
    QListWidgetItem *m_lastHoverSubMenuItem;
    bool m_isHideBySystem;
    int m_itemHeight;

    QHBoxLayout *m_hBoxLayout;
    MenuActionListWidget *m_view;
    QTimer *m_timer;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    // Mapping for action <-> item
    QHash<QAction*, QListWidgetItem*> m_actionToItem;
    QHash<QListWidgetItem*, QAction*> m_itemToAction;
};


#endif // QEASYWIDGETS_MENU_H