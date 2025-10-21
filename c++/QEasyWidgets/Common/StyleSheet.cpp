#include "StyleSheet.h"
#include "Signals.h"

#include <QFile>
#include <QTextStream>


QMap<QString, QString> StyleSheetBase::m_styleSheetCache;
QMap<QPointer<QWidget>, StyleSheetBase::StyleType> StyleSheetBase::m_registeredWidgets;


QString StyleSheetBase::loadStyleSheet(const QString &fileName, Theme theme) {
    QString themeName = ThemeString(theme);
    QString cacheKey = QString("%1_%2").arg(fileName).arg(themeName);
    
    // Check cache first
    if (m_styleSheetCache.contains(cacheKey)) {
        return m_styleSheetCache[cacheKey];
    }
    
    // Load from resource file
    QString filePath = QString(":/QSS/qss/%1/%2.qss").arg(themeName).arg(fileName);
    QFile file(filePath);
    
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    }
    
    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    file.close();
    
    // Cache the stylesheet
    m_styleSheetCache[cacheKey] = styleSheet;
    
    return styleSheet;
}


QString StyleSheetBase::getStyleSheet(StyleType type, Theme theme) {
    QString fileName;
    
    switch (type) {
        case Label: fileName = "Label"; break;
        case Button: fileName = "Button"; break;
        case CheckBox: fileName = "CheckBox"; break;
        case ScrollArea: fileName = "ScrollArea"; break;
        case Tree: fileName = "Tree"; break;
        case List: fileName = "List"; break;
        case ToolBox: fileName = "ToolBox"; break;
        case GroupBox: fileName = "GroupBox"; break;
        case Slider: fileName = "Slider"; break;
        case SpinBox: fileName = "SpinBox"; break;
        case ComboBox: fileName = "ComboBox"; break;
        case Edit: fileName = "Edit"; break;
        case Browser: fileName = "Browser"; break;
        case ProgressBar: fileName = "ProgressBar"; break;
        case Player: fileName = "Player"; break;
        case Tab: fileName = "Tab"; break;
        case Table: fileName = "Table"; break;
        case ChatWidget: fileName = "ChatWidget"; break;
        case Bar: fileName = "Bar"; break;
        case DockWidget: fileName = "DockWidget"; break;
        case Menu: fileName = "Menu"; break;
        default: return QString();
    }
    
    return loadStyleSheet(fileName, theme);
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
    
    QString styleSheet = getStyleSheet(type, theme);
    widget->setStyleSheet(styleSheet);
    
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
    QObject::connect(componentsSignals, &CustomSignals::setTheme, [](const QString &themeStr) {
        Theme theme = currentTheme();
        StyleSheetBase::updateAllStyleSheets(theme);
    });
    return true;
}


static bool g_styleSheetSignalsInitialized = initStyleSheetSignals();