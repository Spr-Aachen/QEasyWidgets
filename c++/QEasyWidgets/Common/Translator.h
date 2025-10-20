#ifndef QEASYWIDGETS_TRANSLATOR_H
#define QEASYWIDGETS_TRANSLATOR_H

#include <QTranslator>
#include <QApplication>

#include "Language.h"
#include "Signals.h"


namespace QEW {

/**
 * @brief Translation base class
 */
class TranslationBase : public QTranslator
{
    Q_OBJECT

public:
    explicit TranslationBase(QObject *parent = nullptr);
    ~TranslationBase() override = default;

    bool load(Language language);
    bool load(const QString &languageStr = QString());

private:
    QString getTranslationFilePath(Language language) const;
};

/**
 * @brief Global translator instance
 */
extern TranslationBase *translator;

/**
 * @brief Update language and install translator
 */
void updateLanguage(Language language);
void updateLanguage(const QString &languageStr = QString());

} // namespace QEW

#endif // QEASYWIDGETS_TRANSLATOR_H