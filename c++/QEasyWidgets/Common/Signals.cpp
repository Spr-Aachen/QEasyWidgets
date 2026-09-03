#include "Signals.h"
#include "Translator.h"
#include "Theme.h"


CustomSignals::CustomSignals(QObject *parent)
    : QObject(parent) {
    // Connect language change signal
    connect(this, &CustomSignals::setLanguage, this, [](const QString &lang) {
        updateLanguage(lang);
    });
    
    // Connect theme change signal
    connect(this, &CustomSignals::setTheme, this, [](const QString &themeStr) {
        if (themeStr == "light" || themeStr == ThemeString(Theme::Light)) {
            setCurrentTheme(Theme::Light);
        } else if (themeStr == "dark" || themeStr == ThemeString(Theme::Dark)) {
            setCurrentTheme(Theme::Dark);
        } else if (themeStr == "auto" || themeStr == ThemeString(Theme::Auto)) {
            setCurrentTheme(Theme::Auto);
        }
    });
}


CustomSignals *componentsSignals = new CustomSignals();