import os
from pathlib import Path
from typing import Union, Optional, Sequence
from PyEasyUtils import toIterable, normPath, runCMD
from PySide6.QtCore import Qt, QSettings, QPoint, QRect, QSize, QPropertyAnimation, QParallelAnimationGroup, QEasingCurve, QUrl
from PySide6.QtGui import QColor, QRgba64, QFont, QScreen, QDesktopServices, QAction, QCursor
from PySide6.QtWidgets import *

##############################################################################################################################

def findChild(
    parent: QWidget,
    childType: object
):
    """
    Function to find child widget
    """
    parentWidget_Children = parent.children()

    for parentWidget_Child in parentWidget_Children:
        if isinstance(parentWidget_Child, childType):
            return parentWidget_Child


def findParent(
    child: QWidget,
    parentType: object
):
    """
    Function to find parent widget
    """
    childWidget_Parent = child.parent()

    while not isinstance(childWidget_Parent, parentType):
        try:
            childWidget_Parent = childWidget_Parent.parent()
        except:
            childWidget_Parent = None
            break

    return childWidget_Parent

##############################################################################################################################

def getWidth(widget: QWidget):
    return widget.geometry().width() if widget.size() == QSize(100, 30) else widget.width()


def getHeight(widget: QWidget):
    return widget.geometry().height() if widget.size() == QSize(100, 30) else widget.height()

##############################################################################################################################

def removeSubWidgets(
    widget: QWidget
):
    if isinstance(widget, QStackedWidget):
        while widget.count():
            widget.removeWidget(widget.widget(0))


def setRetainSizeWhenHidden(
    widget: QWidget,
    retainSize: bool = True
):
    sizePolicy = widget.sizePolicy()
    sizePolicy.setRetainSizeWhenHidden(retainSize)
    widget.setSizePolicy(sizePolicy)


def setDropShadowEffect(
    widget: QWidget,
    blurRadius: float = 3.,
    color: Union[QColor, QRgba64] = Qt.gray,
    offset_dx: float = 0.,
    offset_dy: float = 0.,
):
    dropShadowEffect = QGraphicsDropShadowEffect()
    dropShadowEffect.setOffset(0, 0)
    dropShadowEffect.setBlurRadius(blurRadius)
    dropShadowEffect.setColor(color)
    dropShadowEffect.setOffset(offset_dx, offset_dy)
    widget.setGraphicsEffect(dropShadowEffect)
    return dropShadowEffect


def setOpacityEffect(
    widget: QWidget,
    parent: Optional[QWidget] = None,
    duration: int = 123,
):
    opacityAnim = QPropertyAnimation(widget, b'windowOpacity', parent)
    opacityAnim.setDuration(duration)
    return opacityAnim

##############################################################################################################################

def setAnimation(
    animation: QPropertyAnimation,
    startValue,
    endValue,
    duration: int
):
    animation.setStartValue(startValue)
    animation.setEndValue(endValue)
    animation.setDuration(duration)
    animation.setEasingCurve(QEasingCurve.InOutQuart)
    return animation


def setWidgetPosAnimation(
    widget: QWidget,
    duration: int = 99
):
    OriginalGeometry = widget.geometry()
    AlteredGeometry = QRect(OriginalGeometry.left(), OriginalGeometry.top() + OriginalGeometry.height() / duration, OriginalGeometry.width(), OriginalGeometry.height())

    WidgetAnimation = QPropertyAnimation(widget, b"geometry", widget)

    return setAnimation(WidgetAnimation, OriginalGeometry, AlteredGeometry, duration)


def setWidgetSizeAnimation(
    frame: QWidget,
    targetWidth: Optional[int] = None,
    targetHeight: Optional[int] = None,
    duration: int = 210,
):
    """
    Function to animate widget size
    """
    currentWidth, currentHeight = getWidth(frame), getHeight(frame)

    animationGroup = QParallelAnimationGroup(frame)

    if frame.property("currentWidth") and frame.property("currentHeight"):
        frameAnimationWidth = QPropertyAnimation(frame, b"currentWidth", frame)
        frameAnimationHeight = QPropertyAnimation(frame, b"currentHeight", frame)
        animationGroup.addAnimation(setAnimation(frameAnimationWidth, currentWidth, targetWidth, duration)) if targetWidth is not None else None
        animationGroup.addAnimation(setAnimation(frameAnimationHeight, currentHeight, targetHeight, duration)) if targetHeight is not None else None
    else:
        frameAnimationMinWidth = QPropertyAnimation(frame, b"minimumWidth", frame)
        frameAnimationMaxWidth = QPropertyAnimation(frame, b"maximumWidth", frame)
        frameAnimationMinHeight = QPropertyAnimation(frame, b"minimumHeight", frame)
        frameAnimationMaxHeight = QPropertyAnimation(frame, b"maximumHeight", frame)
        animationGroup.addAnimation(setAnimation(frameAnimationMinWidth, currentWidth, targetWidth, duration)) if targetWidth is not None else None
        animationGroup.addAnimation(setAnimation(frameAnimationMaxWidth, currentWidth, targetWidth, duration)) if targetWidth is not None else None
        animationGroup.addAnimation(setAnimation(frameAnimationMinHeight, currentHeight, targetHeight, duration)) if targetHeight is not None else None
        animationGroup.addAnimation(setAnimation(frameAnimationMaxHeight, currentHeight, targetHeight, duration)) if targetHeight is not None else None

    return animationGroup


def setWidgetOpacityAnimation(
    widget: QWidget,
    originalOpacity: float,
    targetOpacity: float,
    duration: int = 99
):
    OpacityEffect = QGraphicsOpacityEffect()
    widget.setGraphicsEffect(OpacityEffect)

    originalOpacity = originalOpacity
    AlteredOpacity = targetOpacity

    WidgetAnimation = QPropertyAnimation(OpacityEffect, b"opacity", widget)

    return setAnimation(WidgetAnimation, originalOpacity, AlteredOpacity, duration)

##############################################################################################################################

def setFont(
    widget: QWidget,
    fontSize:int = 12,
    weight: QFont.Weight = QFont.Normal,
    families: Sequence[str] = ['Microsoft YaHei']
):
    """
    Set the font of widget
    """
    font = QFont()
    font.setFamilies(families)
    font.setPixelSize(fontSize)
    font.setWeight(weight)
    widget.setFont(font)


def setText(
    widget: QWidget,
    text: str,
    setHtml: bool = True,
    setPlaceholderText: bool = False,
    placeholderText: Optional[str] = None
):
    if hasattr(widget, 'setText'):
        widget.setText(text)
    if hasattr(widget, 'setPlainText'):
        widget.setPlainText(text)
    if hasattr(widget, 'setHtml') and setHtml:
        widget.setHtml(text)
    if hasattr(widget, 'setPlaceholderText') and setPlaceholderText:
        widget.setPlaceholderText(str(placeholderText) if text.strip() in ('', str(None)) else text)


def getText(
    widget: QWidget,
    getHtml: bool = False,
    getPlaceholderText: bool = False
):
    if hasattr(widget, 'text'):
        text = widget.text()
    if hasattr(widget, 'toPlainText'):
        text = widget.toPlainText()
    if hasattr(widget, 'toHtml') and getHtml:
        text = widget.toHtml()
    if hasattr(widget, 'placeholderText') and getPlaceholderText:
        text = widget.placeholderText() if text.strip() in ('', str(None)) else text
    return text

##############################################################################################################################

class FileDialogMode: 
    SelectFolder = 0
    SelectFile = 1
    SaveFile = 2


def getFileDialog(
    mode: FileDialogMode,
    fileType: Optional[str] = None,
    directory: Optional[str] = None
):
    displayText = ""
    os.makedirs(directory, exist_ok = True) if directory is not None and Path(directory).exists() == False else None
    if mode == FileDialogMode.SelectFolder:
        displayText = QFileDialog.getExistingDirectory(
            caption = "选择文件夹 | SelectFolder",
            dir = directory if directory is not None else os.getcwd()
        )
    if mode == FileDialogMode.SelectFile:
        displayText, _ = QFileDialog.getOpenFileName(
            caption = "选择文件 | SelectFile",
            dir = directory if directory is not None else os.getcwd(),
            filter = fileType if fileType is not None else '任意类型 (*.*)'
        )
    if mode == FileDialogMode.SaveFile:
        displayText, _ = QFileDialog.getSaveFileName(
            caption = "保存文件 | SaveFile",
            dir = directory if directory is not None else os.getcwd(),
            filter = fileType if fileType is not None else '任意类型 (*.*)'
        )
    return displayText# if displayText != '' else None

##############################################################################################################################

def setContextMenu(parent: QWidget, contextMenu: QMenu, actions: dict):
    for actionName, events in actions.items():
        action = QAction(actionName, parent)
        for event in toIterable(events):
            action.triggered.connect(event)
        contextMenu.addAction(action)
    return contextMenu


def showContextMenu(parent: QWidget, contextMenu: QMenu, actions: dict, position: QPoint):
    contextMenu = setContextMenu(parent, contextMenu, actions)
    contextMenu.exec(position)

##############################################################################################################################

def openURL(
    url: Union[str, list],
    createIfNotExist: bool = False
):
    """
    Function to open web/local url
    """
    def OpenURL(url):
        QURL = QUrl().fromLocalFile(normPath(url))
        if QURL.isValid():
            os.makedirs(normPath(url), exist_ok = True) if createIfNotExist else None
            isSucceeded = QDesktopServices.openUrl(QURL)
            runCMD([f'start {url}']) if not isSucceeded else None
        else:
            print(f"Invalid url: {url} !")

    if isinstance(url, str):
        OpenURL(url)
    else:
        URLList = toIterable(url)
        for Index, url in enumerate(URLList):
            #url = Function_ParamsChecker(URLList)[Index] if isinstance(url, QObject) else url
            OpenURL(url)

##############################################################################################################################

def saveLayout(widget: QWidget, settings: QSettings):
    settings.setValue("layout/geometry", widget.saveGeometry())
    if isinstance(widget, QMainWindow):
        settings.setValue("layout/state", widget.saveState())
    settings.sync()


def resetLayout(widget: QWidget, settings: QSettings):
    widget.restoreGeometry(settings.value("layout/geometry"))
    if isinstance(widget, QMainWindow):
        widget.restoreState(settings.value("layout/state"))
        for dockWidget in widget.findChildren(QDockWidget):
            widget.restoreDockWidget(dockWidget)

##############################################################################################################################

def getCurrentScreen():
    """
    Get current screen
    """
    cursorPos = QCursor.pos()
    for screen in QApplication.screens():
        if screen.geometry().contains(cursorPos):
            return screen


def getScreenGeometry(
    screen: Optional[QScreen] = None,
    getAvaliableGeometry: bool = True
):
    """
    Get screen geometry
    """
    if screen is None:
        screen = getCurrentScreen() or QApplication.primaryScreen()
    return screen.availableGeometry() if getAvaliableGeometry else screen.geometry()

##############################################################################################################################