object Form5: TForm5
  Left = 0
  Top = 0
  Caption = 'WAV Information'
  ClientHeight = 372
  ClientWidth = 505
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ScrollBox1: TScrollBox
    Left = 152
    Top = 0
    Width = 353
    Height = 372
    HorzScrollBar.Visible = False
    VertScrollBar.Style = ssFlat
    Align = alRight
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 0
    object tvDesc: TValueListEditor
      Left = 0
      Top = 0
      Width = 349
      Height = 368
      Align = alClient
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Strings.Strings = (
        'Offset=-'
        'Size=-')
      TabOrder = 0
      ColWidths = (
        152
        191)
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 153
    Height = 372
    Align = alLeft
    BevelOuter = bvNone
    TabOrder = 1
    object Image1: TImage
      Left = 0
      Top = 222
      Width = 153
      Height = 150
      Align = alBottom
      Stretch = True
      ExplicitWidth = 150
    end
    object tvTree: TTreeView
      Left = 0
      Top = 0
      Width = 153
      Height = 225
      Align = alTop
      Indent = 19
      TabOrder = 0
      OnClick = tvTreeClick
      ExplicitWidth = 201
    end
  end
end
