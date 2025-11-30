from enum import Enum

##############################################################################################################################

class Position(Enum):
    Top = 0
    Bottom = 1
    Left = 2
    Right = 3
    TopLeft = 4
    TopRight = 5
    BottomLeft = 6
    BottomRight = 7


class Status(Enum):
    Loading = 0


class ChatRole(Enum):
    User = 0
    Contact = 1

##############################################################################################################################