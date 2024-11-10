object Form1: TForm1
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Upconv Frontend 1.0.x'
  ClientHeight = 625
  ClientWidth = 547
  Color = clMenu
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel6: TBevel
    Left = 0
    Top = 563
    Width = 547
    Height = 62
    Align = alBottom
    Style = bsRaised
    ExplicitTop = 560
    ExplicitWidth = 543
  end
  object lbl_exec_status: TLabel
    Left = 12
    Top = 598
    Width = 93
    Height = 13
    AutoSize = False
    Color = clHighlightText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
  end
  object lbl_count: TLabel
    Left = 11
    Top = 572
    Width = 72
    Height = 13
    Caption = '[0000/0000]'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object lbl_message: TLabel
    Left = 360
    Top = 568
    Width = 73
    Height = 13
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clFuchsia
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lbl_co: TLabel
    Left = 112
    Top = 572
    Width = 73
    Height = 13
    AutoSize = False
  end
  object lbl_addp: TLabel
    Left = 113
    Top = 572
    Width = 3
    Height = 13
  end
  object pageMain: TPageControl
    Left = -2
    Top = 1
    Width = 1103
    Height = 561
    ActivePage = tabUpconv
    TabOrder = 0
    object tabUpconv: TTabSheet
      Caption = 'Upconv'
      object Label1: TLabel
        Left = 8
        Top = 8
        Width = 25
        Height = 13
        Caption = 'Files'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label2: TLabel
        Left = 8
        Top = 272
        Width = 45
        Height = 13
        Caption = 'Convert'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Bevel1: TBevel
        Left = 8
        Top = 264
        Width = 457
        Height = 9
        Shape = bsTopLine
      end
      object Bevel2: TBevel
        Left = 8
        Top = 432
        Width = 525
        Height = 9
        Shape = bsTopLine
      end
      object Label3: TLabel
        Left = 8
        Top = 440
        Width = 39
        Height = 13
        Caption = 'Output'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label22: TLabel
        Left = 20
        Top = 185
        Width = 56
        Height = 13
        Caption = 'Information'
      end
      object Label25: TLabel
        Left = 19
        Top = 210
        Width = 84
        Height = 13
        Caption = 'Output Size( MB )'
      end
      object btnOK_Clear: TBitBtn
        Left = 288
        Top = 232
        Width = 41
        Height = 25
        Caption = 'St Clr'
        Enabled = False
        TabOrder = 18
        OnClick = btnOK_ClearClick
      end
      object LvFile: TListView
        Left = 16
        Top = 24
        Width = 517
        Height = 153
        BevelInner = bvLowered
        Columns = <
          item
            Caption = 'Status'
            MaxWidth = 50
            MinWidth = 50
          end
          item
            Caption = 'Filename'
            MaxWidth = 900
            MinWidth = 180
            Width = 180
          end
          item
            Caption = 'Time'
            MaxWidth = 60
            MinWidth = 60
            Width = 60
          end
          item
            Caption = 'Information'
            MaxWidth = 170
            MinWidth = 170
            Width = 170
          end
          item
            Caption = 'Lv'
          end
          item
            Caption = 'HFC'
            Width = 70
          end
          item
            Caption = 'Offset'
            Width = 70
          end>
        ColumnClick = False
        FlatScrollBars = True
        MultiSelect = True
        ReadOnly = True
        RowSelect = True
        PopupMenu = PopupMenuList
        SmallImages = ImageList1
        TabOrder = 0
        ViewStyle = vsReport
        OnChange = LvFileChange
        OnColumnClick = LvFileColumnClick
        OnSelectItem = LvFileSelectItem
      end
      object btnAdd: TBitBtn
        Left = 24
        Top = 232
        Width = 73
        Height = 25
        Caption = 'Add'
        Default = True
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
          3333333333333333333333330000333333333333333333333333F33333333333
          00003333344333333333333333388F3333333333000033334224333333333333
          338338F3333333330000333422224333333333333833338F3333333300003342
          222224333333333383333338F3333333000034222A22224333333338F338F333
          8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
          33333338F83338F338F33333000033A33333A222433333338333338F338F3333
          0000333333333A222433333333333338F338F33300003333333333A222433333
          333333338F338F33000033333333333A222433333333333338F338F300003333
          33333333A222433333333333338F338F00003333333333333A22433333333333
          3338F38F000033333333333333A223333333333333338F830000333333333333
          333A333333333333333338330000333333333333333333333333333333333333
          0000}
        ModalResult = 1
        NumGlyphs = 2
        TabOrder = 1
        OnClick = btnAddClick
      end
      object btnDel: TBitBtn
        Left = 104
        Top = 232
        Width = 73
        Height = 25
        Cancel = True
        Caption = 'Del'
        Enabled = False
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
          333333333333333333333333000033338833333333333333333F333333333333
          0000333911833333983333333388F333333F3333000033391118333911833333
          38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
          911118111118333338F3338F833338F3000033333911111111833333338F3338
          3333F8330000333333911111183333333338F333333F83330000333333311111
          8333333333338F3333383333000033333339111183333333333338F333833333
          00003333339111118333333333333833338F3333000033333911181118333333
          33338333338F333300003333911183911183333333383338F338F33300003333
          9118333911183333338F33838F338F33000033333913333391113333338FF833
          38F338F300003333333333333919333333388333338FFF830000333333333333
          3333333333333333333888330000333333333333333333333333333333333333
          0000}
        ModalResult = 2
        NumGlyphs = 2
        TabOrder = 2
        OnClick = btnDelClick
      end
      object btnClear: TBitBtn
        Left = 184
        Top = 232
        Width = 73
        Height = 25
        Cancel = True
        Caption = 'Clear'
        Enabled = False
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
          333333333333333333333333000033338833333333333333333F333333333333
          0000333911833333983333333388F333333F3333000033391118333911833333
          38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
          911118111118333338F3338F833338F3000033333911111111833333338F3338
          3333F8330000333333911111183333333338F333333F83330000333333311111
          8333333333338F3333383333000033333339111183333333333338F333833333
          00003333339111118333333333333833338F3333000033333911181118333333
          33338333338F333300003333911183911183333333383338F338F33300003333
          9118333911183333338F33838F338F33000033333913333391113333338FF833
          38F338F300003333333333333919333333388333338FFF830000333333333333
          3333333333333333333888330000333333333333333333333333333333333333
          0000}
        ModalResult = 2
        NumGlyphs = 2
        TabOrder = 3
        OnClick = btnClearClick
      end
      object btnUp: TBitBtn
        Left = 336
        Top = 232
        Width = 25
        Height = 25
        Caption = #9650
        Enabled = False
        TabOrder = 4
        OnClick = btnUpClick
      end
      object btnDown: TBitBtn
        Left = 368
        Top = 232
        Width = 25
        Height = 25
        Caption = #9660
        Enabled = False
        TabOrder = 5
        OnClick = btnDownClick
      end
      object btnInfo: TBitBtn
        Left = 400
        Top = 232
        Width = 25
        Height = 25
        Caption = #65281
        Enabled = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 6
        OnClick = btnInfoClick
      end
      object GroupBox1: TGroupBox
        Left = 16
        Top = 288
        Width = 81
        Height = 49
        Caption = 'Sampling rate'
        TabOrder = 7
        object cmbSampRate: TComboBox
          Left = 8
          Top = 16
          Width = 65
          Height = 21
          Style = csDropDownList
          ItemIndex = 1
          TabOrder = 0
          Text = '44100'
          OnChange = cmbSampRateChange
          Items.Strings = (
            '32000'
            '44100'
            '48000'
            '88200'
            '96000'
            '176400'
            '192000'
            '352800'
            '384000'
            '705600'
            '768000'
            '1536000')
        end
      end
      object GroupBox2: TGroupBox
        Left = 104
        Top = 288
        Width = 73
        Height = 49
        Caption = 'Bit Width'
        TabOrder = 8
        object cmbBitWidth: TComboBox
          Left = 8
          Top = 16
          Width = 57
          Height = 21
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 0
          Text = '16'
          OnChange = cmbBitWidthChange
          Items.Strings = (
            '16'
            '24'
            '32(Float)'
            '64(Float)'
            '32(Int)')
        end
      end
      object GroupBox5: TGroupBox
        Left = 184
        Top = 288
        Width = 167
        Height = 49
        Caption = 'HFA'
        TabOrder = 9
        object cmbHFA: TComboBox
          Left = 8
          Top = 16
          Width = 129
          Height = 21
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 0
          Text = 'Cutoff(HFA0)'
          Items.Strings = (
            'Cutoff(HFA0)'
            'Noise(HFA1)'
            'OverTone((HFA2)'
            'OverTone Ex(HFA3)'
            'OverTone Ex(HFA4)')
        end
      end
      object GroupBox7: TGroupBox
        Left = 16
        Top = 344
        Width = 517
        Height = 73
        Caption = 'Default Parameter'
        TabOrder = 10
        object cmbDefaultParameter: TComboBox
          Left = 8
          Top = 16
          Width = 393
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cmbDefaultParameterChange
        end
        object btnSetting: TButton
          Left = 448
          Top = 16
          Width = 49
          Height = 20
          Caption = 'Setting'
          TabOrder = 1
          OnClick = btnSettingClick
        end
        object edtDefaultParam: TEdit
          Left = 8
          Top = 45
          Width = 429
          Height = 21
          ReadOnly = True
          TabOrder = 2
        end
        object btnReload: TButton
          Left = 448
          Top = 45
          Width = 49
          Height = 20
          Caption = 'Reload'
          TabOrder = 3
          OnClick = btnReloadClick
        end
        object btnParam2: TPanel
          Left = 412
          Top = 15
          Width = 25
          Height = 20
          BorderWidth = 1
          Caption = #8593
          Color = clMenu
          ParentBackground = False
          TabOrder = 4
          OnClick = btnParam2Click
        end
      end
      object GroupBox8: TGroupBox
        Left = 16
        Top = 464
        Width = 346
        Height = 65
        Caption = 'Output Path'
        TabOrder = 11
        object lbl_select_directory: TLabel
          Left = 8
          Top = 16
          Width = 273
          Height = 13
          AutoSize = False
          Visible = False
        end
        object edtOutputPath: TButtonedEdit
          Left = 8
          Top = 34
          Width = 325
          Height = 21
          Images = ImageList2
          LeftButton.ImageIndex = 0
          LeftButton.Visible = True
          ReadOnly = True
          RightButton.ImageIndex = 1
          TabOrder = 0
          OnLeftButtonClick = edtOutputPathLeftButtonClick
          OnRightButtonClick = edtOutputPathRightButtonClick
        end
      end
      object GroupBox9: TGroupBox
        Left = 375
        Top = 465
        Width = 158
        Height = 65
        Caption = 'Suffix'
        TabOrder = 12
        object edtSuffix: TButtonedEdit
          Left = 8
          Top = 34
          Width = 129
          Height = 21
          Images = ImageList2
          RightButton.ImageIndex = 1
          RightButton.Visible = True
          TabOrder = 0
          Text = 'upconv'
          OnRightButtonClick = edtSuffixRightButtonClick
        end
        object chkLockSuffix: TCheckBox
          Left = 8
          Top = 16
          Width = 97
          Height = 17
          Caption = 'Lock Suffix'
          TabOrder = 1
        end
      end
      object memoLog: TMemo
        Left = 628
        Top = 16
        Width = 449
        Height = 545
        Lines.Strings = (
          '')
        TabOrder = 13
      end
      object edtSaveSuffix: TEdit
        Left = 574
        Top = 208
        Width = 113
        Height = 21
        ReadOnly = True
        TabOrder = 14
        Visible = False
      end
      object GroupBox6: TGroupBox
        Left = 357
        Top = 288
        Width = 176
        Height = 49
        Caption = 'Normalize'
        TabOrder = 15
        object cmbNormalize: TComboBox
          Left = 8
          Top = 16
          Width = 97
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 1
          TabOrder = 0
          Text = 'Normalize(File)'
          OnChange = cmbNormalizeChange
          Items.Strings = (
            'None'
            'Normalize(File)'
            'Volume(Lv)'
            'Volume(%)')
        end
        object edt_volume_level: TEdit
          Left = 111
          Top = 16
          Width = 54
          Height = 21
          Enabled = False
          NumbersOnly = True
          TabOrder = 1
          Visible = False
          OnChange = edt_volume_levelChange
        end
      end
      object edtTempParam: TEdit
        Left = 574
        Top = 245
        Width = 129
        Height = 21
        TabOrder = 16
        TextHint = 'TempParam'
        Visible = False
      end
      object btnDP: TBitBtn
        Left = 432
        Top = 232
        Width = 25
        Height = 25
        Caption = 'DP'
        Enabled = False
        TabOrder = 17
        OnClick = btnDPClick
      end
      object view_information: TEdit
        Left = 120
        Top = 183
        Width = 413
        Height = 19
        BevelOuter = bvNone
        BorderStyle = bsNone
        Color = clMenu
        Ctl3D = False
        ParentCtl3D = False
        ReadOnly = True
        TabOrder = 19
      end
      object edt_OutputSize: TEdit
        Left = 120
        Top = 209
        Width = 413
        Height = 19
        AutoSize = False
        BevelOuter = bvNone
        BorderStyle = bsNone
        Color = clMenu
        Ctl3D = False
        ParentCtl3D = False
        ReadOnly = True
        TabOrder = 20
      end
      object btn_level_scan: TBitBtn
        Left = 468
        Top = 232
        Width = 65
        Height = 25
        Caption = 'Level Scan'
        TabOrder = 21
        OnClick = btn_level_scanClick
      end
      object cmb_volume_level: TComboBox
        Left = 468
        Top = 304
        Width = 53
        Height = 22
        Style = csOwnerDrawFixed
        Enabled = False
        TabOrder = 22
        OnChange = cmb_volume_levelChange
        Items.Strings = (
          '20'
          '21'
          '22'
          '23'
          '24'
          '25'
          '26'
          '27'
          '28'
          '29'
          '30'
          '31'
          '32'
          '33'
          '34'
          '35'
          '36'
          '37'
          '38'
          '39'
          '40'
          '41'
          '42'
          '43'
          '44'
          '45'
          '46'
          '47'
          '48'
          '49'
          '50'
          '51'
          '52'
          '53'
          '54'
          '55'
          '56'
          '57'
          '58'
          '59'
          '60'
          '61'
          '62'
          '63'
          '64'
          '65'
          '66'
          '67'
          '68'
          '69'
          '70'
          '71'
          '72'
          '73'
          '74'
          '75'
          '76'
          '77'
          '78'
          '79'
          '80'
          '81'
          '82'
          '83'
          '84'
          '85'
          '86'
          '87'
          '88'
          '89'
          '90'
          '91'
          '92'
          '93'
          '94'
          '95'
          '96'
          '97'
          '98'
          '99'
          '100'
          '101'
          '102'
          '103'
          '104'
          '105'
          '106'
          '107'
          '108'
          '109'
          '110'
          '111'
          '112'
          '113'
          '114'
          '115'
          '116'
          '117'
          '118'
          '119'
          '120')
      end
      object edt_output_max_size: TEdit
        Left = 375
        Top = 207
        Width = 158
        Height = 19
        AutoSize = False
        BevelInner = bvNone
        BorderStyle = bsNone
        Color = clMenu
        TabOrder = 23
      end
      object pnlPopup: TPanel
        Left = 16
        Top = 197
        Width = 437
        Height = 157
        BorderWidth = 3
        BorderStyle = bsSingle
        Color = clMenu
        ParentBackground = False
        TabOrder = 24
        Visible = False
        object Label8: TLabel
          Left = 9
          Top = 7
          Width = 134
          Height = 13
          Caption = 'Parameter2(Overwrite)'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object GroupBox3: TGroupBox
          Left = 9
          Top = 27
          Width = 97
          Height = 49
          Caption = 'HFC'
          TabOrder = 0
          object edtHFC_Overwrite: TEdit
            Left = 8
            Top = 16
            Width = 81
            Height = 21
            NumbersOnly = True
            TabOrder = 0
          end
        end
        object GroupBox4: TGroupBox
          Left = 112
          Top = 27
          Width = 257
          Height = 49
          Caption = 'OverSampling'
          TabOrder = 1
          object cmbOverSamplingOverWrite: TComboBox
            Left = 8
            Top = 16
            Width = 137
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 0
            Text = 'None'
            OnChange = cmbOverSamplingOverWriteChange
            Items.Strings = (
              'None'
              'Over Sampling (x2)'
              'Over Sampling (192000)'
              'Over Sampling (384000)'
              'Over Sampling (768000)'
              'Over Sampling (1536000)'
              'Over Sampling (3072000)')
          end
          object chkOS_Overwrite: TCheckBox
            Left = 152
            Top = 18
            Width = 97
            Height = 17
            Caption = 'Overwrite'
            TabOrder = 1
          end
        end
        object GroupBox12: TGroupBox
          Left = 9
          Top = 82
          Width = 404
          Height = 55
          Caption = 'Enable Overrite'
          TabOrder = 2
          object chk_ABE_Overrite: TCheckBox
            Left = 8
            Top = 24
            Width = 70
            Height = 17
            Caption = 'ABE'
            TabOrder = 0
          end
          object chk_PostABE_Overrite: TCheckBox
            Left = 84
            Top = 24
            Width = 70
            Height = 17
            Caption = 'Post ABE'
            TabOrder = 1
          end
          object chk_NR_Overrite: TCheckBox
            Left = 160
            Top = 24
            Width = 70
            Height = 17
            Caption = 'NR'
            TabOrder = 2
          end
          object chk_MS_Overrite: TCheckBox
            Left = 236
            Top = 24
            Width = 70
            Height = 17
            Caption = 'Mid/Side'
            TabOrder = 3
          end
          object chk_LRC_Overrite: TCheckBox
            Left = 312
            Top = 24
            Width = 70
            Height = 17
            Caption = 'LRC'
            TabOrder = 4
          end
        end
      end
      object ver: TMemo
        Left = 400
        Top = 3
        Width = 133
        Height = 21
        BevelInner = bvNone
        BevelOuter = bvNone
        BorderStyle = bsNone
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlue
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        Lines.Strings = (
          'Ver 1.0.12 (2024/10/27)')
        ParentFont = False
        ReadOnly = True
        TabOrder = 25
      end
    end
    object TabSheet1: TTabSheet
      Caption = 'Option'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object CategoryPanelGroup1: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 1095
        Height = 533
        VertScrollBar.Tracking = True
        VertScrollBar.Visible = False
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel1_2: TCategoryPanel
          Top = 501
          Height = 30
          Caption = 'Config Option'
          Collapsed = True
          TabOrder = 0
          OnExpand = CategoryPanel1_2Expand
          ExplicitWidth = 185
          ExpandedHeight = 498
          object Label4: TLabel
            Left = 15
            Top = 11
            Width = 128
            Height = 13
            Caption = 'Resource Option (CPU)'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label5: TLabel
            Left = 23
            Top = 43
            Width = 62
            Height = 13
            Caption = 'Thread(Max)'
          end
          object Label7: TLabel
            Left = 25
            Top = 223
            Width = 31
            Height = 13
            Caption = 'File IO'
            Visible = False
          end
          object Label6: TLabel
            Left = 23
            Top = 82
            Width = 64
            Height = 13
            Caption = 'Upconv(Max)'
          end
          object labelpri: TLabel
            Left = 255
            Top = 43
            Width = 57
            Height = 13
            Caption = 'CPU Priority'
          end
          object Bevel5: TBevel
            Left = 8
            Top = 258
            Width = 457
            Height = 9
            Shape = bsTopLine
          end
          object Label20: TLabel
            Left = 15
            Top = 273
            Width = 88
            Height = 13
            Caption = 'Portable Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Bevel7: TBevel
            Left = 8
            Top = 367
            Width = 457
            Height = 9
            Shape = bsTopLine
          end
          object Label9: TLabel
            Left = 15
            Top = 374
            Width = 94
            Height = 13
            Caption = 'Complete Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label23: TLabel
            Left = 15
            Top = 115
            Width = 146
            Height = 13
            Caption = 'Resource Option (Disk IO)'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object cmbThread: TComboBox
            Left = 103
            Top = 41
            Width = 46
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 0
            Text = '1'
            Items.Strings = (
              '1'
              '2'
              '3'
              '4'
              '5'
              '6'
              '7'
              '8'
              '9'
              '10'
              '11'
              '12'
              '13'
              '14'
              '15'
              '16'
              '17'
              '18'
              '19'
              '20'
              '21'
              '22'
              '23'
              '24')
          end
          object cmbFileIO: TComboBox
            Left = 105
            Top = 220
            Width = 46
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 1
            Text = '1'
            Visible = False
            Items.Strings = (
              '1'
              '2'
              '3'
              '4'
              '5'
              '6'
              '7'
              '8'
              '9'
              '10'
              '11'
              '12'
              '13'
              '14'
              '15'
              '16'
              '17'
              '18'
              '19'
              '20'
              '21'
              '22'
              '23'
              '24')
          end
          object cmbUpconvFile: TComboBox
            Left = 103
            Top = 79
            Width = 46
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 2
            Text = '1'
            Items.Strings = (
              '1'
              '2'
              '4'
              '8')
          end
          object cmbPriority: TComboBox
            Left = 334
            Top = 40
            Width = 81
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 3
            Text = 'Low (IDLE)'
            Items.Strings = (
              'Low (IDLE)'
              'Normal'
              'High')
          end
          object chkFIO_Ex: TCheckBox
            Left = 202
            Top = 222
            Width = 128
            Height = 17
            Caption = 'File IO Auto(Max Size)'
            TabOrder = 4
            Visible = False
            OnClick = chkFIO_ExClick
          end
          object cmbFileIOMax: TComboBox
            Left = 336
            Top = 223
            Width = 81
            Height = 22
            Style = csOwnerDrawFixed
            Enabled = False
            ItemIndex = 0
            TabOrder = 5
            Text = '100'
            Visible = False
            Items.Strings = (
              '100'
              '200'
              '250'
              '500'
              '1000'
              '1200'
              '1400'
              '1600'
              '1800'
              '2000'
              '3000'
              '4000'
              '6000'
              '8000'
              '10000')
          end
          object grp_portable: TGroupBox
            Left = 25
            Top = 292
            Width = 393
            Height = 41
            Enabled = False
            TabOrder = 6
            object rdo_upconv_mode_1: TRadioButton
              Left = 15
              Top = 14
              Width = 161
              Height = 17
              Caption = 'Normal'
              Checked = True
              TabOrder = 0
              TabStop = True
            end
            object rdo_upconv_mode_2: TRadioButton
              Left = 176
              Top = 14
              Width = 169
              Height = 17
              Caption = 'Portable Mode'
              TabOrder = 1
            end
          end
          object GroupBox10: TGroupBox
            Left = 25
            Top = 393
            Width = 392
            Height = 45
            TabOrder = 7
            object chkPowerOff: TCheckBox
              Left = 15
              Top = 16
              Width = 103
              Height = 17
              Caption = 'Power Off'
              TabOrder = 0
              OnClick = chkPowerOffClick
            end
            object chkDeleteSourceFile: TCheckBox
              Left = 176
              Top = 16
              Width = 148
              Height = 17
              Caption = 'Delete Source File'
              TabOrder = 1
            end
          end
          object GroupBox13: TGroupBox
            Left = 23
            Top = 136
            Width = 442
            Height = 73
            TabOrder = 8
            object rdo_file_io_sec: TRadioButton
              Left = 17
              Top = 12
              Width = 161
              Height = 17
              Caption = 'File IO Size (Buffering n sec)'
              Checked = True
              TabOrder = 0
              TabStop = True
            end
            object cmbFIOSecSize: TComboBox
              Left = 32
              Top = 39
              Width = 141
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 1
              Text = '10 Sec'
              Items.Strings = (
                '10 Sec'
                '30 Sec'
                '60 Sec'
                '120 Sec'
                '300 Sec')
            end
            object rdo_file_io_mb: TRadioButton
              Left = 232
              Top = 12
              Width = 181
              Height = 17
              Caption = 'File IO Size (Buffering n MB)'
              TabOrder = 2
            end
            object cmbFIOMBSize: TComboBox
              Left = 232
              Top = 40
              Width = 141
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 3
              Text = '10MB'
              Items.Strings = (
                '10MB'
                '50MB'
                '100MB'
                '500MB'
                '1000MB')
            end
          end
        end
        object CategoryPanel1_1: TCategoryPanel
          Top = 0
          Height = 501
          Caption = 'Convert / Output Option'
          TabOrder = 1
          OnExpand = CategoryPanel1_1Expand
          ExplicitWidth = 185
          object Label13: TLabel
            Left = 3
            Top = 3
            Width = 85
            Height = 13
            Caption = 'Convert Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label15: TLabel
            Left = 18
            Top = 91
            Width = 29
            Height = 13
            Caption = 'Count'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
          object Bevel4: TBevel
            Left = 3
            Top = 152
            Width = 510
            Height = 9
            Shape = bsTopLine
          end
          object Label12: TLabel
            Left = 3
            Top = 160
            Width = 79
            Height = 13
            Caption = 'Output Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label10: TLabel
            Left = 18
            Top = 188
            Width = 34
            Height = 13
            Caption = 'Format'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
          object Label14: TLabel
            Left = 275
            Top = 187
            Width = 42
            Height = 13
            Caption = 'Split Size'
          end
          object Label11: TLabel
            Left = 18
            Top = 218
            Width = 39
            Height = 13
            Caption = 'Encoder'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
          object lbl_cdp_from_to: TLabel
            Left = 147
            Top = 90
            Width = 3
            Height = 13
            Transparent = False
          end
          object Bevel3: TBevel
            Left = 3
            Top = 288
            Width = 510
            Height = 9
            Shape = bsTopLine
          end
          object Label24: TLabel
            Left = 3
            Top = 295
            Width = 101
            Height = 13
            Caption = 'Output File Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label21: TLabel
            Left = 16
            Top = 27
            Width = 273
            Height = 13
            Caption = 'ADDP Mode ( File Split(30 Sec) -> Processing -> Concat) '
          end
          object chk_change_default_parameter: TCheckBox
            Left = 16
            Top = 56
            Width = 233
            Height = 17
            Caption = 'CDP Mode(Change Default Parameter)'
            TabOrder = 0
            OnClick = chk_change_default_parameterClick
          end
          object edtDefParamCount: TEdit
            Left = 66
            Top = 87
            Width = 41
            Height = 21
            NumbersOnly = True
            TabOrder = 1
            Text = '1'
            OnChange = edtDefParamCountChange
          end
          object UpDDefParamCount: TUpDown
            Left = 107
            Top = 87
            Width = 16
            Height = 21
            Associate = edtDefParamCount
            Min = 1
            Max = 99
            Position = 1
            TabOrder = 2
          end
          object chk_conversion_test: TCheckBox
            Left = 16
            Top = 119
            Width = 185
            Height = 17
            Caption = 'Conversion Test'
            TabOrder = 3
          end
          object cmb_ct_min: TComboBox
            Left = 140
            Top = 119
            Width = 73
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 4
            Text = '10   Sec'
            Items.Strings = (
              '10   Sec'
              '30   Sec'
              '60   Sec'
              '90  Sec'
              '120 Sec')
          end
          object cmbOutputFormat: TComboBox
            Left = 66
            Top = 184
            Width = 181
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 5
            Text = 'WAV(Microsoft)'
            Items.Strings = (
              'WAV(Microsoft)'
              'WAV(Max 2G Microsoft)'
              'BWF(Broadcast Wave Format)'
              'RF64')
          end
          object cmbSplit: TComboBox
            Left = 328
            Top = 184
            Width = 185
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 6
            Text = 'None'
            OnChange = cmbSplitChange
            Items.Strings = (
              'None'
              '300'
              '600')
          end
          object cmbEncoder: TComboBox
            Left = 66
            Top = 215
            Width = 147
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 7
            Text = 'None'
            OnChange = cmbEncoderChange
            Items.Strings = (
              'None')
          end
          object chk_save_dp: TCheckBox
            Left = 66
            Top = 252
            Width = 185
            Height = 17
            Caption = 'Output Default Parameter(WAV)'
            TabOrder = 8
          end
          object chk_output_mid_side: TCheckBox
            Left = 18
            Top = 321
            Width = 197
            Height = 17
            Caption = 'Mid / Side File'
            TabOrder = 9
            OnClick = chk_output_mid_sideClick
          end
          object chk_output_Left_Right_Center: TCheckBox
            Left = 18
            Top = 348
            Width = 233
            Height = 17
            Caption = 'Left_Right_Center File( LRC_Process Only )'
            TabOrder = 10
            OnClick = chk_output_Left_Right_CenterClick
          end
          object chk_output_mp4: TCheckBox
            Left = 18
            Top = 377
            Width = 241
            Height = 17
            Caption = 'MP4 (video + upconvfe wav)'
            TabOrder = 11
          end
          object edtEncoderInfo: TEdit
            Left = 246
            Top = 215
            Width = 267
            Height = 22
            AutoSize = False
            BevelInner = bvNone
            BevelOuter = bvNone
            ReadOnly = True
            TabOrder = 12
          end
          object memoEncoderInfo: TMemo
            Left = 572
            Top = 184
            Width = 369
            Height = 89
            TabOrder = 13
            WantReturns = False
            WordWrap = False
          end
          object memoEncoderExt: TMemo
            Left = 572
            Top = 284
            Width = 369
            Height = 57
            Lines.Strings = (
              'memoEncoderExt')
            TabOrder = 14
          end
          object cmbADDP: TComboBox
            Left = 327
            Top = 24
            Width = 81
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 15
            Text = 'None'
            OnChange = cmbADDPChange
            Items.Strings = (
              'None'
              '2'
              '4'
              '6'
              '8'
              '10'
              '12'
              '14'
              '16'
              '18'
              '20'
              '22'
              '24')
          end
        end
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Default Parameter'
      ImageIndex = 4
      object CategoryPanelGroup2: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 545
        Height = 533
        VertScrollBar.Tracking = True
        VertScrollBar.Visible = False
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel2_2: TCategoryPanel
          Top = 502
          Height = 30
          Caption = 'Default Parameter(Import)'
          Collapsed = True
          TabOrder = 0
          OnExpand = CategoryPanel2_2Expand
          ExpandedHeight = 498
          object Label18: TLabel
            Left = 20
            Top = 20
            Width = 40
            Height = 13
            Caption = 'Import'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label19: TLabel
            Left = 20
            Top = 221
            Width = 167
            Height = 13
            Caption = 'Open Default Parameter Pack'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object list_import_list: TListView
            Left = 20
            Top = 44
            Width = 413
            Height = 150
            Checkboxes = True
            Columns = <
              item
                Caption = 'Default Parameter Files'
                MaxWidth = 150
                MinWidth = 150
                Width = 150
              end
              item
                Caption = 'Title'
                Width = 250
              end>
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            ViewStyle = vsReport
            OnChange = list_import_listChange
          end
          object btn_DPPack: TBitBtn
            Left = 265
            Top = 205
            Width = 81
            Height = 25
            Caption = 'Open'
            Default = True
            ModalResult = 1
            NumGlyphs = 2
            TabOrder = 1
            OnClick = btn_DPPackClick
          end
          object btn_DP_Import: TBitBtn
            Left = 352
            Top = 205
            Width = 81
            Height = 25
            Caption = 'Import'
            Default = True
            Enabled = False
            Glyph.Data = {
              DE010000424DDE01000000000000760000002800000024000000120000000100
              0400000000006801000000000000000000001000000000000000000000000000
              80000080000000808000800000008000800080800000C0C0C000808080000000
              FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
              3333333333333333333333330000333333333333333333333333F33333333333
              00003333344333333333333333388F3333333333000033334224333333333333
              338338F3333333330000333422224333333333333833338F3333333300003342
              222224333333333383333338F3333333000034222A22224333333338F338F333
              8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
              33333338F83338F338F33333000033A33333A222433333338333338F338F3333
              0000333333333A222433333333333338F338F33300003333333333A222433333
              333333338F338F33000033333333333A222433333333333338F338F300003333
              33333333A222433333333333338F338F00003333333333333A22433333333333
              3338F38F000033333333333333A223333333333333338F830000333333333333
              333A333333333333333338330000333333333333333333333333333333333333
              0000}
            ModalResult = 1
            NumGlyphs = 2
            TabOrder = 2
            OnClick = btn_DP_ImportClick
          end
          object memoDPI: TMemo
            Left = 20
            Top = 248
            Width = 413
            Height = 89
            TabOrder = 3
            Visible = False
            WantReturns = False
            WordWrap = False
          end
        end
        object CategoryPanel2_1: TCategoryPanel
          Top = 0
          Height = 502
          Caption = 'Default Parameter(Active)'
          TabOrder = 1
          OnExpand = CategoryPanel2_1Expand
          object Label16: TLabel
            Left = 16
            Top = 16
            Width = 144
            Height = 13
            Caption = 'Active Default Parameter'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label17: TLabel
            Left = 16
            Top = 224
            Width = 155
            Height = 13
            Caption = 'Inactive Default Parameter'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object list_dp_active: TListBox
            Left = 16
            Top = 40
            Width = 417
            Height = 137
            ItemHeight = 13
            Sorted = True
            TabOrder = 0
            OnClick = list_dp_activeClick
          end
          object btn_to_inactive: TButton
            Left = 16
            Top = 192
            Width = 75
            Height = 25
            Caption = #8595
            Enabled = False
            TabOrder = 1
            OnClick = btn_to_inactiveClick
          end
          object btn_to_active: TButton
            Left = 112
            Top = 192
            Width = 75
            Height = 25
            Caption = #8593
            Enabled = False
            TabOrder = 2
            OnClick = btn_to_activeClick
          end
          object list_dp_inactive: TListBox
            Left = 16
            Top = 248
            Width = 417
            Height = 137
            ItemHeight = 13
            Sorted = True
            TabOrder = 3
            OnClick = list_dp_inactiveClick
          end
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'File Extension'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox11: TGroupBox
        Left = 10
        Top = 15
        Width = 469
        Height = 166
        Caption = 'File Extension ( ffmpeg )'
        TabOrder = 0
        object btn_ext_add: TBitBtn
          Left = 270
          Top = 66
          Width = 75
          Height = 23
          Caption = 'Add'
          Default = True
          Enabled = False
          Glyph.Data = {
            DE010000424DDE01000000000000760000002800000024000000120000000100
            0400000000006801000000000000000000001000000000000000000000000000
            80000080000000808000800000008000800080800000C0C0C000808080000000
            FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
            3333333333333333333333330000333333333333333333333333F33333333333
            00003333344333333333333333388F3333333333000033334224333333333333
            338338F3333333330000333422224333333333333833338F3333333300003342
            222224333333333383333338F3333333000034222A22224333333338F338F333
            8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
            33333338F83338F338F33333000033A33333A222433333338333338F338F3333
            0000333333333A222433333333333338F338F33300003333333333A222433333
            333333338F338F33000033333333333A222433333333333338F338F300003333
            33333333A222433333333333338F338F00003333333333333A22433333333333
            3338F38F000033333333333333A223333333333333338F830000333333333333
            333A333333333333333338330000333333333333333333333333333333333333
            0000}
          ModalResult = 1
          NumGlyphs = 2
          TabOrder = 0
          OnClick = btn_ext_addClick
        end
        object btn_ext_del: TBitBtn
          Left = 359
          Top = 64
          Width = 75
          Height = 25
          Caption = 'Del'
          Enabled = False
          Glyph.Data = {
            DE010000424DDE01000000000000760000002800000024000000120000000100
            0400000000006801000000000000000000001000000000000000000000000000
            80000080000000808000800000008000800080800000C0C0C000808080000000
            FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
            333333333333333333333333000033338833333333333333333F333333333333
            0000333911833333983333333388F333333F3333000033391118333911833333
            38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
            911118111118333338F3338F833338F3000033333911111111833333338F3338
            3333F8330000333333911111183333333338F333333F83330000333333311111
            8333333333338F3333383333000033333339111183333333333338F333833333
            00003333339111118333333333333833338F3333000033333911181118333333
            33338333338F333300003333911183911183333333383338F338F33300003333
            9118333911183333338F33838F338F33000033333913333391113333338FF833
            38F338F300003333333333333919333333388333338FFF830000333333333333
            3333333333333333333888330000333333333333333333333333333333333333
            0000}
          ModalResult = 2
          NumGlyphs = 2
          TabOrder = 1
          OnClick = btn_ext_delClick
        end
        object edt_ext_edit: TEdit
          Left = 270
          Top = 27
          Width = 164
          Height = 21
          MaxLength = 10
          TabOrder = 2
          OnChange = edt_ext_editChange
        end
        object clist_extension: TCheckListBox
          Left = 15
          Top = 27
          Width = 242
          Height = 114
          OnClickCheck = clist_extensionClickCheck
          AutoComplete = False
          Items.Strings = (
            'flac'
            'mp3'
            'mp4'
            'wma'
            'opus'
            'wv')
          Style = lbOwnerDrawFixed
          TabOrder = 3
        end
      end
    end
  end
  object btnExit: TBitBtn
    Left = 494
    Top = 592
    Width = 41
    Height = 25
    Caption = 'Close'
    NumGlyphs = 2
    TabOrder = 1
    OnClick = btnExitClick
  end
  object btnAbort: TBitBtn
    Left = 288
    Top = 592
    Width = 65
    Height = 25
    Caption = 'Abort'
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      333333333333333333333333000033338833333333333333333F333333333333
      0000333911833333983333333388F333333F3333000033391118333911833333
      38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
      911118111118333338F3338F833338F3000033333911111111833333338F3338
      3333F8330000333333911111183333333338F333333F83330000333333311111
      8333333333338F3333383333000033333339111183333333333338F333833333
      00003333339111118333333333333833338F3333000033333911181118333333
      33338333338F333300003333911183911183333333383338F338F33300003333
      9118333911183333338F33838F338F33000033333913333391113333338FF833
      38F338F300003333333333333919333333388333338FFF830000333333333333
      3333333333333333333888330000333333333333333333333333333333333333
      0000}
    ModalResult = 3
    NumGlyphs = 2
    TabOrder = 2
    Visible = False
    OnClick = btnAbortClick
  end
  object btnStart: TBitBtn
    Left = 288
    Top = 592
    Width = 65
    Height = 25
    Caption = 'Start'
    Default = True
    Enabled = False
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    ModalResult = 1
    NumGlyphs = 2
    TabOrder = 3
    OnClick = btnStartClick
  end
  object prgExec: TProgressBar
    Left = 112
    Top = 596
    Width = 169
    Height = 17
    Step = 1
    TabOrder = 4
    Visible = False
  end
  object memo_output_files: TMemo
    Left = 576
    Top = 16
    Width = 369
    Height = 57
    TabOrder = 5
    WantReturns = False
    WordWrap = False
  end
  object memo_output_end_files: TMemo
    Left = 576
    Top = 88
    Width = 369
    Height = 57
    TabOrder = 6
    WantReturns = False
    WordWrap = False
  end
  object memo_LvFile_backup: TMemo
    Left = 576
    Top = 160
    Width = 369
    Height = 57
    TabOrder = 7
  end
  object btnPause: TBitBtn
    Left = 418
    Top = 593
    Width = 65
    Height = 25
    Caption = 'Pause'
    Enabled = False
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      333333333333333333333333000033338833333333333333333F333333333333
      0000333911833333983333333388F333333F3333000033391118333911833333
      38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
      911118111118333338F3338F833338F3000033333911111111833333338F3338
      3333F8330000333333911111183333333338F333333F83330000333333311111
      8333333333338F3333383333000033333339111183333333333338F333833333
      00003333339111118333333333333833338F3333000033333911181118333333
      33338333338F333300003333911183911183333333383338F338F33300003333
      9118333911183333338F33838F338F33000033333913333391113333338FF833
      38F338F300003333333333333919333333388333338FFF830000333333333333
      3333333333333333333888330000333333333333333333333333333333333333
      0000}
    ModalResult = 3
    NumGlyphs = 2
    TabOrder = 8
    OnClick = btnPauseClick
  end
  object chkStop: TCheckBox
    Left = 359
    Top = 596
    Width = 57
    Height = 17
    Caption = 'Stop'
    TabOrder = 9
  end
  object OpenDialog: TOpenDialog
    Ctl3D = False
    Filter = 'WAV|*.WAV'
    Options = [ofReadOnly, ofHideReadOnly, ofAllowMultiSelect, ofExtensionDifferent, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Upconv'
    Left = 896
    Top = 588
  end
  object ImageList1: TImageList
    Left = 840
    Top = 588
    Bitmap = {
      494C010104000800C80410001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000002000000001002000000000000020
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F000000000000000000000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000BFBFBF00BFBFBF00BFBFBF00000000000000000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080800000000000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000BFBFBF00BFBFBF00BFBFBF00FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF0000000000000000000000
      0000BFBFBF00BFBFBF00BFBFBF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00008080000080800000808000007F7F7F000000000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000808000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      00008080000080800000808000007F7F7F00FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00000000000000
      00000000000080800000808000007F7F7F00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000FFFF0000808000008080000080800000BFBFBF0000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF00008080
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000808000008080000080800000BFBFBF00FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF000000
      000000000000000000008080000080800000BFBFBF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00007F7F7F0080800000FFFF000080800000BFBFBF00BFBFBF00000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF0000FFFF
      000080800000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      00007F7F7F0080800000FFFF000080800000BFBFBF00BFBFBF00FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF0000000000000000000000000080800000BFBFBF00BFBFBF00FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00000000000000000000BFBFBF0080800000BFBFBF0080800000000000000000
      0000000000007F7F7F0000000000000000000000000000000000808000008080
      00008080000080800000808000008080000080800000FFFF0000FFFF0000FFFF
      0000FFFF0000808000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000BFBFBF0080800000BFBFBF0080800000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00000000000000000000000000BFBFBF0080800000FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000808000000000000080800000000000000000
      0000000000007F7F7F0000000000000000000000000000000000FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF0000FFFF00008080000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000808000000000000080800000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000000000000000000080800000FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000BFBFBF00BFBFBF0080800000000000000000
      0000000000007F7F7F0000000000000000000000000000000000FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000BFBFBF00BFBFBF0080800000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000BFBFBF00FFFF0000BFBFBF00000000000000
      0000000000007F7F7F0000000000000000000000000000000000FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF0000FFFF00000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000BFBFBF00FFFF0000BFBFBF00FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00BFBFBF00000000000000000000000000FFFF
      FF00FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      000000000000000000000000000080800000FFFF000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF0000FFFF
      0000FFFF0000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF000080800000FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080800000FFFF000000000000000000000000
      0000FFFFFF007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      0000000000000000000000000000BFBFBF000000000000000000000000000000
      0000000000007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF0000FFFF
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000BFBFBF00FFFF0000FFFF0000FFFF0000FFFF
      0000FFFF00007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00BFBFBF00FFFFFF00FFFFFF00000000000000
      0000000000007F7F7F00FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00000000000000000000000000000000000000000000000000007F7F7F007F7F
      7F007F7F7F007F7F7F0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000FFFF00000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF00007F7F7F007F7F
      7F007F7F7F007F7F7F000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF007F7F7F000000
      00000000000000000000FFFFFF00FFFFFF00000000007F7F7F00000000000000
      00000000000000000000000000000000000000000000000000007F7F7F000000
      00007F7F7F000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFF0000000000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF00007F7F7F000000
      00007F7F7F00000000000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF007F7F7F00FFFF
      FF00000000000000000000000000FFFFFF00000000007F7F7F00000000000000
      00000000000000000000000000000000000000000000000000007F7F7F007F7F
      7F00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F00FFFF0000FFFF
      0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF00007F7F7F007F7F
      7F0000000000000000000000000000000000FFFFFF007F7F7F00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF007F7F7F007F7F
      7F00FFFFFF00000000000000000000000000000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F000000
      000000000000000000000000000000000000FFFFFF007F7F7F007F7F7F007F7F
      7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F00FFFF
      FF00FFFFFF00FFFFFF000000000000000000424D3E000000000000003E000000
      2800000040000000200000000100010000000000000100000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000008003FFFF80038000BFFBFFFF8003E000
      B1FBFFBF80037000B0FBFF9F80033800B07BFF8F80031C00B03BFF8780030E00
      BC3BC00380030700BEBBC00180830380BE3BC001800301C0BE3BC003800300E0
      BE7BFF8780030070BEFBFF8F80030038BFC3FF9F8003001CBFD7FFBF8017000E
      BFCFFFFF800F0007801FFFFF801F000300000000000000000000000000000000
      000000000000}
  end
  object ImageList2: TImageList
    Left = 780
    Top = 588
    Bitmap = {
      494C010102000800BC0410001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      00000000000000000000000000000000000000000000078DBE00078DBE00078D
      BE00078DBE00078DBE00078DBE00078DBE00078DBE00078DBE00078DBE00078D
      BE00078DBE000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000732DE000732DE000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0025A1D10071C6E80084D7
      FA0066CDF90065CDF90065CDF90065CDF90065CDF80065CDF90065CDF80066CE
      F9003AADD8001999C9000000000000000000000000000732DE000732DE000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000732DE000732DE00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE004CBCE70039A8D100A0E2
      FB006FD4FA006FD4F9006ED4FA006FD4F9006FD4FA006FD4FA006FD4FA006ED4
      F9003EB1D900C9F0F300078DBE0000000000000000000732DE000732DE000732
      DE00000000000000000000000000000000000000000000000000000000000000
      00000732DE000732DE0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0072D6FA00078DBE00AEE9
      FC0079DCFB0079DCFB0079DCFB0079DCFB0079DCFB007ADCFB0079DCFA0079DC
      FA0044B5D900C9F0F300078DBE0000000000000000000732DE000732DD000732
      DE000732DE000000000000000000000000000000000000000000000000000732
      DE000732DE000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0079DDFB001899C7009ADF
      F30092E7FC0084E4FB0083E4FC0083E4FC0084E4FC0083E4FC0083E4FB0084E5
      FC0048B9DA00C9F0F3001496C4000000000000000000000000000534ED000732
      DF000732DE000732DE00000000000000000000000000000000000732DE000732
      DE00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0082E3FC0043B7DC0065C2
      E000ABF0FC008DEBFC008DEBFC008DEBFD008DEBFD008DEBFC008DEBFD008DEB
      FC004CBBDA00C9F0F300C9F0F300078DBE000000000000000000000000000000
      00000732DE000732DE000732DD00000000000732DD000732DE000732DE000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE008AEAFC0077DCF300219C
      C700FEFFFF00C8F7FD00C9F7FD00C9F7FD00C9F7FE00C8F7FE00C9F7FD00C8F7
      FE009BD5E600EAFEFE00D2F3F800078DBE000000000000000000000000000000
      0000000000000732DD000633E6000633E6000633E9000732DC00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE0093F0FE0093F0FD001697
      C500078DBE00078DBE00078DBE00078DBE00078DBE00078DBE00078DBE00078D
      BE00078DBE00078DBE00078DBE00078DBE000000000000000000000000000000
      000000000000000000000633E3000732E3000534EF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE009BF5FE009AF6FE009AF6
      FE009BF5FD009BF6FE009AF6FE009BF5FE009AF6FD009BF5FE009AF6FE009AF6
      FE000989BA000000000000000000000000000000000000000000000000000000
      0000000000000732DD000534ED000533E9000434EF000434F500000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000078DBE00FEFEFE00A0FBFF00A0FB
      FE00A0FBFE00A1FAFE00A1FBFE00A0FAFE00A1FBFE00A1FBFF00A0FBFF00A1FB
      FF000989BA000000000000000000000000000000000000000000000000000000
      00000434F4000534EF000533EB0000000000000000000434F4000335F8000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000078DBE00FEFEFE00A5FE
      FF00A5FEFF00A5FEFF00078DBE00078DBE00078DBE00078DBE00078DBE00078D
      BE00000000000000000000000000000000000000000000000000000000000335
      FC000534EF000434F800000000000000000000000000000000000335FC000335
      FB00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000078DBE00078D
      BE00078DBE00078DBE0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000335FB000335
      FB000335FC000000000000000000000000000000000000000000000000000335
      FB000335FB000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000335FB000335FB000335
      FB00000000000000000000000000000000000000000000000000000000000000
      0000000000000335FB0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000335FB000335FB000335FB000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000335FB000335FB00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF008007FFFC0000000000039FF900000000
      00018FF300000000000187E7000000000001C3CF000000000000F11F00000000
      0000F83F000000000000FC7F000000000007F83F000000000007F19F00000000
      800FE3CF00000000C3FFC7E700000000FFFF8FFB00000000FFFF1FFF00000000
      FFFF3FFF00000000FFFFFFFF0000000000000000000000000000000000000000
      000000000000}
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 724
    Top = 588
  end
  object OpenDialogZip: TOpenDialog
    Filter = 'zip|*.zip'
    Options = [ofReadOnly, ofEnableSizing]
    Left = 672
    Top = 588
  end
  object PopupMenuList: TPopupMenu
    Left = 940
    Top = 589
    object fileHFC: TMenuItem
      Caption = 'File HFC'
      OnClick = fileHFCClick
    end
    object estConversionOffset1: TMenuItem
      Caption = 'Test Conversion Offset'
      OnClick = estConversionOffset1Click
    end
    object CopyCommandLine1: TMenuItem
      Caption = 'Copy Command Line'
      OnClick = CopyCommandLine1Click
    end
  end
end
