import os
import sys
import re
import json
import unicodedata
import io
import shutil
import psutil
import signal
import shlex
import subprocess
import collections
collections.Iterable = collections.abc.Iterable
import inspect
import hashlib
import urllib
import urllib.parse as urlparse
import polars
import sqlalchemy
import platform
import configparser
from pathlib import Path
from github import Github
from packaging import version
from tqdm import tqdm
from enum import Enum
from typing import Union, Optional, Type, Tuple, Callable, Any
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
from functools import singledispatch, wraps
from decimal import Decimal
from datetime import datetime

#############################################################################################################

def toIterable(
    items,
    ignoreString: bool = True
):
    '''
    Function to make item iterable
    '''
    if isinstance(items, collections.Iterable) or hasattr(items, '__iter__'):
        ItemList = [items] if isinstance(items, (str, bytes)) and ignoreString else items
    else:
        ItemList = [items]

    return ItemList


def itemReplacer(
    dict: dict,
    items: object
):
    '''
    Function to replace item using dictionary lookup
    '''
    ItemList = toIterable(items, ignoreString = False)

    ItemList_New = [dict.get(Item, Item) for Item in ItemList]

    if isinstance(items, list):
        return ItemList_New
    if isinstance(items, tuple):
        return tuple(ItemList_New)
    if isinstance(items, (int, float, bool)):
        return ItemList_New[0]
    if isinstance(items, str):
        return str().join(ItemList_New)


def findKey(
    dict: dict,
    targetValue
):
    """
     Find key from dictionary
     """
    for Key, value in dict.items():
        if value == targetValue:
            return Key

#############################################################################################################

def normPath(
    string: Union[str, Path],
    pathType: Optional[str] = None,
    trailingSlash: Optional[bool] = None
):
    """
    Normalize path string
    """
    try:
        if str(string).strip() == '':
            raise
        PathString = Path(string)#.resolve()

    except:
        return None

    else: #if re.search(r':[/\\\\]', str(string)) or re.search(r'\./', str(string)):
        if trailingSlash is None:
            trailingSlash = True if str(string).endswith(('/', '\\')) else False
        if platform.system() == 'Windows' or pathType == 'Win32':
            string = PathString.as_posix().replace(r'/', '\\')
            string += '\\' if trailingSlash else ''
        if platform.system() == 'Linux' or pathType == 'Posix':
            string = PathString.as_posix()
            string += '/' if trailingSlash else ''
        return string

#############################################################################################################

def rawString(
    text: str
):
    """
    Return as raw string representation of text
    """
    RawMap = {
        7: r'\a',
        8: r'\b',
        9: r'\t',
        10: r'\n',
        11: r'\v',
        12: r'\f',
        13: r'\r'
    }
    text = r''.join([RawMap.get(ord(Char), Char) for Char in text])
    '''
    StringRepresentation = repr(text)[1:-1] #StringRepresentation = text.encode('unicode_escape').decode()
    return re.sub(r'\\+', lambda arg: r'\\', StringRepresentation).replace(r'\\', '\\').replace(r'\'', '\'') #return eval("'%s'" % canonical_string)
    '''
    return unicodedata.normalize('NFKC', text)


class subprocessManager:
    """

    """
    def __init__(self,
        communicateThroughConsole: bool = False
    ):
        self.communicateThroughConsole = communicateThroughConsole

        self.Encoding = 'gbk' if platform.system() == 'Windows' else 'utf-8'

    def create(self,
        args: Union[list[Union[list, str]], str],
    ):
        if not self.communicateThroughConsole:
            for Arg in toIterable(args):
                Arg = shlex.split(Arg) if isinstance(Arg, str) else Arg
                self.Subprocess = subprocess.Popen(
                    args = Arg,
                    stdout = subprocess.PIPE,
                    stderr = subprocess.PIPE,
                    env = os.environ,
                    creationflags = subprocess.CREATE_NO_WINDOW
                )

        else:
            TotalInput = str()
            for Arg in toIterable(args):
                Arg = shlex.join(Arg) if isinstance(Arg, list) else Arg
                TotalInput += f'{rawString(Arg)}\n'
            self.TotalInput = TotalInput.encode(self.Encoding, errors = 'replace')
            if platform.system() == 'Windows':
                ShellArgs = ['cmd']
            if platform.system() == 'Linux':
                ShellArgs = ['bash', '-c']
            self.Subprocess = subprocess.Popen(
                args = ShellArgs,
                stdin = subprocess.PIPE,
                stdout = subprocess.PIPE,
                stderr = subprocess.STDOUT,
                env = os.environ,
                creationflags = subprocess.CREATE_NO_WINDOW
            )

        return self.Subprocess

    def monitor(self,
        showProgress: bool = False,
        decodeResult: Optional[bool] = None,
        logPath: Optional[str] = None
    ):
        if not self.communicateThroughConsole:
            TotalOutput, TotalError = (bytes(), bytes())
            if showProgress:
                Output, Error = (bytes(), bytes())
                for Line in io.TextIOWrapper(self.Subprocess.stdout, encoding = self.Encoding, errors = 'replace'):
                    Output += Line.encode(self.Encoding, errors = 'replace')
                    sys.stdout.write(Line) if sys.stdout is not None else None
                    if logPath is not None:
                        with open(logPath, mode = 'a', encoding = 'utf-8') as Log:
                            Log.write(Line)
                    self.Subprocess.stdout.flush()
                    if self.Subprocess.poll() is not None:
                        break
                for Line in io.TextIOWrapper(self.Subprocess.stderr, encoding = self.Encoding, errors = 'replace'):
                    Error += Line.encode(self.Encoding, errors = 'replace')
                    sys.stderr.write(Line) if sys.stderr is not None else None
                    if logPath is not None:
                        with open(logPath, mode = 'a', encoding = 'utf-8') as Log:
                            Log.write(Line)
            else:
                Output, Error = self.Subprocess.communicate()
                Output, Error = b'' if Output is None else Output, b'' if Error is None else Error
            TotalOutput, TotalError = TotalOutput + Output, TotalError + Error

        else:
            if showProgress:
                TotalOutput, TotalError = (bytes(), bytes())
                self.Subprocess.stdin.write(self.TotalInput)
                self.Subprocess.stdin.close()
                for Line in io.TextIOWrapper(self.Subprocess.stdout, encoding = self.Encoding, errors = 'replace'):
                    TotalOutput += Line.encode(self.Encoding, errors = 'replace')
                    sys.stdout.write(Line) if sys.stdout is not None else None
                    if logPath is not None:
                        with open(logPath, mode = 'a', encoding = 'utf-8') as Log:
                            Log.write(Line)
                    self.Subprocess.stdout.flush()
                    if self.Subprocess.poll() is not None:
                        break
                if self.Subprocess.wait() != 0:
                    TotalError = b"Error occurred, please check the logs for full command output."
            else:
                TotalOutput, TotalError = self.Subprocess.communicate(self.TotalInput)
                TotalOutput, TotalError = b'' if TotalOutput is None else TotalOutput, b'' if TotalError is None else TotalError

        TotalOutput, TotalError = TotalOutput.strip(), TotalError.strip()
        TotalOutput, TotalError = TotalOutput.decode(self.Encoding, errors = 'ignore') if decodeResult else TotalOutput, TotalError.decode(self.Encoding, errors = 'ignore') if decodeResult else TotalError

        return None if TotalOutput in ('', b'') else TotalOutput, None if TotalError in ('', b'') else TotalError, self.Subprocess.returncode


def runCMD(
    args: Union[list[Union[list, str]], str],
    showProgress: bool = False,
    communicateThroughConsole: bool = False,
    decodeResult: Optional[bool] = None,
    logPath: Optional[str] = None
):
    """
    Run command
    """
    ManageSubprocess = subprocessManager(communicateThroughConsole)
    ManageSubprocess.create(args)
    return ManageSubprocess.monitor(showProgress, decodeResult, logPath)


def setEnvVar(
    variable: str,
    value: str,
    type: str = 'Temp',
    affectOS: bool = True
):
    """
    Set environment variable
    """
    value = rawString(value)

    if type == 'Sys':
        if platform.system() == 'Windows':
            runCMD(
                # args = [
                #     f'set VAR={value}{os.pathsep}%{variable}%',
                #     f'reg add "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" /v "{variable}" /t REG_EXPAND_SZ /d "%VAR%" /f',
                # ],
                args = [
                    f'for /f "usebackq tokens=2,*" %A in (`reg query "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" /v "{variable}"`) do set sysVAR=%B',
                    f'setx "{variable}" "{value}{os.pathsep}%sysVAR%" /m'
                ],
                communicateThroughConsole = True
            )
        if platform.system() == 'Linux':
            '''
            runCMD(
                f'echo {variable}={value} >> /etc/environment',
                communicateThroughConsole = True
            )
            '''
            with open('/etc/environment', 'a') as f:
                f.write(f'\n{variable}="{value}"\n')

    if type == 'User':
        if platform.system() == 'Windows':
            runCMD(
                # args = [
                #     f'set VAR={value}{os.pathsep}%{variable}%',
                #     f'reg add "HKEY_CURRENT_USER\\Environment" /v "{variable}" /t REG_EXPAND_SZ /d "%VAR%" /f',
                # ],
                args = [
                    f'for /f "usebackq tokens=2,*" %A in (`reg query "HKEY_CURRENT_USER\\Environment" /v "{variable}"`) do set userVAR=%B',
                    f'setx "{variable}" "{value}{os.pathsep}%userVAR%"'
                ],
                communicateThroughConsole = True
            )
        if platform.system() == 'Linux':
            shell = os.environ.get('SHELL', '/bin/bash')
            if 'bash' in shell:
                config_file = os.path.expanduser('~/.bashrc')
            elif 'zsh' in shell:
                config_file = os.path.expanduser('~/.zshrc')
            else:
                config_file = os.path.expanduser('~/.profile')
            with open(config_file, 'a') as f:
                f.write(f'\nexport {variable}="{value}"\n')

    if type == 'Temp' or affectOS:
        EnvValue = os.environ.get(variable)
        if EnvValue is not None and normPath(value, 'Posix') not in [normPath(value, 'Posix') for value in EnvValue.split(os.pathsep)]:
            EnvValue = f'{value}{os.pathsep}{EnvValue}' #EnvValue = f'{EnvValue}{os.pathsep}{value}'
        else:
            EnvValue = value
        os.environ[variable] = EnvValue

#############################################################################################################

def isJson(content: str):
    """
    Check if content is a json
    """
    try:
        json.loads(json.dumps(eval(content)))
        return True
    except:
        return False


def isUrl(content: str):
    """
    Check if content is a url
    """
    if urlparse.urlparse(content).scheme in ['http', 'https']:
        return True
    else:
        return False


@polars.Config( 
    tbl_formatting = "ASCII_MARKDOWN",        
    tbl_hide_column_data_types = True,
    tbl_hide_dataframe_shape = True,
)
def _toMarkdown(df: polars.DataFrame) -> str:
    return str(df)


def toMarkdown(content: str):
    """
    Convert content to markdown
    """
    if isUrl(content):
        content = f"[URL]({content})"
    if isJson(content):
        content = _toMarkdown(polars.DataFrame(json.loads(json.dumps(eval(content)))))
    return content


class richTextManager:
    """
    Manage rich text
    """
    def __init__(self):
        self.richTextLines = []

    def _toHtml(self, text, align, size, weight, letterSpacing, lineHeight):
        Style = f"'text-align:{align}; font-size:{size}pt; font-weight:{weight}; letter-spacing: {letterSpacing}px; line-height: {lineHeight}px'"
        content = re.sub(
            pattern = "[\n]",
            repl = "<br>",
            string = text
        ) if text is not None else None
        return f"<p style={Style}>{content}</p>" if content is not None else ''

    def addTitle(self,
        text: Optional[str] = None,
        align: str = "left",
        size: float = 12.3,
        weight: float = 630.,
        spacing: float = 0.9,
        lineHeight: float = 24.6,
    ):
        head = f"<body>{self._toHtml(text, align, size, weight, spacing, lineHeight)}</body>" #head = f"<head><title>{self._toHtml(text, align, size, weight, spacing, lineHeight)}</title></head>"
        self.richTextLines.append(head)
        return self

    def addBody(self,
        text: Optional[str] = None,
        align: str = "left",
        size: float = 9.3,
        weight: float = 420.,
        spacing: float = 0.6,
        lineHeight: float = 22.2,
    ):
        body = f"<body>{self._toHtml(text, align, size, weight, spacing, lineHeight)}</body>"
        self.richTextLines.append(body)
        return self

    def richText(self):
        richText = "<html>\n%s\n</html>" % '\n'.join(self.richTextLines)
        return (richText)


def setRichText(
    text: str = "",
    align: str = "left",
    size: float = 9.6,
    weight: float = 480.,
    spacing: float = 0.75,
    lineHeight: float = 23.4,
):
    """
    Function to set rich text
    """
    return richTextManager().addBody(text, align, size, weight, spacing, lineHeight).richText()

#############################################################################################################

def findURL(
    string: str
):
    """
    Function to find URL in a string
    """
    URLList = re.compile(r'http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+').findall(rawString(string))
    URL = URLList[0]
    return URL

#############################################################################################################

def getDecimalPlaces(
    number: Union[int, float]
):
    """
    Function to get decimal places of a number
    """
    return abs(Decimal(str(number)).as_tuple().exponent)

#############################################################################################################

def getClassFromMethod(
    method: object
):
    """
    Function to get class from method
    """
    '''
    Modules = list(inspect.getmodule(method).__dict__.values())
    Modules = [Module for Module in Modules if str(Module).startswith("<class '__main__.")]
    return Modules[-1]
    '''
    return inspect.getmodule(method).__dict__[method.__qualname__.split('.')[0]]

#############################################################################################################

def runEvents(
    events: Union[list, dict]
):
    """
    Function to run events
    """
    if isinstance(events, list):
        for Event in events:
            Event() if Event is not None else None
    if isinstance(events, dict):
        for Event, Param in events.items():
            Event(*toIterable(Param if Param is not None else ())) if Event is not None else None

#############################################################################################################

class taskAccelerationManager(Enum):
    """
    Class to accelerate tasks by using thread or process pools
    """
    ThreadPool = 0
    ProcessPool = 1

    def createPool(self, 
        funcDict: dict,
        asynchronous: bool = True,
        maxWorkers: Optional[int] = None
    ) -> Union[ThreadPoolExecutor, ProcessPoolExecutor]:
        if self.value == self.ThreadPool.value:
            self.executor = ThreadPoolExecutor(maxWorkers)
        if self.value == self.ProcessPool.value:
            self.executor = ProcessPoolExecutor(maxWorkers)
        for func, args in funcDict.items():
            future = self.executor.submit(func, *args)
            future.result() if asynchronous == False else None
        return self.executor


def processTerminator(
    program: Union[str, int],
    selfIgnored: bool = True,
    searchKeyword: bool = False
):
    """
    Kill a process by its PID or name
    """
    if isinstance(program, int):
        PID = program
        try:
            Process = psutil.Process(PID)
        except psutil.NoSuchProcess: # Process already terminated
            return

        ProcessList =  Process.children(recursive = True) + [Process]
        for Process in ProcessList:
            try:
                if Process.pid == os.getpid() and selfIgnored:
                    continue
                os.kill(Process.pid, signal.SIGTERM)
            except:
                pass

    if isinstance(program, str):
        name = program
        programPath = normPath(name) if normPath(name) is not None else name
        for Process in psutil.process_iter():
            ProcessList =  Process.children(recursive = True) + [Process]
            try:
                for Process in ProcessList:
                    if Process.pid == os.getpid() and selfIgnored:
                        continue
                    ProcessPath = Process.exe()
                    if programPath == ProcessPath or (programPath.lower() in ProcessPath.lower() and searchKeyword):
                        Process.send_signal(signal.SIGTERM) #Process.kill()
            except:
                pass


def occupationTerminator(
    file: str,
    searchKeyword: bool = False
):
    """
    Terminate all processes that are currently using the file
    """
    filePath = normPath(file) if normPath(file) is not None else file
    for Process in psutil.process_iter():
        try:
            PopenFiles = Process.open_files()
            for PopenFile in PopenFiles:
                PopenFilePath = PopenFile.path
                if filePath == PopenFilePath or (filePath.lower() in PopenFilePath.lower() and searchKeyword):
                    Process.send_signal(signal.SIGTERM) #Process.kill()
        except:
            pass

#############################################################################################################

def renameIfExists(
    pathStr: str
):
    """
    If pathStr already exists, rename it to pathStr(0), pathStr(1), etc.
    """
    ParentDirectory, name = os.path.split(pathStr)
    suffix = Path(name).suffix
    if len(suffix) > 0:
        while Path(pathStr).exists():
            pattern = r'(\d+)\)\.'
            if re.search(pattern, name) is None:
                name = name.replace('.', '(0).')
            else:
                CurrentNumber = int(re.findall(pattern, name)[-1])
                name = name.replace(f'({CurrentNumber}).', f'({CurrentNumber + 1}).')
            pathStr = Path(ParentDirectory).joinpath(name).as_posix()
    else:
        while Path(pathStr).exists():
            pattern = r'(\d+)\)'
            match = re.search(pattern, name)
            if match is None:
                name += '(0)'
            else:
                CurrentNumber = int(match.group(1))
                name = name[:match.start(1)] + f'({CurrentNumber + 1})'
            pathStr = Path(ParentDirectory).joinpath(name).as_posix()
    return pathStr


def cleanDirectory(
    directory: str,
    whiteList: list
):
    """
    Remove all files and folders in directory except those in whiteList
    """
    if os.path.exists(directory):
        for DirPath, Folders, Files in os.walk(directory, topdown = False):
            for file in Files:
                filePath = os.path.join(DirPath, file)
                try:
                    if not any(file in filePath for file in whiteList):
                        os.remove(filePath)
                except:
                    pass
            for Folder in Folders:
                FolderPath = os.path.join(DirPath, Folder)
                try:
                    if not any(Folder in FolderPath for Folder in whiteList):
                        shutil.rmtree(FolderPath)
                except:
                    pass


def moveFiles(
    directory: str,
    destination: str
):
    """
    Move all files and folders in directory to destination
    """
    for DirPath, FolderNames, fileNames in os.walk(directory):
        for FolderName in FolderNames:
            if directory != destination:
                shutil.move(os.path.join(DirPath, FolderName), destination)
        for fileName in fileNames:
            if directory != destination:
                shutil.move(os.path.join(DirPath, fileName), destination)


def getPaths(
    directory: str,
    name: str,
    searchKeyword: bool = True
):
    """
    Get all paths of files and folders in directory
    """
    Result = []

    for DirPath, FolderNames, fileNames in os.walk(directory):
        for FolderName in FolderNames:
            if name == FolderName or (name in FolderName and searchKeyword is True):
                Result.append(os.path.join(DirPath, FolderName))
            else:
                pass
        for fileName in fileNames:
            if name == fileName or (name in fileName and searchKeyword is True):
                Result.append(os.path.join(DirPath, fileName))
            else:
                pass

    return Result if len(Result) > 0 else None

#############################################################################################################

def getBaseDir(
    filePath: Optional[str] = None,
    parentLevel: Optional[int] = None,
    searchMEIPASS: bool = False
):
    """
    Get the parent directory of file, or get the MEIPASS if file is compiled with pyinstaller
    """
    if filePath is not None:
        BaseDir = normPath(Path(str(filePath)).absolute().parents[parentLevel if parentLevel is not None else 0])
    elif searchMEIPASS and getattr(sys, 'frozen', None):
        BaseDir = normPath(sys._MEIPASS)
    else:
        BaseDir = None

    return BaseDir


def getFileInfo(
    file: Optional[str] = None
):
    """
    Check whether python file is compiled
    """
    if file is None:
        fileName = Path(sys.argv[0]).name
        if getattr(sys, 'frozen', None):
            isFileCompiled = True
        else:
            isFileCompiled = False if fileName.endswith('.py') or sys.executable.endswith('python.exe') else True
    else:
        fileName = Path(normPath(file)).name
        isFileCompiled = False if fileName.endswith('.py') else True

    return fileName, isFileCompiled

#############################################################################################################

def isVersionSatisfied(
    currentVersion: str,
    versionReqs: str
):
    """
    Check if the version requirements are satisfied
    """
    if versionReqs is None:
        return True
    versionReqs = versionReqs.split(',') if isinstance(versionReqs, str) else list(versionReqs)
    results = []
    for VersionReq in versionReqs:
        SplitVersionReq = re.split('=|>|<', VersionReq)
        RequiredVersion = SplitVersionReq[-1]
        Req = VersionReq[:len(VersionReq) - len(RequiredVersion)]
        if Req == "==":
            results.append(version.parse(currentVersion) == version.parse(RequiredVersion))
        if Req == ">=":
            results.append(version.parse(currentVersion) >= version.parse(RequiredVersion))
        if Req == "<=":
            results.append(version.parse(currentVersion) <= version.parse(RequiredVersion))
        return True if False not in results else False


def isSystemSatisfied(
    systemReqs: str
):
    """
    Check if the system requirements are satisfied
    """
    if systemReqs is None:
        return True
    systemReqs = systemReqs.split(';') if isinstance(systemReqs, str) else list(systemReqs)
    results = []
    for systemReq in systemReqs:
        SplitsystemReq = re.split('=|>|<', systemReq)
        RequiredSystem = SplitsystemReq[-1].strip()
        Req = systemReq[len(SplitsystemReq[0]) : len(systemReq) - len(RequiredSystem)].strip()
        if Req == "==":
            results.append(sys.platform == eval(RequiredSystem))
        if Req == "!=":
            results.append(sys.platform != eval(RequiredSystem))
        return True if False not in results else False

#############################################################################################################

def runScript(
    commandList: list[str],
    scriptPath: Optional[str]
):
    """
    Run a script with bash or bat
    """
    if platform.system() == 'Linux':
        scriptPath = Path.cwd().joinpath('Bash.sh') if scriptPath is None else normPath(scriptPath)
        with open(scriptPath, 'w') as BashFile:
            Commands = "\n".join(commandList)
            BashFile.write(Commands)
        os.chmod(scriptPath, 0o755) # 给予可执行权限
        subprocess.Popen(['bash', scriptPath])
    if platform.system() == 'Windows':
        scriptPath = Path.cwd().joinpath('Bat.bat') if scriptPath is None else normPath(scriptPath)
        with open(scriptPath, 'w') as BatFile:
            Commands = "\n".join(commandList)
            BatFile.write(Commands)
        subprocess.Popen([scriptPath], creationflags = subprocess.CREATE_NEW_CONSOLE)


def bootWithScript(
    programPath: str = ...,
    delayTime: int = 3,
    scriptPath: Optional[str] = None
):
    """
    Boot the program with a script
    """
    if platform.system() == 'Linux':
        _, isFileCompiled = getFileInfo(programPath)
        runScript(
            commandList = [
                '#!/bin/bash',
                f'sleep {delayTime}',
                f'./"{programPath}"' if isFileCompiled else f'python3 "{programPath}"',
                'rm -- "$0"'
            ],
            scriptPath = scriptPath
        )
    if platform.system() == 'Windows':
        _, isFileCompiled = getFileInfo(programPath)
        runScript(
            commandList = [
                '@echo off',
                f'ping 127.0.0.1 -n {delayTime + 1} > nul',
                f'start "Programm Running" "{programPath}"' if isFileCompiled else f'python "{programPath}"',
                'del "%~f0"'
            ],
            scriptPath = scriptPath
        )

##############################################################################################################################

def checkUpdateFromGithub(
    accessToken: Optional[str] = None,
    repoOwner: str = ...,
    repoName: str = ...,
    fileName: str = ...,
    fileFormat: str = ...,
    currentVersion: str = ...
):
    """
    Check if there is an update available on Github
    """
    try:
        PersonalGit = Github(accessToken)
        Repo = PersonalGit.get_repo(f"{repoOwner}/{repoName}")
        latestVersion = Repo.get_tags()[0].name
        latestRelease = Repo.get_latest_release() #latestRelease = Repo.get_release(latestVersion)
        for Index, Asset in enumerate(latestRelease.assets):
            if Asset.name == f"{fileName}.{fileFormat}":
                IsUpdateNeeded = True if version.parse(currentVersion) < version.parse(latestVersion) else False
                downloadURL = Asset.browser_download_url #downloadURL = f"https://github.com/{repoOwner}/{repoName}/releases/download/{latestVersion}/{fileName}.{fileFormat}"
                VersionInfo = latestRelease.body
                return IsUpdateNeeded, downloadURL, VersionInfo
            elif Index + 1 == len(latestRelease.assets):
                raise Exception(f"No file found with name {fileName}.{fileFormat} in the latest release")

    except Exception as e:
        print(f"Error occurred while checking for updates: \n{e}")


def downloadFile(
    downloadURL: str,
    downloadDir: str,
    fileName: str,
    fileFormat: str,
    sha: Optional[str],
    createNewConsole: bool = False
) -> Tuple[Union[bytes, str], str]:
    """
    Downloads a file from a given URL and saves it to a specified directory
    """
    os.makedirs(downloadDir, exist_ok = True)

    downloadName = fileName + (fileFormat if '.' in fileFormat else f'.{fileFormat}')
    downloadPath = normPath(Path(downloadDir).joinpath(downloadName).absolute())

    def Download():
        try:
            runCMD(
                args = [
                    'aria2c',
                    f'''
                    {('cmd.exe /c start ' if platform.system() == 'Windows' else 'x-terminal-emulator -e ') if createNewConsole else ''}
                    aria2c "{downloadURL}" --dir="{Path(downloadPath).parent.as_posix()}" --out="{Path(downloadPath).name}" -x6 -s6 --file-allocation=none --force-save=false
                    '''
                ]
            )
        except:
            with urllib.request.urlopen(downloadURL) as source, open(downloadPath, "wb") as output:
                with tqdm(total = int(source.info().get("content-Length")), ncols = 80, unit = 'iB', unit_scale = True, unit_divisor = 1024) as loop:
                    while True:
                        buffer = source.read(8192)
                        if not buffer:
                            break
                        output.write(buffer)
                        loop.update(len(buffer))
        finally:
            return open(downloadPath, "rb").read() if Path(downloadPath).exists() else None

    if os.path.exists(downloadPath):
        if os.path.isfile(downloadPath) == False:
            raise RuntimeError(f"{downloadPath} exists and is not a regular file")
        elif sha is not None:
            with open(downloadPath, "rb") as f:
                FileBytes = f.read()
            if len(sha) == 40:
                SHA_Current = hashlib.sha1(FileBytes).hexdigest()
            if len(sha) == 64:
                SHA_Current = hashlib.sha256(FileBytes).hexdigest()
            FileBytes = Download() if SHA_Current != sha else FileBytes #Download() if SHA_Current != sha else None
        else:
            os.remove(downloadPath)
            FileBytes = Download()
    else:
        FileBytes = Download()

    if FileBytes is None:
        raise Exception('Download Failed!')

    return FileBytes, downloadPath

#############################################################################################################

class sqlManager:
    """
    Manage SQL
    """
    historydbName = "history"
    historydbTableName = "historyTable"

    file_path = None
    filedb_name = None

    def exportDataToFiledb(self, df: polars.DataFrame, new: bool = True):
        '''
        将DataFrame导入文件数据库
        '''
        self.filedb_name = f"DB_{datetime.now().strftime('%Y%m%d%H%M%S')}" if new else self.filedb_name
        filedb_engine = sqlalchemy.create_engine(f'sqlite:///{self.filedb_name}.db')
        df.write_database(
            table_name = self.filedb_name,
            connection = filedb_engine,
            if_table_exists = 'replace'
        )

    def loadDataFromFiledb(self):
        '''
        从文件数据库中导出DataFrame
        '''
        filedb_engine = sqlalchemy.create_engine(f'sqlite:///{self.filedb_name}.db') # 与文件数据库建立连接
        df = polars.read_database(
            f"SELECT * FROM {self.filedb_name}",
            connection = filedb_engine
        )
        df.fill_nan("")
        return df

    def createHistorydb(self):
        '''
        创建历史记录数据库并初始化历史记录Table
        '''
        self.historyEngine = sqlalchemy.create_engine(f'sqlite:///{self.historydbName}.db')
        if not sqlalchemy.inspect(self.historyEngine).has_table(self.historydbTableName):
            df = polars.DataFrame({
                "file_hash": [],
                "filedb_name": []
            })
            df.write_database(
                table_name = self.historydbTableName,
                connection = self.historyEngine,
                if_table_exists = 'replace'
            )

    def toHistorydb(self):
        '''
        将[表格哈希值,表格数据库名]写入历史记录数据库
        '''
        file_hash = hashlib.md5(open(self.file_path, 'rb').read()).hexdigest()
        df = polars.DataFrame({
            "file_hash": [file_hash],
            "filedb_name": [self.filedb_name]
        })
        df.write_database(
            table_name = self.historydbTableName,
            connection = self.historyEngine,
            if_table_exists = 'append'
        )
        # TODO 通过redis建立旁路缓存模式

    def chkHistorydb(self):
        '''
        检查文件哈希值在历史记录数据库中的对应值
        '''
        file_hash = hashlib.md5(open(self.file_path, 'rb').read()).hexdigest()
        print(f'checking hash {file_hash} in {self.historydbName}.db')
        df = polars.read_database(
            f"SELECT * FROM {self.historydbTableName} WHERE file_hash = '{file_hash}'",
            connection = self.historyEngine
        )
        filedb_name = df.row(0, named = True)["filedb_name"] if len(df) > 0 else None
        print(f'filedb name found: {filedb_name}')
        return filedb_name

##############################################################################################################################

class configManager:
    """
    Manage config
    """
    def __init__(self,
        configPath: Optional[str] = None
    ):
        self.configPath = normPath(Path(os.getenv('SystemDrive')).joinpath('Config.ini')) if configPath == None else configPath
        os.makedirs(Path(self.configPath).parent, exist_ok = True)

        self.configParser = configparser.ConfigParser()
        try:
            self.configParser.read(self.configPath, encoding = 'utf-8')
        except:
            with open(self.configPath, 'w'):
                pass
            self.configParser.clear()

    def parser(self):
        return self.configParser

    def editConfig(self,
        section: str = ...,
        option: str = ...,
        value: str = ...,
        configParser: Optional[configparser.ConfigParser] = None
    ):
        configParser = self.parser() if configParser == None else configParser
        try:
            configParser.add_section(section)
        except:
            pass
        configParser.set(section, option, value)
        with open(self.configPath, 'w', encoding = 'utf-8') as Config:
            configParser.write(Config)

    def getValue(self,
        section: str = ...,
        option: str = ...,
        initValue: Optional[str] = None,
        configParser: Optional[configparser.ConfigParser] = None
    ):
        configParser = self.parser() if configParser == None else configParser
        try:
            value = configParser.get(section, option)
        except:
            if initValue != None:
                self.editConfig(section, option, initValue, configParser)
                return initValue
            else:
                raise Exception("Need initial value")
        return value

#############################################################################################################

class singledispatchmethod:
    '''
    Single-dispatch generic method descriptor.

    Supports wrapping existing descriptors and handles non-descriptor callables as instance methods.
    '''
    def __init__(self, method: Callable):
        if not (callable(method) and hasattr(method, "__get__")):
            raise TypeError(f"{method!r} is not callable or a descriptor")

        self.method = method

        self.dispatcher = singledispatch(method)

    def register(self, cls: Type, method: Optional[Callable] = None):
        '''
        Register a new implementation for the given class on a generic method.

        :param cls: The class to register the method for.
        :param method: The method to register. If None, returns a decorator.
        :return: A decorator function if method is None, otherwise None.
        '''
        return self.dispatcher.register(cls, func = method)

    @property
    def isAbstractMethod(self) -> bool:
        '''
        Check if the method is an abstract method.
        '''
        return getattr(self.method, '__isabstractmethod__', False)

    def __get__(self, obj: Any, cls: Optional[Type] = None):
        @wraps(self.method)
        def method(*args, **kwargs):
            '''
            Ref: https://stackoverflow.com/questions/24601722
            '''
            if args:
                method = self.dispatcher.dispatch(args[0].__class__)
            else:
                method = self.method
                for v in kwargs.values():
                    if v.__class__ in self.dispatcher.registry:
                        method = self.dispatcher.dispatch(v.__class__)
                        if method is not self.method:
                            break
            return method.__get__(obj, cls)(*args, **kwargs)
        method.register = self.register
        method.__isabstractmethod__ = self.isAbstractMethod
        return method

#############################################################################################################