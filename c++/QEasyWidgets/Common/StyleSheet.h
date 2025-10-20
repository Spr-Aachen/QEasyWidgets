#ifndef QEASYWIDGETS_STYLESHEET_H
#define QEASYWIDGETS_STYLESHEET_H

#include <QString>
#include <QWidget>
#include <QMap>
#include <QPointer>

#include "Theme.h"


namespace QEW {

/**
 * @brief StyleSheet manager for widgets
 */
class StyleSheetBase
{
public:
    enum StyleType {
        Button,
        Label,
        LineEdit,
        ComboBox,
        CheckBox,
        SpinBox,
        Slider,
        ProgressBar,
        ScrollArea,
        Menu,
        Dialog,
        Frame,
        GroupBox,
        TabWidget,
        TreeView,
        TableView,
        ListView
    };

    static QString getStyleSheet(StyleType type, Theme theme = LIGHT);
    static void apply(QWidget *widget, StyleType type, Theme theme = LIGHT, bool registrate = true);
    
    // Widget registration for theme updates
    static void registrate(QWidget *widget, StyleType type);
    static void deregistrate(QWidget *widget);
    static void updateAllStyleSheets(Theme theme);
    
private:
    static QString loadStyleSheet(const QString &fileName, Theme theme);
    static QMap<QString, QString> m_styleSheetCache;
    static QMap<QPointer<QWidget>, StyleType> m_registeredWidgets;
};

} // namespace QEW

#endif // QEASYWIDGETS_STYLESHEET_H