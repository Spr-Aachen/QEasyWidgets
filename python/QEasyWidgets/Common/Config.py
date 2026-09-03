from enum import Enum
import json
import darkdetect
from pathlib import Path
from PySide6.QtCore import QObject, Signal
from PySide6.QtGui import QColor

##############################################################################################################################

class Direction(Enum):
    Up = 1
    Down = 2


class Position(Enum):
    Top = 1
    Bottom = 2
    Left = 3
    Right = 4
    TopLeft = 5
    TopRight = 6
    BottomLeft = 7
    BottomRight = 8


class Status(Enum):
    Loading = 1


class ChatRole(Enum):
    User = 1
    Contact = 2


class Theme(Enum):
    Light = 'light'
    Dark = 'dark'
    Auto = 'auto'

##############################################################################################################################

class ConfigItem(QObject):
    """
    Configuration item
    """
    valueChanged = Signal(object)

    def __init__(self, group, name, default):
        super().__init__()
        self.group = group
        self.name = name
        self.value = default
        self.defaultValue = default

    @property
    def key(self):
        return f"{self.group}.{self.name}" if self.name else self.group


class QConfig(QObject):
    """
    Configuration system
    """
    themeChanged = Signal(Theme)
    
    def __init__(self):
        super().__init__()
        self.file = Path("config/config.json")
        self._theme = Theme.Auto
        
        # Define config items
        self.themeMode = ConfigItem("General", "ThemeMode", Theme.Auto)
        
        # Load config
        self.load()

    @property
    def theme(self):
        if self._theme == Theme.Auto:
            return Theme.Dark if darkdetect.isDark() else Theme.Light
        return self._theme

    @theme.setter
    def theme(self, value):
        if self._theme == value:
            return
        self._theme = value
        self.themeMode.value = value
        self.save()
        self.themeChanged.emit(self.theme)

    def load(self):
        try:
            if self.file.exists():
                with open(self.file, encoding='utf-8') as f:
                    data = json.load(f)
                    
                    # Load theme
                    theme_val = data.get("General", {}).get("ThemeMode")
                    if theme_val:
                        self._theme = Theme(theme_val)
                        self.themeMode.value = self._theme
        except Exception as e:
            print(f"Failed to load config: {e}")

    def save(self):
        try:
            self.file.parent.mkdir(parents=True, exist_ok=True)
            data = {
                "General": {
                    "ThemeMode": self._theme.value
                }
            }
            with open(self.file, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=4)
        except Exception as e:
            print(f"Failed to save config: {e}")


qconfig = QConfig()