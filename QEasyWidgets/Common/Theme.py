import darkdetect

##############################################################################################################################

class Theme:
    '''
    '''
    Dark = 'Dark'
    Light = 'Light'

    Auto = darkdetect.theme()


class ThemeBase:
    '''
    '''
    THEME = Theme.Auto if Theme.Auto is not None else Theme.Dark

    def Update(self, theme: str):
        if theme in (Theme.Dark, Theme.Light):
            self.THEME = theme


EasyTheme = ThemeBase()

##############################################################################################################################