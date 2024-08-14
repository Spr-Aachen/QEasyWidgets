from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *
from PySide6.QtMultimedia import QMediaPlayer, QAudioOutput

from ..Common.Icon import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Button import ButtonBase

##############################################################################################################################

class MediaPlayerBase(QWidget):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.StackedWidget = QStackedWidget()
        self.StackedWidget.setMaximumSize(36, 36)
        self.StackedWidget.setContentsMargins(0, 0, 0, 0)
        self.PlayButton = ButtonBase()
        self.PlayButton.setIcon(IconBase.Play)
        self.PauseButton = ButtonBase()
        self.PauseButton.setIcon(IconBase.Pause)
        self.PauseButton.clicked.connect(lambda: self.StackedWidget.setCurrentWidget(self.PlayButton))
        self.PlayButton.clicked.connect(lambda: self.StackedWidget.setCurrentWidget(self.PauseButton))
        self.StackedWidget.addWidget(self.PlayButton)
        self.StackedWidget.addWidget(self.PauseButton)
        self.StackedWidget.setCurrentWidget(self.PlayButton)

        self.Slider = QSlider()
        self.Slider.setOrientation(Qt.Horizontal)

        HBoxLayout = QHBoxLayout(self)
        HBoxLayout.setSpacing(12)
        HBoxLayout.setContentsMargins(21, 12, 21, 12)
        HBoxLayout.addWidget(self.StackedWidget, stretch = 1)
        HBoxLayout.addWidget(self.Slider, stretch = 5)

        AudioOutput = QAudioOutput(self)
        self.MediaPlayer = QMediaPlayer()
        self.MediaPlayer.setAudioOutput(AudioOutput)
        #self.MediaPlayer.mediaStatusChanged.connect(lambda Status: self.MediaPlayer.stop() if Status == QMediaPlayer.EndOfMedia else None)

        StyleSheetBase.Player.Apply(self)

    def SetMediaPlayer(self, MediaPath: str):
        self.MediaPlayer.setSource(QUrl.fromLocalFile(MediaPath))

        self.PlayButton.clicked.connect(self.MediaPlayer.play)
        self.PauseButton.clicked.connect(self.MediaPlayer.pause)
        self.MediaPlayer.mediaStatusChanged.connect(lambda status: self.StackedWidget.setCurrentWidget(self.PlayButton) if status == QMediaPlayer.EndOfMedia else None)

        self.Slider.setRange(0, 100)
        self.Slider.sliderMoved.connect(lambda: self.MediaPlayer.setPosition(int(self.Slider.value() / 100 * self.MediaPlayer.duration())))
        self.MediaPlayer.positionChanged.connect(lambda Position: self.Slider.setValue(int(Position / self.MediaPlayer.duration() * 100)))

    def ReleaseMediaPlayer(self):
        self.MediaPlayer.stop()
        self.MediaPlayer.setSource('')
        #self.MediaPlayer.deleteLater()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Player.Deregistrate(self)

##############################################################################################################################