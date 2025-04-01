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

    error = Signal(Exception)
    result = Signal(object)


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

        try:
            result = self.fn(*self.args, **self.kwargs)
        except Exception as e:
            self.signals.error.emit(e)
        else:
            self.signals.result.emit(result)

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
        executeClassName, executeMethodName = getNamesFromMethod(executeMethod)
        if executeClassName is not None:
            try:
                self.executeClassInstance = getClassFromMethod(executeMethod)()
                self.executeClassInstanceMethod = getattr(self.executeClassInstance, executeMethodName)
            except:
                self.executeClassInstanceMethod = executeMethod
        else:
            self.executeClassInstanceMethod = executeMethod

        if terminateMethod is not None:
            terminateClassName, terminateMethodName = getNamesFromMethod(terminateMethod)
            if terminateClassName is not None:
                try:
                    self.terminateClassInstance = self.executeClassInstance if terminateClassName == executeClassName else getClassFromMethod(terminateMethod)()
                    self.terminateClassInstanceMethod = getattr(self.terminateClassInstance, terminateMethodName)
                except:
                    self.terminateClassInstanceMethod = terminateMethod
            else:
                self.terminateClassInstanceMethod = terminateMethod
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