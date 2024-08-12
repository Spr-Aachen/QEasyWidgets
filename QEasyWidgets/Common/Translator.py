from pathlib import Path
from typing import Union, Optional
from PySide6.QtCore import QTranslator

from .Language import EasyLanguage

##############################################################################################################################

class TranslationBase(QTranslator):
    '''
    '''
    def __init__(self, parent = None):
        super().__init__(parent)

    def load(self, language: Optional[str] = None):
        EasyLanguage.Update(language) if language is not None else None

        Prefix = 'QM'
        FilePath = f'i18n/{EasyLanguage.LANG}.qm'
        FilePath = Path(f':/{Prefix}').joinpath(FilePath).as_posix()

        super().load(FilePath)


"""
def Function_UpdateLanguage(
    language: Optional[str] = None
):
    '''
    '''
    QApplication.processEvents()

    Translator = TranslationBase()
    Translator.load(language)

    QApplication.instance().installTranslator(Translator)


ComponentsSignals.Signal_SetLanguage.connect(Function_UpdateLanguage)
"""

##############################################################################################################################