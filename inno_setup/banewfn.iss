#ifndef StageDir
#define StageDir "..\release\windows-release\stage"
#endif

#ifndef OutputDir
#define OutputDir "."
#endif

#define MyAppName "Banewfn"
#define MyAppVersion "1.3.0"
#define MyAppPublisher "Bane Dysta"
#define MyAppURL "https://github.com/bane-dysta/banewfn"
#define MyAppExeName "banewfn.exe"

; ===== 文件关联（拆开两个 ProgID，显示名不同；打开命令/图标相同）=====
#define MyAppAssocExtBW  ".bw"
#define MyAppAssocExtBWC ".bwc"

#define MyProgIdBW  "BanewfnScript"
#define MyProgIdBWC "BanewfnScriptWithConf"

#define MyTypeNameBW  "Banewfn Script"
#define MyTypeNameBWC "Banewfn Script with conf"

[Setup]
AppId={{29C7DBFA-7162-4134-8A2C-9A0970BB9925}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName=D:/program\{#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
ChangesAssociations=yes
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename=banewfn_setup
SetupIconFile=..\src\resource\mothwfn.ico
SolidCompression=yes
WizardStyle=classic

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesesimplified"; MessagesFile: ".\ChineseSimplified.isl"

; ===== 让用户分别选择是否关联 .bw 和 .bwc =====
[Tasks]
Name: "assoc_bw";  Description: "Associate .bw (Banewfn Script) with Banewfn";             GroupDescription: "File associations"; Flags: checkedonce
Name: "assoc_bwc"; Description: "Associate .bwc (Banewfn Script with conf) with Banewfn";  GroupDescription: "File associations"; Flags: checkedonce

[Files]
Source: "{#StageDir}\banewfn.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StageDir}\bwpack.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#StageDir}\conf\*"; DestDir: "{app}\conf"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StageDir}\scripts\*"; DestDir: "{app}\scripts"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StageDir}\docs\out\BaneWfn_Project_Manual_zh.pdf"; DestDir: "{app}"; Flags: ignoreversion

; ===== 强关联注册表（分别写两个 ProgID）=====
[Registry]
; --------------------
; .bw -> BanewfnScript
; --------------------
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocExtBW}"; ValueType: string; ValueName: ""; ValueData: "{#MyProgIdBW}"; Flags: uninsdeletevalue; Tasks: assoc_bw
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocExtBW}\OpenWithProgids"; ValueType: string; ValueName: "{#MyProgIdBW}"; ValueData: ""; Flags: uninsdeletevalue; Tasks: assoc_bw

Root: HKA; Subkey: "Software\Classes\{#MyProgIdBW}"; ValueType: string; ValueName: ""; ValueData: "{#MyTypeNameBW}"; Flags: uninsdeletekey; Tasks: assoc_bw
Root: HKA; Subkey: "Software\Classes\{#MyProgIdBW}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Tasks: assoc_bw
Root: HKA; Subkey: "Software\Classes\{#MyProgIdBW}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: assoc_bw

; --------------------------
; .bwc -> BanewfnScriptWithConf
; --------------------------
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocExtBWC}"; ValueType: string; ValueName: ""; ValueData: "{#MyProgIdBWC}"; Flags: uninsdeletevalue; Tasks: assoc_bwc
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocExtBWC}\OpenWithProgids"; ValueType: string; ValueName: "{#MyProgIdBWC}"; ValueData: ""; Flags: uninsdeletevalue; Tasks: assoc_bwc

Root: HKA; Subkey: "Software\Classes\{#MyProgIdBWC}"; ValueType: string; ValueName: ""; ValueData: "{#MyTypeNameBWC}"; Flags: uninsdeletekey; Tasks: assoc_bwc
Root: HKA; Subkey: "Software\Classes\{#MyProgIdBWC}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Tasks: assoc_bwc
Root: HKA; Subkey: "Software\Classes\{#MyProgIdBWC}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: assoc_bwc

; （可选但推荐）告诉系统 banewfn.exe 支持这两种后缀，让“打开方式”更稳定出现
Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: "{#MyAppAssocExtBW}";  ValueData: ""; Flags: uninsdeletevalue; Tasks: assoc_bw
Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: "{#MyAppAssocExtBWC}"; ValueData: ""; Flags: uninsdeletevalue; Tasks: assoc_bwc

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"

[Code]
var
  FilePage: TInputFileWizardPage;
  CoresPage: TInputQueryWizardPage;
  MultiwfnPath, BashPath, ConfPath, CoresValue: string;

// ---- 初始化自定义页面 ----
procedure InitializeWizard();
begin
  MultiwfnPath := '';
  BashPath     := '';
  ConfPath     := '';
  CoresValue   := '';

  // 1) 文件选择页
  FilePage := CreateInputFilePage(
    wpSelectDir,
    'Select Multiwfn and Git-Bash Executables',
    'Please select the Multiwfn and Bash executable files.',
    'If you installed Git Bash normally, bash.exe is likely here.'
  );

  FilePage.Add(
    'Multiwfn Executable (Multiwfn.exe):',
    'Executable files (*.exe)|*.exe|All files (*.*)|*.*',
    '.exe'
  );
  FilePage.Add(
    'Git-Bash Executable (bash.exe):',
    'Executable files (*.exe)|*.exe|All files (*.*)|*.*',
    '.exe'
  );

  // 默认值：环境变量 Multiwfnpath 或简单 "Multiwfn.exe"
  FilePage.Values[0] := GetEnv('Multiwfnpath') + '\Multiwfn.exe';
  if FilePage.Values[0] = '\Multiwfn.exe' then
    FilePage.Values[0] := 'Multiwfn.exe';
  // Bash 默认固定路径
  FilePage.Values[1] := 'C:\Program Files\Git\bin\bash.exe';

  // 2) Cores 输入页
  CoresPage := CreateInputQueryPage(
    FilePage.ID,
    'Multiwfn Default Processor Cores',
    'Please enter the number of processor cores to use defaultly in Multiwfn calculation.',
    'Must be a positive integer.'
  );
  CoresPage.Add('Cores:', False);
  CoresPage.Values[0] := '4';
end;

// ---- 收集所有用户输入 ----
function NextButtonClick(CurPageID: Integer): Boolean;
var
  tmpInt: Integer;
begin
  Result := True;

  if CurPageID = FilePage.ID then
  begin
    MultiwfnPath := Trim(FilePage.Values[0]);
    BashPath     := Trim(FilePage.Values[1]);
    ConfPath     := ExpandConstant('{app}\conf');
  end;

  if CurPageID = CoresPage.ID then
  begin
    tmpInt := StrToIntDef(Trim(CoresPage.Values[0]), 0);
    if tmpInt <= 0 then
    begin
      MsgBox('Please enter a valid positive integer for cores.', mbError, MB_OK);
      Result := False;
      Exit;
    end;
    CoresValue := IntToStr(tmpInt);
  end;
end;

// ---- 展示收集到的信息到“Ready to Install”页面 ----
function UpdateReadyMemo(
  Space, NewLine,
  MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo,
  MemoComponentsInfo, MemoGroupInfo,
  MemoTasksInfo: String
): String;
begin
  Result := '';

  // 先显示默认的目录信息（若有）
  if MemoDirInfo <> '' then
    Result := Result + MemoDirInfo + NewLine + NewLine;

  // 显示收集到的字段
  Result := Result +
    Space + 'Multiwfn Path: ' + MultiwfnPath + NewLine +
    Space + 'Bash Path:   ' + BashPath   + NewLine +
    Space + 'Conf Path:   ' + ConfPath   + NewLine +
    Space + 'Cores:       ' + CoresValue + NewLine;
end;

// ---- 安装结束写配置到 banewfn.rc ----
procedure DeinitializeSetup();
var
  RCFile: string;
begin
  RCFile := ExpandConstant('{app}\banewfn.rc');
  CreateDir(ExtractFileDir(RCFile));

  SaveStringToFile(RCFile, 'Multiwfn_exec=' + MultiwfnPath + #13#10, False);
  SaveStringToFile(RCFile, 'gitbash_exec='     + BashPath     + #13#10, True);
  SaveStringToFile(RCFile, 'confpath='     + ConfPath     + #13#10, True);
  SaveStringToFile(RCFile, 'cores='        + CoresValue   + #13#10, True);
end;
