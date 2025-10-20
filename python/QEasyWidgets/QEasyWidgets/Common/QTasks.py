import os
import sys
import psutil
import pynvml
from pathlib import Path
from PySide6.QtCore import QThread, QMutex, Signal, Slot, QTimer, QEventLoop

##############################################################################################################################

# Handle the consol's output
class ConsolOutputHandler(QThread):
    '''
    Intercept the output of the consol and send to the UI (Not Recommended)
    '''
    consoleInfo = Signal(str)

    def __init__(self):
        super().__init__()

        self.Mutex = QMutex()

    def run(self):
        # Redirect stdout & stderr to the childthread
        sys.stdout = self
        sys.stderr = self

    def write(self,
        Info: object
    ):
        '''
        Function to override the default write functions of sys.stdout & sys.stderr
        '''
        self.Mutex.lock()

        self.consoleInfo.emit(str(Info))

        self.Mutex.unlock()

        EventLoop = QEventLoop()
        QTimer.singleShot(123, EventLoop.quit)
        EventLoop.exec()

    def flush(self):
        '''
        Function to override the default flush functions of sys.stdout & sys.stderr
        '''
        pass


# Monitor the cpu&gpu's usage
class MonitorUsage(QThread):
    '''
    Get the usage of CPU and NVIDIA GPU
    '''
    usageInfo = Signal(str, str)

    def __init__(self):
        super().__init__()

        try:
            pynvml.nvmlInit()
            self.isNVIDIAGPU = True
        except:
            self.isNVIDIAGPU = False

    def run(self):
        while self.isNVIDIAGPU:
            Usage_CPU_Percent = psutil.cpu_percent(interval = 1.)
            Usage_CPU = f"{Usage_CPU_Percent}%"

            #Usage_RAM_Percent = psutil.virtual_memory().percent
            #Usage_RAM = f"{Usage_RAM_Percent}%"

            Usage_GPU_Percent = 0
            for Index in range(pynvml.nvmlDeviceGetCount()):
                Usage_GPU_Percent_Single = pynvml.nvmlDeviceGetUtilizationRates(pynvml.nvmlDeviceGetHandleByIndex(Index)).gpu
                Usage_GPU_Percent += Usage_GPU_Percent_Single #Usage_GPU_Percent = Usage_GPU_Percent_Single if Usage_GPU_Percent < Usage_GPU_Percent_Single else Usage_GPU_Percent
            Usage_GPU = f"{Usage_GPU_Percent / pynvml.nvmlDeviceGetCount()}%" #Usage_GPU = f"{Usage_GPU_Percent}%"

            self.usageInfo.emit(Usage_CPU, Usage_GPU)

            self.msleep(1000)


# Monitor the file's content
class MonitorFile(QThread):
    '''
    Get the content of file and send to the UI
    '''
    Signal_fileContent = Signal(str)

    def __init__(self, filePath):
        super().__init__()

        self.filePath = filePath

    def run(self):
        self.fileContent_Prev = str()

        while Path(self.filePath).exists():
            with open(self.filePath, mode = 'r', encoding = 'utf-8', errors = 'replace') as file:
                fileContent = file.read()

            if fileContent == self.fileContent_Prev:
                self.msleep(100)
            else:
                self.Signal_fileContent.emit(fileContent)
                self.fileContent_Prev = fileContent

        else:
            print("file %s not found, creating new one..." % self.filePath)

            os.makedirs(Path(self.filePath).parent.__str__(), exist_ok = True) if Path(self.filePath).parent.exists() == False else None
            with open(self.filePath, mode = 'w') as file:
                pass


# Monitor the log file's content
class MonitorLogFile(QThread):
    '''
    Get the content of log file and send to the UI (Recommanded)
    '''
    consoleInfo = Signal(str)

    def __init__(self, logPath):
        super().__init__()

        self.logPath = logPath

        if Path(self.logPath).exists():
            self.clear()
        else:
            os.makedirs(Path(self.logPath).parent.__str__(), exist_ok = True) if Path(self.logPath).parent.exists() == False else None
            with open(self.logPath, 'w') as log:
                pass

    def run(self):
        self.logContent_Prev = str()
        while True:
            with open(self.logPath, 'r', encoding = 'utf-8', errors = 'replace') as log:
                LogContent = log.read()

            if LogContent == self.logContent_Prev:
                self.msleep(100)
            else:
                self.consoleInfo.emit(LogContent)
                self.logContent_Prev = LogContent

    def clear(self):
        with open(self.logPath, 'r+') as log:
            log.seek(0)
            log.truncate()

##############################################################################################################################