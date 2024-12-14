from enum import Enum
from pathlib import Path
from typing import Union, Optional
from PySide6.QtCore import Qt, QFile, QRect, QRectF, QSize
from PySide6.QtGui import QIcon, QIconEngine, QPainter, QPixmap, QImage
from PySide6.QtSvg import QSvgRenderer
from PySide6.QtXml import QDomDocument

from .Theme import EasyTheme

##############################################################################################################################

class IconEngine(QIconEngine):
    '''
    '''
    def __init__(self):
        super().__init__()

        self.IsIconSVG = False

    def loadSVG(self, SVGString: str):
        self.IsIconSVG = True
        self.Icon = SVGString.encode(errors = 'replace')

    def paint(self, painter: QPainter, rect: QRect, mode: QIcon.Mode, state: QIcon.State) -> None:
        if self.IsIconSVG:
            renderer = QSvgRenderer(self.Icon)
            renderer.render(painter, QRectF(rect))
        else:
            super().paint(painter, rect, mode, state)

    def pixmap(self, size: QSize, mode: QIcon.Mode, state: QIcon.State) -> QPixmap:
        image = QImage(size, QImage.Format_ARGB32)
        image.fill(Qt.transparent)
        pixmap = QPixmap.fromImage(image, Qt.NoFormatConversion)

        painter = QPainter(pixmap)
        rect = QRect(0, 0, size.width(), size.height())
        self.paint(painter, rect, mode, state)

        return pixmap


class IconBase(Enum):
    '''
    '''
    Chevron_Left = 'Chevron-Left'
    CompactChevron_Left = 'CompactChevron-Left'
    Chevron_Right = 'Chevron-Right'
    CompactChevron_Right = 'CompactChevron-Right'
    Chevron_Top = 'Chevron-Top'
    Chevron_Down = 'Chevron-Down'
    Ellipsis = 'Ellipsis'
    OpenedFolder = 'OpenedFolder'
    Clipboard = 'Clipboard'
    Download = 'Download'
    Play = 'Play'
    Pause = 'Pause'
    Dash = 'Dash'
    Window_FullScreen = 'Window-FullScreen'
    Window_Stack = 'Window-Stack'
    FullScreen = 'FullScreen'
    FullScreen_Exit = 'FullScreen-Exit'
    X = 'X'

    def paint(self, painter: QPainter, rect: Union[QRect, QRectF], theme: Optional[str] = None):
        Prefix = 'Icons'
        IconPath = f'Icons/{theme if theme is not None else EasyTheme.THEME}/{self.value}.svg'
        IconPath = Path(f':/{Prefix}').joinpath(IconPath).as_posix()
        Renderer = QSvgRenderer(IconPath)
        Renderer.render(painter, QRectF(rect))

    def create(self, theme: Optional[str] = None) -> QIcon:
        Prefix = 'Icons'
        IconPath = f'Icons/{theme if theme is not None else EasyTheme.THEME}/{self.value}.svg'
        File = QFile(Path(f':/{Prefix}').joinpath(IconPath))
        File.open(QFile.ReadOnly)
        DomDocument = QDomDocument()
        DomDocument.setContent(File.readAll())
        File.close()

        Engine = IconEngine()
        Engine.loadSVG(DomDocument.toString())
        Icon = QIcon(Engine)

        return Icon


def Function_DrawIcon(
    icon: Union[str, QIcon],
    painter: QPainter,
    rect: Union[QRect, QRectF]
):
    '''
    Draw icon
    '''
    if isinstance(icon, IconBase):
        icon.paint(painter, rect, EasyTheme.THEME)
    else:
        icon = QIcon(icon)
        icon.paint(painter, QRectF(rect).toRect(), Qt.AlignCenter, state = QIcon.Off)


def Function_ToQIcon(
    icon: Union[str, QIcon, IconBase]
):
    if isinstance(icon, IconBase):
        return icon.create()
    else:
        return QIcon(icon)

##############################################################################################################################