#include "StyleSheet.h"
#include "Signals.h"

#include <QFile>
#include <QTextStream>

namespace QEW {

QMap<QString, QString> StyleSheetBase::m_styleSheetCache;
QMap<QPointer<QWidget>, StyleSheetBase::StyleType> StyleSheetBase::m_registeredWidgets;

QString StyleSheetBase::getStyleSheet(StyleType type, Theme theme)
{
    QString fileName;
    
    switch (type) {
        case Button: fileName = "Button"; break;
        case Label: fileName = "Label"; break;
        case LineEdit: fileName = "Edit"; break;
        case ComboBox: fileName = "ComboBox"; break;
        case CheckBox: fileName = "CheckBox"; break;
        case SpinBox: fileName = "SpinBox"; break;
        case Slider: fileName = "Slider"; break;
        case ProgressBar: fileName = "ProgressBar"; break;
        case ScrollArea: fileName = "ScrollArea"; break;
        case Menu: fileName = "Menu"; break;
        case Dialog: fileName = "Dialog"; break;
        case Frame: fileName = "Frame"; break;
        case GroupBox: fileName = "GroupBox"; break;
        case TabWidget: fileName = "Tab"; break;
        case TreeView: fileName = "Tree"; break;
        case TableView: fileName = "Table"; break;
        case ListView: fileName = "List"; break;
        default: return QString();
    }
    
    return loadStyleSheet(fileName, theme);
}

void StyleSheetBase::apply(QWidget *widget, StyleType type, Theme theme, bool registrate)
{
    if (!widget) {
        return;
    }
    
    QString styleSheet = getStyleSheet(type, theme);
    widget->setStyleSheet(styleSheet);
    
    if (registrate) {
        StyleSheetBase::registrate(widget, type);
    }
}

void StyleSheetBase::registrate(QWidget *widget, StyleType type)
{
    if (widget) {
        m_registeredWidgets[widget] = type;
    }
}

void StyleSheetBase::deregistrate(QWidget *widget)
{
    m_registeredWidgets.remove(widget);
}

void StyleSheetBase::updateAllStyleSheets(Theme theme)
{
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

QString StyleSheetBase::loadStyleSheet(const QString &fileName, Theme theme)
{
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

// Initialize connection to theme change signal
static bool initStyleSheetSignals()
{
    QObject::connect(componentsSignals, &CustomSignals::setTheme, [](const QString &themeStr) {
        Theme theme = currentTheme();
        StyleSheetBase::updateAllStyleSheets(theme);
    });
    return true;
}

static bool g_styleSheetSignalsInitialized = initStyleSheetSignals();

} // namespace QEW