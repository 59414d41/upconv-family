object Form1: TForm1
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'Upconv 1.0.x Default Parameter'
  ClientHeight = 652
  ClientWidth = 450
  Color = clBtnFace
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
  object Label25: TLabel
    Left = 4
    Top = 562
    Width = 105
    Height = 13
    Caption = 'Default Parameter'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 8
    Width = 450
    Height = 549
    ActivePage = TabSheet1
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Upconv'
      ExplicitHeight = 527
      object CategoryPanelGroup1: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 442
        Height = 521
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel8: TCategoryPanel
          Top = 487
          Height = 30
          Caption = 'HFA 2/3 Option 2'
          Collapsed = True
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          OnExpand = CategoryPanel8Expand
          ExplicitTop = 150
          ExpandedHeight = 366
          object ScrollBox1: TScrollBox
            Left = 2
            Top = 6
            Width = 434
            Height = 323
            HorzScrollBar.Visible = False
            VertScrollBar.Smooth = True
            TabOrder = 0
            object Label54: TLabel
              Left = 13
              Top = 6
              Width = 169
              Height = 11
              Caption = 'HFA 3 Analyze Option(Ver 0.8.4)'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = [fsBold]
              ParentFont = False
            end
            object Label52: TLabel
              Left = 10
              Top = 34
              Width = 70
              Height = 11
              Caption = 'HFA3 Freq Start'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label53: TLabel
              Left = 186
              Top = 34
              Width = 64
              Height = 11
              Caption = 'HFA3 Freq Len'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label14: TLabel
              Left = 8
              Top = 69
              Width = 79
              Height = 11
              Caption = 'HFA Analyze Step'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label55: TLabel
              Left = 8
              Top = 122
              Width = 89
              Height = 11
              Caption = 'HFA Level Adj Width'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label56: TLabel
              Left = 8
              Top = 170
              Width = 78
              Height = 11
              Caption = 'HFA Level Adj NX'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label58: TLabel
              Left = 12
              Top = 330
              Width = 241
              Height = 11
              Caption = 'HFA 3 Analyze Option(Ver 0.8.6 - LRC Process)'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = [fsBold]
              ParentFont = False
            end
            object Label59: TLabel
              Left = 14
              Top = 357
              Width = 70
              Height = 11
              Caption = 'HFA3 Freq Start'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label60: TLabel
              Left = 200
              Top = 359
              Width = 64
              Height = 11
              Caption = 'HFA3 Freq Len'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label61: TLabel
              Left = 14
              Top = 400
              Width = 79
              Height = 11
              Caption = 'HFA Analyze Step'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label62: TLabel
              Left = 14
              Top = 453
              Width = 89
              Height = 11
              Caption = 'HFA Level Adj Width'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label63: TLabel
              Left = 14
              Top = 501
              Width = 78
              Height = 11
              Caption = 'HFA Level Adj NX'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Bevel18: TBevel
              Left = 3
              Top = 318
              Width = 401
              Height = 9
              Shape = bsTopLine
            end
            object cmb_HFA3_Freq_Start: TComboBox
              Left = 105
              Top = 23
              Width = 65
              Height = 22
              Style = csOwnerDrawFixed
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ItemIndex = 6
              ParentFont = False
              TabOrder = 0
              Text = '8000'
              Items.Strings = (
                '4000'
                '5000'
                '6000'
                '6500'
                '7000'
                '7500'
                '8000'
                '8500'
                '9000'
                '9500'
                '10000'
                '10500'
                '11000'
                '11500'
                '12000')
            end
            object cmb_HFA3_Freq_Len: TComboBox
              Left = 266
              Top = 23
              Width = 65
              Height = 22
              Style = csOwnerDrawFixed
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ItemIndex = 3
              ParentFont = False
              TabOrder = 1
              Text = '3000'
              Items.Strings = (
                '1500'
                '2000'
                '2500'
                '3000'
                '3500'
                '4000')
            end
            object edt_hfa3_ana_step: TEdit
              Left = 103
              Top = 66
              Width = 49
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
              ReadOnly = True
              TabOrder = 2
              Text = '273'
            end
            object upd_hfa3_ana_step: TUpDown
              Left = 152
              Top = 66
              Width = 16
              Height = 21
              Associate = edt_hfa3_ana_step
              Min = 70
              Max = 300
              Position = 273
              TabOrder = 3
            end
            object edtHFA_LevelAdjWidth: TEdit
              Left = 103
              Top = 118
              Width = 49
              Height = 21
              TabOrder = 4
              Text = '2000'
            end
            object upd_hfa_level_adj_width: TUpDown
              Left = 152
              Top = 118
              Width = 16
              Height = 21
              Associate = edtHFA_LevelAdjWidth
              Min = 1000
              Max = 4000
              Increment = 100
              Position = 2000
              TabOrder = 5
              Thousands = False
            end
            object edtHFA_LevelAdjNX: TEdit
              Left = 103
              Top = 166
              Width = 49
              Height = 21
              TabOrder = 6
              Text = '95'
            end
            object upd_hfa_level_adj_nx: TUpDown
              Left = 152
              Top = 166
              Width = 16
              Height = 21
              Associate = edtHFA_LevelAdjNX
              Min = 10
              Increment = 5
              Position = 95
              TabOrder = 7
            end
            object GroupBox5: TGroupBox
              Left = 178
              Top = 56
              Width = 232
              Height = 41
              Caption = 'Preset Step'
              TabOrder = 8
              object rdo_ana_step_1: TRadioButton
                Left = 8
                Top = 16
                Width = 57
                Height = 17
                Caption = '0.7.x'
                TabOrder = 0
                OnClick = rdo_ana_step_1Click
              end
              object rdo_ana_step_2: TRadioButton
                Left = 60
                Top = 16
                Width = 57
                Height = 17
                Caption = '0.8.2'
                TabOrder = 1
                OnClick = rdo_ana_step_2Click
              end
              object rdo_ana_step_3: TRadioButton
                Left = 112
                Top = 16
                Width = 51
                Height = 17
                Caption = '0.8.4'
                TabOrder = 2
                OnClick = rdo_ana_step_3Click
              end
              object rdo_ana_step_4: TRadioButton
                Left = 169
                Top = 16
                Width = 59
                Height = 17
                Caption = '0.8.6'
                TabOrder = 3
                OnClick = rdo_ana_step_4Click
              end
            end
            object GroupBox7: TGroupBox
              Left = 178
              Top = 104
              Width = 232
              Height = 41
              Caption = 'Preset Width'
              TabOrder = 9
              object rdo_level_width_1: TRadioButton
                Left = 8
                Top = 16
                Width = 97
                Height = 17
                Caption = '0.7.7.4, 0.8.2'
                TabOrder = 0
                OnClick = rdo_level_width_1Click
              end
              object rdo_level_width_2: TRadioButton
                Left = 104
                Top = 18
                Width = 53
                Height = 17
                Caption = '0.8.4'
                TabOrder = 1
                OnClick = rdo_level_width_2Click
              end
            end
            object GroupBox8: TGroupBox
              Left = 178
              Top = 152
              Width = 232
              Height = 41
              Caption = 'Preset NX'
              TabOrder = 10
              object rdo_level_nx_1: TRadioButton
                Left = 8
                Top = 16
                Width = 97
                Height = 17
                Caption = '0.7.7.4, 0.8.2'
                TabOrder = 0
                OnClick = rdo_level_nx_1Click
              end
              object rdo_level_nx_2: TRadioButton
                Left = 104
                Top = 16
                Width = 87
                Height = 17
                Caption = '0.8.4'
                TabOrder = 1
                OnClick = rdo_level_nx_2Click
              end
              object rdo_level_nx_3: TRadioButton
                Left = 169
                Top = 16
                Width = 50
                Height = 17
                Caption = '0.8.6'
                TabOrder = 2
                OnClick = rdo_level_nx_3Click
              end
            end
            object RadioGroup1: TRadioGroup
              Left = 178
              Top = 206
              Width = 232
              Height = 41
              Caption = 'HFA3 post process'
              TabOrder = 11
            end
            object rdo_hfa3_pp_1: TRadioButton
              Left = 186
              Top = 226
              Width = 57
              Height = 17
              Caption = '0.7.7.4'
              TabOrder = 12
            end
            object rdo_hfa3_pp_2: TRadioButton
              Left = 250
              Top = 226
              Width = 73
              Height = 17
              Caption = '0.8.2'
              TabOrder = 13
            end
            object rdo_hfa3_pp_3: TRadioButton
              Left = 306
              Top = 226
              Width = 81
              Height = 17
              Caption = 'default'
              TabOrder = 14
            end
            object GroupBox9: TGroupBox
              Left = 10
              Top = 253
              Width = 400
              Height = 52
              Caption = 'HFA3 Sig Option'
              TabOrder = 15
              object Label57: TLabel
                Left = 8
                Top = 19
                Width = 121
                Height = 13
                AutoSize = False
                Caption = 'HFA3 Sig Level Limit '
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'Tahoma'
                Font.Style = []
                ParentFont = False
              end
              object lbl_hfa3_sig_level_limit: TLabel
                Left = 201
                Top = 19
                Width = 33
                Height = 13
                Caption = '12K Hz'
              end
              object tb_hfa3_sig_level_limit: TTrackBar
                Left = 247
                Top = 16
                Width = 129
                Height = 25
                Max = 16
                Min = 9
                Position = 12
                TabOrder = 0
                OnChange = tb_hfa3_sig_level_limitChange
              end
            end
            object cmb_HFA3_Freq_Start_LRC: TComboBox
              Left = 109
              Top = 352
              Width = 65
              Height = 22
              Style = csOwnerDrawFixed
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ItemIndex = 6
              ParentFont = False
              TabOrder = 16
              Text = '8000'
              Items.Strings = (
                '4000'
                '5000'
                '6000'
                '6500'
                '7000'
                '7500'
                '8000'
                '8500'
                '9000'
                '9500'
                '10000'
                '10500'
                '11000'
                '11500'
                '12000')
            end
            object cmb_HFA3_Freq_Len_LRC: TComboBox
              Left = 280
              Top = 354
              Width = 65
              Height = 22
              Style = csOwnerDrawFixed
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ItemIndex = 3
              ParentFont = False
              TabOrder = 17
              Text = '3000'
              Items.Strings = (
                '1500'
                '2000'
                '2500'
                '3000'
                '3500'
                '4000')
            end
            object edt_hfa3_ana_step_LRC: TEdit
              Left = 109
              Top = 396
              Width = 49
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
              ReadOnly = True
              TabOrder = 18
              Text = '273'
            end
            object upd_hfa3_ana_step_LRC: TUpDown
              Left = 158
              Top = 396
              Width = 16
              Height = 21
              Associate = edt_hfa3_ana_step_LRC
              Min = 70
              Max = 300
              Position = 273
              TabOrder = 19
            end
            object GroupBox6: TGroupBox
              Left = 180
              Top = 386
              Width = 230
              Height = 41
              Caption = 'Preset Step'
              TabOrder = 20
              object rdo_ana_step_1_LRC: TRadioButton
                Left = 8
                Top = 16
                Width = 57
                Height = 17
                Caption = '0.7.x'
                TabOrder = 0
                OnClick = rdo_ana_step_1_LRCClick
              end
              object rdo_ana_step_2_LRC: TRadioButton
                Left = 60
                Top = 16
                Width = 57
                Height = 17
                Caption = '0.8.2'
                TabOrder = 1
                OnClick = rdo_ana_step_2_LRCClick
              end
              object rdo_ana_step_3_LRC: TRadioButton
                Left = 111
                Top = 16
                Width = 53
                Height = 17
                Caption = '0.8.4'
                TabOrder = 2
                OnClick = rdo_ana_step_3_LRCClick
              end
              object rdo_ana_step_4_LRC: TRadioButton
                Left = 165
                Top = 16
                Width = 57
                Height = 17
                Caption = '0.8.6'
                TabOrder = 3
                OnClick = rdo_ana_step_4_LRCClick
              end
            end
            object edtHFA_LevelAdjWidth_LRC: TEdit
              Left = 109
              Top = 448
              Width = 49
              Height = 21
              TabOrder = 21
              Text = '2000'
            end
            object upd_hfa_level_adj_width_LRC: TUpDown
              Left = 158
              Top = 448
              Width = 16
              Height = 21
              Associate = edtHFA_LevelAdjWidth_LRC
              Min = 1000
              Max = 4000
              Increment = 100
              Position = 2000
              TabOrder = 22
              Thousands = False
            end
            object GroupBox10: TGroupBox
              Left = 180
              Top = 435
              Width = 230
              Height = 41
              Caption = 'Preset Width'
              TabOrder = 23
              object rdo_level_width_1_LRC: TRadioButton
                Left = 8
                Top = 16
                Width = 97
                Height = 17
                Caption = '0.7.7.4, 0.8.2'
                TabOrder = 0
                OnClick = rdo_level_width_1_LRCClick
              end
              object rdo_level_width_2_LRC: TRadioButton
                Left = 128
                Top = 16
                Width = 87
                Height = 17
                Caption = '0.8.4'
                TabOrder = 1
                OnClick = rdo_level_width_2_LRCClick
              end
            end
            object edtHFA_LevelAdjNX_LRC: TEdit
              Left = 109
              Top = 496
              Width = 49
              Height = 21
              TabOrder = 24
              Text = '95'
            end
            object upd_hfa_level_adj_nx_LRC: TUpDown
              Left = 158
              Top = 496
              Width = 16
              Height = 21
              Associate = edtHFA_LevelAdjNX_LRC
              Min = 10
              Increment = 5
              Position = 95
              TabOrder = 25
            end
            object GroupBox11: TGroupBox
              Left = 180
              Top = 482
              Width = 230
              Height = 41
              Caption = 'Preset NX'
              TabOrder = 26
              object rdo_level_nx_1_LRC: TRadioButton
                Left = 8
                Top = 16
                Width = 97
                Height = 17
                Caption = '0.7.7.4, 0.8.2'
                TabOrder = 0
                OnClick = rdo_level_nx_1_LRCClick
              end
              object rdo_level_nx_2_LRC: TRadioButton
                Left = 111
                Top = 16
                Width = 50
                Height = 17
                Caption = '0.8.4'
                TabOrder = 1
                OnClick = rdo_level_nx_2_LRCClick
              end
              object rdo_level_nx_3_LRC: TRadioButton
                Left = 165
                Top = 16
                Width = 53
                Height = 17
                Caption = '0.8.6'
                TabOrder = 2
                OnClick = rdo_level_nx_3_LRCClick
              end
            end
            object RadioGroup2: TRadioGroup
              Left = 180
              Top = 526
              Width = 230
              Height = 41
              Caption = 'HFA3 post process'
              TabOrder = 27
            end
            object rdo_hfa3_pp_1_LRC: TRadioButton
              Left = 188
              Top = 543
              Width = 57
              Height = 17
              Caption = '0.7.7.4'
              TabOrder = 28
            end
            object rdo_hfa3_pp_2_LRC: TRadioButton
              Left = 251
              Top = 544
              Width = 73
              Height = 17
              Caption = '0.8.2'
              TabOrder = 29
            end
            object rdo_hfa3_pp_3_LRC: TRadioButton
              Left = 307
              Top = 544
              Width = 81
              Height = 17
              Caption = 'default'
              TabOrder = 30
            end
            object GroupBox12: TGroupBox
              Left = 9
              Top = 573
              Width = 401
              Height = 50
              Caption = 'HFA3 Sig Option'
              TabOrder = 31
              object Label64: TLabel
                Left = 8
                Top = 20
                Width = 121
                Height = 13
                AutoSize = False
                Caption = 'HFA3 Sig Level Limit '
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'Tahoma'
                Font.Style = []
                ParentFont = False
              end
              object lbl_hfa3_sig_level_limit_LRC: TLabel
                Left = 200
                Top = 20
                Width = 33
                Height = 13
                Caption = '12K Hz'
              end
              object tb_hfa3_sig_level_limit_LRC: TTrackBar
                Left = 248
                Top = 17
                Width = 129
                Height = 28
                Max = 16
                Min = 9
                Position = 12
                TabOrder = 0
                OnChange = tb_hfa3_sig_level_limit_LRCChange
              end
            end
            object GroupBox13: TGroupBox
              Left = 9
              Top = 635
              Width = 401
              Height = 84
              Caption = 'Center Channel Option'
              TabOrder = 32
              object Label65: TLabel
                Left = 58
                Top = 28
                Width = 29
                Height = 13
                Caption = 'Phase'
              end
              object Label66: TLabel
                Left = 192
                Top = 28
                Width = 103
                Height = 13
                Caption = 'Center Channel Level'
              end
              object Label75: TLabel
                Left = 44
                Top = 58
                Width = 43
                Height = 11
                Caption = 'Cut Freq'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -9
                Font.Name = 'Tahoma'
                Font.Style = [fsBold]
                ParentFont = False
              end
              object cmb_LRC_CC_Phase: TComboBox
                Left = 104
                Top = 24
                Width = 65
                Height = 22
                Style = csOwnerDrawFixed
                ItemIndex = 17
                TabOrder = 0
                Text = '0.18'
                Items.Strings = (
                  '0.01'
                  '0.02'
                  '0.03'
                  '0.04'
                  '0.05'
                  '0.06'
                  '0.07'
                  '0.08'
                  '0.09'
                  '0.10'
                  '0.11'
                  '0.12'
                  '0.13'
                  '0.14'
                  '0.15'
                  '0.16'
                  '0.17'
                  '0.18'
                  '0.19'
                  '0.20'
                  '0.21'
                  '0.22'
                  '0.23'
                  '0.24'
                  '0.25'
                  '0.26'
                  '0.27'
                  '0.28'
                  '0.29'
                  '0.30'
                  '0.31'
                  '0.32'
                  '0.33'
                  '0.34'
                  '0.35')
              end
              object cmb_LRC_CC_Level: TComboBox
                Left = 312
                Top = 24
                Width = 65
                Height = 22
                Style = csOwnerDrawFixed
                ItemIndex = 11
                TabOrder = 1
                Text = '0.89'
                Items.Strings = (
                  '1.00'
                  '0.99'
                  '0.98'
                  '0.97'
                  '0.96'
                  '0.95'
                  '0.94'
                  '0.93'
                  '0.92'
                  '0.91'
                  '0.90'
                  '0.89'
                  '0.88'
                  '0.87'
                  '0.86'
                  '0.85'
                  '0.84'
                  '0.83'
                  '0.82'
                  '0.81'
                  '0.80'
                  '0.79'
                  '0.78'
                  '0.77'
                  '0.76'
                  '0.75')
              end
              object chkEnableCenterHFC: TCheckBox
                Left = 104
                Top = 56
                Width = 73
                Height = 17
                Caption = 'Enable HFC'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'Tahoma'
                Font.Style = []
                ParentFont = False
                TabOrder = 2
              end
              object edtCenterHFC: TEdit
                Left = 192
                Top = 52
                Width = 57
                Height = 21
                NumbersOnly = True
                TabOrder = 3
              end
            end
          end
        end
        object CategoryPanel4: TCategoryPanel
          Top = 457
          Height = 30
          Caption = 'HFA 2/3 Option 1'
          Collapsed = True
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -9
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          OnCollapse = CategoryPanel4Collapse
          OnExpand = CategoryPanel4Expand
          ExplicitTop = 120
          ExpandedHeight = 369
          object Bevel13: TBevel
            Left = 8
            Top = 31
            Width = 401
            Height = 9
            Shape = bsTopLine
          end
          object Label33: TLabel
            Left = 5
            Top = 9
            Width = 70
            Height = 11
            Caption = 'HFA3 Version'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label47: TLabel
            Left = 5
            Top = 45
            Width = 161
            Height = 13
            AutoSize = False
            Caption = 'HFA3 Analysis Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbl_hfa3_ala: TLabel
            Left = 173
            Top = 65
            Width = 73
            Height = 13
            Alignment = taCenter
            AutoSize = False
            Caption = '0kHz'
            Transparent = False
          end
          object Label48: TLabel
            Left = 21
            Top = 65
            Width = 137
            Height = 13
            AutoSize = False
            Caption = 'HFA3 Analysis Limit Adjust'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
          object Bevel16: TBevel
            Left = 7
            Top = 97
            Width = 401
            Height = 17
            Shape = bsTopLine
          end
          object Label70: TLabel
            Left = 5
            Top = 109
            Width = 161
            Height = 13
            AutoSize = False
            Caption = 'HFA3 FFT Window Size'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label71: TLabel
            Left = 19
            Top = 139
            Width = 38
            Height = 11
            Caption = 'Window'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label72: TLabel
            Left = 269
            Top = 139
            Width = 48
            Height = 11
            Caption = 'Level Adj'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label74: TLabel
            Left = 5
            Top = 168
            Width = 161
            Height = 13
            AutoSize = False
            Caption = 'Preset'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object rdoHFA3_0774: TRadioButton
            Left = 112
            Top = 8
            Width = 81
            Height = 17
            Caption = 'Ver 0.7.7.4'
            Checked = True
            TabOrder = 0
            TabStop = True
            OnClick = rdoHFA3_0774Click
          end
          object rdoHFA3_082: TRadioButton
            Left = 200
            Top = 8
            Width = 73
            Height = 17
            Caption = 'Ver 0.8.2'
            TabOrder = 1
            OnClick = rdoHFA3_082Click
          end
          object track_hfa3_ala: TTrackBar
            Left = 245
            Top = 61
            Width = 150
            Height = 33
            Max = 4
            Min = -4
            TabOrder = 2
            OnChange = track_hfa3_alaChange
          end
          object rdoHFA3_084: TRadioButton
            Left = 280
            Top = 8
            Width = 73
            Height = 17
            Caption = 'Ver 0.8.4'
            TabOrder = 3
            OnClick = rdoHFA3_084Click
          end
          object cmbHFA3_FFT_Window_Size: TComboBox
            Left = 81
            Top = 137
            Width = 106
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 4
            Text = 'Default'
            Items.Strings = (
              'Default'
              '10'
              '12'
              '14'
              '16'
              '18'
              '20'
              '22'
              '24'
              '26'
              '28'
              '30')
          end
          object cmbHFA3_FFT_Window_Lvl: TComboBox
            Left = 330
            Top = 137
            Width = 64
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 5
            Text = 'Default'
            Items.Strings = (
              'Default'
              '4'
              '8'
              '12'
              '16'
              '20')
          end
          object ScrollBox2: TScrollBox
            Left = 4
            Top = 187
            Width = 432
            Height = 151
            TabOrder = 6
            object Label17: TLabel
              Left = 8
              Top = 12
              Width = 33
              Height = 11
              Caption = 'Preset'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = [fsBold]
              ParentFont = False
            end
            object Label18: TLabel
              Left = 192
              Top = 14
              Width = 18
              Height = 11
              Caption = 'Title'
            end
            object Label19: TLabel
              Left = 112
              Top = 45
              Width = 45
              Height = 11
              Caption = 'AvgLineNx'
            end
            object Label20: TLabel
              Left = 256
              Top = 45
              Width = 46
              Height = 11
              Caption = 'Sig1 Phase'
            end
            object Label21: TLabel
              Left = 256
              Top = 78
              Width = 46
              Height = 11
              Caption = 'Sig2 Phase'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label22: TLabel
              Left = 256
              Top = 110
              Width = 46
              Height = 11
              Caption = 'Sig3 Phase'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label23: TLabel
              Left = 16
              Top = 138
              Width = 117
              Height = 13
              Caption = 'Noise Blend(Blend Level)'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label49: TLabel
              Left = 256
              Top = 138
              Width = 45
              Height = 11
              Caption = 'HFA Level'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Bevel6: TBevel
              Left = 8
              Top = 162
              Width = 401
              Height = 9
              Shape = bsTopLine
            end
            object Label24: TLabel
              Left = 8
              Top = 168
              Width = 74
              Height = 11
              Caption = 'HFA 2/3 Filter'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = [fsBold]
              ParentFont = False
            end
            object Label43: TLabel
              Left = 8
              Top = 188
              Width = 58
              Height = 11
              Caption = 'Filter1(19kHz)'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label40: TLabel
              Left = 96
              Top = 188
              Width = 58
              Height = 11
              Caption = 'Filter2(24kHz)'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label41: TLabel
              Left = 184
              Top = 188
              Width = 58
              Height = 11
              Caption = 'Filter3(44kHz)'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object Label42: TLabel
              Left = 272
              Top = 188
              Width = 58
              Height = 11
              Caption = 'Filter4(93kHz)'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
            end
            object cmbHFA_Preset: TComboBox
              Left = 48
              Top = 10
              Width = 129
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 0
              Text = 'Preset01'
              OnChange = cmbHFA_PresetChange
              Items.Strings = (
                'Preset01'
                'Preset02'
                'Preset03'
                'Preset04'
                'Preset05'
                'Preset06'
                'Preset07'
                'Preset08'
                'Preset09'
                'Preset10')
            end
            object edtHFA_PresetTitle: TEdit
              Left = 216
              Top = 10
              Width = 129
              Height = 19
              TabOrder = 1
            end
            object btnHFA_PresetSave: TButton
              Left = 352
              Top = 10
              Width = 57
              Height = 21
              Caption = 'Set'
              TabOrder = 2
              OnClick = btnHFA_PresetSaveClick
            end
            object chkHFA_Sig1_Enable: TCheckBox
              Left = 16
              Top = 45
              Width = 81
              Height = 17
              Caption = 'Sig1 On'
              Checked = True
              State = cbChecked
              TabOrder = 3
            end
            object edtHFA_Sig1_LineX: TEdit
              Left = 176
              Top = 42
              Width = 33
              Height = 19
              TabOrder = 4
              Text = '3'
            end
            object UpD_HFASig1_LineX: TUpDown
              Left = 209
              Top = 42
              Width = 16
              Height = 19
              Associate = edtHFA_Sig1_LineX
              Min = 1
              Max = 25
              Position = 3
              TabOrder = 5
            end
            object edtHFA_Sig1_Phase: TEdit
              Left = 320
              Top = 42
              Width = 33
              Height = 19
              TabOrder = 6
              Text = '-4'
            end
            object UpD_HFA_Sig1Phase: TUpDown
              Left = 353
              Top = 42
              Width = 16
              Height = 19
              Associate = edtHFA_Sig1_Phase
              Min = -44
              Max = 44
              Position = -4
              TabOrder = 7
            end
            object chkHFA_Sig2_Enable: TCheckBox
              Left = 16
              Top = 78
              Width = 81
              Height = 17
              Caption = 'Sig2 On'
              Checked = True
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
              State = cbChecked
              TabOrder = 8
            end
            object edtHFA_Sig2_Phase: TEdit
              Left = 320
              Top = 74
              Width = 33
              Height = 19
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
              TabOrder = 9
              Text = '-1'
            end
            object UpD_HFA_Sig2Phase: TUpDown
              Left = 353
              Top = 74
              Width = 16
              Height = 19
              Associate = edtHFA_Sig2_Phase
              Min = -44
              Max = 44
              Position = -1
              TabOrder = 10
            end
            object chkHFA_Sig3_Enable: TCheckBox
              Left = 16
              Top = 110
              Width = 81
              Height = 17
              Caption = 'Sig3 On'
              Checked = True
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
              State = cbChecked
              TabOrder = 11
            end
            object edtHFA_Sig3_Phase: TEdit
              Left = 320
              Top = 106
              Width = 33
              Height = 19
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
              TabOrder = 12
              Text = '-3'
            end
            object UpD_HFA_Sig3Phase: TUpDown
              Left = 353
              Top = 106
              Width = 16
              Height = 19
              Associate = edtHFA_Sig3_Phase
              Min = -44
              Max = 44
              Position = -3
              TabOrder = 13
            end
            object edtHFA_NoiseBlendLv: TEdit
              Left = 176
              Top = 134
              Width = 33
              Height = 19
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentFont = False
              TabOrder = 14
              Text = '0'
            end
            object UpDHFA_NoiseBlend: TUpDown
              Left = 209
              Top = 134
              Width = 16
              Height = 19
              Associate = edtHFA_NoiseBlendLv
              Increment = 5
              TabOrder = 15
            end
            object cmbHFA_Level: TComboBox
              Left = 320
              Top = 134
              Width = 65
              Height = 22
              Style = csOwnerDrawFixed
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              ItemIndex = 0
              ParentFont = False
              TabOrder = 16
              Text = 'None'
              Items.Strings = (
                'None'
                '1 ( High)'
                '2'
                '3'
                '4'
                '5 (Low)'
                'Auto')
            end
            object edtHFA_Filter1: TEdit
              Left = 15
              Top = 205
              Width = 25
              Height = 19
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              NumbersOnly = True
              ParentFont = False
              TabOrder = 17
              Text = '1'
            end
            object UpDHFA_filter1: TUpDown
              Left = 40
              Top = 205
              Width = 16
              Height = 19
              Associate = edtHFA_Filter1
              Min = 1
              Max = 9
              Position = 1
              TabOrder = 18
            end
            object edtHFA_Filter2: TEdit
              Left = 103
              Top = 205
              Width = 25
              Height = 19
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              NumbersOnly = True
              ParentFont = False
              ReadOnly = True
              TabOrder = 19
              Text = '1'
            end
            object UpDHFA_filter2: TUpDown
              Left = 128
              Top = 205
              Width = 16
              Height = 19
              Associate = edtHFA_Filter2
              Min = 1
              Max = 9
              Position = 1
              TabOrder = 20
            end
            object edtHFA_Filter3: TEdit
              Left = 191
              Top = 205
              Width = 25
              Height = 19
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              NumbersOnly = True
              ParentFont = False
              TabOrder = 21
              Text = '1'
            end
            object UpDHFA_filter3: TUpDown
              Left = 216
              Top = 205
              Width = 16
              Height = 19
              Associate = edtHFA_Filter3
              Min = 1
              Max = 9
              Position = 1
              TabOrder = 22
            end
            object edtHFA_Filter4: TEdit
              Left = 279
              Top = 205
              Width = 25
              Height = 19
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -9
              Font.Name = 'Tahoma'
              Font.Style = []
              NumbersOnly = True
              ParentFont = False
              TabOrder = 23
              Text = '1'
            end
            object UpDHFA_filter4: TUpDown
              Left = 304
              Top = 205
              Width = 16
              Height = 19
              Associate = edtHFA_Filter4
              Min = 1
              Max = 9
              Position = 1
              TabOrder = 24
            end
          end
        end
        object CategoryPanel3: TCategoryPanel
          Top = 427
          Height = 30
          Caption = 'ABE Option'
          Collapsed = True
          Ctl3D = False
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentCtl3D = False
          ParentFont = False
          TabOrder = 2
          OnCollapse = CategoryPanel3Collapse
          OnExpand = CategoryPanel3Expand
          ExplicitTop = 90
          ExpandedHeight = 369
          object Label15: TLabel
            Left = 14
            Top = 57
            Width = 76
            Height = 11
            Caption = 'Low Level Data'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label16: TLabel
            Left = 230
            Top = 105
            Width = 111
            Height = 13
            Caption = 'Cut Dither / Noise Data'
          end
          object Bevel14: TBevel
            Left = 8
            Top = 50
            Width = 401
            Height = 9
            Shape = bsTopLine
          end
          object Label34: TLabel
            Left = 8
            Top = 8
            Width = 161
            Height = 13
            AutoSize = False
            Caption = 'ABE Version'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Bevel15: TBevel
            Left = 8
            Top = 160
            Width = 401
            Height = 9
            Shape = bsTopLine
          end
          object Label37: TLabel
            Left = 8
            Top = 168
            Width = 161
            Height = 13
            AutoSize = False
            Caption = 'Clip Noise Cut'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label39: TLabel
            Left = 8
            Top = 191
            Width = 161
            Height = 13
            AutoSize = False
            Caption = 'Click Noise Cut'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label38: TLabel
            Left = 46
            Top = 131
            Width = 45
            Height = 13
            Caption = 'Cut Level'
          end
          object Bevel19: TBevel
            Left = 8
            Top = 216
            Width = 401
            Height = 9
            Shape = bsTopLine
          end
          object Label73: TLabel
            Left = 8
            Top = 224
            Width = 161
            Height = 13
            AutoSize = False
            Caption = 'ABE 1.0.x Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label76: TLabel
            Left = 45
            Top = 251
            Width = 45
            Height = 13
            Caption = 'Cut Level'
          end
          object Label77: TLabel
            Left = 185
            Top = 251
            Width = 46
            Height = 13
            Caption = 'Cut Mode'
          end
          object chkABE_CutLowLevel: TCheckBox
            Left = 30
            Top = 105
            Width = 201
            Height = 17
            Caption = 'Cut Low Level Data'
            TabOrder = 0
          end
          object chkSmoothingLowLevel: TCheckBox
            Left = 230
            Top = 81
            Width = 193
            Height = 17
            Caption = 'Smoothing Low Level'
            TabOrder = 1
          end
          object edtABE_CutDitherLv: TEdit
            Left = 320
            Top = 129
            Width = 33
            Height = 19
            ReadOnly = True
            TabOrder = 2
            Text = '0'
          end
          object UpD_ABE_CutDitherLv: TUpDown
            Left = 353
            Top = 129
            Width = 16
            Height = 19
            Associate = edtABE_CutDitherLv
            Max = 5
            TabOrder = 3
          end
          object rdoABE_0774: TRadioButton
            Left = 30
            Top = 27
            Width = 113
            Height = 17
            Caption = 'Ver 0774'
            Checked = True
            TabOrder = 4
            TabStop = True
            OnClick = rdoABE_0774Click
          end
          object rdoABE_080: TRadioButton
            Left = 110
            Top = 27
            Width = 113
            Height = 17
            Caption = 'Ver 0.8.2/4'
            TabOrder = 5
            OnClick = rdoABE_080Click
          end
          object chkABE_DeClip: TCheckBox
            Left = 110
            Top = 165
            Width = 153
            Height = 17
            Caption = 'Clip Noise Reduction'
            TabOrder = 6
          end
          object chkAdFilter: TCheckBox
            Left = 30
            Top = 81
            Width = 161
            Height = 17
            Caption = 'ABE Adaptive Filter'
            TabOrder = 7
          end
          object chkClickNoiseReduction: TCheckBox
            Left = 110
            Top = 188
            Width = 153
            Height = 17
            Caption = 'Click Noise Reduction'
            TabOrder = 8
          end
          object edtCLLD_Level: TEdit
            Left = 110
            Top = 129
            Width = 33
            Height = 19
            NumbersOnly = True
            TabOrder = 9
            Text = '8'
          end
          object UpD_CLLD_Level: TUpDown
            Left = 143
            Top = 129
            Width = 16
            Height = 19
            Associate = edtCLLD_Level
            Min = 1
            Max = 10
            Position = 8
            TabOrder = 10
          end
          object rdoABE_086: TRadioButton
            Left = 209
            Top = 27
            Width = 72
            Height = 17
            Caption = 'Ver 0.8.6'
            TabOrder = 11
          end
          object rdoABE_10x: TRadioButton
            Left = 287
            Top = 27
            Width = 100
            Height = 17
            Caption = 'Ver 1.0.x'
            TabOrder = 12
          end
          object edtCutLevel: TEdit
            Left = 110
            Top = 247
            Width = 33
            Height = 19
            NumbersOnly = True
            TabOrder = 13
            Text = '1'
          end
          object UpD_CutLevel: TUpDown
            Left = 143
            Top = 247
            Width = 16
            Height = 19
            Associate = edtCutLevel
            Max = 20
            Position = 1
            TabOrder = 14
          end
          object cmbCutMode: TComboBox
            Left = 246
            Top = 248
            Width = 89
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 15
            Text = 'Low'
            Items.Strings = (
              'Low'
              'Medium'
              'High')
          end
        end
        object CategoryPanel10: TCategoryPanel
          Top = 397
          Height = 30
          Caption = 'Option3'
          Collapsed = True
          TabOrder = 3
          OnCollapse = CategoryPanel10Collapse
          OnExpand = CategoryPanel10Expand
          ExplicitTop = 60
          ExpandedHeight = 369
          object Label9: TLabel
            Left = 5
            Top = 6
            Width = 90
            Height = 13
            Caption = 'Encorder Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label10: TLabel
            Left = 19
            Top = 28
            Width = 25
            Height = 13
            Caption = 'FLAC'
          end
          object Label11: TLabel
            Left = 19
            Top = 56
            Width = 44
            Height = 13
            Caption = 'WavPack'
          end
          object Label12: TLabel
            Left = 19
            Top = 83
            Width = 20
            Height = 13
            Caption = 'MP3'
          end
          object Label44: TLabel
            Left = 19
            Top = 111
            Width = 27
            Height = 13
            Caption = 'OPUS'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
          object Label35: TLabel
            Left = 19
            Top = 137
            Width = 21
            Height = 13
            Caption = 'M4A'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
          object Bevel4: TBevel
            Left = 10
            Top = 163
            Width = 381
            Height = 3
          end
          object Label50: TLabel
            Left = 7
            Top = 172
            Width = 136
            Height = 13
            Caption = 'Encorder Custom Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label51: TLabel
            Left = 90
            Top = 195
            Width = 20
            Height = 13
            Caption = 'Title'
          end
          object Label67: TLabel
            Left = 71
            Top = 226
            Width = 39
            Height = 13
            Caption = 'Encoder'
          end
          object Label68: TLabel
            Left = 91
            Top = 237
            Width = 16
            Height = 13
            Caption = 'Ext'
          end
          object Label69: TLabel
            Left = 60
            Top = 262
            Width = 50
            Height = 13
            Caption = 'Parameter'
          end
          object edtEncorder_FLAC: TButtonedEdit
            Left = 88
            Top = 27
            Width = 303
            Height = 21
            TabOrder = 0
            Text = '-s -f --best'
            OnKeyPress = edtEncorder_FLACKeyPress
          end
          object edtEncorder_WavPack: TButtonedEdit
            Left = 88
            Top = 54
            Width = 303
            Height = 21
            TabOrder = 1
            Text = '-hh -q'
            OnKeyPress = edtEncorder_WavPackKeyPress
          end
          object edtEncorder_MP3: TButtonedEdit
            Left = 88
            Top = 81
            Width = 303
            Height = 21
            TabOrder = 2
            Text = '-V 0 -b 256'
            OnKeyPress = edtEncorder_MP3KeyPress
          end
          object edtEncorder_OPUS: TButtonedEdit
            Left = 88
            Top = 109
            Width = 303
            Height = 21
            TabOrder = 3
            Text = '--vbr'
            OnKeyPress = edtEncorder_MP3KeyPress
          end
          object edtEncorder_M4A: TButtonedEdit
            Left = 88
            Top = 136
            Width = 303
            Height = 21
            TabOrder = 4
            Text = '-b:a 256k'
            OnKeyPress = edtEncorder_MP3KeyPress
          end
          object cmbEncoderCustom: TComboBox
            Left = 10
            Top = 191
            Width = 41
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 5
            Text = '1'
            OnChange = cmbEncoderCustomChange
            Items.Strings = (
              '1')
          end
          object edtEncoderCustomTitle: TButtonedEdit
            Left = 128
            Top = 191
            Width = 169
            Height = 21
            Images = ImageList1
            RightButton.ImageIndex = 1
            RightButton.Visible = True
            TabOrder = 6
            OnChange = edtEncoderCustomTitleChange
          end
          object edtEncoderCustomParam: TEdit
            Left = 128
            Top = 259
            Width = 263
            Height = 21
            TabOrder = 7
            OnChange = edtEncoderCustomParamChange
          end
          object btnEncoderCustomSave: TButton
            Left = 309
            Top = 190
            Width = 36
            Height = 23
            Caption = 'Save'
            TabOrder = 8
            OnClick = btnEncoderCustomSaveClick
          end
          object btnEncoderCustomDelete: TButton
            Left = 355
            Top = 190
            Width = 36
            Height = 23
            Caption = 'Del'
            TabOrder = 9
            OnClick = btnEncoderCustomDeleteClick
          end
          object cmbEncoderCustomPg: TComboBox
            Left = 128
            Top = 226
            Width = 168
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 10
            Text = 'ffmpeg.exe'
            OnChange = cmbEncoderCustomPgChange
            Items.Strings = (
              'ffmpeg.exe'
              'lame.exe')
          end
          object cmbEncoderCustomExt: TComboBox
            Left = 309
            Top = 226
            Width = 82
            Height = 22
            Style = csOwnerDrawFixed
            TabOrder = 11
            OnChange = cmbEncoderCustomExtChange
          end
        end
        object CategoryPanel2: TCategoryPanel
          Top = 367
          Height = 30
          Caption = 'Option2'
          Collapsed = True
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 4
          OnCollapse = CategoryPanel2Collapse
          OnExpand = CategoryPanel2Expand
          ExplicitTop = 30
          ExpandedHeight = 367
          object Label8: TLabel
            Left = 8
            Top = 229
            Width = 74
            Height = 13
            Caption = 'De-emphasis'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label36: TLabel
            Left = 19
            Top = 29
            Width = 80
            Height = 13
            Caption = 'HFC Auto Adjust'
          end
          object Label45: TLabel
            Left = 19
            Top = 58
            Width = 96
            Height = 13
            Caption = 'Volume Level Adjust'
          end
          object Label46: TLabel
            Left = 270
            Top = 57
            Width = 33
            Height = 13
            AutoSize = False
            Caption = '%'
          end
          object Label7: TLabel
            Left = 207
            Top = 96
            Width = 57
            Height = 13
            Caption = 'Dither Level'
          end
          object Label6: TLabel
            Left = 8
            Top = 8
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
          object Bevel2: TBevel
            Left = 9
            Top = 131
            Width = 401
            Height = 1
            Shape = bsTopLine
          end
          object Bevel9: TBevel
            Left = 8
            Top = 215
            Width = 401
            Height = 9
            Shape = bsTopLine
          end
          object Label13: TLabel
            Left = 8
            Top = 146
            Width = 50
            Height = 11
            Caption = 'EQ Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object rdoDeE2: TRadioButton
            Left = 199
            Top = 242
            Width = 113
            Height = 17
            Caption = '50/15us(CD-DA)'
            TabOrder = 0
          end
          object rdoDeE3: TRadioButton
            Left = 318
            Top = 242
            Width = 73
            Height = 17
            Caption = 'CCITT J.17'
            TabOrder = 1
          end
          object rdoDeE1: TRadioButton
            Left = 136
            Top = 242
            Width = 65
            Height = 17
            Caption = 'None'
            Checked = True
            TabOrder = 2
            TabStop = True
          end
          object cmbHFC_AutoAdjust: TComboBox
            Left = 207
            Top = 26
            Width = 135
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 3
            Text = 'None'
            Items.Strings = (
              'None'
              'HFC Auto ( -1kHz)'
              'HFC Auto ( -2kHz)'
              'HFC Auto ( -3kHz)'
              'HFC Auto ( -4kHz)'
              'HFC Auto ( -5kHz)'
              'HFC Auto ( -6kHz)'
              'HFC Auto ( -7kHz)'
              'HFC Auto ( -8kHz)'
              'HFC Auto ( -9kHz)')
          end
          object cmbVolumeLevelAdjust: TComboBox
            Left = 207
            Top = 54
            Width = 57
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 4
            Text = '100'
            Items.Strings = (
              '100'
              '95'
              '90'
              '85'
              '80'
              '75'
              '70'
              '65'
              '60'
              '55'
              '50'
              'Auto')
          end
          object cmbOutputDither: TComboBox
            Left = 19
            Top = 93
            Width = 169
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 5
            Text = 'Cutoff'
            Items.Strings = (
              'Cutoff'
              'Dithering'
              'Noise Shaping'
              'Error Diffusion Methos')
          end
          object edtOutDitherLv: TEdit
            Left = 291
            Top = 93
            Width = 33
            Height = 21
            ReadOnly = True
            TabOrder = 6
            Text = '0'
          end
          object UpD_OutLv: TUpDown
            Left = 324
            Top = 93
            Width = 16
            Height = 21
            Associate = edtOutDitherLv
            Max = 16
            TabOrder = 7
          end
          object memo_eq_title: TMemo
            Left = 9
            Top = 168
            Width = 65
            Height = 25
            Enabled = False
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 8
            Visible = False
            WordWrap = False
          end
          object chkPreEQ: TCheckBox
            Left = 97
            Top = 146
            Width = 73
            Height = 17
            Caption = 'Pre EQ'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 9
          end
          object chkPostEQ: TCheckBox
            Left = 97
            Top = 176
            Width = 65
            Height = 17
            Caption = 'Post EQ'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 10
          end
          object cmbPreEQ: TComboBox
            Left = 177
            Top = 144
            Width = 49
            Height = 22
            Style = csOwnerDrawFixed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 11
            OnChange = cmbPreEQChange
          end
          object cmbPostEQ: TComboBox
            Left = 177
            Top = 176
            Width = 49
            Height = 22
            Style = csOwnerDrawFixed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 12
            OnChange = cmbPostEQChange
          end
          object edt_PreEQ_Title: TEdit
            Left = 241
            Top = 144
            Width = 169
            Height = 21
            Enabled = False
            ReadOnly = True
            TabOrder = 13
          end
          object edt_PostEQ_Title: TEdit
            Left = 241
            Top = 176
            Width = 169
            Height = 21
            Enabled = False
            ReadOnly = True
            TabOrder = 14
          end
        end
        object CategoryPanel1: TCategoryPanel
          Top = 0
          Height = 367
          Caption = 'Option1'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 5
          OnCollapse = CategoryPanel1Collapse
          OnExpand = CategoryPanel1Expand
          object Label1: TLabel
            Left = 8
            Top = 8
            Width = 76
            Height = 11
            Caption = 'Convert Option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label2: TLabel
            Left = 17
            Top = 161
            Width = 66
            Height = 13
            Caption = 'OverSampling'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
          object Label3: TLabel
            Left = 8
            Top = 262
            Width = 79
            Height = 11
            Caption = 'Noise Reduction'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label5: TLabel
            Left = 289
            Top = 261
            Width = 42
            Height = 13
            Caption = 'NR Level'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
          object Bevel3: TBevel
            Left = 9
            Top = 241
            Width = 401
            Height = 10
            Shape = bsTopLine
          end
          object Bevel5: TBevel
            Left = 9
            Top = 193
            Width = 401
            Height = 10
            Shape = bsTopLine
          end
          object Label4: TLabel
            Left = 9
            Top = 211
            Width = 43
            Height = 11
            Caption = 'Cut Freq'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -9
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object chkABE: TCheckBox
            Left = 16
            Top = 24
            Width = 169
            Height = 17
            Caption = 'Adjust Bit Extension'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
          end
          object chkPostABE: TCheckBox
            Left = 207
            Top = 25
            Width = 177
            Height = 17
            Caption = 'Post ABE'
            TabOrder = 1
          end
          object chkCutHighPassDither: TCheckBox
            Left = 16
            Top = 48
            Width = 169
            Height = 17
            Caption = 'Cut High Pass Dither'
            TabOrder = 2
          end
          object chkLowAdjust: TCheckBox
            Left = 207
            Top = 73
            Width = 177
            Height = 17
            Caption = 'Low Adjust (100Hz)'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 3
          end
          object chkHFC_Auto: TCheckBox
            Left = 16
            Top = 72
            Width = 97
            Height = 17
            Caption = 'HFC Auto'
            TabOrder = 4
          end
          object cmbOverSamp: TComboBox
            Left = 105
            Top = 157
            Width = 304
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 5
            Text = 'None'
            Items.Strings = (
              'None'
              'Over Sampling ( x2)'
              'Over Sampling (192000)'
              'Over Sampling (384000)'
              'Over Sampling (768000)'
              'Over Sampling (1536000)'
              'Over Sampling (3072000)')
          end
          object chkNREnable: TCheckBox
            Left = 97
            Top = 259
            Width = 73
            Height = 17
            Caption = 'Enable NR'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 6
          end
          object edtNR_Freq: TEdit
            Left = 185
            Top = 257
            Width = 57
            Height = 21
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            NumbersOnly = True
            ParentFont = False
            TabOrder = 7
          end
          object edtNR_Level: TEdit
            Left = 345
            Top = 257
            Width = 42
            Height = 21
            ReadOnly = True
            TabOrder = 8
            Text = '1'
          end
          object upD_NR_Level: TUpDown
            Left = 387
            Top = 257
            Width = 16
            Height = 21
            Associate = edtNR_Level
            Min = 1
            Max = 5
            Position = 1
            TabOrder = 9
          end
          object edtHFC: TEdit
            Left = 185
            Top = 209
            Width = 57
            Height = 21
            NumbersOnly = True
            TabOrder = 10
          end
          object edtLFC: TEdit
            Left = 345
            Top = 209
            Width = 56
            Height = 21
            NumbersOnly = True
            TabOrder = 11
          end
          object chkEnableHFC: TCheckBox
            Left = 97
            Top = 209
            Width = 73
            Height = 17
            Caption = 'Enable HFC'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 12
          end
          object chkEnableLFC: TCheckBox
            Left = 265
            Top = 209
            Width = 73
            Height = 17
            Caption = 'Enable LFC'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 13
          end
          object chkPostNR: TCheckBox
            Left = 207
            Top = 48
            Width = 97
            Height = 17
            Caption = 'Post NR'
            TabOrder = 14
          end
          object chk_ms_process: TCheckBox
            Left = 16
            Top = 96
            Width = 313
            Height = 17
            Caption = 'Mid/Side Process'
            TabOrder = 15
          end
          object cmbMidSide: TComboBox
            Left = 207
            Top = 95
            Width = 202
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 16
            Text = 'Normal'
            Items.Strings = (
              'Normal'
              'Wide'
              'Cutoff'
              'Cutoff (Dynamic)'
              'Cutoff (Dynamic) + SFA')
          end
          object chk_LRC_process: TCheckBox
            Left = 16
            Top = 120
            Width = 169
            Height = 17
            Caption = 'Left / Right / Center (LRC)'
            TabOrder = 17
          end
          object cmb_MS_LRC: TComboBox
            Left = 207
            Top = 123
            Width = 202
            Height = 22
            Style = csOwnerDrawFixed
            ItemIndex = 0
            TabOrder = 18
            Text = 'Mid / Side -> Left / Right  / Center'
            Items.Strings = (
              'Mid / Side -> Left / Right  / Center'
              'Left / Right / Center -> Mid / Side')
          end
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Multi Channel'
      ImageIndex = 1
      ExplicitHeight = 527
      object CategoryPanelGroup2: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 442
        Height = 521
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel5: TCategoryPanel
          Top = 0
          Height = 519
          Caption = 'Multi Channel Option'
          TabOrder = 0
          OnCollapse = CategoryPanel5Collapse
          object Label26: TLabel
            Left = 8
            Top = 8
            Width = 38
            Height = 13
            Caption = 'Center'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Bevel1: TBevel
            Left = 8
            Top = 64
            Width = 401
            Height = 9
            Shape = bsTopLine
          end
          object Label27: TLabel
            Left = 8
            Top = 72
            Width = 52
            Height = 13
            Caption = 'Surround'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Bevel7: TBevel
            Left = 8
            Top = 128
            Width = 401
            Height = 9
            Shape = bsTopLine
          end
          object Label28: TLabel
            Left = 8
            Top = 136
            Width = 18
            Height = 13
            Caption = 'LFE'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label29: TLabel
            Left = 8
            Top = 200
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
          object Bevel8: TBevel
            Left = 8
            Top = 192
            Width = 401
            Height = 9
            Shape = bsTopLine
          end
          object chkMC_Center: TCheckBox
            Left = 16
            Top = 32
            Width = 249
            Height = 17
            Caption = 'Generate Center Channel'
            TabOrder = 0
          end
          object chkMC_Surround: TCheckBox
            Left = 16
            Top = 96
            Width = 297
            Height = 17
            Caption = 'Generate Surround Left/Right'
            TabOrder = 1
          end
          object chkMC_LFE: TCheckBox
            Left = 16
            Top = 160
            Width = 353
            Height = 17
            Caption = 'Generate LFE'
            TabOrder = 2
          end
          object chkMC_Mono: TCheckBox
            Left = 16
            Top = 224
            Width = 281
            Height = 17
            Caption = 'Output Monaural File'
            TabOrder = 3
          end
          object chkMC_DownMix: TCheckBox
            Left = 16
            Top = 256
            Width = 313
            Height = 17
            Caption = 'Down Mix'
            TabOrder = 4
          end
          object chkMC_EchoEffect: TCheckBox
            Left = 16
            Top = 292
            Width = 105
            Height = 17
            Caption = 'Echo Effect'
            TabOrder = 5
          end
          object edtMCEchoEffect: TEdit
            Left = 144
            Top = 288
            Width = 33
            Height = 21
            TabOrder = 6
            Text = '1'
          end
          object UpDMC_EchoEffect: TUpDown
            Left = 177
            Top = 288
            Width = 16
            Height = 21
            Associate = edtMCEchoEffect
            Min = 1
            Max = 5
            Position = 1
            TabOrder = 7
          end
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Common Option'
      ImageIndex = 2
      ExplicitHeight = 527
      object CategoryPanelGroup3: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 442
        Height = 521
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel6: TCategoryPanel
          Top = 0
          Height = 519
          Caption = 'Option'
          TabOrder = 0
          OnCollapse = CategoryPanel6Collapse
          object Label30: TLabel
            Left = 8
            Top = 8
            Width = 81
            Height = 13
            Caption = 'Work File Path'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object edtWorkPath: TButtonedEdit
            Left = 24
            Top = 32
            Width = 377
            Height = 21
            Images = ImageList1
            LeftButton.ImageIndex = 0
            LeftButton.Visible = True
            ReadOnly = True
            RightButton.ImageIndex = 1
            TabOrder = 0
            OnLeftButtonClick = edtWorkPathLeftButtonClick
            OnRightButtonClick = edtWorkPathRightButtonClick
          end
        end
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Overwrite Option'
      ImageIndex = 3
      ExplicitHeight = 527
      object CategoryPanelGroup4: TCategoryPanelGroup
        Left = 0
        Top = 0
        Width = 442
        Height = 521
        VertScrollBar.Tracking = True
        Align = alClient
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'Tahoma'
        HeaderFont.Style = []
        TabOrder = 0
        object CategoryPanel7: TCategoryPanel
          Top = 0
          Height = 519
          Caption = 'Option'
          TabOrder = 0
          object Label32: TLabel
            Left = 8
            Top = 8
            Width = 84
            Height = 13
            Caption = 'Convert option'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Bevel10: TBevel
            Left = 8
            Top = 232
            Width = 401
            Height = 10
            Shape = bsTopLine
          end
          object Bevel11: TBevel
            Left = 8
            Top = 88
            Width = 401
            Height = 10
            Shape = bsTopLine
          end
          object Bevel12: TBevel
            Left = 8
            Top = 160
            Width = 401
            Height = 10
            Shape = bsTopLine
          end
          object Label31: TLabel
            Left = 8
            Top = 240
            Width = 321
            Height = 13
            AutoSize = False
            Caption = 'Suffix'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object chkConvOptionOverWrite: TCheckBox
            Left = 320
            Top = 46
            Width = 97
            Height = 17
            Caption = 'Overwrite'
            TabOrder = 0
          end
          object GroupBox1: TGroupBox
            Left = 16
            Top = 24
            Width = 97
            Height = 49
            Caption = 'Sampling rate'
            TabOrder = 1
            object cmbSamplingOverWrite: TComboBox
              Left = 8
              Top = 19
              Width = 73
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 0
              Text = '32000'
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
                '768000')
            end
          end
          object GroupBox2: TGroupBox
            Left = 128
            Top = 24
            Width = 89
            Height = 49
            Caption = 'Bit Width'
            TabOrder = 2
            object cmbBitWidthOverWrite: TComboBox
              Left = 8
              Top = 19
              Width = 65
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 0
              Text = '16'
              Items.Strings = (
                '16'
                '24'
                'Float(32)'
                'Float(64)'
                'Int(32)')
            end
          end
          object GroupBox3: TGroupBox
            Left = 16
            Top = 96
            Width = 201
            Height = 49
            Caption = 'HFA'
            TabOrder = 3
            object cmbHFA: TComboBox
              Left = 8
              Top = 16
              Width = 177
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 0
              Text = 'cutoff(hfa0)'
              Items.Strings = (
                'cutoff(hfa0)'
                'Noise(hfa1)'
                'OverTone(hfa2)'
                'OverTone(hfa3)')
            end
          end
          object chkNormalizeOverWrite: TCheckBox
            Left = 320
            Top = 184
            Width = 97
            Height = 17
            Caption = 'Overwrite'
            TabOrder = 4
          end
          object GroupBox4: TGroupBox
            Left = 16
            Top = 168
            Width = 201
            Height = 49
            Caption = 'Normalize'
            TabOrder = 5
            object cmbNormalize: TComboBox
              Left = 8
              Top = 16
              Width = 177
              Height = 22
              Style = csOwnerDrawFixed
              ItemIndex = 0
              TabOrder = 0
              Text = 'None'
              Items.Strings = (
                'None'
                'Normalize(File)')
            end
          end
          object chkSuffix: TCheckBox
            Left = 320
            Top = 260
            Width = 81
            Height = 17
            Caption = 'Overwrite'
            TabOrder = 6
          end
          object edtSuffix: TButtonedEdit
            Left = 24
            Top = 264
            Width = 161
            Height = 21
            Images = ImageList1
            RightButton.ImageIndex = 1
            RightButton.Visible = True
            TabOrder = 7
            Text = 'upconv'
            OnKeyPress = edtSuffixKeyPress
            OnRightButtonClick = edtSuffixRightButtonClick
          end
          object chkHFC_OverWrite: TCheckBox
            Left = 320
            Top = 112
            Width = 97
            Height = 17
            Caption = 'Overwrite'
            TabOrder = 8
          end
        end
      end
    end
  end
  object cmbDefaultParameter: TComboBox
    Left = 4
    Top = 585
    Width = 41
    Height = 22
    Style = csOwnerDrawFixed
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnChange = cmbDefaultParameterChange
  end
  object edtTitle: TEdit
    Left = 52
    Top = 585
    Width = 377
    Height = 21
    TabOrder = 2
    OnKeyPress = edtTitleKeyPress
  end
  object btnSave: TButton
    Left = 92
    Top = 621
    Width = 73
    Height = 25
    Caption = 'Save'
    Enabled = False
    TabOrder = 3
    OnClick = btnSaveClick
  end
  object btnLoad: TButton
    Left = 4
    Top = 621
    Width = 73
    Height = 25
    Caption = 'Load'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    OnClick = btnLoadClick
  end
  object btnDelete: TButton
    Left = 180
    Top = 621
    Width = 73
    Height = 25
    Caption = 'Delete'
    Enabled = False
    TabOrder = 5
    OnClick = btnDeleteClick
  end
  object btnExit: TBitBtn
    Left = 364
    Top = 621
    Width = 59
    Height = 25
    Caption = 'Exit'
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00388888888877
      F7F787F8888888888333333F00004444400888FFF444448888888888F333FF8F
      000033334D5007FFF4333388888888883338888F0000333345D50FFFF4333333
      338F888F3338F33F000033334D5D0FFFF43333333388788F3338F33F00003333
      45D50FEFE4333333338F878F3338F33F000033334D5D0FFFF43333333388788F
      3338F33F0000333345D50FEFE4333333338F878F3338F33F000033334D5D0FFF
      F43333333388788F3338F33F0000333345D50FEFE4333333338F878F3338F33F
      000033334D5D0EFEF43333333388788F3338F33F0000333345D50FEFE4333333
      338F878F3338F33F000033334D5D0EFEF43333333388788F3338F33F00003333
      4444444444333333338F8F8FFFF8F33F00003333333333333333333333888888
      8888333F00003333330000003333333333333FFFFFF3333F00003333330AAAA0
      333333333333888888F3333F00003333330000003333333333338FFFF8F3333F
      0000}
    NumGlyphs = 2
    TabOrder = 6
    OnClick = btnExitClick
  end
  object edtHFA_Preset: TMemo
    Left = 448
    Top = 191
    Width = 925
    Height = 161
    TabOrder = 7
    WantReturns = False
    WordWrap = False
  end
  object btn_folder_open: TButton
    Left = 268
    Top = 621
    Width = 73
    Height = 25
    Caption = 'Folder Open'
    TabOrder = 8
    OnClick = btn_folder_openClick
  end
  object Memo1: TMemo
    Left = 456
    Top = 557
    Width = 185
    Height = 89
    TabOrder = 9
  end
  object edtParameter: TMemo
    Left = 448
    Top = 32
    Width = 453
    Height = 153
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -9
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 10
    WantReturns = False
    WordWrap = False
  end
  object edtPTitle: TMemo
    Left = 448
    Top = 363
    Width = 925
    Height = 143
    TabOrder = 11
    WordWrap = False
  end
  object edtParameter2: TMemo
    Left = 907
    Top = 30
    Width = 466
    Height = 155
    ScrollBars = ssBoth
    TabOrder = 12
  end
  object ImageList1: TImageList
    Left = 396
    Top = 8
    Bitmap = {
      494C010102000800E00210001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
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
end
