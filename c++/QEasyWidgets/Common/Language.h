#ifndef QEASYWIDGETS_LANGUAGE_H
#define QEASYWIDGETS_LANGUAGE_H

#include <QString>
#include <QLocale>


namespace QEW {

/**
 * @brief Language enumeration
 */
enum class Language {
    ZH,
    EN
};

/**
 * @brief Convert language to string
 */
inline QString languageString(Language lang) {
    switch (lang) {
        case Language::ZH: return "zh";
        case Language::EN: return "en";
        default: return "zh";
    }
}

/**
 * @brief Convert string to language
 */
inline Language stringToLanguage(const QString &str) {
    if (str == "zh") return Language::ZH;
    if (str == "en") return Language::EN;
    return Language::ZH;
}

/**
 * @brief Language manager
 */
class LanguageBase
{
public:
    LanguageBase();
    ~LanguageBase() = default;

    Language currentLanguage() const;
    void setLanguage(Language lang);
    void setLanguage(const QString &langStr);

private:
    Language m_currentLang;
};

/**
 * @brief Global language instance
 */
extern LanguageBase EasyLanguage;

/**
 * @brief Get current language
 */
inline Language currentLanguage() {
    return EasyLanguage.currentLanguage();
}

} // namespace QEW

#endif // QEASYWIDGETS_LANGUAGE_H