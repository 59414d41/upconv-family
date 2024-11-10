//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <FileCtrl.hpp>
#include <Vcl.Mask.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ImgList.hpp>
#include <shlwapi.h>
#include <shlobj.h>
#include <IOUtils.hpp>
#include <IniFiles.hpp>
#include <system.zip.hpp>
#include "Unit4.h"
#include "Unit7.h"
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include <Vcl.CheckLst.hpp>
#include <Vcl.Menus.hpp>

#define WM_RESET_DP  (1)
#define WM_RELOAD_DP (2)

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TPageControl *pageMain;
	TTabSheet *tabUpconv;
	TLabel *Label1;
	TListView *LvFile;
	TBitBtn *btnAdd;
	TBitBtn *btnDel;
	TBitBtn *btnClear;
	TBitBtn *btnUp;
	TBitBtn *btnDown;
	TBitBtn *btnInfo;
	TLabel *Label2;
	TGroupBox *GroupBox1;
	TComboBox *cmbSampRate;
	TGroupBox *GroupBox2;
	TComboBox *cmbBitWidth;
	TGroupBox *GroupBox5;
	TComboBox *cmbHFA;
	TBevel *Bevel1;
	TGroupBox *GroupBox7;
	TComboBox *cmbDefaultParameter;
	TButton *btnSetting;
	TBevel *Bevel2;
	TLabel *Label3;
	TGroupBox *GroupBox8;
	TGroupBox *GroupBox9;
	TButtonedEdit *edtOutputPath;
	TEdit *edtDefaultParam;
	TOpenDialog *OpenDialog;
	TImageList *ImageList1;
	TImageList *ImageList2;
	TMemo *memoLog;
	TButton *btnReload;
	TTimer *Timer1;
	TBevel *Bevel6;
	TBitBtn *btnExit;
	TBitBtn *btnAbort;
	TBitBtn *btnStart;
	TProgressBar *prgExec;
	TLabel *lbl_exec_status;
	TEdit *edtSaveSuffix;
	TLabel *lbl_count;
	TGroupBox *GroupBox6;
	TComboBox *cmbNormalize;
	TButtonedEdit *edtSuffix;
	TEdit *edtTempParam;
	TPanel *btnParam2;
	TLabel *lbl_message;
	TCheckBox *chkLockSuffix;
	TLabel *lbl_co;
	TMemo *memo_output_files;
	TMemo *memo_output_end_files;
	TMemo *memo_LvFile_backup;
	TBitBtn *btnDP;
	TLabel *lbl_select_directory;
	TBitBtn *btnPause;
	TBitBtn *btnOK_Clear;
	TOpenDialog *OpenDialogZip;
	TEdit *view_information;
	TLabel *Label22;
	TTabSheet *TabSheet4;
	TCategoryPanelGroup *CategoryPanelGroup2;
	TCategoryPanel *CategoryPanel2_1;
	TCategoryPanel *CategoryPanel2_2;
	TLabel *Label18;
	TListView *list_import_list;
	TBitBtn *btn_DPPack;
	TBitBtn *btn_DP_Import;
	TLabel *Label19;
	TMemo *memoDPI;
	TLabel *Label16;
	TListBox *list_dp_active;
	TButton *btn_to_inactive;
	TButton *btn_to_active;
	TLabel *Label17;
	TListBox *list_dp_inactive;
	TTabSheet *TabSheet1;
	TCategoryPanelGroup *CategoryPanelGroup1;
	TCategoryPanel *CategoryPanel1_1;
	TCategoryPanel *CategoryPanel1_2;
	TLabel *Label13;
	TCheckBox *chk_change_default_parameter;
	TLabel *Label15;
	TEdit *edtDefParamCount;
	TUpDown *UpDDefParamCount;
	TCheckBox *chk_conversion_test;
	TComboBox *cmb_ct_min;
	TBevel *Bevel4;
	TLabel *Label12;
	TLabel *Label10;
	TLabel *Label14;
	TComboBox *cmbOutputFormat;
	TComboBox *cmbSplit;
	TLabel *Label11;
	TComboBox *cmbEncoder;
	TCheckBox *chk_save_dp;
	TLabel *lbl_cdp_from_to;
	TLabel *Label4;
	TLabel *Label5;
	TComboBox *cmbThread;
	TLabel *Label7;
	TComboBox *cmbFileIO;
	TLabel *Label6;
	TComboBox *cmbUpconvFile;
	TLabel *labelpri;
	TComboBox *cmbPriority;
	TCheckBox *chkFIO_Ex;
	TComboBox *cmbFileIOMax;
	TBevel *Bevel5;
	TLabel *Label20;
	TGroupBox *grp_portable;
	TRadioButton *rdo_upconv_mode_1;
	TRadioButton *rdo_upconv_mode_2;
	TBevel *Bevel7;
	TLabel *Label9;
	TGroupBox *GroupBox10;
	TCheckBox *chkPowerOff;
	TCheckBox *chkDeleteSourceFile;
	TBevel *Bevel3;
	TLabel *Label24;
	TCheckBox *chk_output_mid_side;
	TCheckBox *chk_output_Left_Right_Center;
	TLabel *Label25;
	TEdit *edt_OutputSize;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox11;
	TBitBtn *btn_ext_add;
	TBitBtn *btn_ext_del;
	TEdit *edt_ext_edit;
	TCheckListBox *clist_extension;
	TCheckBox *chk_output_mp4;
	TBitBtn *btn_level_scan;
	TEdit *edt_volume_level;
	TComboBox *cmb_volume_level;
	TEdit *edt_output_max_size;
	TCheckBox *chkStop;
	TEdit *edtEncoderInfo;
	TMemo *memoEncoderInfo;
	TMemo *memoEncoderExt;
	TPanel *pnlPopup;
	TLabel *Label8;
	TGroupBox *GroupBox3;
	TEdit *edtHFC_Overwrite;
	TGroupBox *GroupBox4;
	TComboBox *cmbOverSamplingOverWrite;
	TCheckBox *chkOS_Overwrite;
	TGroupBox *GroupBox12;
	TCheckBox *chk_ABE_Overrite;
	TCheckBox *chk_PostABE_Overrite;
	TCheckBox *chk_NR_Overrite;
	TCheckBox *chk_MS_Overrite;
	TCheckBox *chk_LRC_Overrite;
	TLabel *Label21;
	TComboBox *cmbADDP;
	TLabel *Label23;
	TGroupBox *GroupBox13;
	TRadioButton *rdo_file_io_sec;
	TComboBox *cmbFIOSecSize;
	TRadioButton *rdo_file_io_mb;
	TComboBox *cmbFIOMBSize;
	TPopupMenu *PopupMenuList;
	TMenuItem *fileHFC;
	TMenuItem *estConversionOffset1;
	TLabel *lbl_addp;
	TMemo *ver;
	TMenuItem *CopyCommandLine1;
	void __fastcall btnExitClick(TObject *Sender);
	void __fastcall btnAddClick(TObject *Sender);
	void __fastcall btnDelClick(TObject *Sender);
	void __fastcall btnUpClick(TObject *Sender);
	void __fastcall btnDownClick(TObject *Sender);
	void __fastcall LvFileSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall btnClearClick(TObject *Sender);
	void __fastcall edtOutputPathLeftButtonClick(TObject *Sender);
	void __fastcall edtOutputPathRightButtonClick(TObject *Sender);
	void __fastcall edtSuffixRightButtonClick(TObject *Sender);
	void __fastcall LvFileChange(TObject *Sender, TListItem *Item, TItemChange Change);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall UpdateProgress(int index,AnsiString status,int persent);
	void __fastcall UpdateADDP(int addp_n,int addp_count);
	void __fastcall UpdateLog(UnicodeString uStr);
	void __fastcall EndExec(int index,DWORD exitCode);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall cmbDefaultParameterChange(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall btnReloadClick(TObject *Sender);
	void __fastcall btnSettingClick(TObject *Sender);
	void __fastcall btnInfoClick(TObject *Sender);
	void __fastcall btnAbortClick(TObject *Sender);
	void __fastcall btnParam2Click(TObject *Sender);
	void __fastcall cmbOverSamplingOverWriteChange(TObject *Sender);
	void __fastcall chkPowerOffClick(TObject *Sender);
	void __fastcall chk_change_default_parameterClick(TObject *Sender);
	void __fastcall cmbSplitChange(TObject *Sender);
	void __fastcall list_dp_activeClick(TObject *Sender);
	void __fastcall list_dp_inactiveClick(TObject *Sender);
	void __fastcall btn_to_inactiveClick(TObject *Sender);
	void __fastcall btn_to_activeClick(TObject *Sender);
	void __fastcall btnDPClick(TObject *Sender);
	void __fastcall btnPauseClick(TObject *Sender);
	void __fastcall btnSortClick(TObject *Sender);
	void __fastcall LvFileColumnClick(TObject *Sender, TListColumn *Column);
	void __fastcall btnOK_ClearClick(TObject *Sender);
	void __fastcall btn_DPPackClick(TObject *Sender);
	void __fastcall btn_DP_ImportClick(TObject *Sender);
	void __fastcall list_import_listChange(TObject *Sender, TListItem *Item, TItemChange Change);
	void __fastcall cmbADDPChange(TObject *Sender);
	void __fastcall edtDefParamCountChange(TObject *Sender);
	void __fastcall chkFIO_ExClick(TObject *Sender);
	void __fastcall CategoryPanel2_2Expand(TObject *Sender);
	void __fastcall CategoryPanel2_1Expand(TObject *Sender);
	void __fastcall CategoryPanel1_1Expand(TObject *Sender);
	void __fastcall CategoryPanel1_2Expand(TObject *Sender);
	void __fastcall chk_output_mid_sideClick(TObject *Sender);
	void __fastcall chk_output_Left_Right_CenterClick(TObject *Sender);
	void __fastcall edt_ext_editChange(TObject *Sender);
	void __fastcall btn_ext_addClick(TObject *Sender);
	void __fastcall clist_extensionClickCheck(TObject *Sender);
	void __fastcall btn_ext_delClick(TObject *Sender);
	void __fastcall btn_level_scanClick(TObject *Sender);
	void __fastcall edt_volume_levelChange(TObject *Sender);
	void __fastcall cmbNormalizeChange(TObject *Sender);
	void __fastcall cmb_volume_levelChange(TObject *Sender);
	void __fastcall cmbSampRateChange(TObject *Sender);
	void __fastcall cmbBitWidthChange(TObject *Sender);
	void __fastcall cmbEncoderChange(TObject *Sender);
	void __fastcall fileHFCClick(TObject *Sender);
	void __fastcall estConversionOffset1Click(TObject *Sender);
	void __fastcall CopyCommandLine1Click(TObject *Sender);


	private:	// ユーザー宣言
	TWndMethod	orgWinProc;
	Exec_c* threadExec[8];
	int cdp_num[256];
	int cdp_num_max;
	int Abort;
	int VisibleIndex;
	int CoDefParamIndex;
	void __fastcall AddToFileList(UnicodeString filename);
	int __fastcall ExecChildProcess(int index);
	void __fastcall ReloadDefaultParameter(int index,int use_index);
	void __fastcall Shutdown(void);
	void __fastcall ListViewWinProc(TMessage& Msg);
	UnicodeString __fastcall GetFilePath(int type,wchar_t *data);
	void __fastcall DeleteFolderAll(UnicodeString folder_name);
	void __fastcall GetDefaultParameterNumberEmptySlot(int *number,int *count);
	void __fastcall UpdateSupportExt(void);
	void __fastcall UpdateFileSize(void);
	AnsiString __fastcall MakeCommandParam(int index,int thread_index,int delete_flag,int *delete_failed);
	void __fastcall DeleteErrorFile(int index);
public:		// ユーザー宣言
	UnicodeString defaultParameterPath;
	UnicodeString defaultParameterEQPath;
	int portableMode;
	__fastcall TForm1(TComponent* Owner);
	void __fastcall ReceiveDPConfigMsg(TMessage & msg);
	void __fastcall ReceiveDPReloadMsg(TMessage & msg);
	void __fastcall ReloadEncoderInfo();
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_APP + WM_RESET_DP, TMessage, ReceiveDPConfigMsg);
	MESSAGE_HANDLER(WM_APP + WM_RELOAD_DP, TMessage, ReceiveDPReloadMsg);
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
