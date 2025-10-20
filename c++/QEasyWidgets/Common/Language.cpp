#include "Language.h"


namespace QEW {

LanguageBase::LanguageBase()
    : m_currentLang(Language::ZH)
{
    // Detect system language
    QLocale systemLocale = QLocale::system();
    QString lang = systemLocale.languageToString(systemLocale.language()).toLower();
    if (lang.contains("chinese") || lang == "zh") {
        m_currentLang = Language::ZH;
    } else {
        m_currentLang = Language::EN;
    }
}

Language LanguageBase::currentLanguage() const
{
    return m_currentLang;
}

void LanguageBase::setLanguage(Language lang)
{
    m_currentLang = lang;
}

void LanguageBase::setLanguage(const QString &langStr)
{
    m_currentLang = stringToLanguage(langStr);
}

LanguageBase EasyLanguage;

} // namespace QEW