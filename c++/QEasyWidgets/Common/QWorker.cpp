#include "QWorker.h"

#include <QDebug>
#include <QException>


WorkerSignals::WorkerSignals(QObject *parent)
    : QObject(parent)
{
}


Worker::Worker(bool autoDelete)
    : m_signals(new WorkerSignals())
    , m_hasResult(false)
{
    setAutoDelete(autoDelete);
}


void Worker::setTask(std::function<QVariant()> task) {
    m_taskWithResult = task;
    m_hasResult = true;
}


void Worker::setTask(std::function<void()> task) {
    m_taskWithoutResult = task;
    m_hasResult = false;
}


void Worker::run() {
    emit m_signals->started();

    try {
        if (m_hasResult && m_taskWithResult) {
            QVariant result = m_taskWithResult();
            emit m_signals->result(result);
        } else if (!m_hasResult && m_taskWithoutResult) {
            m_taskWithoutResult();
        }
    } catch (const std::exception &e) {
        emit m_signals->error(QString::fromStdString(e.what()));
    } catch (...) {
        emit m_signals->error("Unknown error occurred");
    }

    emit m_signals->finished();
}


WorkerManager::WorkerManager(QThreadPool *threadPool, QObject *parent)
    : QObject(parent)
    , m_threadPool(threadPool ? threadPool : QThreadPool::globalInstance())
    , m_worker(nullptr)
    , m_terminateWorker(nullptr)
{
}


WorkerManager::~WorkerManager() {
    if (m_worker) {
        delete m_worker;
    }
    if (m_terminateWorker) {
        delete m_terminateWorker;
    }
}


void WorkerManager::setExecuteTask(std::function<QVariant()> task) {
    m_executeTask = task;
}


void WorkerManager::setTerminateTask(std::function<void()> task) {
    m_terminateTask = task;
}


void WorkerManager::execute() {
    if (!m_executeTask) return;

    if (m_worker) {
        delete m_worker;
    }

    m_worker = new Worker(true);
    m_worker->setTask(m_executeTask);
    m_threadPool->start(m_worker);
}


void WorkerManager::terminate() {
    if (!m_terminateTask) return;

    if (m_terminateWorker) {
        delete m_terminateWorker;
    }

    m_terminateWorker = new Worker(true);
    m_terminateWorker->setTask(m_terminateTask);
    m_threadPool->start(m_terminateWorker);
}