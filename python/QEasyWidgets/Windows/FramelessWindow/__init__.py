import sys

if sys.platform == "win32":
    from .framelessWindow_win32 import WindowBase
else:
    from .framelessWindow_linux import WindowBase