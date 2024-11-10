// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <StrUtils.hpp>
#include <IOUtils.hpp>
#include <time.h>
#include "Unit1.h"
#include "Unit5.h"
#include "Unit6.h"
#include "..\\upconv_src\\upconv\\upconv.h"
#include "..\\upconv_src\\PLG_AUDIO_IO\\PLG_AudioIO.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnExitClick(TObject *Sender) {
	Close();
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::ReceiveDPConfigMsg(TMessage & msg)
{
	btnReload->Tag = 1;
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::ReceiveDPReloadMsg(TMessage & msg)
{
	btnReloadClick(NULL);
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender) {
	FILE *fp;
	TIniFile *iniFile;
	long sampRate, bitWidth, hfc;
	int hfa, norm, norm_opt;
	int fileio, thread, upconv_max;
	int fio_sec_size,fio_mb_size,fio_sec_mb;
	int bwf, rf64, w64, wav2g,wav_auto;
	int def_param;
	int libsndfile;
	int encMode;
	int i,ii,n;
	int cpu_pri;
	int winLeft, winTop;
	int lock_suffix;
	int co_enable, co_count;
	int split_index;
	int save_dp;
	int addp;
	int portable_possible;
	int ct_sec;
	int fileio_auto;
	int ffmpeg_found;
	int volume_level;
	int volume_per;
	long fileio_max;
	char buffer[4096];
	wchar_t file_path[2048];
	TStringList *ext_list = NULL;
	ext_list = new TStringList;

	SYSTEM_INFO sysinfo;
	UnicodeString uFileName;
	UnicodeString cmdName;
	UnicodeString uExtFileName;
	AnsiString tmpDrive;
	defaultParameterPath   = L"C:\\ProgramData\\upconv";
	defaultParameterEQPath = L"C:\\ProgramData\\upconv_eq";
	wav2g = 0;
	portableMode = 0;
	portable_possible = 1;
	libsndfile = 0;
	ffmpeg_found = 0;
	fio_sec_size = 10;
	fio_mb_size  = 5;
	fio_sec_mb   = 1;
	GetNativeSystemInfo(&sysinfo);
//	if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
//		sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
//		Form1->Caption = Form1->Caption + L" (x64)";
//	}
	orgWinProc = LvFile->WindowProc;
	LvFile->WindowProc = ListViewWinProc;
	DragAcceptFiles(LvFile->Handle, true);

	ext_list->Add(L"wav");
	ext_list->Add(L"dsf");

	// ffmpeg
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"flac.exe";
	if (PathFileExists(cmdName.c_str())) {
		cmbEncoder->Items->Add(L"FLAC");
	} else {
		cmbEncoder->Items->Add(L"FLAC: Unavailable");
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"wavpack.exe";
	if (PathFileExists(cmdName.c_str())) {
		cmbEncoder->Items->Add(L"WavPack");
	} else {
		cmbEncoder->Items->Add(L"WavPack: Unavailable");
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"lame.exe";
	if (PathFileExists(cmdName.c_str())) {
		cmbEncoder->Items->Add(L"Lame(MP3)");
	} else {
		cmbEncoder->Items->Add(L"Lame(MP3): Unavailable");
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"opusenc.exe";
	if (PathFileExists(cmdName.c_str())) {
		cmbEncoder->Items->Add(L"OPUS");
	} else {
		cmbEncoder->Items->Add(L"OPUS: Unavailable");
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		ffmpeg_found = 1;
		cmbEncoder->Items->Add(L"ffmpeg(M4A)");
	} else {
		cmbEncoder->Items->Add(L"ffmpeg(M4A): Unavailable");
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"libsndfile-1.dll";
	if (PathFileExists(cmdName.c_str())) {
		libsndfile = 0x01;
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"sndfile-convert.exe";
	if (PathFileExists(cmdName.c_str())) {
		libsndfile |= 0x02;
	}
	if (libsndfile == 0x03) {
		cmbOutputFormat->Items->Add("W64");
		cmbOutputFormat->Tag = 1;
	} else {
		cmbOutputFormat->Items->Add("W64(Not Support)");
		cmbOutputFormat->Tag = 2;
	}
	cmbOutputFormat->Items->Add("Auto(WAV or RF64)");

	if (ExtractFilePath(Application->ExeName) == L"C:\\Program Files") {
		portable_possible = 0;
	}
	if (ExtractFilePath(Application->ExeName) == L"C:\\Program Files (x86)") {
		portable_possible = 0;
	}
//	portable_possible = 0;

	if (portable_possible == 1) {
		defaultParameterPath   = ExtractFilePath(Application->ExeName);
		defaultParameterEQPath = ExtractFilePath(Application->ExeName);
		uFileName = defaultParameterPath + "\\upconvfe.ini";
		uExtFileName = defaultParameterPath + "\\upconvfe_ext.txt";
		iniFile = new TIniFile(uFileName);
		portableMode = iniFile->ReadInteger(L"Env", L"PortableMode",0);
	}
	if (portableMode == 0) {
		SHGetSpecialFolderPath(Form1->Handle, file_path,CSIDL_COMMON_APPDATA, false);
		defaultParameterPath = file_path;
		defaultParameterPath += L"\\upconv";
		defaultParameterEQPath = file_path;
		defaultParameterEQPath += L"\\upconv_eq";
		uFileName = defaultParameterPath + "\\upconvfe.ini";
		uExtFileName = defaultParameterPath + "\\upconvfe_ext.txt";
		iniFile = new TIniFile(uFileName);
	}

	sampRate = iniFile->ReadInteger(L"Convert", L"SamplingRate", 44100);
	bitWidth = iniFile->ReadInteger(L"Convert", L"BitWidth", 16);
	hfa = iniFile->ReadInteger(L"Convert", L"hfa", 0);
	hfc = iniFile->ReadInteger(L"Convert", L"hfc", -1);
	co_enable = iniFile->ReadInteger(L"Convert", L"co_enable", 0);
	co_count = iniFile->ReadInteger(L"Convert", L"co_count", 1);
	ct_sec   = iniFile->ReadInteger(L"Convert", L"ct_sec", 60);
	def_param = iniFile->ReadInteger(L"Convert", L"DefaultParam", 0);
	norm = iniFile->ReadInteger(L"Output", L"Normalize", 0);
	norm_opt = iniFile->ReadInteger(L"Output", L"Normalize_option", 1);
	volume_level = iniFile->ReadInteger(L"Output", L"Volume_Level",25);
	volume_per   = iniFile->ReadInteger(L"Output", L"Volume_Per",100);
	bwf = iniFile->ReadInteger(L"Output", "BWF", 0);
	rf64 = iniFile->ReadInteger(L"Output", "RF64", 0);
	w64 = iniFile->ReadInteger(L"Output", "W64", 0);
	wav2g = iniFile->ReadInteger(L"Output", "WAV2G", 0);
	wav_auto = iniFile->ReadInteger(L"Output", "WAV_AUTO", 0);
	lock_suffix = iniFile->ReadInteger(L"Output", "LockSuffix", 0);
	split_index = iniFile->ReadInteger(L"Output", "SplitIndex", 0);
	save_dp = iniFile->ReadInteger(L"Output", "SaveDP", 1);
	thread = iniFile->ReadInteger("Exec", "Thread", 1);
	fileio = iniFile->ReadInteger("Exec", "file_io", 5);
	fileio_auto = iniFile->ReadInteger("Exec", "file_io_auto",0);
	fileio_max  = iniFile->ReadInteger("Exec", "file_io_max",100);
	fio_sec_mb  = iniFile->ReadInteger("Exec", "file_io_sec_mb",1);
	fio_sec_size = iniFile->ReadInteger("Exec", "file_io_sec_size",10);
	fio_mb_size  = iniFile->ReadInteger("Exec", "file_io_mb_size",5);

	cpu_pri = iniFile->ReadInteger("Exec", "cpu_pri", 0);
	addp   = iniFile->ReadInteger("Exec", "addp", 0);
	upconv_max = iniFile->ReadInteger("Exec", "upconv_max", 1);
	encMode = iniFile->ReadInteger("Encoder", "Mode", 0);
	winLeft = iniFile->ReadInteger("Exec", "WinLeft", 100);
	winTop = iniFile->ReadInteger("Exec", "WinTop", 100);

	tmpDrive = iniFile->ReadString("Exec", "tmpDrive", "");

	switch (sampRate) {
	case 32000:
		cmbSampRate->ItemIndex = 0;
		break;
	case 44100:
		cmbSampRate->ItemIndex = 1;
		break;
	case 48000:
		cmbSampRate->ItemIndex = 2;
		break;
	case 88200:
		cmbSampRate->ItemIndex = 3;
		break;
	case 96000:
		cmbSampRate->ItemIndex = 4;
		break;
	case 176400:
		cmbSampRate->ItemIndex = 5;
		break;
	case 192000:
		cmbSampRate->ItemIndex = 6;
		break;
	case 352800:
		cmbSampRate->ItemIndex = 7;
		break;
	case 384000:
		cmbSampRate->ItemIndex = 8;
		break;
	case 705600:
		cmbSampRate->ItemIndex = 9;
		break;
	case 768000:
		cmbSampRate->ItemIndex = 10;
		break;
	case 1536000:
		cmbSampRate->ItemIndex = 11;
		break;
		// case 2822400:
		// cmbSampRate->ItemIndex = 11;
		// break;
		// case 2822400*2:
		// cmbSampRate->ItemIndex = 12;
		// break;
		// case 2822400*4:
		// cmbSampRate->ItemIndex = 13;
		// break;
	}
	if (bitWidth == 16) {
		cmbBitWidth->ItemIndex = 0;
	}
	else if (bitWidth == 24) {
		cmbBitWidth->ItemIndex = 1;
	}
	else if (bitWidth == 32) {
		cmbBitWidth->ItemIndex = 2;
	}
	else if (bitWidth == 64) {
		cmbBitWidth->ItemIndex = 3;
	} else if (bitWidth == 321) {
		cmbBitWidth->ItemIndex = 4;
	}
	if (hfa >= 0 && hfa <= 4) {
		cmbHFA->ItemIndex = hfa;
	}
	if (norm == 0) {
		cmbNormalize->ItemIndex = 0;
		edt_volume_level->Enabled = false;
	} else if (norm == 1 && norm_opt == 1) {
		cmbNormalize->ItemIndex = 1;
		edt_volume_level->Enabled = false;
		//	} else if (norm == 1 && norm_opt == 2) {
//		cmbNormalize->ItemIndex = 2;
	} else if (norm == 2 && volume_level >= 10 && volume_level <= 100) {
		cmbNormalize->ItemIndex = 2;
		if (volume_level > 0) {
			edt_volume_level->Text = volume_level;
		} else {
			edt_volume_level->Text = L"";
		}
		cmb_volume_level->Enabled = true;
	} else if (norm == 3 && volume_per >= 50 && volume_per <= 120) {
		cmbNormalize->ItemIndex = 3;
		if (volume_per > 0) {
			edt_volume_level->Text = volume_per;
		} else {
			edt_volume_level->Text = L"";
		}
		cmb_volume_level->Enabled = true;
	}
	if (thread >= 1 && thread <= 24) {
		cmbThread->ItemIndex = thread -1;
	}
	cmbOutputFormat->ItemIndex = 0;
	if (wav2g == 1) {
		cmbOutputFormat->ItemIndex = 1;
	}
	if (bwf == 1) {
		cmbOutputFormat->ItemIndex = 2;
	}
	if (rf64 == 1) {
		cmbOutputFormat->ItemIndex = 3;
	}
	if (w64 == 1) {
		cmbOutputFormat->ItemIndex = 4;
	}
	if (wav_auto == 1) {
		cmbOutputFormat->ItemIndex = 5;
	}

	if (split_index > 0 && split_index <= 4) {
		cmbSplit->ItemIndex = split_index;
	}
	if (save_dp) {
		chk_save_dp->Checked = true;
	}

	if (addp >= 0 && addp <= 12) {
		cmbADDP->ItemIndex = addp;
	}
	if (split_index != 0) {
		cmbADDP->Enabled = false;
	}
	if (addp > 0) {
		cmbSplit->Enabled = false;
	}
	//
	// if (hfc == -1) {
	// edtHFC->Text = L"";
	// } else if (hfc >= 4000) {
	// edtHFC->Text.sprintf(L"%d",hfc);
	// }
	edtOutputPath->Text = iniFile->ReadString("Output", "Folder", "");
	lbl_select_directory->Caption = iniFile->ReadString("Output", "FolderHistory", "");
	edtSuffix->Text = iniFile->ReadString("Output", "Suffix", "");
	if (edtOutputPath->Text.Length() > 0)
		edtOutputPath->RightButton->Visible = true;
	if (edtSuffix->Text.Length() > 0)
		edtSuffix->RightButton->Visible = true;
	if (edtOutputPath->Text.Length() == 0)
		edtOutputPath->Text = L"Source Directory";
	if (edtSuffix->Text.Length() == 0)
		edtSuffix->Text = L"upconv";
	if (lock_suffix)
		chkLockSuffix->Checked = true;

	if (encMode >= 0) {
		cmbEncoder->ItemIndex = encMode;
	}

	if (fileio >= 0 && fileio < 24) {
		cmbFileIO->ItemIndex = fileio;
	}
	if (fileio_auto == 1) {
		chkFIO_Ex->Checked = true;
	}
	if (cpu_pri >= 0 && cpu_pri <= 2) {
		cmbPriority->ItemIndex = cpu_pri;
	}

	btnReload->Click();
	if (def_param < cmbDefaultParameter->Items->Count) {
		cmbDefaultParameter->ItemIndex = def_param;
		ReloadDefaultParameter(def_param + 1, def_param + 1);
	}
	if (upconv_max >= 0 && upconv_max < 5) {
		cmbUpconvFile->ItemIndex = upconv_max;
	}
	if (co_enable == 1) {
		chk_change_default_parameter->Checked = true;
		lbl_co->Caption = L"CDP Mode";
	}
	if (co_count >= 1) {
		UpDDefParamCount->Position = co_count;
	}
	if (ct_sec == 30) {
		cmb_ct_min->ItemIndex = 0;
	} else if (ct_sec == 60) {
		cmb_ct_min->ItemIndex = 1;
	} else if (ct_sec == 90) {
		cmb_ct_min->ItemIndex = 2;
	} else if (ct_sec == 120) {
		cmb_ct_min->ItemIndex = 3;
	}
	cdp_num_max = 0;

	for (i = 0; i < 8; i++) {
		threadExec[i] = NULL;
	}
	if (portable_possible == 1) {
		grp_portable->Enabled = true;
	}
	if (portableMode == 0) {
		rdo_upconv_mode_1->Checked = true;
		rdo_upconv_mode_1->Tag = 1;
		rdo_upconv_mode_2->Tag = 0;
	} else {
		rdo_upconv_mode_2->Checked = true;
		rdo_upconv_mode_1->Tag = 0;
		rdo_upconv_mode_2->Tag = 1;
	}

	switch (fileio_max) {
		case 100:
			cmbFileIOMax->ItemIndex = 0;
			break;
		case 200:
			cmbFileIOMax->ItemIndex = 1;
			break;
		case 250:
			cmbFileIOMax->ItemIndex = 2;
			break;
		case 500:
			cmbFileIOMax->ItemIndex = 3;
			break;
		case 1000:
			cmbFileIOMax->ItemIndex = 4;
			break;
		case 1200:
			cmbFileIOMax->ItemIndex = 5;
			break;
		case 1400:
			cmbFileIOMax->ItemIndex = 6;
			break;
		case 1600:
			cmbFileIOMax->ItemIndex = 7;
			break;
		case 1800:
			cmbFileIOMax->ItemIndex = 8;
			break;
		case 2000:
			cmbFileIOMax->ItemIndex = 9;
			break;
		case 3000:
			cmbFileIOMax->ItemIndex = 10;
			break;
		case 4000:
			cmbFileIOMax->ItemIndex = 11;
			break;
		case 6000:
			cmbFileIOMax->ItemIndex = 12;
			break;
		case 8000:
			cmbFileIOMax->ItemIndex = 13;
			break;
		case 10000:
			cmbFileIOMax->ItemIndex = 14;
			break;
	}

	if (fio_sec_mb == 1) {
		rdo_file_io_sec->Checked = true;
	} else {
		rdo_file_io_mb->Checked  = true;
	}
	if (fio_sec_size == 10) {
		cmbFIOSecSize->ItemIndex = 0;
	} else if (fio_sec_size == 30) {
		cmbFIOSecSize->ItemIndex = 1;
	} else if (fio_sec_size == 60) {
		cmbFIOSecSize->ItemIndex = 2;
	} else if (fio_sec_size == 120) {
		cmbFIOSecSize->ItemIndex = 3;
	} else if (fio_sec_size == 300) {
		cmbFIOSecSize->ItemIndex = 4;
	}
	if (fio_mb_size == 10) {
		cmbFIOMBSize->ItemIndex = 0;
	} else if (fio_mb_size == 50) {
		cmbFIOMBSize->ItemIndex = 1;
	} else if (fio_mb_size == 100) {
		cmbFIOMBSize->ItemIndex = 2;
	} else if (fio_mb_size == 500) {
		cmbFIOMBSize->ItemIndex = 3;
	} else if (fio_mb_size == 1000) {
		cmbFIOMBSize->ItemIndex = 4;
	}

	if (ffmpeg_found == 1 && PathFileExists(uExtFileName.c_str())) {
		TStringList *tsl = NULL;
		tsl = new TStringList;
		tsl->LoadFromFile(uExtFileName);
		for (i = 0; i < tsl->Count;i++) {
			UnicodeString ext = tsl->Strings[i];
			ext = ext.LowerCase();
			ext = ext.Trim();
			if (ext.Length() > 0 && ext.Length() <= 10) {
				for (ii = 0;ii < clist_extension->Items->Count;ii++) {
					if (clist_extension->Items->Strings[ii] == ext) break;
				}
				if (ii == clist_extension->Items->Count) {
					clist_extension->Items->Add(ext);
				}
			}
		}
		delete tsl;
		UpdateSupportExt();
	}

	delete iniFile;
}
void __fastcall TForm1::UpdateSupportExt(void)
{
	int i,ii;
	TStringList *ext_list = NULL;
	ext_list = new TStringList;
	UnicodeString cmdName;

	ext_list->Add(L"wav");
	ext_list->Add(L"dsf");
	OpenDialog->Filter = L"WAV|*.WAV";
	OpenDialog->Filter = OpenDialog->Filter + L"|DSF|*.DSF";

	// ffmpeg
	cmdName = ExtractFilePath(Application->ExeName);
//	cmdName += L"flac.exe";
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|FLAC|*.FLAC";
		ext_list->Add(L"flac");
	}

	cmdName = ExtractFilePath(Application->ExeName);
//	cmdName += L"lame.exe";
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|MP3|*.MP3";
		ext_list->Add(L"mp3");
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|MP4|*.MP4";
		ext_list->Add(L"mp4");
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|WMA|*.WMA";
		ext_list->Add(L"wma");
	}

	cmdName = ExtractFilePath(Application->ExeName);
//	cmdName += L"opusdec.exe";
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|OPUS|*.OPUS";
		ext_list->Add(L"opus");
	}

	cmdName = ExtractFilePath(Application->ExeName);
//	cmdName += L"wvunpack.exe";
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		OpenDialog->Filter = OpenDialog->Filter + L"|WV|*.WV";
		ext_list->Add(L"wv");
		// cmbEncoder->Items->Add(L"WavPack");
	}


	for (i = 0;i < clist_extension->Items->Count;i++) {
		for (ii = 0;ii < ext_list->Count;ii++) {
			if (clist_extension->Items->Strings[i] == ext_list->Strings[ii]) break;
		}
		if (ii == ext_list->Count) {
			OpenDialog->Filter = OpenDialog->Filter + L"|" + clist_extension->Items->Strings[i].UpperCase() + "|*." + clist_extension->Items->Strings[i].UpperCase();
		}
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose) {
	TIniFile *iniFile;
	AnsiString sStr;
	wchar_t file_path[2048];
	long temp;
	int norm, norm_opt;
	int bit[] = {16, 24, 32, 64, 321};
	int i,n;

	UnicodeString uFileName;
	UnicodeString uExtFileName;

	if (Abort || Timer1->Enabled == true) {
		CanClose = false;
		return;
	}
	if (portableMode == 0 && rdo_upconv_mode_2->Checked == true) {
		defaultParameterPath   = ExtractFilePath(Application->ExeName);
		defaultParameterEQPath = ExtractFilePath(Application->ExeName);
		uFileName = defaultParameterPath + "\\upconvfe.ini";
		uExtFileName = defaultParameterPath + "\\upconvfe_ext.txt";
	} else if (portableMode == 1 && rdo_upconv_mode_1->Checked == true) {
		uFileName = defaultParameterPath + "\\upconvfe.ini";
		uExtFileName = defaultParameterPath + "\\upconvfe_ext.txt";
		iniFile = new TIniFile(uFileName);
		iniFile->WriteInteger("Env", "PortableMode",0);
		delete iniFile;

		SHGetSpecialFolderPath(Form1->Handle, file_path,CSIDL_COMMON_APPDATA, false);
		defaultParameterPath = file_path;
		defaultParameterPath += L"\\upconv";
		defaultParameterEQPath = file_path;
		defaultParameterEQPath += L"\\upconv_eq";
		uFileName = defaultParameterPath + "\\upconvfe.ini";
	} else {
		uFileName = defaultParameterPath + "\\upconvfe.ini";
		uExtFileName = defaultParameterPath + "\\upconvfe_ext.txt";
	}

	iniFile = new TIniFile(uFileName);
	if (cmbSampRate->ItemIndex < 12) {
		iniFile->WriteInteger("Convert", "SamplingRate",
			cmbSampRate->Text.ToInt());
		// } else if (cmbSampRate->ItemIndex == 11) {
		// iniFile->WriteInteger("Convert","SamplingRate",2822400);
		// } else if (cmbSampRate->ItemIndex == 12) {
		// iniFile->WriteInteger("Convert","SamplingRate",2822400*2);
		// } else if (cmbSampRate->ItemIndex == 13) {
		// iniFile->WriteInteger("Convert","SamplingRate",2822400*4);
	}
	iniFile->WriteInteger(L"Convert", "BitWidth", bit[cmbBitWidth->ItemIndex]);
	iniFile->WriteInteger(L"Convert", "hfa", cmbHFA->ItemIndex);
	iniFile->WriteInteger(L"Convert", "co_enable",
		chk_change_default_parameter->Checked == true ? 1 : 0);
	iniFile->WriteInteger(L"Convert", "co_count", UpDDefParamCount->Position);
	if (cmb_ct_min->ItemIndex == 0) {
		iniFile->WriteInteger(L"Convert", "ct_sec",30);
	} else if (cmb_ct_min->ItemIndex == 1) {
		iniFile->WriteInteger(L"Convert", "ct_sec",60);
	} else if (cmb_ct_min->ItemIndex == 2) {
		iniFile->WriteInteger(L"Convert", "ct_sec",90);
	}else if (cmb_ct_min->ItemIndex == 3) {
		iniFile->WriteInteger(L"Convert", "ct_sec",120);
	}

	// if (edtHFC->Text == L"") {
	// iniFile->WriteInteger(L"Convert","hfc",-1);
	// } else {
	// iniFile->WriteInteger(L"Convert","hfc",edtHFC->Text.ToInt());
	// }
	sStr = cmbDefaultParameter->Items->Strings[cmbDefaultParameter->ItemIndex];
	if (sscanf(sStr.c_str(),"%d%*s",&n) == 1) {
		iniFile->WriteInteger(L"Convert", "DefaultParam",n - 1);
	}
	iniFile->WriteInteger(L"Exec", "file_io", cmbFileIO->ItemIndex);
	iniFile->WriteInteger(L"Exec", "file_io_auto",chkFIO_Ex->Checked == true ? 1 : 0);
	iniFile->WriteString(L"Exec", "file_io_max",cmbFileIOMax->Text);

	iniFile->WriteInteger(L"Exec", "file_io_sec_mb",rdo_file_io_sec->Checked ? 1 : 2);
	if (cmbFIOSecSize->ItemIndex == 0) {
		iniFile->WriteInteger(L"Exec", "file_io_sec_size",10);
	} else if (cmbFIOSecSize->ItemIndex == 1) {
		iniFile->WriteInteger(L"Exec", "file_io_sec_size",30);
	} else if (cmbFIOSecSize->ItemIndex == 2) {
		iniFile->WriteInteger(L"Exec", "file_io_sec_size",60);
	} else if (cmbFIOSecSize->ItemIndex == 3) {
		iniFile->WriteInteger(L"Exec", "file_io_sec_size",120);
	} else if (cmbFIOSecSize->ItemIndex == 4) {
		iniFile->WriteInteger(L"Exec", "file_io_sec_size",300);
	}
	if (cmbFIOMBSize->ItemIndex == 0) {
		iniFile->WriteInteger(L"Exec", "file_io_mb_size",10);
	} else if (cmbFIOMBSize->ItemIndex == 1) {
		iniFile->WriteInteger(L"Exec", "file_io_mb_size",50);
	} else if (cmbFIOMBSize->ItemIndex == 2) {
		iniFile->WriteInteger(L"Exec", "file_io_mb_size",100);
	} else if (cmbFIOMBSize->ItemIndex == 3) {
		iniFile->WriteInteger(L"Exec", "file_io_mb_size",500);
	} else if (cmbFIOMBSize->ItemIndex == 4) {
		iniFile->WriteInteger(L"Exec", "file_io_mb_size",1000);
	}

	iniFile->WriteInteger(L"Exec", "upconv_max", cmbUpconvFile->ItemIndex);
	iniFile->WriteInteger(L"Exec", "cpu_pri", cmbPriority->ItemIndex);
	iniFile->WriteInteger(L"Exec", "addp", cmbADDP->ItemIndex);

	norm = 0;
	norm_opt = 1;
	if (cmbNormalize->ItemIndex == 0) {
		norm = 0;
		norm_opt = 1;
	}
	if (cmbNormalize->ItemIndex == 1) {
		norm = 1;
		norm_opt = 1;
	}
	if (cmbNormalize->ItemIndex == 2) {
		norm = 2;
		norm_opt = 1;
	}
	if (cmbNormalize->ItemIndex == 3) {
		norm = 3;
		norm_opt = 1;
	}

	iniFile->WriteInteger("Output", "Normalize", norm);
	iniFile->WriteInteger("Output", "Normalize_option", norm_opt);
	if (edt_volume_level->Text != "") {
		if (norm == 2) {
			iniFile->WriteInteger("Output", "Volume_Level",edt_volume_level->Text.ToInt());
		} else if (norm == 3) {
			iniFile->WriteInteger("Output", "Volume_Per",edt_volume_level->Text.ToInt());
		}
	} else {
		if (norm == 2) {
			iniFile->WriteInteger("Output", "Volume_Level",0);
		} else if (norm == 3) {
			iniFile->WriteInteger("Output", "Volume_Per",0);
		}
	}
	iniFile->WriteInteger("Output", "SplitIndex", cmbSplit->ItemIndex);
	iniFile->WriteInteger("Exec", "Thread", cmbThread->ItemIndex + 1);

	if (edtOutputPath->Text == L"Source Directory")
		edtOutputPath->Text = L"";
	iniFile->WriteString("Output", "Folder", edtOutputPath->Text);
	iniFile->WriteString("Output", "FolderHistory",lbl_select_directory->Caption);
	iniFile->WriteString("Output", "Suffix", edtSuffix->Text);
	iniFile->WriteInteger("Output", "LockSuffix",chkLockSuffix->Checked == true ? 1 : 0);
	iniFile->WriteInteger("Output", "WAV2G",cmbOutputFormat->ItemIndex == 1 ? 1 : 0);
	iniFile->WriteInteger("Output", "BWF",cmbOutputFormat->ItemIndex == 2 ? 1 : 0);
	iniFile->WriteInteger("Output", "RF64",cmbOutputFormat->ItemIndex == 3 ? 1 : 0);
	iniFile->WriteInteger("Output", "W64",cmbOutputFormat->ItemIndex == 4 ? 1 : 0);
	iniFile->WriteInteger("Output", "WAV_AUTO",cmbOutputFormat->ItemIndex == 5 ? 1 : 0);
	iniFile->WriteInteger("Output","SaveDP",chk_save_dp->Checked == true ? 1 : 0);

	iniFile->WriteString("Encoder", "Mode", cmbEncoder->ItemIndex);

	iniFile->WriteInteger("Env", "PortableMode",rdo_upconv_mode_2->Checked == true ? 1 : 0);

	if (1) {
		HWND hDesktop = GetDesktopWindow();

		WINDOWINFO windowInfo;
		memset(&windowInfo, 0, sizeof(WINDOWINFO));
		windowInfo.cbSize = sizeof(WINDOWINFO);

		GetWindowInfo(hDesktop, &windowInfo);
		iniFile->WriteInteger("Exec", "WinLeft", windowInfo.rcWindow.left);
		iniFile->WriteInteger("Exec", "WinTop", windowInfo.rcWindow.top);
	}

	// iniFile->WriteString("Encode","FLACOpt",edtFLACOption->Text);
	// iniFile->WriteString("Encode","WavPackOpt",edtWavPackOption->Text);

	delete iniFile;

	TStringList *tsl = NULL;
	tsl = new TStringList;
	for (i = 0;i < clist_extension->Items->Count;i++) {
		tsl->Add(clist_extension->Items->Strings[i]);
	}
	tsl->SaveToFile(uExtFileName);
	delete tsl;

	// toF = appData + "\\preset.dat";
	// edtHfaParam->Lines->SaveToFile(toF);

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ReloadDefaultParameter(int index, int use_index) {
	FILE *fp;
	UnicodeString uFileName;
	UnicodeString uStr, uTitle;
	AnsiString sStr;
	AnsiString sFileName;
	char str[4096];
	char *p;
	int error;
	int overwrite_suffix = 0;
	int overwrite_sampling = 0;
	int overwrite_bit = 0;
	int overwrite_hfa = 0;
	int overwrite_norm = 0;
	int d1;

	uFileName.printf(L"%s\\upconv_default_%02d.txt",defaultParameterPath, index);
	sFileName = (AnsiString)uFileName;
	// memoLog->Lines->Add(sFileName);

	error = 0;
	fp = fopen(sFileName.c_str(), "r");
	if (fp) {
		str[0] = '\0';
		if (index == use_index) {
			memoEncoderInfo->Lines->Clear();
			memoEncoderInfo->Lines->Add("");
			memoEncoderExt->Lines->Clear();
			memoEncoderExt->Lines->Add("");
			memoEncoderExt->Lines->Add("");
			memoEncoderExt->Lines->Add("");
			memoEncoderExt->Lines->Add("");
			memoEncoderExt->Lines->Add("");
		}
		// デフォルトパラメーター
		if (fgets(str, 4000, fp) != NULL) {
			if (index == use_index) {
				p = strrchr(str, (int)'\n');
				if (p != NULL)
					*p = '\0';
				uStr.sprintf(L"%s", (UnicodeString)str);
				edtDefaultParam->Text = uStr;
				sStr = (AnsiString)str;
				p = strstr(str, "-enable_suffix:1");
				if (p != NULL) {
					p = strstr(p, "-suffix:");
					if (p != NULL && chkLockSuffix->Checked == false) {
						if (edtSaveSuffix->Text == L"")
							edtSaveSuffix->Text = edtSuffix->Text;
						edtSuffix->Text = (UnicodeString)(p + 8);
					}
				}
				p = strstr(str, "-enable_suffix:0");
				if (p != NULL) {
					if (edtSaveSuffix->Text != L"") {
						edtSuffix->Text = edtSaveSuffix->Text;
						edtSaveSuffix->Text = L"";
					}
				}
				p = strstr(str, "-ovwrite_s:");
				if (p != NULL) {
					if (sscanf(p, "-ovwrite_s:%d", &d1) == 1 && d1 >= 0 && d1 <= 10) {
						overwrite_sampling = d1;
					}
				}
				p = strstr(str, "-ovwrite_w:");
				if (p != NULL) {
					if (sscanf(p, "-ovwrite_w:%d", &d1) == 1 && d1 >= 0 && d1 <= 3) {
						overwrite_bit = d1;
					}
				}
				p = strstr(str, "-enable_ovwrite_sw:");
				if (p != NULL) {
					if (sscanf(p, "-enable_ovwrite_sw:%d", &d1) == 1 && d1 >= 0 && d1 <= 1) {
						if (d1 == 1) {
//							cmbSampRate->ItemIndex = overwrite_sampling;
//							cmbBitWidth->ItemIndex = overwrite_bit;
						}
					}
				}
				p = strstr(str, "-ovwrite_hfa:");
				if (p != NULL) {
					if (sscanf(p, "-ovwrite_hfa:%d", &d1) == 1 && d1 >=
						0 && d1 <= 3) {
						overwrite_hfa = d1;
					}
				}
				p = strstr(str, "-enable_ovwrite_hfa:");
				if (p != NULL) {
					if (sscanf(p, "-enable_ovwrite_hfa:%d", &d1) == 1 && d1 >= 0 && d1 <= 1) {
						if (d1 == 1) {
//							cmbHFA->ItemIndex = overwrite_hfa;
						}
					}
				}
				p = strstr(str, "-ovwrite_norm:");
				if (p != NULL) {
					if (sscanf(p, "-ovwrite_norm:%d", &d1) == 1 && d1 >= 0 && d1 <= 2) {
//						overwrite_norm = d1;
					}
				}
				p = strstr(str, "-enable_ovwrite_norm:");
				if (p != NULL) {
					if (sscanf(p, "-enable_ovwrite_norm:%d", &d1) == 1 && d1 >= 0 && d1 <= 1) {
						if (d1 == 1) {
//							cmbNormalize->ItemIndex = overwrite_norm;
						}
					}
				}
				p = strstr(str, "-abe ");
				if (p != NULL) {
					chk_ABE_Overrite->Checked = true;
				} else {
					chk_ABE_Overrite->Checked = false;
				}
				p = strstr(str, "-post_abe ");
				if (p != NULL) {
					chk_PostABE_Overrite->Checked = true;
				} else {
					chk_PostABE_Overrite->Checked = false;
				}
				p = strstr(str, "-nr:");
				if (p == NULL) {
					chk_NR_Overrite->Enabled = false;
					chk_NR_Overrite->Checked = false;
				} else {
					chk_NR_Overrite->Enabled = true;
				}
				p = strstr(str, "-enable_nr:1");
				if (p != NULL) {
					chk_NR_Overrite->Checked = true;
				} else {
					chk_NR_Overrite->Checked = false;
				}

				p = strstr(str, "-ms_process_save:");
				if (p != NULL) {
					chk_MS_Overrite->Enabled = false;
					chk_MS_Overrite->Checked = false;
				} else {
					chk_MS_Overrite->Enabled = true;
					chk_MS_Overrite->Checked = true;
				}
				p = strstr(str, "-LRC_process:1");
				if (p != NULL) {
					chk_LRC_Overrite->Enabled = true;
					chk_LRC_Overrite->Checked = true;
				} else {
					chk_LRC_Overrite->Enabled = false;
					chk_LRC_Overrite->Checked = false;
				}
			}
		}
		else {
			error = 1;
		}
		// 作業ディレクトリ
		if (error == 0 && fgets(str, 4000, fp) != NULL) {

		}
		else {
			error = 1;
		}
		// Encorder Option1
		if (error == 0 && fgets(str, 4000, fp) != NULL) {
			if (index == use_index) {
				memoEncoderInfo->Lines->Add((UnicodeString)str);
			}
		}
		else {
			error = 1;
		}
		// Encorder Option2
		if (error == 0 && fgets(str, 4000, fp) != NULL) {
			if (index == use_index) {
				memoEncoderInfo->Lines->Add((UnicodeString)str);
			}
		}
		else {
			error = 1;
		}
		// Encorder Option3
		if (error == 0 && fgets(str, 4000, fp) != NULL) {
			if (index == use_index) {
				memoEncoderInfo->Lines->Add((UnicodeString)str);
			}
		}
		else {
			error = 1;
		}
		if (error == 0 && fgets(str, 4000, fp) != NULL) {
			if (use_index == -1) {
				uTitle.sprintf(L"%02d. %s", index, (UnicodeString)str);
				list_dp_active->Items->Add(uTitle);
			}
		}
		// Encorder Option4
		if (error == 0 && fgets(str, 4000, fp) != NULL) {
			if (index == use_index) {
				memoEncoderInfo->Lines->Add((UnicodeString)str);
			}
		}
		else {
			error = 1;
		}
		// Encorder Option5
		if (error == 0 && fgets(str, 4000, fp) != NULL) {
			if (index == use_index) {
				memoEncoderInfo->Lines->Add((UnicodeString)str);
			}
		}
		else {
			error = 1;
		}
		fclose(fp);
		ReloadEncoderInfo();
	}
	else {
		error = 1;
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::cmbDefaultParameterChange(TObject *Sender) {
	int n;
	AnsiString sStr;
	sStr = cmbDefaultParameter->Items->Strings[cmbDefaultParameter->ItemIndex];
	if (sscanf(sStr.c_str(),"%d%*s",&n) == 1) {
		ReloadDefaultParameter(n,n);
		cmbDefaultParameter->TextHint = cmbDefaultParameter->Text;
	}
	UpdateFileSize();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::AddToFileList(UnicodeString filename) {
	SOUNDFMT inFmt;
	SSIZE nSample;
	TListItem *item;
	AnsiString s;
	AnsiString sStr;
	UnicodeString uStr;
	int i;
	int ret;
	int wide_str_len;
	wchar_t *p_wchar;
	long time;
	long long int sampling;
	char fname[MAX_PATH];
	UnicodeString fn;

	uStr = GetFilePath(1, filename.c_str());
	sStr = uStr;
	ret = PLG_InfoAudioData(sStr.c_str(), &inFmt, &nSample, NULL);
	if (ret == STATUS_SUCCESS) {
		fn = ExtractFileName(filename);
		item = LvFile->Items->Add();

		// edtFileInfo に音声ファイルの情報を保存する
		item->Caption = "";
		if ((AnsiString)inFmt.fmt == "wav" || (AnsiString)inFmt.fmt == "rf64" || (AnsiString)inFmt.fmt == "wavp" || (AnsiString)inFmt.fmt == "dsf" || (AnsiString)inFmt.fmt == "mp3" || (AnsiString)inFmt.fmt == "mp4" || (AnsiString)inFmt.fmt == "flac") {
			item->SubItems->Add(fn);
			if (strlen(inFmt.time) > 0) {
				item->SubItems->Add(inFmt.duration_time);
			} else {
				item->SubItems->Add("-");
			}
			if (strlen(inFmt.extra) > 0) {
				item->SubItems->Add(inFmt.extra);
			} else {
				item->SubItems->Add("-");
			}
		} else {
			item->SubItems->Add(fn);
			s.printf("-");
			item->SubItems->Add(s);
			s.printf("-");
			item->SubItems->Add(s);
		}
		item->SubItems->Add("");
		item->SubItems->Add("");
		item->SubItems->Add("");
		wide_str_len = filename.Length();
		p_wchar = new wchar_t[wide_str_len + 2];
		wcscpy(p_wchar, filename.c_str());
		item->Data = p_wchar;
		if (LvFile->Items->Count > 0)
			btnStart->Enabled = true;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ListViewWinProc(TMessage& Msg) {
	TListItem *item;
	HANDLE hDrop;
	int i, nFiles;
	wchar_t szFile[MAX_PATH];

	if (Msg.Msg == WM_DROPFILES) {
		Msg.Result = 0;

		hDrop = (HANDLE)Msg.WParam;
		nFiles = DragQueryFile((HDROP)hDrop, -1, NULL, 0);
		for (i = 0; i < nFiles; i++) {
			if (DragQueryFile((HDROP)hDrop, i, szFile, sizeof(szFile)) > 0) {
				AddToFileList((UnicodeString)szFile);
			}
		}
		DragFinish((HDROP)hDrop);
		btnClear->Enabled = true;
	}
	else {
		orgWinProc(Msg);
	}
}
// ---------------------------------------------------------------------------
// type1 : 読めれば ShortPathName でも良い
// type2 : 書き込む必要がある名前(〜) は変換

UnicodeString __fastcall TForm1::GetFilePath(int type, wchar_t *data) {
	FILE *ifp;
	UnicodeString uStr;
	AnsiString sStr;
	wchar_t *shortPath;

	uStr = (UnicodeString)data;
	sStr = uStr;
	ifp = fopen(sStr.c_str(), "rb");
	if (ifp != NULL) {
		fclose(ifp);
		return uStr;
	}

	if (type == 1) {
		// AnsiString にした後でファイルが開けないとき
		shortPath = (wchar_t *)malloc(uStr.Length() * 4 + 1);
		shortPath[0] = 0;
		GetShortPathName(data, shortPath, uStr.Length() * 4 + 1);
		uStr = UnicodeString(shortPath);
		sStr = uStr;
		free(shortPath);
		ifp = fopen(sStr.c_str(), "rb");
		if (ifp != NULL) {
			fclose(ifp);
			return uStr;
		}
		return "";
	}
	else {
		// 置換する
		uStr = StringReplace(uStr, L"〜", L"～",TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
		uStr = StringReplace(uStr, L"【", L"[",TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
		uStr = StringReplace(uStr, L"】", L"]",TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
		return uStr;
	}
	return sStr;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnAddClick(TObject *Sender) {
	TListItem *item;
	AnsiString s;
	int i;
	int ret;
	long time;
	long size;

	view_information->Text = L"";
    edt_OutputSize->Text   = L"";

	if (OpenDialog->Execute()) {
		for (i = 0; i < OpenDialog->Files->Count; i++) {
			AddToFileList(OpenDialog->Files->Strings[i]);
		}
		if (LvFile->Items->Count > 0) {
			btnClear->Enabled = true;
			btnStart->Enabled = true;
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnDelClick(TObject *Sender) {
	TListItem *item;
	int i;

	view_information->Text = L"";
    edt_OutputSize->Text   = L"";

	if (LvFile->SelCount > 0) {
		for (i = LvFile->Items->Count - 1;i >= 0;i--) {
			if (LvFile->Items->Item[i]->Selected == true) {
				item = LvFile->Items->Item[i];
				if (item->Data) {
					delete[]item->Data;
				}
				item->Delete();
			}
		}
		if (LvFile->Items->Count == 0) {
			btnStart->Enabled = false;
			btnClear->Enabled = false;
			btnUp->Enabled = false;
			btnDown->Enabled = false;
			btnInfo->Enabled = false;
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnUpClick(TObject *Sender) {
	UnicodeString s;
	TListItem *item, *new_item;
	if (LvFile->Selected != NULL && LvFile->Selected->Index > 0) {
		item = LvFile->Selected;
		LvFile->Items->BeginUpdate();
		new_item = LvFile->Items->Insert(item->Index - 1);
		new_item->Assign(item);
		new_item->SubItems->Add(item->SubItems->Strings[1]);
		new_item->SubItems->Add(item->SubItems->Strings[2]);
		item->Delete();
		new_item->Selected = TRUE;
		new_item->MakeVisible(TRUE);
		LvFile->Items->EndUpdate();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnDownClick(TObject *Sender) {
	UnicodeString s;
	TListItem *item, *new_item;
	if (LvFile->Selected != NULL && LvFile->Selected->Index + 1 <
		LvFile->Items->Count) {
		item = LvFile->Selected;
		LvFile->Items->BeginUpdate();
		new_item = LvFile->Items->Insert(item->Index + 2);
		new_item->Assign(item);
		new_item->SubItems->Add(item->SubItems->Strings[1]);
		new_item->SubItems->Add(item->SubItems->Strings[2]);
		item->Delete();
		new_item->Selected = TRUE;
		new_item->MakeVisible(TRUE);
		LvFile->Items->EndUpdate();
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::LvFileSelectItem(TObject *Sender, TListItem *Item, bool Selected)
{
	AnsiString sStr;
	UnicodeString uStr;
	TListItem *item;
	SOUNDFMT inFmt;
	FILEINFO fileInfo;
	SSIZE outFileSize;
	SSIZE outMaxSize;
	SSIZE nSample;
	UPI upi;
	int i,n;
	int ch;
	int mc_ch;
	int samp1,samp2;
//	int rate[]={32000,44100,48000,88200,96000,176400,192000,352800,384000,705600,768000,1536000};
	char *p;

	memset(&inFmt,0,sizeof (SOUNDFMT));
	inFmt.scan_mp3_flag = 1;
	view_information->Text = L"";
	edt_OutputSize->Text   = L"";
	if (btnStart->Visible == true) {
		btnInfo->Enabled = false;
		btnUp->Enabled = false;
		btnDown->Enabled = false;

		if (LvFile->SelCount > 0) {
			btnDel->Enabled = true;

			for (i = 0;i < LvFile->Items->Count;i++) {
				if (LvFile->Items->Item[i]->Selected) {
					item = LvFile->Items->Item[i];
					if (btnPause->Tag == 1) {
						if (item->ImageIndex == 2) {
							btnOK_Clear->Enabled = true;
						} else {
							btnOK_Clear->Enabled = false;
						}
					}

					if (LvFile->SelCount == 1) {
						if (item->Data) {
							uStr = GetFilePath(1, (wchar_t *)item->Data);
							uStr = LowerCase(ExtractFileExt(uStr));

							if (uStr == L".wav" || uStr == L".flac" || uStr == L".dsf") {
								btnInfo->Enabled = true;
							}
							if (uStr == L".wav") {
								uStr = GetFilePath(1, (wchar_t *)item->Data);
								sStr = uStr;
								if (PLG_GetUpcTagInfo(sStr.c_str(),&upi) == STATUS_SUCCESS) {
									if (strlen(upi.version) > 0) {
										btnDP->Enabled = true;
									} else {
										btnDP->Enabled = false;
									}
								}
							}
						}
					}
				}
			}
		} else {
			btnDel->Enabled = false;
			btnInfo->Enabled = false;
			btnDP->Enabled = false;
		}
		if (LvFile->SelCount == 1) {
			if (LvFile->Selected->Index > 0) {
				btnUp->Enabled = true;
			} else {
				btnUp->Enabled = false;
			}
			if (LvFile->Selected->Index + 1 < LvFile->Items->Count) {
				btnDown->Enabled = true;
			} else {
				btnDown->Enabled = false;
			}
			item = NULL;
			for (i = 0;i < LvFile->Items->Count;i++) {
				if (LvFile->Items->Item[i]->Selected) {
					item = LvFile->Items->Item[i];
					break;
				}
			}
			if (item != NULL) {
				UpdateFileSize();
#if 0
				uStr = GetFilePath(1, (wchar_t *)item->Data);
				sStr = uStr;
				if (PLG_InfoAudioData(sStr.c_str(), &inFmt, &nSample, &fileInfo) == STATUS_SUCCESS) {
					view_information->Text = inFmt.extra;
					if (inFmt.sample > 0 && inFmt.channel > 0) {
						mc_ch = 0;
						samp1 = inFmt.sample;
						if (inFmt.fmt == L"dsf") {
							if (inFmt.sample == 2822400) {
								samp1 = 192000;
							} else if (inFmt.sample == 2822400 * 2) {
								samp1 = 192000 * 2;
							} else if (inFmt.sample == 2822400 * 4) {
								samp1 = 192000 * 4;
							}
						}
						samp2 = cmbSampRate->Text.ToInt();

						if (cmbBitWidth->ItemIndex == 0) {
							outFileSize = 2;
						} else if (cmbBitWidth->ItemIndex == 1) {
							outFileSize = 3;
						} else if (cmbBitWidth->ItemIndex == 2) {
							outFileSize = 4;
						} else if (cmbBitWidth->ItemIndex == 3) {
							outFileSize = 8;
						} else if (cmbBitWidth->ItemIndex == 4) {
							outFileSize = 4;
						}
						outFileSize *= samp2;
						outFileSize *= inFmt.duration_sec;
						ch = inFmt.channel;
						sStr = edtDefaultParam->Text;

						p = strstr(sStr.c_str(),"-C");
						if (p != NULL) {
							mc_ch++;
						}
						p = strstr(sStr.c_str(),"-SLR");
						if (p != NULL) {
							mc_ch+=2;
						}
						p = strstr(sStr.c_str(),"-LFE");
						if (p != NULL) {
							mc_ch++;
						}
						p = strstr(sStr.c_str(),"-mc_downmix");
						if (p != NULL) {
							mc_ch = 0;
						}
						ch += mc_ch;

						outFileSize *= ch;
						outFileSize /= 1024;
						outFileSize /= 1024;

						ch = inFmt.channel;
						sStr = edtDefaultParam->Text;
						p = strstr(sStr.c_str(),"-LRC_process:1");
						if (p != NULL) {
							ch++;
						}
						p = strstr(sStr.c_str(),"-oversamp:");
						if (p != NULL) {
							if (sscanf(&p[10],"%d",&n) == 1) {
								if (n == 1) {
									samp2 *= 2;
								} else if (n == 2) {
									samp2 = 384000;
								} else if (n == 3) {
									samp2 = 1536000;
								} else if (n == 4) {
									samp2 = 192000;
								} else if (n == 5) {
									samp2 = 768000;
								} else if (n == 6) {
									samp2 = 3072000;
								}
							}
						}
						outMaxSize = 8;

						ch = inFmt.channel;
						sStr = edtDefaultParam->Text;
						p = strstr(sStr.c_str(),"-C");
						if (p != NULL) {
							mc_ch++;
						}
						p = strstr(sStr.c_str(),"-SLR");
						if (p != NULL) {
							mc_ch+=2;
						}
						p = strstr(sStr.c_str(),"-LFE");
						if (p != NULL) {
							mc_ch++;
						}
						ch += mc_ch;
						p = strstr(sStr.c_str(),"-LRC");
						if (p != NULL) {
							ch++;
						}
						outMaxSize *= samp2;
						outMaxSize *= inFmt.duration_sec;
						outMaxSize *= ch;
						outMaxSize *= 2;	// work file
						outMaxSize /= 1024;
						outMaxSize /= 1024;
						uStr.sprintf(L"FileSize(MB):%lld, WorkFile(MB):%lld",outFileSize,outMaxSize);
						edt_OutputSize->Text = uStr;
					}
				}
#endif
			}
		}
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::UpdateFileSize(void)
{
	AnsiString sStr;
	UnicodeString uStr;
	TListItem *item;
	SOUNDFMT inFmt;
	FILEINFO fileInfo;
	SSIZE outFileSize;
	SSIZE outMaxSize;
	SSIZE nSample;
	UPI upi;
	int i,n;
	int ch;
	int mc_ch;
	int samp1,samp2;
	char *p;

	memset(&inFmt,0,sizeof (SOUNDFMT));
	inFmt.scan_mp3_flag = 1;
	view_information->Text = L"";
	edt_OutputSize->Text   = L"";

	if (btnStart->Visible == true) {
		if (LvFile->SelCount == 1) {
			item = NULL;
			for (i = 0;i < LvFile->Items->Count;i++) {
				if (LvFile->Items->Item[i]->Selected) {
					item = LvFile->Items->Item[i];
					break;
				}
			}
			if (item != NULL) {
				uStr = GetFilePath(1, (wchar_t *)item->Data);
				sStr = uStr;
				if (PLG_InfoAudioData(sStr.c_str(), &inFmt, &nSample, &fileInfo) == STATUS_SUCCESS) {
					view_information->Text = inFmt.extra;
					if (inFmt.sample > 0 && inFmt.channel > 0) {
						mc_ch = 0;
						samp1 = inFmt.sample;
						if (inFmt.fmt == L"dsf") {
							if (inFmt.sample == 2822400) {
								samp1 = 192000;
							} else if (inFmt.sample == 2822400 * 2) {
								samp1 = 192000 * 2;
							} else if (inFmt.sample == 2822400 * 4) {
								samp1 = 192000 * 4;
							}
						}
						samp2 = cmbSampRate->Text.ToInt();

						if (cmbBitWidth->ItemIndex == 0) {
							outFileSize = 2;
						} else if (cmbBitWidth->ItemIndex == 1) {
							outFileSize = 3;
						} else if (cmbBitWidth->ItemIndex == 2) {
							outFileSize = 4;
						} else if (cmbBitWidth->ItemIndex == 3) {
							outFileSize = 8;
						} else if (cmbBitWidth->ItemIndex == 4) {
							outFileSize = 4;
						}
						outFileSize *= samp2;
						outFileSize *= inFmt.duration_sec;
						ch = inFmt.channel;
						sStr = edtDefaultParam->Text;

						p = strstr(sStr.c_str(),"-C");
						if (p != NULL) {
							mc_ch++;
						}
						p = strstr(sStr.c_str(),"-SLR");
						if (p != NULL) {
							mc_ch+=2;
						}
						p = strstr(sStr.c_str(),"-LFE");
						if (p != NULL) {
							mc_ch++;
						}
						p = strstr(sStr.c_str(),"-mc_downmix");
						if (p != NULL) {
							mc_ch = 0;
						}
						ch += mc_ch;

						outFileSize *= ch;
						outFileSize /= 1024;
						outFileSize /= 1024;

						ch = inFmt.channel;
						sStr = edtDefaultParam->Text;
						p = strstr(sStr.c_str(),"-LRC_process:1");
						if (p != NULL) {
							ch++;
						}
						p = strstr(sStr.c_str(),"-oversamp:");
						if (p != NULL) {
							if (sscanf(&p[10],"%d",&n) == 1) {
								if (n == 1) {
									samp2 *= 2;
								} else if (n == 2) {
									samp2 = 384000;
								} else if (n == 3) {
									samp2 = 1536000;
								} else if (n == 4) {
									samp2 = 192000;
								} else if (n == 5) {
									samp2 = 768000;
								} else if (n == 6) {
									samp2 = 3072000;
								}
							}
						}
						outMaxSize = 8;

						ch = inFmt.channel;
						sStr = edtDefaultParam->Text;
						p = strstr(sStr.c_str(),"-C");
						if (p != NULL) {
							mc_ch++;
						}
						p = strstr(sStr.c_str(),"-SLR");
						if (p != NULL) {
							mc_ch+=2;
						}
						p = strstr(sStr.c_str(),"-LFE");
						if (p != NULL) {
							mc_ch++;
						}
						ch += mc_ch;
						p = strstr(sStr.c_str(),"-LRC");
						if (p != NULL) {
							ch++;
						}
						outMaxSize *= samp2;
						outMaxSize *= inFmt.duration_sec;
						outMaxSize *= ch;
						outMaxSize *= 2;	// work file
						outMaxSize /= 1024;
						outMaxSize /= 1024;
						uStr.sprintf(L"FileSize(MB):%lld, WorkFile(MB):%lld",outFileSize,outMaxSize);
						edt_OutputSize->Text = uStr;
					}
				}
			}
		}
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnClearClick(TObject *Sender) {

	view_information->Text = L"";
	edt_OutputSize->Text   = L"";

	LvFile->Items->Clear();
	btnClear->Enabled = false;
	btnDel->Enabled = false;
	btnUp->Enabled = false;
	btnDown->Enabled = false;
	btnInfo->Enabled = false;
	btnStart->Enabled = false;
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::edtOutputPathLeftButtonClick(TObject *Sender) {
	WideString root = "";

	String dir = "";


	if (lbl_select_directory->Caption != "" && DirectoryExists(lbl_select_directory->Caption,false)) {
		dir = lbl_select_directory->Caption;
	}

	if (SelectDirectory("Upconv", root, dir)) {
		edtOutputPath->Text = dir;
		edtOutputPath->RightButton->Visible = true;
		lbl_select_directory->Caption = dir;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::edtOutputPathRightButtonClick(TObject *Sender) {
	edtOutputPath->Text = L"Source Directory";
	if (edtSuffix->Text == "") {
		edtSuffix->Text = "upconv";
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::edtSuffixRightButtonClick(TObject *Sender) {
	if (edtOutputPath->Text == L"Source Directory") {
		edtSuffix->Text = "upconv";
	}
	else {
		edtSuffix->Text = "";
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::LvFileChange(TObject *Sender, TListItem *Item,
	TItemChange Change)

{
	// if (LvFile->Items->Count > 0) {
	// btnStart->Enabled = true;
	// } else {
	// btnStart->Enabled = false;
	// }
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnStartClick(TObject *Sender) {
	int ret;
	int i;
	int hfa;
	int sizeIsOver;
	SOUNDFMT inFmt;
	DWORD nSample;
	double mb, sz;
	int bit[] = {16, 24, 32, 64};
	AnsiString command;
	AnsiString s,sHFA, sRate, sBitWidth, sHFC, sLFC;
	AnsiString exec_count;
	TStringDynArray cdp_array;
	UnicodeString uStr;
	AnsiString sStr;
   	char *p;

	edtEncoderInfo->Text = memoEncoderInfo->Lines->Strings[cmbEncoder->ItemIndex];
	s = cmbEncoder->Text;
	p = strchr(s.c_str(),':');
	if (p != NULL) p++;

	if (p != NULL && strlen(p) > 0 && strcmp(p,"Unavailable")) {
		uStr = cmbEncoder->Text;s = s + L" Unavailable.";
		Application->MessageBoxW((wchar_t *)uStr.c_str(),L"");
		return;
	}

	if (cmbOutputFormat->Text == L"W64(Not Support)") {
		Application->MessageBoxW(L"W64 Not Supported.",L"");
		return;
	}

	if (btn_level_scan->Tag == 0 && cmbNormalize->ItemIndex == 2 && edt_volume_level->Text == L"") {
		Application->MessageBoxW(L"Please Set Volume Level",L"");
		return;
	}

	sizeIsOver = false;
	pageMain->TabIndex = 0;
	if (edtHFC_Overwrite->Text != "" && edtHFC_Overwrite->Text.ToInt() < 4000) {
		edtHFC_Overwrite->Text = "";
		Application->MessageBoxW(L"HFC >= 4000", L"", MB_OK);
		return;
	}

	for (i = 0; i < LvFile->Items->Count; i++) {
		if (btnPause->Tag == 0) {
			LvFile->Items->Item[i]->ImageIndex = 0;
			LvFile->Items->Item[i]->Caption = L"";
		} else {
			if (LvFile->Items->Item[i]->ImageIndex != 2) {
				LvFile->Items->Item[i]->ImageIndex = 0;
				LvFile->Items->Item[i]->Caption = L"";
			}
		}
	}
	btnPause->Tag = 0;

	if (pnlPopup->Visible == true)
		pnlPopup->Tag = 1;
	btnParam2->BevelOuter = bvRaised;
	pnlPopup->Visible = false;
	btnStart->Enabled = false;
	btnStart->Visible = false;
	btnAbort->Visible = true;
	btnOK_Clear->Enabled = false;
	if (chk_change_default_parameter->Checked == false) {
		btnPause->Enabled = true;
	}
	btnExit->Enabled = false;
	prgExec->Visible = true;

	// 2021/12/05
	//pageMain->Enabled = false;
	btnAdd->Enabled = false;
	btnDel->Enabled = false;
	btnClear->Enabled = false;
	btnUp->Enabled = false;
	btnDown->Enabled = false;
	GroupBox1->Enabled = false;
	GroupBox2->Enabled = false;
	GroupBox5->Enabled = false;
	GroupBox6->Enabled = false;
	GroupBox7->Enabled = false;
	GroupBox8->Enabled = false;
	GroupBox9->Enabled = false;


	exec_count.sprintf("[0/%d]", LvFile->Items->Count);
	lbl_count->Caption = exec_count;
	lbl_count->Visible = true;
	DragAcceptFiles(LvFile->Handle, false);
	Abort = 0;
	VisibleIndex = 0;
	LvFile->Items->Item[0]->Selected = true;
	LvFile->Items->Item[0]->MakeVisible(true);

	cmbDefaultParameter->Tag = cmbDefaultParameter->ItemIndex;
	edtDefParamCount->Tag = 0;
	if (chk_change_default_parameter->Checked == true && UpDDefParamCount->Position > 1) {
		edtDefParamCount->Tag = UpDDefParamCount->Position - 1;
	}

	memo_LvFile_backup->Lines->Clear();
	memo_output_files->Lines->Clear();
	memo_output_end_files->Lines->Clear();
	for (i = 0; i < LvFile->Items->Count; i++) {
		memo_output_files->Lines->Add(L"");
		memo_LvFile_backup->Lines->Add((wchar_t *)LvFile->Items->Item[i]->Data);
	}
	cmbSplit->Tag = 0;

	CoDefParamIndex = cmbDefaultParameter->ItemIndex + 1;
	Timer1->Tag = 0;
	Timer1->Enabled = true;
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::DeleteErrorFile(int index){
	int  split_accept;
	char fullpath[_MAX_PATH];
	char workdrive[_MAX_DRIVE];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char workdir[_MAX_DIR];
	char drive2[_MAX_DRIVE];
	char dir2[_MAX_DIR];
	char file[_MAX_FNAME];
	char file2[_MAX_FNAME];
	char workfile[_MAX_FNAME];
	char ext[_MAX_EXT];
	char ext2[_MAX_EXT];
	char suffix[_MAX_EXT];
	char suffix_org[_MAX_EXT];
	AnsiString cmd, cmdParam, defaultParamFile, outDir, fileName;
	AnsiString fromFile, toFile, flacFile, workpath;
	AnsiString sStr;
	UnicodeString uStr;

	uStr = GetFilePath(2, (wchar_t *)(LvFile->Items->Item[index]->Data));
	sStr = AnsiString(uStr);
	strcpy(fullpath, sStr.c_str());
	uStr = GetFilePath(1, (wchar_t *)(LvFile->Items->Item[index]->Data));
	fromFile = uStr;
	_splitpath(fullpath, drive, dir, file, ext);
	suffix[0] = '\0';
	suffix_org[0] = '\0';
	if (edtOutputPath->Text == L"Source Directory" && edtSuffix->Text == "") {
		strcat(suffix, "_upconv");
	} else if (edtSuffix->Text.Length() > 0) {
		strcat(suffix, "_");
		strcat(suffix, ((AnsiString)edtSuffix->Text).c_str());
	}
	strcpy(suffix_org,suffix);
	strcat(suffix,"_temp");

	if (cmbSplit->Tag > 0) {
		int len;
		char *p = NULL;
		p = strstr(file,suffix);
		if (p != NULL) {
			*p = '\0';
			len = strlen(suffix);
			p += len;
			strcpy(file2,p);
			strcat(file,suffix_org);
			strcat(file,file2);
		}
	} else if (split_accept == 1) {
		strcat(file,suffix);
	} else {
		strcat(file,suffix_org);
	}


	if (edtOutputPath->Text.Length() > 0 && edtOutputPath->Text != L"Source Directory") {
		outDir = (AnsiString)edtOutputPath->Text;
		outDir += "\\";
		_splitpath(outDir.c_str(), drive2, dir2, file2, ext2);
		_makepath(fullpath, drive2, dir2, file, "wav");
	} else {
		_makepath(fullpath, drive, dir, file, "wav");
	}

	// err ファイルの削除
	toFile = (AnsiString)(fullpath);
	_splitpath(toFile.c_str(), drive, dir, file, ext);
	_makepath(fullpath, drive, dir, file, "err");
	DeleteFile(fullpath);
}
// ---------------------------------------------------------------------------
AnsiString __fastcall TForm1::MakeCommandParam(int index,int thread_index,int delete_flag,int *delete_failed) {
	int i;
	int mc_flag;
	int wk_num;
	int dp_item_index;
	int  split_accept;
	int error_delete_failed;
	char fullpath[_MAX_PATH];
	char workdrive[_MAX_DRIVE];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char workdir[_MAX_DIR];
	char drive2[_MAX_DRIVE];
	char dir2[_MAX_DIR];
	char file[_MAX_FNAME];
	char file2[_MAX_FNAME];
	char workfile[_MAX_FNAME];
	char ext[_MAX_EXT];
	char ext2[_MAX_EXT];
	char suffix[_MAX_EXT];
	char suffix_org[_MAX_EXT];
	FILE *fp;
	AnsiString cmd, cmdParam, defaultParamFile, outDir, fileName;
	AnsiString sInRate;
	AnsiString fromFile, toFile, flacFile, workpath;
	AnsiString AppPath = ExtractFilePath(Application->ExeName);
	AnsiString sStr;
	AnsiString sTemp;
	UnicodeString uStr;
	int bit[4] = {16, 24, 32, 64};
	SOUNDFMT inFmt;
	SSIZE nSample;
	MEMORYSTATUSEX mse;
	time_t now;
	struct tm *ptr_now;

	srand((unsigned int)time(NULL));
	now = time(NULL);
	ptr_now = localtime(&now);

	mc_flag = 0;
	split_accept = 0;

	uStr = GetFilePath(1, (wchar_t *)LvFile->Items->Item[index]->Data);
	strcpy(fullpath, ((AnsiString)uStr).c_str());
	PLG_InfoAudioData(fullpath, &inFmt, &nSample, NULL);
	if ((AnsiString)inFmt.fmt == "wav" || (AnsiString)inFmt.fmt == "rf64") {
		if ((nSample / inFmt.sample) >= (60)) split_accept = 1;
	}

	cmdParam = "";
	cmdParam += "-s:";
	if (cmbSampRate->ItemIndex < 12) {
		cmdParam += (AnsiString)cmbSampRate->Text;
	}

	sTemp = "";
	if (cmbBitWidth->ItemIndex == 0) {
		sTemp.sprintf(" -w:%d", 16);
	} else if (cmbBitWidth->ItemIndex == 1) {
		sTemp.sprintf(" -w:%d", 24);
	} else if (cmbBitWidth->ItemIndex == 2) {
		sTemp.sprintf(" -w:%d", 32);
	} else if (cmbBitWidth->ItemIndex == 3) {
		sTemp.sprintf(" -w:%d", 64);
	} else if (cmbBitWidth->ItemIndex == 4) {
		sTemp.sprintf(" -w:%d -w_int", 32);
	}
	cmdParam += sTemp;

	cmdParam += " -hfa:";
	cmdParam += (AnsiString)cmbHFA->ItemIndex;

	cmdParam += " -norm:";
	cmdParam += (AnsiString)cmbNormalize->ItemIndex;
	if (cmbNormalize->ItemIndex == 2 && edt_volume_level->Text != L"") {
		sTemp.sprintf(" -volume_level:%d",edt_volume_level->Text.ToInt());
		cmdParam += sTemp;
	}
	if (cmbNormalize->ItemIndex == 3 && edt_volume_level->Text != L"") {
		sTemp.sprintf(" -volume_per:%d",edt_volume_level->Text.ToInt());
		cmdParam += sTemp;
	}

	if (split_accept == 1) {
		if (cmbSplit->ItemIndex == 1) {
			cmdParam += " -split_size:300";
		} else if (cmbSplit->ItemIndex == 2) {
			cmdParam += " -split_size:600";
		} else {
			split_accept = 0;
		}
	}
	if (split_accept == 0 && chk_conversion_test->Checked == false) {

		if (cmbADDP->ItemIndex == 0) {
			cmdParam += " -addp:0";
		} else {
			cmdParam += " -addp:" +  cmbADDP->Items->Strings[cmbADDP->ItemIndex];
		}
	}

	if (chk_conversion_test->Checked == true) {
		if (cmb_ct_min->ItemIndex == 0) {
			cmdParam += " -conv_test:10";
		} else if (cmb_ct_min->ItemIndex == 1) {
			cmdParam += " -conv_test:30";
		} else if (cmb_ct_min->ItemIndex == 2) {
			cmdParam += " -conv_test:60";
		} else if (cmb_ct_min->ItemIndex == 3) {
			cmdParam += " -conv_test:90";
		} else if (cmb_ct_min->ItemIndex == 4) {
			cmdParam += " -conv_test:120";
		}
	}
	if (chk_output_mid_side->Checked == true) {
		cmdParam += " -output_mid_side";
	}
	if (chk_output_Left_Right_Center->Checked == true) {
		cmdParam += " -output_left_right_center";
	}

	cmdParam += " -thread:";
	cmdParam += (AnsiString)cmbThread->Text;

	cmdParam += " -fio:";
	cmdParam += cmbFileIO->Items->Strings[cmbFileIO->ItemIndex];

	cmdParam += " -cpu_pri:";
	cmdParam += cmbPriority->ItemIndex;
	if (chkFIO_Ex->Checked == true) {
		cmdParam += " -fio_auto:1";
		cmdParam += " -fio_max:" + cmbFileIOMax->Text;
	}
	if (rdo_file_io_sec->Checked) {
		if (cmbFIOSecSize->ItemIndex == 0) {
			cmdParam += " -fio_sec_size:10";
		} else if (cmbFIOSecSize->ItemIndex == 1) {
			cmdParam += " -fio_sec_size:30";
		} else if (cmbFIOSecSize->ItemIndex == 2) {
			cmdParam += " -fio_sec_size:60";
		} else if (cmbFIOSecSize->ItemIndex == 3) {
			cmdParam += " -fio_sec_size:120";
		} else if (cmbFIOSecSize->ItemIndex == 4) {
			cmdParam += " -fio_sec_size:300";
		}
	} else {
		if (cmbFIOMBSize->ItemIndex == 0) {
			cmdParam += " -fio_mb_size:5";
		} else if (cmbFIOMBSize->ItemIndex == 1) {
			cmdParam += " -fio_mb_size:50";
		} else if (cmbFIOMBSize->ItemIndex == 2) {
			cmdParam += " -fio_mb_size:100";
		} else if (cmbFIOMBSize->ItemIndex == 3) {
			cmdParam += " -fio_mb_size:500";
		} else if (cmbFIOMBSize->ItemIndex == 4) {
			cmdParam += " -fio_mb_size:1000";
		}
	}

	if (cmbSampRate->ItemIndex < 12) {
		if (cmbOutputFormat->ItemIndex == 1) {
			cmdParam += " -wav2g";
		}
		if (cmbOutputFormat->ItemIndex == 2) {
			cmdParam += " -bwf";
		}
		if (cmbOutputFormat->ItemIndex == 3) {
			cmdParam += " -rf64";
		}
		if (cmbOutputFormat->ItemIndex == 4) {
			cmdParam += " -w64";
		}
		if (cmbOutputFormat->ItemIndex == 5) {
			cmdParam += " -wav_auto";
		}
	}
	if (chk_output_mp4->Checked == true) {
		cmdParam += " -out_mp4";
	}
	if (chk_save_dp->Checked == true) {
		cmdParam += " -dp_save";
	}

	uStr = GetFilePath(2, (wchar_t *)(LvFile->Items->Item[index]->Data));
	sStr = AnsiString(uStr);
	strcpy(fullpath, sStr.c_str());
	uStr = GetFilePath(1, (wchar_t *)(LvFile->Items->Item[index]->Data));
	fromFile = uStr;
	_splitpath(fullpath, drive, dir, file, ext);
	suffix[0] = '\0';
	suffix_org[0] = '\0';
	if (edtOutputPath->Text == L"Source Directory" && edtSuffix->Text == "") {
		strcat(suffix, "_upconv");
	} else if (edtSuffix->Text.Length() > 0) {
		strcat(suffix, "_");
		strcat(suffix, ((AnsiString)edtSuffix->Text).c_str());
	}
	strcpy(suffix_org,suffix);
	strcat(suffix,"_temp");

	if (cmbSplit->Tag > 0) {
		int len;
		char *p = NULL;
		p = strstr(file,suffix);
		if (p != NULL) {
			*p = '\0';
			len = strlen(suffix);
			p += len;
			strcpy(file2,p);
			strcat(file,suffix_org);
			strcat(file,file2);
		}
	} else if (split_accept == 1) {
		strcat(file,suffix);
	} else {
		strcat(file,suffix_org);
	}

	if (chk_change_default_parameter->Checked == true) {
		int n;
		sStr = cmbDefaultParameter->Items->Strings[cmbDefaultParameter->ItemIndex];
		if (sscanf(sStr.c_str(),"%d%*s",&n) == 1) {
			sTemp.sprintf("%s_%d", file,n);
			strcpy(file, sTemp.c_str());
		}
	}

	if (btn_level_scan->Tag == 1) {
		char ls_str[256];
		sprintf(ls_str,"%s_%04d%02d%02d%02d%02d%02d%d",file,ptr_now->tm_year+1900,ptr_now->tm_mon + 1,ptr_now->tm_mday,ptr_now->tm_hour,ptr_now->tm_min,ptr_now->tm_sec,rand());
		strcpy(file,ls_str);
	}

	if (edtOutputPath->Text.Length() > 0 && edtOutputPath->Text != L"Source Directory") {
		outDir = (AnsiString)edtOutputPath->Text;
		outDir += "\\";
		_splitpath(outDir.c_str(), drive2, dir2, file2, ext2);
		_makepath(fullpath, drive2, dir2, file, "wav");
	} else {
		_makepath(fullpath, drive, dir, file, "wav");
	}

	toFile = (AnsiString)(fullpath);

	// ファイルの削除
	_splitpath(toFile.c_str(), drive, dir, file, ext);
	_makepath(fullpath, drive, dir, file, "wav");
	if (delete_flag) {
		threadExec[thread_index]->toFile = (AnsiString)fullpath;
	}
	fileName = (AnsiString)fullpath;
	if (delete_flag) {
		DeleteFile(fileName);
		if (PathFileExists(((UnicodeString)fileName).c_str())) {
			*delete_failed = 1;
		}
	}
	_splitpath(toFile.c_str(), drive, dir, file, ext);
	_makepath(fullpath, drive, dir, file, "err");
	if (delete_flag) {
		threadExec[thread_index]->errFile = (AnsiString)fullpath;
	}
	fileName = (AnsiString)fullpath;
	if (delete_flag) {
		DeleteFile(fileName);
		if (PathFileExists(((UnicodeString)fileName).c_str())) {
			*delete_failed = 1;
		}
	}
	if (cmbEncoder->ItemIndex != 0) {
		if (cmbEncoder->ItemIndex == 1) {
			cmdParam += L" -encorder:1";
			_splitpath(toFile.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file, "flac");
			fileName = (AnsiString)fullpath;
			if (delete_flag) {
				DeleteFile(fileName);
				if (PathFileExists(((UnicodeString)fileName).c_str())) {
					*delete_failed = 1;
				}
			}
		}
		if (cmbEncoder->ItemIndex == 2) {
			cmdParam += L" -encorder:2";
			_splitpath(toFile.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file, "wv");
			fileName = (AnsiString)fullpath;
			if (delete_flag) {
				DeleteFile(fileName);
				if (PathFileExists(((UnicodeString)fileName).c_str())) {
					*delete_failed = 1;
				}
			}
		}
		if (cmbEncoder->ItemIndex == 3) {
			cmdParam += L" -encorder:3";
			_splitpath(toFile.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file, "mp3");
			fileName = (AnsiString)fullpath;
			if (delete_flag) {
				DeleteFile(fileName);
				if (PathFileExists(((UnicodeString)fileName).c_str())) {
					*delete_failed = 1;
				}
			}
		}
		if (cmbEncoder->ItemIndex == 4) {
			cmdParam += L" -encorder:4";
			_splitpath(toFile.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file, "opus");
			fileName = (AnsiString)fullpath;
			if (delete_flag) {
				DeleteFile(fileName);
				if (PathFileExists(((UnicodeString)fileName).c_str())) {
					*delete_failed = 1;
				}
			}
		}
		if (cmbEncoder->ItemIndex == 5) {
			cmdParam += L" -encorder:5";
			_splitpath(toFile.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file, "m4a");
			fileName = (AnsiString)fullpath;
			if (delete_flag) {
				DeleteFile(fileName);
				if (PathFileExists(((UnicodeString)fileName).c_str())) {
					*delete_failed = 1;
				}
			}
		}
		if (cmbEncoder->ItemIndex > 5) {
			sTemp = "";
			sTemp.sprintf(" -encorder:%d",cmbEncoder->ItemIndex);
			cmdParam += sTemp;
			_splitpath(toFile.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file,((AnsiString)(memoEncoderExt->Lines->Strings[cmbEncoder->ItemIndex - 1])).c_str());
			fileName = (AnsiString)fullpath;
			if (delete_flag) {
				DeleteFile(fileName);
				if (PathFileExists(((UnicodeString)fileName).c_str())) {
					*delete_failed = 1;
				}
			}
		}
	}

	if (LvFile->Items->Item[index]->SubItems->Strings[4].Length() > 0) {
		cmdParam += L" -enable_hfc:1 -hfc:" + LvFile->Items->Item[index]->SubItems->Strings[4];
	} else {
		if (edtHFC_Overwrite->Text != L"" && edtHFC_Overwrite->Text.ToInt() >= 4000) {
			cmdParam += L" -enable_hfc:1 -hfc:" + edtHFC_Overwrite->Text;
		}
	}
	if (chkOS_Overwrite->Checked == true) {
		if (cmbOverSamplingOverWrite->ItemIndex == 1) {
			sStr.sprintf(" -oversamp:%d", 1);
		} else if (cmbOverSamplingOverWrite->ItemIndex == 2) {
			sStr.sprintf(" -oversamp:%d",4);
		} else if (cmbOverSamplingOverWrite->ItemIndex == 3) {
			sStr.sprintf(" -oversamp:%d",2);
		} else if (cmbOverSamplingOverWrite->ItemIndex == 4) {
			sStr.sprintf(" -oversamp:%d",5);
		} else if (cmbOverSamplingOverWrite->ItemIndex == 5) {
			sStr.sprintf(" -oversamp:%d",3);
		} else if (cmbOverSamplingOverWrite->ItemIndex == 6) {
			sStr.sprintf(" -oversamp:%d",6);
		}
		cmdParam += sStr;
	}

	if (cmbADDP->ItemIndex == 0) {
		if (LvFile->Items->Item[index]->SubItems->Strings[5].Length() > 0) {
			int hh,mm,ss;
			char ts[20];
			sStr = LvFile->Items->Item[index]->SubItems->Strings[5];
			if (sscanf(sStr.c_str(),"%d:%d",&mm,&ss) == 2) {
				sprintf(ts,"%ld",(mm * 60) + ss);
				cmdParam += " -skip_start_offset:" + (UnicodeString)ts;
			} else if (sscanf(sStr.c_str(),"%d:%d:%d",&hh,&mm,&ss) == 3) {
				sprintf(ts,"%ld",(hh * 3600) + (mm * 60) + ss);
				cmdParam += " -skip_start_offset:" + (UnicodeString)ts;
			}
		}
	}

	sStr.sprintf(" -enable_overwrite_abe:%d",chk_ABE_Overrite->Checked == true ? 1 : 0);cmdParam += sStr;
	sStr.sprintf(" -enable_overwrite_post_abe:%d",chk_PostABE_Overrite->Checked == true ? 1 : 0);cmdParam += sStr;
	sStr.sprintf(" -enable_overwrite_nr:%d",chk_NR_Overrite->Checked == true ? 1 : 0);cmdParam += sStr;
	if (chk_MS_Overrite->Enabled == true) {
		sStr.sprintf(" -enable_overwrite_ms:%d",chk_MS_Overrite->Checked == true ? 1 : 0);cmdParam += sStr;
	}
	if (chk_LRC_Overrite->Enabled == true) {
		sStr.sprintf(" -enable_overwrite_lrc:%d",chk_LRC_Overrite->Checked == true ? 1 : 0);cmdParam += sStr;
	}

	if (btn_level_scan->Tag == 1) {
		cmdParam += L" -level_scan_only";
	}

	sStr = cmbDefaultParameter->Items->Strings[cmbDefaultParameter->ItemIndex];
	if (sscanf(sStr.c_str(),"%d%*s",&dp_item_index) == 1) {
		defaultParamFile.printf("%s\\upconv_default_%02d.txt",(AnsiString)defaultParameterPath,dp_item_index);
		cmd.printf("\"%supconv.exe\" \"%s\" \"%s\" \"%s\" \"%s\"", AppPath.c_str(),fromFile, toFile, defaultParamFile, cmdParam);
	}

	return cmd;
}

// ---------------------------------------------------------------------------
int __fastcall TForm1::ExecChildProcess(int index) {
	int i;
	int thread_index;
	int error_delete_failed;
	AnsiString cmd,fn;
	FILE *fp;
	if (index < LvFile->Items->Count) {
		DeleteErrorFile(index);
		for (thread_index = 0; thread_index < cmbUpconvFile->Text.ToInt();thread_index++) {
			if (threadExec[thread_index] == NULL) {
				threadExec[thread_index] = new Exec_c(true);
				threadExec[thread_index]->FreeOnTerminate = true;
				threadExec[thread_index]->index = index;
				threadExec[thread_index]->flag_progress = 0;
				threadExec[thread_index]->flag_finish = 0;
				threadExec[thread_index]->percent = 0;
				break;
			}
		}
		if (thread_index == cmbUpconvFile->Text.ToInt()) return -1;
		error_delete_failed = 0;

		LvFile->Items->Item[index]->Caption = "...";
		LvFile->Items->Item[index]->ImageIndex = 1;

		threadExec[thread_index]->cpu_pri = cmbPriority->ItemIndex;
		threadExec[thread_index]->cmd = MakeCommandParam(index,thread_index,1,&error_delete_failed);
#if 0
		if (error_delete_failed) {
			if (threadExec[thread_index]->errFile != NULL) {
				fn = threadExec[thread_index]->errFile;
				fp = fopen(fn.c_str(),"w");
				if (fp != NULL) fclose(fp);
				LvFile->Items->Item[index]->ImageIndex = 3;
				LvFile->Items->Item[index]->Caption = L"Error";
			}
			return 0;
		}
#endif
		//memoLog->Lines->Add(threadExec[thread_index]->cmd);
		threadExec[thread_index]->Start();

	}
	return 0;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::EndExec(int index, DWORD exitCode) {
	UnicodeString uStr;
	int i, thread_index;
	int endFlag;
	unsigned short ID;
	int error = true;
	FILE *fp;
	ID = exitCode >> 16;
	exitCode &= 0xFFFF;
	uStr.sprintf(L"EndExec[%d]", index);
	memoLog->Lines->Add(uStr);

	// Sleep(3000);
	for (thread_index = 0; thread_index < cmbUpconvFile->Text.ToInt();thread_index++) {
		if (threadExec[thread_index] != NULL && threadExec[thread_index]->index == index)
			break;
	}
	if (thread_index == cmbUpconvFile->Text.ToInt()) {
		uStr.sprintf(L"Error");
		memoLog->Lines->Add(uStr);
		return;
	}
	if (btn_level_scan->Tag == 0) {
		fp = fopen(threadExec[thread_index]->errFile.c_str(), "r");
		if (fp == NULL) {
			error = false;
		} else {
			fclose(fp);
		}
	} else {
		uStr.printf(L"%d",threadExec[thread_index]->volume_level);
		LvFile->Items->Item[index]->ImageIndex = 0;
		LvFile->Items->Item[index]->Caption = L"";
		LvFile->Items->Item[index]->SubItems->Strings[3] = uStr;
		threadExec[thread_index]->flag_finish = 1;
		Timer1->Tag = 0;
		return;
	}

	if (error == false) {
		fp = fopen(threadExec[thread_index]->toFile.c_str(), "r");
		if (fp == NULL) {
			error = true;
		}
		else {
			fclose(fp);
		}
	}

	if (error) {
		LvFile->Items->Item[index]->ImageIndex = 3;
		LvFile->Items->Item[index]->Caption = L"Error";
	} else {
		LvFile->Items->Item[index]->ImageIndex = 2;
		LvFile->Items->Item[index]->Caption = L"Ok";
		if (memo_output_files->Lines->Strings[index].Length() > 4) {
			memo_output_end_files->Lines->Add(memo_output_files->Lines->Strings[index]);
		}
		if (chkDeleteSourceFile->Checked == true) {
			uStr = GetFilePath(1,(wchar_t *)(LvFile->Items->Item[index]->Data));
			unlink(AnsiString(uStr).c_str());
		}
	}
	uStr.sprintf(L"index:%02d...end.", index);
	memoLog->Lines->Add(uStr);
	threadExec[thread_index]->flag_finish = 1;

	Timer1->Tag = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::UpdateProgress(int index, AnsiString status,int percent) {
	int i;
	UnicodeString ustr;

	for (i = 0; i < cmbUpconvFile->Text.ToInt(); i++) {
		if (threadExec[i] != NULL && threadExec[i]->Finished == false && threadExec[i]->index == index && threadExec[i]->flag_progress == 1) {
			prgExec->Position = percent;
			lbl_exec_status->Caption = status;
			break;
		}
	}

	// ustr.printf(L"%s...%03d",(UnicodeString)status,percent);
	// memoLog->Lines->Add(ustr);
	// Application->ProcessMessages();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::UpdateADDP(int addp_n,int addp_count) {
	UnicodeString ustr;

	ustr.printf(L"ADDP [%d / %d]",addp_n,addp_count);
	lbl_addp->Caption = ustr;

}
// ---------------------------------------------------------------------------
void __fastcall TForm1::UpdateLog(UnicodeString uStr) {
	memoLog->Lines->Add(uStr);
	Application->ProcessMessages();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Shutdown(void) {
	HANDLE hToken;
	TOKEN_PRIVILEGES tknPrvlgs;
	bool res;

	res = OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	if (res) {
		res = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
			&(tknPrvlgs.Privileges[0].Luid));
		if (res) {
			tknPrvlgs.PrivilegeCount = 1;
			tknPrvlgs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, false, &tknPrvlgs,
				sizeof(TOKEN_PRIVILEGES), 0, 0);
			if (GetLastError() == ERROR_SUCCESS) {
				ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
			}
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender) {
	int i,n,count, exec_flag;
	int exec_i;
	int co;
	FILE *fp;
	char fullpath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[_MAX_FNAME];
	char file2[_MAX_FNAME];
	char ext[_MAX_EXT];
	AnsiString exec_count;
	UnicodeString uStr;
	AnsiString    sStr;

	for (co = 0; co < 2; co++) {
		if (Timer1->Tag == 0) {
			Timer1->Tag = 1;

			// スレッド終了検出
			for (i = 0; i < cmbUpconvFile->Text.ToInt(); i++) {
				if (threadExec[i] != NULL && threadExec[i]->Finished && threadExec[i]->flag_finish) {
					threadExec[i] = NULL;
				}
			}
			if (Abort == 0) {
				if (btn_level_scan->Tag == 0) {
					for (i = 0; i < LvFile->Items->Count; i++) {
						if (LvFile->Items->Item[i]->ImageIndex < 2)
							break;
					}
				} else {
					for (i = 0; i < LvFile->Items->Count; i++) {
						if (LvFile->Items->Item[i]->ImageIndex < 2 || LvFile->Items->Item[i]->SubItems->Strings[3] != L"")
							break;
					}
				}
				if (i < LvFile->Items->Count && i != VisibleIndex) {
					LvFile->Items->Item[LvFile->Items->Count - 1]->Selected = true;
					LvFile->Items->Item[LvFile->Items->Count - 1]->MakeVisible(true);
					Application->ProcessMessages();
					LvFile->Items->Item[i]->Selected = true;
					LvFile->Items->Item[i]->MakeVisible(true);
				}
				count = 0;
				VisibleIndex = i;
				for (i = 0; i < LvFile->Items->Count; i++) {
					if (btn_level_scan->Tag == 0) {
						if (LvFile->Items->Item[i]->ImageIndex == 0 && chkStop->Checked == false) {
							if (ExecChildProcess(i) == 0)
								count++;
							if (count >= cmbUpconvFile->Text.ToInt())
								break;
						}
					} else {
						if (LvFile->Items->Item[i]->SubItems->Strings[3] == L"") {
							if (ExecChildProcess(i) == 0)
								count++;
							if (count >= cmbUpconvFile->Text.ToInt())
								break;
						}
					}
				}
			}
		}
		exec_flag = 0;
		exec_i = 0;
		for (i = 0; i < LvFile->Items->Count; i++) {
			if (LvFile->Items->Item[i]->ImageIndex == 1) {
				exec_flag = 1;
				uStr = L"" + LvFile->Items->Item[i]->Caption.SubString(0, 2);
				if (uStr == L"..") {
					uStr = L">..";
				}
				else if (uStr == L">.") {
					uStr = L".>.";
				}
				else if (uStr == L".>") {
					uStr = L"..>";
				}
				LvFile->Items->Item[i]->Caption = uStr;
			} else if (LvFile->Items->Item[i]->ImageIndex == 0) {
				if (btn_level_scan->Tag == 0 && chkStop->Checked == false)
					exec_flag = 1;
			}
			else if (LvFile->Items->Item[i]->ImageIndex == 2 ||
				LvFile->Items->Item[i]->ImageIndex == 3) {
				exec_i++;
			}
		}
		// プログレスバー用
		for (i = 0; i < cmbUpconvFile->Text.ToInt(); i++) {
			if (threadExec[i] != NULL && threadExec[i]->flag_progress == 1) {
				break;
			}
		}
		if (i == cmbUpconvFile->Text.ToInt()) {
			for (i = 0; i < cmbUpconvFile->Text.ToInt(); i++) {
				if (threadExec[i] != NULL && threadExec[i]->flag_progress == 0)
				{
					threadExec[i]->flag_progress = 1;
					break;
				}
			}
		}

		exec_count.sprintf("[%d/%d]", exec_i, LvFile->Items->Count);
		lbl_count->Caption = exec_count;

		if (Abort) {
			for (i = 0; i < cmbUpconvFile->Text.ToInt(); i++) {
				if (threadExec[i] != NULL) {
					break;
				}
			}
			if (i == cmbUpconvFile->Text.ToInt()) {
				exec_flag = 0;
			}
		}
		if (Abort) co = 2;
		if (exec_flag == 0 && Abort == 0 && chkStop->Checked == false) {
			if (edtDefParamCount->Tag > 0) {
				int co_def_param;
				edtDefParamCount->Tag--;
				co_def_param = cmbDefaultParameter->ItemIndex;
				co_def_param++;
				if (co_def_param < cmbDefaultParameter->Items->Count) {
					cmbDefaultParameter->ItemIndex = co_def_param;
					sStr = cmbDefaultParameter->Items->Strings[cmbDefaultParameter->ItemIndex];
					if (sscanf(sStr.c_str(),"%d%*s",&n) == 1) {
						ReloadDefaultParameter(n,n);
						CoDefParamIndex++;
						for (i = 0; i < LvFile->Items->Count; i++) {
							LvFile->Items->Item[i]->ImageIndex = 0;
							LvFile->Items->Item[i]->Caption = L"";
						}
						Timer1->Tag = 0;
					}
				}
				else {
					co = 2;
				}
			}
			else {
				co = 2;
			}
		}
		else {
			co = 2;
		}
	}

	if (Abort == 0 && exec_flag == 0 && cmbSplit->ItemIndex != 0) {
		if (memo_output_end_files->Lines->Count > 0) {
			LvFile->Items->Clear();
			cmbSplit->Tag = cmbSplit->ItemIndex;
			cmbSplit->ItemIndex = 0;
			for (i = 0;i < memo_output_end_files->Lines->Count;i++) {
				sStr = memo_output_end_files->Lines->Strings[i];
				sprintf(fullpath,"%s",sStr.c_str());
				fp = fopen(fullpath,"rb");
				if (fp != NULL) {
					int j;
					fclose(fp);
					AddToFileList(fullpath);
					for (j = 2;;j++) {
						sStr = memo_output_end_files->Lines->Strings[i];
						strcpy(fullpath,sStr.c_str());
						_splitpath(fullpath, drive, dir, file2, ext);
						sprintf(file,"%s_%d",file2,j);
						_makepath(fullpath,drive,dir,file,ext);
						fp = fopen(fullpath,"rb");
						if (fp != NULL) {
							fclose(fp);
							AddToFileList(fullpath);
						} else {
							break;
						}
					}
				}
			}

			if (LvFile->Items->Count > 0) {
				memo_output_files->Lines->Clear();
				memo_output_end_files->Lines->Clear();
				for (i = 0;i < LvFile->Items->Count;i++) {
					memo_output_files->Lines->Add("");
				}
				for (i = 0;i < LvFile->Items->Count; i++) {
					if (LvFile->Items->Item[i]->ImageIndex < 2)
						break;
				}
				if (i < LvFile->Items->Count && i != VisibleIndex) {
					LvFile->Items->Item[LvFile->Items->Count - 1]->Selected = true;
					LvFile->Items->Item[LvFile->Items->Count - 1]->MakeVisible(true);
					Application->ProcessMessages();
					LvFile->Items->Item[i]->Selected = true;
					LvFile->Items->Item[i]->MakeVisible(true);
				}
				count = 0;
				VisibleIndex = i;
				for (i = 0; i < LvFile->Items->Count; i++) {
					if (LvFile->Items->Item[i]->ImageIndex == 0) {
						if (ExecChildProcess(i) == 0) count++;
						if (count >= cmbUpconvFile->Text.ToInt())
							break;
					}
				}
				exec_flag = 1;
			}
		}
	}

#if 0
	if (exec_flag != 0) {
		int end = 1;
		for (i = 0; i < cmbUpconvFile->Text.ToInt(); i++) {
			if (threadExec[i] != NULL) {
				if (threadExec[i]->IsEndProcess() == false) {
					end = 0;
				}
			}
		}
		if (end == 1) {
			exec_flag = 0;
		}
	}
#endif
	if (exec_flag == 0) {
		if (chk_change_default_parameter->Checked == true) {
			cmbDefaultParameter->ItemIndex = cmbDefaultParameter->Tag;
			sStr = cmbDefaultParameter->Items->Strings[cmbDefaultParameter->ItemIndex];
			if (sscanf(sStr.c_str(),"%d%*s",&n) == 1) {
				ReloadDefaultParameter(n,n);
			}
		}
		if (cmbSplit->Tag > 0) {
			for (i = 0;i < LvFile->Items->Count;i++) {
				uStr = (UnicodeString)((wchar_t *)LvFile->Items->Item[i]->Data);
				sStr = uStr;
				unlink(sStr.c_str());
			}
			if (Abort) {
				for (i = 0;i < memo_output_end_files->Lines->Count;i++) {
					uStr = memo_output_end_files->Lines->Strings[i];
					sStr = uStr;
					unlink(sStr.c_str());
				}
			}
			LvFile->Items->Clear();
			for (i = 0;i < memo_LvFile_backup->Lines->Count;i++) {
				AddToFileList(memo_LvFile_backup->Lines->Strings[i]);
			}
			cmbSplit->ItemIndex = cmbSplit->Tag;
			cmbSplit->Tag = 0;
		}

		Timer1->Enabled = false;
		lbl_exec_status->Caption = L"";
		lbl_count->Visible = false;
		prgExec->Visible = false;
		prgExec->Position = 0;
		btnStart->Enabled = true;
		btnStart->Visible = true;
		btnAbort->Visible = false;
		btnAbort->Enabled = true;
		btnPause->Enabled = false;
		btnExit->Enabled = true;
		lbl_addp->Caption = L"";

		// 2021/12/05
		//pageMain->Enabled = true;
		btnAdd->Enabled = true;
		btnClear->Enabled = true;
		GroupBox1->Enabled = true;
		GroupBox2->Enabled = true;
		GroupBox5->Enabled = true;
		GroupBox6->Enabled = true;
		GroupBox7->Enabled = true;
		GroupBox8->Enabled = true;
		GroupBox9->Enabled = true;

		// 2023/09/24
        chkStop->Checked = false;

		DragAcceptFiles(LvFile->Handle, true);
		if (chkPowerOff->Checked == true) {
			Shutdown();
		}
		LvFile->Items->Item[0]->Selected = true;
		LvFile->Items->Item[0]->MakeVisible(true);
		Abort = 0;
		if (edtHFC_Overwrite->Text != L"" || chkOS_Overwrite->Checked == true) {
			btnParam2->BevelOuter = bvLowered;
			pnlPopup->Visible = true;
		}
		btn_level_scan->Tag = 0;
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnReloadClick(TObject *Sender) {
	int i;
	int index_1 = cmbDefaultParameter->ItemIndex;
	int n;
	FILE *fp;
	AnsiString sStr;
	UnicodeString uStr;

	cmbDefaultParameter->Items->Clear();
	list_dp_active->Items->Clear();
	list_dp_inactive->Items->Clear();
	edtDefaultParam->Text = L"";
	for (i = 1; i <= 99; i++) {
		ReloadDefaultParameter(i, -1);
	}
	for (i = 0;i < list_dp_active->Items->Count;i++) {
		uStr = list_dp_active->Items->Strings[i];
		cmbDefaultParameter->Items->Add(uStr);
	}
	if (btnReload->Tag == 0) {
		// リロード前に選択されていたデフォルトパラメーターを選択しなおす
		if (index_1 < cmbDefaultParameter->Items->Count) {
			cmbDefaultParameter->ItemIndex = index_1;
		}
	} else {
		cmbDefaultParameter->ItemIndex = 0;
	}
	uStr.printf(L"%s\\upconv_default_%02d.txt",defaultParameterPath, index_1 + 1);
	sStr = uStr;
	fp = fopen(sStr.c_str(),"r");
	if (fp == NULL) {
		cmbDefaultParameter->ItemIndex = 0;
	} else {
		fclose(fp);
	}

	sStr = cmbDefaultParameter->Items->Strings[cmbDefaultParameter->ItemIndex];
	if (sscanf(sStr.c_str(),"%d%*s",&n) == 1) {
		ReloadDefaultParameter(n,n);
		cmbDefaultParameter->TextHint = cmbDefaultParameter->Text;
	}
	btnReload->Tag = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnSettingClick(TObject *Sender) {
	int n;
	int portable_mode;
	int change_portable_mode;
	AnsiString sStr;
	AnsiString AppPath = ExtractFilePath(Application->ExeName);
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL ret;
	UnicodeString ucmd;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	ret  = TRUE;
	sStr = cmbDefaultParameter->Items->Strings[cmbDefaultParameter->ItemIndex];

	change_portable_mode = 0;
	if (rdo_upconv_mode_1->Checked == true) {
		portable_mode = 0;
		if (rdo_upconv_mode_1->Tag == 0) {
			change_portable_mode = 1;
		}
	} else if (rdo_upconv_mode_2->Checked == true) {
		portable_mode = 1;
		if (rdo_upconv_mode_2->Tag == 0) {
			change_portable_mode = 1;
		}
	}

	if (sscanf(sStr.c_str(),"%d%*s",&n) == 1) {
		ucmd.sprintf(L"%supconv_conf.exe param=%d portable=%d change=%d", (UnicodeString)AppPath,n,portable_mode,change_portable_mode);
		if (!CreateProcess(NULL, ucmd.c_str(), NULL, NULL, FALSE,IDLE_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
			ret = FALSE;
		} else {
			SetPriorityClass(pi.hProcess, NORMAL_PRIORITY_CLASS);
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnInfoClick(TObject *Sender) {
	UnicodeString uStr;
	TListItem *item;
	if (LvFile->Selected != NULL) {
		item = LvFile->Selected;
		if (item->Data) {
			uStr = GetFilePath(1, (wchar_t *)item->Data);
			Form5->filename = (AnsiString)(uStr);
			Form5->ShowModal();
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnAbortClick(TObject *Sender) {
	int index;
	Abort = 1;
	btnAbort->Enabled = false;
	btnPause->Enabled = false;
	for (index = 0; index < cmbUpconvFile->Text.ToInt(); index++) {
		if (threadExec[index] != NULL)
			threadExec[index]->AbortExec();
	}

}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnParam2Click(TObject *Sender) {
	if (btnParam2->BevelOuter == bvRaised) {
		btnParam2->BevelOuter = bvLowered;
		pnlPopup->Visible = true;
	}
	else {
		btnParam2->BevelOuter = bvRaised;
		pnlPopup->Visible = false;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::cmbOverSamplingOverWriteChange(TObject *Sender) {
	chkOS_Overwrite->Checked = true;
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::chkPowerOffClick(TObject *Sender)
{
	if (chkPowerOff->Checked == true) {
		lbl_message->Caption = L"Power Off";
	}
	else {
		lbl_message->Caption = L"";
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::chk_change_default_parameterClick(TObject *Sender)
{
	int i;
	int from_index,to_index;
	if (chk_change_default_parameter->Checked == true) {
		lbl_co->Caption = L"CDP Mode";
		from_index = cmbDefaultParameter->ItemIndex;
		to_index   = cmbDefaultParameter->ItemIndex + (UpDDefParamCount->Position - 1);
		if (from_index == to_index) {
			lbl_cdp_from_to->Caption = cmbDefaultParameter->Items->Strings[from_index];
		} else {
			if (to_index >= cmbDefaultParameter->Items->Count) {
				to_index = cmbDefaultParameter->Items->Count - 1;
				UpDDefParamCount->Position = (to_index - from_index) + 1;
			}
			lbl_cdp_from_to->Caption = "From:" + cmbDefaultParameter->Items->Strings[from_index] + " To:   " + cmbDefaultParameter->Items->Strings[to_index];
		}
	} else {
		lbl_co->Caption = L"";
		lbl_cdp_from_to->Caption = "";
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::cmbSplitChange(TObject *Sender)
{
	if (cmbSplit->ItemIndex != 0 && chk_change_default_parameter->Checked == true) {
		Application->MessageBoxW(L"CDP Mode Off", L"", MB_OK);
		chk_change_default_parameter->Checked = false;
	}
	if (cmbSplit->ItemIndex != 0) {
		cmbADDP->Enabled = false;
		chk_change_default_parameter->Enabled = false;
	} else {
		cmbADDP->Enabled = true;
		chk_change_default_parameter->Enabled = true;
	}
}

// ---------------------------------------------------------------------------


void __fastcall TForm1::list_dp_activeClick(TObject *Sender)
{
	AnsiString sStr;
	int i,n;
	btn_to_inactive->Enabled = false;
	if (list_dp_active->ItemIndex != -1) {
		sStr = list_dp_active->Items->Strings[list_dp_active->ItemIndex];
		if (sscanf(sStr.c_str(),"%d%*s",&n) == 1 && n > 1) {
			btn_to_inactive->Enabled = true;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::list_dp_inactiveClick(TObject *Sender)
{
	if (list_dp_inactive->ItemIndex != -1) {
		btn_to_active->Enabled = true;
	} else {
		btn_to_active->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_to_inactiveClick(TObject *Sender)
{
	UnicodeString uStr;
	int i,n;
	if (list_dp_active->ItemIndex != -1) {
		uStr = list_dp_active->Items->Strings[list_dp_active->ItemIndex];
		list_dp_inactive->Items->Add(uStr);
		list_dp_active->Items->Delete(list_dp_active->ItemIndex);
		list_dp_active->ItemIndex = -1;
		btn_to_active->Enabled = false;
		btn_to_inactive->Enabled = false;
		btnReload->Tag = 1;
		cmbDefaultParameter->Items->Clear();
		for (i = 0;i < list_dp_active->Items->Count;i++) {
			uStr = list_dp_active->Items->Strings[i];
			cmbDefaultParameter->Items->Add(uStr);
		}
		cmbDefaultParameter->ItemIndex = 0;
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::btn_to_activeClick(TObject *Sender)
{
	UnicodeString uStr;
	int i;
	if (list_dp_inactive->ItemIndex != -1) {
		uStr = list_dp_inactive->Items->Strings[list_dp_inactive->ItemIndex];
		list_dp_active->Items->Add(uStr);
		list_dp_inactive->Items->Delete(list_dp_inactive->ItemIndex);
		list_dp_inactive->ItemIndex = -1;
		btn_to_active->Enabled = false;
		btn_to_inactive->Enabled = false;
		btnReload->Tag = 1;
		cmbDefaultParameter->Items->Clear();
		for (i = 0;i < list_dp_active->Items->Count;i++) {
			uStr = list_dp_active->Items->Strings[i];
			cmbDefaultParameter->Items->Add(uStr);
		}
		cmbDefaultParameter->ItemIndex = 0;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnDPClick(TObject *Sender)
{
	UnicodeString uStr;
	AnsiString sStr;
	TListItem *item;
	UPI upi;
	if (LvFile->Selected != NULL) {
		item = LvFile->Selected;
		if (item->Data) {
			uStr = GetFilePath(1, (wchar_t *)item->Data);
			sStr = uStr;
			if (PLG_GetUpcTagInfo(sStr.c_str(),&upi) == STATUS_SUCCESS) {
				if (strlen(upi.version) > 0) {
					strcpy(Form6->dp_version,upi.version);
					strcpy(Form6->dp_mtime,upi.dp_mtime);
					strcpy(Form6->dp_param,upi.default_parameter);
					Form6->ShowModal();
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnPauseClick(TObject *Sender)
{
	btnPause->Tag = 1;
	btnAbort->Click();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnSortClick(TObject *Sender)
{
//	LvFile->SortType = stText;
//	LvFile->SortType = stNone;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LvFileColumnClick(TObject *Sender, TListColumn *Column)
{
	if (btnPause->Tag == 1) {
		if (Column->ImageIndex == 2) {
			Column->Tag = Column->ImageIndex;
			Column->ImageIndex = 0;
		} else if (Column->Tag == 1) {
			Column->ImageIndex = Column->Tag;
			Column->Tag = 0;
		}
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm1::btnOK_ClearClick(TObject *Sender)
{
	TListItem *item;
	if (LvFile->SelCount > 0) {
		if (LvFile->Selected != NULL) {
			item = LvFile->Selected;
			item->ImageIndex = 0;
			item->Caption    = "";
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btn_DPPackClick(TObject *Sender)
{
	TListItem *item;
	AnsiString s;
	int i,n;
	int ret;
	long time;
	long size;
	char zip_temp_name[TMP_MAX];
	list_import_list->Items->Clear();

	if (OpenDialogZip->Execute()) {
		TZipFile* zip = new TZipFile;
		UnicodeString zipTempPath;
		zip->Open(OpenDialogZip->FileName,zmRead);
		if (zip->FileCount > 0) {
			zipTempPath = TPath::GetTempPath();
			zip_temp_name[0] = '\0';
			tmpnam(zip_temp_name);
			if (strlen(zip_temp_name) > 0) {
				zipTempPath = zipTempPath + (UnicodeString)zip_temp_name;
				MkDir(zipTempPath);
				for (i = 0;i < zip->FileCount;i++) {
					s = zip->FileNames[i];
					if (sscanf(s.c_str(),"upconv_default_%02d",&n) == 1) {
						zip->Extract(zip->FileNames[i],zipTempPath);
						memoDPI->Lines->Clear();
						memoDPI->Lines->LoadFromFile(zipTempPath + L"\\" + zip->FileNames[i]);
						if (memoDPI->Lines->Count >= 6) {
							item = list_import_list->Items->Add();
							item->Caption = zip->FileNames[i];
							item->SubItems->Add(memoDPI->Lines->Strings[5]);
							item->Checked = false;
						}
					}
				}
				DeleteFolderAll(zipTempPath);
			}
		}
		zip->Close();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btn_DP_ImportClick(TObject *Sender)
{
	TListItem *item;
	AnsiString s;
	UnicodeString us,us2;
	int i,ii,n,index,count,remain;
	int ret;
	long time;
	long size;
	char zip_temp_name[TMP_MAX];
	FILE *ifp;
	index = 0;
	count = 0;
	remain = 0;
	for (i = 0;i < list_import_list->Items->Count;i++) {
		if (list_import_list->Items->Item[i]->Checked == true) {
			remain++;
		}
	}

	if (OpenDialogZip->FileName.Length() > 0) {
		TZipFile* zip = new TZipFile;
		UnicodeString zipTempPath;
		zip->Open(OpenDialogZip->FileName,zmRead);
		if (zip->FileCount > 0) {
			zipTempPath = TPath::GetTempPath();
			zip_temp_name[0] = '\0';
			tmpnam(zip_temp_name);
			if (strlen(zip_temp_name) > 0) {
				zipTempPath = zipTempPath + (UnicodeString)zip_temp_name;
				MkDir(zipTempPath);
				for (i = 0;i < list_import_list->Items->Count;i++) {
					if (list_import_list->Items->Item[i]->Checked == true) {
						for (ii = 0;ii < zip->FileCount;ii++) {
							s = zip->FileNames[ii];
							if (s == list_import_list->Items->Item[i]->Caption) {
								zip->Extract(zip->FileNames[ii],zipTempPath);
								s = (AnsiString)(zipTempPath + L"\\" + zip->FileNames[ii]);
								ifp = fopen(s.c_str(),"r");
								if (ifp != NULL) {
									fclose(ifp);
									do {
										GetDefaultParameterNumberEmptySlot(&index,&count);
									} while (index < 100 && count == 0);
									if (index > 99) {
										i = list_import_list->Items->Count;
										break;
									} else if (count > 0) {
										us.sprintf(L"%s\\upconv_default_%02d.txt",defaultParameterPath,index);
										us2 = (zipTempPath + L"\\" + zip->FileNames[ii]);
										MoveFileEx(us2.c_str(),us.c_str(),MOVEFILE_COPY_ALLOWED);
										ifp = fopen(((AnsiString)us).c_str(),"r");
										if (ifp != NULL) {
											fclose(ifp);
											remain--;
										}
									}
								}
							}
						}
					}
				}
				DeleteFolderAll(zipTempPath);
			}
		}
		zip->Close();
	}
	if (remain == 0) {
		list_import_list->Items->Clear();
		btnReloadClick(Sender);
		Application->MessageBoxW(L"Import OK",L"",MB_OK);
	} else {
		Application->MessageBoxW(L"Error:Import Failed.",L"",MB_OK);
	}
}

//---------------------------------------------------------------------------
void __fastcall TForm1::DeleteFolderAll(UnicodeString folder_name)
{
	int pathSize;
	wchar_t *path;
	bool result;
	if (folder_name.Length() > 0) {
		folder_name = ExcludeTrailingPathDelimiter(folder_name);

		pathSize = folder_name.Length() + 2;
		path = new wchar_t[pathSize];
		if (path != NULL) {
			wcscpy(path,folder_name.c_str());
			path[pathSize-1] = 0;

			SHFILEOPSTRUCT fs;
			ZeroMemory(&fs,sizeof(SHFILEOPSTRUCT));
			fs.hwnd = Application->Handle;
			fs.wFunc = FO_DELETE;
			fs.pFrom = path;
			fs.pTo = NULL;
			fs.fFlags=FOF_SIMPLEPROGRESS|FOF_NOCONFIRMATION;
			result = (SHFileOperation(&fs) == 0);
			delete[] path;

		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::list_import_listChange(TObject *Sender, TListItem *Item, TItemChange Change)

{
	int i,checked;

	checked = false;
	if (list_import_list->Items->Count > 0) {
		for (i = 0;i < list_import_list->Items->Count;i++) {
			if (list_import_list->Items->Item[i]->Checked) {
				checked = true;
				break;
			}
		}
	}
	if (checked == true) {
		btn_DP_Import->Enabled = true;
	} else {
		btn_DP_Import->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::GetDefaultParameterNumberEmptySlot(int *number,int *count)
{
	UnicodeString uFileName;
	AnsiString sFileName;
	FILE *ifp;
	int i,cnt;
	*count = 0;
	cnt = 0;
	for (i = *number + 1;i <= 99;i++) {
		uFileName.printf(L"%s\\upconv_default_%02d.txt",defaultParameterPath,i);
		sFileName = uFileName;
		ifp = fopen(sFileName.c_str(),"r");
		if (ifp == NULL) {
			break;
		} else {
			fclose(ifp);
		}
	}
	for (;i <= 99;i++) {
		uFileName.printf(L"%s\\upconv_default_%02d.txt",defaultParameterPath,i);
		sFileName = uFileName;
		ifp = fopen(sFileName.c_str(),"r");
		if (ifp == NULL) {
			cnt++;
		} else {
			fclose(ifp);
			break;
		}
	}
	if (cnt == 0) {
		*number = 100;
		*count  = 0;
	} else {
		*number = i - cnt;
		*count = cnt;
	}
}

void __fastcall TForm1::cmbADDPChange(TObject *Sender)
{
	if (cmbADDP->ItemIndex != 0) {
		cmbSplit->Enabled = false;
	} else {
		cmbSplit->Enabled = true;
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::edtDefParamCountChange(TObject *Sender)
{
	int from_index,to_index;

	from_index = cmbDefaultParameter->ItemIndex;
	to_index   = cmbDefaultParameter->ItemIndex + (UpDDefParamCount->Position - 1);
	if (from_index == to_index) {
		lbl_cdp_from_to->Caption = cmbDefaultParameter->Items->Strings[from_index];
	} else {
		if (to_index >= cmbDefaultParameter->Items->Count) {
			to_index = cmbDefaultParameter->Items->Count - 1;
			UpDDefParamCount->Position = (to_index - from_index);
		}
		lbl_cdp_from_to->Caption = "From:" + cmbDefaultParameter->Items->Strings[from_index] + " To:   " + cmbDefaultParameter->Items->Strings[to_index];
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::chkFIO_ExClick(TObject *Sender)
{
	if (chkFIO_Ex->Checked == true) {
		cmbFileIOMax->Enabled = true;
	} else {
		cmbFileIOMax->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel2_2Expand(TObject *Sender)
{
	CategoryPanel2_1->Collapsed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel2_1Expand(TObject *Sender)
{
	CategoryPanel2_2->Collapsed = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CategoryPanel1_1Expand(TObject *Sender)
{
	CategoryPanel1_2->Collapsed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel1_2Expand(TObject *Sender)
{
	CategoryPanel1_1->Collapsed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::chk_output_mid_sideClick(TObject *Sender)
{
	if (chk_output_mid_side->Checked == true) {
		chk_output_Left_Right_Center->Checked = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::chk_output_Left_Right_CenterClick(TObject *Sender)
{
	if (chk_output_Left_Right_Center->Checked == true) {
		 chk_output_mid_side->Checked = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edt_ext_editChange(TObject *Sender)
{
	if (edt_ext_edit->Text.Length() > 0) {
		btn_ext_add->Enabled = true;
	} else {
		btn_ext_add->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_ext_addClick(TObject *Sender)
{
	int i;
	UnicodeString u;

	u = edt_ext_edit->Text.LowerCase();
	u = u.Trim();

	if (u.Length() > 0) {
		for (i = 0;i < clist_extension->Items->Count;i++) {
			if (clist_extension->Items->Strings[i] == u) break;
		}
		if (i == clist_extension->Items->Count) {
			clist_extension->Items->Add(u);
		}
	}
	edt_ext_edit->Text = L"";
	UpdateSupportExt();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::clist_extensionClickCheck(TObject *Sender)
{
	int i,checked;
	checked = 0;
	for (i = 0;i < clist_extension->Items->Count;i++) {
		if (clist_extension->Checked[i] == true) {
			checked = 1;
			break;
		}
	}


	if (checked == 1) {
		btn_ext_del->Enabled = true;
	} else {
		btn_ext_del->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_ext_delClick(TObject *Sender)
{
	int i;
	for (i = clist_extension->Items->Count - 1;i > 5;i--) {
		if (clist_extension->Checked[i] == true) {
			clist_extension->Items->Delete(i);
		}
	}
	UpdateSupportExt();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_level_scanClick(TObject *Sender)
{
	int i;

	for (i = 0;i < LvFile->Items->Count;i++) {
		LvFile->Items->Item[i]->SubItems->Strings[3] = L"";
	}

	btn_level_scan->Tag = 1;
    btnStart->OnClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edt_volume_levelChange(TObject *Sender)
{
	if (cmbNormalize->ItemIndex == 2 && edt_volume_level->Text.Length() > 0) {
		if (edt_volume_level->Text.ToInt() <= 0 || edt_volume_level->Text.ToInt() > 100) {
			Application->MessageBoxW(L"Volume Level(1 - 100",L"");
			edt_volume_level->Text = "25";
		}
	} else if (cmbNormalize->ItemIndex == 3 && edt_volume_level->Text.Length() > 0) {
		if (edt_volume_level->Text.ToInt() <= 0 || edt_volume_level->Text.ToInt() > 120) {
			Application->MessageBoxW(L"Volume Level(1 - 120",L"");
			edt_volume_level->Text = "100";
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbNormalizeChange(TObject *Sender)
{
	int i;
	UnicodeString uStr;
	if (cmbNormalize->ItemIndex < 2) {
		cmb_volume_level->Enabled = false;
	} else if (cmbNormalize->ItemIndex == 2) {
		cmb_volume_level->Items->Clear();
		for (i = 0;i < 91;i++) {
			uStr.sprintf(L"%0d",i + 10);
			cmb_volume_level->Items->Add(uStr);
		}
		cmb_volume_level->ItemIndex = 10;
		cmb_volume_level->Enabled = true;
	} else if (cmbNormalize->ItemIndex == 3) {
		cmb_volume_level->Items->Clear();
		for (i = 0;i < 71;i++) {
			uStr.sprintf(L"%0d",i + 50);
			cmb_volume_level->Items->Add(uStr);
		}
		cmb_volume_level->ItemIndex = 50;
		cmb_volume_level->Enabled = true;

	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmb_volume_levelChange(TObject *Sender)
{
	edt_volume_level->Text = cmb_volume_level->Text;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbSampRateChange(TObject *Sender)
{
	UpdateFileSize();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cmbBitWidthChange(TObject *Sender)
{
	UpdateFileSize();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ReloadEncoderInfo()
{
	UnicodeString uFileName;
	AnsiString sFileName;
	AnsiString sNumber;
	AnsiString ext;

	char work[512];
	char *e_p1,*e_p2,*e_p3,*e_p4;
	FILE *fp = NULL;
	int i,ii,cnt;
	e_p1 = e_p2 = e_p3 = e_p4;
	cnt = 0;

	while (cmbEncoder->Items->Count > 6) {
		cmbEncoder->Items->Delete(6);
	}

	uFileName.printf(L"%s\\upconv_encoder.txt",defaultParameterPath);
	sFileName = (AnsiString)uFileName;
	fp = fopen(sFileName.c_str(),"rt");

	// カスタムのエンコードのパラメータの個数を調べコンボボックスにセットする
	if (fp != NULL) {
		for (i = 0;i < 100;i++) {
			work[0] = '\0';
			if (fgets(work,500,fp) != NULL) {
				if (strlen(work) > 0) {
					e_p1 = work;
					e_p2 = strchr(e_p1,'|');if (e_p2 != NULL) {*e_p2 = '\0';e_p2++;}
					if (e_p2 != NULL) {
						e_p3 = strchr(e_p2,'|');if (e_p3 != NULL) {*e_p3 = '\0';e_p3++;}
					}
					if (e_p3 != NULL) {
						e_p4 = strchr(e_p3,'|');if (e_p4 != NULL) {*e_p4 = '\0';e_p4++;}
					}
					if (e_p1 != NULL && e_p2 != NULL && e_p3 != NULL && e_p4 != NULL) {
						if (strlen(e_p1) > 0 && strlen(e_p2) > 0 && strlen(e_p3) > 0 && strlen(e_p4) > 0) {
							cmbEncoder->Items->Add((UnicodeString)e_p1);
							memoEncoderInfo->Lines->Add((UnicodeString)e_p4);
							memoEncoderExt->Lines->Add((UnicodeString)e_p3);
						}
					}
				}
			} else {
				break;
			}
		}
		fclose(fp);
	}
}



void __fastcall TForm1::cmbEncoderChange(TObject *Sender)
{
	edtEncoderInfo->Text =  memoEncoderInfo->Lines->Strings[cmbEncoder->ItemIndex];
}
//---------------------------------------------------------------------------


void __fastcall TForm1::fileHFCClick(TObject *Sender)
{
	int i;
	long hfc;
	UnicodeString uStr;
	UnicodeString uDefault = "";
	AnsiString sStr;
	if (btnStart->Visible == true) {
		if (LvFile->SelCount > 0) {
			if (LvFile->SelCount == 1) {
				uDefault = LvFile->Selected->SubItems->Strings[4];
			}
			uStr = InputBox(L"Upconvfe",L"File HFC",uDefault);
			for (i = 0;i < LvFile->Items->Count;i++) {
				if (LvFile->Items->Item[i]->Selected) {
					if (uStr != L"") {
						sStr = uStr;
						if (sscanf(sStr.c_str(),"%ld",&hfc) == 1 && hfc >= 4000) {
							LvFile->Items->Item[i]->SubItems->Strings[4] = uStr;
						}
					} else {
						LvFile->Items->Item[i]->SubItems->Strings[4] = L"";
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::estConversionOffset1Click(TObject *Sender)
{
	int i;
	int hh,mm,ss;
	long offset,nsec = -1,duration;
	char ts[20];
	UnicodeString uStr;
	UnicodeString uDefault = "0";
	AnsiString sStr;
	if (btnStart->Visible == true) {
		if (LvFile->SelCount > 0) {
			for (i = 0;i < LvFile->Items->Count;i++) {
				if (LvFile->Items->Item[i]->Selected) {
					sStr = LvFile->Items->Item[i]->SubItems->Strings[1];
					if (sscanf(sStr.c_str(),"%d:%d:%d",&hh,&mm,&ss) == 3) {
						duration = (hh * 60 * 60) + (mm * 60) + ss;
						if (nsec == -1 || nsec > duration) {
							nsec = duration;
						}
					}
				}
			}
			if (LvFile->SelCount == 1) {
				uDefault = LvFile->Selected->SubItems->Strings[5];
			}
			uStr = InputBox(L"Upconvfe",L"Test Conversion Offset",uDefault);
			for (i = 0;i < LvFile->Items->Count;i++) {
				if (LvFile->Items->Item[i]->Selected) {
					if (uStr != L"") {
						sStr = uStr;
						offset = -1;
						if (sscanf(sStr.c_str(),"%d:%d:%d",&hh,&mm,&ss) == 3) {
							// 時：分:秒の入力
							offset = (hh * 60 * 60) + (mm * 60) + ss;
						} else {
							if (sscanf(sStr.c_str(),"%d:%d",&mm,&ss) == 2) {
								// 分:秒の入力
								offset = (mm * 60) + ss;
							} else {
								if (sscanf(sStr.c_str(),"%d",&ss) == 1) {
									// 秒の入力
									offset = ss;
								}
							}
						}
						if (offset >= 0 && offset + 5 <= nsec) {
							hh = offset / 3600;
							mm = (offset / 60) % 60;
							ss = (offset % 60);
							if (offset < 3600) {
								sprintf(ts,"%02d:%02d",mm,ss);
							} else {
								sprintf(ts,"%d:%02d:%02d",hh,mm,ss);
							}
							LvFile->Items->Item[i]->SubItems->Strings[5] = (UnicodeString)ts;
						}
					} else {
						LvFile->Items->Item[i]->SubItems->Strings[5] = L"";
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CopyCommandLine1Click(TObject *Sender)
{
	int i;
	int delete_failed = 0;
	AnsiString command_line;
	if (LvFile->SelCount > 0) {
		for (i = 0;i < LvFile->Items->Count;i++) {
			if (LvFile->Items->Item[i]->Selected) {
				command_line = 	MakeCommandParam(i,0,0,&delete_failed);
				Form7->Memo1->Lines->Clear();
				Form7->Memo1->Lines->Add(command_line);
				Form7->ShowModal();
			}
		}
	}
}
//---------------------------------------------------------------------------

