from pathlib import Path
from typing import Union, Optional
from PySide6.QtCore import QTranslator
from PySide6.QtWidgets import QApplication

from .Signals import ComponentsSignals
from .Language import EasyLanguage, currentLanguage

##############################################################################################################################

class TranslationBase(QTranslator):
    '''
    '''
    def __init__(self, parent = None):
        super().__init__(parent)

    def load(self, language: Optional[str] = None):
        EasyLanguage.update(language) if language is not None else None

        Prefix = 'QM'
        FilePath = f'i18n/{currentLanguage()}.qm'
        FilePath = Path(f':/{Prefix}').joinpath(FilePath).as_posix()

        super().load(FilePath)


Translator = None
def Function_UpdateLanguage(
    language: Optional[str] = None
):
    '''
    '''
    global Translator

    QApplication.instance().processEvents()

    # Remove old translator
    QApplication.instance().removeTranslator(Translator) if Translator is not None else None

    # Install new translator
    Translator = TranslationBase()
    Translator.load(language)
    QApplication.instance().installTranslator(Translator)


ComponentsSignals.Signal_SetLanguage.connect(Function_UpdateLanguage)

##############################################################################################################################