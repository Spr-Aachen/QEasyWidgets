from enum import Enum
from pathlib import Path
from typing import Union, Optional
from PySide6.QtCore import QFile
from PySide6.QtWidgets import QApplication, QWidget

from .Signals import ComponentsSignals
from .Theme import EasyTheme

##############################################################################################################################

RegistratedWidgets = {}


class StyleSheetBase(Enum):
    '''
    '''
    Label = 'Label'
    Button = 'Button'
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
    Menu = 'Menu'

    def Registrate(self, widget, value):
        RegistratedWidgets[widget] = value

    def Deregistrate(self, widget):
        RegistratedWidgets.pop(widget)

    def Apply(self, widget: QWidget, theme: Optional[str] = None, registrate: bool = True):
        EasyTheme.Update(theme) if theme is not None else None

        Prefix = 'QSS'
        FilePath = f'QSS/{EasyTheme.THEME}/{self.value}.qss'
        File = QFile(Path(f':/{Prefix}').joinpath(FilePath))
        File.open(QFile.ReadOnly | QFile.Text)
        QSS = str(File.readAll(), encoding = 'utf-8')
        File.close()

        widget.setStyleSheet(QSS)

        self.Registrate(widget, self.value) if registrate else None


def Function_UpdateStyleSheet(
    theme: Optional[str] = None
):
    '''
    '''
    for Widget, value in list(RegistratedWidgets.items()):
        for Value in StyleSheetBase:
            if Value.value != value:
                continue
            try:
                Value.Apply(Widget, theme)
            except RuntimeError:
                Value.Deregistrate(Widget)
            finally:
                continue


ComponentsSignals.Signal_SetTheme.connect(Function_UpdateStyleSheet)

##############################################################################################################################