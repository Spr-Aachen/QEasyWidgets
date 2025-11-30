import os
import sys
import psutil
import pynvml
from pathlib import Path
from PySide6.QtCore import QThread, QMutex, Signal, Slot, QTimer, QEventLoop

##############################################################################################################################

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


class MonitorFile(QThread):
    '''
    Get the content of file and send to the UI
    '''
    contentChanged = Signal(str)

    def __init__(self, filePath, mode: str = 'append'):
        super().__init__()

        self.filePath = filePath
        self.mode = mode

        if Path(self.filePath).exists():
            self.clear()
        else:
            os.makedirs(Path(self.filePath).parent.__str__(), exist_ok = True) if Path(self.filePath).parent.exists() == False else None
            with open(self.filePath, 'w') as fileContent:
                pass

        self.pos: int = 0
        self.content_prev: str = ''

    def _resetPos(self):
        self.pos = 0

    def _resetContentPrev(self):
        self.content_prev = ''

    def run(self):
        while True:
            if self.mode == 'append':
                if Path(self.filePath).stat().st_size < self.pos:
                    self._resetPos()
                with open(self.filePath, 'rb') as fileContent:
                    fileContent.seek(self.pos)
                    chunk = fileContent.read()
                if chunk:
                    text = chunk.decode(encoding = 'utf-8', errors = 'replace')
                    self.contentChanged.emit(text)
                    self.pos += len(chunk)
                else:
                    self.msleep(100)
            if self.mode == 'all':
                with open(self.filePath, 'rb') as fileContent:
                    chunk = fileContent.read()
                if chunk:
                    content = chunk.decode(encoding = 'utf-8', errors = 'replace')
                    if content == self.content_prev:
                        self.msleep(100)
                    else:
                        self.contentChanged.emit(content)
                        self.content_prev = content

    def clear(self):
        with open(self.filePath, 'r+') as fileContent:
            fileContent.seek(0)
            fileContent.truncate()
        self._resetPos()
        self._resetContentPrev()

##############################################################################################################################