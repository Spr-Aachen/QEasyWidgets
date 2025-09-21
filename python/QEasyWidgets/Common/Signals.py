from PySide6.QtCore import QObject, Signal

##############################################################################################################################

class CustomSignals_ComponentsCustomizer(QObject):
    '''
    Set up signals for components
    '''
    # Set theme
    setTheme = Signal(str)

    # Set language
    setLanguage = Signal(str)


componentsSignals = CustomSignals_ComponentsCustomizer()

##############################################################################################################################