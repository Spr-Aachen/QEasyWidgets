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

    ## Get clicked button
    #Signal_ClickedButton = Signal(QMessageBox.StandardButton)


componentsSignals = CustomSignals_ComponentsCustomizer()

##############################################################################################################################