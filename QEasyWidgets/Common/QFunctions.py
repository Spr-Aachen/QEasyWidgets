import os
from pathlib import Path
from typing import Union, Optional
from PySide6.QtCore import Qt, QPoint, QRect, QSize, QPropertyAnimation, QParallelAnimationGroup, QEasingCurve, QUrl
from PySide6.QtGui import QGuiApplication, QColor, QRgba64, QFont, QDesktopServices, QAction
from PySide6.QtWidgets import *

from .Utils import *
from ..Resources.Sources import *

##############################################################################################################################

def findChildUI(
    parentUI: QWidget,
    childType: object
):
    """
    Function to find child UI
    """
    parentUI_Children = parentUI.children()

    for parentUI_Child in parentUI_Children:
        if isinstance(parentUI_Child, childType):
            return parentUI_Child


def findParentUI(
    childUI: QWidget,
    parentType: object
):
    """
    Function to find parent UI
    """
    childUI_Parent = childUI.parent()

    while not isinstance(childUI_Parent, parentType):
        try:
            childUI_Parent = childUI_Parent.parent()
        except:
            raise Exception(f"{childUI}'s parent UI not found! Please check if the layout is correct.")

    return childUI_Parent

##############################################################################################################################

def setFont(
    widget: QWidget,
    fontSize:int = 12,
    weight = QFont.Normal
):
    """
    Set the font of widget
    """
    Font = QFont()
    Font.setFamilies(['Microsoft YaHei'])
    Font.setPixelSize(fontSize)
    Font.setWeight(weight)
    widget.setFont(Font)

##############################################################################################################################

def setNoContents(
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
    Radius: float = 3.,
    Color: Union[QColor, QRgba64] = Qt.gray
):
    DropShadowEffect = QGraphicsDropShadowEffect()
    DropShadowEffect.setOffset(0, 0)
    DropShadowEffect.setBlurRadius(Radius)
    DropShadowEffect.setColor(Color)
    widget.setGraphicsEffect(DropShadowEffect)

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
    supportSplitter: bool = False
):
    """
    Function to animate widget size
    """
    CurrentWidth = frame.geometry().width() if frame.size() == QSize(100, 30) else frame.width()
    CurrentHeight = frame.geometry().height() if frame.size() == QSize(100, 30) else frame.height()

    FrameAnimationMinWidth = QPropertyAnimation(frame, b"minimumWidth", frame)
    FrameAnimationMaxWidth = QPropertyAnimation(frame, b"maximumWidth", frame)
    FrameAnimationMinHeight = QPropertyAnimation(frame, b"minimumHeight", frame)
    FrameAnimationMaxHeight = QPropertyAnimation(frame, b"maximumHeight", frame)

    AnimationGroup = QParallelAnimationGroup(frame)

    AnimationGroup.addAnimation(setAnimation(FrameAnimationMinWidth, CurrentWidth, targetWidth, duration)) if targetWidth is not None and not supportSplitter else None
    AnimationGroup.addAnimation(setAnimation(FrameAnimationMaxWidth, CurrentWidth, targetWidth, duration)) if targetWidth is not None else None
    AnimationGroup.addAnimation(setAnimation(FrameAnimationMinHeight, CurrentHeight, targetHeight, duration)) if targetHeight is not None and not supportSplitter else None
    AnimationGroup.addAnimation(setAnimation(FrameAnimationMaxHeight, CurrentHeight, targetHeight, duration)) if targetHeight is not None else None

    return AnimationGroup


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

def getFileDialog(
    mode: str,
    fileType: Optional[str] = None,
    directory: Optional[str] = None
):
    os.makedirs(directory, exist_ok = True) if directory is not None and Path(directory).exists() == False else None
    if mode == 'SelectFolder':
        DisplayText = QFileDialog.getExistingDirectory(
            caption = "选择文件夹",
            dir = directory if directory is not None else os.getcwd()
        )
    if mode == 'SelectFile':
        DisplayText, _ = QFileDialog.getOpenFileName(
            caption = "选择文件",
            dir = directory if directory is not None else os.getcwd(),
            filter = fileType if fileType is not None else '任意类型 (*.*)'
        )
    if mode == 'SaveFile':
        DisplayText, _ = QFileDialog.getSaveFileName(
            caption = "保存文件",
            dir = directory if directory is not None else os.getcwd(),
            filter = fileType if fileType is not None else '任意类型 (*.*)'
        )
    return DisplayText

##############################################################################################################################

def showContextMenu(parent: QWidget, contextMenu: QMenu, actions: dict, position: QPoint):
    for actionName, events in actions.items():
        action = QAction(actionName, parent)
        for event in toIterable(events):
            action.triggered.connect(event)
        contextMenu.addAction(action)
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
            IsSucceeded = QDesktopServices.openUrl(QURL)
            runCMD([f'start {url}']) if not IsSucceeded else None
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