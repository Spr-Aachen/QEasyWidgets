import locale

##############################################################################################################################

class Language:
    '''
    '''
    ZH = 'zh'
    EN = 'en'

    Auto = locale.getdefaultlocale()[0]


class LanguageBase:
    '''
    '''
    LANG = Language.ZH if Language.Auto in ('zh', 'zh_CN') else Language.EN

    def update(self, language: str):
        if language in (Language.ZH, Language.EN):
            self.LANG = language


EasyLanguage = LanguageBase()


def currentLanguage():
    return EasyLanguage.LANG

##############################################################################################################################