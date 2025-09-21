#ifndef QEASYWIDGETS_SIGNALS_H
#define QEASYWIDGETS_SIGNALS_H

#include <QObject>


/**
 * Custom signals for components
 */
class CustomSignals : public QObject {
    Q_OBJECT

public:
    explicit CustomSignals(QObject *parent = nullptr);

signals:
    // Set theme
    void setTheme(const QString &theme);

    // Set language
    void setLanguage(const QString &language);
};


/**
 * Global signals instance
 */
extern CustomSignals *componentsSignals;


#endif // QEASYWIDGETS_SIGNALS_H