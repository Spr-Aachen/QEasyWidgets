#include "StyleSheet.h"
#include "Signals.h"
#include "Config.h"

#include <QFile>
#include <QTextStream>


QMap<QString, QString> StyleSheetBase::m_styleSheetCache;
QMap<QPointer<QWidget>, StyleSheetBase::StyleType> StyleSheetBase::m_registeredWidgets;


bool StyleSheetBase::loadStyleSheet(const QString &fileName, Theme theme, QString &styleSheet) {
    QString themeName = ThemeString(theme);
    QString cacheKey = QString("%1_%2").arg(fileName).arg(themeName);
    
    // Check cache first
    auto cached = m_styleSheetCache.constFind(cacheKey);
    if (cached != m_styleSheetCache.constEnd()) {
        styleSheet = cached.value();
        return true;
    }
    
    // Load from resource file
    QString filePath = QString(":/QSS/qss/%1/%2.qss").arg(themeName).arg(fileName);
    QFile file(filePath);
    
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
    styleSheet = stream.readAll();
    file.close();
    
    // Cache the stylesheet
    m_styleSheetCache[cacheKey] = styleSheet;
    
    return true;
}


bool StyleSheetBase::tryGetStyleSheet(StyleType type, Theme theme, QString &styleSheet) {
    static const char* const typeNames[] = {
        "Label", "ToolTip", "Button", "CheckBox", "ScrollArea",
        "Tree", "List", "ToolBox", "GroupBox", "Slider",
        "SpinBox", "ComboBox", "Edit", "Browser", "ProgressBar",
        "Player", "Tab", "Table", "ChatWidget", "Bar",
        "DockWidget", "Menu"
    };
    const int typeCount = int(sizeof(typeNames) / sizeof(typeNames[0]));
    
    if (type < 0 || type >= typeCount) {
        return false;
    }
    
    return loadStyleSheet(QString::fromLatin1(typeNames[type]), theme, styleSheet);
}


QString StyleSheetBase::getStyleSheet(StyleType type, Theme theme) {
    QString styleSheet;
    tryGetStyleSheet(type, theme, styleSheet);
    return styleSheet;
}


void StyleSheetBase::registrate(QWidget *widget, StyleType type) {
    if (widget) {
        m_registeredWidgets[widget] = type;
    }
}


void StyleSheetBase::deregistrate(QWidget *widget) {
    m_registeredWidgets.remove(widget);
}


void StyleSheetBase::apply(QWidget *widget, StyleType type, Theme theme, bool registrate) {
    if (!widget) {
        return;
    }
    
    QString styleSheet;
    // Skip setStyleSheet() if the resource is missing, otherwise the widget's current stylesheet would be cleared
    if (tryGetStyleSheet(type, theme, styleSheet)) {
        widget->setStyleSheet(styleSheet);
    }
    
    if (registrate) {
        StyleSheetBase::registrate(widget, type);
    }
}


void StyleSheetBase::updateAllStyleSheets(Theme theme) {
    // Iterate through registered widgets and update their stylesheets
    auto it = m_registeredWidgets.begin();
    while (it != m_registeredWidgets.end()) {
        QPointer<QWidget> widget = it.key();
        StyleType type = it.value();
        
        if (widget.isNull()) {
            // Widget was deleted, remove from registry
            it = m_registeredWidgets.erase(it);
        } else {
            // Update stylesheet
            apply(widget, type, theme, false);
            ++it;
        }
    }
}


// Initialize connection to theme change signal
static bool initStyleSheetSignals() {
    if (componentsSignals) {
        QObject::connect(componentsSignals, &CustomSignals::setTheme, [](const QString &) {
            Theme theme = currentTheme();
            StyleSheetBase::updateAllStyleSheets(theme);
        });
    }
    return true;
}


static bool g_styleSheetSignalsInitialized = initStyleSheetSignals();