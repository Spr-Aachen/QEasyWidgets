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
import platform
import configparser
from pathlib import Path
from github import Github
from packaging import version
from tqdm import tqdm
from typing import Union, Optional, Type, Tuple, Callable, Any
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
from threading import currentThread
from functools import singledispatch, wraps

#############################################################################################################

def ToIterable(
    Items,
    IgnoreStr: bool = True
):
    '''
    Function to make item iterable
    '''
    if isinstance(Items, collections.Iterable) or hasattr(Items, '__iter__'):
        ItemList = [Items] if isinstance(Items, (str, bytes)) and IgnoreStr else Items
    else:
        ItemList = [Items]

    return ItemList


def ItemReplacer(
    Dict: dict,
    Items: object
):
    '''
    Function to replace item using dictionary lookup
    '''
    ItemList = ToIterable(Items, IgnoreStr = False)

    ItemList_New = [Dict.get(Item, Item) for Item in ItemList]

    if isinstance(Items, list):
        return ItemList_New
    if isinstance(Items, tuple):
        return tuple(ItemList_New)
    if isinstance(Items, (int, float, bool)):
        return ItemList_New[0]
    if isinstance(Items, str):
        return str().join(ItemList_New)


def FindKey(
    Dict: dict,
    TargetValue
):
    for Key, Value in Dict.items():
        if Value == TargetValue:
            return Key

#############################################################################################################

def NormPath(
    String: Union[str, Path],
    PathType: Optional[str] = None,
    TrailingSlash: Optional[bool] = None
):
    '''
    '''
    try:
        if str(String).strip() == '':
            raise
        PathString = Path(String)#.resolve()

    except:
        return None

    else: #if re.search(r':[/\\\\]', str(String)) or re.search(r'\./', str(String)):
        if TrailingSlash is None:
            TrailingSlash = True if str(String).endswith(('/', '\\')) else False
        if platform.system() == 'Windows' or PathType == 'Win32':
            String = PathString.as_posix().replace(r'/', '\\')
            String += '\\' if TrailingSlash else ''
        if platform.system() == 'Linux' or PathType == 'Posix':
            String = PathString.as_posix()
            String += '/' if TrailingSlash else ''
        return String

#############################################################################################################

def RawString(
    Text: str
):
    '''
    Return as raw string representation of text
    '''
    RawMap = {
        7: r'\a',
        8: r'\b',
        9: r'\t',
        10: r'\n',
        11: r'\v',
        12: r'\f',
        13: r'\r'
    }
    Text = r''.join([RawMap.get(ord(Char), Char) for Char in Text])
    '''
    StringRepresentation = repr(Text)[1:-1] #StringRepresentation = Text.encode('unicode_escape').decode()
    return re.sub(r'\\+', lambda arg: r'\\', StringRepresentation).replace(r'\\', '\\').replace(r'\'', '\'') #return eval("'%s'" % canonical_string)
    '''
    return unicodedata.normalize('NFKC', Text)


class SubprocessManager:
    '''
    '''
    def __init__(self,
        CommunicateThroughConsole: bool = False
    ):
        self.CommunicateThroughConsole = CommunicateThroughConsole

        self.Encoding = 'gbk' if platform.system() == 'Windows' else 'utf-8'

    def create(self,
        Args: Union[list[Union[list, str]], str],
    ):
        if not self.CommunicateThroughConsole:
            for Arg in ToIterable(Args):
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
            for Arg in ToIterable(Args):
                Arg = shlex.join(Arg) if isinstance(Arg, list) else Arg
                TotalInput += f'{RawString(Arg)}\n'
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
        ShowProgress: bool = False,
        DecodeResult: Optional[bool] = None,
        LogPath: Optional[str] = None
    ):
        if not self.CommunicateThroughConsole:
            TotalOutput, TotalError = (bytes(), bytes())
            if ShowProgress:
                Output, Error = (bytes(), bytes())
                for Line in io.TextIOWrapper(self.Subprocess.stdout, encoding = self.Encoding, errors = 'replace'):
                    Output += Line.encode(self.Encoding, errors = 'replace')
                    sys.stdout.write(Line) if sys.stdout is not None else None
                    if LogPath is not None:
                        with open(LogPath, mode = 'a', encoding = 'utf-8') as Log:
                            Log.write(Line)
                    self.Subprocess.stdout.flush()
                    if self.Subprocess.poll() is not None:
                        break
                for Line in io.TextIOWrapper(self.Subprocess.stderr, encoding = self.Encoding, errors = 'replace'):
                    Error += Line.encode(self.Encoding, errors = 'replace')
                    sys.stderr.write(Line) if sys.stderr is not None else None
                    if LogPath is not None:
                        with open(LogPath, mode = 'a', encoding = 'utf-8') as Log:
                            Log.write(Line)
            else:
                Output, Error = self.Subprocess.communicate()
                Output, Error = b'' if Output is None else Output, b'' if Error is None else Error
            TotalOutput, TotalError = TotalOutput + Output, TotalError + Error

        else:
            if ShowProgress:
                TotalOutput, TotalError = (bytes(), bytes())
                self.Subprocess.stdin.write(self.TotalInput)
                self.Subprocess.stdin.close()
                for Line in io.TextIOWrapper(self.Subprocess.stdout, encoding = self.Encoding, errors = 'replace'):
                    TotalOutput += Line.encode(self.Encoding, errors = 'replace')
                    sys.stdout.write(Line) if sys.stdout is not None else None
                    if LogPath is not None:
                        with open(LogPath, mode = 'a', encoding = 'utf-8') as Log:
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
        TotalOutput, TotalError = TotalOutput.decode(self.Encoding, errors = 'ignore') if DecodeResult else TotalOutput, TotalError.decode(self.Encoding, errors = 'ignore') if DecodeResult else TotalError

        return None if TotalOutput in ('', b'') else TotalOutput, None if TotalError in ('', b'') else TotalError, self.Subprocess.returncode


def RunCMD(
    Args: Union[list[Union[list, str]], str],
    ShowProgress: bool = False,
    CommunicateThroughConsole: bool = False,
    DecodeResult: Optional[bool] = None,
    LogPath: Optional[str] = None
):
    '''
    '''
    ManageSubprocess = SubprocessManager(CommunicateThroughConsole)
    ManageSubprocess.create(Args)
    return ManageSubprocess.monitor(ShowProgress, DecodeResult, LogPath)


def SetEnvVar(
    Variable: str,
    Value: str,
    Type: str = 'Temp',
    AffectOS: bool = True
):
    '''
    '''
    Value = RawString(Value)

    if Type == 'Sys':
        if platform.system() == 'Windows':
            RunCMD(
                # Args = [
                #     f'set VAR={Value}{os.pathsep}%{Variable}%',
                #     f'reg add "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" /v "{Variable}" /t REG_EXPAND_SZ /d "%VAR%" /f',
                # ],
                Args = [
                    f'for /f "usebackq tokens=2,*" %A in (`reg query "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" /v "{Variable}"`) do set sysVAR=%B',
                    f'setx "{Variable}" "{Value}{os.pathsep}%sysVAR%" /m'
                ],
                CommunicateThroughConsole = True
            )
        if platform.system() == 'Linux':
            '''
            RunCMD(
                f'echo {Variable}={Value} >> /etc/environment',
                CommunicateThroughConsole = True
            )
            '''
            with open('/etc/environment', 'a') as f:
                f.write(f'\n{Variable}="{Value}"\n')

    if Type == 'User':
        if platform.system() == 'Windows':
            RunCMD(
                # Args = [
                #     f'set VAR={Value}{os.pathsep}%{Variable}%',
                #     f'reg add "HKEY_CURRENT_USER\\Environment" /v "{Variable}" /t REG_EXPAND_SZ /d "%VAR%" /f',
                # ],
                Args = [
                    f'for /f "usebackq tokens=2,*" %A in (`reg query "HKEY_CURRENT_USER\\Environment" /v "{Variable}"`) do set userVAR=%B',
                    f'setx "{Variable}" "{Value}{os.pathsep}%userVAR%"'
                ],
                CommunicateThroughConsole = True
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
                f.write(f'\nexport {Variable}="{Value}"\n')

    if Type == 'Temp' or AffectOS:
        EnvValue = os.environ.get(Variable)
        if EnvValue is not None and NormPath(Value, 'Posix') not in [NormPath(Value, 'Posix') for Value in EnvValue.split(os.pathsep)]:
            EnvValue = f'{Value}{os.pathsep}{EnvValue}' #EnvValue = f'{EnvValue}{os.pathsep}{Value}'
        else:
            EnvValue = Value
        os.environ[Variable] = EnvValue

#############################################################################################################

def isJson(content: str):
    try:
        json.loads(json.dumps(eval(content)))
        return True
    except:
        return False


def isUrl(content: str):
    if urlparse.urlparse(content).scheme in ['http', 'https']:
        return True
    else:
        return False


@polars.Config( 
    tbl_formatting = "ASCII_MARKDOWN",        
    tbl_hide_column_data_types = True,
    tbl_hide_dataframe_shape = True,
)
def toMarkdown(df: polars.DataFrame) -> str:
    return str(df)


def ToMarkdown(content: str):
    if isUrl(content):
        content = f"[URL]({content})"
    if isJson(content):
        content = toMarkdown(polars.DataFrame(json.loads(json.dumps(eval(content)))))
    return content


def ToHtml(Content, Align, Size, Weight, LetterSpacing, LineHeight):
    Style = f"'text-align:{Align}; font-size:{Size}pt; font-weight:{Weight}; letter-spacing: {LetterSpacing}px; line-height: {LineHeight}px'"
    Content = re.sub(
        pattern = "[\n]",
        repl = "<br>",
        string = Content
    ) if Content is not None else None
    return f"<p style={Style}>{Content}</p>" if Content is not None else ''


def SetRichText(
    Title: Optional[str] = None,
    TitleAlign: str = "left",
    TitleSize: float = 12.3,
    TitleWeight: float = 630.,
    TitleSpacing: float = 0.9,
    TitleLineHeight: float = 24.6,
    Body: Optional[str] = None,
    BodyAlign: str = "left",
    BodySize: float = 9.3,
    BodyWeight: float = 420.,
    BodySpacing: float = 0.6,
    BodyLineHeight: float = 22.2,
):
    '''
    Function to set text for widget
    '''

    RichText = (
        "<html>"
            "<head>"
                f"<title>{ToHtml(Title, TitleAlign, TitleSize, TitleWeight, TitleSpacing, TitleLineHeight)}</title>" # Not working with QWidgets
            "</head>"
            "<body>"
                f"{ToHtml(Title, TitleAlign, TitleSize, TitleWeight, TitleSpacing, TitleLineHeight)}"
                f"{ToHtml(Body, BodyAlign, BodySize, BodyWeight, BodySpacing, BodyLineHeight)}"
            "</body>"
        "</html>"
    )

    return RichText

#############################################################################################################

def FindURL(
    String: str
):
    '''
    '''
    URLList = re.compile(r'http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+').findall(RawString(String))
    URL = URLList[0]

    return URL

#############################################################################################################

def GetClassFromMethod(Method):
    '''
    Modules = list(inspect.getmodule(Method).__dict__.values())
    Modules = [Module for Module in Modules if str(Module).startswith("<class '__main__.")]
    return Modules[-1]
    '''
    return inspect.getmodule(Method).__dict__[Method.__qualname__.split('.')[0]]

#############################################################################################################

def RunEvents(
    Events: Union[list, dict]
):
    '''
    '''
    if isinstance(Events, list):
        for Event in Events:
            Event() if Event is not None else None
    if isinstance(Events, dict):
        for Event, Param in Events.items():
            Event(*ToIterable(Param if Param is not None else ())) if Event is not None else None

#############################################################################################################

def TaskAccelerating(
    TargetList: list,
    ArgsList: list = [(), ],
    TypeList: list = ['MultiProcessing', ],
    Workers: Optional[int] = None,
    Asynchronous: bool = False,
    ShowMessages: bool = True
):
    '''
    Function to create pool for multiprocessing/multithreading to accelerate tasks
    '''
    #StartTime = int(time.time())

    ProcessPool = ProcessPoolExecutor(max_workers = Workers)
    ThreadPool = ThreadPoolExecutor(max_workers = Workers)

    for Index, Target in enumerate(TargetList):
        Args = ArgsList[Index]
        Type = TypeList[Index]

        if Type == None:
            pass

        elif Type == 'MultiProcessing':
            Process = ProcessPool.submit(Target, *Args)
            print(
                "Task start\n" if Asynchronous == True else f"Task start ({Index + 1}/{len(TargetList)})\n"
                f"Name : {Target.__name__}\n"
                f"PID  : {os.getpid()}\n"
                "Please wait...\n"
            ) if ShowMessages == True else print('')
            Process.result() if Asynchronous == False else None

        elif Type == 'MultiThreading':
            Thread = ThreadPool.submit(Target, *Args)
            print(
                "Task start\n" if Asynchronous == True else f"Task start ({Index + 1}/{len(TargetList)})\n"
                f"Name : {Target.__name__}\n"
                f"TID  : {currentThread().ident}\n"
                "Please wait...\n"
            ) if ShowMessages == True else print('')
            Thread.result() if Asynchronous == False else None

        else:
            raise Exception(f"{Type} not found! Use 'MultiProcessing' or 'MultiThreading' instead.")

        print(
            f"Task done ({Index + 1}/{len(TargetList)})\n"
        ) if ShowMessages == True and Asynchronous == False else print('')

    ProcessPool.shutdown(
        wait = True,
        cancel_futures = True
    )
    ThreadPool.shutdown(
        wait = True,
        cancel_futures = True
    )

    print(
        "All done\n"
        "全部完成\n"
    ) if ShowMessages == True else print('')

    #Endtime = int(time.time())


def ProcessTerminator(
    Program: Union[str, int],
    SelfIgnored: bool = True,
    SearchKeyword: bool = False
):
    '''
    '''
    if isinstance(Program, int):
        PID = Program
        try:
            Process = psutil.Process(PID)
        except psutil.NoSuchProcess:
            # Process already terminated
            return

        ProcessList =  Process.children(recursive = True) + [Process]
        for Process in ProcessList:
            try:
                if Process.pid == os.getpid() and SelfIgnored:
                    continue
                os.kill(Process.pid, signal.SIGTERM)
            except:
                pass

    if isinstance(Program, str):
        Name = Program
        ProgramPath = NormPath(Name) if NormPath(Name) is not None else Name
        for Process in psutil.process_iter():
            ProcessList =  Process.children(recursive = True) + [Process]
            try:
                for Process in ProcessList:
                    if Process.pid == os.getpid() and SelfIgnored:
                        continue
                    ProcessPath = Process.exe()
                    if ProgramPath == ProcessPath or (ProgramPath.lower() in ProcessPath.lower() and SearchKeyword):
                        Process.send_signal(signal.SIGTERM) #Process.kill()
            except:
                pass


def OccupationTerminator(
    File: str,
    SearchKeyword: bool = False
):
    '''
    '''
    FilePath = NormPath(File) if NormPath(File) is not None else File
    for Process in psutil.process_iter():
        try:
            PopenFiles = Process.open_files()
            for PopenFile in PopenFiles:
                PopenFilePath = PopenFile.path
                if FilePath == PopenFilePath or (FilePath.lower() in PopenFilePath.lower() and SearchKeyword):
                    Process.send_signal(signal.SIGTERM) #Process.kill()
        except:
            pass

#############################################################################################################

def RenameIfExists(PathStr: str):
    ParentDirectory, Name = os.path.split(PathStr)
    suffix = Path(Name).suffix
    if len(suffix) > 0:
        while Path(PathStr).exists():
            pattern = r'(\d+)\)\.'
            if re.search(pattern, Name) is None:
                Name = Name.replace('.', '(0).')
            else:
                CurrentNumber = int(re.findall(pattern, Name)[-1])
                Name = Name.replace(f'({CurrentNumber}).', f'({CurrentNumber + 1}).')
            PathStr = Path(ParentDirectory).joinpath(Name).as_posix()
    else:
        while Path(PathStr).exists():
            pattern = r'(\d+)\)'
            match = re.search(pattern, Name)
            if match is None:
                Name += '(0)'
            else:
                CurrentNumber = int(match.group(1))
                Name = Name[:match.start(1)] + f'({CurrentNumber + 1})'
            PathStr = Path(ParentDirectory).joinpath(Name).as_posix()
    return PathStr


def CleanDirectory(
    Directory: str,
    WhiteList: list
):
    '''
    '''
    if os.path.exists(Directory):
        for DirPath, Folders, Files in os.walk(Directory, topdown = False):
            for File in Files:
                FilePath = os.path.join(DirPath, File)
                try:
                    if not any(File in FilePath for File in WhiteList):
                        os.remove(FilePath)
                except:
                    pass
            for Folder in Folders:
                FolderPath = os.path.join(DirPath, Folder)
                try:
                    if not any(Folder in FolderPath for Folder in WhiteList):
                        shutil.rmtree(FolderPath)
                except:
                    pass


def MoveFiles(
    Dir: str,
    Dst: str
):
    '''
    '''
    for DirPath, FolderNames, FileNames in os.walk(Dir):
        for FolderName in FolderNames:
            if Dir != Dst:
                shutil.move(os.path.join(DirPath, FolderName), Dst)
        for FileName in FileNames:
            if Dir != Dst:
                shutil.move(os.path.join(DirPath, FileName), Dst)


def GetPaths(
    Dir: str,
    Name: str,
    SearchKeyword: bool = True
):
    '''
    '''
    Result = []

    for DirPath, FolderNames, FileNames in os.walk(Dir):
        for FolderName in FolderNames:
            if Name == FolderName or (Name in FolderName and SearchKeyword is True):
                Result.append(os.path.join(DirPath, FolderName))
            else:
                pass
        for FileName in FileNames:
            if Name == FileName or (Name in FileName and SearchKeyword is True):
                Result.append(os.path.join(DirPath, FileName))
            else:
                pass

    return Result if len(Result) > 0 else None

#############################################################################################################

def GetBaseDir(
    FilePath: Optional[str] = None,
    ParentLevel: Optional[int] = None,
    SearchMEIPASS: bool = False
):
    '''
    Get the parent directory of file, or get the MEIPASS if file is compiled with pyinstaller
    '''
    if FilePath is not None:
        BaseDir = NormPath(Path(str(FilePath)).absolute().parents[ParentLevel if ParentLevel is not None else 0])
    elif SearchMEIPASS and getattr(sys, 'frozen', None):
        BaseDir = NormPath(sys._MEIPASS)
    else:
        BaseDir = None

    return BaseDir


def GetFileInfo(
    File: Optional[str] = None
):
    '''
    Check whether python file is compiled
    '''
    if File is None:
        FileName = Path(sys.argv[0]).name
        if getattr(sys, 'frozen', None):
            IsFileCompiled = True
        else:
            IsFileCompiled = False if FileName.endswith('.py') or sys.executable.endswith('python.exe') else True
    else:
        FileName = Path(NormPath(File)).name
        IsFileCompiled = False if FileName.endswith('.py') else True

    return FileName, IsFileCompiled


#############################################################################################################

def IsVersionSatisfied(CurrentVersion, VersionReqs):
    if VersionReqs is None:
        return True
    VersionReqs = VersionReqs.split(',') if isinstance(VersionReqs, str) else list(VersionReqs)
    Results = []
    for VersionReq in VersionReqs:
        SplitVersionReq = re.split('=|>|<', VersionReq)
        RequiredVersion = SplitVersionReq[-1]
        Req = VersionReq[:len(VersionReq) - len(RequiredVersion)]
        if Req == "==":
            Results.append(version.parse(CurrentVersion) == version.parse(RequiredVersion))
        if Req == ">=":
            Results.append(version.parse(CurrentVersion) >= version.parse(RequiredVersion))
        if Req == "<=":
            Results.append(version.parse(CurrentVersion) <= version.parse(RequiredVersion))
        return True if False not in Results else False


def IsSystemSatisfied(SystemReqs):
    if SystemReqs is None:
        return True
    SystemReqs = SystemReqs.split(';') if isinstance(SystemReqs, str) else list(SystemReqs)
    Results = []
    for SystemReq in SystemReqs:
        SplitSystemReq = re.split('=|>|<', SystemReq)
        RequiredSystem = SplitSystemReq[-1].strip()
        Req = SystemReq[len(SplitSystemReq[0]) : len(SystemReq) - len(RequiredSystem)].strip()
        if Req == "==":
            Results.append(sys.platform == eval(RequiredSystem))
        if Req == "!=":
            Results.append(sys.platform != eval(RequiredSystem))
        return True if False not in Results else False

#############################################################################################################

def RunScript(
    CommandList: list[str],
    ScriptPath: Optional[str]
):
    '''
    '''
    if platform.system() == 'Linux':
        ScriptPath = Path.cwd().joinpath('Bash.sh') if ScriptPath is None else NormPath(ScriptPath)
        with open(ScriptPath, 'w') as BashFile:
            Commands = "\n".join(CommandList)
            BashFile.write(Commands)
        os.chmod(ScriptPath, 0o755) # 给予可执行权限
        subprocess.Popen(['bash', ScriptPath])
    if platform.system() == 'Windows':
        ScriptPath = Path.cwd().joinpath('Bat.bat') if ScriptPath is None else NormPath(ScriptPath)
        with open(ScriptPath, 'w') as BatFile:
            Commands = "\n".join(CommandList)
            BatFile.write(Commands)
        subprocess.Popen([ScriptPath], creationflags = subprocess.CREATE_NEW_CONSOLE)


def BootWithScript(
    ProgramPath: str = ...,
    DelayTime: int = 3,
    ScriptPath: Optional[str] = None
):
    '''
    '''
    if platform.system() == 'Linux':
        _, IsFileCompiled = GetFileInfo(ProgramPath)
        RunScript(
            CommandList = [
                '#!/bin/bash',
                f'sleep {DelayTime}',
                f'./"{ProgramPath}"' if IsFileCompiled else f'python3 "{ProgramPath}"',
                'rm -- "$0"'
            ],
            ScriptPath = ScriptPath
        )
    if platform.system() == 'Windows':
        _, IsFileCompiled = GetFileInfo(ProgramPath)
        RunScript(
            CommandList = [
                '@echo off',
                f'ping 127.0.0.1 -n {DelayTime + 1} > nul',
                f'start "Programm Running" "{ProgramPath}"' if IsFileCompiled else f'python "{ProgramPath}"',
                'del "%~f0"'
            ],
            ScriptPath = ScriptPath
        )

##############################################################################################################################

def CheckUpdateFromGithub(
    AccessToken: Optional[str] = None,
    RepoOwner: str = ...,
    RepoName: str = ...,
    FileName: str = ...,
    FileFormat: str = ...,
    Version_Current: str = ...
):
    '''
    '''
    try:
        PersonalGit = Github(AccessToken)
        Repo = PersonalGit.get_repo(f"{RepoOwner}/{RepoName}")
        Version_Latest = Repo.get_tags()[0].name
        LatestRelease = Repo.get_latest_release() #LatestRelease = Repo.get_release(Version_Latest)
        for Index, Asset in enumerate(LatestRelease.assets):
            if Asset.name == f"{FileName}.{FileFormat}":
                IsUpdateNeeded = True if version.parse(Version_Current) < version.parse(Version_Latest) else False
                DownloadURL = Asset.browser_download_url #DownloadURL = f"https://github.com/{RepoOwner}/{RepoName}/releases/download/{Version_Latest}/{FileName}.{FileFormat}"
                return IsUpdateNeeded, DownloadURL
            elif Index + 1 == len(LatestRelease.assets):
                raise Exception(f"No file found with name {FileName}.{FileFormat} in the latest release")

    except Exception as e:
        print(f"Error occurred while checking for updates: \n{e}")


def DownloadFile(
    DownloadURL: str,
    DownloadDir: str,
    FileName: str,
    FileFormat: str,
    SHA_Expected: Optional[str],
    CreateNewConsole: bool = False
) -> Tuple[Union[bytes, str], str]:
    '''
    '''
    os.makedirs(DownloadDir, exist_ok = True)

    DownloadName = FileName + (FileFormat if '.' in FileFormat else f'.{FileFormat}')
    DownloadPath = NormPath(Path(DownloadDir).joinpath(DownloadName).absolute())

    def Download():
        try:
            RunCMD(
                Args = [
                    'aria2c',
                    f'''
                    {('cmd.exe /c start ' if platform.system() == 'Windows' else 'x-terminal-emulator -e ') if CreateNewConsole else ''}
                    aria2c "{DownloadURL}" --dir="{Path(DownloadPath).parent.as_posix()}" --out="{Path(DownloadPath).name}" -x6 -s6 --file-allocation=none --force-save=false
                    '''
                ]
            )
        except:
            with urllib.request.urlopen(DownloadURL) as source, open(DownloadPath, "wb") as output:
                with tqdm(total = int(source.info().get("Content-Length")), ncols = 80, unit = 'iB', unit_scale = True, unit_divisor = 1024) as loop:
                    while True:
                        buffer = source.read(8192)
                        if not buffer:
                            break
                        output.write(buffer)
                        loop.update(len(buffer))
        finally:
            return open(DownloadPath, "rb").read() if Path(DownloadPath).exists() else None

    if os.path.exists(DownloadPath):
        if os.path.isfile(DownloadPath) == False:
            raise RuntimeError(f"{DownloadPath} exists and is not a regular file")
        elif SHA_Expected is not None:
            with open(DownloadPath, "rb") as f:
                FileBytes = f.read()
            if len(SHA_Expected) == 40:
                SHA_Current = hashlib.sha1(FileBytes).hexdigest()
            if len(SHA_Expected) == 64:
                SHA_Current = hashlib.sha256(FileBytes).hexdigest()
            FileBytes = Download() if SHA_Current != SHA_Expected else FileBytes #Download() if SHA_Current != SHA_Expected else None
        else:
            os.remove(DownloadPath)
            FileBytes = Download()
    else:
        FileBytes = Download()

    if FileBytes is None:
        raise Exception('Download Failed!')

    return FileBytes, DownloadPath

#############################################################################################################

class ManageConfig:
    '''
    Manage config
    '''
    def __init__(self,
        Config_Path: Optional[str] = None
    ):
        self.Config_Path = NormPath(Path(os.getenv('SystemDrive')).joinpath('Config.ini')) if Config_Path == None else Config_Path
        os.makedirs(Path(self.Config_Path).parent, exist_ok = True)

        self.ConfigParser = configparser.ConfigParser()
        try:
            self.ConfigParser.read(self.Config_Path, encoding = 'utf-8')
        except:
            with open(self.Config_Path, 'w'):
                pass
            self.ConfigParser.clear()

    def Parser(self):
        return self.ConfigParser

    def EditConfig(self,
        Section: str = ...,
        Option: str = ...,
        Value: str = ...,
        ConfigParser: Optional[configparser.ConfigParser] = None
    ):
        ConfigParser = self.Parser() if ConfigParser == None else ConfigParser
        try:
            ConfigParser.add_section(Section)
        except:
            pass
        ConfigParser.set(Section, Option, Value)
        with open(self.Config_Path, 'w', encoding = 'utf-8') as Config:
            ConfigParser.write(Config)

    def GetValue(self,
        Section: str = ...,
        Option: str = ...,
        InitValue: Optional[str] = None,
        ConfigParser: Optional[configparser.ConfigParser] = None
    ):
        ConfigParser = self.Parser() if ConfigParser == None else ConfigParser
        try:
            Value = ConfigParser.get(Section, Option)
        except:
            if InitValue != None:
                self.EditConfig(Section, Option, InitValue, ConfigParser)
                return InitValue
            else:
                raise Exception("Need initial value")
        return Value

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