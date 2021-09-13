unit MainWnd;

{$mode objfpc}{$H+}

interface

uses
  Classes,
  SysUtils,
  FileUtil,
  Forms,
  Controls,
  Graphics,
  Dialogs, ExtCtrls, ActnList, Menus, StdCtrls, ComCtrls, Buttons;

type
  { TStatusConversion }
  TStatusConversion = (
    scIdle,
    scProgress
  );
  { TMainWndForm }
  TMainWndForm = class(TForm)
    ActionRun: TAction;
    ActionRemove: TAction;
    ActionAddFolder: TAction;
    ActionAdds: TAction;
    ActionDestFolderChange: TAction;
    ActionQuit: TAction;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    RunBitBtn: TBitBtn;
    Button1: TButton;
    DestFolderEdit: TEdit;
    AddsMenuItem: TMenuItem;
    AddFolderMenuItem: TMenuItem;
    MenuItem3: TMenuItem;
    DestFolderPanel: TPanel;
    AddsOpenDialog: TOpenDialog;
    QuitMenuItem: TMenuItem;
    DestFolderSelectDirectoryDialog: TSelectDirectoryDialog;
    AddFolderSelectDirectoryDialog: TSelectDirectoryDialog;
    SepMenuItem1: TMenuItem;
    QuitBitBtn: TBitBtn;
    BitBtn5: TBitBtn;
    BitBtn6: TBitBtn;
    MainMenuActionList: TActionList;
    FilesLabel: TLabel;
    ProgressLabel: TLabel;
    FilesListView: TListView;
    MainMenu: TMainMenu;
    FileMenuItem: TMenuItem;
    OptionMenuItem: TMenuItem;
    AboutMenuItem: TMenuItem;
    ConvProgressBar: TProgressBar;
    ProgressPanel: TPanel;
    Shape1: TShape;
    Shape2: TShape;
    Shape3: TShape;
    Shape4: TShape;
    ToolPanel: TPanel;
    UserControlPanel: TPanel;
    ConvertorPanel: TPanel;
    PopupMenu1: TPopupMenu;
    MainStatusBar: TStatusBar;
    procedure ActionAddFolderExecute(Sender: TObject);
    procedure ActionAddsExecute(Sender: TObject);
    procedure ActionDestFolderChangeExecute(Sender: TObject);
    procedure ActionQuitExecute(Sender: TObject);
    procedure ActionRemoveExecute(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    fStatus: TStatusConversion;
  private
    procedure SearchFolder(const APath: String);
  public
    procedure SetStatus(const AStatus: TStatusConversion);
  end;

var
  MainWndForm: TMainWndForm;

implementation

{$R *.lfm}

{ TMainWndForm }

procedure TMainWndForm.ActionQuitExecute(Sender: TObject);
begin
  Close;
end;

procedure TMainWndForm.ActionRemoveExecute(Sender: TObject);
var I: Integer;
begin
  case FilesListView.SelCount of
    0: begin
      Exit;
    end;
    1: begin
      FilesListView.Items.Delete(FilesListView.Selected.Index);
    end;
    else begin
      for I := FilesListView.Items.Count - 1 downto 0 do
      begin
        with FilesListView.Items[I] do
        begin
          if Selected then
            FilesListView.Items.Delete(Index);
        end;
      end;
    end;
  end;
end;

procedure TMainWndForm.FormCreate(Sender: TObject);
begin
  SetStatus(scIdle);
end;

procedure TMainWndForm.SearchFolder(const APath: String);
var Src: SysUtils.TSearchRec;
begin
  if SysUtils.FindFirst(APath + '\*.*', faAnyFile, Src) = 0 then
  begin
    repeat
      if ((Src.Attr and faDirectory) = faDirectory) and
       (Src.Name <> '..') and (Src.Name <> '.')then
        SearchFolder(APath + '\' + Src.Name)
      else if (LowerCase(ExtractFileExt(Src.Name)) = '.rc') then
      begin
        with FilesListView.Items.Add do
        begin
          Checked := True;
          Caption := Src.Name;
          SubItems.Add(APath + '\' + Src.Name);
        end;
      end;
    until FindNext(Src) <> 0;
  end
  else
      SysUtils.RaiseLastOSError;
end;

procedure TMainWndForm.SetStatus(const AStatus: TStatusConversion);
begin
  fStatus := AStatus;
  case AStatus of
    scIdle: begin
      MainStatusBar.Panels[0].Text := 'Idle';
    end;
    scProgress: begin
      MainStatusBar.Panels[0].Text := 'Conversion...';
    end;
  end;
end;

procedure TMainWndForm.ActionDestFolderChangeExecute(Sender: TObject);
begin
  DestFolderSelectDirectoryDialog.FileName := '';
  DestFolderSelectDirectoryDialog.InitialDir := ExtractFilePath(Application.ExeName);
  if DestFolderSelectDirectoryDialog.Execute then
  begin
    if not DirectoryExists(DestFolderSelectDirectoryDialog.FileName) then
    begin
       if not ForceDirectories(DestFolderSelectDirectoryDialog.FileName) then
       begin
          SysUtils.RaiseLastOSError;
          Exit;
       end;
    end;
    DestFolderEdit.Text := DestFolderSelectDirectoryDialog.FileName;
  end;
end;

procedure TMainWndForm.ActionAddsExecute(Sender: TObject);
var I: Integer;
    S: String;
begin
  AddsOpenDialog.FileName := '';
  if AddsOpenDialog.Execute then
  begin
    for I := 0 to AddsOpenDialog.Files.Count - 1 do
    begin
      S := AddsOpenDialog.Files[I];
      if FileExists(S) and
          (LowerCase(ExtractFileExt(S)) = '.rc') then
      begin
        with FilesListView.Items.Add do
        begin
          Checked := True;
          Caption := ExtractFileName(S);
          SubItems.Add(S);
        end;
      end;
    end;
  end;
end;

procedure TMainWndForm.ActionAddFolderExecute(Sender: TObject);
begin
  AddFolderSelectDirectoryDialog.FileName := '';
  AddFolderSelectDirectoryDialog.InitialDir := ExtractFilePath(Application.ExeName);
  if AddFolderSelectDirectoryDialog.Execute then
     SearchFolder(AddFolderSelectDirectoryDialog.FileName);
end;

end.

