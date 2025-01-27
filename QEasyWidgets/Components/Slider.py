from typing import Optional, overload
from PyEasyUtils import singledispatchmethod, getDecimalPlaces
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class SliderBase(QSlider):
    """
    Base class for slider components
    """
    _times = {
        "minimum": 1,
        "maximum": 1,
        "singleStep": 1,
        "value": 1
    }

    valueChanged = Signal(float)

    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        StyleSheetBase.Slider.apply(self)

    @__init__.register
    def _(self, arg__1: Qt.Orientation, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setOrientation(arg__1)

    def _updateTimes(self, key, value):
        Time = 10 ** getDecimalPlaces(value)
        for time in self._times.values():
            if time > Time:
                Time = time
        self._times[key] = Time
        return Time

    def _updateValues(self, Time, min = None, max = None, singleStep = None, value = None):
        min = ((super().minimum() / self._times["minimum"]) if min is None else min) * Time
        self._times["minimum"] = Time
        max = ((super().maximum() / self._times["maximum"]) if max is None else max) * Time
        self._times["maximum"] = Time
        super().setRange(min, max)
        singleStep = ((super().singleStep() / self._times["singleStep"]) if singleStep is None else singleStep) * Time
        self._times["singleStep"] = Time
        super().setSingleStep(singleStep)
        value = ((super().value() / self._times["value"]) if value is None else value) * Time
        self._times["value"] = Time
        super().setValue(value)

    def setMinimum(self, arg__1):
        Time = self._updateTimes("minimum", arg__1)
        self._updateValues(Time, min = arg__1)

    def minimum(self):
        return super().minimum() / self._times["minimum"]

    def setMaximum(self, arg__1):
        Time = self._updateTimes("maximum", arg__1)
        self._updateValues(Time, max = arg__1)

    def maximum(self):
        return super().maximum() / self._times["maximum"]

    def setRange(self, min, max):
        self.setMinimum(min)
        self.setMaximum(max)

    def range(self):
        return (self.minimum(), self.maximum())

    def setSingleStep(self, arg__1):
        Time = self._updateTimes("singleStep", arg__1)
        self._updateValues(Time, singleStep = arg__1)

    def singleStep(self):
        return super().singleStep() / self._times["singleStep"]

    def setValue(self, arg__1):
        Time = self._updateTimes("value", arg__1)
        self._updateValues(Time, value = arg__1)

    def value(self):
        return super().value() / self._times["value"]

    def sliderChange(self, change: QAbstractSlider.SliderChange) -> None:
        if change == QAbstractSlider.SliderChange.SliderValueChange:
            self.valueChanged.emit(self.value())
        super().sliderChange(change)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Slider.deregistrate(self)

##############################################################################################################################