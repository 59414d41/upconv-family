object Form1: TForm1
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Upconv EQ'
  ClientHeight = 584
  ClientWidth = 1240
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
    Top = 490
    Width = 401
    Height = 95
    Align = alCustom
    Style = bsRaised
  end
  object lbl_exec_status: TLabel
    Left = 8
    Top = 534
    Width = 97
    Height = 13
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbl_count: TLabel
    Left = 19
    Top = 498
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
  object lbl_co: TLabel
    Left = 296
    Top = 496
    Width = 73
    Height = 13
    AutoSize = False
  end
  object lbl_play_set_device: TLabel
    Left = 19
    Top = 558
    Width = 161
    Height = 13
    Caption = 'Set Option -> Play -> Device'
    Color = clSilver
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
    Visible = False
  end
  object pageMain: TPageControl
    Left = 5
    Top = 8
    Width = 401
    Height = 473
    ActivePage = tabUpconv
    TabOrder = 0
    OnChange = pageMainChange
    object tabUpconv: TTabSheet
      Caption = 'EQ'
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
        Top = 215
        Width = 51
        Height = 13
        Caption = 'Equalizer'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Bevel2: TBevel
        Left = 8
        Top = 336
        Width = 377
        Height = 9
        Shape = bsTopLine
      end
      object Label3: TLabel
        Left = 8
        Top = 343
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
      object Label186: TLabel
        Left = 18
        Top = 163
        Width = 56
        Height = 13
        Caption = 'Information'
      end
      object LvFile: TListView
        Left = 16
        Top = 24
        Width = 361
        Height = 131
        BevelInner = bvLowered
        Columns = <
          item
            Caption = 'Status'
            MaxWidth = 50
            MinWidth = 50
          end
          item
            Caption = 'Filename'
            MaxWidth = 155
            MinWidth = 155
            Width = 155
          end
          item
            Caption = 'Time'
            MaxWidth = 65
            MinWidth = 65
            Width = 65
          end
          item
            Caption = 'Information'
            MaxWidth = 200
            MinWidth = 200
            Width = 200
          end>
        ColumnClick = False
        FlatScrollBars = True
        ReadOnly = True
        RowSelect = True
        SmallImages = ImageList1
        TabOrder = 0
        ViewStyle = vsReport
        OnChange = LvFileChange
        OnColumnClick = LvFileColumnClick
        OnSelectItem = LvFileSelectItem
      end
      object btnAdd: TBitBtn
        Left = 24
        Top = 184
        Width = 65
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
        Left = 96
        Top = 184
        Width = 65
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
        Left = 168
        Top = 184
        Width = 65
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
        Left = 239
        Top = 184
        Width = 25
        Height = 25
        Caption = #9650
        Enabled = False
        TabOrder = 4
        OnClick = btnUpClick
      end
      object btnDown: TBitBtn
        Left = 271
        Top = 184
        Width = 25
        Height = 25
        Caption = #9660
        Enabled = False
        TabOrder = 5
        OnClick = btnDownClick
      end
      object GroupBox7: TGroupBox
        Left = 14
        Top = 234
        Width = 362
        Height = 44
        Caption = 'Preset Equalizer'
        TabOrder = 6
        object cmbEQParameter: TComboBox
          Left = 8
          Top = 16
          Width = 211
          Height = 21
          Style = csDropDownList
          TabOrder = 0
          OnChange = cmbEQParameterChange
        end
        object btnLoad: TBitBtn
          Left = 448
          Top = 24
          Width = 65
          Height = 25
          Kind = bkAll
          NumGlyphs = 2
          TabOrder = 1
        end
        object btn_Load: TButton
          Left = 236
          Top = 16
          Width = 46
          Height = 21
          Caption = 'Load'
          TabOrder = 2
          OnClick = btn_LoadClick
        end
        object btn_eq_delete: TBitBtn
          Left = 288
          Top = 16
          Width = 46
          Height = 21
          Caption = 'Delete'
          Enabled = False
          ModalResult = 3
          NumGlyphs = 2
          TabOrder = 3
          OnClick = btn_eq_deleteClick
        end
      end
      object GroupBox8: TGroupBox
        Left = 16
        Top = 360
        Width = 249
        Height = 65
        Caption = 'Output Path'
        TabOrder = 7
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
          Width = 233
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
        Left = 272
        Top = 360
        Width = 105
        Height = 65
        Caption = 'Suffix'
        TabOrder = 8
        object edtSuffix: TButtonedEdit
          Left = 8
          Top = 34
          Width = 81
          Height = 21
          Images = ImageList2
          RightButton.ImageIndex = 1
          RightButton.Visible = True
          TabOrder = 0
          Text = 'eq'
          OnRightButtonClick = edtSuffixRightButtonClick
        end
      end
      object GroupBox6: TGroupBox
        Left = 10
        Top = 281
        Width = 367
        Height = 49
        Caption = 'Normalize'
        TabOrder = 9
        object Label177: TLabel
          Left = 127
          Top = 24
          Width = 47
          Height = 13
          Caption = 'Level (%)'
        end
        object cmb_volume_level: TComboBox
          Left = 180
          Top = 20
          Width = 50
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 50
          TabOrder = 0
          Text = '100'
          Items.Strings = (
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
        object cmbNormalize: TComboBox
          Left = 12
          Top = 20
          Width = 101
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 0
          TabOrder = 1
          Text = 'None'
          OnChange = cmbNormalizeChange
          Items.Strings = (
            'None'
            'Normalize (File)'
            'Volume (%)')
        end
      end
      object edtVInformation: TEdit
        Left = 80
        Top = 161
        Width = 153
        Height = 17
        BevelInner = bvNone
        BorderStyle = bsNone
        Color = clMenu
        Ctl3D = False
        ParentCtl3D = False
        ReadOnly = True
        TabOrder = 10
      end
      object edtFileSize: TEdit
        Left = 239
        Top = 161
        Width = 138
        Height = 17
        BevelOuter = bvNone
        BorderStyle = bsNone
        Color = clMenu
        ReadOnly = True
        TabOrder = 11
      end
    end
    object tabAnalyze: TTabSheet
      Caption = 'EQ Analyze'
      ImageIndex = 1
      object Label15: TLabel
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
      object Bevel3: TBevel
        Left = 8
        Top = 216
        Width = 377
        Height = 9
        Shape = bsTopLine
      end
      object Label176: TLabel
        Left = 8
        Top = 224
        Width = 45
        Height = 13
        Caption = 'Analyze'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object LvAnaFile: TListView
        Left = 16
        Top = 24
        Width = 360
        Height = 150
        Columns = <
          item
            Caption = 'Status'
            MaxWidth = 50
            MinWidth = 50
          end
          item
            Caption = 'Filename'
            MaxWidth = 220
            MinWidth = 220
            Width = 220
          end
          item
            Caption = 'Time'
            MaxWidth = 75
            MinWidth = 75
            Width = 75
          end>
        ReadOnly = True
        RowSelect = True
        SmallImages = ImageList1
        TabOrder = 0
        ViewStyle = vsReport
        OnChange = LvAnaFileChange
        OnSelectItem = LvAnaFileSelectItem
      end
      object btn_ana_add: TBitBtn
        Left = 24
        Top = 184
        Width = 65
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
        OnClick = btn_ana_addClick
      end
      object btnAnaDel: TBitBtn
        Left = 96
        Top = 183
        Width = 65
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
        OnClick = btnAnaDelClick
      end
      object btnAnaClear: TBitBtn
        Left = 168
        Top = 183
        Width = 65
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
        OnClick = btnAnaClearClick
      end
      object btnAnaUp: TBitBtn
        Left = 288
        Top = 183
        Width = 25
        Height = 25
        Caption = #9650
        Enabled = False
        TabOrder = 4
        OnClick = btnAnaUpClick
      end
      object btnAnaDown: TBitBtn
        Left = 320
        Top = 183
        Width = 25
        Height = 25
        Caption = #9660
        Enabled = False
        TabOrder = 5
        OnClick = btnAnaDownClick
      end
      object GroupBox12: TGroupBox
        Left = 16
        Top = 248
        Width = 361
        Height = 73
        Caption = 'Analyze'
        TabOrder = 6
        object cmbEQ_Analyze: TComboBox
          Left = 9
          Top = 16
          Width = 273
          Height = 22
          Style = csOwnerDrawFixed
          TabOrder = 0
          OnChange = cmbEQ_AnalyzeChange
        end
        object btn_EQ_Set: TButton
          Left = 288
          Top = 16
          Width = 65
          Height = 22
          Caption = 'EQ Set'
          Enabled = False
          TabOrder = 1
          OnClick = btn_EQ_SetClick
        end
        object rdo_Analyze_Phase1: TRadioButton
          Left = 13
          Top = 44
          Width = 85
          Height = 17
          Caption = '1. From File'
          Checked = True
          TabOrder = 2
          TabStop = True
          OnClick = rdo_Analyze_Phase1Click
        end
        object rdo_Analyze_Phase2: TRadioButton
          Left = 104
          Top = 44
          Width = 74
          Height = 17
          Caption = '2. To File'
          TabOrder = 3
          OnClick = rdo_Analyze_Phase2Click
        end
        object rdo_Analyze_Phase3: TRadioButton
          Left = 184
          Top = 44
          Width = 73
          Height = 17
          Caption = '3. EQ Set'
          TabOrder = 4
          OnClick = rdo_Analyze_Phase3Click
        end
      end
    end
    object Option: TTabSheet
      Caption = 'Option'
      ImageIndex = 3
      object CategoryPanelGroup1: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 393
        Height = 445
        VertScrollBar.Tracking = True
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel1: TCategoryPanel
          Top = 413
          Height = 30
          Caption = 'Play Option'
          Collapsed = True
          TabOrder = 0
          OnExpand = CategoryPanel1Expand
          ExpandedHeight = 411
          object Label13: TLabel
            Left = 12
            Top = 5
            Width = 150
            Height = 13
            Caption = 'Preview Option (Play Only)'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object GroupBox13: TGroupBox
            Left = 7
            Top = 24
            Width = 373
            Height = 89
            Caption = 'Device'
            TabOrder = 0
            object cmbPlayDevice: TComboBox
              Left = 13
              Top = 24
              Width = 342
              Height = 22
              Style = csOwnerDrawFixed
              TabOrder = 0
              OnChange = cmbPlayDeviceChange
            end
            object chk_44100: TCheckBox
              Left = 16
              Top = 52
              Width = 97
              Height = 17
              Caption = '44.1kHz'
              Enabled = False
              TabOrder = 1
            end
            object chk_48000: TCheckBox
              Left = 119
              Top = 52
              Width = 97
              Height = 17
              Caption = '48kHz'
              Enabled = False
              TabOrder = 2
            end
            object chk_96000: TCheckBox
              Left = 222
              Top = 52
              Width = 97
              Height = 17
              Caption = '96kHz'
              Enabled = False
              TabOrder = 3
            end
          end
          object GroupBox11: TGroupBox
            Left = 6
            Top = 119
            Width = 374
            Height = 57
            Caption = 'Sampling Rate'
            Color = clMenuBar
            ParentBackground = False
            ParentColor = False
            TabOrder = 1
            object rdo_preview_rate_44100: TRadioButton
              Left = 16
              Top = 24
              Width = 65
              Height = 17
              Caption = '44100'
              Checked = True
              Color = clMenu
              ParentColor = False
              TabOrder = 0
              TabStop = True
            end
            object rdo_preview_rate_48000: TRadioButton
              Left = 120
              Top = 24
              Width = 65
              Height = 17
              Caption = '48000'
              Color = clMenuBar
              ParentColor = False
              TabOrder = 1
            end
            object rdo_preview_rate_96000: TRadioButton
              Left = 219
              Top = 24
              Width = 113
              Height = 17
              Caption = '96000'
              TabOrder = 2
            end
          end
        end
        object CategoryPanel2: TCategoryPanel
          Top = 0
          Height = 413
          Caption = 'Option'
          TabOrder = 1
          OnExpand = CategoryPanel2Expand
          object Label178: TLabel
            Left = 14
            Top = 10
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
          object Label179: TLabel
            Left = 18
            Top = 38
            Width = 34
            Height = 13
            Caption = 'Format'
          end
          object Label180: TLabel
            Left = 263
            Top = 38
            Width = 43
            Height = 13
            Caption = 'Encorder'
          end
          object Bevel11: TBevel
            Left = 6
            Top = 68
            Width = 382
            Height = 9
            Shape = bsTopLine
          end
          object Label181: TLabel
            Left = 14
            Top = 78
            Width = 93
            Height = 13
            Caption = 'Resource Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label182: TLabel
            Left = 18
            Top = 102
            Width = 62
            Height = 13
            Caption = 'Thread(Max)'
          end
          object Label183: TLabel
            Left = 215
            Top = 102
            Width = 57
            Height = 13
            Caption = 'CPU Priority'
          end
          object Label184: TLabel
            Left = 18
            Top = 135
            Width = 31
            Height = 13
            Caption = 'File IO'
          end
          object Bevel12: TBevel
            Left = 8
            Top = 260
            Width = 382
            Height = 9
            Shape = bsTopLine
          end
          object Label185: TLabel
            Left = 15
            Top = 275
            Width = 76
            Height = 13
            Caption = 'Debug Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label188: TLabel
            Left = 14
            Top = 162
            Width = 224
            Height = 13
            Caption = 'ADDP(Audio Data Division Processing) [30 Sec]'
          end
          object cmbOutputFormat: TComboBox
            Left = 68
            Top = 35
            Width = 170
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 0
            Text = 'WAV(Microsoft)'
            Items.Strings = (
              'WAV(Microsoft)'
              'WAV(Max 2G Microsoft)'
              'BWF(Broadcast Wave Format)'
              'RF64')
          end
          object cmbEncoder: TComboBox
            Left = 315
            Top = 33
            Width = 64
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 1
            Text = 'None'
            Items.Strings = (
              'None')
          end
          object cmbThread: TComboBox
            Left = 97
            Top = 99
            Width = 73
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 2
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
          object cmbPriority: TComboBox
            Left = 292
            Top = 99
            Width = 87
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
          object cmbFileIO: TComboBox
            Left = 98
            Top = 131
            Width = 72
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 4
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
          object be_log: TButtonedEdit
            Left = 154
            Top = 329
            Width = 228
            Height = 21
            Enabled = False
            Images = ImageList2
            LeftButton.ImageIndex = 0
            LeftButton.Visible = True
            ReadOnly = True
            RightButton.ImageIndex = 1
            RightButton.Visible = True
            TabOrder = 5
            Text = 'C:\ProgramData\upconv_eq'
            OnLeftButtonClick = be_logLeftButtonClick
            OnRightButtonClick = be_logRightButtonClick
          end
          object rdo_log_disable: TRadioButton
            Left = 19
            Top = 301
            Width = 113
            Height = 17
            Caption = 'LOG (Disable)'
            TabOrder = 6
            OnClick = rdo_log_disableClick
          end
          object rdo_log_enable: TRadioButton
            Left = 152
            Top = 302
            Width = 113
            Height = 17
            Caption = 'LOG (Enable)'
            TabOrder = 7
            OnClick = rdo_log_enableClick
          end
          object cmbADDP: TComboBox
            Left = 292
            Top = 163
            Width = 87
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 8
            Text = 'None'
            Items.Strings = (
              'None'
              '2'
              '4'
              '6'
              '8')
          end
          object rdo_file_io_sec: TRadioButton
            Left = 18
            Top = 199
            Width = 174
            Height = 17
            Caption = 'File IO Size (Buffering n Sec)'
            Checked = True
            TabOrder = 9
            TabStop = True
          end
          object cmbFIOSecSize: TComboBox
            Left = 18
            Top = 222
            Width = 145
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 10
            Text = '10 Sec'
            Items.Strings = (
              '10 Sec'
              '30 Sec'
              '60 Sec'
              '120 Sec'
              '300 Sec')
          end
          object cmbFIOMBSize: TComboBox
            Left = 234
            Top = 222
            Width = 145
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 11
            Text = '10 MB'
            Items.Strings = (
              '10 MB'
              '50 MB'
              '100 MB'
              '500 MB'
              '1000 MB')
          end
          object rdo_file_io_mb: TRadioButton
            Left = 215
            Top = 199
            Width = 170
            Height = 17
            Caption = 'File IO Size (Buffering n MB)'
            TabOrder = 12
          end
        end
      end
    end
  end
  object btnExit: TBitBtn
    Left = 344
    Top = 528
    Width = 41
    Height = 25
    Caption = 'Close'
    NumGlyphs = 2
    TabOrder = 1
    OnClick = btnExitClick
  end
  object btnAbort: TBitBtn
    Left = 199
    Top = 528
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
    Left = 199
    Top = 528
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
    Top = 532
    Width = 81
    Height = 17
    Step = 1
    TabOrder = 4
    Visible = False
  end
  object btnPause: TBitBtn
    Left = 272
    Top = 528
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
    TabOrder = 5
    OnClick = btnPauseClick
  end
  object PageEQ: TPageControl
    Left = 407
    Top = 3
    Width = 833
    Height = 481
    ActivePage = EQ_Type4
    TabOrder = 6
    object EQ_Type1: TTabSheet
      Caption = 'EQ(Type1)'
      object Label171: TLabel
        Left = 24
        Top = 148
        Width = 6
        Height = 13
        Caption = '0'
      end
      object GroupBox1: TGroupBox
        Left = 16
        Top = 16
        Width = 805
        Height = 417
        Caption = 'EQ(20Hz - 50kHz)'
        TabOrder = 0
        object Label173: TLabel
          Left = 392
          Top = 386
          Width = 43
          Height = 13
          Caption = 'EQ Value'
        end
        object Label209: TLabel
          Left = 156
          Top = 384
          Width = 4
          Height = 13
          Caption = '-'
        end
        object T1_range_label: TLabel
          Left = 225
          Top = 384
          Width = 3
          Height = 13
        end
        object t1_scrbox: TScrollBox
          Left = 8
          Top = 24
          Width = 773
          Height = 345
          VertScrollBar.Visible = False
          TabOrder = 0
          object Panel3: TPanel
            Left = 0
            Top = 3
            Width = 1182
            Height = 321
            Ctl3D = True
            ParentBackground = False
            ParentCtl3D = False
            ShowCaption = False
            TabOrder = 0
            object eq_1_1: TLabel
              Left = 44
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_2: TLabel
              Left = 76
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_3: TLabel
              Left = 108
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_4: TLabel
              Left = 140
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_5: TLabel
              Left = 172
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_6: TLabel
              Left = 204
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_7: TLabel
              Left = 236
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_8: TLabel
              Left = 268
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_9: TLabel
              Left = 298
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_10: TLabel
              Left = 330
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_11: TLabel
              Left = 364
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_12: TLabel
              Left = 394
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_13: TLabel
              Left = 426
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_14: TLabel
              Left = 460
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_15: TLabel
              Left = 492
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_16: TLabel
              Left = 522
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_17: TLabel
              Left = 555
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Label4: TLabel
              Left = 32
              Top = 288
              Width = 21
              Height = 11
              Caption = '20Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label35: TLabel
              Left = 64
              Top = 288
              Width = 21
              Height = 11
              Caption = '25Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label36: TLabel
              Left = 96
              Top = 288
              Width = 21
              Height = 11
              Caption = '31Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label5: TLabel
              Left = 128
              Top = 288
              Width = 21
              Height = 11
              Caption = '40Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label37: TLabel
              Left = 160
              Top = 288
              Width = 21
              Height = 11
              Caption = '50Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label38: TLabel
              Left = 192
              Top = 288
              Width = 21
              Height = 11
              Caption = '63Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label6: TLabel
              Left = 224
              Top = 288
              Width = 21
              Height = 11
              Caption = '80Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label39: TLabel
              Left = 256
              Top = 288
              Width = 26
              Height = 11
              Caption = '100Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label40: TLabel
              Left = 288
              Top = 288
              Width = 26
              Height = 11
              Caption = '125Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label10: TLabel
              Left = 320
              Top = 288
              Width = 26
              Height = 11
              Caption = '160Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label41: TLabel
              Left = 352
              Top = 288
              Width = 26
              Height = 11
              Caption = '200Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label42: TLabel
              Left = 384
              Top = 288
              Width = 26
              Height = 11
              Caption = '250Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label11: TLabel
              Left = 416
              Top = 288
              Width = 26
              Height = 11
              Caption = '315Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label43: TLabel
              Left = 448
              Top = 288
              Width = 26
              Height = 11
              Caption = '400Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label44: TLabel
              Left = 480
              Top = 288
              Width = 26
              Height = 11
              Caption = '500Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label12: TLabel
              Left = 512
              Top = 288
              Width = 26
              Height = 11
              Caption = '630Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label24: TLabel
              Left = 543
              Top = 288
              Width = 26
              Height = 11
              Caption = '800Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object eq_1_18: TLabel
              Left = 588
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_19: TLabel
              Left = 620
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_20: TLabel
              Left = 652
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_21: TLabel
              Left = 684
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_22: TLabel
              Left = 716
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_23: TLabel
              Left = 748
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_24: TLabel
              Left = 782
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_25: TLabel
              Left = 812
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_26: TLabel
              Left = 842
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_27: TLabel
              Left = 874
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_28: TLabel
              Left = 908
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_29: TLabel
              Left = 938
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_30: TLabel
              Left = 970
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_31: TLabel
              Left = 1004
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_32: TLabel
              Left = 1036
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_33: TLabel
              Left = 1066
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_34: TLabel
              Left = 1100
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_1_35: TLabel
              Left = 1132
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Label7: TLabel
              Left = 576
              Top = 288
              Width = 21
              Height = 11
              Caption = '1kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label45: TLabel
              Left = 600
              Top = 288
              Width = 34
              Height = 11
              Caption = '1.25kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label46: TLabel
              Left = 640
              Top = 288
              Width = 29
              Height = 11
              Caption = '1.6kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label8: TLabel
              Left = 672
              Top = 288
              Width = 21
              Height = 11
              Caption = '2kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label47: TLabel
              Left = 696
              Top = 288
              Width = 29
              Height = 11
              Caption = '2.5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label48: TLabel
              Left = 728
              Top = 288
              Width = 34
              Height = 11
              Caption = '3.15kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label14: TLabel
              Left = 768
              Top = 288
              Width = 21
              Height = 11
              Caption = '4kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label49: TLabel
              Left = 800
              Top = 288
              Width = 21
              Height = 11
              Caption = '5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label50: TLabel
              Left = 824
              Top = 288
              Width = 29
              Height = 11
              Caption = '6.3kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label16: TLabel
              Left = 864
              Top = 288
              Width = 21
              Height = 11
              Caption = '8kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label51: TLabel
              Left = 888
              Top = 288
              Width = 26
              Height = 11
              Caption = '10kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label52: TLabel
              Left = 920
              Top = 288
              Width = 34
              Height = 11
              Caption = '12.5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label17: TLabel
              Left = 960
              Top = 288
              Width = 26
              Height = 11
              Caption = '16kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label53: TLabel
              Left = 992
              Top = 288
              Width = 26
              Height = 11
              Caption = '20kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label54: TLabel
              Left = 1024
              Top = 288
              Width = 26
              Height = 11
              Caption = '25kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label9: TLabel
              Left = 1056
              Top = 288
              Width = 34
              Height = 11
              Caption = '31.5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label55: TLabel
              Left = 1096
              Top = 288
              Width = 26
              Height = 11
              Caption = '40kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label25: TLabel
              Left = 1128
              Top = 288
              Width = 26
              Height = 11
              Caption = '50kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label34: TLabel
              Left = 20
              Top = 140
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Bevel7: TBevel
              Left = 20
              Top = 280
              Width = 262
              Height = 28
            end
            object Bevel8: TBevel
              Left = 600
              Top = 280
              Width = 418
              Height = 26
            end
            object TB_1_1: TTrackBar
              Left = 32
              Top = 24
              Width = 25
              Height = 250
              Hint = '20Hz'
              BorderWidth = 1
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              PageSize = 1
              Frequency = 2
              TabOrder = 0
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_1Change
            end
            object TB_1_2: TTrackBar
              Left = 64
              Top = 24
              Width = 25
              Height = 250
              Hint = '25Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 1
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_2Change
            end
            object TB_1_3: TTrackBar
              Left = 96
              Top = 24
              Width = 25
              Height = 250
              Hint = '31Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 2
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_3Change
            end
            object TB_1_4: TTrackBar
              Left = 128
              Top = 24
              Width = 25
              Height = 250
              Hint = '40Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 3
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_4Change
            end
            object TB_1_5: TTrackBar
              Left = 160
              Top = 24
              Width = 25
              Height = 250
              Hint = '50Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 4
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_5Change
            end
            object TB_1_6: TTrackBar
              Left = 192
              Top = 24
              Width = 25
              Height = 250
              Hint = '63Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 5
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_6Change
            end
            object TB_1_7: TTrackBar
              Left = 224
              Top = 24
              Width = 25
              Height = 250
              Hint = '80Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 6
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_7Change
            end
            object TB_1_8: TTrackBar
              Left = 256
              Top = 24
              Width = 25
              Height = 250
              Hint = '100Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 7
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_8Change
            end
            object TB_1_9: TTrackBar
              Left = 288
              Top = 24
              Width = 25
              Height = 250
              Hint = '125Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 8
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_9Change
            end
            object TB_1_10: TTrackBar
              Left = 320
              Top = 24
              Width = 25
              Height = 250
              Hint = '160Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 9
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_10Change
            end
            object TB_1_11: TTrackBar
              Left = 352
              Top = 24
              Width = 25
              Height = 250
              Hint = '200Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 10
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_11Change
            end
            object TB_1_12: TTrackBar
              Left = 384
              Top = 24
              Width = 25
              Height = 250
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 11
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_12Change
            end
            object TB_1_13: TTrackBar
              Left = 416
              Top = 24
              Width = 25
              Height = 250
              Hint = '315Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 12
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_13Change
            end
            object TB_1_14: TTrackBar
              Left = 448
              Top = 24
              Width = 25
              Height = 250
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 13
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_14Change
            end
            object TB_1_15: TTrackBar
              Left = 480
              Top = 24
              Width = 25
              Height = 250
              Hint = '500Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 14
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_15Change
            end
            object TB_1_16: TTrackBar
              Left = 512
              Top = 24
              Width = 25
              Height = 250
              Hint = '630Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 15
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_16Change
            end
            object TB_1_17: TTrackBar
              Left = 544
              Top = 24
              Width = 25
              Height = 250
              Hint = '800Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 16
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_17Change
            end
            object TB_1_18: TTrackBar
              Left = 576
              Top = 24
              Width = 25
              Height = 250
              Hint = '1kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 17
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_18Change
            end
            object TB_1_19: TTrackBar
              Left = 608
              Top = 24
              Width = 25
              Height = 250
              Hint = '1.25kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 18
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_19Change
            end
            object TB_1_20: TTrackBar
              Left = 640
              Top = 24
              Width = 25
              Height = 250
              Hint = '1.6kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 19
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_20Change
            end
            object TB_1_21: TTrackBar
              Left = 672
              Top = 24
              Width = 25
              Height = 250
              Hint = '2kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 20
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_21Change
            end
            object TB_1_22: TTrackBar
              Left = 704
              Top = 24
              Width = 25
              Height = 250
              Hint = '2.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 21
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_22Change
            end
            object TB_1_23: TTrackBar
              Left = 736
              Top = 24
              Width = 25
              Height = 250
              Hint = '3.15kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 22
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_23Change
            end
            object TB_1_24: TTrackBar
              Left = 768
              Top = 24
              Width = 25
              Height = 250
              Hint = '4kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 23
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_24Change
            end
            object TB_1_25: TTrackBar
              Left = 800
              Top = 24
              Width = 25
              Height = 250
              Hint = '5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 24
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_25Change
            end
            object TB_1_26: TTrackBar
              Left = 832
              Top = 24
              Width = 25
              Height = 250
              Hint = '6.3kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 25
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_26Change
            end
            object TB_1_27: TTrackBar
              Left = 864
              Top = 24
              Width = 25
              Height = 250
              Hint = '8kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 26
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_27Change
            end
            object TB_1_28: TTrackBar
              Left = 896
              Top = 24
              Width = 25
              Height = 250
              Hint = '10kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 27
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_28Change
            end
            object TB_1_29: TTrackBar
              Left = 928
              Top = 24
              Width = 25
              Height = 250
              Hint = '12.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 28
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_29Change
            end
            object TB_1_30: TTrackBar
              Left = 960
              Top = 24
              Width = 25
              Height = 250
              Hint = '16kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 29
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_30Change
            end
            object TB_1_31: TTrackBar
              Left = 992
              Top = 24
              Width = 25
              Height = 250
              Hint = '20kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 30
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_31Change
            end
            object TB_1_32: TTrackBar
              Left = 1024
              Top = 24
              Width = 25
              Height = 250
              Hint = '25kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 31
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_32Change
            end
            object TB_1_33: TTrackBar
              Left = 1056
              Top = 24
              Width = 25
              Height = 250
              Hint = '31.5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 32
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_33Change
            end
            object TB_1_34: TTrackBar
              Left = 1088
              Top = 24
              Width = 25
              Height = 250
              Hint = '40kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 33
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_34Change
            end
            object TB_1_35: TTrackBar
              Left = 1120
              Top = 24
              Width = 25
              Height = 250
              Hint = '50kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 34
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_1_35Change
            end
          end
        end
        object btn_type1_all_center: TButton
          Left = 716
          Top = 375
          Width = 65
          Height = 25
          Caption = 'Reset EQ'
          TabOrder = 1
          OnClick = btn_type1_all_centerClick
        end
        object btn_type1_all_up: TButton
          Left = 8
          Top = 380
          Width = 33
          Height = 21
          Caption = #9650
          TabOrder = 2
          OnClick = btn_type1_all_upClick
        end
        object btn_type1_all_down: TButton
          Left = 48
          Top = 380
          Width = 33
          Height = 21
          Caption = #9660
          TabOrder = 3
          OnClick = btn_type1_all_downClick
        end
        object rdo_type1_value_x1: TRadioButton
          Left = 440
          Top = 384
          Width = 49
          Height = 17
          Caption = 'x1'
          Checked = True
          TabOrder = 4
          TabStop = True
        end
        object rdo_type1_value_x2: TRadioButton
          Left = 496
          Top = 384
          Width = 49
          Height = 17
          Caption = 'x2'
          TabOrder = 5
        end
        object cmbT1_range_from: TComboBox
          Left = 97
          Top = 379
          Width = 53
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 0
          TabOrder = 6
          Text = '---'
          OnChange = cmbT1_range_fromChange
          Items.Strings = (
            '---'
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
            '35')
        end
        object cmbT1_range_to: TComboBox
          Left = 166
          Top = 379
          Width = 53
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 0
          TabOrder = 7
          Text = '---'
          OnChange = cmbT1_range_toChange
          Items.Strings = (
            '---'
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
            '35')
        end
      end
    end
    object EQ_Type2: TTabSheet
      Caption = 'EQ(Type2)'
      ImageIndex = 1
      object GroupBox10: TGroupBox
        Left = 16
        Top = 16
        Width = 805
        Height = 417
        Caption = 'EQ(20Hz - 50kHz)'
        TabOrder = 0
        object Label174: TLabel
          Left = 392
          Top = 382
          Width = 43
          Height = 13
          Caption = 'EQ Value'
        end
        object Label210: TLabel
          Left = 156
          Top = 384
          Width = 4
          Height = 13
          Caption = '-'
        end
        object T2_range_label: TLabel
          Left = 232
          Top = 384
          Width = 3
          Height = 13
        end
        object t2_scrbox: TScrollBox
          Left = 8
          Top = 24
          Width = 773
          Height = 345
          HorzScrollBar.Position = 879
          VertScrollBar.Visible = False
          TabOrder = 0
          object Panel2: TPanel
            Left = -879
            Top = 0
            Width = 1648
            Height = 321
            TabOrder = 0
            object Label114: TLabel
              Left = 24
              Top = 288
              Width = 21
              Height = 11
              Caption = '20Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label121: TLabel
              Left = 56
              Top = 288
              Width = 21
              Height = 11
              Caption = '25Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label122: TLabel
              Left = 88
              Top = 288
              Width = 21
              Height = 11
              Caption = '31Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label115: TLabel
              Left = 120
              Top = 288
              Width = 21
              Height = 11
              Caption = '40Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label123: TLabel
              Left = 152
              Top = 288
              Width = 21
              Height = 11
              Caption = '50Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label124: TLabel
              Left = 184
              Top = 288
              Width = 21
              Height = 11
              Caption = '63Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label116: TLabel
              Left = 216
              Top = 288
              Width = 21
              Height = 11
              Caption = '80Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label125: TLabel
              Left = 248
              Top = 288
              Width = 26
              Height = 11
              Caption = '100Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label126: TLabel
              Left = 280
              Top = 288
              Width = 26
              Height = 11
              Caption = '125Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label117: TLabel
              Left = 312
              Top = 288
              Width = 26
              Height = 11
              Caption = '160Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label127: TLabel
              Left = 344
              Top = 288
              Width = 26
              Height = 11
              Caption = '200Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label128: TLabel
              Left = 376
              Top = 288
              Width = 26
              Height = 11
              Caption = '250Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label118: TLabel
              Left = 408
              Top = 288
              Width = 26
              Height = 11
              Caption = '315Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label129: TLabel
              Left = 440
              Top = 288
              Width = 26
              Height = 11
              Caption = '400Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label130: TLabel
              Left = 472
              Top = 288
              Width = 26
              Height = 11
              Caption = '500Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label119: TLabel
              Left = 504
              Top = 288
              Width = 26
              Height = 11
              Caption = '630Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label120: TLabel
              Left = 536
              Top = 288
              Width = 26
              Height = 11
              Caption = '800Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label131: TLabel
              Left = 568
              Top = 288
              Width = 21
              Height = 11
              Caption = '1kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label138: TLabel
              Left = 592
              Top = 288
              Width = 34
              Height = 11
              Caption = '1.25kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label139: TLabel
              Left = 632
              Top = 288
              Width = 29
              Height = 11
              Caption = '1.6kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label132: TLabel
              Left = 664
              Top = 288
              Width = 21
              Height = 11
              Caption = '2kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label140: TLabel
              Left = 688
              Top = 288
              Width = 29
              Height = 11
              Caption = '2.5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label141: TLabel
              Left = 720
              Top = 288
              Width = 34
              Height = 11
              Caption = '3.15kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label133: TLabel
              Left = 760
              Top = 288
              Width = 21
              Height = 11
              Caption = '4kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label142: TLabel
              Left = 792
              Top = 288
              Width = 21
              Height = 11
              Caption = '5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label143: TLabel
              Left = 824
              Top = 288
              Width = 21
              Height = 11
              Caption = '6kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label134: TLabel
              Left = 856
              Top = 288
              Width = 21
              Height = 11
              Caption = '7kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label144: TLabel
              Left = 888
              Top = 288
              Width = 21
              Height = 11
              Caption = '8kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label145: TLabel
              Left = 920
              Top = 288
              Width = 21
              Height = 11
              Caption = '9kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label135: TLabel
              Left = 952
              Top = 288
              Width = 26
              Height = 11
              Caption = '10kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label146: TLabel
              Left = 984
              Top = 288
              Width = 26
              Height = 11
              Caption = '11kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label147: TLabel
              Left = 1016
              Top = 288
              Width = 26
              Height = 11
              Caption = '12kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label136: TLabel
              Left = 1048
              Top = 288
              Width = 26
              Height = 11
              Caption = '13kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label137: TLabel
              Left = 1080
              Top = 288
              Width = 26
              Height = 11
              Caption = '14kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label148: TLabel
              Left = 1112
              Top = 288
              Width = 26
              Height = 11
              Caption = '15kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label155: TLabel
              Left = 1144
              Top = 288
              Width = 26
              Height = 11
              Caption = '16kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label156: TLabel
              Left = 1176
              Top = 288
              Width = 26
              Height = 11
              Caption = '17kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label149: TLabel
              Left = 1208
              Top = 288
              Width = 26
              Height = 11
              Caption = '18kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label157: TLabel
              Left = 1240
              Top = 288
              Width = 26
              Height = 11
              Caption = '19kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label158: TLabel
              Left = 1272
              Top = 288
              Width = 26
              Height = 11
              Caption = '20kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label150: TLabel
              Left = 1304
              Top = 288
              Width = 26
              Height = 11
              Caption = '21kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label159: TLabel
              Left = 1336
              Top = 288
              Width = 26
              Height = 11
              Caption = '22kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label160: TLabel
              Left = 1368
              Top = 288
              Width = 26
              Height = 11
              Caption = '23kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label151: TLabel
              Left = 1400
              Top = 288
              Width = 26
              Height = 11
              Caption = '24kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label161: TLabel
              Left = 1432
              Top = 288
              Width = 26
              Height = 11
              Caption = '25kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label162: TLabel
              Left = 1464
              Top = 288
              Width = 26
              Height = 11
              Caption = '30kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label152: TLabel
              Left = 1496
              Top = 288
              Width = 26
              Height = 11
              Caption = '35kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label163: TLabel
              Left = 1528
              Top = 288
              Width = 26
              Height = 11
              Caption = '40kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label164: TLabel
              Left = 1560
              Top = 288
              Width = 26
              Height = 11
              Caption = '45kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label153: TLabel
              Left = 1592
              Top = 288
              Width = 26
              Height = 11
              Caption = '50kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object eq_2_1: TLabel
              Left = 36
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_2: TLabel
              Left = 68
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_3: TLabel
              Left = 96
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_4: TLabel
              Left = 132
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_5: TLabel
              Left = 164
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_6: TLabel
              Left = 196
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_7: TLabel
              Left = 230
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_8: TLabel
              Left = 260
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_9: TLabel
              Left = 290
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_10: TLabel
              Left = 322
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_11: TLabel
              Left = 356
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_12: TLabel
              Left = 386
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_13: TLabel
              Left = 418
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_14: TLabel
              Left = 452
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_15: TLabel
              Left = 484
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_16: TLabel
              Left = 514
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_17: TLabel
              Left = 547
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_18: TLabel
              Left = 580
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_19: TLabel
              Left = 612
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_20: TLabel
              Left = 644
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_21: TLabel
              Left = 676
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_22: TLabel
              Left = 708
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_23: TLabel
              Left = 740
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_24: TLabel
              Left = 774
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_25: TLabel
              Left = 804
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_26: TLabel
              Left = 834
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_27: TLabel
              Left = 866
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_28: TLabel
              Left = 900
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_29: TLabel
              Left = 930
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_30: TLabel
              Left = 962
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_31: TLabel
              Left = 996
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_32: TLabel
              Left = 1028
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_33: TLabel
              Left = 1058
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_34: TLabel
              Left = 1092
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_35: TLabel
              Left = 1124
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_50: TLabel
              Left = 1604
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_49: TLabel
              Left = 1572
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_48: TLabel
              Left = 1538
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_47: TLabel
              Left = 1508
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_46: TLabel
              Left = 1476
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_45: TLabel
              Left = 1442
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_44: TLabel
              Left = 1410
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_43: TLabel
              Left = 1380
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_42: TLabel
              Left = 1346
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_41: TLabel
              Left = 1314
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_40: TLabel
              Left = 1284
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_39: TLabel
              Left = 1254
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_38: TLabel
              Left = 1220
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_37: TLabel
              Left = 1188
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_2_36: TLabel
              Left = 1156
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Label170: TLabel
              Left = 16
              Top = 140
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Bevel9: TBevel
              Left = 16
              Top = 280
              Width = 265
              Height = 26
            end
            object Bevel10: TBevel
              Left = 592
              Top = 280
              Width = 713
              Height = 25
            end
            object TB_2_1: TTrackBar
              Left = 24
              Top = 24
              Width = 25
              Height = 250
              Hint = '20Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              PageSize = 1
              Frequency = 2
              TabOrder = 0
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_1Change
            end
            object TB_2_2: TTrackBar
              Left = 56
              Top = 24
              Width = 25
              Height = 250
              Hint = '25Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 1
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_2Change
            end
            object TB_2_3: TTrackBar
              Left = 88
              Top = 24
              Width = 25
              Height = 250
              Hint = '31Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 2
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_3Change
            end
            object TB_2_4: TTrackBar
              Left = 120
              Top = 24
              Width = 25
              Height = 250
              Hint = '40Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 3
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_4Change
            end
            object TB_2_5: TTrackBar
              Left = 152
              Top = 24
              Width = 25
              Height = 250
              Hint = '50Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 4
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_5Change
            end
            object TB_2_6: TTrackBar
              Left = 184
              Top = 24
              Width = 25
              Height = 250
              Hint = '63Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 5
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_6Change
            end
            object TB_2_7: TTrackBar
              Left = 216
              Top = 24
              Width = 25
              Height = 250
              Hint = '80Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 6
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_7Change
            end
            object TB_2_8: TTrackBar
              Left = 248
              Top = 24
              Width = 25
              Height = 250
              Hint = '100Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 7
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_8Change
            end
            object TB_2_9: TTrackBar
              Left = 280
              Top = 24
              Width = 25
              Height = 250
              Hint = '125Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 8
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_9Change
            end
            object TB_2_10: TTrackBar
              Left = 312
              Top = 24
              Width = 25
              Height = 250
              Hint = '160Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 9
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_10Change
            end
            object TB_2_11: TTrackBar
              Left = 344
              Top = 24
              Width = 25
              Height = 250
              Hint = '200Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 10
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_11Change
            end
            object TB_2_12: TTrackBar
              Left = 376
              Top = 24
              Width = 25
              Height = 250
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 11
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_12Change
            end
            object TB_2_13: TTrackBar
              Left = 408
              Top = 24
              Width = 25
              Height = 250
              Hint = '315Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 12
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_13Change
            end
            object TB_2_14: TTrackBar
              Left = 440
              Top = 24
              Width = 25
              Height = 250
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 13
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_14Change
            end
            object TB_2_15: TTrackBar
              Left = 472
              Top = 24
              Width = 25
              Height = 250
              Hint = '500Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 14
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_15Change
            end
            object TB_2_16: TTrackBar
              Left = 504
              Top = 24
              Width = 25
              Height = 250
              Hint = '630Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 15
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_16Change
            end
            object TB_2_17: TTrackBar
              Left = 536
              Top = 24
              Width = 25
              Height = 250
              Hint = '800Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 16
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_17Change
            end
            object TB_2_18: TTrackBar
              Left = 568
              Top = 24
              Width = 25
              Height = 250
              Hint = '20Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              PageSize = 1
              Frequency = 2
              TabOrder = 17
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_18Change
            end
            object TB_2_19: TTrackBar
              Left = 600
              Top = 24
              Width = 25
              Height = 250
              Hint = '25Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 18
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_19Change
            end
            object TB_2_20: TTrackBar
              Left = 632
              Top = 24
              Width = 25
              Height = 250
              Hint = '31Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 19
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_20Change
            end
            object TB_2_21: TTrackBar
              Left = 664
              Top = 24
              Width = 25
              Height = 250
              Hint = '40Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 20
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_21Change
            end
            object TB_2_22: TTrackBar
              Left = 696
              Top = 24
              Width = 25
              Height = 250
              Hint = '50Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 21
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_22Change
            end
            object TB_2_23: TTrackBar
              Left = 728
              Top = 24
              Width = 25
              Height = 250
              Hint = '63Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 22
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_23Change
            end
            object TB_2_24: TTrackBar
              Left = 760
              Top = 24
              Width = 25
              Height = 250
              Hint = '80Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 23
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_24Change
            end
            object TB_2_25: TTrackBar
              Left = 792
              Top = 24
              Width = 25
              Height = 250
              Hint = '100Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 24
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_25Change
            end
            object TB_2_26: TTrackBar
              Left = 824
              Top = 24
              Width = 25
              Height = 250
              Hint = '125Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 25
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_26Change
            end
            object TB_2_27: TTrackBar
              Left = 856
              Top = 24
              Width = 25
              Height = 250
              Hint = '160Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 26
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_27Change
            end
            object TB_2_28: TTrackBar
              Left = 888
              Top = 24
              Width = 25
              Height = 250
              Hint = '200Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 27
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_28Change
            end
            object TB_2_29: TTrackBar
              Left = 920
              Top = 24
              Width = 25
              Height = 250
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 28
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_29Change
            end
            object TB_2_30: TTrackBar
              Left = 952
              Top = 24
              Width = 25
              Height = 250
              Hint = '315Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 29
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_30Change
            end
            object TB_2_31: TTrackBar
              Left = 984
              Top = 24
              Width = 25
              Height = 250
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 30
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_31Change
            end
            object TB_2_32: TTrackBar
              Left = 1016
              Top = 24
              Width = 25
              Height = 250
              Hint = '500Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 31
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_32Change
            end
            object TB_2_33: TTrackBar
              Left = 1048
              Top = 24
              Width = 25
              Height = 250
              Hint = '630Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 32
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_33Change
            end
            object TB_2_34: TTrackBar
              Left = 1080
              Top = 24
              Width = 25
              Height = 250
              Hint = '800Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 33
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_34Change
            end
            object TB_2_35: TTrackBar
              Left = 1112
              Top = 24
              Width = 25
              Height = 250
              Hint = '20Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              PageSize = 1
              Frequency = 2
              TabOrder = 34
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_35Change
            end
            object TB_2_36: TTrackBar
              Left = 1144
              Top = 24
              Width = 25
              Height = 250
              Hint = '25Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 35
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_36Change
            end
            object TB_2_37: TTrackBar
              Left = 1176
              Top = 24
              Width = 25
              Height = 250
              Hint = '31Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 36
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_37Change
            end
            object TB_2_38: TTrackBar
              Left = 1208
              Top = 24
              Width = 25
              Height = 250
              Hint = '40Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 37
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_38Change
            end
            object TB_2_39: TTrackBar
              Left = 1240
              Top = 24
              Width = 25
              Height = 250
              Hint = '50Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 38
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_39Change
            end
            object TB_2_40: TTrackBar
              Left = 1272
              Top = 24
              Width = 25
              Height = 250
              Hint = '63Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 39
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_40Change
            end
            object TB_2_41: TTrackBar
              Left = 1304
              Top = 24
              Width = 25
              Height = 250
              Hint = '80Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 40
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_41Change
            end
            object TB_2_42: TTrackBar
              Left = 1336
              Top = 24
              Width = 25
              Height = 250
              Hint = '100Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 41
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_42Change
            end
            object TB_2_43: TTrackBar
              Left = 1368
              Top = 24
              Width = 25
              Height = 250
              Hint = '125Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 42
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_43Change
            end
            object TB_2_44: TTrackBar
              Left = 1400
              Top = 24
              Width = 25
              Height = 250
              Hint = '160Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 43
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_44Change
            end
            object TB_2_45: TTrackBar
              Left = 1432
              Top = 24
              Width = 25
              Height = 250
              Hint = '200Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 44
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_45Change
            end
            object TB_2_46: TTrackBar
              Left = 1464
              Top = 24
              Width = 25
              Height = 250
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 45
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_46Change
            end
            object TB_2_47: TTrackBar
              Left = 1496
              Top = 24
              Width = 25
              Height = 250
              Hint = '315Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 46
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_47Change
            end
            object TB_2_48: TTrackBar
              Left = 1528
              Top = 24
              Width = 25
              Height = 250
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 47
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_48Change
            end
            object TB_2_49: TTrackBar
              Left = 1560
              Top = 24
              Width = 25
              Height = 250
              Hint = '500Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 48
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_49Change
            end
            object TB_2_50: TTrackBar
              Left = 1592
              Top = 24
              Width = 25
              Height = 250
              Hint = '500Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 49
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_2_50Change
            end
          end
        end
        object btn_type2_all_down: TButton
          Left = 48
          Top = 380
          Width = 33
          Height = 20
          Caption = #9660
          TabOrder = 1
          OnClick = btn_type2_all_downClick
        end
        object btn_type2_all_center: TButton
          Left = 716
          Top = 375
          Width = 65
          Height = 25
          Caption = 'Reset EQ'
          TabOrder = 2
          OnClick = btn_type2_all_centerClick
        end
        object btn_type2_all_up: TButton
          Left = 8
          Top = 380
          Width = 33
          Height = 20
          Caption = #9650
          TabOrder = 3
          OnClick = btn_type2_all_upClick
        end
        object btn_type2_copy_from_type1: TButton
          Left = 551
          Top = 375
          Width = 129
          Height = 25
          Caption = 'Copy from EQ(Type1)'
          TabOrder = 4
          OnClick = btn_type2_copy_from_type1Click
        end
        object rdo_type2_value_x1: TRadioButton
          Left = 440
          Top = 380
          Width = 49
          Height = 17
          Caption = 'x1'
          Checked = True
          TabOrder = 5
          TabStop = True
        end
        object rdo_type2_value_x2: TRadioButton
          Left = 496
          Top = 380
          Width = 49
          Height = 17
          Caption = 'x2'
          TabOrder = 6
        end
        object cmbT2_range_from: TComboBox
          Left = 97
          Top = 379
          Width = 53
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 0
          TabOrder = 7
          Text = '---'
          OnChange = cmbT2_range_fromChange
          Items.Strings = (
            '---'
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
            '50')
        end
        object cmbT2_range_to: TComboBox
          Left = 166
          Top = 379
          Width = 53
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 0
          TabOrder = 8
          Text = '---'
          OnChange = cmbT2_range_toChange
          Items.Strings = (
            '---'
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
            '50')
        end
      end
    end
    object EQ_Type3: TTabSheet
      Caption = 'EQ(Type3)'
      ImageIndex = 2
      object GroupBox3: TGroupBox
        Left = 16
        Top = 16
        Width = 805
        Height = 417
        Caption = 'EQ(5Hz - 100kHz)'
        TabOrder = 0
        object Label175: TLabel
          Left = 392
          Top = 382
          Width = 43
          Height = 13
          Caption = 'EQ Value'
        end
        object Label211: TLabel
          Left = 156
          Top = 384
          Width = 4
          Height = 13
          Caption = '-'
        end
        object T3_range_label: TLabel
          Left = 232
          Top = 384
          Width = 3
          Height = 13
        end
        object t3_scrbox: TScrollBox
          Left = 8
          Top = 25
          Width = 773
          Height = 345
          HorzScrollBar.Position = 1773
          TabOrder = 0
          object Panel4: TPanel
            Left = -1775
            Top = 3
            Width = 2544
            Height = 321
            TabOrder = 0
            object Label18: TLabel
              Left = 24
              Top = 286
              Width = 16
              Height = 11
              Caption = '5Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label56: TLabel
              Left = 56
              Top = 286
              Width = 16
              Height = 11
              Caption = '6Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label57: TLabel
              Left = 88
              Top = 286
              Width = 16
              Height = 11
              Caption = '8Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label58: TLabel
              Left = 120
              Top = 286
              Width = 21
              Height = 11
              Caption = '10Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label19: TLabel
              Left = 152
              Top = 286
              Width = 21
              Height = 11
              Caption = '12Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label20: TLabel
              Left = 184
              Top = 286
              Width = 21
              Height = 11
              Caption = '16Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label21: TLabel
              Left = 216
              Top = 286
              Width = 21
              Height = 11
              Caption = '20Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label22: TLabel
              Left = 248
              Top = 286
              Width = 21
              Height = 11
              Caption = '25Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label23: TLabel
              Left = 280
              Top = 286
              Width = 21
              Height = 11
              Caption = '31Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label26: TLabel
              Left = 312
              Top = 286
              Width = 21
              Height = 11
              Caption = '40Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label59: TLabel
              Left = 344
              Top = 286
              Width = 21
              Height = 11
              Caption = '50Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label60: TLabel
              Left = 376
              Top = 286
              Width = 21
              Height = 11
              Caption = '63Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label61: TLabel
              Left = 408
              Top = 286
              Width = 21
              Height = 11
              Caption = '80Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label62: TLabel
              Left = 440
              Top = 286
              Width = 26
              Height = 11
              Caption = '100Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label63: TLabel
              Left = 472
              Top = 286
              Width = 26
              Height = 11
              Caption = '125Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label64: TLabel
              Left = 504
              Top = 286
              Width = 26
              Height = 11
              Caption = '160Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label65: TLabel
              Left = 536
              Top = 286
              Width = 26
              Height = 11
              Caption = '200Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label66: TLabel
              Left = 568
              Top = 286
              Width = 26
              Height = 11
              Caption = '250Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label27: TLabel
              Left = 600
              Top = 288
              Width = 26
              Height = 11
              Caption = '315Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label67: TLabel
              Left = 632
              Top = 288
              Width = 26
              Height = 11
              Caption = '400Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label68: TLabel
              Left = 664
              Top = 288
              Width = 26
              Height = 11
              Caption = '500Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label28: TLabel
              Left = 696
              Top = 288
              Width = 26
              Height = 11
              Caption = '630Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label29: TLabel
              Left = 728
              Top = 288
              Width = 26
              Height = 11
              Caption = '800Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label30: TLabel
              Left = 760
              Top = 288
              Width = 21
              Height = 11
              Caption = '1kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label31: TLabel
              Left = 784
              Top = 288
              Width = 34
              Height = 11
              Caption = '1.25kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label32: TLabel
              Left = 824
              Top = 288
              Width = 29
              Height = 11
              Caption = '1.6kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label33: TLabel
              Left = 856
              Top = 288
              Width = 21
              Height = 11
              Caption = '2kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label69: TLabel
              Left = 880
              Top = 288
              Width = 29
              Height = 11
              Caption = '2.5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label70: TLabel
              Left = 912
              Top = 288
              Width = 34
              Height = 11
              Caption = '3.15kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label71: TLabel
              Left = 952
              Top = 288
              Width = 21
              Height = 11
              Caption = '4kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label72: TLabel
              Left = 984
              Top = 288
              Width = 21
              Height = 11
              Caption = '5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label73: TLabel
              Left = 1016
              Top = 288
              Width = 21
              Height = 11
              Caption = '6kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label74: TLabel
              Left = 1048
              Top = 288
              Width = 21
              Height = 11
              Caption = '7kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label75: TLabel
              Left = 1080
              Top = 288
              Width = 21
              Height = 11
              Caption = '8kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label76: TLabel
              Left = 1112
              Top = 288
              Width = 21
              Height = 11
              Caption = '9kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label77: TLabel
              Left = 1144
              Top = 288
              Width = 26
              Height = 11
              Caption = '10kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label78: TLabel
              Left = 1176
              Top = 288
              Width = 26
              Height = 11
              Caption = '11kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label80: TLabel
              Left = 1208
              Top = 288
              Width = 26
              Height = 11
              Caption = '12kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label81: TLabel
              Left = 1240
              Top = 288
              Width = 26
              Height = 11
              Caption = '13kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label79: TLabel
              Left = 1272
              Top = 288
              Width = 26
              Height = 11
              Caption = '14kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label82: TLabel
              Left = 1304
              Top = 288
              Width = 26
              Height = 11
              Caption = '15kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label83: TLabel
              Left = 1336
              Top = 288
              Width = 26
              Height = 11
              Caption = '16kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label84: TLabel
              Left = 1368
              Top = 288
              Width = 26
              Height = 11
              Caption = '17kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label85: TLabel
              Left = 1400
              Top = 288
              Width = 26
              Height = 11
              Caption = '18kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label86: TLabel
              Left = 1432
              Top = 288
              Width = 26
              Height = 11
              Caption = '19kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label87: TLabel
              Left = 1464
              Top = 288
              Width = 26
              Height = 11
              Caption = '20kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label88: TLabel
              Left = 1496
              Top = 288
              Width = 26
              Height = 11
              Caption = '21kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label89: TLabel
              Left = 1528
              Top = 288
              Width = 26
              Height = 11
              Caption = '22kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label90: TLabel
              Left = 1560
              Top = 288
              Width = 26
              Height = 11
              Caption = '23kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label91: TLabel
              Left = 1592
              Top = 288
              Width = 26
              Height = 11
              Caption = '24kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label92: TLabel
              Left = 1624
              Top = 288
              Width = 26
              Height = 11
              Caption = '25kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label93: TLabel
              Left = 1656
              Top = 288
              Width = 26
              Height = 11
              Caption = '26kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label94: TLabel
              Left = 1688
              Top = 288
              Width = 26
              Height = 11
              Caption = '27kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label95: TLabel
              Left = 1720
              Top = 288
              Width = 26
              Height = 11
              Caption = '28kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label96: TLabel
              Left = 1752
              Top = 288
              Width = 26
              Height = 11
              Caption = '29kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label98: TLabel
              Left = 1784
              Top = 288
              Width = 26
              Height = 11
              Caption = '30kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label99: TLabel
              Left = 1816
              Top = 288
              Width = 26
              Height = 11
              Caption = '31kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label97: TLabel
              Left = 1848
              Top = 288
              Width = 26
              Height = 11
              Caption = '32kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label100: TLabel
              Left = 1880
              Top = 288
              Width = 26
              Height = 11
              Caption = '33kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label101: TLabel
              Left = 1912
              Top = 288
              Width = 26
              Height = 11
              Caption = '34kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label102: TLabel
              Left = 1944
              Top = 288
              Width = 26
              Height = 11
              Caption = '35kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label103: TLabel
              Left = 1976
              Top = 288
              Width = 26
              Height = 11
              Caption = '36kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label104: TLabel
              Left = 2008
              Top = 288
              Width = 26
              Height = 11
              Caption = '37kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label105: TLabel
              Left = 2040
              Top = 288
              Width = 26
              Height = 11
              Caption = '38kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label106: TLabel
              Left = 2072
              Top = 288
              Width = 26
              Height = 11
              Caption = '39kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label107: TLabel
              Left = 2104
              Top = 288
              Width = 26
              Height = 11
              Caption = '40kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label108: TLabel
              Left = 2136
              Top = 288
              Width = 26
              Height = 11
              Caption = '45kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label109: TLabel
              Left = 2168
              Top = 288
              Width = 26
              Height = 11
              Caption = '50kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label110: TLabel
              Left = 2200
              Top = 288
              Width = 26
              Height = 11
              Caption = '55kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label111: TLabel
              Left = 2232
              Top = 288
              Width = 26
              Height = 11
              Caption = '60kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label112: TLabel
              Left = 2264
              Top = 288
              Width = 26
              Height = 11
              Caption = '65kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label113: TLabel
              Left = 2296
              Top = 288
              Width = 26
              Height = 11
              Caption = '70kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object eq_3_1: TLabel
              Left = 36
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_2: TLabel
              Left = 68
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_3: TLabel
              Left = 96
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_4: TLabel
              Left = 132
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_5: TLabel
              Left = 164
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_6: TLabel
              Left = 196
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_7: TLabel
              Left = 230
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_8: TLabel
              Left = 260
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_9: TLabel
              Left = 290
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_10: TLabel
              Left = 322
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_11: TLabel
              Left = 356
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_12: TLabel
              Left = 388
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_13: TLabel
              Left = 416
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_14: TLabel
              Left = 452
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_15: TLabel
              Left = 484
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_16: TLabel
              Left = 516
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_17: TLabel
              Left = 550
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_18: TLabel
              Left = 580
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_19: TLabel
              Left = 610
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_20: TLabel
              Left = 642
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_21: TLabel
              Left = 676
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_22: TLabel
              Left = 708
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_23: TLabel
              Left = 736
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_24: TLabel
              Left = 772
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_25: TLabel
              Left = 804
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_26: TLabel
              Left = 836
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_27: TLabel
              Left = 870
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_28: TLabel
              Left = 900
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_29: TLabel
              Left = 930
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_30: TLabel
              Left = 962
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_31: TLabel
              Left = 996
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_32: TLabel
              Left = 1028
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_33: TLabel
              Left = 1056
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_34: TLabel
              Left = 1092
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_35: TLabel
              Left = 1124
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_36: TLabel
              Left = 1156
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_37: TLabel
              Left = 1190
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_38: TLabel
              Left = 1220
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_39: TLabel
              Left = 1250
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_40: TLabel
              Left = 1282
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_41: TLabel
              Left = 1316
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_42: TLabel
              Left = 1348
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_43: TLabel
              Left = 1376
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_44: TLabel
              Left = 1412
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_45: TLabel
              Left = 1444
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_46: TLabel
              Left = 1476
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_47: TLabel
              Left = 1510
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_48: TLabel
              Left = 1540
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_49: TLabel
              Left = 1570
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_50: TLabel
              Left = 1602
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_51: TLabel
              Left = 1636
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_52: TLabel
              Left = 1668
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_53: TLabel
              Left = 1696
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_54: TLabel
              Left = 1732
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_55: TLabel
              Left = 1764
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_56: TLabel
              Left = 1796
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_57: TLabel
              Left = 1830
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_58: TLabel
              Left = 1860
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_59: TLabel
              Left = 1890
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_60: TLabel
              Left = 1922
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_61: TLabel
              Left = 1956
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_62: TLabel
              Left = 1988
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_63: TLabel
              Left = 2016
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_64: TLabel
              Left = 2052
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_65: TLabel
              Left = 2084
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_66: TLabel
              Left = 2116
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_67: TLabel
              Left = 2150
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_68: TLabel
              Left = 2180
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_69: TLabel
              Left = 2210
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_70: TLabel
              Left = 2242
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Label154: TLabel
              Left = 2328
              Top = 288
              Width = 26
              Height = 11
              Caption = '75kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label165: TLabel
              Left = 2360
              Top = 288
              Width = 26
              Height = 11
              Caption = '80kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label166: TLabel
              Left = 2392
              Top = 288
              Width = 26
              Height = 11
              Caption = '85kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label167: TLabel
              Left = 2424
              Top = 288
              Width = 26
              Height = 11
              Caption = '90kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label168: TLabel
              Left = 2456
              Top = 288
              Width = 26
              Height = 11
              Caption = '95kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label169: TLabel
              Left = 2488
              Top = 288
              Width = 31
              Height = 11
              Caption = '100kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object eq_3_71: TLabel
              Left = 2272
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_72: TLabel
              Left = 2308
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_73: TLabel
              Left = 2340
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_74: TLabel
              Left = 2372
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_75: TLabel
              Left = 2406
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_76: TLabel
              Left = 2436
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_77: TLabel
              Left = 2466
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_3_78: TLabel
              Left = 2498
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Label172: TLabel
              Left = 16
              Top = 140
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Bevel4: TBevel
              Left = 16
              Top = 280
              Width = 450
              Height = 24
            end
            object Bevel5: TBevel
              Left = 784
              Top = 281
              Width = 706
              Height = 26
            end
            object TB_3_1: TTrackBar
              Left = 24
              Top = 24
              Width = 25
              Height = 250
              Hint = '5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              PageSize = 1
              Frequency = 2
              TabOrder = 0
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_1Change
            end
            object TB_3_2: TTrackBar
              Left = 56
              Top = 24
              Width = 25
              Height = 250
              Hint = '10Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 1
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_2Change
            end
            object TB_3_3: TTrackBar
              Left = 88
              Top = 24
              Width = 25
              Height = 250
              Hint = '20Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 2
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_3Change
            end
            object TB_3_4: TTrackBar
              Left = 120
              Top = 24
              Width = 25
              Height = 250
              Hint = '30Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 3
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_4Change
            end
            object TB_3_5: TTrackBar
              Left = 152
              Top = 24
              Width = 25
              Height = 250
              Hint = '40Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 4
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_5Change
            end
            object TB_3_6: TTrackBar
              Left = 184
              Top = 24
              Width = 25
              Height = 250
              Hint = '50Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 5
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_6Change
            end
            object TB_3_7: TTrackBar
              Left = 216
              Top = 24
              Width = 25
              Height = 250
              Hint = '60Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 6
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_7Change
            end
            object TB_3_8: TTrackBar
              Left = 248
              Top = 24
              Width = 25
              Height = 250
              Hint = '70Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 7
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_8Change
            end
            object TB_3_9: TTrackBar
              Left = 280
              Top = 24
              Width = 25
              Height = 250
              Hint = '80Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 8
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_9Change
            end
            object TB_3_10: TTrackBar
              Left = 312
              Top = 24
              Width = 25
              Height = 250
              Hint = '90Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 9
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_10Change
            end
            object TB_3_11: TTrackBar
              Left = 344
              Top = 24
              Width = 25
              Height = 250
              Hint = '100Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 10
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_11Change
            end
            object TB_3_12: TTrackBar
              Left = 376
              Top = 24
              Width = 25
              Height = 250
              Hint = '110Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 11
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_12Change
            end
            object TB_3_13: TTrackBar
              Left = 408
              Top = 24
              Width = 25
              Height = 250
              Hint = '120Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 12
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_13Change
            end
            object TB_3_14: TTrackBar
              Left = 440
              Top = 24
              Width = 25
              Height = 250
              Hint = '130Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 13
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_14Change
            end
            object TB_3_15: TTrackBar
              Left = 472
              Top = 24
              Width = 25
              Height = 250
              Hint = '140Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 14
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_15Change
            end
            object TB_3_16: TTrackBar
              Left = 504
              Top = 24
              Width = 25
              Height = 250
              Hint = '150Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 15
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_16Change
            end
            object TB_3_17: TTrackBar
              Left = 536
              Top = 24
              Width = 25
              Height = 250
              Hint = '175Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 16
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_17Change
            end
            object TB_3_18: TTrackBar
              Left = 568
              Top = 24
              Width = 25
              Height = 250
              Hint = '200Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 17
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_18Change
            end
            object TB_3_19: TTrackBar
              Left = 600
              Top = 24
              Width = 25
              Height = 250
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 18
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_19Change
            end
            object TB_3_20: TTrackBar
              Left = 632
              Top = 24
              Width = 25
              Height = 250
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 19
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_20Change
            end
            object TB_3_21: TTrackBar
              Left = 664
              Top = 24
              Width = 25
              Height = 250
              Hint = '550Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 20
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_21Change
            end
            object TB_3_22: TTrackBar
              Left = 696
              Top = 24
              Width = 25
              Height = 250
              Hint = '700Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 21
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_22Change
            end
            object TB_3_23: TTrackBar
              Left = 728
              Top = 24
              Width = 25
              Height = 250
              Hint = '850Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 22
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_23Change
            end
            object TB_3_24: TTrackBar
              Left = 760
              Top = 24
              Width = 25
              Height = 250
              Hint = '1kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 23
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_24Change
            end
            object TB_3_25: TTrackBar
              Left = 792
              Top = 24
              Width = 25
              Height = 250
              Hint = '1.5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 24
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_25Change
            end
            object TB_3_26: TTrackBar
              Left = 824
              Top = 24
              Width = 25
              Height = 250
              Hint = '2kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 25
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_26Change
            end
            object TB_3_27: TTrackBar
              Left = 856
              Top = 24
              Width = 25
              Height = 250
              Hint = '2.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 26
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_27Change
            end
            object TB_3_28: TTrackBar
              Left = 888
              Top = 24
              Width = 25
              Height = 250
              Hint = '3kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 27
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_28Change
            end
            object TB_3_29: TTrackBar
              Left = 920
              Top = 24
              Width = 25
              Height = 250
              Hint = '3.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 28
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_29Change
            end
            object TB_3_30: TTrackBar
              Left = 952
              Top = 24
              Width = 25
              Height = 250
              Hint = '4kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 29
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_30Change
            end
            object TB_3_31: TTrackBar
              Left = 984
              Top = 24
              Width = 25
              Height = 250
              Hint = '4.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 30
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_31Change
            end
            object TB_3_32: TTrackBar
              Left = 1016
              Top = 24
              Width = 25
              Height = 250
              Hint = '5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 31
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_32Change
            end
            object TB_3_33: TTrackBar
              Left = 1048
              Top = 24
              Width = 25
              Height = 250
              Hint = '5.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 32
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_33Change
            end
            object TB_3_34: TTrackBar
              Left = 1080
              Top = 24
              Width = 25
              Height = 250
              Hint = '6kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 33
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_34Change
            end
            object TB_3_35: TTrackBar
              Left = 1112
              Top = 24
              Width = 25
              Height = 250
              Hint = '6.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 34
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_35Change
            end
            object TB_3_36: TTrackBar
              Left = 1144
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 35
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_36Change
            end
            object TB_3_37: TTrackBar
              Left = 1176
              Top = 24
              Width = 25
              Height = 250
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 36
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_37Change
            end
            object TB_3_38: TTrackBar
              Left = 1208
              Top = 24
              Width = 25
              Height = 250
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 37
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_38Change
            end
            object TB_3_39: TTrackBar
              Left = 1240
              Top = 24
              Width = 25
              Height = 250
              Hint = '550Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 38
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_39Change
            end
            object TB_3_40: TTrackBar
              Left = 1272
              Top = 24
              Width = 25
              Height = 250
              Hint = '700Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 39
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_40Change
            end
            object TB_3_41: TTrackBar
              Left = 1304
              Top = 24
              Width = 25
              Height = 250
              Hint = '850Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 40
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_41Change
            end
            object TB_3_42: TTrackBar
              Left = 1336
              Top = 24
              Width = 25
              Height = 250
              Hint = '1kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 41
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_42Change
            end
            object TB_3_43: TTrackBar
              Left = 1368
              Top = 24
              Width = 25
              Height = 250
              Hint = '1.5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 42
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_43Change
            end
            object TB_3_44: TTrackBar
              Left = 1400
              Top = 24
              Width = 25
              Height = 250
              Hint = '2kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 43
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_44Change
            end
            object TB_3_45: TTrackBar
              Left = 1432
              Top = 24
              Width = 25
              Height = 250
              Hint = '2.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 44
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_45Change
            end
            object TB_3_46: TTrackBar
              Left = 1464
              Top = 24
              Width = 25
              Height = 250
              Hint = '3kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 45
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_46Change
            end
            object TB_3_47: TTrackBar
              Left = 1496
              Top = 24
              Width = 25
              Height = 250
              Hint = '3.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 46
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_47Change
            end
            object TB_3_48: TTrackBar
              Left = 1528
              Top = 24
              Width = 25
              Height = 250
              Hint = '4kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 47
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_48Change
            end
            object TB_3_49: TTrackBar
              Left = 1560
              Top = 24
              Width = 25
              Height = 250
              Hint = '4.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 48
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_49Change
            end
            object TB_3_50: TTrackBar
              Left = 1592
              Top = 24
              Width = 25
              Height = 250
              Hint = '5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 49
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_50Change
            end
            object TB_3_51: TTrackBar
              Left = 1624
              Top = 24
              Width = 25
              Height = 250
              Hint = '5.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 50
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_51Change
            end
            object TB_3_52: TTrackBar
              Left = 1656
              Top = 24
              Width = 25
              Height = 250
              Hint = '6kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 51
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_52Change
            end
            object TB_3_53: TTrackBar
              Left = 1688
              Top = 24
              Width = 25
              Height = 250
              Hint = '6.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 52
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_53Change
            end
            object TB_3_54: TTrackBar
              Left = 1720
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 53
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_54Change
            end
            object TB_3_55: TTrackBar
              Left = 1752
              Top = 24
              Width = 25
              Height = 250
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 54
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_55Change
            end
            object TB_3_56: TTrackBar
              Left = 1784
              Top = 24
              Width = 25
              Height = 250
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 55
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_56Change
            end
            object TB_3_57: TTrackBar
              Left = 1816
              Top = 24
              Width = 25
              Height = 250
              Hint = '550Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 56
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_57Change
            end
            object TB_3_58: TTrackBar
              Left = 1848
              Top = 24
              Width = 25
              Height = 250
              Hint = '700Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 57
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_58Change
            end
            object TB_3_59: TTrackBar
              Left = 1880
              Top = 24
              Width = 25
              Height = 250
              Hint = '850Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 58
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_59Change
            end
            object TB_3_60: TTrackBar
              Left = 1912
              Top = 24
              Width = 25
              Height = 250
              Hint = '1kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 59
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_60Change
            end
            object TB_3_61: TTrackBar
              Left = 1944
              Top = 24
              Width = 25
              Height = 250
              Hint = '1.5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 60
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_61Change
            end
            object TB_3_62: TTrackBar
              Left = 1976
              Top = 24
              Width = 25
              Height = 250
              Hint = '2kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 61
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_62Change
            end
            object TB_3_63: TTrackBar
              Left = 2008
              Top = 24
              Width = 25
              Height = 250
              Hint = '2.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 62
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_63Change
            end
            object TB_3_64: TTrackBar
              Left = 2040
              Top = 24
              Width = 25
              Height = 250
              Hint = '3kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 63
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_64Change
            end
            object TB_3_65: TTrackBar
              Left = 2072
              Top = 24
              Width = 25
              Height = 250
              Hint = '3.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 64
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_65Change
            end
            object TB_3_66: TTrackBar
              Left = 2104
              Top = 24
              Width = 25
              Height = 250
              Hint = '4kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 65
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_66Change
            end
            object TB_3_67: TTrackBar
              Left = 2136
              Top = 24
              Width = 25
              Height = 250
              Hint = '4.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 66
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_67Change
            end
            object TB_3_68: TTrackBar
              Left = 2168
              Top = 24
              Width = 25
              Height = 250
              Hint = '5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 67
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_68Change
            end
            object TB_3_69: TTrackBar
              Left = 2200
              Top = 24
              Width = 25
              Height = 250
              Hint = '5.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 68
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_69Change
            end
            object TB_3_70: TTrackBar
              Left = 2232
              Top = 24
              Width = 25
              Height = 250
              Hint = '6kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 69
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_70Change
            end
            object TB_3_71: TTrackBar
              Left = 2264
              Top = 24
              Width = 25
              Height = 250
              Hint = '6.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 70
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_71Change
            end
            object TB_3_72: TTrackBar
              Left = 2296
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 71
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_72Change
            end
            object TB_3_73: TTrackBar
              Left = 2328
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 72
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_73Change
            end
            object TB_3_74: TTrackBar
              Left = 2360
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 73
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_74Change
            end
            object TB_3_75: TTrackBar
              Left = 2392
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 74
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_75Change
            end
            object TB_3_76: TTrackBar
              Left = 2424
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 75
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_76Change
            end
            object TB_3_77: TTrackBar
              Left = 2456
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 76
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_77Change
            end
            object TB_3_78: TTrackBar
              Left = 2488
              Top = 24
              Width = 25
              Height = 250
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 77
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_3_78Change
            end
          end
        end
        object btn_type3_all_center: TButton
          Left = 716
          Top = 376
          Width = 65
          Height = 25
          Caption = 'Reset EQ'
          TabOrder = 1
          OnClick = btn_type3_all_centerClick
        end
        object btn_type3_all_up: TButton
          Left = 8
          Top = 380
          Width = 33
          Height = 21
          Caption = #9650
          TabOrder = 2
          OnClick = btn_type3_all_upClick
        end
        object btn_type3_all_down: TButton
          Left = 48
          Top = 380
          Width = 33
          Height = 21
          Caption = #9660
          TabOrder = 3
          OnClick = btn_type3_all_downClick
        end
        object btn_type3_copy_from_type2: TButton
          Left = 551
          Top = 376
          Width = 129
          Height = 25
          Caption = 'Copy from EQ(Type2)'
          TabOrder = 4
          OnClick = btn_type3_copy_from_type2Click
        end
        object rdo_type3_value_x1: TRadioButton
          Left = 440
          Top = 380
          Width = 49
          Height = 17
          Caption = 'x1'
          Checked = True
          TabOrder = 5
          TabStop = True
        end
        object rdo_type3_value_x2: TRadioButton
          Left = 496
          Top = 380
          Width = 49
          Height = 17
          Caption = 'x2'
          TabOrder = 6
        end
        object cmbT3_range_from: TComboBox
          Left = 96
          Top = 379
          Width = 53
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 0
          TabOrder = 7
          Text = '---'
          OnChange = cmbT3_range_fromChange
          Items.Strings = (
            '---'
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
            '78')
        end
        object cmbT3_range_to: TComboBox
          Left = 165
          Top = 379
          Width = 53
          Height = 22
          Style = csOwnerDrawFixed
          ItemIndex = 0
          TabOrder = 8
          Text = '---'
          OnChange = cmbT3_range_toChange
          Items.Strings = (
            '---'
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
            '78')
        end
      end
    end
    object EQ_Type4: TTabSheet
      Caption = 'EQ(Type4)'
      ImageIndex = 3
      object GroupBox5: TGroupBox
        Left = 16
        Top = 16
        Width = 805
        Height = 417
        Caption = 'EQ(5Hz - 384kHz)'
        TabOrder = 0
        object Label187: TLabel
          Left = 392
          Top = 382
          Width = 43
          Height = 13
          Caption = 'EQ Value'
        end
        object Label213: TLabel
          Left = 156
          Top = 384
          Width = 4
          Height = 13
          Caption = '-'
        end
        object T4_range_label: TLabel
          Left = 232
          Top = 384
          Width = 3
          Height = 13
        end
        object t4_scrbox: TScrollBox
          Left = 8
          Top = 25
          Width = 773
          Height = 345
          TabOrder = 0
          object Panel5: TPanel
            Left = -2
            Top = 3
            Width = 3170
            Height = 321
            TabOrder = 0
            object l4_1: TLabel
              Left = 24
              Top = 286
              Width = 16
              Height = 11
              Caption = '5Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_2: TLabel
              Left = 56
              Top = 286
              Width = 16
              Height = 11
              Caption = '6Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_3: TLabel
              Left = 90
              Top = 286
              Width = 16
              Height = 11
              Caption = '8Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_4: TLabel
              Left = 120
              Top = 286
              Width = 21
              Height = 11
              Caption = '10Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_5: TLabel
              Left = 152
              Top = 286
              Width = 21
              Height = 11
              Caption = '12Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_6: TLabel
              Left = 184
              Top = 286
              Width = 21
              Height = 11
              Caption = '16Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_7: TLabel
              Left = 216
              Top = 286
              Width = 21
              Height = 11
              Caption = '20Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_8: TLabel
              Left = 248
              Top = 286
              Width = 21
              Height = 11
              Caption = '25Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_9: TLabel
              Left = 280
              Top = 286
              Width = 21
              Height = 11
              Caption = '31Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_10: TLabel
              Left = 312
              Top = 286
              Width = 21
              Height = 11
              Caption = '40Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_11: TLabel
              Left = 344
              Top = 286
              Width = 21
              Height = 11
              Caption = '50Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_12: TLabel
              Left = 376
              Top = 286
              Width = 21
              Height = 11
              Caption = '63Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_13: TLabel
              Left = 408
              Top = 286
              Width = 21
              Height = 11
              Caption = '80Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_14: TLabel
              Left = 440
              Top = 286
              Width = 26
              Height = 11
              Caption = '100Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_15: TLabel
              Left = 472
              Top = 286
              Width = 26
              Height = 11
              Caption = '125Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_16: TLabel
              Left = 504
              Top = 286
              Width = 26
              Height = 11
              Caption = '160Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_17: TLabel
              Left = 536
              Top = 286
              Width = 26
              Height = 11
              Caption = '200Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_18: TLabel
              Left = 568
              Top = 286
              Width = 26
              Height = 11
              Caption = '250Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_19: TLabel
              Left = 600
              Top = 288
              Width = 26
              Height = 11
              Caption = '315Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_20: TLabel
              Left = 632
              Top = 288
              Width = 26
              Height = 11
              Caption = '400Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_21: TLabel
              Left = 664
              Top = 288
              Width = 26
              Height = 11
              Caption = '500Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_22: TLabel
              Left = 696
              Top = 288
              Width = 26
              Height = 11
              Caption = '630Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_23: TLabel
              Left = 728
              Top = 288
              Width = 26
              Height = 11
              Caption = '800Hz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_24: TLabel
              Left = 760
              Top = 288
              Width = 21
              Height = 11
              Caption = '1kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_25: TLabel
              Left = 784
              Top = 288
              Width = 34
              Height = 11
              Caption = '1.25kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_26: TLabel
              Left = 824
              Top = 288
              Width = 29
              Height = 11
              Caption = '1.6kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_27: TLabel
              Left = 856
              Top = 288
              Width = 21
              Height = 11
              Caption = '2kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_28: TLabel
              Left = 880
              Top = 288
              Width = 29
              Height = 11
              Caption = '2.5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_29: TLabel
              Left = 912
              Top = 288
              Width = 34
              Height = 11
              Caption = '3.15kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_30: TLabel
              Left = 952
              Top = 288
              Width = 21
              Height = 11
              Caption = '4kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_31: TLabel
              Left = 984
              Top = 288
              Width = 21
              Height = 11
              Caption = '5kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_32: TLabel
              Left = 1016
              Top = 288
              Width = 21
              Height = 11
              Caption = '6kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_33: TLabel
              Left = 1048
              Top = 288
              Width = 21
              Height = 11
              Caption = '7kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_34: TLabel
              Left = 1080
              Top = 288
              Width = 21
              Height = 11
              Caption = '8kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_35: TLabel
              Left = 1112
              Top = 288
              Width = 21
              Height = 11
              Caption = '9kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_36: TLabel
              Left = 1144
              Top = 288
              Width = 26
              Height = 11
              Caption = '10kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_37: TLabel
              Left = 1176
              Top = 288
              Width = 26
              Height = 11
              Caption = '11kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_38: TLabel
              Left = 1208
              Top = 288
              Width = 26
              Height = 11
              Caption = '12kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_39: TLabel
              Left = 1240
              Top = 288
              Width = 26
              Height = 11
              Caption = '13kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_40: TLabel
              Left = 1272
              Top = 288
              Width = 26
              Height = 11
              Caption = '14kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_41: TLabel
              Left = 1304
              Top = 288
              Width = 26
              Height = 11
              Caption = '15kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_42: TLabel
              Left = 1336
              Top = 288
              Width = 26
              Height = 11
              Caption = '16kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_43: TLabel
              Left = 1368
              Top = 288
              Width = 26
              Height = 11
              Caption = '17kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_44: TLabel
              Left = 1400
              Top = 288
              Width = 26
              Height = 11
              Caption = '18kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_45: TLabel
              Left = 1432
              Top = 288
              Width = 26
              Height = 11
              Caption = '19kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_46: TLabel
              Left = 1464
              Top = 288
              Width = 26
              Height = 11
              Caption = '20kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_47: TLabel
              Left = 1496
              Top = 288
              Width = 26
              Height = 11
              Caption = '21kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_48: TLabel
              Left = 1528
              Top = 288
              Width = 26
              Height = 11
              Caption = '22kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_49: TLabel
              Left = 1560
              Top = 288
              Width = 26
              Height = 11
              Caption = '23kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_50: TLabel
              Left = 1592
              Top = 288
              Width = 26
              Height = 11
              Caption = '24kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_51: TLabel
              Left = 1624
              Top = 288
              Width = 26
              Height = 11
              Caption = '25kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_52: TLabel
              Left = 1656
              Top = 288
              Width = 26
              Height = 11
              Caption = '26kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_53: TLabel
              Left = 1688
              Top = 288
              Width = 26
              Height = 11
              Caption = '27kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_54: TLabel
              Left = 1720
              Top = 288
              Width = 26
              Height = 11
              Caption = '28kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_55: TLabel
              Left = 1752
              Top = 288
              Width = 26
              Height = 11
              Caption = '29kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_56: TLabel
              Left = 1784
              Top = 288
              Width = 26
              Height = 11
              Caption = '30kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_57: TLabel
              Left = 1816
              Top = 288
              Width = 26
              Height = 11
              Caption = '31kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_58: TLabel
              Left = 1848
              Top = 288
              Width = 26
              Height = 11
              Caption = '32kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_59: TLabel
              Left = 1880
              Top = 288
              Width = 26
              Height = 11
              Caption = '33kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_60: TLabel
              Left = 1912
              Top = 288
              Width = 26
              Height = 11
              Caption = '34kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_61: TLabel
              Left = 1944
              Top = 288
              Width = 26
              Height = 11
              Caption = '35kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_62: TLabel
              Left = 1976
              Top = 288
              Width = 26
              Height = 11
              Caption = '36kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_63: TLabel
              Left = 2008
              Top = 288
              Width = 26
              Height = 11
              Caption = '37kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_64: TLabel
              Left = 2040
              Top = 288
              Width = 26
              Height = 11
              Caption = '38kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_65: TLabel
              Left = 2072
              Top = 288
              Width = 26
              Height = 11
              Caption = '39kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_66: TLabel
              Left = 2104
              Top = 288
              Width = 26
              Height = 11
              Caption = '40kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_67: TLabel
              Left = 2136
              Top = 288
              Width = 26
              Height = 11
              Caption = '45kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_68: TLabel
              Left = 2168
              Top = 288
              Width = 26
              Height = 11
              Caption = '50kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_69: TLabel
              Left = 2200
              Top = 288
              Width = 26
              Height = 11
              Caption = '55kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_70: TLabel
              Left = 2232
              Top = 288
              Width = 26
              Height = 11
              Caption = '60kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_71: TLabel
              Left = 2264
              Top = 288
              Width = 26
              Height = 11
              Caption = '65kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_72: TLabel
              Left = 2296
              Top = 288
              Width = 26
              Height = 11
              Caption = '70kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object eq_4_1: TLabel
              Left = 36
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_2: TLabel
              Left = 68
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_3: TLabel
              Left = 98
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_4: TLabel
              Left = 130
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_5: TLabel
              Left = 164
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_6: TLabel
              Left = 196
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_7: TLabel
              Left = 230
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_8: TLabel
              Left = 260
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_9: TLabel
              Left = 290
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_10: TLabel
              Left = 322
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_11: TLabel
              Left = 354
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_12: TLabel
              Left = 388
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_13: TLabel
              Left = 416
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_14: TLabel
              Left = 452
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_15: TLabel
              Left = 484
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_16: TLabel
              Left = 516
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_17: TLabel
              Left = 548
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_18: TLabel
              Left = 580
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_19: TLabel
              Left = 610
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_20: TLabel
              Left = 642
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_21: TLabel
              Left = 676
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_22: TLabel
              Left = 708
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_23: TLabel
              Left = 736
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_24: TLabel
              Left = 772
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_25: TLabel
              Left = 804
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_26: TLabel
              Left = 836
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_27: TLabel
              Left = 870
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_28: TLabel
              Left = 900
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_29: TLabel
              Left = 930
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_30: TLabel
              Left = 962
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_31: TLabel
              Left = 996
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_32: TLabel
              Left = 1028
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_33: TLabel
              Left = 1056
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_34: TLabel
              Left = 1092
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_35: TLabel
              Left = 1124
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_36: TLabel
              Left = 1156
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_37: TLabel
              Left = 1190
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_38: TLabel
              Left = 1220
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_39: TLabel
              Left = 1250
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_40: TLabel
              Left = 1282
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_41: TLabel
              Left = 1316
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_42: TLabel
              Left = 1348
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_43: TLabel
              Left = 1376
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_44: TLabel
              Left = 1412
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_45: TLabel
              Left = 1444
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_46: TLabel
              Left = 1476
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_47: TLabel
              Left = 1510
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_48: TLabel
              Left = 1540
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_49: TLabel
              Left = 1570
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_50: TLabel
              Left = 1602
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_51: TLabel
              Left = 1636
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_52: TLabel
              Left = 1668
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_53: TLabel
              Left = 1696
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_54: TLabel
              Left = 1732
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_55: TLabel
              Left = 1764
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_56: TLabel
              Left = 1796
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_57: TLabel
              Left = 1830
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_58: TLabel
              Left = 1860
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_59: TLabel
              Left = 1890
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_60: TLabel
              Left = 1922
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_61: TLabel
              Left = 1956
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_62: TLabel
              Left = 1988
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_63: TLabel
              Left = 2016
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_64: TLabel
              Left = 2052
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_65: TLabel
              Left = 2084
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_66: TLabel
              Left = 2116
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_67: TLabel
              Left = 2150
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_68: TLabel
              Left = 2180
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_69: TLabel
              Left = 2210
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_70: TLabel
              Left = 2242
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object l4_73: TLabel
              Left = 2328
              Top = 288
              Width = 26
              Height = 11
              Caption = '75kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_74: TLabel
              Left = 2360
              Top = 288
              Width = 26
              Height = 11
              Caption = '80kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_75: TLabel
              Left = 2392
              Top = 288
              Width = 26
              Height = 11
              Caption = '85kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_76: TLabel
              Left = 2424
              Top = 288
              Width = 26
              Height = 11
              Caption = '90kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_77: TLabel
              Left = 2456
              Top = 288
              Width = 26
              Height = 11
              Caption = '95kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object l4_78: TLabel
              Left = 2488
              Top = 288
              Width = 31
              Height = 11
              Caption = '100kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object eq_4_71: TLabel
              Left = 2272
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_72: TLabel
              Left = 2308
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_73: TLabel
              Left = 2340
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_74: TLabel
              Left = 2372
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_75: TLabel
              Left = 2406
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_76: TLabel
              Left = 2436
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_77: TLabel
              Left = 2466
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_78: TLabel
              Left = 2498
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Label345: TLabel
              Left = 16
              Top = 140
              Width = 6
              Height = 13
              Caption = '0'
            end
            object Label189: TLabel
              Left = 2519
              Top = 296
              Width = 31
              Height = 11
              Caption = '105kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label190: TLabel
              Left = 2547
              Top = 288
              Width = 31
              Height = 11
              Caption = '110kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label191: TLabel
              Left = 2581
              Top = 296
              Width = 31
              Height = 11
              Caption = '115kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label192: TLabel
              Left = 2612
              Top = 288
              Width = 31
              Height = 11
              Caption = '120kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label193: TLabel
              Left = 2638
              Top = 296
              Width = 31
              Height = 11
              Caption = '125kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label194: TLabel
              Left = 2670
              Top = 288
              Width = 31
              Height = 11
              Caption = '130kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label195: TLabel
              Left = 2702
              Top = 296
              Width = 31
              Height = 11
              Caption = '135kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label196: TLabel
              Left = 2734
              Top = 288
              Width = 31
              Height = 11
              Caption = '140kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label197: TLabel
              Left = 2766
              Top = 296
              Width = 31
              Height = 11
              Caption = '145kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label198: TLabel
              Left = 2797
              Top = 288
              Width = 31
              Height = 11
              Caption = '150kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label199: TLabel
              Left = 2834
              Top = 296
              Width = 31
              Height = 11
              Caption = '155kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label200: TLabel
              Left = 2865
              Top = 288
              Width = 31
              Height = 11
              Caption = '160kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label201: TLabel
              Left = 2890
              Top = 296
              Width = 31
              Height = 11
              Caption = '165kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label202: TLabel
              Left = 2922
              Top = 288
              Width = 31
              Height = 11
              Caption = '170kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label203: TLabel
              Left = 2954
              Top = 296
              Width = 31
              Height = 11
              Caption = '175kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label204: TLabel
              Left = 2986
              Top = 288
              Width = 31
              Height = 11
              Caption = '180kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label205: TLabel
              Left = 3018
              Top = 296
              Width = 31
              Height = 11
              Caption = '185kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label206: TLabel
              Left = 3049
              Top = 288
              Width = 31
              Height = 11
              Caption = '190kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label207: TLabel
              Left = 3086
              Top = 296
              Width = 31
              Height = 11
              Caption = '195kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label208: TLabel
              Left = 3117
              Top = 288
              Width = 31
              Height = 11
              Caption = '200kHz'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object eq_4_98: TLabel
              Left = 3129
              Top = 3
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_97: TLabel
              Left = 3097
              Top = 3
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_96: TLabel
              Left = 3067
              Top = 3
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_95: TLabel
              Left = 3037
              Top = 3
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_94: TLabel
              Left = 3003
              Top = 3
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_93: TLabel
              Left = 2971
              Top = 3
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_92: TLabel
              Left = 2940
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_91: TLabel
              Left = 2907
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_90: TLabel
              Left = 2877
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_89: TLabel
              Left = 2845
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_88: TLabel
              Left = 2815
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_87: TLabel
              Left = 2782
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_86: TLabel
              Left = 2751
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_85: TLabel
              Left = 2719
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_84: TLabel
              Left = 2687
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_83: TLabel
              Left = 2651
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_82: TLabel
              Left = 2623
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_81: TLabel
              Left = 2591
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_80: TLabel
              Left = 2560
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object eq_4_79: TLabel
              Left = 2528
              Top = 4
              Width = 6
              Height = 13
              Caption = '0'
            end
            object TB_4_1: TTrackBar
              Left = 24
              Top = 24
              Width = 25
              Height = 230
              Hint = '5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              PageSize = 1
              Frequency = 2
              TabOrder = 0
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_1Change
            end
            object TB_4_2: TTrackBar
              Left = 56
              Top = 24
              Width = 25
              Height = 230
              Hint = '10Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 1
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_2Change
            end
            object TB_4_3: TTrackBar
              Left = 88
              Top = 24
              Width = 25
              Height = 230
              Hint = '20Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 2
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_3Change
            end
            object TB_4_4: TTrackBar
              Left = 120
              Top = 24
              Width = 25
              Height = 230
              Hint = '30Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 3
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_4Change
            end
            object TB_4_5: TTrackBar
              Left = 152
              Top = 24
              Width = 25
              Height = 230
              Hint = '40Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 4
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_5Change
            end
            object TB_4_6: TTrackBar
              Left = 184
              Top = 24
              Width = 25
              Height = 230
              Hint = '50Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 5
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_6Change
            end
            object TB_4_7: TTrackBar
              Left = 216
              Top = 24
              Width = 25
              Height = 230
              Hint = '60Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 6
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_7Change
            end
            object TB_4_8: TTrackBar
              Left = 248
              Top = 24
              Width = 25
              Height = 230
              Hint = '70Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 7
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_8Change
            end
            object TB_4_9: TTrackBar
              Left = 280
              Top = 24
              Width = 25
              Height = 230
              Hint = '80Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 8
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_9Change
            end
            object TB_4_10: TTrackBar
              Left = 312
              Top = 24
              Width = 25
              Height = 230
              Hint = '90Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 9
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_10Change
            end
            object TB_4_11: TTrackBar
              Left = 344
              Top = 24
              Width = 25
              Height = 230
              Hint = '100Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 10
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_11Change
            end
            object TB_4_12: TTrackBar
              Left = 376
              Top = 24
              Width = 25
              Height = 230
              Hint = '110Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 11
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_12Change
            end
            object TB_4_13: TTrackBar
              Left = 408
              Top = 24
              Width = 25
              Height = 230
              Hint = '120Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 12
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_13Change
            end
            object TB_4_14: TTrackBar
              Left = 440
              Top = 24
              Width = 25
              Height = 230
              Hint = '130Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 13
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_14Change
            end
            object TB_4_15: TTrackBar
              Left = 472
              Top = 24
              Width = 25
              Height = 230
              Hint = '140Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 14
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_15Change
            end
            object TB_4_16: TTrackBar
              Left = 504
              Top = 24
              Width = 25
              Height = 230
              Hint = '150Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 15
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_16Change
            end
            object TB_4_17: TTrackBar
              Left = 536
              Top = 24
              Width = 25
              Height = 230
              Hint = '175Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 16
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_17Change
            end
            object TB_4_18: TTrackBar
              Left = 568
              Top = 24
              Width = 25
              Height = 230
              Hint = '200Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 17
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_18Change
            end
            object TB_4_19: TTrackBar
              Left = 600
              Top = 24
              Width = 25
              Height = 230
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 18
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_19Change
            end
            object TB_4_20: TTrackBar
              Left = 632
              Top = 24
              Width = 25
              Height = 230
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 19
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_20Change
            end
            object TB_4_21: TTrackBar
              Left = 664
              Top = 24
              Width = 25
              Height = 230
              Hint = '550Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 20
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_21Change
            end
            object TB_4_22: TTrackBar
              Left = 696
              Top = 24
              Width = 25
              Height = 230
              Hint = '700Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 21
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_22Change
            end
            object TB_4_23: TTrackBar
              Left = 728
              Top = 24
              Width = 25
              Height = 230
              Hint = '850Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 22
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_23Change
            end
            object TB_4_24: TTrackBar
              Left = 760
              Top = 24
              Width = 25
              Height = 230
              Hint = '1kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 23
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_24Change
            end
            object TB_4_25: TTrackBar
              Left = 792
              Top = 24
              Width = 25
              Height = 230
              Hint = '1.5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 24
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_25Change
            end
            object TB_4_26: TTrackBar
              Left = 824
              Top = 24
              Width = 25
              Height = 230
              Hint = '2kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 25
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_26Change
            end
            object TB_4_27: TTrackBar
              Left = 856
              Top = 24
              Width = 25
              Height = 230
              Hint = '2.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 26
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_27Change
            end
            object TB_4_28: TTrackBar
              Left = 888
              Top = 24
              Width = 25
              Height = 230
              Hint = '3kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 27
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_28Change
            end
            object TB_4_29: TTrackBar
              Left = 920
              Top = 24
              Width = 25
              Height = 230
              Hint = '3.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 28
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_29Change
            end
            object TB_4_30: TTrackBar
              Left = 952
              Top = 24
              Width = 25
              Height = 230
              Hint = '4kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 29
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_30Change
            end
            object TB_4_31: TTrackBar
              Left = 984
              Top = 24
              Width = 25
              Height = 230
              Hint = '4.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 30
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_31Change
            end
            object TB_4_32: TTrackBar
              Left = 1016
              Top = 24
              Width = 25
              Height = 230
              Hint = '5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 31
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_32Change
            end
            object TB_4_33: TTrackBar
              Left = 1048
              Top = 24
              Width = 25
              Height = 230
              Hint = '5.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 32
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_33Change
            end
            object TB_4_34: TTrackBar
              Left = 1080
              Top = 24
              Width = 25
              Height = 230
              Hint = '6kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 33
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_34Change
            end
            object TB_4_35: TTrackBar
              Left = 1112
              Top = 24
              Width = 25
              Height = 230
              Hint = '6.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 34
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_35Change
            end
            object TB_4_36: TTrackBar
              Left = 1144
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 35
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_36Change
            end
            object TB_4_37: TTrackBar
              Left = 1176
              Top = 24
              Width = 25
              Height = 230
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 36
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_37Change
            end
            object TB_4_38: TTrackBar
              Left = 1208
              Top = 24
              Width = 25
              Height = 230
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 37
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_38Change
            end
            object TB_4_39: TTrackBar
              Left = 1240
              Top = 24
              Width = 25
              Height = 230
              Hint = '550Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 38
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_39Change
            end
            object TB_4_40: TTrackBar
              Left = 1272
              Top = 24
              Width = 25
              Height = 230
              Hint = '700Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 39
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_40Change
            end
            object TB_4_41: TTrackBar
              Left = 1304
              Top = 24
              Width = 25
              Height = 230
              Hint = '850Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 40
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_41Change
            end
            object TB_4_42: TTrackBar
              Left = 1336
              Top = 24
              Width = 25
              Height = 230
              Hint = '1kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 41
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_42Change
            end
            object TB_4_43: TTrackBar
              Left = 1368
              Top = 24
              Width = 25
              Height = 230
              Hint = '1.5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 42
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_43Change
            end
            object TB_4_44: TTrackBar
              Left = 1400
              Top = 24
              Width = 25
              Height = 230
              Hint = '2kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 43
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_44Change
            end
            object TB_4_45: TTrackBar
              Left = 1432
              Top = 24
              Width = 25
              Height = 230
              Hint = '2.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 44
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_45Change
            end
            object TB_4_46: TTrackBar
              Left = 1464
              Top = 24
              Width = 25
              Height = 230
              Hint = '3kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 45
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_46Change
            end
            object TB_4_47: TTrackBar
              Left = 1496
              Top = 24
              Width = 25
              Height = 230
              Hint = '3.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 46
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_47Change
            end
            object TB_4_48: TTrackBar
              Left = 1528
              Top = 24
              Width = 25
              Height = 230
              Hint = '4kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 47
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_48Change
            end
            object TB_4_49: TTrackBar
              Left = 1560
              Top = 24
              Width = 25
              Height = 230
              Hint = '4.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 48
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_49Change
            end
            object TB_4_50: TTrackBar
              Left = 1592
              Top = 24
              Width = 25
              Height = 230
              Hint = '5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 49
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_50Change
            end
            object TB_4_51: TTrackBar
              Left = 1624
              Top = 24
              Width = 25
              Height = 230
              Hint = '5.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 50
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_51Change
            end
            object TB_4_52: TTrackBar
              Left = 1656
              Top = 24
              Width = 25
              Height = 230
              Hint = '6kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 51
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_52Change
            end
            object TB_4_53: TTrackBar
              Left = 1688
              Top = 24
              Width = 25
              Height = 230
              Hint = '6.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 52
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_53Change
            end
            object TB_4_54: TTrackBar
              Left = 1720
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 53
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_54Change
            end
            object TB_4_55: TTrackBar
              Left = 1752
              Top = 24
              Width = 25
              Height = 230
              Hint = '250Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 54
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_55Change
            end
            object TB_4_56: TTrackBar
              Left = 1784
              Top = 24
              Width = 25
              Height = 230
              Hint = '400Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 55
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_56Change
            end
            object TB_4_57: TTrackBar
              Left = 1816
              Top = 24
              Width = 25
              Height = 230
              Hint = '550Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 56
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_57Change
            end
            object TB_4_58: TTrackBar
              Left = 1848
              Top = 24
              Width = 25
              Height = 230
              Hint = '700Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 57
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_58Change
            end
            object TB_4_59: TTrackBar
              Left = 1880
              Top = 24
              Width = 25
              Height = 230
              Hint = '850Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 58
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_59Change
            end
            object TB_4_60: TTrackBar
              Left = 1912
              Top = 24
              Width = 25
              Height = 230
              Hint = '1kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 59
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_60Change
            end
            object TB_4_61: TTrackBar
              Left = 1944
              Top = 24
              Width = 25
              Height = 230
              Hint = '1.5Hz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 60
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_61Change
            end
            object TB_4_62: TTrackBar
              Left = 1976
              Top = 24
              Width = 25
              Height = 230
              Hint = '2kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 61
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_62Change
            end
            object TB_4_63: TTrackBar
              Left = 2008
              Top = 24
              Width = 25
              Height = 230
              Hint = '2.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 62
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_63Change
            end
            object TB_4_64: TTrackBar
              Left = 2040
              Top = 24
              Width = 25
              Height = 230
              Hint = '3kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 63
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_64Change
            end
            object TB_4_65: TTrackBar
              Left = 2072
              Top = 24
              Width = 25
              Height = 230
              Hint = '3.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 64
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_65Change
            end
            object TB_4_66: TTrackBar
              Left = 2104
              Top = 24
              Width = 25
              Height = 230
              Hint = '4kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 65
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_66Change
            end
            object TB_4_67: TTrackBar
              Left = 2136
              Top = 24
              Width = 25
              Height = 230
              Hint = '4.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 66
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_67Change
            end
            object TB_4_68: TTrackBar
              Left = 2168
              Top = 24
              Width = 25
              Height = 230
              Hint = '5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 67
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_68Change
            end
            object TB_4_69: TTrackBar
              Left = 2200
              Top = 24
              Width = 25
              Height = 230
              Hint = '5.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 68
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_69Change
            end
            object TB_4_70: TTrackBar
              Left = 2232
              Top = 24
              Width = 25
              Height = 230
              Hint = '6kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 69
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_70Change
            end
            object TB_4_71: TTrackBar
              Left = 2264
              Top = 24
              Width = 25
              Height = 230
              Hint = '6.5kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 70
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_71Change
            end
            object TB_4_72: TTrackBar
              Left = 2296
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 71
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_72Change
            end
            object TB_4_73: TTrackBar
              Left = 2328
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 72
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_73Change
            end
            object TB_4_74: TTrackBar
              Left = 2360
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 73
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_74Change
            end
            object TB_4_75: TTrackBar
              Left = 2392
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 74
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_75Change
            end
            object TB_4_76: TTrackBar
              Left = 2424
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 75
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_76Change
            end
            object TB_4_77: TTrackBar
              Left = 2456
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 76
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_77Change
            end
            object TB_4_78: TTrackBar
              Left = 2488
              Top = 24
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 77
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_78Change
            end
            object TB_4_79: TTrackBar
              Left = 2519
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 78
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_79Change
            end
            object TB_4_80: TTrackBar
              Left = 2550
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 79
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_80Change
            end
            object TB_4_81: TTrackBar
              Left = 2581
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 80
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_81Change
            end
            object TB_4_83: TTrackBar
              Left = 2644
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 81
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_83Change
            end
            object TB_4_82: TTrackBar
              Left = 2612
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 82
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_82Change
            end
            object TB_4_84: TTrackBar
              Left = 2676
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 83
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_84Change
            end
            object TB_4_85: TTrackBar
              Left = 2708
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 84
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_85Change
            end
            object TB_4_86: TTrackBar
              Left = 2740
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 85
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_86Change
            end
            object TB_4_87: TTrackBar
              Left = 2772
              Top = 23
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 86
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_87Change
            end
            object TB_4_88: TTrackBar
              Left = 2803
              Top = 22
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 87
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_88Change
            end
            object TB_4_89: TTrackBar
              Left = 2834
              Top = 22
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 88
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_89Change
            end
            object TB_4_90: TTrackBar
              Left = 2865
              Top = 22
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 89
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_90Change
            end
            object TB_4_98: TTrackBar
              Left = 3117
              Top = 21
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 90
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_98Change
            end
            object TB_4_97: TTrackBar
              Left = 3086
              Top = 21
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 91
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_97Change
            end
            object TB_4_96: TTrackBar
              Left = 3055
              Top = 21
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 92
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_96Change
            end
            object TB_4_95: TTrackBar
              Left = 3024
              Top = 22
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 93
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_95Change
            end
            object TB_4_94: TTrackBar
              Left = 2992
              Top = 22
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 94
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_94Change
            end
            object TB_4_93: TTrackBar
              Left = 2960
              Top = 22
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 95
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_93Change
            end
            object TB_4_92: TTrackBar
              Left = 2928
              Top = 22
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 96
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_92Change
            end
            object TB_4_91: TTrackBar
              Left = 2896
              Top = 22
              Width = 25
              Height = 230
              Hint = '7kHz'
              Ctl3D = False
              Max = 14
              Min = -14
              Orientation = trVertical
              ParentCtl3D = False
              Frequency = 2
              TabOrder = 97
              ThumbLength = 12
              TickMarks = tmBoth
              OnChange = TB_4_91Change
            end
            object TB_4_1_Cutoff: TCheckBox
              Left = 28
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 98
              OnClick = TB_4_1_CutoffClick
            end
            object TB_4_2_Cutoff: TCheckBox
              Left = 60
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 99
              OnClick = TB_4_2_CutoffClick
            end
            object TB_4_3_Cutoff: TCheckBox
              Left = 93
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 100
              OnClick = TB_4_3_CutoffClick
            end
            object TB_4_4_Cutoff: TCheckBox
              Left = 125
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 101
              OnClick = TB_4_4_CutoffClick
            end
            object TB_4_5_Cutoff: TCheckBox
              Left = 156
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 102
              OnClick = TB_4_5_CutoffClick
            end
            object TB_4_6_Cutoff: TCheckBox
              Left = 188
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 103
              OnClick = TB_4_6_CutoffClick
            end
            object TB_4_7_Cutoff: TCheckBox
              Left = 222
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 104
              OnClick = TB_4_7_CutoffClick
            end
            object TB_4_8_Cutoff: TCheckBox
              Left = 253
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 105
              OnClick = TB_4_8_CutoffClick
            end
            object TB_4_9_Cutoff: TCheckBox
              Left = 287
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 106
              OnClick = TB_4_9_CutoffClick
            end
            object TB_4_10_Cutoff: TCheckBox
              Left = 319
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 107
              OnClick = TB_4_10_CutoffClick
            end
            object TB_4_11_Cutoff: TCheckBox
              Left = 351
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 108
              OnClick = TB_4_11_CutoffClick
            end
            object TB_4_12_Cutoff: TCheckBox
              Left = 380
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 109
              OnClick = TB_4_12_CutoffClick
            end
            object TB_4_13_Cutoff: TCheckBox
              Left = 413
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 110
              OnClick = TB_4_13_CutoffClick
            end
            object TB_4_36_Cutoff: TCheckBox
              Left = 1148
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 111
              OnClick = TB_4_36_CutoffClick
            end
            object TB_4_37_Cutoff: TCheckBox
              Left = 1182
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 112
              OnClick = TB_4_37_CutoffClick
            end
            object TB_4_38_Cutoff: TCheckBox
              Left = 1212
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 113
              OnClick = TB_4_38_CutoffClick
            end
            object TB_4_39_Cutoff: TCheckBox
              Left = 1242
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 114
              OnClick = TB_4_39_CutoffClick
            end
            object TB_4_40_Cutoff: TCheckBox
              Left = 1276
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 115
              OnClick = TB_4_40_CutoffClick
            end
            object TB_4_41_Cutoff: TCheckBox
              Left = 1308
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 116
              OnClick = TB_4_41_CutoffClick
            end
            object TB_4_42_Cutoff: TCheckBox
              Left = 1341
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 117
              OnClick = TB_4_42_CutoffClick
            end
            object TB_4_43_Cutoff: TCheckBox
              Left = 1373
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 118
              OnClick = TB_4_43_CutoffClick
            end
            object TB_4_44_Cutoff: TCheckBox
              Left = 1404
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 119
              OnClick = TB_4_44_CutoffClick
            end
            object TB_4_45_Cutoff: TCheckBox
              Left = 1436
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 120
              OnClick = TB_4_45_CutoffClick
            end
            object TB_4_46_Cutoff: TCheckBox
              Left = 1468
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 121
              OnClick = TB_4_46_CutoffClick
            end
            object TB_4_47_Cutoff: TCheckBox
              Left = 1502
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 122
              OnClick = TB_4_47_CutoffClick
            end
            object TB_4_48_Cutoff: TCheckBox
              Left = 1532
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 123
              OnClick = TB_4_48_CutoffClick
            end
            object TB_4_49_Cutoff: TCheckBox
              Left = 1566
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 124
              OnClick = TB_4_49_CutoffClick
            end
            object TB_4_50_Cutoff: TCheckBox
              Left = 1598
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 125
              OnClick = TB_4_50_CutoffClick
            end
            object TB_4_51_Cutoff: TCheckBox
              Left = 1628
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 126
              OnClick = TB_4_51_CutoffClick
            end
            object TB_4_52_Cutoff: TCheckBox
              Left = 1660
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 127
              OnClick = TB_4_52_CutoffClick
            end
            object TB_4_53_Cutoff: TCheckBox
              Left = 1691
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 128
              OnClick = TB_4_53_CutoffClick
            end
            object TB_4_54_Cutoff: TCheckBox
              Left = 1724
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 129
              OnClick = TB_4_54_CutoffClick
            end
            object TB_4_55_Cutoff: TCheckBox
              Left = 1756
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 130
              OnClick = TB_4_55_CutoffClick
            end
            object TB_4_56_Cutoff: TCheckBox
              Left = 1788
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 131
              OnClick = TB_4_56_CutoffClick
            end
            object TB_4_57_Cutoff: TCheckBox
              Left = 1822
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 132
              OnClick = TB_4_57_CutoffClick
            end
            object TB_4_58_Cutoff: TCheckBox
              Left = 1852
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 133
              OnClick = TB_4_58_CutoffClick
            end
            object TB_4_59_Cutoff: TCheckBox
              Left = 1884
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 134
              OnClick = TB_4_59_CutoffClick
            end
            object TB_4_60_Cutoff: TCheckBox
              Left = 1914
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 135
              OnClick = TB_4_60_CutoffClick
            end
            object TB_4_61_Cutoff: TCheckBox
              Left = 1948
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 136
              OnClick = TB_4_61_CutoffClick
            end
            object TB_4_62_Cutoff: TCheckBox
              Left = 1980
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 137
              OnClick = TB_4_62_CutoffClick
            end
            object TB_4_63_Cutoff: TCheckBox
              Left = 2011
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 138
              OnClick = TB_4_63_CutoffClick
            end
            object TB_4_64_Cutoff: TCheckBox
              Left = 2044
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 139
              OnClick = TB_4_64_CutoffClick
            end
            object TB_4_65_Cutoff: TCheckBox
              Left = 2079
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 140
              OnClick = TB_4_65_CutoffClick
            end
            object TB_4_66_Cutoff: TCheckBox
              Left = 2112
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 141
              OnClick = TB_4_66_CutoffClick
            end
            object TB_4_67_Cutoff: TCheckBox
              Left = 2142
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 142
              OnClick = TB_4_67_CutoffClick
            end
            object TB_4_68_Cutoff: TCheckBox
              Left = 2172
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 143
              OnClick = TB_4_68_CutoffClick
            end
            object TB_4_69_Cutoff: TCheckBox
              Left = 2202
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 144
              OnClick = TB_4_69_CutoffClick
            end
            object TB_4_70_Cutoff: TCheckBox
              Left = 2235
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 145
              OnClick = TB_4_70_CutoffClick
            end
            object TB_4_71_Cutoff: TCheckBox
              Left = 2268
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 146
              OnClick = TB_4_71_CutoffClick
            end
            object TB_4_72_Cutoff: TCheckBox
              Left = 2300
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 147
              OnClick = TB_4_72_CutoffClick
            end
            object TB_4_73_Cutoff: TCheckBox
              Left = 2332
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 148
              OnClick = TB_4_73_CutoffClick
            end
            object TB_4_74_Cutoff: TCheckBox
              Left = 2364
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 149
              OnClick = TB_4_74_CutoffClick
            end
            object TB_4_75_Cutoff: TCheckBox
              Left = 2398
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 150
              OnClick = TB_4_75_CutoffClick
            end
            object TB_4_76_Cutoff: TCheckBox
              Left = 2428
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 151
              OnClick = TB_4_76_CutoffClick
            end
            object TB_4_77_Cutoff: TCheckBox
              Left = 2462
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 152
              OnClick = TB_4_77_CutoffClick
            end
            object TB_4_78_Cutoff: TCheckBox
              Left = 2494
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 153
              OnClick = TB_4_78_CutoffClick
            end
            object TB_4_79_Cutoff: TCheckBox
              Left = 2524
              Top = 260
              Width = 14
              Height = 17
              TabOrder = 154
              OnClick = TB_4_79_CutoffClick
            end
            object TB_4_80_Cutoff: TCheckBox
              Left = 2556
              Top = 259
              Width = 14
              Height = 17
              TabOrder = 155
              OnClick = TB_4_80_CutoffClick
            end
            object TB_4_81_Cutoff: TCheckBox
              Left = 2586
              Top = 259
              Width = 14
              Height = 17
              TabOrder = 156
              OnClick = TB_4_81_CutoffClick
            end
            object TB_4_82_Cutoff: TCheckBox
              Left = 2618
              Top = 259
              Width = 14
              Height = 17
              TabOrder = 157
              OnClick = TB_4_82_CutoffClick
            end
            object TB_4_83_Cutoff: TCheckBox
              Left = 2651
              Top = 259
              Width = 14
              Height = 17
              TabOrder = 158
              OnClick = TB_4_83_CutoffClick
            end
            object TB_4_84_Cutoff: TCheckBox
              Left = 2683
              Top = 259
              Width = 14
              Height = 17
              TabOrder = 159
              OnClick = TB_4_84_CutoffClick
            end
            object TB_4_85_Cutoff: TCheckBox
              Left = 2715
              Top = 259
              Width = 14
              Height = 17
              TabOrder = 160
              OnClick = TB_4_85_CutoffClick
            end
            object TB_4_86_Cutoff: TCheckBox
              Left = 2746
              Top = 259
              Width = 14
              Height = 17
              TabOrder = 161
              OnClick = TB_4_86_CutoffClick
            end
            object TB_4_87_Cutoff: TCheckBox
              Left = 2778
              Top = 259
              Width = 14
              Height = 17
              TabOrder = 162
              OnClick = TB_4_87_CutoffClick
            end
            object TB_4_88_Cutoff: TCheckBox
              Left = 2807
              Top = 258
              Width = 14
              Height = 17
              TabOrder = 163
              OnClick = TB_4_88_CutoffClick
            end
            object TB_4_89_Cutoff: TCheckBox
              Left = 2837
              Top = 258
              Width = 14
              Height = 17
              TabOrder = 164
              OnClick = TB_4_89_CutoffClick
            end
            object TB_4_90_Cutoff: TCheckBox
              Left = 2869
              Top = 258
              Width = 14
              Height = 17
              TabOrder = 165
              OnClick = TB_4_90_CutoffClick
            end
            object TB_4_91_Cutoff: TCheckBox
              Left = 2899
              Top = 258
              Width = 14
              Height = 17
              TabOrder = 166
              OnClick = TB_4_91_CutoffClick
            end
            object TB_4_92_Cutoff: TCheckBox
              Left = 2932
              Top = 258
              Width = 14
              Height = 17
              TabOrder = 167
              OnClick = TB_4_92_CutoffClick
            end
            object TB_4_93_Cutoff: TCheckBox
              Left = 2963
              Top = 258
              Width = 14
              Height = 17
              TabOrder = 168
              OnClick = TB_4_93_CutoffClick
            end
            object TB_4_94_Cutoff: TCheckBox
              Left = 2995
              Top = 258
              Width = 14
              Height = 17
              TabOrder = 169
              OnClick = TB_4_94_CutoffClick
            end
            object TB_4_95_Cutoff: TCheckBox
              Left = 3029
              Top = 258
              Width = 14
              Height = 17
              TabOrder = 170
              OnClick = TB_4_95_CutoffClick
            end
            object TB_4_96_Cutoff: TCheckBox
              Left = 3059
              Top = 257
              Width = 14
              Height = 17
              TabOrder = 171
              OnClick = TB_4_96_CutoffClick
            end
            object TB_4_97_Cutoff: TCheckBox
              Left = 3093
              Top = 257
              Width = 14
              Height = 17
              TabOrder = 172
              OnClick = TB_4_97_CutoffClick
            end
            object TB_4_98_Cutoff: TCheckBox
              Left = 3125
              Top = 257
              Width = 14
              Height = 17
              TabOrder = 173
              OnClick = TB_4_98_CutoffClick
            end
          end
        end
        object Button1: TButton
          Left = 716
          Top = 376
          Width = 65
          Height = 25
          Caption = 'Reset EQ'
          TabOrder = 1
          OnClick = Button1Click
        end
        object btn_type4_all_up: TButton
          Left = 8
          Top = 380
          Width = 33
          Height = 21
          Caption = #9650
          TabOrder = 2
          OnClick = btn_type4_all_upClick
        end
        object btn_type4_all_down: TButton
          Left = 48
          Top = 380
          Width = 33
          Height = 21
          Caption = #9660
          TabOrder = 3
          OnClick = btn_type4_all_downClick
        end
        object btn_type4_copy_from_type3: TButton
          Left = 551
          Top = 376
          Width = 129
          Height = 25
          Caption = 'Copy from EQ(Type3)'
          TabOrder = 4
          OnClick = btn_type4_copy_from_type3Click
        end
        object rdo_type4_value_x1: TRadioButton
          Left = 440
          Top = 380
          Width = 49
          Height = 17
          Caption = 'x1'
          Checked = True
          TabOrder = 5
          TabStop = True
        end
        object rdo_type4_value_x2: TRadioButton
          Left = 496
          Top = 380
          Width = 49
          Height = 17
          Caption = 'x2'
          TabOrder = 6
        end
      end
      object cmbT4_range_from: TComboBox
        Left = 112
        Top = 394
        Width = 53
        Height = 22
        Style = csOwnerDrawFixed
        ItemIndex = 0
        TabOrder = 1
        Text = '---'
        OnChange = cmbT4_range_fromChange
        Items.Strings = (
          '---'
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
          '98')
      end
      object cmbT4_range_to: TComboBox
        Left = 181
        Top = 394
        Width = 53
        Height = 22
        Style = csOwnerDrawFixed
        ItemIndex = 0
        TabOrder = 2
        Text = '---'
        OnChange = cmbT4_range_toChange
        Items.Strings = (
          '---'
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
          '98')
      end
    end
  end
  object Panel1: TPanel
    Left = 408
    Top = 488
    Width = 833
    Height = 97
    TabOrder = 7
    object edt_eq_title: TEdit
      Left = 16
      Top = 14
      Width = 241
      Height = 21
      TabOrder = 0
      OnChange = edt_eq_titleChange
    end
    object btn_eq_save: TBitBtn
      Left = 16
      Top = 40
      Width = 75
      Height = 25
      Caption = 'Save'
      Default = True
      ModalResult = 1
      NumGlyphs = 2
      TabOrder = 1
      OnClick = btn_eq_saveClick
    end
    object GroupBox4: TGroupBox
      Left = 272
      Top = 8
      Width = 409
      Height = 81
      Caption = 'Preview'
      TabOrder = 2
      object lbl_time: TLabel
        Left = 160
        Top = 28
        Width = 3
        Height = 13
        Caption = ' '
      end
      object lbl_selected_len: TLabel
        Left = 102
        Top = 9
        Width = 154
        Height = 13
        Caption = 'Start Offset : 0:00:00 / 0:00:00'
      end
      object btnPlay: TButton
        Left = 8
        Top = 24
        Width = 73
        Height = 25
        Caption = 'Play'
        Enabled = False
        TabOrder = 0
        OnClick = btnPlayClick
      end
      object track_skip_position: TTrackBar
        Left = 87
        Top = 28
        Width = 211
        Height = 25
        Max = 60
        PageSize = 10
        Frequency = 10
        TabOrder = 1
        OnChange = track_skip_positionChange
      end
      object chk_play_loop: TCheckBox
        Left = 8
        Top = 56
        Width = 57
        Height = 17
        Caption = 'Loop'
        TabOrder = 2
      end
      object cmb_preview_length: TComboBox
        Left = 304
        Top = 26
        Width = 89
        Height = 22
        Style = csOwnerDrawFixed
        ItemIndex = 2
        TabOrder = 3
        Text = '15 Sec'
        OnChange = cmb_preview_lengthChange
        Items.Strings = (
          '5   Sec'
          '10 Sec'
          '15 Sec'
          '20 Sec'
          '30 Sec')
      end
    end
    object GroupBox2: TGroupBox
      Left = 694
      Top = 8
      Width = 123
      Height = 81
      Caption = 'Window Size'
      TabOrder = 3
      object rdo_window_size1: TRadioButton
        Left = 8
        Top = 20
        Width = 73
        Height = 17
        Caption = 'Small'
        Checked = True
        TabOrder = 0
        TabStop = True
        OnClick = rdo_window_size1Click
      end
      object rdo_window_size2: TRadioButton
        Left = 8
        Top = 40
        Width = 65
        Height = 17
        Caption = 'Medium'
        TabOrder = 1
        OnClick = rdo_window_size2Click
      end
      object rdo_window_size3: TRadioButton
        Left = 8
        Top = 61
        Width = 69
        Height = 17
        Caption = 'Large'
        TabOrder = 2
        OnClick = rdo_window_size3Click
      end
    end
  end
  object edtParameter: TMemo
    Left = 8
    Top = 724
    Width = 1191
    Height = 213
    ScrollBars = ssBoth
    TabOrder = 8
    Visible = False
    WordWrap = False
  end
  object memo_log: TMemo
    Left = 409
    Top = 599
    Width = 434
    Height = 114
    TabOrder = 9
  end
  object memo_DeviceInfo: TMemo
    Left = 8
    Top = 599
    Width = 386
    Height = 114
    TabOrder = 10
  end
  object Memo1: TMemo
    Left = 311
    Top = 8
    Width = 90
    Height = 13
    BorderStyle = bsNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Lines.Strings = (
      'Ver 2024/11/03')
    ParentFont = False
    ReadOnly = True
    TabOrder = 11
  end
  object OpenDialog: TOpenDialog
    Ctl3D = False
    Filter = 'wav|*.wav'
    Options = [ofReadOnly, ofHideReadOnly, ofAllowMultiSelect, ofExtensionDifferent, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Upconv_eq'
    Left = 52
    Top = 440
  end
  object ImageList1: TImageList
    Left = 132
    Top = 428
    Bitmap = {
      494C010104000800F40410001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
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
    Left = 172
    Top = 428
    Bitmap = {
      494C010102000800E80410001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
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
    Left = 212
    Top = 428
  end
  object SaveDialog1: TSaveDialog
    Filter = 'eq|*.eq|dat|*.dat'
    Left = 92
    Top = 440
  end
  object OpenDialog1: TOpenDialog
    Filter = 'exe|*.exe'
    Options = [ofReadOnly, ofHideReadOnly, ofEnableSizing]
    Left = 16
    Top = 440
  end
  object Timer2: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer2Timer
    Left = 252
    Top = 428
  end
  object Timer3: TTimer
    Enabled = False
    OnTimer = Timer3Timer
    Left = 293
    Top = 428
  end
  object Timer4: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = Timer4Timer
    Left = 333
    Top = 428
  end
end
