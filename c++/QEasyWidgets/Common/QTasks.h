#ifndef QEASYWIDGETS_QTASKS_H
#define QEASYWIDGETS_QTASKS_H

#include <QThread>
#include <QMutex>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QTextStream>


namespace QEW {

/**
 * @brief Console output handler thread
 */
class ConsoleOutputHandler : public QThread
{
    Q_OBJECT

public:
    explicit ConsoleOutputHandler(QObject *parent = nullptr);
    ~ConsoleOutputHandler() override;

    void write(const QString &info);

signals:
    void consoleInfo(const QString &info);

protected:
    void run() override;

private:
    QMutex m_mutex;
    QString m_buffer;
};

/**
 * @brief System usage monitor thread
 */
class MonitorUsage : public QThread
{
    Q_OBJECT

public:
    explicit MonitorUsage(QObject *parent = nullptr);
    ~MonitorUsage() override;

signals:
    void usageInfo(const QString &cpuUsage, const QString &gpuUsage);

protected:
    void run() override;

private:
    bool m_isNVIDIAGPU;
};

/**
 * @brief File content monitor thread
 */
class MonitorFile : public QThread
{
    Q_OBJECT

public:
    explicit MonitorFile(const QString &filePath, QObject *parent = nullptr);
    ~MonitorFile() override;

signals:
    void fileContent(const QString &content);

protected:
    void run() override;

private:
    QString m_filePath;
    QString m_previousContent;
};

/**
 * @brief Log file monitor thread
 */
class MonitorLogFile : public QThread
{
    Q_OBJECT

public:
    explicit MonitorLogFile(const QString &logPath, QObject *parent = nullptr);
    ~MonitorLogFile() override;

    void clear();

signals:
    void consoleInfo(const QString &content);

protected:
    void run() override;

private:
    QString m_logPath;
    QString m_previousContent;
};

} // namespace QEW

#endif // QEASYWIDGETS_QTASKS_H