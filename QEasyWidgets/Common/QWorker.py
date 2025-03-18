from typing import Union, Optional, overload
from PyEasyUtils import singledispatchmethod, getNamesFromMethod, getClassFromMethod
from PySide6.QtCore import QObject, Signal, QThreadPool, QRunnable
from PySide6.QtWidgets import *

##############################################################################################################################

class WorkerSignals(QObject):
    """
    """
    started = Signal()
    finished = Signal()

    errChk = Signal(str)


class Worker(QRunnable):
    """
    """
    @singledispatchmethod
    def __init__(self):
        super().__init__()

        #self.setAutoDelete(True)

        self.signals = WorkerSignals()

    # @__init__.register
    # def _(self, fn, *args, **kwargs):
    #     self.__init__()
    #     self.setTask(fn, *args, **kwargs)

    def setTask(self, fn, *args, **kwargs):
        self.fn = fn
        self.args = args
        self.kwargs = kwargs

    def run(self):
        self.signals.started.emit()

        result = self.fn(*self.args, **self.kwargs)
        self.signals.errChk.emit(str(result))

        self.signals.finished.emit()

##############################################################################################################################

class WorkerManager:
    """
    """
    def __init__(self,
        executeMethod: object = ...,
        terminateMethod: Optional[object] = None,
        threadPool: Optional[QThreadPool] = None
    ):
        self.executeQualName, self.executeMethodName = getNamesFromMethod(executeMethod)
        executeClassInstance = getClassFromMethod(executeMethod)()
        self.executeClassInstanceMethod = getattr(executeClassInstance, self.executeMethodName)

        if terminateMethod is not None:
            terminateQualName, terminateMethodName = getNamesFromMethod(terminateMethod)
            terminateClassInstance = executeClassInstance if terminateQualName == self.executeQualName else getClassFromMethod(terminateMethod)()
            self.terminateClassInstanceMethod = getattr(terminateClassInstance, terminateMethodName)
        else:
            self.terminateClassInstanceMethod = None

        self.threadPool = threadPool or QThreadPool()

        self.worker = Worker()

    def execute(self, *executeParams):
        self.worker.setTask(self.executeClassInstanceMethod, *executeParams)
        self.threadPool.start(self.worker)

    def terminate(self):
        self.terminateClassInstanceMethod() if self.terminateClassInstanceMethod is not None else None

##############################################################################################################################