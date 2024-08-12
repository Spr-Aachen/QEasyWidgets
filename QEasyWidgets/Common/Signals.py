from PySide6.QtCore import QObject, Signal

##############################################################################################################################

class CustomSignals_ComponentsCustomizer(QObject):
    '''
    Set up signals for components
    '''
    # Set theme
    Signal_SetTheme = Signal(str)

    # Set language
    Signal_SetLanguage = Signal(str)
    '''
    # Get clicked button
    Signal_ClickedButton = Signal(QMessageBox.StandardButton)
    '''

ComponentsSignals = CustomSignals_ComponentsCustomizer()

##############################################################################################################################