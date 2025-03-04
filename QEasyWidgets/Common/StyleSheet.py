from enum import Enum
from pathlib import Path
from typing import Union, Optional
from PySide6.QtCore import QFile
from PySide6.QtWidgets import QApplication, QWidget

from .Signals import ComponentsSignals
from .Theme import EasyTheme, currentTheme

##############################################################################################################################

registratedWidgets = {}


class StyleSheetBase(Enum):
    '''
    '''
    Label = 'Label'
    Button = 'Button'
    CheckBox = 'CheckBox'
    ScrollArea = 'ScrollArea'
    Tree = 'Tree'
    List = 'List'
    ToolBox = 'ToolBox'
    GroupBox = 'GroupBox'
    Slider = 'Slider'
    SpinBox = 'SpinBox'
    ComboBox = 'ComboBox'
    Edit = 'Edit'
    Browser = 'Browser'
    ProgressBar = 'ProgressBar'
    Player = 'Player'
    Tab = 'Tab'
    Table = 'Table'
    ChatWidget = 'ChatWidget'
    Bar = 'Bar'
    DockWidget = 'DockWidget'
    Menu = 'Menu'

    def registrate(self, widget, value):
        registratedWidgets[widget] = value

    def deregistrate(self, widget):
        registratedWidgets.pop(widget)

    def apply(self, widget: QWidget, theme: Optional[str] = None, registrate: bool = True):
        EasyTheme.update(theme) if theme is not None else None

        Prefix = 'QSS'
        FilePath = f'QSS/{currentTheme()}/{self.value}.qss'
        File = QFile(Path(f':/{Prefix}').joinpath(FilePath))
        File.open(QFile.ReadOnly | QFile.Text)
        QSS = str(File.readAll(), encoding = 'utf-8')
        File.close()

        widget.setStyleSheet(QSS)

        self.registrate(widget, self.value) if registrate else None


def Function_UpdateStyleSheet(
    theme: Optional[str] = None
):
    '''
    '''
    for widget, value in list(registratedWidgets.items()):
        for Value in StyleSheetBase:
            if Value.value != value:
                continue
            try:
                Value.apply(widget, theme)
            except RuntimeError:
                Value.deregistrate(widget)
            finally:
                #QApplication.instance().processEvents()
                continue


ComponentsSignals.Signal_SetTheme.connect(Function_UpdateStyleSheet)

##############################################################################################################################