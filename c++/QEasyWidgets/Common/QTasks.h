#ifndef QEASYWIDGETS_QTASKS_H
#define QEASYWIDGETS_QTASKS_H

#include <QThread>
#include <QMutex>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QTextStream>


/**
 * Console output handler thread
 */
class ConsoleOutputHandler : public QThread {
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
 * System usage monitor thread
 */
class MonitorUsage : public QThread {
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
 * File content monitor thread
 */
class MonitorFile : public QThread {
    Q_OBJECT

public:
    explicit MonitorFile(const QString &filePath, const QString &mode = "append", QObject *parent = nullptr);
    ~MonitorFile() override;

signals:
    void contentChanged(const QString &content);

protected:
    void run() override;
    void clear();

private:
    QString m_filePath;
    QString m_mode;
    qint64 m_pos;
    QString m_contentPrev;

    void resetPos();
    void resetContentPrev();
};


#endif // QEASYWIDGETS_QTASKS_H