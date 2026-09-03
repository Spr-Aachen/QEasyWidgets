from enum import Enum
from pathlib import Path
from typing import Union, Optional
from weakref import WeakKeyDictionary
from PySide6.QtCore import QFile
from PySide6.QtWidgets import QApplication, QWidget

from .Signals import componentsSignals
from .Theme import EasyTheme, currentTheme

##############################################################################################################################

registratedWidgets = WeakKeyDictionary()


class StyleSheetBase(Enum):
    '''
    '''
    Label = 'Label'
    ToolTip = 'ToolTip'
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
        if widget in registratedWidgets:
            del registratedWidgets[widget]

    def apply(self, widget: QWidget, theme: Optional[str] = None, registrate: bool = True):
        EasyTheme.update(theme) if theme is not None else None

        prefix = 'QSS'
        # Ensure currentTheme() returns a string value
        current_theme = currentTheme()
        if hasattr(current_theme, 'value'):
            current_theme = current_theme.value
        filePath = f'qss/{current_theme}/{self.value}.qss'
        file = QFile(Path(f':/{prefix}').joinpath(filePath))
        file.open(QFile.ReadOnly | QFile.Text)
        QSS = str(file.readAll(), encoding = 'utf-8')
        file.close()

        widget.setStyleSheet(QSS)

        self.registrate(widget, self.value) if registrate else None


def Function_UpdateStyleSheet(
    theme: Optional[str] = None
):
    '''
    '''
    # Use list() to avoid runtime error if dictionary changes during iteration
    for widget, value in list(registratedWidgets.items()):
        for Value in StyleSheetBase:
            if Value.value != value:
                continue
            try:
                # Check if widget is still valid (C++ object might be deleted)
                if hasattr(widget, 'isVisible'): 
                    Value.apply(widget, theme, registrate = False)
            except RuntimeError:
                # Widget C++ object deleted
                pass
            except Exception:
                pass


componentsSignals.setTheme.connect(Function_UpdateStyleSheet)

##############################################################################################################################