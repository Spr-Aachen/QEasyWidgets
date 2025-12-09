from enum import Enum

##############################################################################################################################

class Direction(Enum):
    Up = 1
    Down = 2


class Position(Enum):
    Top = 1
    Bottom = 2
    Left = 3
    Right = 4
    TopLeft = 5
    TopRight = 6
    BottomLeft = 7
    BottomRight = 8


class Status(Enum):
    Loading = 1


class ChatRole(Enum):
    User = 1
    Contact = 2

##############################################################################################################################