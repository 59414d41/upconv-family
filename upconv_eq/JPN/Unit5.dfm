object Form5: TForm5
  Left = 0
  Top = 0
  Caption = 'WAV '#24773#22577
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
  object tvTree: TTreeView
    Left = 0
    Top = 0
    Width = 201
    Height = 372
    Align = alLeft
    Indent = 19
    TabOrder = 0
    OnClick = tvTreeClick
  end
  object tvDesc: TValueListEditor
    Left = 199
    Top = 0
    Width = 306
    Height = 372
    Align = alRight
    Anchors = [akLeft, akTop, akRight, akBottom]
    Strings.Strings = (
      'Offset=-'
      'Size=-')
    TabOrder = 1
  end
end
