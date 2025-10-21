#include "Translator.h"

#include <QDir>
#include <QCoreApplication>


TranslationBase::TranslationBase(QObject *parent)
    : QTranslator(parent) {
}


bool TranslationBase::load(Language language) {
    QString filePath = getTranslationFilePath(language);
    return QTranslator::load(filePath);
}


bool TranslationBase::load(const QString &languageStr) {
    Language lang = languageStr.isEmpty() ? currentLanguage() : stringToLanguage(languageStr);
    return load(lang);
}


QString TranslationBase::getTranslationFilePath(Language language) const {
    QString langStr = languageString(language);
    QString filePath = QString(":/QM/i18n/%1.qm").arg(langStr);
    return filePath;
}


TranslationBase *translator = nullptr;


void updateLanguage(Language language) {
    if (!QApplication::instance()) {
        return;
    }

    QApplication::instance()->processEvents();

    // Remove old translator
    if (translator) {
        QApplication::instance()->removeTranslator(translator);
        delete translator;
        translator = nullptr;
    }

    // Install new translator
    translator = new TranslationBase();
    if (translator->load(language)) {
        QApplication::instance()->installTranslator(translator);
    } else {
        delete translator;
        translator = nullptr;
    }

    EasyLanguage.setLanguage(language);
}


void updateLanguage(const QString &languageStr) {
    Language lang = languageStr.isEmpty() ? currentLanguage() : stringToLanguage(languageStr);
    updateLanguage(lang);
}