#include "QTasks.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>
#include <QSysInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#elif defined(Q_OS_LINUX)
#include <fstream>
#include <sstream>
#include <unistd.h>
#elif defined(Q_OS_MACOS)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#endif


namespace QEW {

ConsoleOutputHandler::ConsoleOutputHandler(QObject *parent)
    : QThread(parent)
{
}

ConsoleOutputHandler::~ConsoleOutputHandler()
{
    quit();
    wait();
}

void ConsoleOutputHandler::write(const QString &info)
{
    QMutexLocker locker(&m_mutex);
    m_buffer.append(info);
}

void ConsoleOutputHandler::run()
{
    // Note: In a real implementation, you would redirect stdout/stderr
    // This is a simplified version
    while (!isInterruptionRequested()) {
        {
            QMutexLocker locker(&m_mutex);
            if (!m_buffer.isEmpty()) {
                emit consoleInfo(m_buffer);
                m_buffer.clear();
            }
        }
        msleep(100);
    }
}

MonitorUsage::MonitorUsage(QObject *parent)
    : QThread(parent)
    , m_isNVIDIAGPU(false)
{
#ifdef Q_OS_WIN
    // Check for NVIDIA GPU by looking for nvidia-smi
    QProcess process;
    process.start("nvidia-smi", QStringList() << "--query-gpu=name" << "--format=csv,noheader");
    if (process.waitForFinished(1000)) {
        m_isNVIDIAGPU = (process.exitCode() == 0);
    }
#endif
}

MonitorUsage::~MonitorUsage()
{
    quit();
    wait();
}

void MonitorUsage::run()
{
#ifdef Q_OS_WIN
    // Windows implementation using PDH (Performance Data Helper)
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
#endif

    while (!isInterruptionRequested()) {
        QString cpuUsage = "0%";
        QString gpuUsage = "0%";

#ifdef Q_OS_WIN
        // Get CPU usage on Windows
        PDH_FMT_COUNTERVALUE counterVal;
        PdhCollectQueryData(cpuQuery);
        PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
        cpuUsage = QString("%1%").arg(counterVal.doubleValue, 0, 'f', 1);

        // Get GPU usage if NVIDIA GPU is available
        if (m_isNVIDIAGPU) {
            QProcess process;
            process.start("nvidia-smi", QStringList() << "--query-gpu=utilization.gpu" << "--format=csv,noheader,nounits");
            if (process.waitForFinished(500)) {
                QString output = process.readAllStandardOutput().trimmed();
                if (!output.isEmpty()) {
                    gpuUsage = output + "%";
                }
            }
        }
#elif defined(Q_OS_LINUX)
        // Linux implementation using /proc/stat
        static unsigned long long lastTotalUser = 0, lastTotalUserLow = 0, lastTotalSys = 0, lastTotalIdle = 0;
        std::ifstream file("/proc/stat");
        std::string line;
        if (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string cpu;
            unsigned long long user, nice, system, idle;
            ss >> cpu >> user >> nice >> system >> idle;
            
            if (lastTotalUser != 0) {
                unsigned long long total = (user - lastTotalUser) + (nice - lastTotalUserLow) + (system - lastTotalSys);
                unsigned long long totalIdle = idle - lastTotalIdle;
                double percent = (total * 100.0) / (total + totalIdle);
                cpuUsage = QString("%1%").arg(percent, 0, 'f', 1);
            }
            
            lastTotalUser = user;
            lastTotalUserLow = nice;
            lastTotalSys = system;
            lastTotalIdle = idle;
        }
        file.close();
#elif defined(Q_OS_MACOS)
        // macOS implementation
        host_cpu_load_info_data_t cpuinfo;
        mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
        if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
            unsigned long long totalTicks = 0;
            for (int i = 0; i < CPU_STATE_MAX; i++) totalTicks += cpuinfo.cpu_ticks[i];
            double percent = 100.0 * (totalTicks - cpuinfo.cpu_ticks[CPU_STATE_IDLE]) / totalTicks;
            cpuUsage = QString("%1%").arg(percent, 0, 'f', 1);
        }
#endif

        emit usageInfo(cpuUsage, gpuUsage);
        msleep(1000);
    }

#ifdef Q_OS_WIN
    PdhCloseQuery(cpuQuery);
#endif
}

MonitorFile::MonitorFile(const QString &filePath, QObject *parent)
    : QThread(parent)
    , m_filePath(filePath)
{
}

MonitorFile::~MonitorFile()
{
    quit();
    wait();
}

void MonitorFile::run()
{
    while (!isInterruptionRequested()) {
        QFile file(m_filePath);
        if (file.exists()) {
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                QString content = stream.readAll();
                file.close();

                if (content != m_previousContent) {
                    emit fileContent(content);
                    m_previousContent = content;
                }
            }
        } else {
            // Create the file if it doesn't exist
            QDir dir = QFileInfo(m_filePath).absoluteDir();
            if (dir.mkpath(dir.absolutePath())) {
                if (file.open(QIODevice::WriteOnly)) {
                    file.close();
                }
            }
            qDebug() << "File" << m_filePath << "not found, creating new one...";
        }

        msleep(100);
    }
}

MonitorLogFile::MonitorLogFile(const QString &logPath, QObject *parent)
    : QThread(parent)
    , m_logPath(logPath)
{
    if (!QFile::exists(m_logPath)) {
        QDir dir = QFileInfo(m_logPath).absoluteDir();
        if (dir.mkpath(dir.absolutePath())) {
            QFile file(m_logPath);
            if (file.open(QIODevice::WriteOnly)) {
                file.close();
            }
        }
    } else {
        clear();
    }
}

MonitorLogFile::~MonitorLogFile()
{
    quit();
    wait();
}

void MonitorLogFile::clear()
{
    QFile file(m_logPath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        file.close();
    }
}

void MonitorLogFile::run()
{
    while (!isInterruptionRequested()) {
        QFile file(m_logPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString content = stream.readAll();
            file.close();

            if (content != m_previousContent) {
                emit consoleInfo(content);
                m_previousContent = content;
            }
        }

        msleep(100);
    }
}

} // namespace QEW