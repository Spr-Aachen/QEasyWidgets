import sys

from .Bar import TitleBarBase
if sys.platform == "win32":
    from .Window_win32 import WindowBase
else:
    from .Window_linux import WindowBase