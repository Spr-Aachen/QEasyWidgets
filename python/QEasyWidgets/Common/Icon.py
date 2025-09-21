from enum import Enum
from pathlib import Path
from typing import Union, Optional
from PySide6.QtCore import Qt, QFile, QRect, QRectF, QSize
from PySide6.QtGui import QIcon, QIconEngine, QPainter, QPixmap, QImage
from PySide6.QtSvg import QSvgRenderer
from PySide6.QtXml import QDomDocument

from .Theme import Theme, currentTheme

##############################################################################################################################

class IconEngine(QIconEngine):
    '''
    '''
    def __init__(self):
        super().__init__()

        self.isIconSVG = False

    def loadSVG(self, SVGString: str):
        self.isIconSVG = True
        self.icon = SVGString.encode(errors = 'replace')

    def paint(self, painter: QPainter, rect: QRect, mode: QIcon.Mode, state: QIcon.State) -> None:
        if self.isIconSVG:
            renderer = QSvgRenderer(self.icon)
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
    Dash = 'Dash'
    FullScreen = 'FullScreen'
    FullScreen_Exit = 'FullScreen-Exit'
    X = 'X'
    Arrow_Clockwise = 'Arrow-Clockwise'
    Arrow_Repeat = 'Arrow-Repeat'
    Chevron_Left = 'Chevron-Left'
    CompactChevron_Left = 'CompactChevron-Left'
    Chevron_Right = 'Chevron-Right'
    CompactChevron_Right = 'CompactChevron-Right'
    Chevron_Up = 'Chevron-Up'
    Chevron_Down = 'Chevron-Down'
    Ellipsis = 'Ellipsis'
    OpenedFolder = 'OpenedFolder'
    Clipboard = 'Clipboard'
    Download = 'Download'
    Send = 'Send'
    Play = 'Play'
    Pause = 'Pause'
    Stop = 'Stop'

    def paint(self, painter: QPainter, rect: Union[QRect, QRectF], theme: Optional[Theme] = None):
        prefix = 'Icons'
        iconPath = f'icons/{theme if theme is not None else currentTheme()}/{self.value}.svg'
        iconPath = Path(f':/{prefix}').joinpath(iconPath).as_posix()
        renderer = QSvgRenderer(iconPath)
        renderer.render(painter, QRectF(rect))

    def create(self, theme: Optional[Theme] = None) -> QIcon:
        prefix = 'Icons'
        iconPath = f'icons/{theme if theme is not None else currentTheme()}/{self.value}.svg'
        file = QFile(Path(f':/{prefix}').joinpath(iconPath))
        file.open(QFile.ReadOnly)
        domDocument = QDomDocument()
        domDocument.setContent(file.readAll())
        file.close()

        engine = IconEngine()
        engine.loadSVG(domDocument.toString())
        icon = QIcon(engine)

        return icon


def Function_DrawIcon(
    icon: Union[str, QIcon],
    painter: QPainter,
    rect: Union[QRect, QRectF]
):
    '''
    Draw icon
    '''
    if isinstance(icon, IconBase):
        icon.paint(painter, rect, currentTheme())
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