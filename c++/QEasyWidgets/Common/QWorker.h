#ifndef QEASYWIDGETS_QWORKER_H
#define QEASYWIDGETS_QWORKER_H

#include <QRunnable>
#include <QObject>
#include <QThreadPool>
#include <QVariant>


namespace QEW {

/**
 * @brief Worker signals
 */
class WorkerSignals : public QObject
{
    Q_OBJECT

public:
    explicit WorkerSignals(QObject *parent = nullptr);

signals:
    void started();
    void finished();
    void error(const QString &errorMessage);
    void result(const QVariant &result);
};

/**
 * @brief Worker runnable class
 */
class Worker : public QRunnable
{
public:
    explicit Worker(bool autoDelete = true);
    ~Worker() override = default;

    void setTask(std::function<QVariant()> task);
    void setTask(std::function<void()> task);

    WorkerSignals* signals() const { return m_signals; }

    void run() override;

private:
    WorkerSignals *m_signals;
    std::function<QVariant()> m_taskWithResult;
    std::function<void()> m_taskWithoutResult;
    bool m_hasResult;
};

/**
 * @brief Worker manager
 */
class WorkerManager : public QObject
{
    Q_OBJECT

public:
    explicit WorkerManager(QThreadPool *threadPool = nullptr, QObject *parent = nullptr);
    ~WorkerManager() override;

    void setExecuteTask(std::function<QVariant()> task);
    void setTerminateTask(std::function<void()> task);

    void execute();
    void terminate();

private:
    QThreadPool *m_threadPool;
    Worker *m_worker;
    Worker *m_terminateWorker;
    std::function<QVariant()> m_executeTask;
    std::function<void()> m_terminateTask;
};

} // namespace QEW

#endif // QEASYWIDGETS_QWORKER_H