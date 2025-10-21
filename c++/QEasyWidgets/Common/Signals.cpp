#include "Signals.h"
#include "Translator.h"
#include "Theme.h"

namespace QEW {

CustomSignals::CustomSignals(QObject *parent)
    : QObject(parent)
{
    // Connect language change signal
    connect(this, &CustomSignals::setLanguage, this, [](const QString &lang) {
        updateLanguage(lang);
    });
    
    // Connect theme change signal
    connect(this, &CustomSignals::setTheme, this, [](const QString &themeStr) {
        if (themeStr == "light" || themeStr == ThemeString(LIGHT)) {
            setCurrentTheme(LIGHT);
        } else if (themeStr == "dark" || themeStr == ThemeString(DARK)) {
            setCurrentTheme(DARK);
        } else if (themeStr == "auto" || themeStr == ThemeString(AUTO)) {
            setCurrentTheme(detectSystemTheme());
        }
    });
}

CustomSignals *componentsSignals = new CustomSignals();

} // namespace QEW