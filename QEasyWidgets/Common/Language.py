import locale

##############################################################################################################################

class Language:
    '''
    '''
    ZH = 'Chinese'
    EN = 'English'

    Auto = locale.getdefaultlocale()[0]


class LanguageBase:
    '''
    '''
    LANG = 'Chinese' if Language.Auto in ('zh', 'zh_CN') else 'English'

    def update(self, language: str):
        if language in (Language.ZH, Language.EN):
            self.LANG = language


EasyLanguage = LanguageBase()


def currentLanguage():
    return EasyLanguage.LANG

##############################################################################################################################