// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <StrUtils.hpp>
#include "Unit1.h"
#include "Unit5.h"
#include "Unit6.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "perfgrap"
#pragma resource "*.dfm"
TForm1 *Form1;
char g_log_filename[MAX_PATH];
char g_log_enable;


#define	PRINT_LOG(s)			do {																		\
								if (g_log_enable) {                                                         \
									FILE *log_fn;															\
									log_fn = fopen(g_log_filename,"a");										\
									if (log_fn) {															\
										fprintf(log_fn,"%s [%d] %s\n",__FUNCTION__,__LINE__,s);				\
										fclose(log_fn);														\
									}																		\
								}																			\
								} while (0)


#define	PRINT_FN_LOG(pn,s)	do {																			\
								if (g_log_enable) {                                                         \
									FILE *log_fn;															\
									log_fn = fopen(g_log_filename,"a");										\
									if (log_fn) {															\
										fprintf(log_fn,"[%05d]%s:%s\n",__LINE__,pn,s);						\
										fclose(log_fn);														\
									}																		\
								}                                                                           \
							} while (0)



// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnExitClick(TObject *Sender) {
	Close();
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender) {
	FILE *fp;
	TIniFile *iniFile;
	int def_param;
	int i,n;
	int vla;
	char exe_path[MAX_PATH];
	char *p;
	char buffer[4096];
	wchar_t file_path[2048];
	int samp_rate,bit_width;
	int sec_index;
	int libsndfile;
	int cpu_pri;
	int thread;
	int fileio;
	int fio_sec_mb,fio_sec_size,fio_mb_size;

	int log_enable;
	int	window_size;
	int addp;
	int norm;

	SYSTEM_INFO sysinfo;
	UnicodeString uFileName;
	UnicodeString cmdName;
	UnicodeString playDeviceString;
	UnicodeString uStr;
	AnsiString log_path;
	AnsiString tmpDrive;
	AnsiString exe_path2;

	libsndfile = 0;
	SelectFileIndex = 0;
	preview_input_filename = "";
	preview_input_filename_state_change = 0;


	strcpy(g_log_filename,"D:\\upconv_eq_gui.log");
	g_log_enable = 0;

	eqParameterPath = "C:\\ProgramData\\upconv_eq";
	uFileName = eqParameterPath + "\\upconv_eq.ini";
	iniFile = new TIniFile(uFileName);

	log_enable = iniFile->ReadInteger("Option", "Log", 0);
	if (log_enable) {
		rdo_log_enable->Checked = true;
	}
	log_path = iniFile->ReadString("Option", "LogPath", "C:\\ProgramData\\upconv_eq");
	be_log->Text = log_path;
	if (log_enable) {
		g_log_enable = 1;
		strcpy(g_log_filename,log_path.c_str());
		strcat(g_log_filename,"\\upconv_eq_gui.log");
	}
	exe_path2 = ExtractFilePath(Application->ExeName);
	strcpy(exe_path,exe_path2.c_str());
	p = strrchr(exe_path,'\\');
	if (p != NULL) {
		*p = '\0';
		p = strrchr(exe_path,'\\');
		if (p != NULL) {
			*p = '\0';
			if (strcmp(exe_path,"C:\\Program Files (x86)") == 0 || strcmp(exe_path,"C:\\Program Files") == 0) {
				rdo_log_disable->Enabled = false;
				rdo_log_enable->Enabled  = false;
				be_log->Enabled = false;
			}
		}
	}
	if (rdo_log_enable->Enabled == false) {
		g_log_enable = 0;
	}

	PRINT_FN_LOG("FormCreate","Start");
	GetNativeSystemInfo(&sysinfo);
	if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
		Form1->Caption = Form1->Caption + L" (x64)";
	}

	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"flac.exe";
	if (PathFileExists(cmdName.c_str())) {
		PRINT_LOG("flac.exe");
		OpenDialog->Filter = OpenDialog->Filter + L"|FLAC|*.FLAC";
		cmbEncoder->Items->Add(L"FLAC");
	}
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"wvunpack.exe";
	if (PathFileExists(cmdName.c_str())) {
		PRINT_LOG("wvunpack.exe");
		OpenDialog->Filter = OpenDialog->Filter + L"|WV|*.WV";
		// cmbEncoder->Items->Add(L"WavPack");
	}
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"wavpack.exe";
	if (PathFileExists(cmdName.c_str())) {
		PRINT_LOG("wavpack.exe");
		cmbEncoder->Items->Add(L"WavPack");
	}
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"lame.exe";
	if (PathFileExists(cmdName.c_str())) {
		PRINT_LOG("lame.exe");
		OpenDialog->Filter = OpenDialog->Filter + L"|MP3|*.MP3";
		cmbEncoder->Items->Add(L"MP3");
	}
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"opusdec.exe";
	if (PathFileExists(cmdName.c_str())) {
		PRINT_LOG("opusdec.exe");
		OpenDialog->Filter = OpenDialog->Filter + L"|OPUS|*.OPUS";
	}
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		PRINT_LOG("ffmpeg.exe");
		OpenDialog->Filter = OpenDialog->Filter + L"|WMA|*.WMA";
	}
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"ffmpeg.exe";
	if (PathFileExists(cmdName.c_str())) {
		PRINT_LOG("ffmpeg.exe");
		OpenDialog->Filter = OpenDialog->Filter + L"|MP4|*.MP4";
	}

	OpenDialog->Filter = OpenDialog->Filter + L"|DSF|*.DSF";

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
		PRINT_LOG("sndfile-convert.exe");
		cmbOutputFormat->Items->Add("W64");
		cmbOutputFormat->Tag = 1;
	}

	orgLvFileWinProc = LvFile->WindowProc;
	LvFile->WindowProc = LvFileWinProc;
	DragAcceptFiles(LvFile->Handle, true);
	orgLvAnaFileWinProc = LvAnaFile->WindowProc;
	LvAnaFile->WindowProc = LvAnaFileWinProc;
	DragAcceptFiles(LvFile->Handle, true);
	DragAcceptFiles(LvAnaFile->Handle, true);

	SHGetSpecialFolderPath(Form1->Handle, file_path,CSIDL_COMMON_APPDATA, false);
	eqParameterPath = file_path;
	eqParameterPath += L"\\upconv_eq";

	window_size = iniFile->ReadInteger(L"DefaultParam", L"WindowSize", 0);
	if (window_size == 0) {
		rdo_window_size1->Checked = true;
	} else if (window_size == 1) {
		rdo_window_size2->Checked = true;
	} else if (window_size == 2) {
		rdo_window_size3->Checked = true;
	}
	def_param = iniFile->ReadInteger(L"DefaultParam", L"PresetEQ", 0);

	edtOutputPath->Text = iniFile->ReadString("Output", "Folder", "");
	lbl_select_directory->Caption = iniFile->ReadString("Output", "FolderHistory", "");
	edtSuffix->Text = iniFile->ReadString("Output", "Suffix", "");
	//edtPreviewApp->Text = iniFile->ReadString("Preview", "AppName", "");
	playDeviceString = iniFile->ReadString("Preview", "DeviceNameString", "");
	samp_rate = iniFile->ReadInteger("Preview", "SampRate",1);

	norm = iniFile->ReadInteger(L"Output", L"Normalize", 0);
	if (norm == 0) {
		cmbNormalize->ItemIndex = 0;
		cmb_volume_level->Enabled = false;
	} else if (norm == 1) {
		cmbNormalize->ItemIndex = 1;
		cmb_volume_level->Enabled = false;
	} else if (norm == 2) {
		cmbNormalize->ItemIndex = 2;
		cmb_volume_level->Enabled = true;
	}

	//	vla = iniFile->ReadInteger("Output", "VLA",5);
	vla = iniFile->ReadInteger("Output", "VLAInt",100);
	if (vla >= 50 && vla <= 120) {
		int ii;
		for (ii = 0;ii < cmb_volume_level->Items->Count;ii++) {
			if (cmb_volume_level->Items->Strings[ii] == vla) {
				cmb_volume_level->ItemIndex = ii;
				break;
			}
		}
	}

	if (samp_rate == 1) {
//		rdo_preview_rate_none->Checked = true;
	} else if (samp_rate == 2) {
		rdo_preview_rate_44100->Checked = true;
		BeforeRateConv = 44100;
	} else if (samp_rate == 3) {
		rdo_preview_rate_48000->Checked = true;
		BeforeRateConv = 48000;
	} else if (samp_rate == 4) {
		rdo_preview_rate_96000->Checked = true;
		BeforeRateConv = 96000;
	}
	bit_width = iniFile->ReadInteger("Preview", "BitWidth",1);
	//if (bit_width == 1) {
	//	rdo_preview_bit_16->Checked = true;
	//} else if (bit_width == 2) {
	//	rdo_preview_bit_24->Checked = true;
	//}
	sec_index = iniFile->ReadInteger("Preview", "Length",0);
	if (sec_index >= 0 && sec_index < 5) {
		cmb_preview_length->ItemIndex = sec_index;
	}
	thread = iniFile->ReadInteger("Exec", "Thread", 1);
	fileio = iniFile->ReadInteger("Exec", "file_io", 5);
	cpu_pri = iniFile->ReadInteger("Exec", "cpu_pri", 0);
	addp   = iniFile->ReadInteger("Exec", "addp", 0);
	fio_sec_mb  = iniFile->ReadInteger("Exec", "file_io_sec_mb",1);
	fio_sec_size = iniFile->ReadInteger("Exec", "file_io_sec_size",10);
	fio_mb_size  = iniFile->ReadInteger("Exec", "file_io_mb_size",5);

	if (thread >= 1 && thread <= 24) {
		cmbThread->ItemIndex = thread -1;
	}
	if (fileio >= 0 && fileio < 24) {
		cmbFileIO->ItemIndex = fileio;
	}
	if (cpu_pri >= 0 && cpu_pri <= 2) {
		cmbPriority->ItemIndex = cpu_pri;
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

	if (addp == 0 || addp == 2 || addp == 4 || addp == 6 || addp == 8) {
		if (addp == 0) cmbADDP->ItemIndex = 0;
		if (addp == 2) cmbADDP->ItemIndex = 1;
		if (addp == 4) cmbADDP->ItemIndex = 2;
		if (addp == 6) cmbADDP->ItemIndex = 3;
		if (addp == 8) cmbADDP->ItemIndex = 4;
	}

	if (edtOutputPath->Text.Length() > 0)
		edtOutputPath->RightButton->Visible = true;
	if (edtSuffix->Text.Length() > 0)
		edtSuffix->RightButton->Visible = true;
	if (edtOutputPath->Text.Length() == 0)
		edtOutputPath->Text = L"Source Directory";
	if (edtSuffix->Text.Length() == 0)
		edtSuffix->Text = L"upconv_eq";

	LoadPresetParameter();
	ReloadDefaultParameter(def_param + 1, def_param + 1);

	cmbEQParameter->ItemIndex = def_param;
	btn_Load->Tag = 0;
	cdp_num_max = 0;

	for (i = 0;i < 15;i++) {
		memset(&SoundBuffer[i],0,sizeof (SOUND_BUFFER));
		SoundBuffer[i].status = BUFFER_FREE;
	}
	CountSoundBuffer = 0;
	IndexSoundBuffer = 0;
	AnalyzePhase = 1;
	for (i = 0; i < 1; i++)
		threadExec[i] = NULL;

	InitSound();
	delete iniFile;

	if (cmbEQParameter->Items->Count > 0) {
		cmbEQ_Analyze->ItemIndex = cmbEQParameter->ItemIndex;
	}
	if (cmbPlayDevice->Items->Count > 0 && playDeviceString.Length() > 0) {
		cmbPlayDevice->ItemIndex = 0;
		for (i = 0;i < cmbPlayDevice->Items->Count;i++) {
			if (cmbPlayDevice->Items->Strings[i] == playDeviceString) {
				cmbPlayDevice->ItemIndex = i;
				cmbPlayDevice->OnChange(Sender);
				break;
			}
		}

	}
	if (cmbEQ_Analyze->ItemIndex > 0) {
		rdo_Analyze_Phase1->Enabled = true;
	} else {
		rdo_Analyze_Phase1->Enabled = false;
	}
	PRINT_FN_LOG("FormCreate","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose) {
	TIniFile *iniFile;
	AnsiString sStr;
	long temp;
	int i,n;
	int samp_rate,bit_width;
	int sec_index;
	int cpu_pri;
	int thread;
	int fileio;
	int addp;

	UnicodeString uFileName;

	if (Abort || Timer1->Enabled == true) {
		CanClose = false;
		return;
	}
	PRINT_FN_LOG("FormCloseQuery","Start");

	uFileName = eqParameterPath + L"\\upconv_eq.ini";
	iniFile = new TIniFile(uFileName);
	sStr = cmbEQParameter->Items->Strings[cmbEQParameter->ItemIndex];
	if (sscanf(sStr.c_str(),"%d%*s",&n) == 1) {
		iniFile->WriteInteger(L"Convert", "DefaultParam",n - 1);
	}

	if (edtOutputPath->Text == L"Source Directory")
		edtOutputPath->Text = L"";
	iniFile->WriteString("Output", "Folder", edtOutputPath->Text);
	iniFile->WriteString("Output", "FolderHistory",lbl_select_directory->Caption);
	iniFile->WriteString("Output", "Suffix", edtSuffix->Text);
	if (cmb_volume_level->Text.Length() > 0) {
		iniFile->WriteString("Output", "VLAInt",cmb_volume_level->Text);
	} else {
		iniFile->WriteString("Output", "VLAInt", "");
	}
	iniFile->WriteInteger(L"Output", L"Normalize", cmbNormalize->ItemIndex);

	iniFile->WriteInteger(L"Exec", "file_io", cmbFileIO->ItemIndex);
	iniFile->WriteInteger(L"Exec", "thread", cmbThread->ItemIndex + 1);
	iniFile->WriteInteger(L"Exec", "cpu_pri", cmbPriority->ItemIndex);

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

	if (cmbADDP->ItemIndex == 0) {
		iniFile->WriteInteger(L"Exec", "addp",0);
	} else if (cmbADDP->ItemIndex == 1) {
		iniFile->WriteInteger(L"Exec", "addp",2);
	} else if (cmbADDP->ItemIndex == 2) {
		iniFile->WriteInteger(L"Exec", "addp",4);
	} else if (cmbADDP->ItemIndex == 3) {
		iniFile->WriteInteger(L"Exec", "addp",6);
	} else if (cmbADDP->ItemIndex == 4) {
		iniFile->WriteInteger(L"Exec", "addp",8);
	}
	if (rdo_window_size1->Checked) {
		iniFile->WriteInteger(L"DefaultParam", L"WindowSize", 0);
	} else if (rdo_window_size2->Checked) {
		iniFile->WriteInteger(L"DefaultParam", L"WindowSize", 1);
	} else if (rdo_window_size3->Checked) {
		iniFile->WriteInteger(L"DefaultParam", L"WindowSize", 2);
	}

	//iniFile->WriteString("Preview","AppName",edtPreviewApp->Text);

	samp_rate = 1;
//	if (rdo_preview_rate_none->Checked) {
//		// Default
//		samp_rate = 1;
//	}
	if (rdo_preview_rate_44100->Checked) {
		samp_rate = 2;
	}
	if (rdo_preview_rate_48000->Checked) {
		samp_rate = 3;
	}
	if (rdo_preview_rate_96000->Checked) {
		samp_rate = 4;
	}

	iniFile->WriteString("Preview", "SampRate",samp_rate);
	bit_width = 1;
//	if (rdo_preview_bit_16->Checked) {
//		// Default
//		bit_width = 1;
//	}
//	if (rdo_preview_bit_24->Checked) {
//		bit_width = 2;
//	}
	iniFile->WriteString("Preview", "BitWidth",bit_width);

	iniFile->WriteString("Preview", "Length",cmb_preview_length->ItemIndex);
	iniFile->WriteString("Preview", "DeviceNameString", cmbPlayDevice->Items->Strings[cmbPlayDevice->ItemIndex]);

	if (rdo_log_enable->Checked == true) {
		iniFile->WriteInteger(L"Option","Log",1);
	} else {
		iniFile->WriteInteger(L"Option","Log",0);
	}
	iniFile->WriteString("Option", "LogPath",be_log->Text);

	delete iniFile;
	PRINT_FN_LOG("FormCloseQuery","End");

}
// ---------------------------------------------------------------------------
void __fastcall TForm1::LoadPresetParameter(void)
{
	FILE *fp;
	UnicodeString uFileName;
	AnsiString sFileName;
	AnsiString sStr;
	char str[4096];
	char param[6192];
	char title[4096];
	char tmp_str[100];
	char *p;
	int error;
	int i,ii;
	int temp_val;
	int last_number;
	int active_page;
	int flag;
	int flag_384k;
	double temp_double;

	PRINT_FN_LOG("LoadPresetParameter","Start");

	error = 0;
	active_page = 0;

	cmbEQParameter->Items->Clear();
	cmbEQ_Analyze->Items->Clear();
	edtParameter->Lines->Clear();

	for (i = 0;i < 1000;i++) {
		eq_val[i] = NULL;
	}

	for (i = 0;i + 1 < 1000;i++) {
		title[0] = '\0';
		uFileName.printf(L"%s\\upconv_eq_%03d.txt",eqParameterPath, i + 1);
		sFileName = (AnsiString)uFileName;
		fp = fopen(sFileName.c_str(), "r");

		error = 1;
		flag_384k = 0;
		flag = 0;
		if (fp) {
			eq_val[i] = (double *)malloc(400000 * sizeof (double));
			if (eq_val[i] == NULL) break;

			for (ii = 0;ii < 400000;ii++) {
				eq_val[i][ii] = 1.0;
			}

			str[0]   = '\0';
			param[0] = '\0';
			for (ii = 0;ii < 192000;ii++) {
				if (fscanf(fp,"%lf",&eq_val[i][ii]) == 0) break;
			}
			if (fscanf(fp,"%lf",&eq_val[i][ii]) == 1) {
				ii++;
				for (;ii < 384000;ii++) {
					if (fscanf(fp,"%lf",&eq_val[i][ii]) == 0) break;
				}
				fgets(str, 4000, fp);
				fgets(str, 4000, fp);
			} else {
				eq_val[i][ii] = 1.0;
			}
			// 空
			flag |= 0x01;
			if (ii == 192000 || ii == 384000) {
				if (ii == 384000) flag_384k = 1;

				if (fgets(str, 4000, fp) != NULL) {
					p = strrchr(str,'\n');if (p != NULL) *p = '\0';
					if (strlen(str) > 0) {
						if (sscanf(str,"ActiveEQ:%d",&temp_val) == 1) {
							active_page = temp_val;
							sprintf(tmp_str,"ActiveEQ:%d",active_page);
							strcat(param,tmp_str);
							// Active Page
							flag |= 0x02;
						}
					}
				}
				if (fgets(str, 4000, fp) != NULL) {
					p = strrchr(str,'\n');if (p != NULL) *p = '\0';
					if (strlen(str) > 0) {
						// EQ(Type1)
						strcat(param,"|");
						strcat(param,str);
						flag |= 0x04;
					}
				}
				if (fgets(str, 4000, fp) != NULL) {
					p = strrchr(str,'\n');if (p != NULL) *p = '\0';
					if (strlen(str) > 0) {
						// EQ(Type2)
						strcat(param,"|");
						strcat(param,str);
						flag |= 0x08;
					}
				}
				if (fgets(str, 4000, fp) != NULL) {
					p = strrchr(str,'\n');if (p != NULL) *p = '\0';
					if (strlen(str) > 0) {
						// EQ(Type3)
						strcat(param,"|");
						strcat(param,str);
						flag |= 0x10;
					}
				}
				if (flag_384k) {
					if (fgets(str, 4000, fp) != NULL) {
						p = strrchr(str,'\n');if (p != NULL) *p = '\0';
						if (strlen(str) > 0) {
							// EQ(Type4)
							strcat(param,"|");
							strcat(param,str);
							flag |= 0x20;
						}
					}
				} else {
					// EQ(Type4)
					strcat(param,"|");
					strcat(param,"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1");
					flag |= 0x20;
				}
				if (fgets(str, 4000, fp) != NULL) {
					p = strrchr(str,'\n');if (p != NULL) *p = '\0';
					if (strlen(str) > 0) {
						// Title
						strcat(param,"|");
						strcat(param,str);
						strcpy(title,str);
					}
				}
			}
			if (flag == 0x1F || flag == 0x3F) {
				error = 0;
			}
			fclose(fp);
		} else {
			break;//
		}
		if (error == 0) {
			sStr.printf("%03d.%s",i+1,title);
			cmbEQParameter->Items->Add(sStr);
			cmbEQ_Analyze->Items->Add(sStr);
			edtParameter->Lines->Add(param);	// EQ 値を1行にした値を保持する
		}
	}
	if (i + 1 < 1000) {
		sStr.printf("%03d.%s",i+1,"EQ.Sample");
		cmbEQParameter->Items->Add(sStr);
		edtParameter->Lines->Add(L"");
	}
	PRINT_FN_LOG("LoadPresetParameter","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::ReloadDefaultParameter(int index, int use_index)
{
	FILE *fp;
	UnicodeString uStr, uTitle;
	AnsiString sStr;
	AnsiString eqParam1,eqParam2,eqParam3,eqParam4;
	char str[4096];
	char *p,*pp;
	int error;
	int d1;
	int i,ii;
	int flag;
	int active_page;
	int temp_val;
	int value_type;
	double eq;
	int eq_val[98];

	PRINT_FN_LOG("ReloadDefaultParameter","Start");

	error = 0;
	active_page = 0;
	flag = 0;
	if (index == use_index && use_index > 0) {
		sStr = edtParameter->Lines->Strings[use_index - 1];
		if (sStr.Length() > 0) {
			strcpy(str,sStr.c_str());
			p = str;
			pp = strchr(p,'|');if (pp != NULL) *pp = '\0';
			if (p != NULL) {
				if (sscanf(p,"ActiveEQ:%d",&temp_val) == 1) {
					if (temp_val == 1) {
						PageEQ->ActivePage = EQ_Type1;
					} else if (temp_val == 2) {
						PageEQ->ActivePage = EQ_Type2;
					} else if (temp_val == 3) {
						PageEQ->ActivePage = EQ_Type3;
					} else if (temp_val == 4) {
						PageEQ->ActivePage = EQ_Type4;
					}
					active_page = temp_val;
					// Active Page
					flag |= 0x01;
					p = pp + 1;
				}
			}
			if (p != NULL) {
				pp = strchr(p,'|');if (pp != NULL) *pp = '\0';
				// EQ(Type1)
				eqParam1 = (AnsiString)p;
				flag |= 0x02;
				p = pp + 1;
			}
			if (p != NULL) {
				pp = strchr(p,'|');if (pp != NULL) *pp = '\0';
				// EQ(Type2)
				eqParam2 = (AnsiString)p;
				flag |= 0x04;
				p = pp + 1;
			}
			if (p != NULL) {
				pp = strchr(p,'|');if (pp != NULL) *pp = '\0';
				// EQ(Type3)
				eqParam3 = (AnsiString)p;
				flag |= 0x08;
				p = pp + 1;
			}
			if (p != NULL) {
				pp = strchr(p,'|');if (pp != NULL) *pp = '\0';
				// EQ(Type4)
				eqParam4 = (AnsiString)p;
				flag |= 0x10;
				p = pp + 1;
			}
			if (p != NULL) {
				pp = strrchr(p,'\n');if (pp != NULL) *pp = '\0';
				// Title
				edt_eq_title->Text = (UnicodeString)p;
			}
			if (flag == 0x1F && index == use_index) {
				error = 1;
				// EQ(Type1)
				if (sscanf(eqParam1.c_str(),"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
						&eq_val[0],&eq_val[1],&eq_val[2],&eq_val[3],&eq_val[4],&eq_val[5],&eq_val[6],&eq_val[7],&eq_val[8],&eq_val[9],&eq_val[10],&eq_val[11],&eq_val[12],&eq_val[13],&eq_val[14],&eq_val[15],&eq_val[16],&eq_val[17],&eq_val[18],&eq_val[19],&eq_val[20],&eq_val[21],&eq_val[22],&eq_val[23],&eq_val[24],&eq_val[25],&eq_val[26],&eq_val[27],&eq_val[28],&eq_val[29],&eq_val[30],&eq_val[31],&eq_val[32],&eq_val[33],&eq_val[34],&value_type) == 36) {
					for (ii = 0;ii < 35;ii++) {
						if (eq_val[ii] < -14 || eq_val[ii] > 14) break;
						eq_val[ii] = eq_val[ii] * -1;
					}
					if (ii == 35) {
						TB_1_1->Position = eq_val[0];
						TB_1_2->Position = eq_val[1];
						TB_1_3->Position = eq_val[2];
						TB_1_4->Position = eq_val[3];
						TB_1_5->Position = eq_val[4];
						TB_1_6->Position = eq_val[5];
						TB_1_7->Position = eq_val[6];
						TB_1_8->Position = eq_val[7];
						TB_1_9->Position = eq_val[8];
						TB_1_10->Position = eq_val[9];
						TB_1_11->Position = eq_val[10];
						TB_1_12->Position = eq_val[11];
						TB_1_13->Position = eq_val[12];
						TB_1_14->Position = eq_val[13];
						TB_1_15->Position = eq_val[14];
						TB_1_16->Position = eq_val[15];
						TB_1_17->Position = eq_val[16];
						TB_1_18->Position = eq_val[17];
						TB_1_19->Position = eq_val[18];
						TB_1_20->Position = eq_val[19];
						TB_1_21->Position = eq_val[20];
						TB_1_22->Position = eq_val[21];
						TB_1_23->Position = eq_val[22];
						TB_1_24->Position = eq_val[23];
						TB_1_25->Position = eq_val[24];
						TB_1_26->Position = eq_val[25];
						TB_1_27->Position = eq_val[26];
						TB_1_28->Position = eq_val[27];
						TB_1_29->Position = eq_val[28];
						TB_1_30->Position = eq_val[29];
						TB_1_31->Position = eq_val[30];
						TB_1_32->Position = eq_val[31];
						TB_1_33->Position = eq_val[32];
						TB_1_34->Position = eq_val[33];
						TB_1_35->Position = eq_val[34];
						rdo_type1_value_x1->Checked = true;
						if (value_type == 2) {
							rdo_type1_value_x2->Checked = true;
						}
						error = 0;
					}
				}
				if (error == 0) {
					error = 1;
					if (sscanf(eqParam2.c_str(),"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
								&eq_val[0],&eq_val[1],&eq_val[2],&eq_val[3],&eq_val[4],&eq_val[5],&eq_val[6],&eq_val[7],&eq_val[8],&eq_val[9],&eq_val[10],&eq_val[11],&eq_val[12],&eq_val[13],&eq_val[14],&eq_val[15],&eq_val[16],&eq_val[17],&eq_val[18],&eq_val[19],&eq_val[20],&eq_val[21],&eq_val[22],&eq_val[23],&eq_val[24],&eq_val[25],&eq_val[26],&eq_val[27],&eq_val[28],&eq_val[29],&eq_val[30],&eq_val[31],&eq_val[32],&eq_val[33],&eq_val[34],&eq_val[35],&eq_val[36],&eq_val[37],&eq_val[38],&eq_val[39],&eq_val[40],&eq_val[41],&eq_val[42],&eq_val[43],&eq_val[44],&eq_val[45],&eq_val[46],&eq_val[47],&eq_val[48],&eq_val[49],&value_type) == 51) {
						for (ii = 0;ii < 50;ii++) {
							if (eq_val[ii] < -14 || eq_val[ii] > 14) break;
							eq_val[ii] = eq_val[ii] * -1;
						}
						if (ii == 50) {
							TB_2_1->Position = eq_val[0];
							TB_2_2->Position = eq_val[1];
							TB_2_3->Position = eq_val[2];
							TB_2_4->Position = eq_val[3];
							TB_2_5->Position = eq_val[4];
							TB_2_6->Position = eq_val[5];
							TB_2_7->Position = eq_val[6];
							TB_2_8->Position = eq_val[7];
							TB_2_9->Position = eq_val[8];
							TB_2_10->Position = eq_val[9];
							TB_2_11->Position = eq_val[10];
							TB_2_12->Position = eq_val[11];
							TB_2_13->Position = eq_val[12];
							TB_2_14->Position = eq_val[13];
							TB_2_15->Position = eq_val[14];
							TB_2_16->Position = eq_val[15];
							TB_2_17->Position = eq_val[16];
							TB_2_18->Position = eq_val[17];
							TB_2_19->Position = eq_val[18];
							TB_2_20->Position = eq_val[19];
							TB_2_21->Position = eq_val[20];
							TB_2_22->Position = eq_val[21];
							TB_2_23->Position = eq_val[22];
							TB_2_24->Position = eq_val[23];
							TB_2_25->Position = eq_val[24];
							TB_2_26->Position = eq_val[25];
							TB_2_27->Position = eq_val[26];
							TB_2_28->Position = eq_val[27];
							TB_2_29->Position = eq_val[28];
							TB_2_30->Position = eq_val[29];
							TB_2_31->Position = eq_val[30];
							TB_2_32->Position = eq_val[31];
							TB_2_33->Position = eq_val[32];
							TB_2_34->Position = eq_val[33];
							TB_2_35->Position = eq_val[34];
							TB_2_36->Position = eq_val[35];
							TB_2_37->Position = eq_val[36];
							TB_2_38->Position = eq_val[37];
							TB_2_39->Position = eq_val[38];
							TB_2_40->Position = eq_val[39];
							TB_2_41->Position = eq_val[40];
							TB_2_42->Position = eq_val[41];
							TB_2_43->Position = eq_val[42];
							TB_2_44->Position = eq_val[43];
							TB_2_45->Position = eq_val[44];
							TB_2_46->Position = eq_val[45];
							TB_2_47->Position = eq_val[46];
							TB_2_48->Position = eq_val[47];
							TB_2_49->Position = eq_val[48];
							TB_2_50->Position = eq_val[49];
							rdo_type2_value_x1->Checked = true;
							if (value_type == 2) {
								rdo_type2_value_x2->Checked = true;
							}
							error = 0;
						}
					}
				}
				if (error == 0) {
					if (sscanf(eqParam3.c_str(),"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
							&eq_val[0],&eq_val[1],&eq_val[2],&eq_val[3],&eq_val[4],&eq_val[5],&eq_val[6],&eq_val[7],&eq_val[8],&eq_val[9],&eq_val[10],&eq_val[11],&eq_val[12],&eq_val[13],&eq_val[14],&eq_val[15],&eq_val[16],&eq_val[17],&eq_val[18],&eq_val[19],&eq_val[20],&eq_val[21],&eq_val[22],&eq_val[23],&eq_val[24],&eq_val[25],&eq_val[26],&eq_val[27],&eq_val[28],&eq_val[29],&eq_val[30],&eq_val[31],&eq_val[32],&eq_val[33],&eq_val[34],&eq_val[35],&eq_val[36],&eq_val[37],&eq_val[38],&eq_val[39],&eq_val[40],&eq_val[41],&eq_val[42],&eq_val[43],&eq_val[44],&eq_val[45],&eq_val[46],&eq_val[47],&eq_val[48],&eq_val[49],&eq_val[50],&eq_val[51],&eq_val[52],&eq_val[53],&eq_val[54],&eq_val[55],&eq_val[56],&eq_val[57],&eq_val[58],&eq_val[59],&eq_val[60],&eq_val[61],&eq_val[62],&eq_val[63],&eq_val[64],&eq_val[65],&eq_val[66],&eq_val[67],&eq_val[68],&eq_val[69],&eq_val[70],&eq_val[71],&eq_val[72],&eq_val[73],&eq_val[74],&eq_val[75],&eq_val[76],&eq_val[77],&value_type) == 79) {
						for (ii = 0;ii < 78;ii++) {
							if (eq_val[ii] < -14 || eq_val[ii] > 14) break;
							eq_val[ii] = eq_val[ii] * -1;
						}
						if (ii == 78) {
							TB_3_1->Position = eq_val[0];
							TB_3_2->Position = eq_val[1];
							TB_3_3->Position = eq_val[2];
							TB_3_4->Position = eq_val[3];
							TB_3_5->Position = eq_val[4];
							TB_3_6->Position = eq_val[5];
							TB_3_7->Position = eq_val[6];
							TB_3_8->Position = eq_val[7];
							TB_3_9->Position = eq_val[8];
							TB_3_10->Position = eq_val[9];
							TB_3_11->Position = eq_val[10];
							TB_3_12->Position = eq_val[11];
							TB_3_13->Position = eq_val[12];
							TB_3_14->Position = eq_val[13];
							TB_3_15->Position = eq_val[14];
							TB_3_16->Position = eq_val[15];
							TB_3_17->Position = eq_val[16];
							TB_3_18->Position = eq_val[17];
							TB_3_19->Position = eq_val[18];
							TB_3_20->Position = eq_val[19];
							TB_3_21->Position = eq_val[20];
							TB_3_22->Position = eq_val[21];
							TB_3_23->Position = eq_val[22];
							TB_3_24->Position = eq_val[23];
							TB_3_25->Position = eq_val[24];
							TB_3_26->Position = eq_val[25];
							TB_3_27->Position = eq_val[26];
							TB_3_28->Position = eq_val[27];
							TB_3_29->Position = eq_val[28];
							TB_3_30->Position = eq_val[29];
							TB_3_31->Position = eq_val[30];
							TB_3_32->Position = eq_val[31];
							TB_3_33->Position = eq_val[32];
							TB_3_34->Position = eq_val[33];
							TB_3_35->Position = eq_val[34];
							TB_3_36->Position = eq_val[35];
							TB_3_37->Position = eq_val[36];
							TB_3_38->Position = eq_val[37];
							TB_3_39->Position = eq_val[38];
							TB_3_40->Position = eq_val[39];
							TB_3_41->Position = eq_val[40];
							TB_3_42->Position = eq_val[41];
							TB_3_43->Position = eq_val[42];
							TB_3_44->Position = eq_val[43];
							TB_3_45->Position = eq_val[44];
							TB_3_46->Position = eq_val[45];
							TB_3_47->Position = eq_val[46];
							TB_3_48->Position = eq_val[47];
							TB_3_49->Position = eq_val[48];
							TB_3_50->Position = eq_val[49];
							TB_3_51->Position = eq_val[50];
							TB_3_52->Position = eq_val[51];
							TB_3_53->Position = eq_val[52];
							TB_3_54->Position = eq_val[53];
							TB_3_55->Position = eq_val[54];
							TB_3_56->Position = eq_val[55];
							TB_3_57->Position = eq_val[56];
							TB_3_58->Position = eq_val[57];
							TB_3_59->Position = eq_val[58];
							TB_3_60->Position = eq_val[59];
							TB_3_61->Position = eq_val[60];
							TB_3_62->Position = eq_val[61];
							TB_3_63->Position = eq_val[62];
							TB_3_64->Position = eq_val[63];
							TB_3_65->Position = eq_val[64];
							TB_3_66->Position = eq_val[65];
							TB_3_67->Position = eq_val[66];
							TB_3_68->Position = eq_val[67];
							TB_3_69->Position = eq_val[68];
							TB_3_70->Position = eq_val[69];
							TB_3_71->Position = eq_val[70];
							TB_3_72->Position = eq_val[71];
							TB_3_73->Position = eq_val[72];
							TB_3_74->Position = eq_val[73];
							TB_3_75->Position = eq_val[74];
							TB_3_76->Position = eq_val[75];
							TB_3_77->Position = eq_val[76];
							TB_3_78->Position = eq_val[77];
							rdo_type3_value_x1->Checked = true;
							if (value_type == 2) {
								rdo_type3_value_x2->Checked = true;
							}
						}
					}
				}
				if (error == 0) {
					if (sscanf(eqParam4.c_str(),"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
							&eq_val[0],&eq_val[1],&eq_val[2],&eq_val[3],&eq_val[4],&eq_val[5],&eq_val[6],&eq_val[7],&eq_val[8],&eq_val[9],&eq_val[10],&eq_val[11],&eq_val[12],&eq_val[13],&eq_val[14],&eq_val[15],&eq_val[16],&eq_val[17],&eq_val[18],&eq_val[19],&eq_val[20],&eq_val[21],&eq_val[22],&eq_val[23],&eq_val[24],&eq_val[25],&eq_val[26],&eq_val[27],&eq_val[28],&eq_val[29],&eq_val[30],&eq_val[31],&eq_val[32],&eq_val[33],&eq_val[34],&eq_val[35],&eq_val[36],&eq_val[37],&eq_val[38],&eq_val[39],&eq_val[40],&eq_val[41],&eq_val[42],&eq_val[43],&eq_val[44],&eq_val[45],&eq_val[46],&eq_val[47],&eq_val[48],&eq_val[49],&eq_val[50],&eq_val[51],&eq_val[52],&eq_val[53],&eq_val[54],&eq_val[55],&eq_val[56],&eq_val[57],&eq_val[58],&eq_val[59],&eq_val[60],&eq_val[61],&eq_val[62],&eq_val[63],&eq_val[64],&eq_val[65],&eq_val[66],&eq_val[67],&eq_val[68],&eq_val[69],&eq_val[70],&eq_val[71],&eq_val[72],&eq_val[73],&eq_val[74],&eq_val[75],&eq_val[76],&eq_val[77],&eq_val[78],&eq_val[79],&eq_val[80],&eq_val[81],&eq_val[82],&eq_val[83],&eq_val[84],&eq_val[85],&eq_val[86],&eq_val[87],&eq_val[88],&eq_val[89],&eq_val[90],&eq_val[91],&eq_val[92],&eq_val[93],&eq_val[94],&eq_val[95],&eq_val[96],&eq_val[97],&value_type) == 99) {
						for (ii = 0;ii < 98;ii++) {
							if (eq_val[ii] >= -14 && eq_val[ii] <= 14) {
								eq_val[ii] = eq_val[ii] * -1;
							}
						}
						if (ii == 98) {
							TB_4_1_Cutoff->Checked = false;
							TB_4_2_Cutoff->Checked = false;
							TB_4_3_Cutoff->Checked = false;
							TB_4_4_Cutoff->Checked = false;
							TB_4_5_Cutoff->Checked = false;
							TB_4_6_Cutoff->Checked = false;
							TB_4_7_Cutoff->Checked = false;
							TB_4_8_Cutoff->Checked = false;
							TB_4_9_Cutoff->Checked = false;
							TB_4_10_Cutoff->Checked = false;
							TB_4_11_Cutoff->Checked = false;
							TB_4_12_Cutoff->Checked = false;
							TB_4_13_Cutoff->Checked = false;
							TB_4_36_Cutoff->Checked = false;
							TB_4_37_Cutoff->Checked = false;
							TB_4_38_Cutoff->Checked = false;
							TB_4_39_Cutoff->Checked = false;
							TB_4_40_Cutoff->Checked = false;
							TB_4_41_Cutoff->Checked = false;
							TB_4_42_Cutoff->Checked = false;
							TB_4_43_Cutoff->Checked = false;
							TB_4_44_Cutoff->Checked = false;
							TB_4_45_Cutoff->Checked = false;
							TB_4_46_Cutoff->Checked = false;
							TB_4_47_Cutoff->Checked = false;
							TB_4_48_Cutoff->Checked = false;
							TB_4_49_Cutoff->Checked = false;
							TB_4_50_Cutoff->Checked = false;
							TB_4_51_Cutoff->Checked = false;
							TB_4_52_Cutoff->Checked = false;
							TB_4_53_Cutoff->Checked = false;
							TB_4_54_Cutoff->Checked = false;
							TB_4_55_Cutoff->Checked = false;
							TB_4_56_Cutoff->Checked = false;
							TB_4_57_Cutoff->Checked = false;
							TB_4_58_Cutoff->Checked = false;
							TB_4_59_Cutoff->Checked = false;
							TB_4_60_Cutoff->Checked = false;
							TB_4_61_Cutoff->Checked = false;
							TB_4_62_Cutoff->Checked = false;
							TB_4_63_Cutoff->Checked = false;
							TB_4_64_Cutoff->Checked = false;
							TB_4_65_Cutoff->Checked = false;
							TB_4_66_Cutoff->Checked = false;
							TB_4_67_Cutoff->Checked = false;
							TB_4_68_Cutoff->Checked = false;
							TB_4_69_Cutoff->Checked = false;
							TB_4_70_Cutoff->Checked = false;
							TB_4_71_Cutoff->Checked = false;
							TB_4_72_Cutoff->Checked = false;
							TB_4_73_Cutoff->Checked = false;
							TB_4_74_Cutoff->Checked = false;
							TB_4_75_Cutoff->Checked = false;
							TB_4_76_Cutoff->Checked = false;
							TB_4_77_Cutoff->Checked = false;
							TB_4_78_Cutoff->Checked = false;
							TB_4_79_Cutoff->Checked = false;
							TB_4_80_Cutoff->Checked = false;
							TB_4_81_Cutoff->Checked = false;
							TB_4_82_Cutoff->Checked = false;
							TB_4_83_Cutoff->Checked = false;
							TB_4_84_Cutoff->Checked = false;
							TB_4_85_Cutoff->Checked = false;
							TB_4_86_Cutoff->Checked = false;
							TB_4_87_Cutoff->Checked = false;
							TB_4_88_Cutoff->Checked = false;
							TB_4_89_Cutoff->Checked = false;
							TB_4_90_Cutoff->Checked = false;
							TB_4_91_Cutoff->Checked = false;
							TB_4_92_Cutoff->Checked = false;
							TB_4_93_Cutoff->Checked = false;
							TB_4_94_Cutoff->Checked = false;
							TB_4_95_Cutoff->Checked = false;
							TB_4_96_Cutoff->Checked = false;
							TB_4_97_Cutoff->Checked = false;
							TB_4_98_Cutoff->Checked = false;

							if (eq_val[0] == -99) {TB_4_1_Cutoff->Checked = true;TB_4_1->Position = 14;}
							if (eq_val[1] == -99) {TB_4_2_Cutoff->Checked = true;TB_4_2->Position = 14;}
							if (eq_val[2] == -99) {TB_4_3_Cutoff->Checked = true;TB_4_3->Position = 14;}
							if (eq_val[3] == -99) {TB_4_4_Cutoff->Checked = true;TB_4_4->Position = 14;}
							if (eq_val[4] == -99) {TB_4_5_Cutoff->Checked = true;TB_4_5->Position = 14;}
							if (eq_val[5] == -99) {TB_4_6_Cutoff->Checked = true;TB_4_6->Position = 14;}
							if (eq_val[6] == -99) {TB_4_7_Cutoff->Checked = true;TB_4_7->Position = 14;}
							if (eq_val[7] == -99) {TB_4_8_Cutoff->Checked = true;TB_4_8->Position = 14;}
							if (eq_val[8] == -99) {TB_4_9_Cutoff->Checked = true;TB_4_9->Position = 14;}
							if (eq_val[9] == -99) {TB_4_10_Cutoff->Checked = true;TB_4_10->Position = 14;;}
							if (eq_val[10] == -99) {TB_4_11_Cutoff->Checked = true;TB_4_11->Position = 14;}
							if (eq_val[11] == -99) {TB_4_12_Cutoff->Checked = true;TB_4_12->Position = 14;}
							if (eq_val[12] == -99) {TB_4_13_Cutoff->Checked = true;TB_4_13->Position = 14;}
							if (eq_val[35] == -99) {TB_4_36_Cutoff->Checked = true;TB_4_36->Position = 14;}
							if (eq_val[36] == -99) {TB_4_37_Cutoff->Checked = true;TB_4_37->Position = 14;}
							if (eq_val[37] == -99) {TB_4_38_Cutoff->Checked = true;TB_4_38->Position = 14;}
							if (eq_val[38] == -99) {TB_4_39_Cutoff->Checked = true;TB_4_39->Position = 14;}
							if (eq_val[39] == -99) {TB_4_40_Cutoff->Checked = true;TB_4_40->Position = 14;}
							if (eq_val[40] == -99) {TB_4_41_Cutoff->Checked = true;TB_4_41->Position = 14;}
							if (eq_val[41] == -99) {TB_4_42_Cutoff->Checked = true;TB_4_42->Position = 14;}
							if (eq_val[42] == -99) {TB_4_43_Cutoff->Checked = true;TB_4_43->Position = 14;}
							if (eq_val[43] == -99) {TB_4_44_Cutoff->Checked = true;TB_4_44->Position = 14;}
							if (eq_val[44] == -99) {TB_4_45_Cutoff->Checked = true;TB_4_45->Position = 14;}
							if (eq_val[45] == -99) {TB_4_46_Cutoff->Checked = true;TB_4_46->Position = 14;}
							if (eq_val[46] == -99) {TB_4_47_Cutoff->Checked = true;TB_4_47->Position = 14;}
							if (eq_val[47] == -99) {TB_4_48_Cutoff->Checked = true;TB_4_48->Position = 14;}
							if (eq_val[48] == -99) {TB_4_49_Cutoff->Checked = true;TB_4_49->Position = 14;}
							if (eq_val[49] == -99) {TB_4_50_Cutoff->Checked = true;TB_4_50->Position = 14;}
							if (eq_val[50] == -99) {TB_4_51_Cutoff->Checked = true;TB_4_51->Position = 14;}
							if (eq_val[51] == -99) {TB_4_52_Cutoff->Checked = true;TB_4_52->Position = 14;}
							if (eq_val[52] == -99) {TB_4_53_Cutoff->Checked = true;TB_4_53->Position = 14;}
							if (eq_val[53] == -99) {TB_4_54_Cutoff->Checked = true;TB_4_54->Position = 14;}
							if (eq_val[54] == -99) {TB_4_55_Cutoff->Checked = true;TB_4_55->Position = 14;}
							if (eq_val[55] == -99) {TB_4_56_Cutoff->Checked = true;TB_4_56->Position = 14;}
							if (eq_val[56] == -99) {TB_4_57_Cutoff->Checked = true;TB_4_57->Position = 14;}
							if (eq_val[57] == -99) {TB_4_58_Cutoff->Checked = true;TB_4_58->Position = 14;}
							if (eq_val[58] == -99) {TB_4_59_Cutoff->Checked = true;TB_4_59->Position = 14;}
							if (eq_val[59] == -99) {TB_4_60_Cutoff->Checked = true;TB_4_60->Position = 14;}
							if (eq_val[60] == -99) {TB_4_61_Cutoff->Checked = true;TB_4_61->Position = 14;}
							if (eq_val[61] == -99) {TB_4_62_Cutoff->Checked = true;TB_4_62->Position = 14;}
							if (eq_val[62] == -99) {TB_4_63_Cutoff->Checked = true;TB_4_63->Position = 14;}
							if (eq_val[63] == -99) {TB_4_64_Cutoff->Checked = true;TB_4_64->Position = 14;}
							if (eq_val[64] == -99) {TB_4_65_Cutoff->Checked = true;TB_4_65->Position = 14;}
							if (eq_val[65] == -99) {TB_4_66_Cutoff->Checked = true;TB_4_66->Position = 14;}
							if (eq_val[66] == -99) {TB_4_67_Cutoff->Checked = true;TB_4_67->Position = 14;}
							if (eq_val[67] == -99) {TB_4_68_Cutoff->Checked = true;TB_4_68->Position = 14;}
							if (eq_val[68] == -99) {TB_4_69_Cutoff->Checked = true;TB_4_69->Position = 14;}
							if (eq_val[69] == -99) {TB_4_70_Cutoff->Checked = true;TB_4_70->Position = 14;}
							if (eq_val[70] == -99) {TB_4_71_Cutoff->Checked = true;TB_4_71->Position = 14;}
							if (eq_val[71] == -99) {TB_4_72_Cutoff->Checked = true;TB_4_72->Position = 14;}
							if (eq_val[72] == -99) {TB_4_73_Cutoff->Checked = true;TB_4_73->Position = 14;}
							if (eq_val[73] == -99) {TB_4_74_Cutoff->Checked = true;TB_4_74->Position = 14;}
							if (eq_val[74] == -99) {TB_4_75_Cutoff->Checked = true;TB_4_75->Position = 14;}
							if (eq_val[75] == -99) {TB_4_76_Cutoff->Checked = true;TB_4_76->Position = 14;}
							if (eq_val[76] == -99) {TB_4_77_Cutoff->Checked = true;TB_4_77->Position = 14;}
							if (eq_val[77] == -99) {TB_4_78_Cutoff->Checked = true;TB_4_78->Position = 14;}
							if (eq_val[78] == -99) {TB_4_79_Cutoff->Checked = true;TB_4_79->Position = 14;}
							if (eq_val[79] == -99) {TB_4_80_Cutoff->Checked = true;TB_4_80->Position = 14;}
							if (eq_val[80] == -99) {TB_4_81_Cutoff->Checked = true;TB_4_81->Position = 14;}
							if (eq_val[81] == -99) {TB_4_82_Cutoff->Checked = true;TB_4_82->Position = 14;}
							if (eq_val[82] == -99) {TB_4_83_Cutoff->Checked = true;TB_4_83->Position = 14;}
							if (eq_val[83] == -99) {TB_4_84_Cutoff->Checked = true;TB_4_84->Position = 14;}
							if (eq_val[84] == -99) {TB_4_85_Cutoff->Checked = true;TB_4_85->Position = 14;}
							if (eq_val[85] == -99) {TB_4_86_Cutoff->Checked = true;TB_4_86->Position = 14;}
							if (eq_val[86] == -99) {TB_4_87_Cutoff->Checked = true;TB_4_87->Position = 14;}
							if (eq_val[87] == -99) {TB_4_88_Cutoff->Checked = true;TB_4_88->Position = 14;}
							if (eq_val[88] == -99) {TB_4_89_Cutoff->Checked = true;TB_4_89->Position = 14;}
							if (eq_val[89] == -99) {TB_4_90_Cutoff->Checked = true;TB_4_90->Position = 14;}
							if (eq_val[90] == -99) {TB_4_91_Cutoff->Checked = true;TB_4_91->Position = 14;}
							if (eq_val[91] == -99) {TB_4_92_Cutoff->Checked = true;TB_4_92->Position = 14;}
							if (eq_val[92] == -99) {TB_4_93_Cutoff->Checked = true;TB_4_93->Position = 14;}
							if (eq_val[93] == -99) {TB_4_94_Cutoff->Checked = true;TB_4_94->Position = 14;}
							if (eq_val[94] == -99) {TB_4_95_Cutoff->Checked = true;TB_4_95->Position = 14;}
							if (eq_val[95] == -99) {TB_4_96_Cutoff->Checked = true;TB_4_96->Position = 14;}
							if (eq_val[96] == -99) {TB_4_97_Cutoff->Checked = true;TB_4_97->Position = 14;}
							if (eq_val[97] == -99) {TB_4_98_Cutoff->Checked = true;TB_4_98->Position = 14;}


							if (eq_val[0] != -99) TB_4_1->Position = eq_val[0];
							if (eq_val[1] != -99) TB_4_2->Position = eq_val[1];
							if (eq_val[2] != -99) TB_4_3->Position = eq_val[2];
							if (eq_val[3] != -99) TB_4_4->Position = eq_val[3];
							if (eq_val[4] != -99) TB_4_5->Position = eq_val[4];
							if (eq_val[5] != -99) TB_4_6->Position = eq_val[5];
							if (eq_val[6] != -99) TB_4_7->Position = eq_val[6];
							if (eq_val[7] != -99) TB_4_8->Position = eq_val[7];
							if (eq_val[8] != -99) TB_4_9->Position = eq_val[8];
							if (eq_val[9] != -99) TB_4_10->Position = eq_val[9];
							if (eq_val[10] != -99) TB_4_11->Position = eq_val[10];
							if (eq_val[11] != -99) TB_4_12->Position = eq_val[11];
							if (eq_val[12] != -99) TB_4_13->Position = eq_val[12];
							TB_4_14->Position = eq_val[13];
							TB_4_15->Position = eq_val[14];
							TB_4_16->Position = eq_val[15];
							TB_4_17->Position = eq_val[16];
							TB_4_18->Position = eq_val[17];
							TB_4_19->Position = eq_val[18];
							TB_4_20->Position = eq_val[19];
							TB_4_21->Position = eq_val[20];
							TB_4_22->Position = eq_val[21];
							TB_4_23->Position = eq_val[22];
							TB_4_24->Position = eq_val[23];
							TB_4_25->Position = eq_val[24];
							TB_4_26->Position = eq_val[25];
							TB_4_27->Position = eq_val[26];
							TB_4_28->Position = eq_val[27];
							TB_4_29->Position = eq_val[28];
							TB_4_30->Position = eq_val[29];
							TB_4_31->Position = eq_val[30];
							TB_4_32->Position = eq_val[31];
							TB_4_33->Position = eq_val[32];
							TB_4_34->Position = eq_val[33];
							TB_4_35->Position = eq_val[34];
							if (eq_val[35] != -99) TB_4_36->Position = eq_val[35];
							if (eq_val[36] != -99) TB_4_37->Position = eq_val[36];
							if (eq_val[37] != -99) TB_4_38->Position = eq_val[37];
							if (eq_val[38] != -99) TB_4_39->Position = eq_val[38];
							if (eq_val[39] != -99) TB_4_40->Position = eq_val[39];
							if (eq_val[40] != -99) TB_4_41->Position = eq_val[40];
							if (eq_val[41] != -99) TB_4_42->Position = eq_val[41];
							if (eq_val[42] != -99) TB_4_43->Position = eq_val[42];
							if (eq_val[43] != -99) TB_4_44->Position = eq_val[43];
							if (eq_val[44] != -99) TB_4_45->Position = eq_val[44];
							if (eq_val[45] != -99) TB_4_46->Position = eq_val[45];
							if (eq_val[46] != -99) TB_4_47->Position = eq_val[46];
							if (eq_val[47] != -99) TB_4_48->Position = eq_val[47];
							if (eq_val[48] != -99) TB_4_49->Position = eq_val[48];
							if (eq_val[49] != -99) TB_4_50->Position = eq_val[49];
							if (eq_val[50] != -99) TB_4_51->Position = eq_val[50];
							if (eq_val[51] != -99) TB_4_52->Position = eq_val[51];
							if (eq_val[52] != -99) TB_4_53->Position = eq_val[52];
							if (eq_val[53] != -99) TB_4_54->Position = eq_val[53];
							if (eq_val[54] != -99) TB_4_55->Position = eq_val[54];
							if (eq_val[55] != -99) TB_4_56->Position = eq_val[55];
							if (eq_val[56] != -99) TB_4_57->Position = eq_val[56];
							if (eq_val[57] != -99) TB_4_58->Position = eq_val[57];
							if (eq_val[58] != -99) TB_4_59->Position = eq_val[58];
							if (eq_val[59] != -99) TB_4_60->Position = eq_val[59];
							if (eq_val[60] != -99) TB_4_61->Position = eq_val[60];
							if (eq_val[61] != -99) TB_4_62->Position = eq_val[61];
							if (eq_val[62] != -99) TB_4_63->Position = eq_val[62];
							if (eq_val[63] != -99) TB_4_64->Position = eq_val[63];
							if (eq_val[64] != -99) TB_4_65->Position = eq_val[64];
							if (eq_val[65] != -99) TB_4_66->Position = eq_val[65];
							if (eq_val[66] != -99) TB_4_67->Position = eq_val[66];
							if (eq_val[67] != -99) TB_4_68->Position = eq_val[67];
							if (eq_val[68] != -99) TB_4_69->Position = eq_val[68];
							if (eq_val[69] != -99) TB_4_70->Position = eq_val[69];
							if (eq_val[70] != -99) TB_4_71->Position = eq_val[70];
							if (eq_val[71] != -99) TB_4_72->Position = eq_val[71];
							if (eq_val[72] != -99) TB_4_73->Position = eq_val[72];
							if (eq_val[73] != -99) TB_4_74->Position = eq_val[73];
							if (eq_val[74] != -99) TB_4_75->Position = eq_val[74];
							if (eq_val[75] != -99) TB_4_76->Position = eq_val[75];
							if (eq_val[76] != -99) TB_4_77->Position = eq_val[76];
							if (eq_val[77] != -99) TB_4_78->Position = eq_val[77];
							if (eq_val[78] != -99) TB_4_79->Position = eq_val[78];
							if (eq_val[79] != -99) TB_4_80->Position = eq_val[79];
							if (eq_val[80] != -99) TB_4_81->Position = eq_val[80];
							if (eq_val[81] != -99) TB_4_82->Position = eq_val[81];
							if (eq_val[82] != -99) TB_4_83->Position = eq_val[82];
							if (eq_val[83] != -99) TB_4_84->Position = eq_val[83];
							if (eq_val[84] != -99) TB_4_85->Position = eq_val[84];
							if (eq_val[85] != -99) TB_4_86->Position = eq_val[85];
							if (eq_val[86] != -99) TB_4_87->Position = eq_val[86];
							if (eq_val[86] != -99) TB_4_88->Position = eq_val[87];
							if (eq_val[88] != -99) TB_4_89->Position = eq_val[88];
							if (eq_val[89] != -99) TB_4_90->Position = eq_val[89];
							if (eq_val[90] != -99) TB_4_91->Position = eq_val[90];
							if (eq_val[91] != -99) TB_4_92->Position = eq_val[91];
							if (eq_val[92] != -99) TB_4_93->Position = eq_val[92];
							if (eq_val[93] != -99) TB_4_94->Position = eq_val[93];
							if (eq_val[94] != -99) TB_4_95->Position = eq_val[94];
							if (eq_val[95] != -99) TB_4_96->Position = eq_val[95];
							if (eq_val[96] != -99) TB_4_97->Position = eq_val[96];
							if (eq_val[97] != -99) TB_4_98->Position = eq_val[97];
							rdo_type4_value_x1->Checked = true;
							if (value_type == 2) {
								rdo_type4_value_x2->Checked = true;
							}
						}
					}
				}
			}
			SetPositionToTag();
		}
	}
	PRINT_FN_LOG("ReloadDefaultParameter","End");
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

	PRINT_FN_LOG("AddToFileList","Start");
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
		wide_str_len = filename.Length();
		p_wchar = new wchar_t[wide_str_len + 2];
		wcscpy(p_wchar, filename.c_str());
		item->Data = p_wchar;
		if (LvFile->Items->Count > 0)
			btnStart->Enabled = true;
	}
	PRINT_FN_LOG("AddToFileList","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::AddToAnaFileList(UnicodeString filename) {
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

	PRINT_FN_LOG("AddToAnaFileList","Start");

	uStr = GetFilePath(1, filename.c_str());
	sStr = uStr;
	ret = PLG_InfoAudioData(sStr.c_str(), &inFmt, &nSample, NULL);
	if (ret == STATUS_SUCCESS) {
		fn = ExtractFileName(filename);
		item = LvAnaFile->Items->Add();

		// edtFileInfo に音声ファイルの情報を保存する
		item->Caption = "";
		if ((AnsiString)inFmt.fmt == "wav" || (AnsiString)inFmt.fmt == "rf64") {
			item->SubItems->Add(fn);
			time = nSample / inFmt.sample;
			s.printf("%02d:%02d", time / 60, time % 60);
			item->SubItems->Add(s);
			sampling = inFmt.sample;
			s.printf("%ld", sampling);
			item->SubItems->Add(s);
		} else {
			item->SubItems->Add(fn);
			s.printf("-");
			item->SubItems->Add(s);
			s.printf("-");
			item->SubItems->Add(s);
		}
		wide_str_len = filename.Length();
		p_wchar = new wchar_t[wide_str_len + 2];
		wcscpy(p_wchar, filename.c_str());
		item->Data = p_wchar;
		if (LvFile->Items->Count > 0 || LvAnaFile->Items->Count > 0)
			btnStart->Enabled = true;
	}
	PRINT_FN_LOG("AddToAnaFileList","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::LvFileWinProc(TMessage& Msg) {
	TListItem *item;
	HANDLE hDrop;
	int i, nFiles;
	wchar_t szFile[MAX_PATH];

//	PRINT_FN_LOG("LvFileWinProc","Start");
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
	} else {
		orgLvFileWinProc(Msg);
	}
//	PRINT_FN_LOG("LvFileWinProc","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::LvAnaFileWinProc(TMessage& Msg) {
	TListItem *item;
	HANDLE hDrop;
	int i, nFiles;
	wchar_t szFile[MAX_PATH];

//	PRINT_FN_LOG("LvAnaFileWinProc","Start");
	if (Msg.Msg == WM_DROPFILES) {
		Msg.Result = 0;

		hDrop = (HANDLE)Msg.WParam;
		nFiles = DragQueryFile((HDROP)hDrop, -1, NULL, 0);
		for (i = 0; i < nFiles; i++) {
			if (DragQueryFile((HDROP)hDrop, i, szFile, sizeof(szFile)) > 0) {
				AddToAnaFileList((UnicodeString)szFile);
			}
		}
		DragFinish((HDROP)hDrop);
		btnClear->Enabled = true;
	} else {
		orgLvAnaFileWinProc(Msg);
	}
//	PRINT_FN_LOG("LvAnaFileWinProc","End");
}
// ---------------------------------------------------------------------------
// type1 : 読めれば ShortPathName でも良い
// type2 : 書き込む必要がある名前(〜) は変換
UnicodeString __fastcall TForm1::GetFilePath(int type, wchar_t *data) {
	FILE *ifp;
	UnicodeString uStr;
	AnsiString sStr;
	wchar_t *shortPath;

	PRINT_FN_LOG("GetFilePath","Start");

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
		uStr = StringReplace(uStr, L"〜", L"～",
			TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
		return uStr;
	}
	PRINT_FN_LOG("GetFilePath","End");
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

	PRINT_FN_LOG("btnAddClick","Start");
	if (OpenDialog->Execute()) {
		for (i = 0; i < OpenDialog->Files->Count; i++) {
			AddToFileList(OpenDialog->Files->Strings[i]);
		}
		if (LvFile->Items->Count > 0) {
			btnClear->Enabled = true;
			btnStart->Enabled = true;
		}
	}
	PRINT_FN_LOG("btnAddClick","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnDelClick(TObject *Sender) {
	TListItem *item;
	PRINT_FN_LOG("btnDelClick","Start");

	if (LvFile->Selected != NULL) {
		item = LvFile->Selected;
		if (item->Data) {
			delete[]item->Data;
		}
		item->Delete();
		if (LvFile->Items->Count == 0) {
			btnStart->Enabled = false;
			btnClear->Enabled = false;
			btnUp->Enabled = false;
			btnDown->Enabled = false;
		}
	}
	PRINT_FN_LOG("btnDelClick","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnUpClick(TObject *Sender) {
	UnicodeString s;
	TListItem *item, *new_item;

	PRINT_FN_LOG("btnUpClick","Start");
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
	PRINT_FN_LOG("btnUpClick","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnDownClick(TObject *Sender) {
	UnicodeString s;
	TListItem *item, *new_item;
	PRINT_FN_LOG("btnDownClick","Start");

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
	PRINT_FN_LOG("btnDownClick","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::LvFileSelectItem(TObject *Sender, TListItem *Item,bool Selected)
{
	AnsiString sStr;
	UnicodeString uStr;
	TListItem *item;
	int hh1,mm1,ss1,hh2,mm2,ss2;
	int i;
	UPI upi;
	SSIZE nSample;
	SSIZE outFileSize;
	SSIZE outMaxSize;

	track_skip_position->Enabled = false;
	btnDel->Enabled = false;
	btnPlay->Enabled = false;
	memset(&InFmt,0,sizeof(SOUNDFMT));
	memset(&FileInfo,0,sizeof (FILEINFO));

	if (LvFile->SelCount == 1) {
		btnDel->Enabled = true;
		if (LvFile->Selected != NULL) {
			SelectFileIndex = LvFile->Selected->Index;
			item = LvFile->Selected;
			if (item->Data) {
				uStr = GetFilePath(1, (wchar_t *)item->Data);
				sStr = uStr;
				PLG_InfoAudioData(sStr.c_str(),&InFmt,&FilenSample,&FileInfo);
				edtVInformation->Text = InFmt.extra;
				if (FilenSample > 0 && InFmt.sample > 0 && InFmt.channel > 0 && InFmt.bitsPerSample > 0) {
					if ((AnsiString)InFmt.fmt == (AnsiString)"wav" || (AnsiString)InFmt.fmt == (AnsiString)"rf64") {
						if (strlen(InFmt.time) > 0) {
							item->SubItems->Add(InFmt.duration_time);
						} else {
							item->SubItems->Add("-");
						}
						if (strlen(InFmt.extra) > 0) {
							item->SubItems->Add(InFmt.extra);
						} else {
							item->SubItems->Add("-");
						}
						btnPlay->Enabled = true;
						track_skip_position->Enabled = true;
					}
					if (InFmt.bitsPerSample == 16) {
						outFileSize = 2;
					} else if (InFmt.bitsPerSample == 24) {
						outFileSize = 3;
					} else if (InFmt.bitsPerSample == 32) {
						outFileSize = 4;
					} else if (InFmt.bitsPerSample == 64) {
						outFileSize = 8;
					}
					outFileSize *= InFmt.sample;
					outFileSize *= InFmt.duration_sec;
					outFileSize /= 1024;
					outFileSize /= 1024;
					outFileSize *= InFmt.channel;

					outMaxSize = 8;
					outMaxSize *= InFmt.sample;
					outMaxSize *= 2;
					outMaxSize *= 2;
					outMaxSize *= InFmt.duration_sec;
					outMaxSize /= 1024;
					outMaxSize /= 1024;
					outMaxSize *= InFmt.channel;
					uStr.sprintf(L"Size:%lld / Max:%lld",outFileSize,outMaxSize);
					edtFileSize->Text = uStr;
				}
			}
		}
	}

	if (LvFile->Selected != NULL && LvFile->Selected->Index > 0) {
		btnUp->Enabled = true;
	} else {
		btnUp->Enabled = false;
	}
	if (LvFile->Selected != NULL && LvFile->Selected->Index + 1 < LvFile->Items->Count) {
		btnDown->Enabled = true;
	} else {
		btnDown->Enabled = false;
	}

	PRINT_FN_LOG("LvFileSelectItem","Start");
	if (cmbPlayDevice->ItemIndex == -1) {
		PRINT_LOG("No Select Device");
		PRINT_FN_LOG("LvFileSelectItem","End");
        lbl_play_set_device->Visible = true;
		return;
	}

	if (CountSoundBuffer > 0) {
		for (i = 0;i < CountSoundBuffer;i++) {
			if (SoundBuffer[i].whdr.lpData != NULL) {
				free(SoundBuffer[i].whdr.lpData);
				SoundBuffer[i].whdr.lpData = NULL;
			}
			memset(&SoundBuffer[i],0,sizeof (SOUND_BUFFER));
			SoundBuffer[i].status = BUFFER_FREE;
		}
		CountSoundBuffer = 0;
	}
	IndexSoundBuffer = 0;

	PRINT_FN_LOG("LvFileSelectItem","End");
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnClearClick(TObject *Sender) {
	PRINT_FN_LOG("btnClearClick","Start");
	LvFile->Items->Clear();
	btnClear->Enabled = false;
	btnDel->Enabled = false;
	btnUp->Enabled = false;
	btnDown->Enabled = false;
	btnStart->Enabled = false;
	PRINT_FN_LOG("btnClearClick","End");
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::edtOutputPathLeftButtonClick(TObject *Sender) {

	WideString root = "";

	String dir = "";
	PRINT_FN_LOG("edtOutputPathLeftButtonClick","Start");


	if (lbl_select_directory->Caption != "" && DirectoryExists(lbl_select_directory->Caption,false)) {
		dir = lbl_select_directory->Caption;
	}

	if (SelectDirectory("EQ", root, dir)) {
		edtOutputPath->Text = dir;
		edtOutputPath->RightButton->Visible = true;
		lbl_select_directory->Caption = dir;
	}
	PRINT_FN_LOG("edtOutputPathLeftButtonClick","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::edtOutputPathRightButtonClick(TObject *Sender) {
	PRINT_FN_LOG("edtOutputPathRightButtonClick","Start");

	edtOutputPath->Text = L"Source Directory";
	if (edtSuffix->Text == "") {
		edtSuffix->Text = "eq";
	}
	PRINT_FN_LOG("edtOutputPathRightButtonClick","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::edtSuffixRightButtonClick(TObject *Sender) {
	PRINT_FN_LOG("edtSuffixRightButtonClick","Start");
	if (edtOutputPath->Text == L"Source Directory") {
		edtSuffix->Text = "eq";
	}
	else {
		edtSuffix->Text = "";
	}
	PRINT_FN_LOG("edtSuffixRightButtonClick","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::LvFileChange(TObject *Sender, TListItem *Item,TItemChange Change)
{
	if (btnStart->Visible == true) PRINT_FN_LOG("LvFileChange","Start");

	if (btnPlay->Tag == 0) {
		if (LvFile->Items->Count > 0) {
			btnStart->Enabled = true;
		} else {
			btnStart->Enabled = false;
		}
	}
	if (btnStart->Visible == true) 	PRINT_FN_LOG("LvFileChange","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnStartClick(TObject *Sender) {
	int i;
	int mode;
	char fullpath[MAX_PATH];
	char sss[256];
	char drive2[_MAX_DRIVE];
	char dir2[_MAX_DIR];
	char file[_MAX_FNAME];
	char file2[_MAX_FNAME];
	char ext2[_MAX_EXT];
	UnicodeString uStr;
	AnsiString outDir;
	PRINT_FN_LOG("btnStartClick","Start");

	preview_input_filename = "";

	if (pageMain->ActivePage == tabUpconv) {
		mode = 1;
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
	} else if (pageMain->ActivePage == tabAnalyze) {
		mode = 2;
		for (i = 0; i < LvAnaFile->Items->Count; i++) {
			if (btnPause->Tag == 0) {
				LvAnaFile->Items->Item[i]->ImageIndex = 0;
				LvAnaFile->Items->Item[i]->Caption = L"";
			} else {
				if (LvAnaFile->Items->Item[i]->ImageIndex != 2) {
					LvAnaFile->Items->Item[i]->ImageIndex = 0;
					LvAnaFile->Items->Item[i]->Caption = L"";
				}
			}
		}
	}

	outDir = (AnsiString)eqParameterPath;
	outDir += "\\";
	_splitpath(outDir.c_str(), drive2, dir2, file2, ext2);
	sprintf(fullpath,"%s%s%s_%02d%s",drive2,dir2,"eq_preview_input",0,".wav");
	sprintf(sss,"Delete:%s",fullpath);
	PRINT_LOG(sss);
	DeleteFile(fullpath);
	sprintf(fullpath,"%s%s%s_%02d%s",drive2,dir2,"eq_preview_input",0,".err");
	sprintf(sss,"Delete:%s",fullpath);
	PRINT_LOG(sss);
	DeleteFile(fullpath);
	for (i = 0;i < 16;i++) {
		outDir = (AnsiString)eqParameterPath;
		outDir += "\\";
		_splitpath(outDir.c_str(), drive2, dir2, file2, ext2);
		sprintf(fullpath,"%s%s%s_%02d%s",drive2,dir2,"eq_preview_fin",i,".wav");
		sprintf(sss,"Delete:%s",fullpath);
		PRINT_LOG(sss);
		DeleteFile(fullpath);
		sprintf(fullpath,"%s%s%s_%02d%s",drive2,dir2,"eq_preview",i,".err");
		sprintf(sss,"Delete:%s",fullpath);
		PRINT_LOG(sss);
		DeleteFile(fullpath);
	}

	btnPause->Tag = 0;

	btnStart->Enabled = false;
	btnStart->Visible = false;
	btnAbort->Visible = true;
	btnPause->Enabled = true;

	btnExit->Enabled = false;
	prgExec->Visible = true;
	pageMain->Enabled = false;

	if (mode == 1) {
		uStr.sprintf(L"[0/%d]", LvFile->Items->Count);
		DragAcceptFiles(LvFile->Handle, false);
		LvFile->Items->Item[0]->Selected = true;
		LvFile->Items->Item[0]->MakeVisible(true);
	} else {
		uStr.sprintf(L"[0/%d]", LvAnaFile->Items->Count);
		DragAcceptFiles(LvAnaFile->Handle, false);
		LvAnaFile->Items->Item[0]->Selected = true;
		LvAnaFile->Items->Item[0]->MakeVisible(true);
	}
	lbl_count->Caption = uStr;
	lbl_count->Visible = true;
	Abort = 0;
	VisibleIndex = 0;

	cmbEQParameter->Tag = cmbEQParameter->ItemIndex;

	CoDefParamIndex = cmbEQParameter->ItemIndex + 1;
	Timer1->Tag = 0;
	Timer1->Enabled = true;
	PRINT_FN_LOG("btnStartClick","End");
}

// ---------------------------------------------------------------------------
int __fastcall TForm1::ExecChildProcess(int index,int play_flag) {
	int i;
	int thread_index;
	int mc_flag;
	int wk_num;
	int dp_item_index;
	int split_accept;
	int mode;
	int file_count;
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
	double vla_nx;
	FILE *fp;
	AnsiString cmd, cmdParam, logParam,defaultParamFile, defaultParameterPath,outDir, fileName;
	AnsiString sTemp;
	AnsiString sInRate;
	AnsiString fromFile, toFile, flacFile, workpath;
	AnsiString AppPath = ExtractFilePath(Application->ExeName);
	AnsiString sStr;
	UnicodeString uStr;
	int bit[4] = {16, 24, 32, 64};
	SOUNDFMT inFmt;
	SSIZE nSample;
	MEMORYSTATUSEX mse;
	PRINT_FN_LOG("ExecChildProcess","Start");

	memset(&mse, 0, sizeof(MEMORYSTATUSEX));
	mse.dwLength = sizeof(MEMORYSTATUSEX);
	if (GlobalMemoryStatusEx(&mse)) {
		if (mse.ullAvailPhys < 700 * 1024 * 1024) {
			int i;
			for (i = 0; i < 10; i++) {
				Sleep(1000);
			}
		}
	}
	mc_flag = 0;
	split_accept = 0;
	if (pageMain->ActivePage == tabUpconv) {
		mode = 1;
		file_count = LvFile->Items->Count;
	} else if (pageMain->ActivePage == tabAnalyze) {
		mode = 2;
		file_count = LvAnaFile->Items->Count;
	}

	if (index < file_count) {
		for (thread_index = 0; thread_index < 1;thread_index++) {
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
		if (thread_index == 1) return -1;
		if (mode == 1) {
			uStr = GetFilePath(1, (wchar_t *)LvFile->Items->Item[index]->Data);
		} else {
			uStr = GetFilePath(1, (wchar_t *)LvAnaFile->Items->Item[index]->Data);
		}
		if (preview_input_filename != "" && FileExists(preview_input_filename)) {
			strcpy(fullpath,preview_input_filename.c_str());
		} else {
			strcpy(fullpath, ((AnsiString)uStr).c_str());
		}
		PLG_InfoAudioData(fullpath, &inFmt, &nSample, NULL);
		if ((AnsiString)inFmt.fmt == "wav" || (AnsiString)inFmt.fmt == "rf64") {
			if ((nSample / inFmt.sample) >= (60 * 10)) split_accept = 1;
		}

		cmdParam = "";
		sTemp = "";
		cmdParam += " -norm:0";

		cmdParam += " -thread:";
		cmdParam += (AnsiString)cmbThread->Text;

		cmdParam += " -fio:";
		cmdParam += cmbFileIO->Items->Strings[cmbFileIO->ItemIndex];

		cmdParam += " -cpu_pri:";
		cmdParam += cmbPriority->ItemIndex;

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

		if (play_flag == 1) {
			// Preview でイコライザの値を仮保存する
			if (PageEQ->ActivePage == EQ_Type1) {
				SaveEQ_Type1(1);
			}
			if (PageEQ->ActivePage == EQ_Type2) {
				SaveEQ_Type2(1);
			}
			if (PageEQ->ActivePage == EQ_Type3) {
				SaveEQ_Type3(1);
			}
			if (PageEQ->ActivePage == EQ_Type4) {
				SaveEQ_Type4(1);
			}
			SoundBuffer[IndexSoundBuffer].offset = track_skip_position->Position;
			if (rdo_preview_rate_44100->Checked == true) {
				cmdParam += " -no_rate_conv:1";
			} else if (rdo_preview_rate_48000->Checked == true) {
				cmdParam += " -no_rate_conv:2";
			} else if (rdo_preview_rate_96000->Checked == true) {
				cmdParam += " -no_rate_conv:3";
			}
			cmdParam += " -w:16";

			if (IndexSoundBuffer >= CountSoundBuffer) {
				IndexSoundBuffer = 0;
			}

			if (preview_input_filename_state_change == 1) {
				preview_input_filename_state_change = 0;
				if (FileExists(preview_input_filename)) {
					DeleteFile(preview_input_filename.c_str());
				}
			}

			PRINT_LOG("ExecChildProcess:1");
			if (preview_input_filename == "") {
				PRINT_LOG("ExecChildProcess:2");
				sStr.printf(" -skip_start_offset:%ld",SoundBuffer[IndexSoundBuffer].offset);
				cmdParam += sStr;
				PRINT_LOG("ExecChildProcess:5");
				sStr.printf(" -conv_test:%d",SoundBuffer[IndexSoundBuffer].sec);
				cmdParam += sStr;
			} else {
				PRINT_LOG("ExecChildProcess:3");
				if (FileExists(preview_input_filename) == false) {
					PRINT_LOG("ExecChildProcess:4");
					sStr.printf(" -skip_start_offset:%ld",SoundBuffer[IndexSoundBuffer].offset);
					cmdParam += sStr;
					PRINT_LOG("ExecChildProcess:5");
					sStr.printf(" -conv_test:%d",SoundBuffer[IndexSoundBuffer].sec);
					cmdParam += sStr;
				}
			}
		} else {
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

			cmdParam += " -no_rate_conv:0";
			if (cmbEncoder->ItemIndex != 0) {
				if (cmbEncoder->Text == L"FLAC") {
					cmdParam += L" -encorder:1";
					_splitpath(toFile.c_str(), drive, dir, file, ext);
					_makepath(fullpath, drive, dir, file, "flac");
					fileName = (AnsiString)fullpath;
					DeleteFile(fileName);
				}
				if (cmbEncoder->Text == L"WavPack") {
					cmdParam += L" -encorder:2";
					_splitpath(toFile.c_str(), drive, dir, file, ext);
					_makepath(fullpath, drive, dir, file, "wv");
					fileName = (AnsiString)fullpath;
					DeleteFile(fileName);
				}
				if (cmbEncoder->Text == L"MP3") {
					cmdParam += L" -encorder:3";
					_splitpath(toFile.c_str(), drive, dir, file, ext);
					_makepath(fullpath, drive, dir, file, "mp3");
					fileName = (AnsiString)fullpath;
					DeleteFile(fileName);
				}
			}
			if (mode == 2) {
				cmdParam += " -addp:0";
			} else {
				if (cmbADDP->ItemIndex == 0) {
					cmdParam += " -addp:0";
				} else {
					cmdParam += " -addp:" +  cmbADDP->Items->Strings[cmbADDP->ItemIndex];
				}
			}
		}

		cmdParam += " -hfa:0";
		cmdParam += " -oversamp:1";
		if (mode == 1) {
			LvFile->Items->Item[index]->Caption = "...";
			LvFile->Items->Item[index]->ImageIndex = 1;
			uStr = GetFilePath(2, (wchar_t *)(LvFile->Items->Item[index]->Data));
			sStr = AnsiString(uStr);
			strcpy(fullpath, sStr.c_str());
			uStr = GetFilePath(1, (wchar_t *)(LvFile->Items->Item[index]->Data));
		} else {
			LvAnaFile->Items->Item[index]->Caption = "...";
			LvAnaFile->Items->Item[index]->ImageIndex = 1;
			uStr = GetFilePath(2, (wchar_t *)(LvAnaFile->Items->Item[index]->Data));
			sStr = AnsiString(uStr);
			strcpy(fullpath, sStr.c_str());
			uStr = GetFilePath(1, (wchar_t *)(LvAnaFile->Items->Item[index]->Data));
		}

		if (preview_input_filename != "" && FileExists(preview_input_filename)) {
			uStr = preview_input_filename;
		}
		fromFile = uStr;
		_splitpath(fullpath, drive, dir, file, ext);
		suffix[0] = '\0';
		suffix_org[0] = '\0';
		if (edtOutputPath->Text == L"Source Directory" && edtSuffix->Text == "") {
			strcat(suffix, "_eq");
		} else if (edtSuffix->Text.Length() > 0) {
			strcat(suffix, "_");
			strcat(suffix, ((AnsiString)edtSuffix->Text).c_str());
		}
		strcpy(suffix_org,suffix);
		strcat(suffix,"_temp");

		strcat(file,suffix_org);

		if (mode == 1) {
			if (btnPlay->Tag == 0) {
				if (edtOutputPath->Text.Length() > 0 && edtOutputPath->Text != L"Source Directory") {
					outDir = (AnsiString)edtOutputPath->Text;
					outDir += "\\";
					_splitpath(outDir.c_str(), drive2, dir2, file2, ext2);
					_makepath(fullpath, drive2, dir2, file, "wav");
				} else {
					_makepath(fullpath, drive, dir, file, "wav");
				}
			} else {
				strcpy(fullpath,SoundBuffer[IndexSoundBuffer].file_name.c_str());
			}
		} else {
			fileName = (AnsiString)eqParameterPath + L"\\eq_analyze.wav";
			strcpy(fullpath,fileName.c_str());
		}

		toFile = (AnsiString)(fullpath);

		// ファイルの削除
		if (btnPlay->Tag == 0) {
			_splitpath(toFile.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file, "wav");
			threadExec[thread_index]->toFile = (AnsiString)fullpath;
			fileName = (AnsiString)fullpath;
			DeleteFile(fileName);
			_splitpath(toFile.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file, "err");
			threadExec[thread_index]->errFile = (AnsiString)fullpath;
			fileName = (AnsiString)fullpath;
			DeleteFile(fileName);
		} else {
			_splitpath(SoundBuffer[IndexSoundBuffer].file_name.c_str(), drive, dir, file, ext);
			_makepath(fullpath, drive, dir, file, "wav");
			threadExec[thread_index]->toFile = (AnsiString)fullpath;
			DeleteFile(fullpath);
			_makepath(fullpath, drive, dir, file, "err");
			threadExec[thread_index]->errFile = (AnsiString)fullpath;
			DeleteFile(fileName);
		}
		if (mode == 1) {
			sStr = cmbEQParameter->Items->Strings[cmbEQParameter->ItemIndex];
		} else {
			sStr = cmbEQ_Analyze->Items->Strings[cmbEQ_Analyze->ItemIndex];
		}

		if (mode == 1) {
			cmdParam += " -norm:";
			if (cmbNormalize->ItemIndex < 2) {
				cmdParam += (AnsiString)cmbNormalize->ItemIndex;
			} else {
				cmdParam += "3";
			}
			if (cmbNormalize->ItemIndex == 2 && cmb_volume_level->Text != L"") {
				sTemp.sprintf(" -volume_per:%d",cmb_volume_level->Text.ToInt());
				cmdParam += sTemp;
			}
		}

		if (sscanf(sStr.c_str(),"%d%*s",&dp_item_index) == 1) {
			defaultParameterPath = ExtractFilePath(Application->ExeName);
			defaultParamFile.printf("%s\\upconv_default_eq.txt",(AnsiString)defaultParameterPath);

			if (btnPlay->Tag == 0) {
				sTemp.sprintf(" -pre_eq:%03d",dp_item_index);
			} else {
				sTemp.sprintf(" -pre_eq:000");
			}
			cmdParam += sTemp;
			if (preview_input_filename == "" || (preview_input_filename != "" && (FileExists(preview_input_filename)))) {
				cmdParam += " -enable_pre_eq:1 ";
			}

			if (mode == 2) {
				if (rdo_Analyze_Phase1->Checked == true) {
					cmdParam += " -enable_eq_analyze:1 ";
				} else if (rdo_Analyze_Phase2->Checked == true) {
					cmdParam += " -enable_eq_analyze:2 ";
				}
			}
		}
		if (cmbEQParameter->ItemIndex >= 0) {
			SoundBuffer[IndexSoundBuffer].status = BUFFER_CONVERT;
			if (rdo_log_enable->Enabled == false) {
				threadExec[thread_index]->cmd.printf("%supconv.exe \"%s\" \"%s\" \"%s\" \"%s\"", AppPath.c_str(),fromFile, toFile, defaultParamFile, cmdParam);
			} else {
				logParam = be_log->Text + L"\\upconv_eq.log";
				threadExec[thread_index]->cmd.printf("%supconv.exe \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"", AppPath.c_str(),fromFile, toFile, defaultParamFile, cmdParam, logParam);
			}
			threadExec[thread_index]->cpu_pri = 0;
			threadExec[thread_index]->Start();
		}
	}
	PRINT_FN_LOG("ExecChildProcess","End");
	return 0;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::EndExec(int index, DWORD exitCode) {
	UnicodeString uStr;
	int i, thread_index;
	int endFlag;
	unsigned short ID;
	int error = true;
	int mode;
	FILE *fp;

	PRINT_FN_LOG("EndExec","Start");
	if (pageMain->ActivePage == tabUpconv) {
		mode = 1;
	} else if (pageMain->ActivePage == tabAnalyze) {
		mode = 2;
	}

	ID = exitCode >> 16;
	exitCode &= 0xFFFF;
	uStr.sprintf(L"EndExec[%d]", index);

	for (thread_index = 0; thread_index < 1;thread_index++) {
		if (threadExec[thread_index] != NULL && threadExec[thread_index]->index == index)
			break;
	}
	if (thread_index == 1) {
		PRINT_LOG("thread_index=1 error exit");
		uStr.sprintf(L"Error");
		return;
	}
	fp = fopen(threadExec[thread_index]->errFile.c_str(), "r");
	if (fp == NULL) {
		PRINT_LOG("error file not found.OK");
		error = false;
	} else {
		PRINT_LOG("error file found. !error!.");
		fclose(fp);
	}

	if (error == false) {
		fp = fopen(threadExec[thread_index]->toFile.c_str(), "r");
		if (fp == NULL) {
			PRINT_LOG("toFile not found. !error!.");
			error = true;
		}
		else {
			PRINT_LOG("toFile found OK.");
			fclose(fp);
		}
	}

	if (Timer4->Enabled == false) {
		if (mode == 1) {
			if (error) {
				PRINT_LOG("EQ File !error!.");
				LvFile->Items->Item[index]->ImageIndex = 3;
				LvFile->Items->Item[index]->Caption = L"Error";
			} else {
				PRINT_LOG("EQ File complated OK.");
				LvFile->Items->Item[index]->ImageIndex = 2;
				LvFile->Items->Item[index]->Caption = L"Ok";
			}
		} else {
			if (error) {
				PRINT_LOG("EQ File !error!.");
				LvAnaFile->Items->Item[index]->ImageIndex = 3;
				LvAnaFile->Items->Item[index]->Caption = L"Error";
			} else {
				PRINT_LOG("EQ File complated OK.");
				LvAnaFile->Items->Item[index]->ImageIndex = 2;
				LvAnaFile->Items->Item[index]->Caption = L"Ok";
			}
		}
	} else {
		LvAnaFile->Items->Item[index]->ImageIndex = 0;
		LvAnaFile->Items->Item[index]->Caption = L"";
	}
	uStr.sprintf(L"index:%02d...end.", index);
	threadExec[thread_index]->flag_finish = 1;
	if (btnPlay->Tag == 0) {
		Timer1->Tag = 0;
	} else {
		Timer3->Tag = 0;
	}

	PRINT_FN_LOG("EndExec","End");
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::UpdateProgress(int index, AnsiString status,
	int percent) {
	int i;
	UnicodeString ustr;

	for (i = 0;i < 1;i++) {
		if (threadExec[i] != NULL && threadExec[i]->Finished == false && threadExec[i]->index == index && threadExec[i]->flag_progress == 1) {
			prgExec->Position = percent;
			lbl_exec_status->Caption = status;
			break;
		}
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	if (pageMain->ActivePage == tabUpconv) {
		Timer1Func();
	} else if (pageMain->ActivePage == tabAnalyze) {
		Timer1AnaFunc();
	}
}
// ---------------------------------------------------------------------------
// 変換部分のタイマー処理
void __fastcall TForm1::Timer1Func(void)
{
	int i,n,count, exec_flag;
	int exec_finish_flag;
	int exec_i;
	FILE *fp;
	char fullpath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[_MAX_FNAME];
	char file2[_MAX_FNAME];
	char ext[_MAX_EXT];
	AnsiString exec_count;
	UnicodeString uStr;
	UnicodeString f1,f2;
	AnsiString    sStr;

	exec_finish_flag = 0;
	if (Timer1->Tag == 0) {
		Timer1->Tag = 1;

		// スレッド終了検出
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL && threadExec[i]->Finished && threadExec[i]->flag_finish) {
				threadExec[i] = NULL;
				exec_finish_flag = 1;
			}
		}
		if (Abort == 0) {
			for (i = 0; i < LvFile->Items->Count; i++) {
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
					if (ExecChildProcess(i,0) == 0)
						count++;
					break;
				}
			}
		}
	}
	exec_flag = 0;
	exec_i = 0;
	if (btnPlay->Tag == 0) {
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
				if (btnPlay->Tag == 0) {
					exec_flag = 1;
				} else if (btnPlay->Tag == 1) {
					if (SoundBuffer[IndexSoundBuffer].status == BUFFER_CONVERT) {
						exec_flag = 1;
					}
				}
			} else if (LvFile->Items->Item[i]->ImageIndex == 2 || LvFile->Items->Item[i]->ImageIndex == 3) {
				exec_i++;
			}
		}
	}
	// プログレスバー用
	for (i = 0; i < 1; i++) {
		if (threadExec[i] != NULL && threadExec[i]->flag_progress == 1) {
			break;
		}
	}
	if (i == 1) {
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL && threadExec[i]->flag_progress == 0) {
				threadExec[i]->flag_progress = 1;
				break;
			}
		}
	}

	exec_count.sprintf("[%d/%d]", exec_i, LvFile->Items->Count);
	lbl_count->Caption = exec_count;

	if (Abort) {
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL) {
				break;
			}
		}
		if (i == 1) {
			exec_flag = 0;
		}
	}

	if (exec_flag == 0) {
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
		pageMain->Enabled = true;
		DragAcceptFiles(LvFile->Handle, true);
		LvFile->Items->Item[0]->Selected = true;
		LvFile->Items->Item[0]->MakeVisible(true);
		Abort = 0;
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::Timer1AnaFunc(void)
{
	int i,n,count, exec_flag;
	int exec_finish_flag;
	int exec_i;
	FILE *fp;
	char fullpath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[_MAX_FNAME];
	char file2[_MAX_FNAME];
	char ext[_MAX_EXT];
	AnsiString exec_count;
	UnicodeString uStr;
	UnicodeString f1,f2;
	AnsiString    sStr;

	exec_finish_flag = 0;
	if (Timer1->Tag == 0) {
		Timer1->Tag = 1;

		// スレッド終了検出
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL && threadExec[i]->Finished && threadExec[i]->flag_finish) {
				threadExec[i] = NULL;
				exec_finish_flag = 1;
			}
		}
		if (btnPlay->Tag == 0 && Abort == 0) {
			for (i = 0; i < LvAnaFile->Items->Count; i++) {
				if (LvAnaFile->Items->Item[i]->ImageIndex < 2)
					break;
			}
			if (i < LvAnaFile->Items->Count && i != VisibleIndex) {
				LvAnaFile->Items->Item[LvAnaFile->Items->Count - 1]->Selected = true;
				LvAnaFile->Items->Item[LvAnaFile->Items->Count - 1]->MakeVisible(true);
				Application->ProcessMessages();
				LvAnaFile->Items->Item[i]->Selected = true;
				LvAnaFile->Items->Item[i]->MakeVisible(true);
			}
			count = 0;
			VisibleIndex = i;
			for (i = 0; i < LvAnaFile->Items->Count; i++) {
				if (LvAnaFile->Items->Item[i]->ImageIndex == 0) {
					if (ExecChildProcess(i,0) == 0)
						count++;
					break;
				}
			}
		}
	}
	exec_flag = 0;
	exec_i = 0;
	for (i = 0; i < LvAnaFile->Items->Count; i++) {
		if (LvAnaFile->Items->Item[i]->ImageIndex == 1) {
			exec_flag = 1;
			uStr = L"" + LvAnaFile->Items->Item[i]->Caption.SubString(0, 2);
			if (uStr == L"..") {
				uStr = L">..";
			}
			else if (uStr == L">.") {
				uStr = L".>.";
			}
			else if (uStr == L".>") {
				uStr = L"..>";
			}
			LvAnaFile->Items->Item[i]->Caption = uStr;
		} else if (LvAnaFile->Items->Item[i]->ImageIndex == 0) {
			exec_flag = 1;
		} else if (LvAnaFile->Items->Item[i]->ImageIndex == 2 || LvAnaFile->Items->Item[i]->ImageIndex == 3) {
			exec_i++;
		}
	}
	// プログレスバー用
	for (i = 0; i < 1; i++) {
		if (threadExec[i] != NULL && threadExec[i]->flag_progress == 1) {
			break;
		}
	}
	if (i == 1) {
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL && threadExec[i]->flag_progress == 0) {
				threadExec[i]->flag_progress = 1;
				break;
			}
		}
	}

	exec_count.sprintf("[%d/%d]", exec_i, LvFile->Items->Count);
	lbl_count->Caption = exec_count;

	if (Abort) {
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL) {
				break;
			}
		}
		if (i == 1) {
			exec_flag = 0;
		}
	}

	if (exec_flag == 0) {
		if (exec_flag == 0) {
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
			pageMain->Enabled = true;
			DragAcceptFiles(LvFile->Handle, true);
			LvAnaFile->Items->Item[0]->Selected = true;
			LvAnaFile->Items->Item[0]->MakeVisible(true);
			Abort = 0;
			if (rdo_Analyze_Phase1->Checked == true) {
				LvAnaFile->Items->Clear();
				rdo_Analyze_Phase2->Checked = true;
			} else if (rdo_Analyze_Phase2->Checked == true) {
				LvAnaFile->Items->Clear();
				rdo_Analyze_Phase3->Checked = true;
				btn_EQ_Set->Enabled = true;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
	if (pageMain->ActivePage == tabUpconv) {
		Timer2Func();
	} else if (pageMain->ActivePage == tabAnalyze) {
		Timer2AnaFunc();
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer2Func(void)
{
	if (btnPlay->Tag == 1) {
		if (SelectFileIndex < LvFile->Items->Count) {
			if (SoundBuffer[IndexPlayBuffer].status == BUFFER_READY) {
				btnPlay->Caption = L"Stop";
				PlaySound();
			} else if (SoundBuffer[IndexPlayBuffer].status == BUFFER_PLAY_DONE) {
				if (chk_play_loop->Checked == true) {
					PlaySound();
				} else {
					StopSound();
					Timer2->Enabled = false;
					btnPlay->Caption = L"Play";
					btnPlay->Tag = 0;
				}
			}
		}
	} else if (btnPlay->Tag == -1) {
		StopSound();
		Timer2->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer2AnaFunc(void)
{
	int i;
	for (i = 0; i < LvAnaFile->Items->Count;i++) {
		if (LvFile->Selected == LvAnaFile->Items->Item[i]) {
			break;
		}
	}
	if (i < LvAnaFile->Items->Count) {
		if (SoundBuffer[IndexPlayBuffer].status == BUFFER_READY) {
			btnPlay->Caption = L"Stop";
			PlaySound();
		} else if (SoundBuffer[IndexPlayBuffer].status == BUFFER_PLAY_DONE) {
			if (chk_play_loop->Checked == true) {
				PlaySound();
			} else {
				StopSound();
				Timer2->Enabled = false;
				btnPlay->Caption = L"Play";
				btnPlay->Tag = 0;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer3Timer(TObject *Sender)
{
	int i,n,count, exec_flag;
	int exec_finish_flag;
	int exec_i;
	FILE *fp;
	char fullpath[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[_MAX_FNAME];
	char file2[_MAX_FNAME];
	char ext[_MAX_EXT];
	AnsiString exec_count;
	UnicodeString uStr;
	UnicodeString f1,f2,f3,f4;
	AnsiString    sStr;
	int index;

	exec_finish_flag = 0;
	if (Timer3->Tag == 0) {
		Timer3->Tag = 1;

		// スレッド終了検出
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL && threadExec[i]->Finished && threadExec[i]->flag_finish) {
				threadExec[i] = NULL;
				exec_finish_flag = 1;
			}
		}
	}

	exec_flag = 0;
	exec_i = 0;
	if (btnPlay->Tag == 1) {
		if (SelectFileIndex < LvFile->Items->Count) {
			if (LvFile->Items->Item[SelectFileIndex]->ImageIndex == 1) {
				exec_flag = 1;
				uStr = L"" + LvFile->Items->Item[SelectFileIndex]->Caption.SubString(0, 2);
				if (uStr == L"..") {
					uStr = L">..";
				}
				else if (uStr == L">.") {
					uStr = L".>.";
				}
				else if (uStr == L".>") {
					uStr = L"..>";
				}
				LvFile->Items->Item[SelectFileIndex]->Caption = uStr;
			} else if (LvFile->Items->Item[SelectFileIndex]->ImageIndex == 0) {
				if (SoundBuffer[IndexSoundBuffer].status == BUFFER_CONVERT) {
					exec_flag = 1;
				}
			}
		}
	}
	// プログレスバー用
	for (i = 0; i < 1; i++) {
		if (threadExec[i] != NULL && threadExec[i]->flag_progress == 1) {
			break;
		}
	}
	if (i == 1) {
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL && threadExec[i]->flag_progress == 0) {
				threadExec[i]->flag_progress = 1;
				break;
			}
		}
	}

	if (Abort) {
		for (i = 0; i < 1; i++) {
			if (threadExec[i] != NULL) {
				break;
			}
		}
		if (i == 1) {
			exec_flag = 0;
		}
	}

	if (exec_flag == 0) {
		if (btnPlay->Tag == 1) {
			if (SelectFileIndex < LvFile->Items->Count) {
				if (LvFile->Items->Item[SelectFileIndex]->ImageIndex == 0) {
					if (SoundBuffer[IndexSoundBuffer].status == BUFFER_FREE) {
						ExecChildProcess(SelectFileIndex,1);
						exec_flag = 1;
					}
				} else if (LvFile->Items->Item[SelectFileIndex]->ImageIndex == 2) {
					int buffer_ready = 0;
					preview_err_remain = 3;
					if (SoundBuffer[IndexSoundBuffer].status == BUFFER_CONVERT) {
						f1 = (UnicodeString)SoundBuffer[IndexSoundBuffer].file_name;
						f2 = (UnicodeString)SoundBuffer[IndexSoundBuffer].fin_file_name;
						DeleteFile(f2.c_str());
						MoveFile(f1.c_str(),f2.c_str());
						if (preview_input_filename != "" && FileExists(preview_input_filename) == false) {
							f3 = preview_input_filename + ".tmp";
							f4 = preview_input_filename;
							CopyFile(f2.c_str(),f3.c_str(),FALSE);
							MoveFile(f3.c_str(),f4.c_str());
							Sleep(2000);
						}
						if (FileExists(preview_input_filename) == false) {
							Sleep(2000);
						}
						if (FileExists(preview_input_filename) == false) {
							Sleep(2000);
						}
						ReadBuffer(IndexSoundBuffer,&buffer_ready);
						if (buffer_ready == 1) {
							IndexSoundBuffer++;
							if (IndexSoundBuffer >= CountSoundBuffer) {
								IndexSoundBuffer = 0;
							}
							if (chk_play_loop->Checked == true) {
								ExecChildProcess(SelectFileIndex,1);
							}
							exec_flag = 1;
							if (Timer2->Enabled == false) {
								Timer2->Enabled = true;
							}
						}
					}
				} else if (LvFile->Items->Item[SelectFileIndex]->ImageIndex == 3) {
					preview_err_remain--;
					if (preview_err_remain <= 0) {
						PRINT_LOG("Stop");
						for (index = 0; index < 1; index++) {
							if (threadExec[index] != NULL)
								threadExec[index]->AbortExec();
							}
						}
						btnPlay->Tag = -1;
						btnPlay->Enabled = false;
						Timer4->Enabled = true;
					} else {
						if (chk_play_loop->Checked == true) {
							ExecChildProcess(SelectFileIndex,1);
						}
						exec_flag = 1;
						if (Timer2->Enabled == false) {
						Timer2->Enabled = true;
					}
				}
			}
		} else if (btnPlay->Tag == -1) {
			btnPlay->Caption = L"Play";
			btnPlay->Tag = 0;
			Timer2->Enabled = false;
			exec_flag = 0;
		}
		if (exec_flag == 0) {
			if (btnPlay->Tag != 1 || (btnPlay->Tag == 1 && Timer2->Enabled == false && chk_play_loop->Checked == false)) {
				btnPlay->Caption = L"Play";
				btnPlay->Tag = 0;
			} else {
				return;
			}
			Timer3->Enabled = false;
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
			pageMain->Enabled = true;
			DragAcceptFiles(LvFile->Handle, true);
			LvFile->Items->Item[SelectFileIndex]->Selected = true;
			LvFile->SetFocus();

			for (i = 0;i < LvFile->Items->Count;i++) {
				LvFile->Items->Item[i]->ImageIndex = 0;
			}
			Abort = 0;
		}
	}
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnReloadClick(TObject *Sender) {
#if 0
	int i;
	int index_1 = cmbEQParameter->ItemIndex;
	int n;
	FILE *fp;
	AnsiString sStr;
	UnicodeString uStr;

	PRINT_FN_LOG("btnReloadClick","Start");

	cmbEQParameter->Items->Clear();
	edtDefaultParam->Text = L"";
	for (i = 1; i <= 99; i++) {
		ReloadDefaultParameter(i, -1);
	}
	for (i = 0;i < list_dp_active->Items->Count;i++) {
		uStr = list_dp_active->Items->Strings[i];
		cmbEQParameter->Items->Add(uStr);
	}
	if (btnReload->Tag == 0) {
		// リロード前に選択されていたデフォルトパラメーターを選択しなおす
		if (index_1 < cmbEQParameter->Items->Count) {
			cmbEQParameter->ItemIndex = index_1;
		}
	} else {
		cmbDefaultParameter->ItemIndex = 0;
	}
	uStr.printf(L"%s\\upconv_eq_%02d.txt",eqParameterPath, index_1 + 1);
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
		cmbEQParameter->TextHint = cmbDefaultParameter->Text;
	}
	btnReload->Tag = 0;
	PRINT_FN_LOG("btnReloadClick","End");

	#endif
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::btnInfoClick(TObject *Sender) {
	UnicodeString uStr;
	TListItem *item;
	PRINT_FN_LOG("btnInfoClick","Start");

	if (LvFile->Selected != NULL) {
		item = LvFile->Selected;
		if (item->Data) {
			uStr = GetFilePath(1, (wchar_t *)item->Data);
			Form5->filename = (AnsiString)(uStr);
			Form5->ShowModal();
		}
	}
	PRINT_FN_LOG("btnInfoClick","End");
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnAbortClick(TObject *Sender) {
	int index;
	Abort = 1;
	PRINT_FN_LOG("btnAbortClick","Start");
	btnAbort->Enabled = false;
	btnPause->Enabled = false;
	for (index = 0; index < 1; index++) {
		if (threadExec[index] != NULL)
			threadExec[index]->AbortExec();
	}
	PRINT_FN_LOG("btnAbortClick","End");
}
// ---------------------------------------------------------------------------


void __fastcall TForm1::chk_change_default_parameterClick(TObject *Sender)
{
	PRINT_FN_LOG("chk_change_default_parameterClick","Start");
	lbl_co->Caption = L"";
	PRINT_FN_LOG("chk_change_default_parameterClick","End");
}
// ---------------------------------------------------------------------------
void __fastcall TForm1::btnDPClick(TObject *Sender)
{
	PRINT_FN_LOG("btnDPClick","Start");
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
	PRINT_FN_LOG("btnDPClick","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnPauseClick(TObject *Sender)
{
	PRINT_FN_LOG("btnPauseClick","Start");
	btnPause->Tag = 1;
	btnAbort->Click();
	PRINT_FN_LOG("btnPauseClick","End");
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
	PRINT_FN_LOG("LvFileColumnClick","Start");
	if (btnPause->Tag == 1) {
		if (Column->ImageIndex == 2) {
			Column->Tag = Column->ImageIndex;
			Column->ImageIndex = 0;
		} else if (Column->Tag == 1) {
			Column->ImageIndex = Column->Tag;
			Column->Tag = 0;
		}
	}
	PRINT_FN_LOG("LvFileColumnClick","End");
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btnOK_ClearClick(TObject *Sender)
{
	TListItem *item;
	PRINT_FN_LOG("btnOK_ClearClick","Start");
	if (LvFile->SelCount > 0) {
		if (LvFile->Selected != NULL) {
			item = LvFile->Selected;
			item->ImageIndex = 0;
			item->Caption    = "";
		}
	}
	PRINT_FN_LOG("btnOK_ClearClick","End");
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btn_eq_saveClick(TObject *Sender)
{
	int i,index;
	char *p;
	AnsiString sTitle;
	AnsiString sStr;

	PRINT_FN_LOG("btn_eq_saveClick","Start");

	if (PageEQ->ActivePage == EQ_Type1) {
		SaveEQ_Type1(0);
	}
	if (PageEQ->ActivePage == EQ_Type2) {
		SaveEQ_Type2(0);
	}
	if (PageEQ->ActivePage == EQ_Type3) {
		SaveEQ_Type3(0);
	}
	if (PageEQ->ActivePage == EQ_Type4) {
		SaveEQ_Type4(0);
	}
	index = cmbEQParameter->ItemIndex;
	sTitle = edt_eq_title->Text;
	sStr.printf("%03d.%s",index + 1,sTitle.c_str());
	cmbEQParameter->Items->Strings[index] = sStr;
	cmbEQParameter->ItemIndex = index;
	if (index + 1 == cmbEQParameter->Items->Count) {
		sStr.printf("%03d.%s",index + 2,"EQ.Sample");
		cmbEQParameter->Items->Add(sStr);
		edtParameter->Lines->Add(L"");
		cmbEQ_Analyze->Items->Clear();
		for (i = 0;i + 1 < cmbEQParameter->Items->Count;i++) {
			cmbEQ_Analyze->Items->Add(cmbEQParameter->Items->Strings[i]);
		}
	}
	if (cmbEQ_Analyze->ItemIndex < 0) {
		cmbEQ_Analyze->ItemIndex = 0;
	}
	SetPositionToTag();
	PRINT_FN_LOG("btn_eq_saveClick","End");
}

//---------------------------------------------------------------------------
void __fastcall TForm1::SaveEQ_Type1(int test_mode)
{
	FILE *fp;
	int i,j,k,x,hz;
	int disable_spline;
	char *p,work[4096],param[4096],title[4096];
	int  eq_val[110];
	long eq_hz[]={20,25,31,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6300,8000,10000,12500,16000,20000,25000,31500,40000,50000,52000,53000,54000,55000,56000};
	double per1[]={0.1995,0.2238,0.2511,0.2818,0.3162,0.3548,0.3981,0.4466,0.5011,0.5623,0.6309,0.7079,0.7943,0.8912,1.0,1.122,1.2589,1.4125,1.5848,1.7782,1.9952,2.2387,2.511,2.8183,3.1622,3.5481,3.981,4.4668,5.0118};
	double per2[]={0.0398,0.0501,0.0630,0.0794,0.1,0.1258,0.1584,0.1995,0.2511,0.3162,0.3981,0.5011,0.6309,0.7943,1.0,1.2589,1.5848,1.9952,2.5118,3.1622,3.9810,5.0118,6.3095,7.9432,10,12.5892,15.8489,19.9526,25.1188};
	double *per;
	PT2D pt2d[51];
	double h[50];
	double b[50];
	double d[50];
	double g[50];
	double u[50];
	double r[51];
	double y;
	double v;

	int start_index,start_len;
	int max_diff;
	UnicodeString uFileName;
	UnicodeString eqParam1,eqParam2,eqParam3,eqParam4;
	AnsiString sStr;

	PRINT_FN_LOG("SaveEQ_Type1","Start");
	if (test_mode == 0) {
		uFileName.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,cmbEQParameter->ItemIndex + 1);
	} else {
		uFileName.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,0);
	}
	disable_spline = 0;
	max_diff = 0;
	fp = fopen(((AnsiString)uFileName).c_str(),"wt");
	if (fp != NULL) {
		// EQ(Type3)
		eq_val[0]  = TB_3_1->Position * -1;
		eq_val[1]  = TB_3_2->Position * -1;
		eq_val[2]  = TB_3_3->Position * -1;
		eq_val[3]  = TB_3_4->Position * -1;
		eq_val[4]  = TB_3_5->Position * -1;
		eq_val[5]  = TB_3_6->Position * -1;
		eq_val[6]  = TB_3_7->Position * -1;
		eq_val[7]  = TB_3_8->Position * -1;
		eq_val[8]  = TB_3_9->Position * -1;
		eq_val[9]  = TB_3_10->Position * -1;
		eq_val[10] = TB_3_11->Position * -1;
		eq_val[11] = TB_3_12->Position * -1;
		eq_val[12] = TB_3_13->Position * -1;
		eq_val[13] = TB_3_14->Position * -1;
		eq_val[14] = TB_3_15->Position * -1;
		eq_val[15] = TB_3_16->Position * -1;
		eq_val[16] = TB_3_17->Position * -1;
		eq_val[17] = TB_3_18->Position * -1;
		eq_val[18] = TB_3_19->Position * -1;
		eq_val[19] = TB_3_20->Position * -1;
		eq_val[20] = TB_3_21->Position * -1;
		eq_val[21] = TB_3_22->Position * -1;
		eq_val[22] = TB_3_23->Position * -1;
		eq_val[23] = TB_3_24->Position * -1;
		eq_val[24] = TB_3_25->Position * -1;
		eq_val[25] = TB_3_26->Position * -1;
		eq_val[26] = TB_3_27->Position * -1;
		eq_val[27] = TB_3_28->Position * -1;
		eq_val[28] = TB_3_29->Position * -1;
		eq_val[29] = TB_3_30->Position * -1;
		eq_val[30] = TB_3_31->Position * -1;
		eq_val[31] = TB_3_32->Position * -1;
		eq_val[32] = TB_3_33->Position * -1;
		eq_val[33] = TB_3_34->Position * -1;
		eq_val[34] = TB_3_35->Position * -1;
		eq_val[35] = TB_3_36->Position * -1;
		eq_val[36] = TB_3_37->Position * -1;
		eq_val[37] = TB_3_38->Position * -1;
		eq_val[38] = TB_3_39->Position * -1;
		eq_val[39] = TB_3_40->Position * -1;
		eq_val[40] = TB_3_41->Position * -1;
		eq_val[41] = TB_3_42->Position * -1;
		eq_val[42] = TB_3_43->Position * -1;
		eq_val[43] = TB_3_44->Position * -1;
		eq_val[44] = TB_3_45->Position * -1;
		eq_val[45] = TB_3_46->Position * -1;
		eq_val[46] = TB_3_47->Position * -1;
		eq_val[47] = TB_3_48->Position * -1;
		eq_val[48] = TB_3_49->Position * -1;
		eq_val[49] = TB_3_50->Position * -1;
		eq_val[50] = TB_3_51->Position * -1;
		eq_val[51] = TB_3_52->Position * -1;
		eq_val[52] = TB_3_53->Position * -1;
		eq_val[53] = TB_3_54->Position * -1;
		eq_val[54] = TB_3_55->Position * -1;
		eq_val[55] = TB_3_56->Position * -1;
		eq_val[56] = TB_3_57->Position * -1;
		eq_val[57] = TB_3_58->Position * -1;
		eq_val[58] = TB_3_59->Position * -1;
		eq_val[59] = TB_3_60->Position * -1;
		eq_val[60] = TB_3_61->Position * -1;
		eq_val[61] = TB_3_62->Position * -1;
		eq_val[62] = TB_3_63->Position * -1;
		eq_val[63] = TB_3_64->Position * -1;
		eq_val[64] = TB_3_65->Position * -1;
		eq_val[65] = TB_3_66->Position * -1;
		eq_val[66] = TB_3_67->Position * -1;
		eq_val[67] = TB_3_68->Position * -1;
		eq_val[68] = TB_3_69->Position * -1;
		eq_val[69] = TB_3_70->Position * -1;
		eq_val[70] = TB_3_71->Position * -1;
		eq_val[71] = TB_3_72->Position * -1;
		eq_val[72] = TB_3_73->Position * -1;
		eq_val[73] = TB_3_74->Position * -1;
		eq_val[74] = TB_3_75->Position * -1;
		eq_val[75] = TB_3_76->Position * -1;
		eq_val[76] = TB_3_77->Position * -1;
		eq_val[77] = TB_3_78->Position * -1;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 78;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type3_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}
		eqParam3 = (UnicodeString)param;

		// EQ(Type2)
		eq_val[0]  = TB_2_1->Position * -1;
		eq_val[1]  = TB_2_2->Position * -1;
		eq_val[2]  = TB_2_3->Position * -1;
		eq_val[3]  = TB_2_4->Position * -1;
		eq_val[4]  = TB_2_5->Position * -1;
		eq_val[5]  = TB_2_6->Position * -1;
		eq_val[6]  = TB_2_7->Position * -1;
		eq_val[7]  = TB_2_8->Position * -1;
		eq_val[8]  = TB_2_9->Position * -1;
		eq_val[9]  = TB_2_10->Position * -1;
		eq_val[10] = TB_2_11->Position * -1;
		eq_val[11] = TB_2_12->Position * -1;
		eq_val[12] = TB_2_13->Position * -1;
		eq_val[13] = TB_2_14->Position * -1;
		eq_val[14] = TB_2_15->Position * -1;
		eq_val[15] = TB_2_16->Position * -1;
		eq_val[16] = TB_2_17->Position * -1;
		eq_val[17] = TB_2_18->Position * -1;
		eq_val[18] = TB_2_19->Position * -1;
		eq_val[19] = TB_2_20->Position * -1;
		eq_val[20] = TB_2_21->Position * -1;
		eq_val[21] = TB_2_22->Position * -1;
		eq_val[22] = TB_2_23->Position * -1;
		eq_val[23] = TB_2_24->Position * -1;
		eq_val[24] = TB_2_25->Position * -1;
		eq_val[25] = TB_2_26->Position * -1;
		eq_val[26] = TB_2_27->Position * -1;
		eq_val[27] = TB_2_28->Position * -1;
		eq_val[28] = TB_2_29->Position * -1;
		eq_val[29] = TB_2_30->Position * -1;
		eq_val[30] = TB_2_31->Position * -1;
		eq_val[31] = TB_2_32->Position * -1;
		eq_val[32] = TB_2_33->Position * -1;
		eq_val[33] = TB_2_34->Position * -1;
		eq_val[34] = TB_2_35->Position * -1;
		eq_val[35] = TB_2_36->Position * -1;
		eq_val[36] = TB_2_37->Position * -1;
		eq_val[37] = TB_2_38->Position * -1;
		eq_val[38] = TB_2_39->Position * -1;
		eq_val[39] = TB_2_40->Position * -1;
		eq_val[40] = TB_2_41->Position * -1;
		eq_val[41] = TB_2_42->Position * -1;
		eq_val[42] = TB_2_43->Position * -1;
		eq_val[43] = TB_2_44->Position * -1;
		eq_val[44] = TB_2_45->Position * -1;
		eq_val[45] = TB_2_46->Position * -1;
		eq_val[46] = TB_2_47->Position * -1;
		eq_val[47] = TB_2_48->Position * -1;
		eq_val[48] = TB_2_49->Position * -1;
		eq_val[49] = TB_2_50->Position * -1;
		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 50;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type2_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}
		eqParam2 = (UnicodeString)param;

		// EQ(Type4)
		eq_val[0]  = TB_4_1->Position * -1;
		eq_val[1]  = TB_4_2->Position * -1;
		eq_val[2]  = TB_4_3->Position * -1;
		eq_val[3]  = TB_4_4->Position * -1;
		eq_val[4]  = TB_4_5->Position * -1;
		eq_val[5]  = TB_4_6->Position * -1;
		eq_val[6]  = TB_4_7->Position * -1;
		eq_val[7]  = TB_4_8->Position * -1;
		eq_val[8]  = TB_4_9->Position * -1;
		eq_val[9]  = TB_4_10->Position * -1;
		eq_val[10] = TB_4_11->Position * -1;
		eq_val[11] = TB_4_12->Position * -1;
		eq_val[12] = TB_4_13->Position * -1;
		eq_val[13] = TB_4_14->Position * -1;
		eq_val[14] = TB_4_15->Position * -1;
		eq_val[15] = TB_4_16->Position * -1;
		eq_val[16] = TB_4_17->Position * -1;
		eq_val[17] = TB_4_18->Position * -1;
		eq_val[18] = TB_4_19->Position * -1;
		eq_val[19] = TB_4_20->Position * -1;
		eq_val[20] = TB_4_21->Position * -1;
		eq_val[21] = TB_4_22->Position * -1;
		eq_val[22] = TB_4_23->Position * -1;
		eq_val[23] = TB_4_24->Position * -1;
		eq_val[24] = TB_4_25->Position * -1;
		eq_val[25] = TB_4_26->Position * -1;
		eq_val[26] = TB_4_27->Position * -1;
		eq_val[27] = TB_4_28->Position * -1;
		eq_val[28] = TB_4_29->Position * -1;
		eq_val[29] = TB_4_30->Position * -1;
		eq_val[30] = TB_4_31->Position * -1;
		eq_val[31] = TB_4_32->Position * -1;
		eq_val[32] = TB_4_33->Position * -1;
		eq_val[33] = TB_4_34->Position * -1;
		eq_val[34] = TB_4_35->Position * -1;
		eq_val[35] = TB_4_36->Position * -1;
		eq_val[36] = TB_4_37->Position * -1;
		eq_val[37] = TB_4_38->Position * -1;
		eq_val[38] = TB_4_39->Position * -1;
		eq_val[39] = TB_4_40->Position * -1;
		eq_val[40] = TB_4_41->Position * -1;
		eq_val[41] = TB_4_42->Position * -1;
		eq_val[42] = TB_4_43->Position * -1;
		eq_val[43] = TB_4_44->Position * -1;
		eq_val[44] = TB_4_45->Position * -1;
		eq_val[45] = TB_4_46->Position * -1;
		eq_val[46] = TB_4_47->Position * -1;
		eq_val[47] = TB_4_48->Position * -1;
		eq_val[48] = TB_4_49->Position * -1;
		eq_val[49] = TB_4_50->Position * -1;
		eq_val[50] = TB_4_51->Position * -1;
		eq_val[51] = TB_4_52->Position * -1;
		eq_val[52] = TB_4_53->Position * -1;
		eq_val[53] = TB_4_54->Position * -1;
		eq_val[54] = TB_4_55->Position * -1;
		eq_val[55] = TB_4_56->Position * -1;
		eq_val[56] = TB_4_57->Position * -1;
		eq_val[57] = TB_4_58->Position * -1;
		eq_val[58] = TB_4_59->Position * -1;
		eq_val[59] = TB_4_60->Position * -1;
		eq_val[60] = TB_4_61->Position * -1;
		eq_val[61] = TB_4_62->Position * -1;
		eq_val[62] = TB_4_63->Position * -1;
		eq_val[63] = TB_4_64->Position * -1;
		eq_val[64] = TB_4_65->Position * -1;
		eq_val[65] = TB_4_66->Position * -1;
		eq_val[66] = TB_4_67->Position * -1;
		eq_val[67] = TB_4_68->Position * -1;
		eq_val[68] = TB_4_69->Position * -1;
		eq_val[69] = TB_4_70->Position * -1;
		eq_val[70] = TB_4_71->Position * -1;
		eq_val[71] = TB_4_72->Position * -1;
		eq_val[72] = TB_4_73->Position * -1;
		eq_val[73] = TB_4_74->Position * -1;
		eq_val[74] = TB_4_75->Position * -1;
		eq_val[75] = TB_4_76->Position * -1;
		eq_val[76] = TB_4_77->Position * -1;
		eq_val[77] = TB_4_78->Position * -1;
		eq_val[78] = TB_4_79->Position * -1;
		eq_val[79] = TB_4_80->Position * -1;
		eq_val[80] = TB_4_81->Position * -1;
		eq_val[81] = TB_4_82->Position * -1;
		eq_val[82] = TB_4_83->Position * -1;
		eq_val[83] = TB_4_84->Position * -1;
		eq_val[84] = TB_4_85->Position * -1;
		eq_val[85] = TB_4_86->Position * -1;
		eq_val[86] = TB_4_87->Position * -1;
		eq_val[87] = TB_4_88->Position * -1;
		eq_val[88] = TB_4_89->Position * -1;
		eq_val[89] = TB_4_90->Position * -1;
		eq_val[90] = TB_4_91->Position * -1;
		eq_val[91] = TB_4_92->Position * -1;
		eq_val[92] = TB_4_93->Position * -1;
		eq_val[93] = TB_4_94->Position * -1;
		eq_val[94] = TB_4_95->Position * -1;
		eq_val[95] = TB_4_96->Position * -1;
		eq_val[96] = TB_4_97->Position * -1;
		eq_val[97] = TB_4_98->Position * -1;

		// 指定周波数のカットオフ対応
		if (TB_4_1_Cutoff->Checked == true) eq_val[0] = -99;
		if (TB_4_2_Cutoff->Checked == true) eq_val[1] = -99;
		if (TB_4_3_Cutoff->Checked == true) eq_val[2] = -99;
		if (TB_4_4_Cutoff->Checked == true) eq_val[3] = -99;
		if (TB_4_5_Cutoff->Checked == true) eq_val[4] = -99;
		if (TB_4_6_Cutoff->Checked == true) eq_val[5] = -99;
		if (TB_4_7_Cutoff->Checked == true) eq_val[6] = -99;
		if (TB_4_8_Cutoff->Checked == true) eq_val[7] = -99;
		if (TB_4_9_Cutoff->Checked == true) eq_val[8] = -99;
		if (TB_4_10_Cutoff->Checked == true) eq_val[9] = -99;
		if (TB_4_11_Cutoff->Checked == true) eq_val[10] = -99;
		if (TB_4_12_Cutoff->Checked == true) eq_val[11] = -99;
		if (TB_4_13_Cutoff->Checked == true) eq_val[12] = -99;
		if (TB_4_36_Cutoff->Checked == true) eq_val[35] = -99;
		if (TB_4_37_Cutoff->Checked == true) eq_val[36] = -99;
		if (TB_4_38_Cutoff->Checked == true) eq_val[37] = -99;
		if (TB_4_39_Cutoff->Checked == true) eq_val[38] = -99;
		if (TB_4_40_Cutoff->Checked == true) eq_val[39] = -99;
		if (TB_4_41_Cutoff->Checked == true) eq_val[40] = -99;
		if (TB_4_42_Cutoff->Checked == true) eq_val[41] = -99;
		if (TB_4_43_Cutoff->Checked == true) eq_val[42] = -99;
		if (TB_4_44_Cutoff->Checked == true) eq_val[43] = -99;
		if (TB_4_45_Cutoff->Checked == true) eq_val[44] = -99;
		if (TB_4_46_Cutoff->Checked == true) eq_val[45] = -99;
		if (TB_4_47_Cutoff->Checked == true) eq_val[46] = -99;
		if (TB_4_48_Cutoff->Checked == true) eq_val[47] = -99;
		if (TB_4_49_Cutoff->Checked == true) eq_val[48] = -99;
		if (TB_4_50_Cutoff->Checked == true) eq_val[49] = -99;
		if (TB_4_51_Cutoff->Checked == true) eq_val[50] = -99;
		if (TB_4_52_Cutoff->Checked == true) eq_val[51] = -99;
		if (TB_4_53_Cutoff->Checked == true) eq_val[52] = -99;
		if (TB_4_54_Cutoff->Checked == true) eq_val[53] = -99;
		if (TB_4_55_Cutoff->Checked == true) eq_val[54] = -99;
		if (TB_4_56_Cutoff->Checked == true) eq_val[55] = -99;
		if (TB_4_57_Cutoff->Checked == true) eq_val[56] = -99;
		if (TB_4_58_Cutoff->Checked == true) eq_val[57] = -99;
		if (TB_4_59_Cutoff->Checked == true) eq_val[58] = -99;
		if (TB_4_60_Cutoff->Checked == true) eq_val[59] = -99;
		if (TB_4_61_Cutoff->Checked == true) eq_val[60] = -99;
		if (TB_4_62_Cutoff->Checked == true) eq_val[61] = -99;
		if (TB_4_63_Cutoff->Checked == true) eq_val[62] = -99;
		if (TB_4_64_Cutoff->Checked == true) eq_val[63] = -99;
		if (TB_4_65_Cutoff->Checked == true) eq_val[64] = -99;
		if (TB_4_66_Cutoff->Checked == true) eq_val[65] = -99;
		if (TB_4_67_Cutoff->Checked == true) eq_val[66] = -99;
		if (TB_4_68_Cutoff->Checked == true) eq_val[67] = -99;
		if (TB_4_69_Cutoff->Checked == true) eq_val[68] = -99;
		if (TB_4_70_Cutoff->Checked == true) eq_val[69] = -99;
		if (TB_4_71_Cutoff->Checked == true) eq_val[70] = -99;
		if (TB_4_72_Cutoff->Checked == true) eq_val[71] = -99;
		if (TB_4_73_Cutoff->Checked == true) eq_val[72] = -99;
		if (TB_4_74_Cutoff->Checked == true) eq_val[73] = -99;
		if (TB_4_75_Cutoff->Checked == true) eq_val[74] = -99;
		if (TB_4_76_Cutoff->Checked == true) eq_val[75] = -99;
		if (TB_4_77_Cutoff->Checked == true) eq_val[76] = -99;
		if (TB_4_78_Cutoff->Checked == true) eq_val[77] = -99;
		if (TB_4_79_Cutoff->Checked == true) eq_val[78] = -99;
		if (TB_4_80_Cutoff->Checked == true) eq_val[79] = -99;
		if (TB_4_81_Cutoff->Checked == true) eq_val[80] = -99;
		if (TB_4_82_Cutoff->Checked == true) eq_val[81] = -99;
		if (TB_4_83_Cutoff->Checked == true) eq_val[82] = -99;
		if (TB_4_84_Cutoff->Checked == true) eq_val[83] = -99;
		if (TB_4_85_Cutoff->Checked == true) eq_val[84] = -99;
		if (TB_4_86_Cutoff->Checked == true) eq_val[85] = -99;
		if (TB_4_87_Cutoff->Checked == true) eq_val[86] = -99;
		if (TB_4_88_Cutoff->Checked == true) eq_val[87] = -99;
		if (TB_4_89_Cutoff->Checked == true) eq_val[88] = -99;
		if (TB_4_90_Cutoff->Checked == true) eq_val[89] = -99;
		if (TB_4_91_Cutoff->Checked == true) eq_val[90] = -99;
		if (TB_4_92_Cutoff->Checked == true) eq_val[91] = -99;
		if (TB_4_93_Cutoff->Checked == true) eq_val[92] = -99;
		if (TB_4_94_Cutoff->Checked == true) eq_val[93] = -99;
		if (TB_4_95_Cutoff->Checked == true) eq_val[94] = -99;
		if (TB_4_96_Cutoff->Checked == true) eq_val[95] = -99;
		if (TB_4_97_Cutoff->Checked == true) eq_val[96] = -99;
		if (TB_4_98_Cutoff->Checked == true) eq_val[97] = -99;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 98;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type4_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}
		eqParam4 = (UnicodeString)param;

		// EQ(Type1)
		eq_val[0]  = TB_1_1->Position * -1;
		eq_val[1]  = TB_1_2->Position * -1;
		eq_val[2]  = TB_1_3->Position * -1;
		eq_val[3]  = TB_1_4->Position * -1;
		eq_val[4]  = TB_1_5->Position * -1;
		eq_val[5]  = TB_1_6->Position * -1;
		eq_val[6]  = TB_1_7->Position * -1;
		eq_val[7]  = TB_1_8->Position * -1;
		eq_val[8]  = TB_1_9->Position * -1;
		eq_val[9]  = TB_1_10->Position * -1;
		eq_val[10] = TB_1_11->Position * -1;
		eq_val[11] = TB_1_12->Position * -1;
		eq_val[12] = TB_1_13->Position * -1;
		eq_val[13] = TB_1_14->Position * -1;
		eq_val[14] = TB_1_15->Position * -1;
		eq_val[15] = TB_1_16->Position * -1;
		eq_val[16] = TB_1_17->Position * -1;
		eq_val[17] = TB_1_18->Position * -1;
		eq_val[18] = TB_1_19->Position * -1;
		eq_val[19] = TB_1_20->Position * -1;
		eq_val[20] = TB_1_21->Position * -1;
		eq_val[21] = TB_1_22->Position * -1;
		eq_val[22] = TB_1_23->Position * -1;
		eq_val[23] = TB_1_24->Position * -1;
		eq_val[24] = TB_1_25->Position * -1;
		eq_val[25] = TB_1_26->Position * -1;
		eq_val[26] = TB_1_27->Position * -1;
		eq_val[27] = TB_1_28->Position * -1;
		eq_val[28] = TB_1_29->Position * -1;
		eq_val[29] = TB_1_30->Position * -1;
		eq_val[30] = TB_1_31->Position * -1;
		eq_val[31] = TB_1_32->Position * -1;
		eq_val[32] = TB_1_33->Position * -1;
		eq_val[33] = TB_1_34->Position * -1;
		eq_val[34] = TB_1_35->Position * -1;

		for (i = 0;i < 34;i++) {
			if (abs(eq_val[i] - eq_val[i + 1]) >= 3) {
				disable_spline = 1;
			}
		}
		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 35;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type1_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}
		eqParam1 = (UnicodeString)param;

		// スプライン補間により、なめらかに EQ 値をつなぐ。
		for (i = 0;i < 192000;i++) {
			eq[i] = 1.00;
		}
		pt2d[0].x = 1;
		pt2d[0].y = 1.0;
		pt2d[1].x = 2;
		pt2d[1].y = 1.0;
		pt2d[2].x = 5;
		pt2d[2].y = 1.0;
		pt2d[3].x = 10;
		pt2d[3].y = 1.0;
		pt2d[4].x = 15;
		pt2d[4].y = 1.0;
		per = per1;
		if (rdo_type1_value_x2->Checked == true) {
			per = per2;
		}
		for (i = 0;i < 35;i++) {
			pt2d[5 + i].x = eq_hz[i];
			pt2d[5 + i].y = per[14 + eq_val[i]];
		}
		if (eq_val[34] > 0) {
			// 0 レベルにする
			pt2d[40].x = eq_hz[35];
			pt2d[40].y = 1.0 + ((per[14 + eq_val[34]] - 1.0) / 4) * 3;
			pt2d[41].x = eq_hz[36];
			pt2d[41].y = 1.0 + ((per[14 + eq_val[34]] - 1.0) / 4) * 2;
			pt2d[42].x = eq_hz[37];
			pt2d[42].y = 1.0 + ((per[14 + eq_val[34]] - 1.0) / 4) * 1;
			pt2d[43].x = eq_hz[38];
			pt2d[43].y = 1.0;
			pt2d[44].x = eq_hz[39];
			pt2d[44].y = 1.0;
		} else {
			// 低いレベルを継続する
			pt2d[40].x = eq_hz[35];
			pt2d[40].y = per[14 + eq_val[34]];
			pt2d[41].x = eq_hz[36];
			pt2d[41].y = per[14 + eq_val[34]];
			pt2d[42].x = eq_hz[37];
			pt2d[42].y = per[14 + eq_val[34]];
			pt2d[43].x = eq_hz[38];
			pt2d[43].y = per[14 + eq_val[34]];
			pt2d[44].x = eq_hz[39];
			pt2d[44].y = per[14 + eq_val[34]];
		}
		for (x = 1;x <= eq_hz[39];x++) {
			y = spline(pt2d,45,x,h,b,d,g,u,r);
			eq[x] = y;
		}
		for (;x < 192000;x++) {
			eq[x] = y;
		}
		// データ補正
		for (i = 0;i < 35 - 2;i++) {
			start_len = 0;
			if (eq_val[i] == eq_val[i + 1] && eq_val[i + 1] == eq_val[i + 2]) {
				start_index = i;
				start_len   = 2;
				for (j = start_len + 1;start_index + j < 35 && eq_val[start_index] == eq_val[start_index + j];j++) {
					start_len = j;
				}
				for (j = eq_hz[start_index];j <= eq_hz[start_index + start_len];j++) {
					eq[j] = eq[eq_hz[start_index]];
				}
				i = start_index + start_len;
			}
		}
		for (i = 0;i < 192000;i++) {
			if(IsNan(eq[i])) break;
		}
		// 数字にならないときはスプライン補間をしないようにする
		if (i < 192000) {
			for (i = 0; i <= 39;i++) {
				for (x = pt2d[i].x; x < pt2d[i + 1].x;x++) {
					if (x < 192000) {
						eq[x] = pt2d[i].y;
					}
				}
			}
			v = pt2d[39].y;
			for (;x < 192000;x++) {
				eq[x] = v;
				if (v > 1.0) {
					v -= 0.1;
				}
			}
		}
		per = per1;
		if (rdo_type1_value_x2->Checked == true) {
			per = per2;
		}

		for (i = 0;i < 192000;i++) {
			fprintf(fp,"%lf\n",eq[i]);
		}
		for (;i < 384000;i++) {
			fprintf(fp,"%lf\n",eq[192000 - 1]);
		}
		fprintf(fp,"\nActiveEQ:1\n");
		sStr = eqParam1;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam2;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam3;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam4;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = edt_eq_title->Text;
		fprintf(fp,"%s\n",sStr.c_str());
		fclose(fp);
		edtParameter->Lines->Strings[cmbEQParameter->ItemIndex] = (UnicodeString)L"ActiveEQ:1" + L"|" +  eqParam1 + L"|" + eqParam2 + L"|" + eqParam3 + L"|" + eqParam4 + L"|" + edt_eq_title->Text;
	}
	PRINT_FN_LOG("SaveEQ_Type1","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SaveEQ_Type2(int test_mode)
{
	FILE *fp;
	int i,j,k,x,hz;
	int disable_spline;
	char *p,work[4096],param[4096],title[4096];
	int  eq_val[110];
	long eq_hz[]={20,25,31,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6000,7000,8000,9000,10000,11000,12000,13000,14000,15000,16000,17000,18000,19000,20000,21000,22000,23000,24000,25000,30000,35000,40000,45000,50000,52000,53000,54000,55000,56000};
	double per1[]={0.1995,0.2238,0.2511,0.2818,0.3162,0.3548,0.3981,0.4466,0.5011,0.5623,0.6309,0.7079,0.7943,0.8912,1.0,1.122,1.2589,1.4125,1.5848,1.7782,1.9952,2.2387,2.511,2.8183,3.1622,3.5481,3.981,4.4668,5.0118};
	double per2[]={0.0398,0.0501,0.0630,0.0794,0.1,0.1258,0.1584,0.1995,0.2511,0.3162,0.3981,0.5011,0.6309,0.7943,1.0,1.2589,1.5848,1.9952,2.5118,3.1622,3.9810,5.0118,6.3095,7.9432,10,12.5892,15.8489,19.9526,25.1188};
	double *per;

	PT2D pt2d[61];
	double h[60];
	double b[60];
	double d[60];
	double g[60];
	double u[60];
	double r[61];
	double y;
	double v;
	int start_index,start_len;
	UnicodeString uFileName;
	UnicodeString eqParam1,eqParam2,eqParam3,eqParam4;
	AnsiString sStr;

	PRINT_FN_LOG("SaveEQ_Type2","Start");
	if (test_mode == 0) {
		uFileName.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,cmbEQParameter->ItemIndex + 1);
	} else {
		uFileName.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,0);
	}
	disable_spline = 0;
	fp = fopen(((AnsiString)uFileName).c_str(),"wt");
	if (fp != NULL) {
		// EQ(Type3)
		eq_val[0]  = TB_3_1->Position * -1;
		eq_val[1]  = TB_3_2->Position * -1;
		eq_val[2]  = TB_3_3->Position * -1;
		eq_val[3]  = TB_3_4->Position * -1;
		eq_val[4]  = TB_3_5->Position * -1;
		eq_val[5]  = TB_3_6->Position * -1;
		eq_val[6]  = TB_3_7->Position * -1;
		eq_val[7]  = TB_3_8->Position * -1;
		eq_val[8]  = TB_3_9->Position * -1;
		eq_val[9]  = TB_3_10->Position * -1;
		eq_val[10] = TB_3_11->Position * -1;
		eq_val[11] = TB_3_12->Position * -1;
		eq_val[12] = TB_3_13->Position * -1;
		eq_val[13] = TB_3_14->Position * -1;
		eq_val[14] = TB_3_15->Position * -1;
		eq_val[15] = TB_3_16->Position * -1;
		eq_val[16] = TB_3_17->Position * -1;
		eq_val[17] = TB_3_18->Position * -1;
		eq_val[18] = TB_3_19->Position * -1;
		eq_val[19] = TB_3_20->Position * -1;
		eq_val[20] = TB_3_21->Position * -1;
		eq_val[21] = TB_3_22->Position * -1;
		eq_val[22] = TB_3_23->Position * -1;
		eq_val[23] = TB_3_24->Position * -1;
		eq_val[24] = TB_3_25->Position * -1;
		eq_val[25] = TB_3_26->Position * -1;
		eq_val[26] = TB_3_27->Position * -1;
		eq_val[27] = TB_3_28->Position * -1;
		eq_val[28] = TB_3_29->Position * -1;
		eq_val[29] = TB_3_30->Position * -1;
		eq_val[30] = TB_3_31->Position * -1;
		eq_val[31] = TB_3_32->Position * -1;
		eq_val[32] = TB_3_33->Position * -1;
		eq_val[33] = TB_3_34->Position * -1;
		eq_val[34] = TB_3_35->Position * -1;
		eq_val[35] = TB_3_36->Position * -1;
		eq_val[36] = TB_3_37->Position * -1;
		eq_val[37] = TB_3_38->Position * -1;
		eq_val[38] = TB_3_39->Position * -1;
		eq_val[39] = TB_3_40->Position * -1;
		eq_val[40] = TB_3_41->Position * -1;
		eq_val[41] = TB_3_42->Position * -1;
		eq_val[42] = TB_3_43->Position * -1;
		eq_val[43] = TB_3_44->Position * -1;
		eq_val[44] = TB_3_45->Position * -1;
		eq_val[45] = TB_3_46->Position * -1;
		eq_val[46] = TB_3_47->Position * -1;
		eq_val[47] = TB_3_48->Position * -1;
		eq_val[48] = TB_3_49->Position * -1;
		eq_val[49] = TB_3_50->Position * -1;
		eq_val[50] = TB_3_51->Position * -1;
		eq_val[51] = TB_3_52->Position * -1;
		eq_val[52] = TB_3_53->Position * -1;
		eq_val[53] = TB_3_54->Position * -1;
		eq_val[54] = TB_3_55->Position * -1;
		eq_val[55] = TB_3_56->Position * -1;
		eq_val[56] = TB_3_57->Position * -1;
		eq_val[57] = TB_3_58->Position * -1;
		eq_val[58] = TB_3_59->Position * -1;
		eq_val[59] = TB_3_60->Position * -1;
		eq_val[60] = TB_3_61->Position * -1;
		eq_val[61] = TB_3_62->Position * -1;
		eq_val[62] = TB_3_63->Position * -1;
		eq_val[63] = TB_3_64->Position * -1;
		eq_val[64] = TB_3_65->Position * -1;
		eq_val[65] = TB_3_66->Position * -1;
		eq_val[66] = TB_3_67->Position * -1;
		eq_val[67] = TB_3_68->Position * -1;
		eq_val[68] = TB_3_69->Position * -1;
		eq_val[69] = TB_3_70->Position * -1;
		eq_val[70] = TB_3_71->Position * -1;
		eq_val[71] = TB_3_72->Position * -1;
		eq_val[72] = TB_3_73->Position * -1;
		eq_val[73] = TB_3_74->Position * -1;
		eq_val[74] = TB_3_75->Position * -1;
		eq_val[75] = TB_3_76->Position * -1;
		eq_val[76] = TB_3_77->Position * -1;
		eq_val[77] = TB_3_78->Position * -1;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 78;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type3_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam3 = (UnicodeString)param;

		// EQ(Type1)
		eq_val[0]  = TB_1_1->Position * -1;
		eq_val[1]  = TB_1_2->Position * -1;
		eq_val[2]  = TB_1_3->Position * -1;
		eq_val[3]  = TB_1_4->Position * -1;
		eq_val[4]  = TB_1_5->Position * -1;
		eq_val[5]  = TB_1_6->Position * -1;
		eq_val[6]  = TB_1_7->Position * -1;
		eq_val[7]  = TB_1_8->Position * -1;
		eq_val[8]  = TB_1_9->Position * -1;
		eq_val[9]  = TB_1_10->Position * -1;
		eq_val[10] = TB_1_11->Position * -1;
		eq_val[11] = TB_1_12->Position * -1;
		eq_val[12] = TB_1_13->Position * -1;
		eq_val[13] = TB_1_14->Position * -1;
		eq_val[14] = TB_1_15->Position * -1;
		eq_val[15] = TB_1_16->Position * -1;
		eq_val[16] = TB_1_17->Position * -1;
		eq_val[17] = TB_1_18->Position * -1;
		eq_val[18] = TB_1_19->Position * -1;
		eq_val[19] = TB_1_20->Position * -1;
		eq_val[20] = TB_1_21->Position * -1;
		eq_val[21] = TB_1_22->Position * -1;
		eq_val[22] = TB_1_23->Position * -1;
		eq_val[23] = TB_1_24->Position * -1;
		eq_val[24] = TB_1_25->Position * -1;
		eq_val[25] = TB_1_26->Position * -1;
		eq_val[26] = TB_1_27->Position * -1;
		eq_val[27] = TB_1_28->Position * -1;
		eq_val[28] = TB_1_29->Position * -1;
		eq_val[29] = TB_1_30->Position * -1;
		eq_val[30] = TB_1_31->Position * -1;
		eq_val[31] = TB_1_32->Position * -1;
		eq_val[32] = TB_1_33->Position * -1;
		eq_val[33] = TB_1_34->Position * -1;
		eq_val[34] = TB_1_35->Position * -1;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 35;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type1_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam1 = (UnicodeString)param;

		// EQ(Type4)
		eq_val[0]  = TB_4_1->Position * -1;
		eq_val[1]  = TB_4_2->Position * -1;
		eq_val[2]  = TB_4_3->Position * -1;
		eq_val[3]  = TB_4_4->Position * -1;
		eq_val[4]  = TB_4_5->Position * -1;
		eq_val[5]  = TB_4_6->Position * -1;
		eq_val[6]  = TB_4_7->Position * -1;
		eq_val[7]  = TB_4_8->Position * -1;
		eq_val[8]  = TB_4_9->Position * -1;
		eq_val[9]  = TB_4_10->Position * -1;
		eq_val[10] = TB_4_11->Position * -1;
		eq_val[11] = TB_4_12->Position * -1;
		eq_val[12] = TB_4_13->Position * -1;
		eq_val[13] = TB_4_14->Position * -1;
		eq_val[14] = TB_4_15->Position * -1;
		eq_val[15] = TB_4_16->Position * -1;
		eq_val[16] = TB_4_17->Position * -1;
		eq_val[17] = TB_4_18->Position * -1;
		eq_val[18] = TB_4_19->Position * -1;
		eq_val[19] = TB_4_20->Position * -1;
		eq_val[20] = TB_4_21->Position * -1;
		eq_val[21] = TB_4_22->Position * -1;
		eq_val[22] = TB_4_23->Position * -1;
		eq_val[23] = TB_4_24->Position * -1;
		eq_val[24] = TB_4_25->Position * -1;
		eq_val[25] = TB_4_26->Position * -1;
		eq_val[26] = TB_4_27->Position * -1;
		eq_val[27] = TB_4_28->Position * -1;
		eq_val[28] = TB_4_29->Position * -1;
		eq_val[29] = TB_4_30->Position * -1;
		eq_val[30] = TB_4_31->Position * -1;
		eq_val[31] = TB_4_32->Position * -1;
		eq_val[32] = TB_4_33->Position * -1;
		eq_val[33] = TB_4_34->Position * -1;
		eq_val[34] = TB_4_35->Position * -1;
		eq_val[35] = TB_4_36->Position * -1;
		eq_val[36] = TB_4_37->Position * -1;
		eq_val[37] = TB_4_38->Position * -1;
		eq_val[38] = TB_4_39->Position * -1;
		eq_val[39] = TB_4_40->Position * -1;
		eq_val[40] = TB_4_41->Position * -1;
		eq_val[41] = TB_4_42->Position * -1;
		eq_val[42] = TB_4_43->Position * -1;
		eq_val[43] = TB_4_44->Position * -1;
		eq_val[44] = TB_4_45->Position * -1;
		eq_val[45] = TB_4_46->Position * -1;
		eq_val[46] = TB_4_47->Position * -1;
		eq_val[47] = TB_4_48->Position * -1;
		eq_val[48] = TB_4_49->Position * -1;
		eq_val[49] = TB_4_50->Position * -1;
		eq_val[50] = TB_4_51->Position * -1;
		eq_val[51] = TB_4_52->Position * -1;
		eq_val[52] = TB_4_53->Position * -1;
		eq_val[53] = TB_4_54->Position * -1;
		eq_val[54] = TB_4_55->Position * -1;
		eq_val[55] = TB_4_56->Position * -1;
		eq_val[56] = TB_4_57->Position * -1;
		eq_val[57] = TB_4_58->Position * -1;
		eq_val[58] = TB_4_59->Position * -1;
		eq_val[59] = TB_4_60->Position * -1;
		eq_val[60] = TB_4_61->Position * -1;
		eq_val[61] = TB_4_62->Position * -1;
		eq_val[62] = TB_4_63->Position * -1;
		eq_val[63] = TB_4_64->Position * -1;
		eq_val[64] = TB_4_65->Position * -1;
		eq_val[65] = TB_4_66->Position * -1;
		eq_val[66] = TB_4_67->Position * -1;
		eq_val[67] = TB_4_68->Position * -1;
		eq_val[68] = TB_4_69->Position * -1;
		eq_val[69] = TB_4_70->Position * -1;
		eq_val[70] = TB_4_71->Position * -1;
		eq_val[71] = TB_4_72->Position * -1;
		eq_val[72] = TB_4_73->Position * -1;
		eq_val[73] = TB_4_74->Position * -1;
		eq_val[74] = TB_4_75->Position * -1;
		eq_val[75] = TB_4_76->Position * -1;
		eq_val[76] = TB_4_77->Position * -1;
		eq_val[77] = TB_4_78->Position * -1;
		eq_val[78] = TB_4_79->Position * -1;
		eq_val[79] = TB_4_80->Position * -1;
		eq_val[80] = TB_4_81->Position * -1;
		eq_val[81] = TB_4_82->Position * -1;
		eq_val[82] = TB_4_83->Position * -1;
		eq_val[83] = TB_4_84->Position * -1;
		eq_val[84] = TB_4_85->Position * -1;
		eq_val[85] = TB_4_86->Position * -1;
		eq_val[86] = TB_4_87->Position * -1;
		eq_val[87] = TB_4_88->Position * -1;
		eq_val[88] = TB_4_89->Position * -1;
		eq_val[89] = TB_4_90->Position * -1;
		eq_val[90] = TB_4_91->Position * -1;
		eq_val[91] = TB_4_92->Position * -1;
		eq_val[92] = TB_4_93->Position * -1;
		eq_val[93] = TB_4_94->Position * -1;
		eq_val[94] = TB_4_95->Position * -1;
		eq_val[95] = TB_4_96->Position * -1;
		eq_val[96] = TB_4_97->Position * -1;
		eq_val[97] = TB_4_98->Position * -1;

		// 指定周波数のカットオフ対応
		if (TB_4_1_Cutoff->Checked == true) eq_val[0] = -99;
		if (TB_4_2_Cutoff->Checked == true) eq_val[1] = -99;
		if (TB_4_3_Cutoff->Checked == true) eq_val[2] = -99;
		if (TB_4_4_Cutoff->Checked == true) eq_val[3] = -99;
		if (TB_4_5_Cutoff->Checked == true) eq_val[4] = -99;
		if (TB_4_6_Cutoff->Checked == true) eq_val[5] = -99;
		if (TB_4_7_Cutoff->Checked == true) eq_val[6] = -99;
		if (TB_4_8_Cutoff->Checked == true) eq_val[7] = -99;
		if (TB_4_9_Cutoff->Checked == true) eq_val[8] = -99;
		if (TB_4_10_Cutoff->Checked == true) eq_val[9] = -99;
		if (TB_4_11_Cutoff->Checked == true) eq_val[10] = -99;
		if (TB_4_12_Cutoff->Checked == true) eq_val[11] = -99;
		if (TB_4_13_Cutoff->Checked == true) eq_val[12] = -99;
		if (TB_4_36_Cutoff->Checked == true) eq_val[35] = -99;
		if (TB_4_37_Cutoff->Checked == true) eq_val[36] = -99;
		if (TB_4_38_Cutoff->Checked == true) eq_val[37] = -99;
		if (TB_4_39_Cutoff->Checked == true) eq_val[38] = -99;
		if (TB_4_40_Cutoff->Checked == true) eq_val[39] = -99;
		if (TB_4_41_Cutoff->Checked == true) eq_val[40] = -99;
		if (TB_4_42_Cutoff->Checked == true) eq_val[41] = -99;
		if (TB_4_43_Cutoff->Checked == true) eq_val[42] = -99;
		if (TB_4_44_Cutoff->Checked == true) eq_val[43] = -99;
		if (TB_4_45_Cutoff->Checked == true) eq_val[44] = -99;
		if (TB_4_46_Cutoff->Checked == true) eq_val[45] = -99;
		if (TB_4_47_Cutoff->Checked == true) eq_val[46] = -99;
		if (TB_4_48_Cutoff->Checked == true) eq_val[47] = -99;
		if (TB_4_49_Cutoff->Checked == true) eq_val[48] = -99;
		if (TB_4_50_Cutoff->Checked == true) eq_val[49] = -99;
		if (TB_4_51_Cutoff->Checked == true) eq_val[50] = -99;
		if (TB_4_52_Cutoff->Checked == true) eq_val[51] = -99;
		if (TB_4_53_Cutoff->Checked == true) eq_val[52] = -99;
		if (TB_4_54_Cutoff->Checked == true) eq_val[53] = -99;
		if (TB_4_55_Cutoff->Checked == true) eq_val[54] = -99;
		if (TB_4_56_Cutoff->Checked == true) eq_val[55] = -99;
		if (TB_4_57_Cutoff->Checked == true) eq_val[56] = -99;
		if (TB_4_58_Cutoff->Checked == true) eq_val[57] = -99;
		if (TB_4_59_Cutoff->Checked == true) eq_val[58] = -99;
		if (TB_4_60_Cutoff->Checked == true) eq_val[59] = -99;
		if (TB_4_61_Cutoff->Checked == true) eq_val[60] = -99;
		if (TB_4_62_Cutoff->Checked == true) eq_val[61] = -99;
		if (TB_4_63_Cutoff->Checked == true) eq_val[62] = -99;
		if (TB_4_64_Cutoff->Checked == true) eq_val[63] = -99;
		if (TB_4_65_Cutoff->Checked == true) eq_val[64] = -99;
		if (TB_4_66_Cutoff->Checked == true) eq_val[65] = -99;
		if (TB_4_67_Cutoff->Checked == true) eq_val[66] = -99;
		if (TB_4_68_Cutoff->Checked == true) eq_val[67] = -99;
		if (TB_4_69_Cutoff->Checked == true) eq_val[68] = -99;
		if (TB_4_70_Cutoff->Checked == true) eq_val[69] = -99;
		if (TB_4_71_Cutoff->Checked == true) eq_val[70] = -99;
		if (TB_4_72_Cutoff->Checked == true) eq_val[71] = -99;
		if (TB_4_73_Cutoff->Checked == true) eq_val[72] = -99;
		if (TB_4_74_Cutoff->Checked == true) eq_val[73] = -99;
		if (TB_4_75_Cutoff->Checked == true) eq_val[74] = -99;
		if (TB_4_76_Cutoff->Checked == true) eq_val[75] = -99;
		if (TB_4_77_Cutoff->Checked == true) eq_val[76] = -99;
		if (TB_4_78_Cutoff->Checked == true) eq_val[77] = -99;
		if (TB_4_79_Cutoff->Checked == true) eq_val[78] = -99;
		if (TB_4_80_Cutoff->Checked == true) eq_val[79] = -99;
		if (TB_4_81_Cutoff->Checked == true) eq_val[80] = -99;
		if (TB_4_82_Cutoff->Checked == true) eq_val[81] = -99;
		if (TB_4_83_Cutoff->Checked == true) eq_val[82] = -99;
		if (TB_4_84_Cutoff->Checked == true) eq_val[83] = -99;
		if (TB_4_85_Cutoff->Checked == true) eq_val[84] = -99;
		if (TB_4_86_Cutoff->Checked == true) eq_val[85] = -99;
		if (TB_4_87_Cutoff->Checked == true) eq_val[86] = -99;
		if (TB_4_88_Cutoff->Checked == true) eq_val[87] = -99;
		if (TB_4_89_Cutoff->Checked == true) eq_val[88] = -99;
		if (TB_4_90_Cutoff->Checked == true) eq_val[89] = -99;
		if (TB_4_91_Cutoff->Checked == true) eq_val[90] = -99;
		if (TB_4_92_Cutoff->Checked == true) eq_val[91] = -99;
		if (TB_4_93_Cutoff->Checked == true) eq_val[92] = -99;
		if (TB_4_94_Cutoff->Checked == true) eq_val[93] = -99;
		if (TB_4_95_Cutoff->Checked == true) eq_val[94] = -99;
		if (TB_4_96_Cutoff->Checked == true) eq_val[95] = -99;
		if (TB_4_97_Cutoff->Checked == true) eq_val[96] = -99;
		if (TB_4_98_Cutoff->Checked == true) eq_val[97] = -99;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 98;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type4_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}
		eqParam4 = (UnicodeString)param;

		// EQ(Type2)
		eq_val[0]  = TB_2_1->Position * -1;
		eq_val[1]  = TB_2_2->Position * -1;
		eq_val[2]  = TB_2_3->Position * -1;
		eq_val[3]  = TB_2_4->Position * -1;
		eq_val[4]  = TB_2_5->Position * -1;
		eq_val[5]  = TB_2_6->Position * -1;
		eq_val[6]  = TB_2_7->Position * -1;
		eq_val[7]  = TB_2_8->Position * -1;
		eq_val[8]  = TB_2_9->Position * -1;
		eq_val[9]  = TB_2_10->Position * -1;
		eq_val[10] = TB_2_11->Position * -1;
		eq_val[11] = TB_2_12->Position * -1;
		eq_val[12] = TB_2_13->Position * -1;
		eq_val[13] = TB_2_14->Position * -1;
		eq_val[14] = TB_2_15->Position * -1;
		eq_val[15] = TB_2_16->Position * -1;
		eq_val[16] = TB_2_17->Position * -1;
		eq_val[17] = TB_2_18->Position * -1;
		eq_val[18] = TB_2_19->Position * -1;
		eq_val[19] = TB_2_20->Position * -1;
		eq_val[20] = TB_2_21->Position * -1;
		eq_val[21] = TB_2_22->Position * -1;
		eq_val[22] = TB_2_23->Position * -1;
		eq_val[23] = TB_2_24->Position * -1;
		eq_val[24] = TB_2_25->Position * -1;
		eq_val[25] = TB_2_26->Position * -1;
		eq_val[26] = TB_2_27->Position * -1;
		eq_val[27] = TB_2_28->Position * -1;
		eq_val[28] = TB_2_29->Position * -1;
		eq_val[29] = TB_2_30->Position * -1;
		eq_val[30] = TB_2_31->Position * -1;
		eq_val[31] = TB_2_32->Position * -1;
		eq_val[32] = TB_2_33->Position * -1;
		eq_val[33] = TB_2_34->Position * -1;
		eq_val[34] = TB_2_35->Position * -1;
		eq_val[35] = TB_2_36->Position * -1;
		eq_val[36] = TB_2_37->Position * -1;
		eq_val[37] = TB_2_38->Position * -1;
		eq_val[38] = TB_2_39->Position * -1;
		eq_val[39] = TB_2_40->Position * -1;
		eq_val[40] = TB_2_41->Position * -1;
		eq_val[41] = TB_2_42->Position * -1;
		eq_val[42] = TB_2_43->Position * -1;
		eq_val[43] = TB_2_44->Position * -1;
		eq_val[44] = TB_2_45->Position * -1;
		eq_val[45] = TB_2_46->Position * -1;
		eq_val[46] = TB_2_47->Position * -1;
		eq_val[47] = TB_2_48->Position * -1;
		eq_val[48] = TB_2_49->Position * -1;
		eq_val[49] = TB_2_50->Position * -1;

		for (i = 0;i < 49;i++) {
			if (abs(eq_val[i] - eq_val[i + 1]) >= 3) {
				disable_spline = 1;
			}
		}
		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 50;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type2_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam2 = (UnicodeString)param;

		per = per1;
		if (rdo_type2_value_x2->Checked == true) {
			per = per2;
		}

		// スプライン補間により、なめらかに EQ 値をつなぐ。
		for (i = 0;i < 192000;i++) {
			eq[i] = 1.00;
		}
		pt2d[0].x = 1;
		pt2d[0].y = 1.0;
		pt2d[1].x = 2;
		pt2d[1].y = 1.0;
		pt2d[2].x = 5;
		pt2d[2].y = 1.0;
		pt2d[3].x = 10;
		pt2d[3].y = 1.0;
		pt2d[4].x = 15;
		pt2d[4].y = 1.0;
		for (i = 0;i < 50;i++) {
			pt2d[5 + i].x = eq_hz[i];
			pt2d[5 + i].y = per[14 + eq_val[i]];
		}
		if (eq_val[49] > 0) {
			// 0 レベルにする
			pt2d[55].x = eq_hz[50];
			pt2d[55].y = 1.0 + ((per[14 + eq_val[49]] - 1.0) / 4) * 3;
			pt2d[56].x = eq_hz[51];
			pt2d[56].y = 1.0 + ((per[14 + eq_val[49]] - 1.0) / 4) * 2;
			pt2d[57].x = eq_hz[52];
			pt2d[57].y = 1.0 + ((per[14 + eq_val[49]] - 1.0) / 4) * 1;
			pt2d[58].x = eq_hz[53];
			pt2d[58].y = 1.0;
			pt2d[59].x = eq_hz[54];
			pt2d[59].y = 1.0;
		} else {
			// 低いレベルを継続する
			pt2d[55].x = eq_hz[50];
			pt2d[55].y = per[14 + eq_val[49]];
			pt2d[56].x = eq_hz[51];
			pt2d[56].y = per[14 + eq_val[49]];
			pt2d[57].x = eq_hz[52];
			pt2d[57].y = per[14 + eq_val[49]];
			pt2d[58].x = eq_hz[53];
			pt2d[58].y = per[14 + eq_val[49]];
			pt2d[59].x = eq_hz[54];
			pt2d[59].y = per[14 + eq_val[49]];
		}
		for (x = 1;x <= eq_hz[54];x++) {
			y = spline(pt2d,60,x,h,b,d,g,u,r);
			eq[x] = y;
		}
		for (;x < 192000;x++) {
			eq[x] = y;
		}
		// データ補正
		for (i = 0;i < 50 - 2;i++) {
			start_len = 0;
			if (eq_val[i] == eq_val[i + 1] && eq_val[i + 1] == eq_val[i + 2]) {
				start_index = i;
				start_len   = 2;
				for (j = start_len + 1;start_index + j < 50 && eq_val[start_index] == eq_val[start_index + j];j++) {
					start_len = j;
				}
				for (j = eq_hz[start_index];j <= eq_hz[start_index + start_len];j++) {
					eq[j] = eq[eq_hz[start_index]];
				}
				i = start_index + start_len;
			}
		}
		// 数字にならないときはスプライン補間をしないようにする
		for (i = 0;i < 192000;i++) {
			if(IsNan(eq[i])) break;
		}
		if (i < 192000) {
			for (i = 0; i <= 54;i++) {
				for (x = pt2d[i].x; x < pt2d[i + 1].x;x++) {
					if (x < 192000) {
						eq[x] = pt2d[i].y;
					}
				}
			}
			v = pt2d[54].y;
			for (;x < 192000;x++) {
				eq[x] = v;
				if (v > 1.0) {
					v -= 0.1;
				}
			}
		}

		for (i = 0;i < 192000;i++) {
			fprintf(fp,"%lf\n",eq[i]);
		}
		for (;i < 384000;i++) {
			fprintf(fp,"%lf\n",eq[192000 - 1]);
		}
		fprintf(fp,"\nActiveEQ:2\n");
		sStr = eqParam1;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam2;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam3;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam4;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = edt_eq_title->Text;
		fprintf(fp,"%s\n",sStr.c_str());
		fclose(fp);
		edtParameter->Lines->Strings[cmbEQParameter->ItemIndex] = (UnicodeString)L"ActiveEQ:2" + L"|" +  eqParam1 + L"|" + eqParam2 + L"|" + eqParam3 + L"|" + eqParam4 + L"|" + edt_eq_title->Text;
	}
	PRINT_FN_LOG("SaveEQ_Type2","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SaveEQ_Type3(int test_mode)
{
	FILE *fp;
	int i,j,k,x,hz;
	int disable_spline;
	char *p,work[4096],param[4096],title[4096];
	int  eq_val[110];
	long eq_hz[]={5,6,8,10,12,16,20,25,31,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6000,7000,8000,9000,10000,11000,12000,13000,14000,15000,16000,17000,18000,19000,20000,21000,22000,23000,24000,25000,26000,27000,28000,29000,30000,31000,32000,33000,34000,35000,36000,37000,38000,39000,40000,45000,50000,55000,60000,65000,70000,75000,80000,85000,90000,95000,100000,102000,103000,104000,105000,106000};
	double per1[]={0.1995,0.2238,0.2511,0.2818,0.3162,0.3548,0.3981,0.4466,0.5011,0.5623,0.6309,0.7079,0.7943,0.8912,1.0,1.122,1.2589,1.4125,1.5848,1.7782,1.9952,2.2387,2.511,2.8183,3.1622,3.5481,3.981,4.4668,5.0118};
	double per2[]={0.0398,0.0501,0.0630,0.0794,0.1,0.1258,0.1584,0.1995,0.2511,0.3162,0.3981,0.5011,0.6309,0.7943,1.0,1.2589,1.5848,1.9952,2.5118,3.1622,3.9810,5.0118,6.3095,7.9432,10,12.5892,15.8489,19.9526,25.1188};
	double *per;

	PT2D pt2d[91];
	double h[90];
	double b[90];
	double d[90];
	double g[90];
	double u[90];
	double r[91];
	double y;
	double v;
	int start_index,start_len;
	UnicodeString uFileName;
	UnicodeString eqParam1,eqParam2,eqParam3,eqParam4;
	AnsiString sStr;

	PRINT_FN_LOG("SaveEQ_Type3","Start");
	if (test_mode == 0) {
		uFileName.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,cmbEQParameter->ItemIndex + 1);
	} else {
		uFileName.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,0);
	}
	disable_spline = 0;
	fp = fopen(((AnsiString)uFileName).c_str(),"wt");
	if (fp != NULL) {
		// EQ(Type1)
		eq_val[0]  = TB_1_1->Position * -1;
		eq_val[1]  = TB_1_2->Position * -1;
		eq_val[2]  = TB_1_3->Position * -1;
		eq_val[3]  = TB_1_4->Position * -1;
		eq_val[4]  = TB_1_5->Position * -1;
		eq_val[5]  = TB_1_6->Position * -1;
		eq_val[6]  = TB_1_7->Position * -1;
		eq_val[7]  = TB_1_8->Position * -1;
		eq_val[8]  = TB_1_9->Position * -1;
		eq_val[9]  = TB_1_10->Position * -1;
		eq_val[10] = TB_1_11->Position * -1;
		eq_val[11] = TB_1_12->Position * -1;
		eq_val[12] = TB_1_13->Position * -1;
		eq_val[13] = TB_1_14->Position * -1;
		eq_val[14] = TB_1_15->Position * -1;
		eq_val[15] = TB_1_16->Position * -1;
		eq_val[16] = TB_1_17->Position * -1;
		eq_val[17] = TB_1_18->Position * -1;
		eq_val[18] = TB_1_19->Position * -1;
		eq_val[19] = TB_1_20->Position * -1;
		eq_val[20] = TB_1_21->Position * -1;
		eq_val[21] = TB_1_22->Position * -1;
		eq_val[22] = TB_1_23->Position * -1;
		eq_val[23] = TB_1_24->Position * -1;
		eq_val[24] = TB_1_25->Position * -1;
		eq_val[25] = TB_1_26->Position * -1;
		eq_val[26] = TB_1_27->Position * -1;
		eq_val[27] = TB_1_28->Position * -1;
		eq_val[28] = TB_1_29->Position * -1;
		eq_val[29] = TB_1_30->Position * -1;
		eq_val[30] = TB_1_31->Position * -1;
		eq_val[31] = TB_1_32->Position * -1;
		eq_val[32] = TB_1_33->Position * -1;
		eq_val[33] = TB_1_34->Position * -1;
		eq_val[34] = TB_1_35->Position * -1;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 35;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type1_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam1 = (UnicodeString)param;

		// EQ(Type2)
		eq_val[0]  = TB_2_1->Position * -1;
		eq_val[1]  = TB_2_2->Position * -1;
		eq_val[2]  = TB_2_3->Position * -1;
		eq_val[3]  = TB_2_4->Position * -1;
		eq_val[4]  = TB_2_5->Position * -1;
		eq_val[5]  = TB_2_6->Position * -1;
		eq_val[6]  = TB_2_7->Position * -1;
		eq_val[7]  = TB_2_8->Position * -1;
		eq_val[8]  = TB_2_9->Position * -1;
		eq_val[9]  = TB_2_10->Position * -1;
		eq_val[10] = TB_2_11->Position * -1;
		eq_val[11] = TB_2_12->Position * -1;
		eq_val[12] = TB_2_13->Position * -1;
		eq_val[13] = TB_2_14->Position * -1;
		eq_val[14] = TB_2_15->Position * -1;
		eq_val[15] = TB_2_16->Position * -1;
		eq_val[16] = TB_2_17->Position * -1;
		eq_val[17] = TB_2_18->Position * -1;
		eq_val[18] = TB_2_19->Position * -1;
		eq_val[19] = TB_2_20->Position * -1;
		eq_val[20] = TB_2_21->Position * -1;
		eq_val[21] = TB_2_22->Position * -1;
		eq_val[22] = TB_2_23->Position * -1;
		eq_val[23] = TB_2_24->Position * -1;
		eq_val[24] = TB_2_25->Position * -1;
		eq_val[25] = TB_2_26->Position * -1;
		eq_val[26] = TB_2_27->Position * -1;
		eq_val[27] = TB_2_28->Position * -1;
		eq_val[28] = TB_2_29->Position * -1;
		eq_val[29] = TB_2_30->Position * -1;
		eq_val[30] = TB_2_31->Position * -1;
		eq_val[31] = TB_2_32->Position * -1;
		eq_val[32] = TB_2_33->Position * -1;
		eq_val[33] = TB_2_34->Position * -1;
		eq_val[34] = TB_2_35->Position * -1;
		eq_val[35] = TB_2_36->Position * -1;
		eq_val[36] = TB_2_37->Position * -1;
		eq_val[37] = TB_2_38->Position * -1;
		eq_val[38] = TB_2_39->Position * -1;
		eq_val[39] = TB_2_40->Position * -1;
		eq_val[40] = TB_2_41->Position * -1;
		eq_val[41] = TB_2_42->Position * -1;
		eq_val[42] = TB_2_43->Position * -1;
		eq_val[43] = TB_2_44->Position * -1;
		eq_val[44] = TB_2_45->Position * -1;
		eq_val[45] = TB_2_46->Position * -1;
		eq_val[46] = TB_2_47->Position * -1;
		eq_val[47] = TB_2_48->Position * -1;
		eq_val[48] = TB_2_49->Position * -1;
		eq_val[49] = TB_2_50->Position * -1;
		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 50;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type2_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}
		eqParam2 = (UnicodeString)param;

		// EQ(Type4)
		eq_val[0]  = TB_4_1->Position * -1;
		eq_val[1]  = TB_4_2->Position * -1;
		eq_val[2]  = TB_4_3->Position * -1;
		eq_val[3]  = TB_4_4->Position * -1;
		eq_val[4]  = TB_4_5->Position * -1;
		eq_val[5]  = TB_4_6->Position * -1;
		eq_val[6]  = TB_4_7->Position * -1;
		eq_val[7]  = TB_4_8->Position * -1;
		eq_val[8]  = TB_4_9->Position * -1;
		eq_val[9]  = TB_4_10->Position * -1;
		eq_val[10] = TB_4_11->Position * -1;
		eq_val[11] = TB_4_12->Position * -1;
		eq_val[12] = TB_4_13->Position * -1;
		eq_val[13] = TB_4_14->Position * -1;
		eq_val[14] = TB_4_15->Position * -1;
		eq_val[15] = TB_4_16->Position * -1;
		eq_val[16] = TB_4_17->Position * -1;
		eq_val[17] = TB_4_18->Position * -1;
		eq_val[18] = TB_4_19->Position * -1;
		eq_val[19] = TB_4_20->Position * -1;
		eq_val[20] = TB_4_21->Position * -1;
		eq_val[21] = TB_4_22->Position * -1;
		eq_val[22] = TB_4_23->Position * -1;
		eq_val[23] = TB_4_24->Position * -1;
		eq_val[24] = TB_4_25->Position * -1;
		eq_val[25] = TB_4_26->Position * -1;
		eq_val[26] = TB_4_27->Position * -1;
		eq_val[27] = TB_4_28->Position * -1;
		eq_val[28] = TB_4_29->Position * -1;
		eq_val[29] = TB_4_30->Position * -1;
		eq_val[30] = TB_4_31->Position * -1;
		eq_val[31] = TB_4_32->Position * -1;
		eq_val[32] = TB_4_33->Position * -1;
		eq_val[33] = TB_4_34->Position * -1;
		eq_val[34] = TB_4_35->Position * -1;
		eq_val[35] = TB_4_36->Position * -1;
		eq_val[36] = TB_4_37->Position * -1;
		eq_val[37] = TB_4_38->Position * -1;
		eq_val[38] = TB_4_39->Position * -1;
		eq_val[39] = TB_4_40->Position * -1;
		eq_val[40] = TB_4_41->Position * -1;
		eq_val[41] = TB_4_42->Position * -1;
		eq_val[42] = TB_4_43->Position * -1;
		eq_val[43] = TB_4_44->Position * -1;
		eq_val[44] = TB_4_45->Position * -1;
		eq_val[45] = TB_4_46->Position * -1;
		eq_val[46] = TB_4_47->Position * -1;
		eq_val[47] = TB_4_48->Position * -1;
		eq_val[48] = TB_4_49->Position * -1;
		eq_val[49] = TB_4_50->Position * -1;
		eq_val[50] = TB_4_51->Position * -1;
		eq_val[51] = TB_4_52->Position * -1;
		eq_val[52] = TB_4_53->Position * -1;
		eq_val[53] = TB_4_54->Position * -1;
		eq_val[54] = TB_4_55->Position * -1;
		eq_val[55] = TB_4_56->Position * -1;
		eq_val[56] = TB_4_57->Position * -1;
		eq_val[57] = TB_4_58->Position * -1;
		eq_val[58] = TB_4_59->Position * -1;
		eq_val[59] = TB_4_60->Position * -1;
		eq_val[60] = TB_4_61->Position * -1;
		eq_val[61] = TB_4_62->Position * -1;
		eq_val[62] = TB_4_63->Position * -1;
		eq_val[63] = TB_4_64->Position * -1;
		eq_val[64] = TB_4_65->Position * -1;
		eq_val[65] = TB_4_66->Position * -1;
		eq_val[66] = TB_4_67->Position * -1;
		eq_val[67] = TB_4_68->Position * -1;
		eq_val[68] = TB_4_69->Position * -1;
		eq_val[69] = TB_4_70->Position * -1;
		eq_val[70] = TB_4_71->Position * -1;
		eq_val[71] = TB_4_72->Position * -1;
		eq_val[72] = TB_4_73->Position * -1;
		eq_val[73] = TB_4_74->Position * -1;
		eq_val[74] = TB_4_75->Position * -1;
		eq_val[75] = TB_4_76->Position * -1;
		eq_val[76] = TB_4_77->Position * -1;
		eq_val[77] = TB_4_78->Position * -1;
		eq_val[78] = TB_4_79->Position * -1;
		eq_val[79] = TB_4_80->Position * -1;
		eq_val[80] = TB_4_81->Position * -1;
		eq_val[81] = TB_4_82->Position * -1;
		eq_val[82] = TB_4_83->Position * -1;
		eq_val[83] = TB_4_84->Position * -1;
		eq_val[84] = TB_4_85->Position * -1;
		eq_val[85] = TB_4_86->Position * -1;
		eq_val[86] = TB_4_87->Position * -1;
		eq_val[87] = TB_4_88->Position * -1;
		eq_val[88] = TB_4_89->Position * -1;
		eq_val[89] = TB_4_90->Position * -1;
		eq_val[90] = TB_4_91->Position * -1;
		eq_val[91] = TB_4_92->Position * -1;
		eq_val[92] = TB_4_93->Position * -1;
		eq_val[93] = TB_4_94->Position * -1;
		eq_val[94] = TB_4_95->Position * -1;
		eq_val[95] = TB_4_96->Position * -1;
		eq_val[96] = TB_4_97->Position * -1;
		eq_val[97] = TB_4_98->Position * -1;

				// 指定周波数のカットオフ対応
		if (TB_4_1_Cutoff->Checked == true) eq_val[0] = -99;
		if (TB_4_2_Cutoff->Checked == true) eq_val[1] = -99;
		if (TB_4_3_Cutoff->Checked == true) eq_val[2] = -99;
		if (TB_4_4_Cutoff->Checked == true) eq_val[3] = -99;
		if (TB_4_5_Cutoff->Checked == true) eq_val[4] = -99;
		if (TB_4_6_Cutoff->Checked == true) eq_val[5] = -99;
		if (TB_4_7_Cutoff->Checked == true) eq_val[6] = -99;
		if (TB_4_8_Cutoff->Checked == true) eq_val[7] = -99;
		if (TB_4_9_Cutoff->Checked == true) eq_val[8] = -99;
		if (TB_4_10_Cutoff->Checked == true) eq_val[9] = -99;
		if (TB_4_11_Cutoff->Checked == true) eq_val[10] = -99;
		if (TB_4_12_Cutoff->Checked == true) eq_val[11] = -99;
		if (TB_4_13_Cutoff->Checked == true) eq_val[12] = -99;
		if (TB_4_36_Cutoff->Checked == true) eq_val[35] = -99;
		if (TB_4_37_Cutoff->Checked == true) eq_val[36] = -99;
		if (TB_4_38_Cutoff->Checked == true) eq_val[37] = -99;
		if (TB_4_39_Cutoff->Checked == true) eq_val[38] = -99;
		if (TB_4_40_Cutoff->Checked == true) eq_val[39] = -99;
		if (TB_4_41_Cutoff->Checked == true) eq_val[40] = -99;
		if (TB_4_42_Cutoff->Checked == true) eq_val[41] = -99;
		if (TB_4_43_Cutoff->Checked == true) eq_val[42] = -99;
		if (TB_4_44_Cutoff->Checked == true) eq_val[43] = -99;
		if (TB_4_45_Cutoff->Checked == true) eq_val[44] = -99;
		if (TB_4_46_Cutoff->Checked == true) eq_val[45] = -99;
		if (TB_4_47_Cutoff->Checked == true) eq_val[46] = -99;
		if (TB_4_48_Cutoff->Checked == true) eq_val[47] = -99;
		if (TB_4_49_Cutoff->Checked == true) eq_val[48] = -99;
		if (TB_4_50_Cutoff->Checked == true) eq_val[49] = -99;
		if (TB_4_51_Cutoff->Checked == true) eq_val[50] = -99;
		if (TB_4_52_Cutoff->Checked == true) eq_val[51] = -99;
		if (TB_4_53_Cutoff->Checked == true) eq_val[52] = -99;
		if (TB_4_54_Cutoff->Checked == true) eq_val[53] = -99;
		if (TB_4_55_Cutoff->Checked == true) eq_val[54] = -99;
		if (TB_4_56_Cutoff->Checked == true) eq_val[55] = -99;
		if (TB_4_57_Cutoff->Checked == true) eq_val[56] = -99;
		if (TB_4_58_Cutoff->Checked == true) eq_val[57] = -99;
		if (TB_4_59_Cutoff->Checked == true) eq_val[58] = -99;
		if (TB_4_60_Cutoff->Checked == true) eq_val[59] = -99;
		if (TB_4_61_Cutoff->Checked == true) eq_val[60] = -99;
		if (TB_4_62_Cutoff->Checked == true) eq_val[61] = -99;
		if (TB_4_63_Cutoff->Checked == true) eq_val[62] = -99;
		if (TB_4_64_Cutoff->Checked == true) eq_val[63] = -99;
		if (TB_4_65_Cutoff->Checked == true) eq_val[64] = -99;
		if (TB_4_66_Cutoff->Checked == true) eq_val[65] = -99;
		if (TB_4_67_Cutoff->Checked == true) eq_val[66] = -99;
		if (TB_4_68_Cutoff->Checked == true) eq_val[67] = -99;
		if (TB_4_69_Cutoff->Checked == true) eq_val[68] = -99;
		if (TB_4_70_Cutoff->Checked == true) eq_val[69] = -99;
		if (TB_4_71_Cutoff->Checked == true) eq_val[70] = -99;
		if (TB_4_72_Cutoff->Checked == true) eq_val[71] = -99;
		if (TB_4_73_Cutoff->Checked == true) eq_val[72] = -99;
		if (TB_4_74_Cutoff->Checked == true) eq_val[73] = -99;
		if (TB_4_75_Cutoff->Checked == true) eq_val[74] = -99;
		if (TB_4_76_Cutoff->Checked == true) eq_val[75] = -99;
		if (TB_4_77_Cutoff->Checked == true) eq_val[76] = -99;
		if (TB_4_78_Cutoff->Checked == true) eq_val[77] = -99;
		if (TB_4_79_Cutoff->Checked == true) eq_val[78] = -99;
		if (TB_4_80_Cutoff->Checked == true) eq_val[79] = -99;
		if (TB_4_81_Cutoff->Checked == true) eq_val[80] = -99;
		if (TB_4_82_Cutoff->Checked == true) eq_val[81] = -99;
		if (TB_4_83_Cutoff->Checked == true) eq_val[82] = -99;
		if (TB_4_84_Cutoff->Checked == true) eq_val[83] = -99;
		if (TB_4_85_Cutoff->Checked == true) eq_val[84] = -99;
		if (TB_4_86_Cutoff->Checked == true) eq_val[85] = -99;
		if (TB_4_87_Cutoff->Checked == true) eq_val[86] = -99;
		if (TB_4_88_Cutoff->Checked == true) eq_val[87] = -99;
		if (TB_4_89_Cutoff->Checked == true) eq_val[88] = -99;
		if (TB_4_90_Cutoff->Checked == true) eq_val[89] = -99;
		if (TB_4_91_Cutoff->Checked == true) eq_val[90] = -99;
		if (TB_4_92_Cutoff->Checked == true) eq_val[91] = -99;
		if (TB_4_93_Cutoff->Checked == true) eq_val[92] = -99;
		if (TB_4_94_Cutoff->Checked == true) eq_val[93] = -99;
		if (TB_4_95_Cutoff->Checked == true) eq_val[94] = -99;
		if (TB_4_96_Cutoff->Checked == true) eq_val[95] = -99;
		if (TB_4_97_Cutoff->Checked == true) eq_val[96] = -99;
		if (TB_4_98_Cutoff->Checked == true) eq_val[97] = -99;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 98;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type4_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}
		eqParam4 = (UnicodeString)param;

		// EQ(Type3)
		eq_val[0]  = TB_3_1->Position * -1;
		eq_val[1]  = TB_3_2->Position * -1;
		eq_val[2]  = TB_3_3->Position * -1;
		eq_val[3]  = TB_3_4->Position * -1;
		eq_val[4]  = TB_3_5->Position * -1;
		eq_val[5]  = TB_3_6->Position * -1;
		eq_val[6]  = TB_3_7->Position * -1;
		eq_val[7]  = TB_3_8->Position * -1;
		eq_val[8]  = TB_3_9->Position * -1;
		eq_val[9]  = TB_3_10->Position * -1;
		eq_val[10] = TB_3_11->Position * -1;
		eq_val[11] = TB_3_12->Position * -1;
		eq_val[12] = TB_3_13->Position * -1;
		eq_val[13] = TB_3_14->Position * -1;
		eq_val[14] = TB_3_15->Position * -1;
		eq_val[15] = TB_3_16->Position * -1;
		eq_val[16] = TB_3_17->Position * -1;
		eq_val[17] = TB_3_18->Position * -1;
		eq_val[18] = TB_3_19->Position * -1;
		eq_val[19] = TB_3_20->Position * -1;
		eq_val[20] = TB_3_21->Position * -1;
		eq_val[21] = TB_3_22->Position * -1;
		eq_val[22] = TB_3_23->Position * -1;
		eq_val[23] = TB_3_24->Position * -1;
		eq_val[24] = TB_3_25->Position * -1;
		eq_val[25] = TB_3_26->Position * -1;
		eq_val[26] = TB_3_27->Position * -1;
		eq_val[27] = TB_3_28->Position * -1;
		eq_val[28] = TB_3_29->Position * -1;
		eq_val[29] = TB_3_30->Position * -1;
		eq_val[30] = TB_3_31->Position * -1;
		eq_val[31] = TB_3_32->Position * -1;
		eq_val[32] = TB_3_33->Position * -1;
		eq_val[33] = TB_3_34->Position * -1;
		eq_val[34] = TB_3_35->Position * -1;
		eq_val[35] = TB_3_36->Position * -1;
		eq_val[36] = TB_3_37->Position * -1;
		eq_val[37] = TB_3_38->Position * -1;
		eq_val[38] = TB_3_39->Position * -1;
		eq_val[39] = TB_3_40->Position * -1;
		eq_val[40] = TB_3_41->Position * -1;
		eq_val[41] = TB_3_42->Position * -1;
		eq_val[42] = TB_3_43->Position * -1;
		eq_val[43] = TB_3_44->Position * -1;
		eq_val[44] = TB_3_45->Position * -1;
		eq_val[45] = TB_3_46->Position * -1;
		eq_val[46] = TB_3_47->Position * -1;
		eq_val[47] = TB_3_48->Position * -1;
		eq_val[48] = TB_3_49->Position * -1;
		eq_val[49] = TB_3_50->Position * -1;
		eq_val[50] = TB_3_51->Position * -1;
		eq_val[51] = TB_3_52->Position * -1;
		eq_val[52] = TB_3_53->Position * -1;
		eq_val[53] = TB_3_54->Position * -1;
		eq_val[54] = TB_3_55->Position * -1;
		eq_val[55] = TB_3_56->Position * -1;
		eq_val[56] = TB_3_57->Position * -1;
		eq_val[57] = TB_3_58->Position * -1;
		eq_val[58] = TB_3_59->Position * -1;
		eq_val[59] = TB_3_60->Position * -1;
		eq_val[60] = TB_3_61->Position * -1;
		eq_val[61] = TB_3_62->Position * -1;
		eq_val[62] = TB_3_63->Position * -1;
		eq_val[63] = TB_3_64->Position * -1;
		eq_val[64] = TB_3_65->Position * -1;
		eq_val[65] = TB_3_66->Position * -1;
		eq_val[66] = TB_3_67->Position * -1;
		eq_val[67] = TB_3_68->Position * -1;
		eq_val[68] = TB_3_69->Position * -1;
		eq_val[69] = TB_3_70->Position * -1;
		eq_val[70] = TB_3_71->Position * -1;
		eq_val[71] = TB_3_72->Position * -1;
		eq_val[72] = TB_3_73->Position * -1;
		eq_val[73] = TB_3_74->Position * -1;
		eq_val[74] = TB_3_75->Position * -1;
		eq_val[75] = TB_3_76->Position * -1;
		eq_val[76] = TB_3_77->Position * -1;
		eq_val[77] = TB_3_78->Position * -1;

		for (i = 0;i < 77;i++) {
			if (abs(eq_val[i] - eq_val[i+1]) >= 3) {
				disable_spline = 1;
			}
		}

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 78;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type3_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam3 = (UnicodeString)param;
		per = per1;
		if (rdo_type3_value_x2->Checked == true) {
			per = per2;
		}

		// スプライン補間により、なめらかに EQ 値をつなぐ。
		for (i = 0;i < 192000;i++) {
			eq[i] = 1.00;
		}
		pt2d[0].x = 0.5;
		pt2d[0].y = 1.0;
		pt2d[1].x = 1;
		pt2d[1].y = 1.0;
		pt2d[2].x = 2;
		pt2d[2].y = 1.0;
		pt2d[3].x = 3;
		pt2d[3].y = 1.0;
		pt2d[4].x = 4;
		pt2d[4].y = 1.0;
		for (i = 0;i < 78;i++) {
			pt2d[5 + i].x = eq_hz[i];
			pt2d[5 + i].y = per[14 + eq_val[i]];
		}
		if (eq_val[77] > 0) {
			// 0 レベルにする
			pt2d[83].x = eq_hz[78];
			pt2d[83].y = 1.0 + ((per[14 + eq_val[77]] - 1.0) / 4) * 3;
			pt2d[84].x = eq_hz[79];
			pt2d[84].y = 1.0 + ((per[14 + eq_val[77]] - 1.0) / 4) * 2;
			pt2d[85].x = eq_hz[80];
			pt2d[85].y = 1.0 + ((per[14 + eq_val[77]] - 1.0) / 4) * 1;
			pt2d[86].x = eq_hz[81];
			pt2d[86].y = 1.0;
			pt2d[87].x = eq_hz[82];
			pt2d[87].y = 1.0;
		} else {
			// 低いレベルを継続する
			pt2d[83].x = eq_hz[78];
			pt2d[83].y = per[14 + eq_val[77]];
			pt2d[84].x = eq_hz[79];
			pt2d[84].y = per[14 + eq_val[77]];
			pt2d[85].x = eq_hz[80];
			pt2d[85].y = per[14 + eq_val[77]];
			pt2d[86].x = eq_hz[81];
			pt2d[86].y = per[14 + eq_val[77]];
			pt2d[87].x = eq_hz[82];
			pt2d[87].y = per[14 + eq_val[77]];
		}
		for (x = 1;x <= eq_hz[82];x++) {
			y = spline(pt2d,88,x,h,b,d,g,u,r);
			eq[x] = y;
		}
		for (;x < 192000;x++) {
			eq[x] = y;
		}
		// データ補正
		for (i = 0;i < 78 - 2;i++) {
			start_len = 0;
			if (eq_val[i] == eq_val[i + 1] && eq_val[i + 1] == eq_val[i + 2]) {
				start_index = i;
				start_len   = 2;
				for (j = start_len + 1;start_index + j < 78 && eq_val[start_index] == eq_val[start_index + j];j++) {
					start_len = j;
				}
				for (j = eq_hz[start_index];j <= eq_hz[start_index + start_len];j++) {
					eq[j] = eq[eq_hz[start_index]];
				}
				i = start_index + start_len;
			}
		}
		// 数字にならないときはスプライン補間をしないようにする
		for (i = 0;i < 192000;i++) {
			if(IsNan(eq[i])) break;
		}
		if (i < 192000) {
			for (i = 0; i <= 82;i++) {
				for (x = pt2d[i].x; x < pt2d[i + 1].x;x++) {
					if (x < 192000) {
						eq[x] = pt2d[i].y;
					}
				}
			}
			v = pt2d[82].y;
			for (;x < 192000;x++) {
				eq[x] = v;
				if (v > 1.0) {
					v -= 0.1;
				}
			}
		}

		for (i = 0;i < 192000;i++) {
			fprintf(fp,"%lf\n",eq[i]);
		}
		for (;i < 384000;i++) {
			fprintf(fp,"%lf\n",eq[192000 - 1]);
		}
		fprintf(fp,"\nActiveEQ:3\n");
		sStr = eqParam1;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam2;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam3;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam4;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = edt_eq_title->Text;
		fprintf(fp,"%s\n",sStr.c_str());
		fclose(fp);
		edtParameter->Lines->Strings[cmbEQParameter->ItemIndex] = (UnicodeString)L"ActiveEQ:3" + L"|" +  eqParam1 + L"|" + eqParam2 + L"|" + eqParam3 + L"|" + eqParam4 + L"|" + edt_eq_title->Text;
	}
	PRINT_FN_LOG("SaveEQ_Type3","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SaveEQ_Type4(int test_mode)
{
	FILE *fp;
	int i,j,k,x,hz;
	int disable_spline;
	char *p,work[4096],param[4096],title[4096];
	int  eq_val[110];
	long eq_hz[]={5,6,8,10,12,16,20,25,31,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6000,7000,8000,9000,10000,11000,12000,13000,14000,15000,16000,17000,18000,19000,20000,21000,22000,23000,24000,25000,26000,27000,28000,29000,30000,31000,32000,33000,34000,35000,36000,37000,38000,39000,40000,45000,50000,55000,60000,65000,70000,75000,80000,85000,90000,95000,100000,105000,110000,115000,120000,125000,130000,135000,140000,145000,150000,155000,160000,165000,170000,175000,180000,185000,190000,195000,200000,202000,203000,204000,205000,206000};
	double per1[]={0.1995,0.2238,0.2511,0.2818,0.3162,0.3548,0.3981,0.4466,0.5011,0.5623,0.6309,0.7079,0.7943,0.8912,1.0,1.122,1.2589,1.4125,1.5848,1.7782,1.9952,2.2387,2.511,2.8183,3.1622,3.5481,3.981,4.4668,5.0118};
	double per2[]={0.0398,0.0501,0.0630,0.0794,0.1,0.1258,0.1584,0.1995,0.2511,0.3162,0.3981,0.5011,0.6309,0.7943,1.0,1.2589,1.5848,1.9952,2.5118,3.1622,3.9810,5.0118,6.3095,7.9432,10,12.5892,15.8489,19.9526,25.1188};
	double *per;

	PT2D pt2d[111];
	double h[110];
	double b[110];
	double d[110];
	double g[110];
	double u[110];
	double r[111];
	double y;
	double v;
	int start_index,start_len;
	UnicodeString uFileName;
	UnicodeString eqParam1,eqParam2,eqParam3,eqParam4;
	AnsiString sStr;

	PRINT_FN_LOG("SaveEQ_Type4","Start");
	if (test_mode == 0) {
		uFileName.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,cmbEQParameter->ItemIndex + 1);
	} else {
		uFileName.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,0);
	}
	disable_spline = 0;
	fp = fopen(((AnsiString)uFileName).c_str(),"wt");
	if (fp != NULL) {
		// EQ(Type1)
		eq_val[0]  = TB_1_1->Position * -1;
		eq_val[1]  = TB_1_2->Position * -1;
		eq_val[2]  = TB_1_3->Position * -1;
		eq_val[3]  = TB_1_4->Position * -1;
		eq_val[4]  = TB_1_5->Position * -1;
		eq_val[5]  = TB_1_6->Position * -1;
		eq_val[6]  = TB_1_7->Position * -1;
		eq_val[7]  = TB_1_8->Position * -1;
		eq_val[8]  = TB_1_9->Position * -1;
		eq_val[9]  = TB_1_10->Position * -1;
		eq_val[10] = TB_1_11->Position * -1;
		eq_val[11] = TB_1_12->Position * -1;
		eq_val[12] = TB_1_13->Position * -1;
		eq_val[13] = TB_1_14->Position * -1;
		eq_val[14] = TB_1_15->Position * -1;
		eq_val[15] = TB_1_16->Position * -1;
		eq_val[16] = TB_1_17->Position * -1;
		eq_val[17] = TB_1_18->Position * -1;
		eq_val[18] = TB_1_19->Position * -1;
		eq_val[19] = TB_1_20->Position * -1;
		eq_val[20] = TB_1_21->Position * -1;
		eq_val[21] = TB_1_22->Position * -1;
		eq_val[22] = TB_1_23->Position * -1;
		eq_val[23] = TB_1_24->Position * -1;
		eq_val[24] = TB_1_25->Position * -1;
		eq_val[25] = TB_1_26->Position * -1;
		eq_val[26] = TB_1_27->Position * -1;
		eq_val[27] = TB_1_28->Position * -1;
		eq_val[28] = TB_1_29->Position * -1;
		eq_val[29] = TB_1_30->Position * -1;
		eq_val[30] = TB_1_31->Position * -1;
		eq_val[31] = TB_1_32->Position * -1;
		eq_val[32] = TB_1_33->Position * -1;
		eq_val[33] = TB_1_34->Position * -1;
		eq_val[34] = TB_1_35->Position * -1;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 35;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type1_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam1 = (UnicodeString)param;

		// EQ(Type2)
		eq_val[0]  = TB_2_1->Position * -1;
		eq_val[1]  = TB_2_2->Position * -1;
		eq_val[2]  = TB_2_3->Position * -1;
		eq_val[3]  = TB_2_4->Position * -1;
		eq_val[4]  = TB_2_5->Position * -1;
		eq_val[5]  = TB_2_6->Position * -1;
		eq_val[6]  = TB_2_7->Position * -1;
		eq_val[7]  = TB_2_8->Position * -1;
		eq_val[8]  = TB_2_9->Position * -1;
		eq_val[9]  = TB_2_10->Position * -1;
		eq_val[10] = TB_2_11->Position * -1;
		eq_val[11] = TB_2_12->Position * -1;
		eq_val[12] = TB_2_13->Position * -1;
		eq_val[13] = TB_2_14->Position * -1;
		eq_val[14] = TB_2_15->Position * -1;
		eq_val[15] = TB_2_16->Position * -1;
		eq_val[16] = TB_2_17->Position * -1;
		eq_val[17] = TB_2_18->Position * -1;
		eq_val[18] = TB_2_19->Position * -1;
		eq_val[19] = TB_2_20->Position * -1;
		eq_val[20] = TB_2_21->Position * -1;
		eq_val[21] = TB_2_22->Position * -1;
		eq_val[22] = TB_2_23->Position * -1;
		eq_val[23] = TB_2_24->Position * -1;
		eq_val[24] = TB_2_25->Position * -1;
		eq_val[25] = TB_2_26->Position * -1;
		eq_val[26] = TB_2_27->Position * -1;
		eq_val[27] = TB_2_28->Position * -1;
		eq_val[28] = TB_2_29->Position * -1;
		eq_val[29] = TB_2_30->Position * -1;
		eq_val[30] = TB_2_31->Position * -1;
		eq_val[31] = TB_2_32->Position * -1;
		eq_val[32] = TB_2_33->Position * -1;
		eq_val[33] = TB_2_34->Position * -1;
		eq_val[34] = TB_2_35->Position * -1;
		eq_val[35] = TB_2_36->Position * -1;
		eq_val[36] = TB_2_37->Position * -1;
		eq_val[37] = TB_2_38->Position * -1;
		eq_val[38] = TB_2_39->Position * -1;
		eq_val[39] = TB_2_40->Position * -1;
		eq_val[40] = TB_2_41->Position * -1;
		eq_val[41] = TB_2_42->Position * -1;
		eq_val[42] = TB_2_43->Position * -1;
		eq_val[43] = TB_2_44->Position * -1;
		eq_val[44] = TB_2_45->Position * -1;
		eq_val[45] = TB_2_46->Position * -1;
		eq_val[46] = TB_2_47->Position * -1;
		eq_val[47] = TB_2_48->Position * -1;
		eq_val[48] = TB_2_49->Position * -1;
		eq_val[49] = TB_2_50->Position * -1;
		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 50;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type2_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}
		eqParam2 = (UnicodeString)param;

		// EQ(Type3)
		eq_val[0]  = TB_3_1->Position * -1;
		eq_val[1]  = TB_3_2->Position * -1;
		eq_val[2]  = TB_3_3->Position * -1;
		eq_val[3]  = TB_3_4->Position * -1;
		eq_val[4]  = TB_3_5->Position * -1;
		eq_val[5]  = TB_3_6->Position * -1;
		eq_val[6]  = TB_3_7->Position * -1;
		eq_val[7]  = TB_3_8->Position * -1;
		eq_val[8]  = TB_3_9->Position * -1;
		eq_val[9]  = TB_3_10->Position * -1;
		eq_val[10] = TB_3_11->Position * -1;
		eq_val[11] = TB_3_12->Position * -1;
		eq_val[12] = TB_3_13->Position * -1;
		eq_val[13] = TB_3_14->Position * -1;
		eq_val[14] = TB_3_15->Position * -1;
		eq_val[15] = TB_3_16->Position * -1;
		eq_val[16] = TB_3_17->Position * -1;
		eq_val[17] = TB_3_18->Position * -1;
		eq_val[18] = TB_3_19->Position * -1;
		eq_val[19] = TB_3_20->Position * -1;
		eq_val[20] = TB_3_21->Position * -1;
		eq_val[21] = TB_3_22->Position * -1;
		eq_val[22] = TB_3_23->Position * -1;
		eq_val[23] = TB_3_24->Position * -1;
		eq_val[24] = TB_3_25->Position * -1;
		eq_val[25] = TB_3_26->Position * -1;
		eq_val[26] = TB_3_27->Position * -1;
		eq_val[27] = TB_3_28->Position * -1;
		eq_val[28] = TB_3_29->Position * -1;
		eq_val[29] = TB_3_30->Position * -1;
		eq_val[30] = TB_3_31->Position * -1;
		eq_val[31] = TB_3_32->Position * -1;
		eq_val[32] = TB_3_33->Position * -1;
		eq_val[33] = TB_3_34->Position * -1;
		eq_val[34] = TB_3_35->Position * -1;
		eq_val[35] = TB_3_36->Position * -1;
		eq_val[36] = TB_3_37->Position * -1;
		eq_val[37] = TB_3_38->Position * -1;
		eq_val[38] = TB_3_39->Position * -1;
		eq_val[39] = TB_3_40->Position * -1;
		eq_val[40] = TB_3_41->Position * -1;
		eq_val[41] = TB_3_42->Position * -1;
		eq_val[42] = TB_3_43->Position * -1;
		eq_val[43] = TB_3_44->Position * -1;
		eq_val[44] = TB_3_45->Position * -1;
		eq_val[45] = TB_3_46->Position * -1;
		eq_val[46] = TB_3_47->Position * -1;
		eq_val[47] = TB_3_48->Position * -1;
		eq_val[48] = TB_3_49->Position * -1;
		eq_val[49] = TB_3_50->Position * -1;
		eq_val[50] = TB_3_51->Position * -1;
		eq_val[51] = TB_3_52->Position * -1;
		eq_val[52] = TB_3_53->Position * -1;
		eq_val[53] = TB_3_54->Position * -1;
		eq_val[54] = TB_3_55->Position * -1;
		eq_val[55] = TB_3_56->Position * -1;
		eq_val[56] = TB_3_57->Position * -1;
		eq_val[57] = TB_3_58->Position * -1;
		eq_val[58] = TB_3_59->Position * -1;
		eq_val[59] = TB_3_60->Position * -1;
		eq_val[60] = TB_3_61->Position * -1;
		eq_val[61] = TB_3_62->Position * -1;
		eq_val[62] = TB_3_63->Position * -1;
		eq_val[63] = TB_3_64->Position * -1;
		eq_val[64] = TB_3_65->Position * -1;
		eq_val[65] = TB_3_66->Position * -1;
		eq_val[66] = TB_3_67->Position * -1;
		eq_val[67] = TB_3_68->Position * -1;
		eq_val[68] = TB_3_69->Position * -1;
		eq_val[69] = TB_3_70->Position * -1;
		eq_val[70] = TB_3_71->Position * -1;
		eq_val[71] = TB_3_72->Position * -1;
		eq_val[72] = TB_3_73->Position * -1;
		eq_val[73] = TB_3_74->Position * -1;
		eq_val[74] = TB_3_75->Position * -1;
		eq_val[75] = TB_3_76->Position * -1;
		eq_val[76] = TB_3_77->Position * -1;
		eq_val[77] = TB_3_78->Position * -1;

		for (i = 0;i < 77;i++) {
			if (abs(eq_val[i] - eq_val[i+1]) >= 3) {
				disable_spline = 1;
			}
		}

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 78;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type3_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam3 = (UnicodeString)param;

		// EQ(Type4)
		eq_val[0]  = TB_4_1->Position * -1;
		eq_val[1]  = TB_4_2->Position * -1;
		eq_val[2]  = TB_4_3->Position * -1;
		eq_val[3]  = TB_4_4->Position * -1;
		eq_val[4]  = TB_4_5->Position * -1;
		eq_val[5]  = TB_4_6->Position * -1;
		eq_val[6]  = TB_4_7->Position * -1;
		eq_val[7]  = TB_4_8->Position * -1;
		eq_val[8]  = TB_4_9->Position * -1;
		eq_val[9]  = TB_4_10->Position * -1;
		eq_val[10] = TB_4_11->Position * -1;
		eq_val[11] = TB_4_12->Position * -1;
		eq_val[12] = TB_4_13->Position * -1;
		eq_val[13] = TB_4_14->Position * -1;
		eq_val[14] = TB_4_15->Position * -1;
		eq_val[15] = TB_4_16->Position * -1;
		eq_val[16] = TB_4_17->Position * -1;
		eq_val[17] = TB_4_18->Position * -1;
		eq_val[18] = TB_4_19->Position * -1;
		eq_val[19] = TB_4_20->Position * -1;
		eq_val[20] = TB_4_21->Position * -1;
		eq_val[21] = TB_4_22->Position * -1;
		eq_val[22] = TB_4_23->Position * -1;
		eq_val[23] = TB_4_24->Position * -1;
		eq_val[24] = TB_4_25->Position * -1;
		eq_val[25] = TB_4_26->Position * -1;
		eq_val[26] = TB_4_27->Position * -1;
		eq_val[27] = TB_4_28->Position * -1;
		eq_val[28] = TB_4_29->Position * -1;
		eq_val[29] = TB_4_30->Position * -1;
		eq_val[30] = TB_4_31->Position * -1;
		eq_val[31] = TB_4_32->Position * -1;
		eq_val[32] = TB_4_33->Position * -1;
		eq_val[33] = TB_4_34->Position * -1;
		eq_val[34] = TB_4_35->Position * -1;
		eq_val[35] = TB_4_36->Position * -1;
		eq_val[36] = TB_4_37->Position * -1;
		eq_val[37] = TB_4_38->Position * -1;
		eq_val[38] = TB_4_39->Position * -1;
		eq_val[39] = TB_4_40->Position * -1;
		eq_val[40] = TB_4_41->Position * -1;
		eq_val[41] = TB_4_42->Position * -1;
		eq_val[42] = TB_4_43->Position * -1;
		eq_val[43] = TB_4_44->Position * -1;
		eq_val[44] = TB_4_45->Position * -1;
		eq_val[45] = TB_4_46->Position * -1;
		eq_val[46] = TB_4_47->Position * -1;
		eq_val[47] = TB_4_48->Position * -1;
		eq_val[48] = TB_4_49->Position * -1;
		eq_val[49] = TB_4_50->Position * -1;
		eq_val[50] = TB_4_51->Position * -1;
		eq_val[51] = TB_4_52->Position * -1;
		eq_val[52] = TB_4_53->Position * -1;
		eq_val[53] = TB_4_54->Position * -1;
		eq_val[54] = TB_4_55->Position * -1;
		eq_val[55] = TB_4_56->Position * -1;
		eq_val[56] = TB_4_57->Position * -1;
		eq_val[57] = TB_4_58->Position * -1;
		eq_val[58] = TB_4_59->Position * -1;
		eq_val[59] = TB_4_60->Position * -1;
		eq_val[60] = TB_4_61->Position * -1;
		eq_val[61] = TB_4_62->Position * -1;
		eq_val[62] = TB_4_63->Position * -1;
		eq_val[63] = TB_4_64->Position * -1;
		eq_val[64] = TB_4_65->Position * -1;
		eq_val[65] = TB_4_66->Position * -1;
		eq_val[66] = TB_4_67->Position * -1;
		eq_val[67] = TB_4_68->Position * -1;
		eq_val[68] = TB_4_69->Position * -1;
		eq_val[69] = TB_4_70->Position * -1;
		eq_val[70] = TB_4_71->Position * -1;
		eq_val[71] = TB_4_72->Position * -1;
		eq_val[72] = TB_4_73->Position * -1;
		eq_val[73] = TB_4_74->Position * -1;
		eq_val[74] = TB_4_75->Position * -1;
		eq_val[75] = TB_4_76->Position * -1;
		eq_val[76] = TB_4_77->Position * -1;
		eq_val[77] = TB_4_78->Position * -1;
		eq_val[78] = TB_4_79->Position * -1;
		eq_val[79] = TB_4_80->Position * -1;
		eq_val[80] = TB_4_81->Position * -1;
		eq_val[81] = TB_4_82->Position * -1;
		eq_val[82] = TB_4_83->Position * -1;
		eq_val[83] = TB_4_84->Position * -1;
		eq_val[84] = TB_4_85->Position * -1;
		eq_val[85] = TB_4_86->Position * -1;
		eq_val[86] = TB_4_87->Position * -1;
		eq_val[87] = TB_4_88->Position * -1;
		eq_val[88] = TB_4_89->Position * -1;
		eq_val[89] = TB_4_90->Position * -1;
		eq_val[90] = TB_4_91->Position * -1;
		eq_val[91] = TB_4_92->Position * -1;
		eq_val[92] = TB_4_93->Position * -1;
		eq_val[93] = TB_4_94->Position * -1;
		eq_val[94] = TB_4_95->Position * -1;
		eq_val[95] = TB_4_96->Position * -1;
		eq_val[96] = TB_4_97->Position * -1;
		eq_val[97] = TB_4_98->Position * -1;

		for (i = 0;i < 97;i++) {
			if (abs(eq_val[i] - eq_val[i+1]) >= 3) {
				disable_spline = 1;
			}
		}

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 98;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type3_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam4 = (UnicodeString)param;

		per = per1;
		if (rdo_type3_value_x2->Checked == true) {
			per = per2;
		}

		// スプライン補間により、なめらかに EQ 値をつなぐ。
		for (i = 0;i < 384000;i++) {
			eq[i] = 1.00;
		}
		pt2d[0].x = 0.5;
		pt2d[0].y = 1.0;
		pt2d[1].x = 1;
		pt2d[1].y = 1.0;
		pt2d[2].x = 2;
		pt2d[2].y = 1.0;
		pt2d[3].x = 3;
		pt2d[3].y = 1.0;
		pt2d[4].x = 4;
		pt2d[4].y = 1.0;
		for (i = 0;i < 98;i++) {
			pt2d[5 + i].x = eq_hz[i];
			pt2d[5 + i].y = per[14 + eq_val[i]];
		}

		if (TB_4_1_Cutoff->Checked == true) pt2d[5 + 0].y = 0;
		if (TB_4_2_Cutoff->Checked == true) pt2d[5 + 1].y = 0;
		if (TB_4_3_Cutoff->Checked == true) pt2d[5 + 2].y = 0;
		if (TB_4_4_Cutoff->Checked == true) pt2d[5 + 3].y = 0;
		if (TB_4_5_Cutoff->Checked == true) pt2d[5 + 4].y = 0;
		if (TB_4_6_Cutoff->Checked == true) pt2d[5 + 5].y = 0;
		if (TB_4_7_Cutoff->Checked == true) pt2d[5 + 6].y = 0;
		if (TB_4_8_Cutoff->Checked == true) pt2d[5 + 7].y = 0;
		if (TB_4_9_Cutoff->Checked == true) pt2d[5 + 8].y = 0;
		if (TB_4_10_Cutoff->Checked == true) pt2d[5 + 9].y = 0;
		if (TB_4_11_Cutoff->Checked == true) pt2d[5 + 10].y = 0;
		if (TB_4_12_Cutoff->Checked == true) pt2d[5 + 11].y = 0;
		if (TB_4_13_Cutoff->Checked == true) pt2d[5 + 12].y = 0;
		if (TB_4_36_Cutoff->Checked == true) pt2d[5 + 35].y = 0;
		if (TB_4_37_Cutoff->Checked == true) pt2d[5 + 36].y = 0;
		if (TB_4_38_Cutoff->Checked == true) pt2d[5 + 37].y = 0;
		if (TB_4_39_Cutoff->Checked == true) pt2d[5 + 38].y = 0;
		if (TB_4_40_Cutoff->Checked == true) pt2d[5 + 39].y = 0;
		if (TB_4_41_Cutoff->Checked == true) pt2d[5 + 40].y = 0;
		if (TB_4_42_Cutoff->Checked == true) pt2d[5 + 41].y = 0;
		if (TB_4_43_Cutoff->Checked == true) pt2d[5 + 42].y = 0;
		if (TB_4_44_Cutoff->Checked == true) pt2d[5 + 43].y = 0;
		if (TB_4_45_Cutoff->Checked == true) pt2d[5 + 44].y = 0;
		if (TB_4_46_Cutoff->Checked == true) pt2d[5 + 45].y = 0;
		if (TB_4_47_Cutoff->Checked == true) pt2d[5 + 46].y = 0;
		if (TB_4_48_Cutoff->Checked == true) pt2d[5 + 47].y = 0;
		if (TB_4_49_Cutoff->Checked == true) pt2d[5 + 48].y = 0;
		if (TB_4_50_Cutoff->Checked == true) pt2d[5 + 49].y = 0;
		if (TB_4_51_Cutoff->Checked == true) pt2d[5 + 50].y = 0;
		if (TB_4_52_Cutoff->Checked == true) pt2d[5 + 51].y = 0;
		if (TB_4_53_Cutoff->Checked == true) pt2d[5 + 52].y = 0;
		if (TB_4_54_Cutoff->Checked == true) pt2d[5 + 53].y = 0;
		if (TB_4_55_Cutoff->Checked == true) pt2d[5 + 54].y = 0;
		if (TB_4_56_Cutoff->Checked == true) pt2d[5 + 55].y = 0;
		if (TB_4_57_Cutoff->Checked == true) pt2d[5 + 56].y = 0;
		if (TB_4_58_Cutoff->Checked == true) pt2d[5 + 57].y = 0;
		if (TB_4_59_Cutoff->Checked == true) pt2d[5 + 58].y = 0;
		if (TB_4_60_Cutoff->Checked == true) pt2d[5 + 59].y = 0;
		if (TB_4_61_Cutoff->Checked == true) pt2d[5 + 60].y = 0;
		if (TB_4_62_Cutoff->Checked == true) pt2d[5 + 61].y = 0;
		if (TB_4_63_Cutoff->Checked == true) pt2d[5 + 62].y = 0;
		if (TB_4_64_Cutoff->Checked == true) pt2d[5 + 63].y = 0;
		if (TB_4_65_Cutoff->Checked == true) pt2d[5 + 64].y = 0;
		if (TB_4_66_Cutoff->Checked == true) pt2d[5 + 65].y = 0;
		if (TB_4_67_Cutoff->Checked == true) pt2d[5 + 66].y = 0;
		if (TB_4_68_Cutoff->Checked == true) pt2d[5 + 67].y = 0;
		if (TB_4_69_Cutoff->Checked == true) pt2d[5 + 68].y = 0;
		if (TB_4_70_Cutoff->Checked == true) pt2d[5 + 69].y = 0;
		if (TB_4_71_Cutoff->Checked == true) pt2d[5 + 70].y = 0;
		if (TB_4_72_Cutoff->Checked == true) pt2d[5 + 71].y = 0;
		if (TB_4_73_Cutoff->Checked == true) pt2d[5 + 72].y = 0;
		if (TB_4_74_Cutoff->Checked == true) pt2d[5 + 73].y = 0;
		if (TB_4_75_Cutoff->Checked == true) pt2d[5 + 74].y = 0;
		if (TB_4_76_Cutoff->Checked == true) pt2d[5 + 75].y = 0;
		if (TB_4_77_Cutoff->Checked == true) pt2d[5 + 76].y = 0;
		if (TB_4_78_Cutoff->Checked == true) pt2d[5 + 77].y = 0;
		if (TB_4_79_Cutoff->Checked == true) pt2d[5 + 78].y = 0;
		if (TB_4_80_Cutoff->Checked == true) pt2d[5 + 79].y = 0;
		if (TB_4_81_Cutoff->Checked == true) pt2d[5 + 80].y = 0;
		if (TB_4_82_Cutoff->Checked == true) pt2d[5 + 81].y = 0;
		if (TB_4_83_Cutoff->Checked == true) pt2d[5 + 82].y = 0;
		if (TB_4_84_Cutoff->Checked == true) pt2d[5 + 83].y = 0;
		if (TB_4_85_Cutoff->Checked == true) pt2d[5 + 84].y = 0;
		if (TB_4_86_Cutoff->Checked == true) pt2d[5 + 85].y = 0;
		if (TB_4_87_Cutoff->Checked == true) pt2d[5 + 86].y = 0;
		if (TB_4_88_Cutoff->Checked == true) pt2d[5 + 87].y = 0;
		if (TB_4_89_Cutoff->Checked == true) pt2d[5 + 88].y = 0;
		if (TB_4_90_Cutoff->Checked == true) pt2d[5 + 89].y = 0;
		if (TB_4_91_Cutoff->Checked == true) pt2d[5 + 90].y = 0;
		if (TB_4_92_Cutoff->Checked == true) pt2d[5 + 91].y = 0;
		if (TB_4_93_Cutoff->Checked == true) pt2d[5 + 92].y = 0;
		if (TB_4_94_Cutoff->Checked == true) pt2d[5 + 93].y = 0;
		if (TB_4_95_Cutoff->Checked == true) pt2d[5 + 94].y = 0;
		if (TB_4_96_Cutoff->Checked == true) pt2d[5 + 95].y = 0;
		if (TB_4_97_Cutoff->Checked == true) pt2d[5 + 96].y = 0;
		if (TB_4_98_Cutoff->Checked == true) pt2d[5 + 97].y = 0;

		if (eq_val[97] > 0) {
			// 0 レベルにする
			pt2d[103].x = eq_hz[98];
			pt2d[103].y = 1.0 + ((per[14 + eq_val[97]] - 1.0) / 4) * 3;
			pt2d[104].x = eq_hz[99];
			pt2d[104].y = 1.0 + ((per[14 + eq_val[97]] - 1.0) / 4) * 2;
			pt2d[105].x = eq_hz[100];
			pt2d[105].y = 1.0 + ((per[14 + eq_val[97]] - 1.0) / 4) * 1;
			pt2d[106].x = eq_hz[101];
			pt2d[106].y = 1.0;
			pt2d[107].x = eq_hz[102];
			pt2d[107].y = 1.0;
		} else {
			// 低いレベルを継続する
			pt2d[103].x = eq_hz[98];
			pt2d[103].y = per[14 + eq_val[97]];
			pt2d[104].x = eq_hz[99];
			pt2d[104].y = per[14 + eq_val[97]];
			pt2d[105].x = eq_hz[100];
			pt2d[105].y = per[14 + eq_val[97]];
			pt2d[106].x = eq_hz[101];
			pt2d[106].y = per[14 + eq_val[97]];
			pt2d[107].x = eq_hz[102];
			pt2d[107].y = per[14 + eq_val[97]];
		}
		for (x = 1;x <= eq_hz[102];x++) {
			y = spline(pt2d,108,x,h,b,d,g,u,r);
			eq[x] = y;
		}
		for (;x < 384000;x++) {
			eq[x] = y;
		}
		// データ補正
		for (i = 0;i < 98 - 2;i++) {
			start_len = 0;
			if (eq_val[i] == eq_val[i + 1] && eq_val[i + 1] == eq_val[i + 2]) {
				start_index = i;
				start_len   = 2;
				for (j = start_len + 1;start_index + j < 98 && eq_val[start_index] == eq_val[start_index + j];j++) {
					start_len = j;
				}
				for (j = eq_hz[start_index];j <= eq_hz[start_index + start_len];j++) {
					eq[j] = eq[eq_hz[start_index]];
				}
				i = start_index + start_len;
			}
		}
		// 数字にならないときはスプライン補間をしないようにする
		for (i = 0;i < 384000;i++) {
			if(IsNan(eq[i])) break;
		}
		if (i < 384000) {
			for (i = 0; i <= 102;i++) {
				for (x = pt2d[i].x; x < pt2d[i + 1].x;x++) {
					if (x < 384000) {
						eq[x] = pt2d[i].y;
					}
				}
			}
			v = pt2d[102].y;
			for (;x < 384000;x++) {
				eq[x] = v;
				if (v > 1.0) {
					v -= 0.1;
				}
			}
		}

		for (i = 0;i < 200000;i++) {
			fprintf(fp,"%lf\n",eq[i]);
		}
		for (;i < 384000;i++) {
			fprintf(fp,"%lf\n",eq[200000 - 1]);
		}

		// 指定周波数のカットオフ対応
		if (TB_4_1_Cutoff->Checked == true) eq_val[0] = -99;
		if (TB_4_2_Cutoff->Checked == true) eq_val[1] = -99;
		if (TB_4_3_Cutoff->Checked == true) eq_val[2] = -99;
		if (TB_4_4_Cutoff->Checked == true) eq_val[3] = -99;
		if (TB_4_5_Cutoff->Checked == true) eq_val[4] = -99;
		if (TB_4_6_Cutoff->Checked == true) eq_val[5] = -99;
		if (TB_4_7_Cutoff->Checked == true) eq_val[6] = -99;
		if (TB_4_8_Cutoff->Checked == true) eq_val[7] = -99;
		if (TB_4_9_Cutoff->Checked == true) eq_val[8] = -99;
		if (TB_4_10_Cutoff->Checked == true) eq_val[9] = -99;
		if (TB_4_11_Cutoff->Checked == true) eq_val[10] = -99;
		if (TB_4_12_Cutoff->Checked == true) eq_val[11] = -99;
		if (TB_4_13_Cutoff->Checked == true) eq_val[12] = -99;
		if (TB_4_36_Cutoff->Checked == true) eq_val[35] = -99;
		if (TB_4_37_Cutoff->Checked == true) eq_val[36] = -99;
		if (TB_4_38_Cutoff->Checked == true) eq_val[37] = -99;
		if (TB_4_39_Cutoff->Checked == true) eq_val[38] = -99;
		if (TB_4_40_Cutoff->Checked == true) eq_val[39] = -99;
		if (TB_4_41_Cutoff->Checked == true) eq_val[40] = -99;
		if (TB_4_42_Cutoff->Checked == true) eq_val[41] = -99;
		if (TB_4_43_Cutoff->Checked == true) eq_val[42] = -99;
		if (TB_4_44_Cutoff->Checked == true) eq_val[43] = -99;
		if (TB_4_45_Cutoff->Checked == true) eq_val[44] = -99;
		if (TB_4_46_Cutoff->Checked == true) eq_val[45] = -99;
		if (TB_4_47_Cutoff->Checked == true) eq_val[46] = -99;
		if (TB_4_48_Cutoff->Checked == true) eq_val[47] = -99;
		if (TB_4_49_Cutoff->Checked == true) eq_val[48] = -99;
		if (TB_4_50_Cutoff->Checked == true) eq_val[49] = -99;
		if (TB_4_51_Cutoff->Checked == true) eq_val[50] = -99;
		if (TB_4_52_Cutoff->Checked == true) eq_val[51] = -99;
		if (TB_4_53_Cutoff->Checked == true) eq_val[52] = -99;
		if (TB_4_54_Cutoff->Checked == true) eq_val[53] = -99;
		if (TB_4_55_Cutoff->Checked == true) eq_val[54] = -99;
		if (TB_4_56_Cutoff->Checked == true) eq_val[55] = -99;
		if (TB_4_57_Cutoff->Checked == true) eq_val[56] = -99;
		if (TB_4_58_Cutoff->Checked == true) eq_val[57] = -99;
		if (TB_4_59_Cutoff->Checked == true) eq_val[58] = -99;
		if (TB_4_60_Cutoff->Checked == true) eq_val[59] = -99;
		if (TB_4_61_Cutoff->Checked == true) eq_val[60] = -99;
		if (TB_4_62_Cutoff->Checked == true) eq_val[61] = -99;
		if (TB_4_63_Cutoff->Checked == true) eq_val[62] = -99;
		if (TB_4_64_Cutoff->Checked == true) eq_val[63] = -99;
		if (TB_4_65_Cutoff->Checked == true) eq_val[64] = -99;
		if (TB_4_66_Cutoff->Checked == true) eq_val[65] = -99;
		if (TB_4_67_Cutoff->Checked == true) eq_val[66] = -99;
		if (TB_4_68_Cutoff->Checked == true) eq_val[67] = -99;
		if (TB_4_69_Cutoff->Checked == true) eq_val[68] = -99;
		if (TB_4_70_Cutoff->Checked == true) eq_val[69] = -99;
		if (TB_4_71_Cutoff->Checked == true) eq_val[70] = -99;
		if (TB_4_72_Cutoff->Checked == true) eq_val[71] = -99;
		if (TB_4_73_Cutoff->Checked == true) eq_val[72] = -99;
		if (TB_4_74_Cutoff->Checked == true) eq_val[73] = -99;
		if (TB_4_75_Cutoff->Checked == true) eq_val[74] = -99;
		if (TB_4_76_Cutoff->Checked == true) eq_val[75] = -99;
		if (TB_4_77_Cutoff->Checked == true) eq_val[76] = -99;
		if (TB_4_78_Cutoff->Checked == true) eq_val[77] = -99;
		if (TB_4_79_Cutoff->Checked == true) eq_val[78] = -99;
		if (TB_4_80_Cutoff->Checked == true) eq_val[79] = -99;
		if (TB_4_81_Cutoff->Checked == true) eq_val[80] = -99;
		if (TB_4_82_Cutoff->Checked == true) eq_val[81] = -99;
		if (TB_4_83_Cutoff->Checked == true) eq_val[82] = -99;
		if (TB_4_84_Cutoff->Checked == true) eq_val[83] = -99;
		if (TB_4_85_Cutoff->Checked == true) eq_val[84] = -99;
		if (TB_4_86_Cutoff->Checked == true) eq_val[85] = -99;
		if (TB_4_87_Cutoff->Checked == true) eq_val[86] = -99;
		if (TB_4_88_Cutoff->Checked == true) eq_val[87] = -99;
		if (TB_4_89_Cutoff->Checked == true) eq_val[88] = -99;
		if (TB_4_90_Cutoff->Checked == true) eq_val[89] = -99;
		if (TB_4_91_Cutoff->Checked == true) eq_val[90] = -99;
		if (TB_4_92_Cutoff->Checked == true) eq_val[91] = -99;
		if (TB_4_93_Cutoff->Checked == true) eq_val[92] = -99;
		if (TB_4_94_Cutoff->Checked == true) eq_val[93] = -99;
		if (TB_4_95_Cutoff->Checked == true) eq_val[94] = -99;
		if (TB_4_96_Cutoff->Checked == true) eq_val[95] = -99;
		if (TB_4_97_Cutoff->Checked == true) eq_val[96] = -99;
		if (TB_4_98_Cutoff->Checked == true) eq_val[97] = -99;

		param[0] = '\0';
		p = param;
		sprintf(p,"%d",eq_val[0]);
		for (i = 1;i < 98;i++) {
			sprintf(work,",%d",eq_val[i]);
			strcat(p,work);
		}
		if (rdo_type3_value_x1->Checked == true) {
			sprintf(work,",%d",1);
			strcat(p,work);
		} else {
			sprintf(work,",%d",2);
			strcat(p,work);
		}

		eqParam4 = (UnicodeString)param;

		fprintf(fp,"\nActiveEQ:4\n");
		sStr = eqParam1;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam2;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam3;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = eqParam4;
		fprintf(fp,"%s\n",sStr.c_str());
		sStr = edt_eq_title->Text;
		fprintf(fp,"%s\n",sStr.c_str());
		fclose(fp);
		edtParameter->Lines->Strings[cmbEQParameter->ItemIndex] = (UnicodeString)L"ActiveEQ:4" + L"|" +  eqParam1 + L"|" + eqParam2 + L"|" + eqParam3 + L"|" + eqParam3 + L"|" + edt_eq_title->Text;
	}
	PRINT_FN_LOG("SaveEQ_Type4","End");
}
//---------------------------------------------------------------------------
/* スプライン補間
 * pn : 点群の配列
 * n  : 点群の個数
 * x  : 求めたいxの値
 * return : xに対応する関数値y
 *
 * 参考
 * https://www.sist.ac.jp/~suganuma/programming/9-sho/num/spline/spline.htm
 */
double __fastcall TForm1::spline(PT2D *pt,int n,double x1,double *h, double *b, double *d, double *g, double *u, double *r)
{
	int i = -1, i1, k;
	double y1, qi, si, xx;

//	PRINT_FN_LOG("spline","Start");

	// 区間の決定
	for (i1 = 1; i1 < n && i < 0; i1++) {
		if (x1 < pt[i1].x)
			i = i1 - 1;
	}

	if (i < 0)
		i = n - 1;

	// ステップ１
	for (i1 = 0; i1 < n; i1++)
		h[i1] = pt[i1+1].x - pt[i1].x;
	for (i1 = 1; i1 < n; i1++) {
		b[i1] = 2.0 * (h[i1] + h[i1-1]);
		d[i1] = 3.0 * ((pt[i1+1].y - pt[i1].y) / h[i1] - (pt[i1].y - pt[i1-1].y) / h[i1-1]);
	}

	// ステップ２
	g[1] = h[1] / b[1];
	for (i1 = 2; i1 < n-1; i1++)
		g[i1] = h[i1] / (b[i1] - h[i1-1] * g[i1-1]);

	u[1] = d[1] / b[1];
	for (i1 = 2; i1 < n; i1++)
		u[i1] = (d[i1] - h[i1-1] * u[i1-1]) / (b[i1] - h[i1-1] * g[i1-1]);

	// ステップ３
	k      = (i > 1) ? i : 1;
	r[0]   = 0.0;
	r[n]   = 0.0;
	r[n-1] = u[n-1];
	for (i1 = n-2; i1 >= k; i1--)
		r[i1] = u[i1] - g[i1] * r[i1+1];
					// ステップ４
	xx = x1 - pt[i].x;
	qi = (pt[i+1].y - pt[i].y) / h[i] - h[i] * (r[i+1] + 2.0 * r[i]) / 3.0;
	si = (r[i+1] - r[i]) / (3.0 * h[i]);
	y1 = pt[i].y + xx * (qi + xx * (r[i]  + si * xx));

//	PRINT_FN_LOG("spline","End");
	return y1;
}
//---------------------------------------------------------------------------
/*
 * pn : 点群の配列
 * n  : 点群の個数
 * x  : 求めたいxの値
 * return : xに対応する関数値y
 *
 * 参考
 * http://www.isc.meiji.ac.jp/~mcelab/www_jyo_en2/jyo_en_2_4_j_2015_f/index.html
 */
double __fastcall TForm1::lag_interp(PT2D *pt,int n,double x)
{
	int i,j;
	/* 計算用変数の初期化 */
	double p, s = 0.0;

	for (i = 0; i < n; i++) {

		p = pt[i].y;   /* 積の計算 */

		for (j = 0; j < n; j++) {
			if (i != j) {
				p *= (x - pt[j].x) / (pt[i].x - pt[j].x);
			}
		}
		s += p;
	}

	return s;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_LoadClick(TObject *Sender)
{
	int modify;
	int def_param;
	int ii;
	char param[8192];

	PRINT_FN_LOG("btn_LoadClick","Start");

	modify = CheckModifyEQ();
	if (modify == 1) {
		if (Application->MessageBoxW(L"Save?",L"EQ",MB_OKCANCEL) == idOK) {
			btn_eq_saveClick(Sender);
		}
		btnLoad->Tag = 0;
	}
	def_param = cmbEQParameter->ItemIndex;
	if (edtParameter->Lines->Strings[def_param].Length() == 0) {
		eq_val[def_param] = (double *)malloc(192000 * sizeof (double));
		if (eq_val[def_param] != NULL) {
			for (ii = 0;ii < 400000;ii++) {
				eq_val[def_param][ii] = 1.0;
			}
			strcpy(param,"ActiveEQ:1|0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1|0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1|0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1|0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1|EQ.Sample");
			edtParameter->Lines->Strings[def_param] = (UnicodeString)param;	// EQ 値を1行にした値を保持する
		}
	}
	ReloadDefaultParameter(def_param + 1, def_param + 1);
	btn_eq_save->Enabled = true;
	btnLoad->Tag = 0;
	PRINT_FN_LOG("btn_LoadClick","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_1Change(TObject *Sender)
{
	eq_1_1->Caption = TB_1_1->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_2Change(TObject *Sender)
{
	eq_1_2->Caption = TB_1_2->Position * -1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TB_1_3Change(TObject *Sender)
{
	eq_1_3->Caption = TB_1_3->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_4Change(TObject *Sender)
{
	eq_1_4->Caption = TB_1_4->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_5Change(TObject *Sender)
{
	eq_1_5->Caption = TB_1_5->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_6Change(TObject *Sender)
{
	eq_1_6->Caption = TB_1_6->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_7Change(TObject *Sender)
{
	eq_1_7->Caption = TB_1_7->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_8Change(TObject *Sender)
{
	eq_1_8->Caption = TB_1_8->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_9Change(TObject *Sender)
{
	eq_1_9->Caption = TB_1_9->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_10Change(TObject *Sender)
{
	eq_1_10->Caption = TB_1_10->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_11Change(TObject *Sender)
{
	eq_1_11->Caption = TB_1_11->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_12Change(TObject *Sender)
{
	eq_1_12->Caption = TB_1_12->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_13Change(TObject *Sender)
{
	eq_1_13->Caption = TB_1_13->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_14Change(TObject *Sender)
{
	eq_1_14->Caption = TB_1_14->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_15Change(TObject *Sender)
{
	eq_1_15->Caption = TB_1_15->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_16Change(TObject *Sender)
{
	eq_1_16->Caption = TB_1_16->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_17Change(TObject *Sender)
{
	eq_1_17->Caption = TB_1_17->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_18Change(TObject *Sender)
{
	eq_1_18->Caption = TB_1_18->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_19Change(TObject *Sender)
{
	eq_1_19->Caption = TB_1_19->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_20Change(TObject *Sender)
{
	eq_1_20->Caption = TB_1_20->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_21Change(TObject *Sender)
{
	eq_1_21->Caption = TB_1_21->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_22Change(TObject *Sender)
{
	eq_1_22->Caption = TB_1_22->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_23Change(TObject *Sender)
{
	eq_1_23->Caption = TB_1_23->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_24Change(TObject *Sender)
{
	eq_1_24->Caption = TB_1_24->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_25Change(TObject *Sender)
{
	eq_1_25->Caption = TB_1_25->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_26Change(TObject *Sender)
{
	eq_1_26->Caption = TB_1_26->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_27Change(TObject *Sender)
{
	eq_1_27->Caption = TB_1_27->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_28Change(TObject *Sender)
{
	eq_1_28->Caption = TB_1_28->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_29Change(TObject *Sender)
{
	eq_1_29->Caption = TB_1_29->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_30Change(TObject *Sender)
{
	eq_1_30->Caption = TB_1_30->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_31Change(TObject *Sender)
{
	eq_1_31->Caption = TB_1_31->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_32Change(TObject *Sender)
{
	eq_1_32->Caption = TB_1_32->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_33Change(TObject *Sender)
{
	eq_1_33->Caption = TB_1_33->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_34Change(TObject *Sender)
{
	eq_1_34->Caption = TB_1_34->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_1_35Change(TObject *Sender)
{
	eq_1_35->Caption = TB_1_35->Position * -1;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::btn_type1_all_centerClick(TObject *Sender)
{
	TB_1_1->Position = 0;
	TB_1_2->Position = 0;
	TB_1_3->Position = 0;
	TB_1_4->Position = 0;
	TB_1_5->Position = 0;
	TB_1_6->Position = 0;
	TB_1_7->Position = 0;
	TB_1_8->Position = 0;
	TB_1_9->Position = 0;
	TB_1_10->Position = 0;
	TB_1_11->Position = 0;
	TB_1_12->Position = 0;
	TB_1_13->Position = 0;
	TB_1_14->Position = 0;
	TB_1_15->Position = 0;
	TB_1_16->Position = 0;
	TB_1_17->Position = 0;
	TB_1_18->Position = 0;
	TB_1_19->Position = 0;
	TB_1_20->Position = 0;
	TB_1_21->Position = 0;
	TB_1_22->Position = 0;
	TB_1_23->Position = 0;
	TB_1_24->Position = 0;
	TB_1_25->Position = 0;
	TB_1_26->Position = 0;
	TB_1_27->Position = 0;
	TB_1_28->Position = 0;
	TB_1_29->Position = 0;
	TB_1_30->Position = 0;
	TB_1_31->Position = 0;
	TB_1_32->Position = 0;
	TB_1_33->Position = 0;
	TB_1_34->Position = 0;
	TB_1_35->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type1_all_upClick(TObject *Sender)
{
	int range_from,range_to;

	range_from = -1;
	range_to   = 999;
	if (cmbT1_range_from->ItemIndex > 0 && cmbT1_range_to->ItemIndex > 0) {
		if (cmbT1_range_from->ItemIndex <= cmbT1_range_to->ItemIndex) {
			range_from = cmbT1_range_from->ItemIndex;
			range_to   = cmbT1_range_to->ItemIndex;
		} else {
			range_to = cmbT1_range_to->ItemIndex;
			range_from   = cmbT1_range_from->ItemIndex;
		}
	} else {
		if (cmbT1_range_from->ItemIndex > 0) {
			range_from = cmbT1_range_from->ItemIndex;
		}
		if (cmbT1_range_to->ItemIndex > 0) {
			range_to = cmbT1_range_to->ItemIndex;
		}
	}

	if (range_from <= 1  && range_to >= 1) TB_1_1->Position = TB_1_1->Position - 1;
	if (range_from <= 2  && range_to >= 2) TB_1_2->Position = TB_1_2->Position - 1;
	if (range_from <= 3  && range_to >= 3) TB_1_3->Position = TB_1_3->Position - 1;
	if (range_from <= 4  && range_to >= 4) TB_1_4->Position = TB_1_4->Position - 1;
	if (range_from <= 5  && range_to >= 5) TB_1_5->Position = TB_1_5->Position - 1;
	if (range_from <= 6  && range_to >= 6) TB_1_6->Position = TB_1_6->Position - 1;
	if (range_from <= 7  && range_to >= 7) TB_1_7->Position = TB_1_7->Position - 1;
	if (range_from <= 8  && range_to >= 8) TB_1_8->Position = TB_1_8->Position - 1;
	if (range_from <= 9  && range_to >= 9) TB_1_9->Position = TB_1_9->Position - 1;
	if (range_from <= 10 && range_to >= 10) TB_1_10->Position = TB_1_10->Position - 1;
	if (range_from <= 11 && range_to >= 11) TB_1_11->Position = TB_1_11->Position - 1;
	if (range_from <= 12 && range_to >= 12) TB_1_12->Position = TB_1_12->Position - 1;
	if (range_from <= 13 && range_to >= 13) TB_1_13->Position = TB_1_13->Position - 1;
	if (range_from <= 14 && range_to >= 14) TB_1_14->Position = TB_1_14->Position - 1;
	if (range_from <= 15 && range_to >= 15) TB_1_15->Position = TB_1_15->Position - 1;
	if (range_from <= 16 && range_to >= 16) TB_1_16->Position = TB_1_16->Position - 1;
	if (range_from <= 17 && range_to >= 17) TB_1_17->Position = TB_1_17->Position - 1;
	if (range_from <= 18 && range_to >= 18) TB_1_18->Position = TB_1_18->Position - 1;
	if (range_from <= 19 && range_to >= 19) TB_1_19->Position = TB_1_19->Position - 1;
	if (range_from <= 20 && range_to >= 20) TB_1_20->Position = TB_1_20->Position - 1;
	if (range_from <= 21 && range_to >= 21) TB_1_21->Position = TB_1_21->Position - 1;
	if (range_from <= 22 && range_to >= 22) TB_1_22->Position = TB_1_22->Position - 1;
	if (range_from <= 23 && range_to >= 23) TB_1_23->Position = TB_1_23->Position - 1;
	if (range_from <= 24 && range_to >= 24) TB_1_24->Position = TB_1_24->Position - 1;
	if (range_from <= 25 && range_to >= 25) TB_1_25->Position = TB_1_25->Position - 1;
	if (range_from <= 26 && range_to >= 26) TB_1_26->Position = TB_1_26->Position - 1;
	if (range_from <= 27 && range_to >= 27) TB_1_27->Position = TB_1_27->Position - 1;
	if (range_from <= 28 && range_to >= 28) TB_1_28->Position = TB_1_28->Position - 1;
	if (range_from <= 29 && range_to >= 29) TB_1_29->Position = TB_1_29->Position - 1;
	if (range_from <= 30 && range_to >= 30) TB_1_30->Position = TB_1_30->Position - 1;
	if (range_from <= 31 && range_to >= 31) TB_1_31->Position = TB_1_31->Position - 1;
	if (range_from <= 32 && range_to >= 32) TB_1_32->Position = TB_1_32->Position - 1;
	if (range_from <= 33 && range_to >= 33) TB_1_33->Position = TB_1_33->Position - 1;
	if (range_from <= 34 && range_to >= 34) TB_1_34->Position = TB_1_34->Position - 1;
	if (range_from <= 35 && range_to >= 35) TB_1_35->Position = TB_1_35->Position - 1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type1_all_downClick(TObject *Sender)
{
	int range_from,range_to;

	range_from = -1;
	range_to   = 999;
	if (cmbT1_range_from->ItemIndex > 0 && cmbT1_range_to->ItemIndex > 0) {
		if (cmbT1_range_from->ItemIndex <= cmbT1_range_to->ItemIndex) {
			range_from = cmbT1_range_from->ItemIndex;
			range_to   = cmbT1_range_to->ItemIndex;
		} else {
			range_to = cmbT1_range_to->ItemIndex;
			range_from   = cmbT1_range_from->ItemIndex;
		}
	} else {
		if (cmbT1_range_from->ItemIndex > 0) {
			range_from = cmbT1_range_from->ItemIndex;
		}
		if (cmbT1_range_to->ItemIndex > 0) {
			range_to = cmbT1_range_to->ItemIndex;
		}
	}
	if (range_from <= 1  && range_to >= 1) TB_1_1->Position = TB_1_1->Position + 1;
	if (range_from <= 2  && range_to >= 2) TB_1_2->Position = TB_1_2->Position + 1;
	if (range_from <= 3  && range_to >= 3) TB_1_3->Position = TB_1_3->Position + 1;
	if (range_from <= 4  && range_to >= 4) TB_1_4->Position = TB_1_4->Position + 1;
	if (range_from <= 5  && range_to >= 5) TB_1_5->Position = TB_1_5->Position + 1;
	if (range_from <= 6  && range_to >= 6) TB_1_6->Position = TB_1_6->Position + 1;
	if (range_from <= 7  && range_to >= 7) TB_1_7->Position = TB_1_7->Position + 1;
	if (range_from <= 8  && range_to >= 8) TB_1_8->Position = TB_1_8->Position + 1;
	if (range_from <= 9  && range_to >= 9) TB_1_9->Position = TB_1_9->Position + 1;
	if (range_from <= 10  && range_to >= 10) TB_1_10->Position = TB_1_10->Position + 1;
	if (range_from <= 11  && range_to >= 11) TB_1_11->Position = TB_1_11->Position + 1;
	if (range_from <= 12  && range_to >= 12) TB_1_12->Position = TB_1_12->Position + 1;
	if (range_from <= 13  && range_to >= 13) TB_1_13->Position = TB_1_13->Position + 1;
	if (range_from <= 14  && range_to >= 14) TB_1_14->Position = TB_1_14->Position + 1;
	if (range_from <= 15  && range_to >= 15) TB_1_15->Position = TB_1_15->Position + 1;
	if (range_from <= 16  && range_to >= 16) TB_1_16->Position = TB_1_16->Position + 1;
	if (range_from <= 17  && range_to >= 17) TB_1_17->Position = TB_1_17->Position + 1;
	if (range_from <= 18  && range_to >= 18) TB_1_18->Position = TB_1_18->Position + 1;
	if (range_from <= 19  && range_to >= 19) TB_1_19->Position = TB_1_19->Position + 1;
	if (range_from <= 20  && range_to >= 20) TB_1_20->Position = TB_1_20->Position + 1;
	if (range_from <= 21  && range_to >= 21) TB_1_21->Position = TB_1_21->Position + 1;
	if (range_from <= 22  && range_to >= 22) TB_1_22->Position = TB_1_22->Position + 1;
	if (range_from <= 23  && range_to >= 23) TB_1_23->Position = TB_1_23->Position + 1;
	if (range_from <= 24  && range_to >= 24) TB_1_24->Position = TB_1_24->Position + 1;
	if (range_from <= 25  && range_to >= 25) TB_1_25->Position = TB_1_25->Position + 1;
	if (range_from <= 26  && range_to >= 26) TB_1_26->Position = TB_1_26->Position + 1;
	if (range_from <= 27  && range_to >= 27) TB_1_27->Position = TB_1_27->Position + 1;
	if (range_from <= 28  && range_to >= 28) TB_1_28->Position = TB_1_28->Position + 1;
	if (range_from <= 29  && range_to >= 29) TB_1_29->Position = TB_1_29->Position + 1;
	if (range_from <= 30  && range_to >= 30) TB_1_30->Position = TB_1_30->Position + 1;
	if (range_from <= 31  && range_to >= 31) TB_1_31->Position = TB_1_31->Position + 1;
	if (range_from <= 32  && range_to >= 32) TB_1_32->Position = TB_1_32->Position + 1;
	if (range_from <= 33  && range_to >= 33) TB_1_33->Position = TB_1_33->Position + 1;
	if (range_from <= 34  && range_to >= 34) TB_1_34->Position = TB_1_34->Position + 1;
	if (range_from <= 35  && range_to >= 35) TB_1_35->Position = TB_1_35->Position + 1;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::TB_2_1Change(TObject *Sender)
{
	eq_2_1->Caption = TB_2_1->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_2Change(TObject *Sender)
{
	eq_2_2->Caption = TB_2_2->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_3Change(TObject *Sender)
{
	eq_2_3->Caption = TB_2_3->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_4Change(TObject *Sender)
{
	eq_2_4->Caption = TB_2_4->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_5Change(TObject *Sender)
{
	eq_2_5->Caption = TB_2_5->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_6Change(TObject *Sender)
{
	eq_2_6->Caption = TB_2_6->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_7Change(TObject *Sender)
{
	eq_2_7->Caption = TB_2_7->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_8Change(TObject *Sender)
{
	eq_2_8->Caption = TB_2_8->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_9Change(TObject *Sender)
{
	eq_2_9->Caption = TB_2_9->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_10Change(TObject *Sender)
{
	eq_2_10->Caption = TB_2_10->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_11Change(TObject *Sender)
{
	eq_2_11->Caption = TB_2_11->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_12Change(TObject *Sender)
{
	eq_2_12->Caption = TB_2_12->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_13Change(TObject *Sender)
{
	eq_2_13->Caption = TB_2_13->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_14Change(TObject *Sender)
{
	eq_2_14->Caption = TB_2_14->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_15Change(TObject *Sender)
{
	eq_2_15->Caption = TB_2_15->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_16Change(TObject *Sender)
{
	eq_2_16->Caption = TB_2_16->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_17Change(TObject *Sender)
{
	eq_2_17->Caption = TB_2_17->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_18Change(TObject *Sender)
{
	eq_2_18->Caption = TB_2_18->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_19Change(TObject *Sender)
{
	eq_2_19->Caption = TB_2_19->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_20Change(TObject *Sender)
{
	eq_2_20->Caption = TB_2_20->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_21Change(TObject *Sender)
{
	eq_2_21->Caption = TB_2_21->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_22Change(TObject *Sender)
{
	eq_2_22->Caption = TB_2_22->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_23Change(TObject *Sender)
{
	eq_2_23->Caption = TB_2_23->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_24Change(TObject *Sender)
{
	eq_2_24->Caption = TB_2_24->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_25Change(TObject *Sender)
{
	eq_2_25->Caption = TB_2_25->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_26Change(TObject *Sender)
{
	eq_2_26->Caption = TB_2_26->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_27Change(TObject *Sender)
{
	eq_2_27->Caption = TB_2_27->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_28Change(TObject *Sender)
{
	eq_2_28->Caption = TB_2_28->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_29Change(TObject *Sender)
{
	eq_2_29->Caption = TB_2_29->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_30Change(TObject *Sender)
{
	eq_2_30->Caption = TB_2_30->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_31Change(TObject *Sender)
{
	eq_2_31->Caption = TB_2_31->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_32Change(TObject *Sender)
{
	eq_2_32->Caption = TB_2_32->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_33Change(TObject *Sender)
{
	eq_2_33->Caption = TB_2_33->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_34Change(TObject *Sender)
{
	eq_2_34->Caption = TB_2_34->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_35Change(TObject *Sender)
{
	eq_2_35->Caption = TB_2_35->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_36Change(TObject *Sender)
{
	eq_2_36->Caption = TB_2_36->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_37Change(TObject *Sender)
{
	eq_2_37->Caption = TB_2_37->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_38Change(TObject *Sender)
{
	eq_2_38->Caption = TB_2_38->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_39Change(TObject *Sender)
{
	eq_2_39->Caption = TB_2_39->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_40Change(TObject *Sender)
{
	eq_2_40->Caption = TB_2_40->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_41Change(TObject *Sender)
{
	eq_2_41->Caption = TB_2_41->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_42Change(TObject *Sender)
{
	eq_2_42->Caption = TB_2_42->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_43Change(TObject *Sender)
{
	eq_2_43->Caption = TB_2_43->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_44Change(TObject *Sender)
{
	eq_2_44->Caption = TB_2_44->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_45Change(TObject *Sender)
{
	eq_2_45->Caption = TB_2_45->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_46Change(TObject *Sender)
{
	eq_2_46->Caption = TB_2_46->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_47Change(TObject *Sender)
{
	eq_2_47->Caption = TB_2_47->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_48Change(TObject *Sender)
{
	eq_2_48->Caption = TB_2_48->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_49Change(TObject *Sender)
{
	eq_2_49->Caption = TB_2_49->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_2_50Change(TObject *Sender)
{
	eq_2_50->Caption = TB_2_50->Position * -1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btn_type2_all_centerClick(TObject *Sender)
{
	TB_2_1->Position = 0;
	TB_2_2->Position = 0;
	TB_2_3->Position = 0;
	TB_2_4->Position = 0;
	TB_2_5->Position = 0;
	TB_2_6->Position = 0;
	TB_2_7->Position = 0;
	TB_2_8->Position = 0;
	TB_2_9->Position = 0;
	TB_2_10->Position = 0;
	TB_2_11->Position = 0;
	TB_2_12->Position = 0;
	TB_2_13->Position = 0;
	TB_2_14->Position = 0;
	TB_2_15->Position = 0;
	TB_2_16->Position = 0;
	TB_2_17->Position = 0;
	TB_2_18->Position = 0;
	TB_2_19->Position = 0;
	TB_2_20->Position = 0;
	TB_2_21->Position = 0;
	TB_2_22->Position = 0;
	TB_2_23->Position = 0;
	TB_2_24->Position = 0;
	TB_2_25->Position = 0;
	TB_2_26->Position = 0;
	TB_2_27->Position = 0;
	TB_2_28->Position = 0;
	TB_2_29->Position = 0;
	TB_2_30->Position = 0;
	TB_2_31->Position = 0;
	TB_2_32->Position = 0;
	TB_2_33->Position = 0;
	TB_2_34->Position = 0;
	TB_2_35->Position = 0;
	TB_2_36->Position = 0;
	TB_2_37->Position = 0;
	TB_2_38->Position = 0;
	TB_2_39->Position = 0;
	TB_2_40->Position = 0;
	TB_2_41->Position = 0;
	TB_2_42->Position = 0;
	TB_2_43->Position = 0;
	TB_2_44->Position = 0;
	TB_2_45->Position = 0;
	TB_2_46->Position = 0;
	TB_2_47->Position = 0;
	TB_2_48->Position = 0;
	TB_2_49->Position = 0;
	TB_2_50->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type2_all_upClick(TObject *Sender)
{
	int range_from,range_to;

	range_from = -1;
	range_to   = 999;
	if (cmbT2_range_from->ItemIndex > 0 && cmbT2_range_to->ItemIndex > 0) {
		if (cmbT2_range_from->ItemIndex <= cmbT2_range_to->ItemIndex) {
			range_from = cmbT2_range_from->ItemIndex;
			range_to   = cmbT2_range_to->ItemIndex;
		} else {
			range_to = cmbT2_range_to->ItemIndex;
			range_from   = cmbT2_range_from->ItemIndex;
		}
	} else {
		if (cmbT2_range_from->ItemIndex > 0) {
			range_from = cmbT2_range_from->ItemIndex;
		}
		if (cmbT2_range_to->ItemIndex > 0) {
			range_to = cmbT2_range_to->ItemIndex;
		}
	}

	if (range_from <= 1  && range_to >= 1) 	TB_2_1->Position = TB_2_1->Position - 1;
	if (range_from <= 2  && range_to >= 2) 	TB_2_2->Position = TB_2_2->Position - 1;
	if (range_from <= 3  && range_to >= 3) 	TB_2_3->Position = TB_2_3->Position - 1;
	if (range_from <= 4  && range_to >= 4) 	TB_2_4->Position = TB_2_4->Position - 1;
	if (range_from <= 5  && range_to >= 5) 	TB_2_5->Position = TB_2_5->Position - 1;
	if (range_from <= 6  && range_to >= 6) 	TB_2_6->Position = TB_2_6->Position - 1;
	if (range_from <= 7  && range_to >= 7) 	TB_2_7->Position = TB_2_7->Position - 1;
	if (range_from <= 8  && range_to >= 8) 	TB_2_8->Position = TB_2_8->Position - 1;
	if (range_from <= 9  && range_to >= 9) 	TB_2_9->Position = TB_2_9->Position - 1;
	if (range_from <= 10  && range_to >= 10) 	TB_2_10->Position = TB_2_10->Position - 1;
	if (range_from <= 11  && range_to >= 11) 	TB_2_11->Position = TB_2_11->Position - 1;
	if (range_from <= 12  && range_to >= 12) 	TB_2_12->Position = TB_2_12->Position - 1;
	if (range_from <= 13  && range_to >= 13) 	TB_2_13->Position = TB_2_13->Position - 1;
	if (range_from <= 14  && range_to >= 14) 	TB_2_14->Position = TB_2_14->Position - 1;
	if (range_from <= 15  && range_to >= 15) 	TB_2_15->Position = TB_2_15->Position - 1;
	if (range_from <= 16  && range_to >= 16) 	TB_2_16->Position = TB_2_16->Position - 1;
	if (range_from <= 17  && range_to >= 17) 	TB_2_17->Position = TB_2_17->Position - 1;
	if (range_from <= 18  && range_to >= 18) 	TB_2_18->Position = TB_2_18->Position - 1;
	if (range_from <= 19  && range_to >= 19) 	TB_2_19->Position = TB_2_19->Position - 1;
	if (range_from <= 20  && range_to >= 20) 	TB_2_20->Position = TB_2_20->Position - 1;
	if (range_from <= 21  && range_to >= 21) 	TB_2_21->Position = TB_2_21->Position - 1;
	if (range_from <= 22  && range_to >= 22) 	TB_2_22->Position = TB_2_22->Position - 1;
	if (range_from <= 23  && range_to >= 23) 	TB_2_23->Position = TB_2_23->Position - 1;
	if (range_from <= 24  && range_to >= 24) 	TB_2_24->Position = TB_2_24->Position - 1;
	if (range_from <= 25  && range_to >= 25) 	TB_2_25->Position = TB_2_25->Position - 1;
	if (range_from <= 26  && range_to >= 26) 	TB_2_26->Position = TB_2_26->Position - 1;
	if (range_from <= 27  && range_to >= 27) 	TB_2_27->Position = TB_2_27->Position - 1;
	if (range_from <= 28  && range_to >= 28) 	TB_2_28->Position = TB_2_28->Position - 1;
	if (range_from <= 29  && range_to >= 29) 	TB_2_29->Position = TB_2_29->Position - 1;
	if (range_from <= 30  && range_to >= 30) 	TB_2_30->Position = TB_2_30->Position - 1;
	if (range_from <= 31  && range_to >= 31) 	TB_2_31->Position = TB_2_31->Position - 1;
	if (range_from <= 32  && range_to >= 32) 	TB_2_32->Position = TB_2_32->Position - 1;
	if (range_from <= 33  && range_to >= 33) 	TB_2_33->Position = TB_2_33->Position - 1;
	if (range_from <= 34  && range_to >= 34) 	TB_2_34->Position = TB_2_34->Position - 1;
	if (range_from <= 35  && range_to >= 35) 	TB_2_35->Position = TB_2_35->Position - 1;
	if (range_from <= 36  && range_to >= 36) 	TB_2_36->Position = TB_2_36->Position - 1;
	if (range_from <= 37  && range_to >= 37) 	TB_2_37->Position = TB_2_37->Position - 1;
	if (range_from <= 38  && range_to >= 38) 	TB_2_38->Position = TB_2_38->Position - 1;
	if (range_from <= 39  && range_to >= 39) 	TB_2_39->Position = TB_2_39->Position - 1;
	if (range_from <= 40  && range_to >= 40) 	TB_2_40->Position = TB_2_40->Position - 1;
	if (range_from <= 41  && range_to >= 41) 	TB_2_41->Position = TB_2_41->Position - 1;
	if (range_from <= 42  && range_to >= 42) 	TB_2_42->Position = TB_2_42->Position - 1;
	if (range_from <= 43  && range_to >= 43) 	TB_2_43->Position = TB_2_43->Position - 1;
	if (range_from <= 44  && range_to >= 44) 	TB_2_44->Position = TB_2_44->Position - 1;
	if (range_from <= 45  && range_to >= 45) 	TB_2_45->Position = TB_2_45->Position - 1;
	if (range_from <= 46  && range_to >= 46) 	TB_2_46->Position = TB_2_46->Position - 1;
	if (range_from <= 47  && range_to >= 47) 	TB_2_47->Position = TB_2_47->Position - 1;
	if (range_from <= 48  && range_to >= 48) 	TB_2_48->Position = TB_2_48->Position - 1;
	if (range_from <= 49  && range_to >= 49) 	TB_2_49->Position = TB_2_49->Position - 1;
	if (range_from <= 50  && range_to >= 50) 	TB_2_50->Position = TB_2_50->Position - 1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type2_all_downClick(TObject *Sender)
{
	int range_from,range_to;

	range_from = -1;
	range_to   = 999;
	if (cmbT2_range_from->ItemIndex > 0 && cmbT2_range_to->ItemIndex > 0) {
		if (cmbT2_range_from->ItemIndex <= cmbT2_range_to->ItemIndex) {
			range_from = cmbT2_range_from->ItemIndex;
			range_to   = cmbT2_range_to->ItemIndex;
		} else {
			range_to = cmbT2_range_to->ItemIndex;
			range_from   = cmbT2_range_from->ItemIndex;
		}
	} else {
		if (cmbT2_range_from->ItemIndex > 0) {
			range_from = cmbT2_range_from->ItemIndex;
		}
		if (cmbT2_range_to->ItemIndex > 0) {
			range_to = cmbT2_range_to->ItemIndex;
		}
	}

	if (range_from <= 1  && range_to >= 1) 	TB_2_1->Position = TB_2_1->Position + 1;
	if (range_from <= 2  && range_to >= 2) 	TB_2_2->Position = TB_2_2->Position + 1;
	if (range_from <= 3  && range_to >= 3) 	TB_2_3->Position = TB_2_3->Position + 1;
	if (range_from <= 4  && range_to >= 4) 	TB_2_4->Position = TB_2_4->Position + 1;
	if (range_from <= 5  && range_to >= 5) 	TB_2_5->Position = TB_2_5->Position + 1;
	if (range_from <= 6  && range_to >= 6) 	TB_2_6->Position = TB_2_6->Position + 1;
	if (range_from <= 7  && range_to >= 7) 	TB_2_7->Position = TB_2_7->Position + 1;
	if (range_from <= 8  && range_to >= 8) 	TB_2_8->Position = TB_2_8->Position + 1;
	if (range_from <= 9  && range_to >= 9) 	TB_2_9->Position = TB_2_9->Position + 1;
	if (range_from <= 10  && range_to >= 10) 	TB_2_10->Position = TB_2_10->Position + 1;
	if (range_from <= 11  && range_to >= 11) 	TB_2_11->Position = TB_2_11->Position + 1;
	if (range_from <= 12  && range_to >= 12) 	TB_2_12->Position = TB_2_12->Position + 1;
	if (range_from <= 13  && range_to >= 13) 	TB_2_13->Position = TB_2_13->Position + 1;
	if (range_from <= 14  && range_to >= 14) 	TB_2_14->Position = TB_2_14->Position + 1;
	if (range_from <= 15  && range_to >= 15) 	TB_2_15->Position = TB_2_15->Position + 1;
	if (range_from <= 16  && range_to >= 16) 	TB_2_16->Position = TB_2_16->Position + 1;
	if (range_from <= 17  && range_to >= 17) 	TB_2_17->Position = TB_2_17->Position + 1;
	if (range_from <= 18  && range_to >= 18) 	TB_2_18->Position = TB_2_18->Position + 1;
	if (range_from <= 19  && range_to >= 19) 	TB_2_19->Position = TB_2_19->Position + 1;
	if (range_from <= 20  && range_to >= 20) 	TB_2_20->Position = TB_2_20->Position + 1;
	if (range_from <= 21  && range_to >= 21) 	TB_2_21->Position = TB_2_21->Position + 1;
	if (range_from <= 22  && range_to >= 22) 	TB_2_22->Position = TB_2_22->Position + 1;
	if (range_from <= 23  && range_to >= 23) 	TB_2_23->Position = TB_2_23->Position + 1;
	if (range_from <= 24  && range_to >= 24) 	TB_2_24->Position = TB_2_24->Position + 1;
	if (range_from <= 25  && range_to >= 25) 	TB_2_25->Position = TB_2_25->Position + 1;
	if (range_from <= 26  && range_to >= 26) 	TB_2_26->Position = TB_2_26->Position + 1;
	if (range_from <= 27  && range_to >= 27) 	TB_2_27->Position = TB_2_27->Position + 1;
	if (range_from <= 28  && range_to >= 28) 	TB_2_28->Position = TB_2_28->Position + 1;
	if (range_from <= 29  && range_to >= 29) 	TB_2_29->Position = TB_2_29->Position + 1;
	if (range_from <= 30  && range_to >= 30) 	TB_2_30->Position = TB_2_30->Position + 1;
	if (range_from <= 31  && range_to >= 31) 	TB_2_31->Position = TB_2_31->Position + 1;
	if (range_from <= 32  && range_to >= 32) 	TB_2_32->Position = TB_2_32->Position + 1;
	if (range_from <= 33  && range_to >= 33) 	TB_2_33->Position = TB_2_33->Position + 1;
	if (range_from <= 34  && range_to >= 34) 	TB_2_34->Position = TB_2_34->Position + 1;
	if (range_from <= 35  && range_to >= 35) 	TB_2_35->Position = TB_2_35->Position + 1;
	if (range_from <= 36  && range_to >= 36) 	TB_2_36->Position = TB_2_36->Position + 1;
	if (range_from <= 37  && range_to >= 37) 	TB_2_37->Position = TB_2_37->Position + 1;
	if (range_from <= 38  && range_to >= 38) 	TB_2_38->Position = TB_2_38->Position + 1;
	if (range_from <= 39  && range_to >= 39) 	TB_2_39->Position = TB_2_39->Position + 1;
	if (range_from <= 40  && range_to >= 40) 	TB_2_40->Position = TB_2_40->Position + 1;
	if (range_from <= 41  && range_to >= 41) 	TB_2_41->Position = TB_2_41->Position + 1;
	if (range_from <= 42  && range_to >= 42) 	TB_2_42->Position = TB_2_42->Position + 1;
	if (range_from <= 43  && range_to >= 43) 	TB_2_43->Position = TB_2_43->Position + 1;
	if (range_from <= 44  && range_to >= 44) 	TB_2_44->Position = TB_2_44->Position + 1;
	if (range_from <= 45  && range_to >= 45) 	TB_2_45->Position = TB_2_45->Position + 1;
	if (range_from <= 46  && range_to >= 46) 	TB_2_46->Position = TB_2_46->Position + 1;
	if (range_from <= 47  && range_to >= 47) 	TB_2_47->Position = TB_2_47->Position + 1;
	if (range_from <= 48  && range_to >= 48) 	TB_2_48->Position = TB_2_48->Position + 1;
	if (range_from <= 49  && range_to >= 49) 	TB_2_49->Position = TB_2_49->Position + 1;
	if (range_from <= 50  && range_to >= 50) 	TB_2_50->Position = TB_2_50->Position + 1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_1Change(TObject *Sender)
{
	eq_3_1->Caption = TB_3_1->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_2Change(TObject *Sender)
{
	eq_3_2->Caption = TB_3_2->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_3Change(TObject *Sender)
{
	eq_3_3->Caption = TB_3_3->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_4Change(TObject *Sender)
{
	eq_3_4->Caption = TB_3_4->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_5Change(TObject *Sender)
{
	eq_3_5->Caption = TB_3_5->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_6Change(TObject *Sender)
{
	eq_3_6->Caption = TB_3_6->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_7Change(TObject *Sender)
{
	eq_3_7->Caption = TB_3_7->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_8Change(TObject *Sender)
{
	eq_3_8->Caption = TB_3_8->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_9Change(TObject *Sender)
{
	eq_3_9->Caption = TB_3_9->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_10Change(TObject *Sender)
{
	eq_3_10->Caption = TB_3_10->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_11Change(TObject *Sender)
{
	eq_3_11->Caption = TB_3_11->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_12Change(TObject *Sender)
{
	eq_3_12->Caption = TB_3_12->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_13Change(TObject *Sender)
{
	eq_3_13->Caption = TB_3_13->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_14Change(TObject *Sender)
{
	eq_3_14->Caption = TB_3_14->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_15Change(TObject *Sender)
{
	eq_3_15->Caption = TB_3_15->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_16Change(TObject *Sender)
{
	eq_3_16->Caption = TB_3_16->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_17Change(TObject *Sender)
{
	eq_3_17->Caption = TB_3_17->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_18Change(TObject *Sender)
{
	eq_3_18->Caption = TB_3_18->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_19Change(TObject *Sender)
{
	eq_3_19->Caption = TB_3_19->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_20Change(TObject *Sender)
{
	eq_3_20->Caption = TB_3_20->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_21Change(TObject *Sender)
{
	eq_3_21->Caption = TB_3_21->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_22Change(TObject *Sender)
{
	eq_3_22->Caption = TB_3_22->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_23Change(TObject *Sender)
{
	eq_3_23->Caption = TB_3_23->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_24Change(TObject *Sender)
{
	eq_3_24->Caption = TB_3_24->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_25Change(TObject *Sender)
{
	eq_3_25->Caption = TB_3_25->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_26Change(TObject *Sender)
{
	eq_3_26->Caption = TB_3_26->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_27Change(TObject *Sender)
{
	eq_3_27->Caption = TB_3_27->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_28Change(TObject *Sender)
{
	eq_3_28->Caption = TB_3_28->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_29Change(TObject *Sender)
{
	eq_3_29->Caption = TB_3_29->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_30Change(TObject *Sender)
{
	eq_3_30->Caption = TB_3_30->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_31Change(TObject *Sender)
{
	eq_3_31->Caption = TB_3_31->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_32Change(TObject *Sender)
{
	eq_3_32->Caption = TB_3_32->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_33Change(TObject *Sender)
{
	eq_3_33->Caption = TB_3_33->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_34Change(TObject *Sender)
{
	eq_3_34->Caption = TB_3_34->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_35Change(TObject *Sender)
{
	eq_3_35->Caption = TB_3_35->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_36Change(TObject *Sender)
{
	eq_3_36->Caption = TB_3_36->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_37Change(TObject *Sender)
{
	eq_3_37->Caption = TB_3_37->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_38Change(TObject *Sender)
{
	eq_3_38->Caption = TB_3_38->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_39Change(TObject *Sender)
{
	eq_3_39->Caption = TB_3_39->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_40Change(TObject *Sender)
{
	eq_3_40->Caption = TB_3_40->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_41Change(TObject *Sender)
{
	eq_3_41->Caption = TB_3_41->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_42Change(TObject *Sender)
{
	eq_3_42->Caption = TB_3_42->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_43Change(TObject *Sender)
{
	eq_3_43->Caption = TB_3_43->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_44Change(TObject *Sender)
{
	eq_3_44->Caption = TB_3_44->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_45Change(TObject *Sender)
{
	eq_3_45->Caption = TB_3_45->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_46Change(TObject *Sender)
{
	eq_3_46->Caption = TB_3_46->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_47Change(TObject *Sender)
{
	eq_3_47->Caption = TB_3_47->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_48Change(TObject *Sender)
{
	eq_3_48->Caption = TB_3_48->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_49Change(TObject *Sender)
{
	eq_3_49->Caption = TB_3_49->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_50Change(TObject *Sender)
{
	eq_3_50->Caption = TB_3_50->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_51Change(TObject *Sender)
{
	eq_3_51->Caption = TB_3_51->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_52Change(TObject *Sender)
{
	eq_3_52->Caption = TB_3_52->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_53Change(TObject *Sender)
{
	eq_3_53->Caption = TB_3_53->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_54Change(TObject *Sender)
{
	eq_3_54->Caption = TB_3_54->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_55Change(TObject *Sender)
{
	eq_3_55->Caption = TB_3_55->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_56Change(TObject *Sender)
{
	eq_3_56->Caption = TB_3_56->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_57Change(TObject *Sender)
{
	eq_3_57->Caption = TB_3_57->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_58Change(TObject *Sender)
{
	eq_3_58->Caption = TB_3_58->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_59Change(TObject *Sender)
{
	eq_3_59->Caption = TB_3_59->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_60Change(TObject *Sender)
{
	eq_3_60->Caption = TB_3_60->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_61Change(TObject *Sender)
{
	eq_3_61->Caption = TB_3_61->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_62Change(TObject *Sender)
{
	eq_3_62->Caption = TB_3_62->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_63Change(TObject *Sender)
{
	eq_3_63->Caption = TB_3_63->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_64Change(TObject *Sender)
{
	eq_3_64->Caption = TB_3_64->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_65Change(TObject *Sender)
{
	eq_3_65->Caption = TB_3_65->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_66Change(TObject *Sender)
{
	eq_3_66->Caption = TB_3_66->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_67Change(TObject *Sender)
{
	eq_3_67->Caption = TB_3_67->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_68Change(TObject *Sender)
{
	eq_3_68->Caption = TB_3_68->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_69Change(TObject *Sender)
{
	eq_3_69->Caption = TB_3_69->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_70Change(TObject *Sender)
{
	eq_3_70->Caption = TB_3_70->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_71Change(TObject *Sender)
{
	eq_3_71->Caption = TB_3_71->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_72Change(TObject *Sender)
{
	eq_3_72->Caption = TB_3_72->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_73Change(TObject *Sender)
{
	eq_3_73->Caption = TB_3_73->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_74Change(TObject *Sender)
{
	eq_3_74->Caption = TB_3_74->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_75Change(TObject *Sender)
{
	eq_3_75->Caption = TB_3_75->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_76Change(TObject *Sender)
{
	eq_3_76->Caption = TB_3_76->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_77Change(TObject *Sender)
{
	eq_3_77->Caption = TB_3_77->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_3_78Change(TObject *Sender)
{
	eq_3_78->Caption = TB_3_78->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type3_all_centerClick(TObject *Sender)
{
	TB_3_1->Position = 0;
	TB_3_2->Position = 0;
	TB_3_3->Position = 0;
	TB_3_4->Position = 0;
	TB_3_5->Position = 0;
	TB_3_6->Position = 0;
	TB_3_7->Position = 0;
	TB_3_8->Position = 0;
	TB_3_9->Position = 0;
	TB_3_10->Position = 0;
	TB_3_11->Position = 0;
	TB_3_12->Position = 0;
	TB_3_13->Position = 0;
	TB_3_14->Position = 0;
	TB_3_15->Position = 0;
	TB_3_16->Position = 0;
	TB_3_17->Position = 0;
	TB_3_18->Position = 0;
	TB_3_19->Position = 0;
	TB_3_20->Position = 0;
	TB_3_21->Position = 0;
	TB_3_22->Position = 0;
	TB_3_23->Position = 0;
	TB_3_24->Position = 0;
	TB_3_25->Position = 0;
	TB_3_26->Position = 0;
	TB_3_27->Position = 0;
	TB_3_28->Position = 0;
	TB_3_29->Position = 0;
	TB_3_30->Position = 0;
	TB_3_31->Position = 0;
	TB_3_32->Position = 0;
	TB_3_33->Position = 0;
	TB_3_34->Position = 0;
	TB_3_35->Position = 0;
	TB_3_36->Position = 0;
	TB_3_37->Position = 0;
	TB_3_38->Position = 0;
	TB_3_39->Position = 0;
	TB_3_40->Position = 0;
	TB_3_41->Position = 0;
	TB_3_42->Position = 0;
	TB_3_43->Position = 0;
	TB_3_44->Position = 0;
	TB_3_45->Position = 0;
	TB_3_46->Position = 0;
	TB_3_47->Position = 0;
	TB_3_48->Position = 0;
	TB_3_49->Position = 0;
	TB_3_50->Position = 0;
	TB_3_51->Position = 0;
	TB_3_52->Position = 0;
	TB_3_53->Position = 0;
	TB_3_54->Position = 0;
	TB_3_55->Position = 0;
	TB_3_56->Position = 0;
	TB_3_57->Position = 0;
	TB_3_58->Position = 0;
	TB_3_59->Position = 0;
	TB_3_60->Position = 0;
	TB_3_61->Position = 0;
	TB_3_62->Position = 0;
	TB_3_63->Position = 0;
	TB_3_64->Position = 0;
	TB_3_65->Position = 0;
	TB_3_66->Position = 0;
	TB_3_67->Position = 0;
	TB_3_68->Position = 0;
	TB_3_69->Position = 0;
	TB_3_70->Position = 0;
	TB_3_71->Position = 0;
	TB_3_72->Position = 0;
	TB_3_73->Position = 0;
	TB_3_74->Position = 0;
	TB_3_75->Position = 0;
	TB_3_76->Position = 0;
	TB_3_77->Position = 0;
	TB_3_78->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type3_all_upClick(TObject *Sender)
{
	int range_from,range_to;

	range_from = -1;
	range_to   = 999;
	if (cmbT3_range_from->ItemIndex > 0 && cmbT3_range_to->ItemIndex > 0) {
		if (cmbT3_range_from->ItemIndex <= cmbT3_range_to->ItemIndex) {
			range_from = cmbT3_range_from->ItemIndex;
			range_to   = cmbT3_range_to->ItemIndex;
		} else {
			range_to = cmbT3_range_to->ItemIndex;
			range_from   = cmbT3_range_from->ItemIndex;
		}
	} else {
		if (cmbT3_range_from->ItemIndex > 0) {
			range_from = cmbT3_range_from->ItemIndex;
		}
		if (cmbT3_range_to->ItemIndex > 0) {
			range_to = cmbT3_range_to->ItemIndex;
		}
	}

	if (range_from <= 1  && range_to >= 1) 	TB_3_1->Position = TB_3_1->Position - 1;
	if (range_from <= 2  && range_to >= 2) 	TB_3_2->Position = TB_3_2->Position - 1;
	if (range_from <= 3  && range_to >= 3) 	TB_3_3->Position = TB_3_3->Position - 1;
	if (range_from <= 4  && range_to >= 4) 	TB_3_4->Position = TB_3_4->Position - 1;
	if (range_from <= 5  && range_to >= 5) 	TB_3_5->Position = TB_3_5->Position - 1;
	if (range_from <= 6  && range_to >= 6) 	TB_3_6->Position = TB_3_6->Position - 1;
	if (range_from <= 7  && range_to >= 7) 	TB_3_7->Position = TB_3_7->Position - 1;
	if (range_from <= 8  && range_to >= 8) 	TB_3_8->Position = TB_3_8->Position - 1;
	if (range_from <= 9  && range_to >= 9) 	TB_3_9->Position = TB_3_9->Position - 1;
	if (range_from <= 10  && range_to >= 10) 	TB_3_10->Position =TB_3_10->Position - 1;
	if (range_from <= 11  && range_to >= 11) 	TB_3_11->Position =TB_3_11->Position - 1;
	if (range_from <= 12  && range_to >= 12) 	TB_3_12->Position =TB_3_12->Position - 1;
	if (range_from <= 13  && range_to >= 13) 	TB_3_13->Position =TB_3_13->Position - 1;
	if (range_from <= 14  && range_to >= 14) 	TB_3_14->Position =TB_3_14->Position - 1;
	if (range_from <= 15  && range_to >= 15) 	TB_3_15->Position =TB_3_15->Position - 1;
	if (range_from <= 16  && range_to >= 16) 	TB_3_16->Position =TB_3_16->Position - 1;
	if (range_from <= 17  && range_to >= 17) 	TB_3_17->Position =TB_3_17->Position - 1;
	if (range_from <= 18  && range_to >= 18) 	TB_3_18->Position =TB_3_18->Position - 1;
	if (range_from <= 19  && range_to >= 19) 	TB_3_19->Position =TB_3_19->Position - 1;
	if (range_from <= 20  && range_to >= 20) 	TB_3_20->Position =TB_3_20->Position - 1;
	if (range_from <= 21  && range_to >= 21) 	TB_3_21->Position =TB_3_21->Position - 1;
	if (range_from <= 22  && range_to >= 22) 	TB_3_22->Position =TB_3_22->Position - 1;
	if (range_from <= 23  && range_to >= 23) 	TB_3_23->Position =TB_3_23->Position - 1;
	if (range_from <= 24  && range_to >= 24) 	TB_3_24->Position =TB_3_24->Position - 1;
	if (range_from <= 25  && range_to >= 25) 	TB_3_25->Position =TB_3_25->Position - 1;
	if (range_from <= 26  && range_to >= 26) 	TB_3_26->Position =TB_3_26->Position - 1;
	if (range_from <= 27  && range_to >= 27) 	TB_3_27->Position =TB_3_27->Position - 1;
	if (range_from <= 28  && range_to >= 28) 	TB_3_28->Position =TB_3_28->Position - 1;
	if (range_from <= 29  && range_to >= 29) 	TB_3_29->Position =TB_3_29->Position - 1;
	if (range_from <= 30  && range_to >= 30) 	TB_3_30->Position =TB_3_30->Position - 1;
	if (range_from <= 31  && range_to >= 31) 	TB_3_31->Position =TB_3_31->Position - 1;
	if (range_from <= 32  && range_to >= 32) 	TB_3_32->Position =TB_3_32->Position - 1;
	if (range_from <= 33  && range_to >= 33) 	TB_3_33->Position =TB_3_33->Position - 1;
	if (range_from <= 34  && range_to >= 34) 	TB_3_34->Position =TB_3_34->Position - 1;
	if (range_from <= 35  && range_to >= 35) 	TB_3_35->Position =TB_3_35->Position - 1;
	if (range_from <= 36  && range_to >= 36) 	TB_3_36->Position =TB_3_36->Position - 1;
	if (range_from <= 37  && range_to >= 37) 	TB_3_37->Position =TB_3_37->Position - 1;
	if (range_from <= 38  && range_to >= 38) 	TB_3_38->Position =TB_3_38->Position - 1;
	if (range_from <= 39  && range_to >= 39) 	TB_3_39->Position =TB_3_39->Position - 1;
	if (range_from <= 40  && range_to >= 40) 	TB_3_40->Position =TB_3_40->Position - 1;
	if (range_from <= 41  && range_to >= 41) 	TB_3_41->Position =TB_3_41->Position - 1;
	if (range_from <= 42  && range_to >= 42) 	TB_3_42->Position =TB_3_42->Position - 1;
	if (range_from <= 43  && range_to >= 43) 	TB_3_43->Position =TB_3_43->Position - 1;
	if (range_from <= 44  && range_to >= 44) 	TB_3_44->Position =TB_3_44->Position - 1;
	if (range_from <= 45  && range_to >= 45) 	TB_3_45->Position =TB_3_45->Position - 1;
	if (range_from <= 46  && range_to >= 46) 	TB_3_46->Position =TB_3_46->Position - 1;
	if (range_from <= 47  && range_to >= 47) 	TB_3_47->Position =TB_3_47->Position - 1;
	if (range_from <= 48  && range_to >= 48) 	TB_3_48->Position =TB_3_48->Position - 1;
	if (range_from <= 49  && range_to >= 49) 	TB_3_49->Position =TB_3_49->Position - 1;
	if (range_from <= 50  && range_to >= 50) 	TB_3_50->Position =TB_3_50->Position - 1;
	if (range_from <= 51  && range_to >= 51) 	TB_3_51->Position =TB_3_51->Position - 1;
	if (range_from <= 52  && range_to >= 52) 	TB_3_52->Position =TB_3_52->Position - 1;
	if (range_from <= 53  && range_to >= 53) 	TB_3_53->Position =TB_3_53->Position - 1;
	if (range_from <= 54  && range_to >= 54) 	TB_3_54->Position =TB_3_54->Position - 1;
	if (range_from <= 55  && range_to >= 55) 	TB_3_55->Position =TB_3_55->Position - 1;
	if (range_from <= 56  && range_to >= 56) 	TB_3_56->Position =TB_3_56->Position - 1;
	if (range_from <= 57  && range_to >= 57) 	TB_3_57->Position =TB_3_57->Position - 1;
	if (range_from <= 58  && range_to >= 58) 	TB_3_58->Position =TB_3_58->Position - 1;
	if (range_from <= 59  && range_to >= 59) 	TB_3_59->Position =TB_3_59->Position - 1;
	if (range_from <= 60  && range_to >= 60) 	TB_3_60->Position =TB_3_60->Position - 1;
	if (range_from <= 61  && range_to >= 61) 	TB_3_61->Position =TB_3_61->Position - 1;
	if (range_from <= 62  && range_to >= 62) 	TB_3_62->Position =TB_3_62->Position - 1;
	if (range_from <= 63  && range_to >= 63) 	TB_3_63->Position =TB_3_63->Position - 1;
	if (range_from <= 64  && range_to >= 64) 	TB_3_64->Position =TB_3_64->Position - 1;
	if (range_from <= 65  && range_to >= 65) 	TB_3_65->Position =TB_3_65->Position - 1;
	if (range_from <= 66  && range_to >= 66) 	TB_3_66->Position =TB_3_66->Position - 1;
	if (range_from <= 67  && range_to >= 67) 	TB_3_67->Position =TB_3_67->Position - 1;
	if (range_from <= 68  && range_to >= 68) 	TB_3_68->Position =TB_3_68->Position - 1;
	if (range_from <= 69  && range_to >= 69) 	TB_3_69->Position =TB_3_69->Position - 1;
	if (range_from <= 70  && range_to >= 70) 	TB_3_70->Position =TB_3_70->Position - 1;
	if (range_from <= 71  && range_to >= 71) 	TB_3_71->Position =TB_3_71->Position - 1;
	if (range_from <= 72  && range_to >= 72) 	TB_3_72->Position =TB_3_72->Position - 1;
	if (range_from <= 73  && range_to >= 73) 	TB_3_73->Position =TB_3_73->Position - 1;
	if (range_from <= 74  && range_to >= 74) 	TB_3_74->Position =TB_3_74->Position - 1;
	if (range_from <= 75  && range_to >= 75) 	TB_3_75->Position =TB_3_75->Position - 1;
	if (range_from <= 76  && range_to >= 76) 	TB_3_76->Position =TB_3_76->Position - 1;
	if (range_from <= 77  && range_to >= 77) 	TB_3_77->Position =TB_3_77->Position - 1;
	if (range_from <= 78  && range_to >= 78) 	TB_3_78->Position =TB_3_78->Position - 1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type3_all_downClick(TObject *Sender)
{
	int range_from,range_to;

	range_from = -1;
	range_to   = 999;
	if (cmbT3_range_from->ItemIndex > 0 && cmbT3_range_to->ItemIndex > 0) {
		if (cmbT3_range_from->ItemIndex <= cmbT3_range_to->ItemIndex) {
			range_from = cmbT3_range_from->ItemIndex;
			range_to   = cmbT3_range_to->ItemIndex;
		} else {
			range_to = cmbT3_range_to->ItemIndex;
			range_from   = cmbT3_range_from->ItemIndex;
		}
	} else {
		if (cmbT3_range_from->ItemIndex > 0) {
			range_from = cmbT3_range_from->ItemIndex;
		}
		if (cmbT3_range_to->ItemIndex > 0) {
			range_to = cmbT3_range_to->ItemIndex;
		}
	}

	if (range_from <= 1  && range_to >= 1) 		TB_3_1->Position = TB_3_1->Position + 1;
	if (range_from <= 2  && range_to >= 2) 		TB_3_2->Position = TB_3_2->Position + 1;
	if (range_from <= 3  && range_to >= 3) 		TB_3_3->Position = TB_3_3->Position + 1;
	if (range_from <= 4  && range_to >= 4) 		TB_3_4->Position = TB_3_4->Position + 1;
	if (range_from <= 5  && range_to >= 5) 		TB_3_5->Position = TB_3_5->Position + 1;
	if (range_from <= 6  && range_to >= 6) 		TB_3_6->Position = TB_3_6->Position + 1;
	if (range_from <= 7  && range_to >= 7) 		TB_3_7->Position = TB_3_7->Position + 1;
	if (range_from <= 8  && range_to >= 8) 		TB_3_8->Position = TB_3_8->Position + 1;
	if (range_from <= 9  && range_to >= 9) 		TB_3_9->Position = TB_3_9->Position + 1;
	if (range_from <= 10  && range_to >= 10)	TB_3_10->Position =TB_3_10->Position + 1;
	if (range_from <= 11  && range_to >= 11)	TB_3_11->Position =TB_3_11->Position + 1;
	if (range_from <= 12  && range_to >= 12)	TB_3_12->Position =TB_3_12->Position + 1;
	if (range_from <= 13  && range_to >= 13)	TB_3_13->Position =TB_3_13->Position + 1;
	if (range_from <= 14  && range_to >= 14)	TB_3_14->Position =TB_3_14->Position + 1;
	if (range_from <= 15  && range_to >= 15)	TB_3_15->Position =TB_3_15->Position + 1;
	if (range_from <= 16  && range_to >= 16)	TB_3_16->Position =TB_3_16->Position + 1;
	if (range_from <= 17  && range_to >= 17)	TB_3_17->Position =TB_3_17->Position + 1;
	if (range_from <= 18  && range_to >= 18)	TB_3_18->Position =TB_3_18->Position + 1;
	if (range_from <= 19  && range_to >= 19)	TB_3_19->Position =TB_3_19->Position + 1;
	if (range_from <= 20  && range_to >= 20)	TB_3_20->Position =TB_3_20->Position + 1;
	if (range_from <= 21  && range_to >= 21)	TB_3_21->Position =TB_3_21->Position + 1;
	if (range_from <= 22  && range_to >= 22)	TB_3_22->Position =TB_3_22->Position + 1;
	if (range_from <= 23  && range_to >= 23)	TB_3_23->Position =TB_3_23->Position + 1;
	if (range_from <= 24  && range_to >= 24)	TB_3_24->Position =TB_3_24->Position + 1;
	if (range_from <= 25  && range_to >= 25)	TB_3_25->Position =TB_3_25->Position + 1;
	if (range_from <= 26  && range_to >= 26)	TB_3_26->Position =TB_3_26->Position + 1;
	if (range_from <= 27  && range_to >= 27)	TB_3_27->Position =TB_3_27->Position + 1;
	if (range_from <= 28  && range_to >= 28)	TB_3_28->Position =TB_3_28->Position + 1;
	if (range_from <= 29  && range_to >= 29)	TB_3_29->Position =TB_3_29->Position + 1;
	if (range_from <= 30  && range_to >= 30)	TB_3_30->Position =TB_3_30->Position + 1;
	if (range_from <= 31  && range_to >= 31)	TB_3_31->Position =TB_3_31->Position + 1;
	if (range_from <= 32  && range_to >= 32)	TB_3_32->Position =TB_3_32->Position + 1;
	if (range_from <= 33  && range_to >= 33)	TB_3_33->Position =TB_3_33->Position + 1;
	if (range_from <= 34  && range_to >= 34)	TB_3_34->Position =TB_3_34->Position + 1;
	if (range_from <= 35  && range_to >= 35)	TB_3_35->Position =TB_3_35->Position + 1;
	if (range_from <= 36  && range_to >= 36)	TB_3_36->Position =TB_3_36->Position + 1;
	if (range_from <= 37  && range_to >= 37)	TB_3_37->Position =TB_3_37->Position + 1;
	if (range_from <= 38  && range_to >= 38)	TB_3_38->Position =TB_3_38->Position + 1;
	if (range_from <= 39  && range_to >= 39)	TB_3_39->Position =TB_3_39->Position + 1;
	if (range_from <= 40  && range_to >= 40)	TB_3_40->Position =TB_3_40->Position + 1;
	if (range_from <= 41  && range_to >= 41)	TB_3_41->Position =TB_3_41->Position + 1;
	if (range_from <= 42  && range_to >= 42)	TB_3_42->Position =TB_3_42->Position + 1;
	if (range_from <= 43  && range_to >= 43)	TB_3_43->Position =TB_3_43->Position + 1;
	if (range_from <= 44  && range_to >= 44)	TB_3_44->Position =TB_3_44->Position + 1;
	if (range_from <= 45  && range_to >= 45)	TB_3_45->Position =TB_3_45->Position + 1;
	if (range_from <= 46  && range_to >= 46)	TB_3_46->Position =TB_3_46->Position + 1;
	if (range_from <= 47  && range_to >= 47)	TB_3_47->Position =TB_3_47->Position + 1;
	if (range_from <= 48  && range_to >= 48)	TB_3_48->Position =TB_3_48->Position + 1;
	if (range_from <= 49  && range_to >= 49)	TB_3_49->Position =TB_3_49->Position + 1;
	if (range_from <= 50  && range_to >= 50)	TB_3_50->Position =TB_3_50->Position + 1;
	if (range_from <= 51  && range_to >= 51)	TB_3_51->Position =TB_3_51->Position + 1;
	if (range_from <= 52  && range_to >= 52)	TB_3_52->Position =TB_3_52->Position + 1;
	if (range_from <= 53  && range_to >= 53)	TB_3_53->Position =TB_3_53->Position + 1;
	if (range_from <= 54  && range_to >= 54)	TB_3_54->Position =TB_3_54->Position + 1;
	if (range_from <= 55  && range_to >= 55)	TB_3_55->Position =TB_3_55->Position + 1;
	if (range_from <= 56  && range_to >= 56)	TB_3_56->Position =TB_3_56->Position + 1;
	if (range_from <= 57  && range_to >= 57)	TB_3_57->Position =TB_3_57->Position + 1;
	if (range_from <= 58  && range_to >= 58)	TB_3_58->Position =TB_3_58->Position + 1;
	if (range_from <= 59  && range_to >= 59)	TB_3_59->Position =TB_3_59->Position + 1;
	if (range_from <= 60  && range_to >= 60)	TB_3_60->Position =TB_3_60->Position + 1;
	if (range_from <= 61  && range_to >= 61)	TB_3_61->Position =TB_3_61->Position + 1;
	if (range_from <= 62  && range_to >= 62)	TB_3_62->Position =TB_3_62->Position + 1;
	if (range_from <= 63  && range_to >= 63)	TB_3_63->Position =TB_3_63->Position + 1;
	if (range_from <= 64  && range_to >= 64)	TB_3_64->Position =TB_3_64->Position + 1;
	if (range_from <= 65  && range_to >= 65)	TB_3_65->Position =TB_3_65->Position + 1;
	if (range_from <= 66  && range_to >= 66)	TB_3_66->Position =TB_3_66->Position + 1;
	if (range_from <= 67  && range_to >= 67)	TB_3_67->Position =TB_3_67->Position + 1;
	if (range_from <= 68  && range_to >= 68)	TB_3_68->Position =TB_3_68->Position + 1;
	if (range_from <= 69  && range_to >= 69)	TB_3_69->Position =TB_3_69->Position + 1;
	if (range_from <= 70  && range_to >= 70)	TB_3_70->Position =TB_3_70->Position + 1;
	if (range_from <= 71  && range_to >= 71)	TB_3_71->Position =TB_3_71->Position + 1;
	if (range_from <= 72  && range_to >= 72)	TB_3_72->Position =TB_3_72->Position + 1;
	if (range_from <= 73  && range_to >= 73)	TB_3_73->Position =TB_3_73->Position + 1;
	if (range_from <= 74  && range_to >= 74)	TB_3_74->Position =TB_3_74->Position + 1;
	if (range_from <= 75  && range_to >= 75)	TB_3_75->Position =TB_3_75->Position + 1;
	if (range_from <= 76  && range_to >= 76)	TB_3_76->Position =TB_3_76->Position + 1;
	if (range_from <= 77  && range_to >= 77)	TB_3_77->Position =TB_3_77->Position + 1;
	if (range_from <= 78  && range_to >= 78)	TB_3_78->Position =TB_3_78->Position + 1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type2_copy_from_type1Click(TObject *Sender)
{
	double d;
	PRINT_FN_LOG("btn_type2_copy_from_type1Click","Start");

	TB_2_1->Position = TB_1_1->Position;								// 20
	TB_2_2->Position = TB_1_2->Position;                                // 25
	TB_2_3->Position = TB_1_3->Position;                                // 31
	TB_2_4->Position = TB_1_4->Position;                                // 40
	TB_2_5->Position = TB_1_5->Position;                                // 50
	TB_2_6->Position = TB_1_6->Position;                                // 63
	TB_2_7->Position = TB_1_7->Position;                                // 80
	TB_2_8->Position = TB_1_8->Position;                                // 100
	TB_2_9->Position = TB_1_9->Position;                                // 125
	TB_2_10->Position = TB_1_10->Position;                              // 160
	TB_2_11->Position = TB_1_11->Position;                              // 200
	TB_2_12->Position = TB_1_12->Position;                              // 250
	TB_2_13->Position = TB_1_13->Position;                              // 315
	TB_2_14->Position = TB_1_14->Position;                              // 400
	TB_2_15->Position = TB_1_15->Position;                              // 500
	TB_2_16->Position = TB_1_16->Position;                              // 630
	TB_2_17->Position = TB_1_17->Position;                              // 800
	TB_2_18->Position = TB_1_18->Position;                              // 1k
	TB_2_19->Position = TB_1_19->Position;                              // 1.25k
	TB_2_20->Position = TB_1_20->Position;                              // 1.6k
	TB_2_21->Position = TB_1_21->Position;                              // 2k
	TB_2_22->Position = TB_1_22->Position;                              // 2.5k
	TB_2_23->Position = TB_1_23->Position;                              // 3.15k
	TB_2_24->Position = TB_1_24->Position;                              // 4k
	TB_2_25->Position = TB_1_25->Position;                              // 5k
	TB_2_26->Position = TB_1_26->Position;                              // 6k
	TB_2_27->Position = (TB_1_26->Position + TB_1_27->Position) / 2;    // 7k
	TB_2_28->Position = TB_1_27->Position;                              // 8k
	TB_2_29->Position = (TB_1_27->Position + TB_1_28->Position) / 2;    // 9k
	TB_2_30->Position = TB_1_28->Position;                              // 10k
	TB_2_31->Position = (TB_1_28->Position + TB_1_29->Position) / 2;    // 11k
	TB_2_32->Position = TB_1_29->Position;                              // 12k
	d = (TB_1_30->Position - TB_1_29->Position);d /= 4;
	TB_2_33->Position = TB_1_29->Position + (int)(d * 1);				// 13k
	TB_2_34->Position = TB_1_29->Position + (int)(d * 2);               // 14k
	TB_2_35->Position = TB_1_29->Position + (int)(d * 3);               // 15k
	TB_2_36->Position = TB_1_30->Position;                              // 16k
	d = (TB_1_31->Position - TB_1_30->Position);d /= 4;
	TB_2_37->Position = TB_1_30->Position + (int)(d * 1);               // 17k
	TB_2_38->Position = TB_1_30->Position + (int)(d * 2);               // 18k
	TB_2_39->Position = TB_1_30->Position + (int)(d * 3);               // 19k
	TB_2_40->Position = TB_1_31->Position;                              // 20k
	d = (TB_1_32->Position - TB_1_31->Position);d /= 5;
	TB_2_41->Position = TB_1_31->Position + (int)(d * 1);               // 21k
	TB_2_42->Position = TB_1_31->Position + (int)(d * 2);               // 22k
	TB_2_43->Position = TB_1_31->Position + (int)(d * 3);               // 23k
	TB_2_44->Position = TB_1_31->Position + (int)(d * 4);               // 24k
	TB_2_45->Position = TB_1_32->Position;                              // 25k
	TB_2_46->Position = TB_1_33->Position;                              // 30k
	d = (TB_1_34->Position - TB_1_33->Position);d /= 10;
	TB_2_47->Position = TB_1_33->Position + (int)(d * 5);               // 35k
	TB_2_48->Position = TB_1_34->Position;                              // 40k
	d = (TB_1_35->Position - TB_1_34->Position);d /= 10;
	TB_2_49->Position = TB_1_34->Position + (int)(d * 5);               // 45k
	TB_2_50->Position = TB_1_35->Position;                              // 50k
	PRINT_FN_LOG("btn_type2_copy_from_type1Click","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btn_type3_copy_from_type2Click(TObject *Sender)
{
	double d;

	PRINT_FN_LOG("btn_type3_copy_from_type2Click","Start");
	TB_3_1->Position = 0;
	TB_3_2->Position = 0;
	TB_3_3->Position = 0;
	TB_3_4->Position = 0;
	TB_3_5->Position = 0;
	TB_3_6->Position = 0;
	TB_3_7->Position  = TB_2_1->Position;	// 20
	TB_3_8->Position  = TB_2_2->Position;	// 25
	TB_3_9->Position  = TB_2_3->Position;	// 31.5
	TB_3_10->Position = TB_2_4->Position;	// 40
	TB_3_11->Position = TB_2_5->Position;	// 50
	TB_3_12->Position = TB_2_6->Position;	// 63
	TB_3_13->Position = TB_2_7->Position;	// 80
	TB_3_14->Position = TB_2_8->Position;	// 100
	TB_3_15->Position = TB_2_9->Position;	// 125
	TB_3_16->Position = TB_2_10->Position;	// 160
	TB_3_17->Position = TB_2_11->Position;	// 200
	TB_3_18->Position = TB_2_12->Position;	// 250
	TB_3_19->Position = TB_2_13->Position;	// 315
	TB_3_20->Position = TB_2_14->Position;	// 400
	TB_3_21->Position = TB_2_15->Position;	// 500
	TB_3_22->Position = TB_2_16->Position;	// 630
	TB_3_23->Position = TB_2_17->Position;	// 800
	TB_3_24->Position = TB_2_18->Position;	// 1k
	TB_3_25->Position = TB_2_19->Position;	// 1.25k
	TB_3_26->Position = TB_2_20->Position;	// 1.6k
	TB_3_27->Position = TB_2_21->Position;	// 2k
	TB_3_28->Position = TB_2_22->Position;	// 2.5k
	TB_3_29->Position = TB_2_23->Position;	// 3.15k
	TB_3_30->Position = TB_2_24->Position;	// 4k
	TB_3_31->Position = TB_2_25->Position;	// 5k
	TB_3_32->Position = TB_2_26->Position;	// 6k
	TB_3_33->Position = TB_2_27->Position;	// 7k
	TB_3_34->Position = TB_2_28->Position;	// 8k
	TB_3_35->Position = TB_2_29->Position;	// 9k
	TB_3_36->Position = TB_2_30->Position;	// 10k
	TB_3_37->Position = TB_2_31->Position;	// 11k
	TB_3_38->Position = TB_2_32->Position;	// 12k
	TB_3_39->Position = TB_2_33->Position;	// 13k
	TB_3_40->Position = TB_2_34->Position;	// 14k
	TB_3_41->Position = TB_2_35->Position;	// 15k
	TB_3_42->Position = TB_2_36->Position;	// 16k
	TB_3_43->Position = TB_2_37->Position;	// 17k
	TB_3_44->Position = TB_2_38->Position;	// 18k
	TB_3_45->Position = TB_2_39->Position;	// 19k
	TB_3_46->Position = TB_2_40->Position;	// 20k
	TB_3_47->Position = TB_2_41->Position;	// 21k
	TB_3_48->Position = TB_2_42->Position;	// 22k
	TB_3_49->Position = TB_2_43->Position;	// 23k
	TB_3_50->Position = TB_2_44->Position;	// 24k
	TB_3_51->Position = TB_2_45->Position;	// 25k
	d = (TB_2_46->Position - TB_2_45->Position);d /= 4;
	TB_3_52->Position = TB_2_45->Position + (int)(d * 1);
	TB_3_53->Position = TB_2_45->Position + (int)(d * 2);
	TB_3_54->Position = TB_2_45->Position + (int)(d * 3);
	TB_3_55->Position = TB_2_45->Position + (int)(d * 4);
	TB_3_56->Position = TB_2_46->Position;	// 30k
	d = (TB_2_47->Position - TB_2_46->Position); d /= 4;
	TB_3_57->Position = TB_2_46->Position + (int)(d * 1);
	TB_3_58->Position = TB_2_46->Position + (int)(d * 2);
	TB_3_59->Position = TB_2_46->Position + (int)(d * 3);
	TB_3_60->Position = TB_2_46->Position + (int)(d * 4);
	TB_3_61->Position = TB_2_47->Position;	// 35k
	d = (TB_2_48->Position - TB_2_47->Position);d /= 4;
	TB_3_62->Position = TB_2_47->Position + (int)(d * 1);
	TB_3_63->Position = TB_2_47->Position + (int)(d * 2);
	TB_3_64->Position = TB_2_47->Position + (int)(d * 3);
	TB_3_65->Position = TB_2_47->Position + (int)(d * 4);
	TB_3_66->Position = TB_2_48->Position;	// 40k
	TB_3_67->Position = TB_2_49->Position;	// 45k
	TB_3_68->Position = TB_2_50->Position;	// 50k
	TB_3_69->Position = 0;
	TB_3_70->Position = 0;
	TB_3_71->Position = 0;
	TB_3_72->Position = 0;
	TB_3_73->Position = 0;
	TB_3_74->Position = 0;
	TB_3_75->Position = 0;
	TB_3_76->Position = 0;
	TB_3_77->Position = 0;
	TB_3_78->Position = 0;
	PRINT_FN_LOG("btn_type3_copy_from_type2Click","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnPreviewAppClick(TObject *Sender)
{
//	if (OpenDialog1->Execute()) {
//		if (FileExists(OpenDialog1->FileName)) {
//			edtPreviewApp->Text = OpenDialog1->FileName;
//		}
//	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edt_eq_titleChange(TObject *Sender)
{
	btnLoad->Tag = 1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btn_eq_deleteClick(TObject *Sender)
{
	int i,index,count;
	int last_flag;
	double *eq_ptr;
	char *p;
	AnsiString sStr;
	UnicodeString uStr,uStr2;

	PRINT_FN_LOG("btn_eq_deleteClick","Start");

	index = cmbEQParameter->ItemIndex;
	last_flag = 0;
	if (index + 1 == cmbEQParameter->Items->Count) {
		last_flag = 1;
	}
	count = cmbEQParameter->Items->Count;
	if (last_flag == 0) {
		for (i = index; i + 1 < cmbEQParameter->Items->Count ;i++) {
			uStr = edtParameter->Lines->Strings[i + 1];
			edtParameter->Lines->Strings[i] = uStr;
			uStr.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,i + 1);
			uStr2.sprintf(L"%s\\upconv_eq_%03d.txt",eqParameterPath,i + 2);
			DeleteFile(uStr.c_str());
			MoveFile(uStr2.c_str(),uStr.c_str());
		}
		edtParameter->Lines->Delete(i);
		cmbEQParameter->Items->Clear();
		cmbEQ_Analyze->Items->Clear();
		for (i = 0;i < count;i++) {
			sStr = edtParameter->Lines->Strings[i];
			p = strrchr(sStr.c_str(),'|');
			if (p != NULL) {
				uStr2.printf(L"%03d.%s",i + 1, (UnicodeString)(p + 1));
				cmbEQParameter->Items->Add(uStr2);
				cmbEQ_Analyze->Items->Add(uStr2);
			}
			if (edtParameter->Lines->Strings[i].Length() == 0) break;
		}
		sStr.printf("%03d.%s",i + 1,"EQ.Sample");
		cmbEQParameter->Items->Add(sStr);
	} else {
		edtParameter->Lines->Delete(index);
		cmbEQParameter->Items->Clear();
		cmbEQ_Analyze->Items->Clear();
		for (i = 0;i < edtParameter->Lines->Count;i++) {
			sStr = edtParameter->Lines->Strings[i];
			p = strrchr(sStr.c_str(),'|');
			if (p != NULL) {
				uStr2.printf(L"%03d.%s",i + 1, (UnicodeString)(p + 1));
				cmbEQParameter->Items->Add(uStr2);
				cmbEQ_Analyze->Items->Add(uStr2);
			}
			if (edtParameter->Lines->Strings[i].Length() == 0) break;
		}
		sStr.printf("%03d.%s",i + 1,"EQ.Sample");
		cmbEQParameter->Items->Add(sStr);
	}
	cmbEQParameter->ItemIndex = 0;
	cmbEQ_Analyze->ItemIndex  = 0;

	PRINT_FN_LOG("btn_eq_deleteClick","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::track_skip_positionChange(TObject *Sender)
{
	int hh1,hh2,mm1,mm2,ss1,ss2;
	int sec;
	SSIZE pos,nSample;
	UnicodeString uStr;

	PRINT_FN_LOG("track_skip_positionChange","Start");
	if (cmb_preview_length->ItemIndex == 0) sec = 5;
	if (cmb_preview_length->ItemIndex == 1) sec = 10;
	if (cmb_preview_length->ItemIndex == 2) sec = 15;
	if (cmb_preview_length->ItemIndex == 3) sec = 20;
	if (cmb_preview_length->ItemIndex == 4) sec = 25;
	if (cmb_preview_length->ItemIndex == 5) sec = 30;

	pos     = track_skip_position->Position;
	nSample = track_skip_position->Max;

	if (nSample > sec) {
		if ((nSample - pos) < sec) {
			track_skip_position->Position = track_skip_position->Max - sec;
		}
	} else {
		track_skip_position->Position = 0;
	}
	pos = track_skip_position->Position;

	hh1 = (pos / 60) / 60;
	mm1 = (pos / 60) % 60;
	ss1 = (pos % 60);

	hh2 = (nSample / 60) / 60;
	mm2 = (nSample / 60) % 60;
	ss2 = (nSample % 60);
	uStr.printf(L"Start Offset : %d:%02d:%02d / %d:%02d:%02d",hh1,mm1,ss1,hh2,mm2,ss2);
	lbl_selected_len->Caption = uStr;
	if (preview_input_filename != "") preview_input_filename_state_change = 1;
	PRINT_FN_LOG("track_skip_positionChange","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnPlayClick(TObject *Sender)
{
	int i;
	int offset;
	int sec;
	int index;
	char fullpath[_MAX_PATH];
	char drive2[_MAX_DRIVE];
	char dir2[_MAX_DIR];
	char file2[_MAX_FNAME];
	char ext2[_MAX_EXT];
	char sss[MAX_PATH];
	TListItem *item;
	AnsiString outDir;

	PRINT_FN_LOG("btnPlayClick","Start");

	item = NULL;

	preview_err_remain = 3;

	if (btnPlay->Tag == 0) {
		PRINT_LOG("Play");

		for (i = 0;i < LvFile->Items->Count;i++) {
			if (i == SelectFileIndex) {
				item = LvFile->Items->Item[i];
				break;
			}
		}

		if (i == LvFile->Items->Count || item == NULL) {
			PRINT_LOG("Play Abort");
			return;
		}

		item->ImageIndex = 0;
		item->Caption = L"";

		btnPlay->Tag = 1;

		btnPlay->Caption = L"...";

		offset = track_skip_position->Position;
		IndexSoundBuffer = 0;
		IndexPlayBuffer  = 0;
		CountSoundBuffer = 0;
		if (cmb_preview_length->ItemIndex == 0) sec = 5;
		if (cmb_preview_length->ItemIndex == 1) sec = 10;
		if (cmb_preview_length->ItemIndex == 2) sec = 15;
		if (cmb_preview_length->ItemIndex == 3) sec = 20;
		if (cmb_preview_length->ItemIndex == 4) sec = 25;
		if (cmb_preview_length->ItemIndex == 5) sec = 30;

		// Preview 用の音声の削除とデータの設定
		outDir = (AnsiString)eqParameterPath;
		outDir += "\\";
		_splitpath(outDir.c_str(), drive2, dir2, file2, ext2);
		sprintf(fullpath,"%s%s%s_%02d%s",drive2,dir2,"eq_preview_input",i,".wav");
		sprintf(sss,"Delete:%s",fullpath);
		PRINT_LOG(sss);
		DeleteFile(fullpath);

		preview_input_filename = "";
		if (chk_play_loop->Checked == true) {
			// 高速化のため44.1、48、96kHz のファイルを最初に作成する
			preview_input_filename = (AnsiString)fullpath;
		}

		for (i = 0;i < 16;i++) {
			if (SoundBuffer[i].whdr.lpData != NULL) {
				sprintf(sss,"Free:SoundBuffer[%d]",i);
				PRINT_LOG(sss);

				free(SoundBuffer[i].whdr.lpData);
				SoundBuffer[i].whdr.lpData = NULL;
			}
			memset(&SoundBuffer[i],0,sizeof (SOUND_BUFFER));
			SoundBuffer[i].status = BUFFER_FREE;
			SoundBuffer[i].offset = offset;
			SoundBuffer[i].sec    = sec;

			outDir = (AnsiString)eqParameterPath;
			outDir += "\\";
			_splitpath(outDir.c_str(), drive2, dir2, file2, ext2);
			sprintf(fullpath,"%s%s%s_%02d%s",drive2,dir2,"eq_preview",i,".err");
			sprintf(sss,"Delete:%s",fullpath);
			PRINT_LOG(sss);
			DeleteFile(fullpath);

			sprintf(fullpath,"%s%s%s_%02d%s",drive2,dir2,"eq_preview",i,".wav");
			sprintf(sss,"Delete:%s",fullpath);
			PRINT_LOG(sss);
			DeleteFile(fullpath);
			SoundBuffer[i].file_name = (AnsiString)fullpath;

			sprintf(fullpath,"%s%s%s_%02d%s",drive2,dir2,"eq_preview_fin",i,".wav");
			sprintf(sss,"Delete:%s",fullpath);
			PRINT_LOG(sss);
			DeleteFile(fullpath);
			SoundBuffer[i].fin_file_name = (AnsiString)fullpath;

			if (SoundBuffer[i].offset + SoundBuffer[i].sec >= track_skip_position->Max) {
				SoundBuffer[i].sec = track_skip_position->Max - offset;
			}
			//offset += SoundBuffer[i].sec;
			//if (SoundBuffer[i].offset + SoundBuffer[i].sec < track_skip_position->Max) {
				CountSoundBuffer++;
			//}
		}

		LvFile->Items->Item[SelectFileIndex]->ImageIndex = 0;
		LvFile->Items->Item[SelectFileIndex]->Caption = L"";
		btnPause->Tag = 0;

		btnStart->Enabled = false;
		btnPause->Enabled = false;
		btnExit->Enabled = false;
		prgExec->Visible = true;
		pageMain->Enabled = false;

		DragAcceptFiles(LvFile->Handle, false);
		LvFile->Items->Item[SelectFileIndex]->Selected = true;
		LvFile->Items->Item[SelectFileIndex]->MakeVisible(true);

		lbl_count->Caption.printf(L"Play(%d)",SelectFileIndex + 1);
		lbl_count->Visible = true;
		Abort = 0;
		VisibleIndex = 0;
		Timer3->Tag = 0;
		Timer3->Enabled = true;
		cmbEQParameter->Tag = cmbEQParameter->ItemIndex;
		pageMain->ActivePageIndex = 0;
	} else if (btnPlay->Tag == 1) {
		PRINT_LOG("Stop");
		for (index = 0; index < 1; index++) {
			if (threadExec[index] != NULL)
				threadExec[index]->AbortExec();
		}
		btnPlay->Tag = -1;
		btnPlay->Enabled = false;
		Timer4->Enabled = true;
	}
	PRINT_FN_LOG("btnPlayClick","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::InitSound(void)
{
	UnicodeString uStr;
	AnsiString sStr;
	char sss[MAX_PATH];
	int i;
	int sound_out_device_num = waveOutGetNumDevs();
	int m_44,s_44,m_48,s_48,m_96,s_96;
	int found,format_first;

	PRINT_FN_LOG("InitSound","Start");
	m_44 = s_44 = m_48 = s_48 = m_96 = s_96 = 0;

	sprintf(sss,"SoundDevice:%d",sound_out_device_num);
	PRINT_LOG(sss);

	for (i = 0;i < sound_out_device_num;i++) {
		WAVEOUTCAPS caps;
		if (waveOutGetDevCaps(i,&caps,sizeof(caps)) == MMSYSERR_NOERROR ) {
			found = 0;
			format_first = 1;
			if (caps.dwFormats & WAVE_FORMAT_4M16 || caps.dwFormats & WAVE_FORMAT_44M16) {
				// 44.1 kHz, mono, 16-bit 4M16
				m_44 = 1;
				found = 1;
			}
			if (caps.dwFormats & WAVE_FORMAT_4S16 || caps.dwFormats & WAVE_FORMAT_44S16) {
				// 44.1 kHz, stereo, 16-bit 4S16
				s_44 = 1;
				found = 1;
			}
			if (caps.dwFormats & WAVE_FORMAT_48M16) {
				// 48 kHz, mono, 16-bit
				m_48 = 1;
				found = 1;
			}
			if (caps.dwFormats & WAVE_FORMAT_48S16) {
				// 48 kHz, stereo, 16-bit
				s_48 = 1;
				found = 1;
			}
			if (caps.dwFormats & WAVE_FORMAT_96M16) {
				// 96 kHz, mono, 16-bit
				m_96 = 1;
				found = 1;
			}
			if (caps.dwFormats & WAVE_FORMAT_96S16) {
				// 96 kHz, stereo, 16-bit
				s_96 = 1;
				found = 1;
			}
			if (found) {
				uStr.printf(L"%d:%s|%d,%d,%d,%d,%d,%d",i,caps.szPname,m_44,s_44,m_48,s_48,m_96,s_96);
				memo_DeviceInfo->Lines->Add(uStr);
				uStr.printf(L"%d:%s",i + 1,caps.szPname);
				cmbPlayDevice->Items->Add(uStr);
				sStr = uStr;
				sprintf(sss,"DeviceInfo:%s",sStr.c_str());
				PRINT_LOG(sss);
			}
		}
	}
	PRINT_FN_LOG("InitSound","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ReadBuffer(int sb_index,int *buffer_ready_flag)
{
	int error;
	char sss[MAX_PATH];
	SSIZE dataSize;
	size_t read_size;
	FILE *ifp;

	PRINT_FN_LOG("ReadBuffer","Start");

	*buffer_ready_flag = 0;
	if (SoundBuffer[sb_index].status != BUFFER_CONVERT) {
		PRINT_LOG("state != BUFFER_CONVERT");
		return;
	}

	PLG_InfoAudioData(SoundBuffer[sb_index].fin_file_name.c_str(),&InFmt,&FilenSample,&FileInfo);
	if (FilenSample > 0) {
		sprintf(sss,"PLG_InfoAudioData:nSample %lld",FilenSample);
		PRINT_LOG(sss);
	}
	if (sb_index == 0) {
		sprintf(sss,"sb_index = 0,sample:%d,bit:%d,ch:%d",InFmt.sample,InFmt.bitsPerSample,InFmt.channel);
		PRINT_LOG(sss);
		Wfe.wFormatTag = WAVE_FORMAT_PCM;
		Wfe.nChannels  = InFmt.channel;
		Wfe.nSamplesPerSec = InFmt.sample;
		Wfe.wBitsPerSample = InFmt.bitsPerSample;
		Wfe.nBlockAlign = Wfe.nChannels * Wfe.wBitsPerSample / 8;
		Wfe.nAvgBytesPerSec = Wfe.nSamplesPerSec * Wfe.nBlockAlign;
	}

	dataSize = FileInfo.dataSize;

	ifp = fopen(SoundBuffer[sb_index].fin_file_name.c_str(),"rb");
	if (ifp == NULL) {
		PRINT_LOG("File is not found!");
		return;
	}
	fseek(ifp,FileInfo.dataOffset,SEEK_SET);

	dataSize = FileInfo.dataSize;

	error = 1;

	if (SoundBuffer[sb_index].whdr.lpData != NULL) {
		free(SoundBuffer[sb_index].whdr.lpData);
	}
	SoundBuffer[sb_index].whdr.lpData = (char *)malloc(dataSize);
	if (SoundBuffer[sb_index].whdr.lpData != NULL) {
		memset(SoundBuffer[sb_index].whdr.lpData,0,dataSize);
		read_size = fread(SoundBuffer[sb_index].whdr.lpData,1,dataSize,ifp);
		if (read_size > 0 && read_size == dataSize) {
			error = 0;
		}
		if (error == 0) {
			SoundBuffer[sb_index].status = BUFFER_READY;
			SoundBuffer[sb_index].whdr.dwBufferLength = dataSize;
			SoundBuffer[sb_index].whdr.dwUser = 0;
			*buffer_ready_flag = 1;
			sprintf(sss,"sb_index:%d",sb_index);
			PRINT_LOG(sss);
		}
	}

	if (ifp != NULL) fclose(ifp);
	PRINT_FN_LOG("ReadBuffer","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PlaySound(void)
{
	int ret;
	int next;
	int dn;
	char sss[MAX_PATH];
	UINT deviceID;
	AnsiString sStr;

	PRINT_FN_LOG("PlaySound","Start");

	sStr = memo_DeviceInfo->Lines->Strings[cmbPlayDevice->ItemIndex];
	if (sscanf(sStr.c_str(),"%d:",&dn) == 1) {
		deviceID = dn;
	} else {
		return;
	}

	sprintf(sss,"deviceID:%d",deviceID);
	PRINT_LOG(sss);

	if (rdo_preview_rate_44100->Checked == true && BeforeRateConv != 44100) {
		cmbPlayDevice->Tag = 1;
		BeforeRateConv = 44100;
	}
	if (rdo_preview_rate_48000->Checked == true && BeforeRateConv != 48000) {
		cmbPlayDevice->Tag = 1;
		BeforeRateConv = 48000;
	}
	if (rdo_preview_rate_96000->Checked == true && BeforeRateConv != 96000) {
		cmbPlayDevice->Tag = 1;
		BeforeRateConv = 96000;
	}

	if (WaveOut != NULL && cmbPlayDevice->Tag == 1) {
		waveOutReset(WaveOut);
		PRINT_LOG("waveOutUnprepareHeader");
		waveOutUnprepareHeader(WaveOut,&SoundBuffer[IndexPlayBuffer].whdr,sizeof (WAVEHDR));
		PRINT_LOG("waveOutClose");
		waveOutClose(WaveOut);
		PRINT_LOG("OK");
		WaveOut = NULL;
	}
	cmbPlayDevice->Tag = 0;

	if (WaveOut == NULL) {
		ret = waveOutOpen(&WaveOut,deviceID,&Wfe,(DWORD)Form1->Handle,0,CALLBACK_WINDOW);
		sprintf(sss,"waveOutOpen:%d",ret);
		PRINT_LOG(sss);
		if(ret != MMSYSERR_NOERROR) return;
	}

	PRINT_LOG("Check:status");
	if (SoundBuffer[IndexPlayBuffer].status == BUFFER_PLAY_DONE) {
		sprintf(sss,"status[%d] = BUFFER_PLAY_DONE",IndexPlayBuffer);
		PRINT_LOG(sss);
		waveOutUnprepareHeader(WaveOut,&SoundBuffer[IndexPlayBuffer].whdr,sizeof (WAVEHDR));
		SoundBuffer[IndexPlayBuffer].status = BUFFER_FREE;
		IndexPlayBuffer++;
		if (IndexPlayBuffer >= CountSoundBuffer) {
			IndexPlayBuffer = 0;
		}
		next = IndexPlayBuffer;
		do {
			next++;
			if (next >= CountSoundBuffer) {
				next = 0;
			}
			if (SoundBuffer[next].status == BUFFER_READY) {
				IndexPlayBuffer = next;
			} else {
				break;
			}
		} while (1);
	}

	PRINT_LOG("Check:status");
	if (SoundBuffer[IndexPlayBuffer].status == BUFFER_READY) {
		sprintf(sss,"status[%d] = BUFFER_PLAY_DONE",IndexPlayBuffer);
		PRINT_LOG(sss);
		ret = waveOutPrepareHeader(WaveOut,&SoundBuffer[IndexPlayBuffer].whdr,sizeof (WAVEHDR));
		sprintf(sss,"waveOutPrepareHeader:%d",ret);
		PRINT_LOG(sss);
		if(ret != MMSYSERR_NOERROR) return;
		ret = waveOutWrite(WaveOut,&SoundBuffer[IndexPlayBuffer].whdr,sizeof (WAVEHDR));
		sprintf(sss,"waveOutWrite:%d",ret);
		PRINT_LOG(sss);
		if(ret != MMSYSERR_NOERROR) return;
		SoundBuffer[IndexPlayBuffer].status = BUFFER_PLAY;
		btnPlay->Caption = L"Stop";
	}
	PRINT_FN_LOG("PlaySound","Stop");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::StopSound(void)
{
	PRINT_FN_LOG("StopSound","Start");
	if (WaveOut != NULL) {
		PRINT_LOG("waveOutReset");
		waveOutReset(WaveOut);
		PRINT_LOG("waveOutUnprepareHeader");
		waveOutUnprepareHeader(WaveOut,&SoundBuffer[IndexPlayBuffer].whdr,sizeof (WAVEHDR));
		PRINT_LOG("waveOutClose");
		waveOutClose(WaveOut);
		PRINT_LOG("OK");
		WaveOut = NULL;
		SoundBuffer[IndexPlayBuffer].status = BUFFER_FREE;
	}
	PRINT_FN_LOG("StopSound","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MmWomOpen(TMessage &Message)
{
	PRINT_FN_LOG("MmWomOpen","Start");
	PRINT_FN_LOG("MmWomOpen","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MmWomClose(TMessage &Message)
{
	PRINT_FN_LOG("MmWomClose","Start");
	PRINT_FN_LOG("MmWomClose","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MmWomDone(TMessage &Message)
{
	char sss[MAX_PATH];
	PRINT_FN_LOG("MmWomDone","Start");
	sprintf(sss,"waveOutUnprepareHeader:%d",IndexPlayBuffer);
	PRINT_LOG(sss);
	waveOutUnprepareHeader(WaveOut,&SoundBuffer[IndexPlayBuffer].whdr,sizeof (WAVEHDR));
	SoundBuffer[IndexPlayBuffer].status = BUFFER_PLAY_DONE;
	PRINT_FN_LOG("MmWomDone","End");
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btn_ana_addClick(TObject *Sender)
{
	TListItem *item;
	AnsiString s;
	int i;
	int ret;
	long time;
	long size;

	PRINT_FN_LOG("btn_ana_addClick","Start");
	if (OpenDialog->Execute()) {
		for (i = 0; i < OpenDialog->Files->Count; i++) {
			AddToAnaFileList(OpenDialog->Files->Strings[i]);
		}
		if (LvAnaFile->Items->Count > 0) {
			btnAnaClear->Enabled = true;
		}
	}
	PRINT_FN_LOG("btn_ana_addClick","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAnaDelClick(TObject *Sender)
{
	TListItem *item;

	PRINT_FN_LOG("btnAnaDelClick","Start");
	if (LvAnaFile->Selected != NULL) {
		item = LvAnaFile->Selected;
		if (item->Data) {
			delete[]item->Data;
		}
		item->Delete();
		if (LvAnaFile->Items->Count == 0) {
			btnStart->Enabled = false;
			btnClear->Enabled = false;
			btnAnaUp->Enabled = false;
			btnAnaDown->Enabled = false;
		}
	}
	PRINT_FN_LOG("btnAnaDelClick","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAnaClearClick(TObject *Sender)
{
	PRINT_FN_LOG("btnAnaClearClick","Start");
	LvAnaFile->Items->Clear();
	btnAnaClear->Enabled = false;
	btnAnaDel->Enabled = false;
	btnAnaUp->Enabled = false;
	btnAnaDown->Enabled = false;
	btnStart->Enabled = false;
	PRINT_FN_LOG("btnAnaClearClick","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAnaUpClick(TObject *Sender)
{
	UnicodeString s;
	TListItem *item, *new_item;

	if (LvAnaFile->Selected != NULL && LvAnaFile->Selected->Index > 0) {
		item = LvAnaFile->Selected;
		LvAnaFile->Items->BeginUpdate();
		new_item = LvAnaFile->Items->Insert(item->Index - 1);
		new_item->Assign(item);
		new_item->SubItems->Add(item->SubItems->Strings[1]);
		new_item->SubItems->Add(item->SubItems->Strings[2]);
		item->Delete();
		new_item->Selected = TRUE;
		new_item->MakeVisible(TRUE);
		LvAnaFile->Items->EndUpdate();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAnaDownClick(TObject *Sender)
{
	UnicodeString s;
	TListItem *item, *new_item;
	if (LvAnaFile->Selected != NULL && LvAnaFile->Selected->Index + 1 <
		LvAnaFile->Items->Count) {
		item = LvAnaFile->Selected;
		LvAnaFile->Items->BeginUpdate();
		new_item = LvAnaFile->Items->Insert(item->Index + 2);
		new_item->Assign(item);
		new_item->SubItems->Add(item->SubItems->Strings[1]);
		new_item->SubItems->Add(item->SubItems->Strings[2]);
		item->Delete();
		new_item->Selected = TRUE;
		new_item->MakeVisible(TRUE);
		LvAnaFile->Items->EndUpdate();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::pageMainChange(TObject *Sender)
{
	PRINT_FN_LOG("pageMainChange","Start");
	if (pageMain->ActivePage == tabUpconv) {
		if (LvFile->Items->Count > 0) {
			btnStart->Enabled = true;
		} else {
			btnStart->Enabled = false;
		}
	} else if (pageMain->ActivePage == tabAnalyze) {
		if (LvAnaFile->Items->Count > 0) {
			btnStart->Enabled = true;
		} else {
			btnStart->Enabled = false;
		}
	}
	PRINT_FN_LOG("pageMainChange","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LvAnaFileSelectItem(TObject *Sender, TListItem *Item, bool Selected)
{
	PRINT_FN_LOG("LvAnaFileSelectItem","Start");
	if (LvAnaFile->Items->Count > 0) {
		btnAnaDel->Enabled = true;
		if (LvAnaFile->Selected != NULL && LvAnaFile->Selected->Index > 0) {
			btnAnaUp->Enabled = true;
		} else {
			btnAnaUp->Enabled = false;
		}
		if (LvAnaFile->Selected != NULL && LvAnaFile->Selected->Index + 1 < LvAnaFile->Items->Count) {
			btnAnaDown->Enabled = true;
		} else {
			btnAnaDown->Enabled = false;
		}
		if (cmbEQ_Analyze->ItemIndex > 0) {
			rdo_Analyze_Phase1->Enabled = true;
		} else {
			rdo_Analyze_Phase1->Enabled = false;
		}
	}
	PRINT_FN_LOG("LvAnaFileSelectItem","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbEQParameterChange(TObject *Sender)
{
	PRINT_FN_LOG("cmbEQParameterChange","Start");
	if (cmbEQParameter->ItemIndex > 0) {
		btn_eq_delete->Enabled = true;
	} else {
		btn_eq_delete->Enabled = false;
	}
	btn_eq_save->Enabled = false;
	PRINT_FN_LOG("cmbEQParameterChange","End");
}
//---------------------------------------------------------------------------


void __fastcall TForm1::cmbEQ_AnalyzeChange(TObject *Sender)
{
	PRINT_FN_LOG("cmbEQ_AnalyzeChange","Start");
	if (LvAnaFile->Items->Count > 0 && LvAnaFile->ItemIndex >= 0) {
		btnStart->Enabled = true;
	}
	if (cmbEQ_Analyze->ItemIndex > 0) {
		rdo_Analyze_Phase1->Enabled = true;
	} else {
		rdo_Analyze_Phase1->Enabled = false;
	}
	PRINT_FN_LOG("cmbEQ_AnalyzeChange","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LvAnaFileChange(TObject *Sender, TListItem *Item, TItemChange Change)
{
	PRINT_FN_LOG("LvAnaFileChange","Start");
	if (LvAnaFile->Items->Count > 0 && cmbEQ_Analyze->ItemIndex >= 0) {
		btnStart->Enabled = true;
	} else {
		btnStart->Enabled = false;
	}
	PRINT_FN_LOG("LvAnaFileChange","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rdo_Analyze_Phase1Click(TObject *Sender)
{
	PRINT_FN_LOG("rdo_Analyze_Phase1Click","Start");
	AnalyzePhase = 1;
	btn_EQ_Set->Enabled = false;
	PRINT_FN_LOG("rdo_Analyze_Phase1Click","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rdo_Analyze_Phase2Click(TObject *Sender)
{
	UnicodeString uStr;
	int error;

	PRINT_FN_LOG("rdo_Analyze_Phase2Click","Start");

	error = 0;
	uStr.printf(L"%s\\upconv_eq_%03d.analyze1",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == FALSE) {
		error = 1;
	}
	if (error == 1) {
		if (AnalyzePhase == 1) {
			rdo_Analyze_Phase1->Checked = TRUE;
		} else if (AnalyzePhase == 2) {
			rdo_Analyze_Phase2->Checked = TRUE;
		} else if (AnalyzePhase == 3) {
			rdo_Analyze_Phase3->Checked = TRUE;
		}
	} else {
		AnalyzePhase = 2;
		btn_EQ_Set->Enabled = false;
	}
	PRINT_FN_LOG("rdo_Analyze_Phase2Click","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rdo_Analyze_Phase3Click(TObject *Sender)
{
	UnicodeString uStr;
	int error;

	PRINT_FN_LOG("rdo_Analyze_Phase3Click","Start");
	error = 0;
	uStr.printf(L"%s\\upconv_eq_%03d.analyze1",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == FALSE) {
		error = 1;
	}
	uStr.printf(L"%s\\upconv_eq_%03d.analyze2",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == FALSE) {
		error = 1;
	}
	if (error == 1) {
		if (AnalyzePhase == 1) {
			rdo_Analyze_Phase1->Checked = TRUE;
		} else if (AnalyzePhase == 2) {
			rdo_Analyze_Phase2->Checked = TRUE;
		} else if (AnalyzePhase == 3) {
			rdo_Analyze_Phase3->Checked = TRUE;
		}
	} else {
		AnalyzePhase = 3;
		btn_EQ_Set->Enabled = true;
	}
	PRINT_FN_LOG("rdo_Analyze_Phase3Click","End");
}
//---------------------------------------------------------------------------


void __fastcall TForm1::btn_EQ_SetClick(TObject *Sender)
{
	PRINT_FN_LOG("btn_EQ_SetClick","Start");
	if (PageEQ->ActivePage == EQ_Type1) {
		EQ_AutoSetType1();
	}
	if (PageEQ->ActivePage == EQ_Type2) {
		EQ_AutoSetType2();
	}
	if (PageEQ->ActivePage == EQ_Type3) {
		EQ_AutoSetType3();
	}
	PRINT_FN_LOG("btn_EQ_SetClick","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EQ_AutoSetType1(void)
{
	UnicodeString uStr;
	AnsiString sStr;
	int i,ii,c,n,v,e,error,size;
	FILE *ifp;
	long eq_hz[]={20,25,31,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6300,8000,10000,12500,16000,20000,25000,31500,40000,50000,52000,53000,54000,55000,56000};
	double per1[]={0.1995,0.2238,0.2511,0.2818,0.3162,0.3548,0.3981,0.4466,0.5011,0.5623,0.6309,0.7079,0.7943,0.8912,1.0,1.122,1.2589,1.4125,1.5848,1.7782,1.9952,2.2387,2.511,2.8183,3.1622,3.5481,3.981,4.4668,5.0118};
	double analyze1[192000];
	double analyze2[192000];
	double eq_val[35];
	double p;
	double nx1,nx2;
	__int64 p2;
	double p1,p3,p4;
	double ana_avg;
	double level_per,level_per1,level_per2;

	PRINT_FN_LOG("EQ_AutoSetType1","Start");
	memset(analyze1,0,192000 * sizeof (double));
	memset(analyze2,0,192000 * sizeof (double));
	memset(eq_val,0,35 * sizeof (double));
	error = 0;
	level_per1 = level_per2 = 0;

	size = (sizeof (per1) / sizeof (double));
	uStr.printf(L"%s\\upconv_eq_%03d.analyze1",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == true) {
		sStr = uStr;
		ifp = fopen(sStr.c_str(),"rt");
		if (ifp != NULL) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(ifp,"%lf\n",&p) == 1) {
					analyze1[i] = p;
				} else {
					error = 1;
					break;
				}
			}
			if (fscanf(ifp,"%lf,%lld,%lf,%lf\n",&p1,&p2,&p3,&p4) == 4) {
				nx1 = p1;
				if (fscanf(ifp,"volume_level:%lf",&level_per) == 1) {
					level_per1 = level_per;
				}
			} else {
				error = 1;
			}
			fclose(ifp);
		}
	}
	if (error == 0) {
		// 平均化する
		for (i = 0;i < 192000;i+=2048) {
			ana_avg = 0;
			for (ii = i,c = 0;c < 2048;ii++,c++) {
				ana_avg += analyze1[i];
			}
			if (c > 0) {
				ana_avg /= c;
				for (ii = i,c = 0;c < 2048;ii++,c++) {
					analyze1[i] = ana_avg;
				}
			}
		}
	}
	uStr.printf(L"%s\\upconv_eq_%03d.analyze2",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == true) {
		sStr = uStr;
		ifp = fopen(sStr.c_str(),"rt");
		if (ifp != NULL) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(ifp,"%lf\n",&p) == 1) {
					analyze2[i] = p;
				} else {
					error = 1;
					break;
				}
			}
			if (fscanf(ifp,"%lf,%lld,%lf,%lf\n",&p1,&p2,&p3,&p4) == 4) {
				nx2 = p1;
				if (fscanf(ifp,"volume_level:%lf",&level_per) == 1) {
					level_per2 = level_per;
				}
			} else {
				error = 1;
			}
			fclose(ifp);
		}
	}
	if (error == 0) {
		// 平均化する
		for (i = 0;i < 192000;i+=2048) {
			ana_avg = 0;
			for (ii = i,c = 0;c < 2048;ii++,c++) {
				ana_avg += analyze2[i];
			}
			if (c > 0) {
				ana_avg /= c;
				for (ii = i,c = 0;c < 2048;ii++,c++) {
					analyze2[i] = ana_avg;
				}
			}
		}
	}
	if (error == 0) {
		if (nx2 > 0) {
			nx1 = nx1 / nx2;
		} else {
			nx1 = 1.0;
		}
		if (level_per1 > 0 && level_per2 > 0) {
			nx1 = level_per2 / level_per1;
		}
		for (i = 0,e = 0,n = 0;i < 192000;i++) {
			if (analyze2[i] > 0) {
				analyze1[i] = analyze2[i] / (analyze1[i] * nx1);
			} else {
				analyze1[i] = 0;
			}
			if (i >= eq_hz[e]) {
				if (n > 0) {
					eq_val[e] /= n;
				}
				e++;
				n = 0;
			}
			if (eq_hz[e] > 50000) break;
			eq_val[e] = eq_val[e] + analyze1[i];
			n++;
		}
		if (n > 0) {
			eq_val[e] /= n;
		}
		if (eq_val[0] <= per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[0] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_1->Position = v;

		if (eq_val[1] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[1] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_2->Position = v;

		if (eq_val[2] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[2] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_3->Position = v;

		if (eq_val[3] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[3] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_4->Position = v;

		if (eq_val[4] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[4] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_5->Position = v;

		if (eq_val[5] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[5] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_6->Position = v;

		if (eq_val[6] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[6] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_7->Position = v;

		if (eq_val[7] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[7] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_8->Position = v;

		if (eq_val[8] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[8] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_9->Position = v;

		if (eq_val[9] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[9] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_10->Position = v;

		if (eq_val[10] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[10] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_11->Position = v;

		if (eq_val[11] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[11] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_12->Position = v;

		if (eq_val[12] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[12] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_13->Position = v;

		if (eq_val[13] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[13] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_14->Position = v;

		if (eq_val[14] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[14] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_15->Position = v;

		if (eq_val[15] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[15] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_16->Position = v;

		if (eq_val[16] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[16] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_17->Position = v;

		if (eq_val[17] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[17] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_18->Position = v;

		if (eq_val[18] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[18] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_19->Position = v;

		if (eq_val[19] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[19] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_20->Position = v;

		if (eq_val[20] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[20] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_21->Position = v;

		if (eq_val[21] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[21] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_22->Position = v;

		if (eq_val[22] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[22] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_23->Position = v;

		if (eq_val[23] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[23] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_24->Position = v;

		if (eq_val[24] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[24] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_25->Position = v;

		if (eq_val[25] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[25] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_26->Position = v;

		if (eq_val[26] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[26] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_27->Position = v;

		if (eq_val[27] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[27] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_28->Position = v;

		if (eq_val[28] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[28] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_29->Position = v;

		if (eq_val[29] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[29] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_30->Position = v;

		if (eq_val[30] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[30] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_31->Position = v;

		if (eq_val[31] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[31] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_32->Position = v;

		if (eq_val[32] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[32] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_33->Position = v;

		if (eq_val[33] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[33] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_34->Position = v;

		if (eq_val[34] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[34] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_1_35->Position = v;

	}
	PRINT_FN_LOG("EQ_AutoSetType1","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EQ_AutoSetType2(void)
{
	UnicodeString uStr;
	AnsiString sStr;
	int i,ii,c,n,v,e,error,size;
	FILE *ifp;
	long eq_hz[]={20,25,31,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6000,7000,8000,9000,10000,11000,12000,13000,14000,15000,16000,17000,18000,19000,20000,21000,22000,23000,24000,25000,30000,35000,40000,45000,50000,52000,53000,54000,55000,56000};
	double per1[]={0.1995,0.2238,0.2511,0.2818,0.3162,0.3548,0.3981,0.4466,0.5011,0.5623,0.6309,0.7079,0.7943,0.8912,1.0,1.122,1.2589,1.4125,1.5848,1.7782,1.9952,2.2387,2.511,2.8183,3.1622,3.5481,3.981,4.4668,5.0118};
	double analyze1[192000];
	double analyze2[192000];
	double eq_val[50];
	double p;
	double nx1,nx2;
	__int64 p2;
	double p1,p3,p4;
	double ana_avg;

	PRINT_FN_LOG("EQ_AutoSetType2","Start");
	memset(analyze1,0,192000 * sizeof (double));
	memset(analyze2,0,192000 * sizeof (double));
	memset(eq_val,0,50 * sizeof (double));
	error = 0;
	size = (sizeof (per1) / sizeof (double));
	uStr.printf(L"%s\\upconv_eq_%03d.analyze1",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == true) {
		sStr = uStr;
		ifp = fopen(sStr.c_str(),"rt");
		if (ifp != NULL) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(ifp,"%lf\n",&p) == 1) {
					analyze1[i] = p;
				} else {
					error = 1;
					break;
				}
			}
			if (fscanf(ifp,"%lf,%lld,%lf,%lf\n",&p1,&p2,&p3,&p4) == 4) {
				nx1 = p1;
			} else {
				error = 1;
			}
			fclose(ifp);
		}
	}
	if (error == 0) {
		// 平均化する
		for (i = 0;i < 192000;i+=2048) {
			ana_avg = 0;
			for (ii = i,c = 0;c < 2048;ii++,c++) {
				ana_avg += analyze1[i];
			}
			if (c > 0) {
				ana_avg /= c;
				for (ii = i,c = 0;c < 2048;ii++,c++) {
					analyze1[i] = ana_avg;
				}
			}
		}
	}

	uStr.printf(L"%s\\upconv_eq_%03d.analyze2",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == true) {
		sStr = uStr;
		ifp = fopen(sStr.c_str(),"rt");
		if (ifp != NULL) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(ifp,"%lf\n",&p) == 1) {
					analyze2[i] = p;
				} else {
					error = 1;
					break;
				}
			}
			if (fscanf(ifp,"%lf,%lld,%lf,%lf\n",&p1,&p2,&p3,&p4) == 4) {
				nx2 = p1;
			} else {
				error = 1;
			}
			fclose(ifp);
		}
	}
	if (error == 0) {
		// 平均化する
		for (i = 0;i < 192000;i+=2048) {
			ana_avg = 0;
			for (ii = i,c = 0;c < 2048;ii++,c++) {
				ana_avg += analyze2[i];
			}
			if (c > 0) {
				ana_avg /= c;
				for (ii = i,c = 0;c < 2048;ii++,c++) {
					analyze2[i] = ana_avg;
				}
			}
		}
	}
	if (error == 0) {
		if (nx2 > 0) {
			nx1 = nx1 / nx2;
		} else {
			nx1 = 1.0;
		}
		for (i = 0,e = 0,n = 0;i < 192000;i++) {
			if (analyze2[i] > 0) {
				analyze1[i] = analyze2[i] / (analyze1[i] * nx1);
			} else {
				analyze1[i] = 0;
			}
			if (i >= eq_hz[e]) {
				if (n > 0) {
					eq_val[e] /= n;
				}
				e++;
				n = 0;
			}
			if (eq_hz[e] > 50000) break;
			eq_val[e] = eq_val[e] + analyze1[i];
			n++;
		}
		if (n > 0) {
			eq_val[e] /= n;
		}
		if (eq_val[0] <= per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[0] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_1->Position = v;

		if (eq_val[1] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[1] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_2->Position = v;

		if (eq_val[2] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[2] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_3->Position = v;

		if (eq_val[3] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[3] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_4->Position = v;

		if (eq_val[4] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[4] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_5->Position = v;

		if (eq_val[5] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[5] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_6->Position = v;

		if (eq_val[6] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[6] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_7->Position = v;

		if (eq_val[7] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[7] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_8->Position = v;

		if (eq_val[8] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[8] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_9->Position = v;

		if (eq_val[9] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[9] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_10->Position = v;

		if (eq_val[10] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[10] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_11->Position = v;

		if (eq_val[11] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[11] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_12->Position = v;

		if (eq_val[12] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[12] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_13->Position = v;

		if (eq_val[13] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[13] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_14->Position = v;

		if (eq_val[14] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[14] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_15->Position = v;

		if (eq_val[15] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[15] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_16->Position = v;

		if (eq_val[16] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[16] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_17->Position = v;

		if (eq_val[17] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[17] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_18->Position = v;

		if (eq_val[18] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[18] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_19->Position = v;

		if (eq_val[19] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[19] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_20->Position = v;

		if (eq_val[20] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[20] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_21->Position = v;

		if (eq_val[21] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[21] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_22->Position = v;

		if (eq_val[22] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[22] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_23->Position = v;

		if (eq_val[23] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[23] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_24->Position = v;

		if (eq_val[24] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[24] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_25->Position = v;

		if (eq_val[25] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[25] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_26->Position = v;

		if (eq_val[26] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[26] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_27->Position = v;

		if (eq_val[27] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[27] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_28->Position = v;

		if (eq_val[28] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[28] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_29->Position = v;

		if (eq_val[29] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[29] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_30->Position = v;

		if (eq_val[30] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[30] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_31->Position = v;

		if (eq_val[31] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[31] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_32->Position = v;

		if (eq_val[32] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[32] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_33->Position = v;

		if (eq_val[33] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[33] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_34->Position = v;

		if (eq_val[34] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[34] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_35->Position = v;

		if (eq_val[35] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[35] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_36->Position = v;

		if (eq_val[36] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[36] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_37->Position = v;

		if (eq_val[37] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[37] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_38->Position = v;

		if (eq_val[38] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[38] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_39->Position = v;

		if (eq_val[39] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[39] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_40->Position = v;

		if (eq_val[40] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[40] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_41->Position = v;

		if (eq_val[41] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[41] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_42->Position = v;

		if (eq_val[42] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[42] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_43->Position = v;

		if (eq_val[43] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[43] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_44->Position = v;

		if (eq_val[44] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[44] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_45->Position = v;

		if (eq_val[45] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[45] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_46->Position = v;

		if (eq_val[46] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[46] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_47->Position = v;

		if (eq_val[47] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[47] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_48->Position = v;

		if (eq_val[48] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[48] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_49->Position = v;

		if (eq_val[49] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[49] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_2_50->Position = v;
	}
	PRINT_FN_LOG("EQ_AutoSetType2","End");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EQ_AutoSetType3(void)
{
	UnicodeString uStr;
	AnsiString sStr;
	int i,ii,c,n,v,e,error,size;
	FILE *ifp;
	long eq_hz[]={5,6,8,10,12,16,20,25,31,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6000,7000,8000,9000,10000,11000,12000,13000,14000,15000,16000,17000,18000,19000,20000,21000,22000,23000,24000,25000,26000,27000,28000,29000,30000,31000,32000,33000,34000,35000,36000,37000,38000,39000,40000,45000,50000,55000,60000,65000,70000,75000,80000,85000,90000,95000,100000,102000,103000,104000,105000,106000};
	double per1[]={0.1995,0.2238,0.2511,0.2818,0.3162,0.3548,0.3981,0.4466,0.5011,0.5623,0.6309,0.7079,0.7943,0.8912,1.0,1.122,1.2589,1.4125,1.5848,1.7782,1.9952,2.2387,2.511,2.8183,3.1622,3.5481,3.981,4.4668,5.0118};
	double analyze1[192000];
	double analyze2[192000];
	double eq_val[80];
	double p;
	double nx1,nx2;
	__int64 p2;
	double p1,p3,p4;
	double ana_avg;

	PRINT_FN_LOG("EQ_AutoSetType3","Start");
	memset(analyze1,0,192000 * sizeof (double));
	memset(analyze2,0,192000 * sizeof (double));
	memset(eq_val,0,80 * sizeof (double));
	error = 0;
	size = (sizeof (per1) / sizeof (double));
	uStr.printf(L"%s\\upconv_eq_%03d.analyze1",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == true) {
		sStr = uStr;
		ifp = fopen(sStr.c_str(),"rt");
		if (ifp != NULL) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(ifp,"%lf\n",&p) == 1) {
					analyze1[i] = p;
				} else {
					error = 1;
					break;
				}
			}
			if (fscanf(ifp,"%lf,%lld,%lf,%lf\n",&p1,&p2,&p3,&p4) == 4) {
				nx1 = p1;
			} else {
				error = 1;
			}
			fclose(ifp);
		}
	}
	if (error == 0) {
		// 平均化する
		for (i = 0;i < 192000;i+=2048) {
			ana_avg = 0;
			for (ii = i,c = 0;c < 2048;ii++,c++) {
				ana_avg += analyze1[i];
			}
			if (c > 0) {
				ana_avg /= c;
				for (ii = i,c = 0;c < 2048;ii++,c++) {
					analyze1[i] = ana_avg;
				}
			}
		}
	}

	uStr.printf(L"%s\\upconv_eq_%03d.analyze2",eqParameterPath,cmbEQ_Analyze->ItemIndex + 1);
	if (PathFileExists(uStr.c_str()) == true) {
		sStr = uStr;
		ifp = fopen(sStr.c_str(),"rt");
		if (ifp != NULL) {
			for (i = 0;i < 192000;i++) {
				if (fscanf(ifp,"%lf\n",&p) == 1) {
					analyze2[i] = p;
				} else {
					error = 1;
					break;
				}
			}
			if (fscanf(ifp,"%lf,%lld,%lf,%lf\n",&p1,&p2,&p3,&p4) == 4) {
				nx2 = p1;
			} else {
				error = 1;
			}
			fclose(ifp);
		}
	}
	if (error == 0) {
		// 平均化する
		for (i = 0;i < 192000;i+=2048) {
			ana_avg = 0;
			for (ii = i,c = 0;c < 2048;ii++,c++) {
				ana_avg += analyze2[i];
			}
			if (c > 0) {
				ana_avg /= c;
				for (ii = i,c = 0;c < 2048;ii++,c++) {
					analyze2[i] = ana_avg;
				}
			}
		}
	}
	if (error == 0) {
		if (nx2 > 0) {
			nx1 = nx1 / nx2;
		} else {
			nx1 = 1.0;
		}
		for (i = 0,e = 0,n = 0;i < 192000;i++) {
			if (analyze2[i] > 0) {
				analyze1[i] = analyze2[i] / (analyze1[i] * nx1);
			} else {
				analyze1[i] = 0;
			}
			if (i >= eq_hz[e]) {
				if (n > 0) {
					eq_val[e] /= n;
				}
				e++;
				n = 0;
			}
			if (eq_hz[e] > 50000) break;
			eq_val[e] = eq_val[e] + analyze1[i];
			n++;
		}
		if (n > 0) {
			eq_val[e] /= n;
		}
		if (eq_val[0] <= per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[0] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_1->Position = v;

		if (eq_val[1] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[1] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_2->Position = v;

		if (eq_val[2] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[2] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_3->Position = v;

		if (eq_val[3] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[3] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_4->Position = v;

		if (eq_val[4] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[4] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_5->Position = v;

		if (eq_val[5] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[5] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_6->Position = v;

		if (eq_val[6] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[6] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_7->Position = v;

		if (eq_val[7] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[7] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_8->Position = v;

		if (eq_val[8] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[8] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_9->Position = v;

		if (eq_val[9] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[9] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_10->Position = v;

		if (eq_val[10] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[10] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_11->Position = v;

		if (eq_val[11] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[11] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_12->Position = v;

		if (eq_val[12] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[12] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_13->Position = v;

		if (eq_val[13] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[13] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_14->Position = v;

		if (eq_val[14] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[14] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_15->Position = v;

		if (eq_val[15] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[15] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_16->Position = v;

		if (eq_val[16] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[16] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_17->Position = v;

		if (eq_val[17] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[17] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_18->Position = v;

		if (eq_val[18] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[18] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_19->Position = v;

		if (eq_val[19] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[19] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_20->Position = v;

		if (eq_val[20] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[20] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_21->Position = v;

		if (eq_val[21] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[21] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_22->Position = v;

		if (eq_val[22] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[22] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_23->Position = v;

		if (eq_val[23] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[23] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_24->Position = v;

		if (eq_val[24] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[24] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_25->Position = v;

		if (eq_val[25] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[25] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_26->Position = v;

		if (eq_val[26] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[26] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_27->Position = v;

		if (eq_val[27] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[27] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_28->Position = v;

		if (eq_val[28] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[28] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_29->Position = v;

		if (eq_val[29] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[29] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_30->Position = v;

		if (eq_val[30] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[30] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_31->Position = v;

		if (eq_val[31] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[31] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_32->Position = v;

		if (eq_val[32] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[32] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_33->Position = v;

		if (eq_val[33] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[33] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_34->Position = v;

		if (eq_val[34] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[34] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_35->Position = v;

		if (eq_val[35] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[35] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_36->Position = v;

		if (eq_val[36] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[36] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_37->Position = v;

		if (eq_val[37] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[37] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_38->Position = v;

		if (eq_val[38] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[38] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_39->Position = v;

		if (eq_val[39] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[39] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_40->Position = v;

		if (eq_val[40] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[40] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_41->Position = v;

		if (eq_val[41] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[41] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_42->Position = v;

		if (eq_val[42] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[42] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_43->Position = v;

		if (eq_val[43] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[43] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_44->Position = v;

		if (eq_val[44] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[44] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_45->Position = v;

		if (eq_val[45] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[45] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_46->Position = v;

		if (eq_val[46] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[46] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_47->Position = v;

		if (eq_val[47] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[47] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_48->Position = v;

		if (eq_val[48] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[48] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_49->Position = v;

		if (eq_val[49] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[49] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_50->Position = v;

		if (eq_val[50] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[50] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_51->Position = v;

		if (eq_val[51] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[51] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_52->Position = v;

		if (eq_val[52] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[52] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_53->Position = v;

		if (eq_val[53] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[53] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_54->Position = v;

		if (eq_val[54] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[54] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_55->Position = v;

		if (eq_val[55] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[55] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_56->Position = v;

		if (eq_val[56] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[56] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_57->Position = v;

		if (eq_val[57] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[57] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_58->Position = v;

		if (eq_val[58] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[58] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_59->Position = v;

		if (eq_val[59] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[59] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_60->Position = v;

		if (eq_val[60] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[60] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_61->Position = v;

		if (eq_val[61] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[61] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_62->Position = v;

		if (eq_val[62] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[62] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_63->Position = v;

		if (eq_val[63] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[63] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_64->Position = v;

		if (eq_val[64] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[64] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_65->Position = v;

		if (eq_val[65] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[65] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_66->Position = v;

		if (eq_val[66] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[66] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_67->Position = v;

		if (eq_val[67] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[67] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_68->Position = v;

		if (eq_val[68] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[68] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_69->Position = v;

		if (eq_val[69] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[69] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_70->Position = v;

		if (eq_val[70] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[70] < per1[e + 1]) break;
			}
			v = 14 - e;
		}

		TB_3_71->Position = v;

		if (eq_val[71] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[71] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_72->Position = v;

		if (eq_val[72] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[72] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_73->Position = v;

		if (eq_val[73] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[73] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_74->Position = v;

		if (eq_val[74] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[74] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_75->Position = v;

		if (eq_val[75] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[75] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_76->Position = v;

		if (eq_val[76] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[76] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_77->Position = v;

		if (eq_val[77] < per1[0]) {
			v = 14;
		} else {
			for (e = 0;e + 1 < 29;e++) {
				if (eq_val[77] < per1[e + 1]) break;
			}
			v = 14 - e;
		}
		TB_3_78->Position = v;

	}
	PRINT_FN_LOG("EQ_AutoSetType3","End");
}

void __fastcall TForm1::cmbPlayDeviceChange(TObject *Sender)
{
	AnsiString sStr;
	int dn,n;
	int i;
	int f_44100,f_48000,f_96000;
	int v1,v2,v3,v4,v5,v6;
	f_44100 = f_48000 = f_96000 = 0;
	char name[60];

	PRINT_FN_LOG("cmbPlayDeviceChange","Start");

	chk_44100->Checked = false;
	chk_48000->Checked = false;
	chk_96000->Checked = false;
	rdo_preview_rate_44100->Enabled = false;
	rdo_preview_rate_48000->Enabled = false;
	rdo_preview_rate_96000->Enabled = false;

	sStr = memo_DeviceInfo->Lines->Strings[cmbPlayDevice->ItemIndex];
	n = sscanf(sStr.c_str(),"%d:%[^|]%*c%d,%d,%d,%d,%d,%d",&dn,name,&v1,&v2,&v3,&v4,&v5,&v6);
	if (n > 0) {
		if (v1 || v2) {
			chk_44100->Checked = true;
			rdo_preview_rate_44100->Enabled = true;
		}
		if (v3 || v4) {
			chk_48000->Checked = true;
			rdo_preview_rate_48000->Enabled = true;
		}
		if (v5 || v6) {
			chk_96000->Checked = true;
			rdo_preview_rate_96000->Enabled = true;
		}
	}
	lbl_play_set_device->Visible = false;
	PRINT_FN_LOG("cmbPlayDeviceChange","End");
	cmbPlayDevice->Tag = 1;
	for (i = 0; i < LvFile->Items->Count;i++) {
		LvFile->Items->Item[i]->Selected = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmb_preview_lengthChange(TObject *Sender)
{
	int i,sec;
	PRINT_FN_LOG("cmb_preview_lengthChange","Start");

	if (cmb_preview_length->ItemIndex == 0) sec = 5;
	if (cmb_preview_length->ItemIndex == 1) sec = 10;
	if (cmb_preview_length->ItemIndex == 2) sec = 15;
	if (cmb_preview_length->ItemIndex == 3) sec = 20;
	if (cmb_preview_length->ItemIndex == 4) sec = 25;
	if (cmb_preview_length->ItemIndex == 5) sec = 30;

	// Preview 用の音声の削除とデータの設定
	for (i = 0;i < 16;i++) {
		SoundBuffer[i].sec    = sec;
	}
	if (preview_input_filename != "") preview_input_filename_state_change = 1;

	PRINT_FN_LOG("cmb_preview_lengthChange","End");
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CategoryPanel1Expand(TObject *Sender)
{
	CategoryPanel2->Collapsed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel2Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_log_disableClick(TObject *Sender)
{
	UnicodeString cmdName,cmdNameNew;
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"upconv_no_log.exe";

	cmdNameNew = ExtractFilePath(Application->ExeName);
	cmdNameNew += L"upconv.exe";
	CopyFile(cmdName.c_str(),cmdNameNew.c_str(),FALSE);
	g_log_enable = 0;

	be_log->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_log_enableClick(TObject *Sender)
{
	UnicodeString cmdName,cmdNameNew;
	AnsiString log_path;
	cmdName = ExtractFilePath(Application->ExeName);
	cmdName += L"upconv_log.exe";

	cmdNameNew = ExtractFilePath(Application->ExeName);
	cmdNameNew += L"upconv.exe";
	CopyFile(cmdName.c_str(),cmdNameNew.c_str(),FALSE);

	log_path = be_log->Text;
	strcpy(g_log_filename,log_path.c_str());
	strcat(g_log_filename,"\\upconv_eq_gui.log");
	g_log_enable = 1;
	be_log->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::be_logLeftButtonClick(TObject *Sender)
{
	WideString root = "";
	AnsiString work_str;
	String dir = "";


	if (DirectoryExists(be_log->Text,false)) {
		dir = be_log->Text;
	}

	if (SelectDirectory("EQ", root, dir)) {
		be_log->Text = dir;
		work_str = be_log->Text;
		strcpy(g_log_filename,work_str.c_str());
		strcat(g_log_filename,"\\upconv_eq_gui.log");
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::be_logRightButtonClick(TObject *Sender)
{
	be_log->Text = L"C:\\ProgramData\\upconv_eq";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer4Timer(TObject *Sender)
{
	int i;
	Timer4->Enabled = false;
	if (SelectFileIndex < LvFile->Items->Count) {
		LvFile->Items->Item[SelectFileIndex]->ImageIndex = 2;
		LvFile->Items->Item[SelectFileIndex]->Caption = L"";
	}

	for (i = 0;i < LvFile->Items->Count;i++) {
		if (LvFile->Items->Item[i]->Selected == true) break;
	}
	if (i < LvFile->Items->Count) {
		btnPlay->Enabled = true;
	}
	for (i = 0; i < 1;i++) {
		if (threadExec[i] != NULL) {
			threadExec[i] = NULL;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_window_size1Click(TObject *Sender)
{
	GroupBox1->Width  = 805;
	t1_scrbox->Width  = 773;
	GroupBox10->Width = 805;
	t2_scrbox->Width  = 773;
	GroupBox3->Width  = 805;
	t3_scrbox->Width  = 773;
	GroupBox5->Width  = 805;
	t4_scrbox->Width  = 773;
	Panel1->Width = 833;
	PageEQ->Width = 833;
	Form1->Width = 1247;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_window_size2Click(TObject *Sender)
{
	GroupBox1->Width  = 1073;
	t1_scrbox->Width  = 1050;
	GroupBox10->Width = 1073;
	t2_scrbox->Width  = 1050;
	GroupBox3->Width  = 1073;
	t3_scrbox->Width  = 1050;
	GroupBox5->Width  = 1073;
	t4_scrbox->Width  = 1050;
	Panel1->Width = 1093;
	PageEQ->Width = 1093;
	Form1->Width = 1514;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_window_size3Click(TObject *Sender)
{
	GroupBox1->Width  = 1405;
	t1_scrbox->Width  = 1377;
	GroupBox10->Width = 1405;
	t2_scrbox->Width  = 1377;
	GroupBox3->Width  = 1405;
	t3_scrbox->Width  = 1377;
	GroupBox5->Width  = 1405;
	t4_scrbox->Width  = 1377;
	Panel1->Width = 1425;
	PageEQ->Width = 1864;
	Form1->Width = 1850;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::cmbNormalizeChange(TObject *Sender)
{
	if (cmbNormalize->ItemIndex == 0) {
		cmb_volume_level->Enabled = false;
	} else if (cmbNormalize->ItemIndex == 1) {
		cmb_volume_level->Enabled = false;
	} else if (cmbNormalize->ItemIndex == 2) {
		cmb_volume_level->Enabled = true;
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::TB_4_1Change(TObject *Sender)
{
	eq_4_1->Caption = TB_4_1->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_2Change(TObject *Sender)
{
	eq_4_2->Caption = TB_4_2->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_3Change(TObject *Sender)
{
	eq_4_3->Caption = TB_4_3->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_4Change(TObject *Sender)
{
	eq_4_4->Caption = TB_4_4->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_5Change(TObject *Sender)
{
	eq_4_5->Caption = TB_4_5->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_6Change(TObject *Sender)
{
	eq_4_6->Caption = TB_4_6->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_7Change(TObject *Sender)
{
	eq_4_7->Caption = TB_4_7->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_8Change(TObject *Sender)
{
	eq_4_8->Caption = TB_4_8->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_9Change(TObject *Sender)
{
	eq_4_9->Caption = TB_4_9->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_10Change(TObject *Sender)
{
	eq_4_10->Caption = TB_4_10->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_11Change(TObject *Sender)
{
	eq_4_11->Caption = TB_4_11->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_12Change(TObject *Sender)
{
	eq_4_12->Caption = TB_4_12->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_13Change(TObject *Sender)
{
	eq_4_13->Caption = TB_4_13->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_14Change(TObject *Sender)
{
	eq_4_14->Caption = TB_4_14->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_15Change(TObject *Sender)
{
	eq_4_15->Caption = TB_4_15->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_16Change(TObject *Sender)
{
	eq_4_16->Caption = TB_4_16->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_17Change(TObject *Sender)
{
	eq_4_17->Caption = TB_4_17->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_18Change(TObject *Sender)
{
	eq_4_18->Caption = TB_4_18->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_19Change(TObject *Sender)
{
	eq_4_19->Caption = TB_4_19->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_20Change(TObject *Sender)
{
	eq_4_20->Caption = TB_4_20->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_21Change(TObject *Sender)
{
	eq_4_21->Caption = TB_4_21->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_22Change(TObject *Sender)
{
	eq_4_22->Caption = TB_4_22->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_23Change(TObject *Sender)
{
	eq_4_23->Caption = TB_4_23->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_24Change(TObject *Sender)
{
	eq_4_24->Caption = TB_4_24->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_25Change(TObject *Sender)
{
	eq_4_25->Caption = TB_4_25->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_26Change(TObject *Sender)
{
	eq_4_26->Caption = TB_4_26->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_27Change(TObject *Sender)
{
	eq_4_27->Caption = TB_4_27->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_28Change(TObject *Sender)
{
	eq_4_28->Caption = TB_4_28->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_29Change(TObject *Sender)
{
	eq_4_29->Caption = TB_4_29->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_30Change(TObject *Sender)
{
	eq_4_30->Caption = TB_4_30->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_31Change(TObject *Sender)
{
	eq_4_31->Caption = TB_4_31->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_32Change(TObject *Sender)
{
	eq_4_32->Caption = TB_4_32->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_33Change(TObject *Sender)
{
	eq_4_33->Caption = TB_4_33->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_34Change(TObject *Sender)
{
	eq_4_34->Caption = TB_4_34->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_35Change(TObject *Sender)
{
	eq_4_35->Caption = TB_4_35->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_36Change(TObject *Sender)
{
	eq_4_36->Caption = TB_4_36->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_37Change(TObject *Sender)
{
	eq_4_37->Caption = TB_4_37->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_38Change(TObject *Sender)
{
	eq_4_38->Caption = TB_4_38->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_39Change(TObject *Sender)
{
	eq_4_39->Caption = TB_4_39->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_40Change(TObject *Sender)
{
	eq_4_40->Caption = TB_4_40->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_41Change(TObject *Sender)
{
	eq_4_41->Caption = TB_4_41->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_42Change(TObject *Sender)
{
	eq_4_42->Caption = TB_4_42->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_43Change(TObject *Sender)
{
	eq_4_43->Caption = TB_4_43->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_44Change(TObject *Sender)
{
	eq_4_44->Caption = TB_4_44->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_45Change(TObject *Sender)
{
	eq_4_45->Caption = TB_4_45->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_46Change(TObject *Sender)
{
	eq_4_46->Caption = TB_4_46->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_47Change(TObject *Sender)
{
	eq_4_47->Caption = TB_4_47->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_48Change(TObject *Sender)
{
	eq_4_48->Caption = TB_4_48->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_49Change(TObject *Sender)
{
	eq_4_49->Caption = TB_4_49->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_50Change(TObject *Sender)
{
	eq_4_50->Caption = TB_4_50->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_51Change(TObject *Sender)
{
	eq_4_51->Caption = TB_4_51->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_52Change(TObject *Sender)
{
	eq_4_52->Caption = TB_4_52->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_53Change(TObject *Sender)
{
	eq_4_53->Caption = TB_4_53->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_54Change(TObject *Sender)
{
	eq_4_54->Caption = TB_4_54->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_55Change(TObject *Sender)
{
	eq_4_55->Caption = TB_4_55->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_56Change(TObject *Sender)
{
	eq_4_56->Caption = TB_4_56->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_57Change(TObject *Sender)
{
	eq_4_57->Caption = TB_4_57->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_58Change(TObject *Sender)
{
	eq_4_58->Caption = TB_4_58->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_59Change(TObject *Sender)
{
	eq_4_59->Caption = TB_4_59->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_60Change(TObject *Sender)
{
	eq_4_60->Caption = TB_4_60->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_61Change(TObject *Sender)
{
	eq_4_61->Caption = TB_4_61->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_62Change(TObject *Sender)
{
	eq_4_62->Caption = TB_4_62->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_63Change(TObject *Sender)
{
	eq_4_63->Caption = TB_4_63->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_64Change(TObject *Sender)
{
	eq_4_64->Caption = TB_4_64->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_65Change(TObject *Sender)
{
	eq_4_65->Caption = TB_4_65->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_66Change(TObject *Sender)
{
	eq_4_66->Caption = TB_4_66->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_67Change(TObject *Sender)
{
	eq_4_67->Caption = TB_4_67->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_68Change(TObject *Sender)
{
	eq_4_68->Caption = TB_4_68->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_69Change(TObject *Sender)
{
	eq_4_69->Caption = TB_4_69->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_70Change(TObject *Sender)
{
	eq_4_70->Caption = TB_4_70->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_71Change(TObject *Sender)
{
	eq_4_71->Caption = TB_4_71->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_72Change(TObject *Sender)
{
	eq_4_72->Caption = TB_4_72->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_73Change(TObject *Sender)
{
	eq_4_73->Caption = TB_4_73->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_74Change(TObject *Sender)
{
	eq_4_74->Caption = TB_4_74->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_75Change(TObject *Sender)
{
	eq_4_75->Caption = TB_4_75->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_76Change(TObject *Sender)
{
	eq_4_76->Caption = TB_4_76->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_77Change(TObject *Sender)
{
	eq_4_77->Caption = TB_4_77->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_78Change(TObject *Sender)
{
	eq_4_78->Caption = TB_4_78->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_79Change(TObject *Sender)
{
	eq_4_79->Caption = TB_4_79->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_80Change(TObject *Sender)
{
	eq_4_80->Caption = TB_4_80->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_81Change(TObject *Sender)
{
	eq_4_81->Caption = TB_4_81->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_82Change(TObject *Sender)
{
	eq_4_82->Caption = TB_4_82->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_83Change(TObject *Sender)
{
	eq_4_83->Caption = TB_4_83->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_84Change(TObject *Sender)
{
	eq_4_84->Caption = TB_4_84->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_85Change(TObject *Sender)
{
	eq_4_85->Caption = TB_4_85->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_86Change(TObject *Sender)
{
	eq_4_86->Caption = TB_4_86->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_87Change(TObject *Sender)
{
	eq_4_87->Caption = TB_4_87->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_88Change(TObject *Sender)
{
	eq_4_88->Caption = TB_4_88->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_89Change(TObject *Sender)
{
	eq_4_89->Caption = TB_4_89->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_90Change(TObject *Sender)
{
	eq_4_90->Caption = TB_4_90->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_91Change(TObject *Sender)
{
	eq_4_91->Caption = TB_4_91->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_92Change(TObject *Sender)
{
	eq_4_92->Caption = TB_4_92->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_93Change(TObject *Sender)
{
	eq_4_93->Caption = TB_4_93->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_94Change(TObject *Sender)
{
	eq_4_94->Caption = TB_4_94->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_95Change(TObject *Sender)
{
	eq_4_95->Caption = TB_4_95->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_96Change(TObject *Sender)
{
	eq_4_96->Caption = TB_4_96->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_97Change(TObject *Sender)
{
	eq_4_97->Caption = TB_4_97->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_98Change(TObject *Sender)
{
	eq_4_98->Caption = TB_4_98->Position * -1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	TB_4_1_Cutoff->Checked = false;
	TB_4_2_Cutoff->Checked = false;
	TB_4_3_Cutoff->Checked = false;
	TB_4_4_Cutoff->Checked = false;
	TB_4_5_Cutoff->Checked = false;
	TB_4_6_Cutoff->Checked = false;
	TB_4_7_Cutoff->Checked = false;
	TB_4_8_Cutoff->Checked = false;
	TB_4_9_Cutoff->Checked = false;
	TB_4_10_Cutoff->Checked = false;
	TB_4_11_Cutoff->Checked = false;
	TB_4_12_Cutoff->Checked = false;
	TB_4_13_Cutoff->Checked = false;
	TB_4_36_Cutoff->Checked = false;
	TB_4_37_Cutoff->Checked = false;
	TB_4_38_Cutoff->Checked = false;
	TB_4_39_Cutoff->Checked = false;
	TB_4_40_Cutoff->Checked = false;
	TB_4_41_Cutoff->Checked = false;
	TB_4_42_Cutoff->Checked = false;
	TB_4_43_Cutoff->Checked = false;
	TB_4_44_Cutoff->Checked = false;
	TB_4_45_Cutoff->Checked = false;
	TB_4_46_Cutoff->Checked = false;
	TB_4_47_Cutoff->Checked = false;
	TB_4_48_Cutoff->Checked = false;
	TB_4_49_Cutoff->Checked = false;
	TB_4_50_Cutoff->Checked = false;
	TB_4_51_Cutoff->Checked = false;
	TB_4_52_Cutoff->Checked = false;
	TB_4_53_Cutoff->Checked = false;
	TB_4_54_Cutoff->Checked = false;
	TB_4_55_Cutoff->Checked = false;
	TB_4_56_Cutoff->Checked = false;
	TB_4_57_Cutoff->Checked = false;
	TB_4_58_Cutoff->Checked = false;
	TB_4_59_Cutoff->Checked = false;
	TB_4_60_Cutoff->Checked = false;
	TB_4_61_Cutoff->Checked = false;
	TB_4_62_Cutoff->Checked = false;
	TB_4_63_Cutoff->Checked = false;
	TB_4_64_Cutoff->Checked = false;
	TB_4_65_Cutoff->Checked = false;
	TB_4_66_Cutoff->Checked = false;
	TB_4_67_Cutoff->Checked = false;
	TB_4_68_Cutoff->Checked = false;
	TB_4_69_Cutoff->Checked = false;
	TB_4_70_Cutoff->Checked = false;
	TB_4_71_Cutoff->Checked = false;
	TB_4_72_Cutoff->Checked = false;
	TB_4_73_Cutoff->Checked = false;
	TB_4_74_Cutoff->Checked = false;
	TB_4_75_Cutoff->Checked = false;
	TB_4_76_Cutoff->Checked = false;
	TB_4_77_Cutoff->Checked = false;
	TB_4_78_Cutoff->Checked = false;
	TB_4_79_Cutoff->Checked = false;
	TB_4_80_Cutoff->Checked = false;
	TB_4_81_Cutoff->Checked = false;
	TB_4_82_Cutoff->Checked = false;
	TB_4_83_Cutoff->Checked = false;
	TB_4_84_Cutoff->Checked = false;
	TB_4_85_Cutoff->Checked = false;
	TB_4_86_Cutoff->Checked = false;
	TB_4_87_Cutoff->Checked = false;
	TB_4_88_Cutoff->Checked = false;
	TB_4_89_Cutoff->Checked = false;
	TB_4_90_Cutoff->Checked = false;
	TB_4_91_Cutoff->Checked = false;
	TB_4_92_Cutoff->Checked = false;
	TB_4_93_Cutoff->Checked = false;
	TB_4_94_Cutoff->Checked = false;
	TB_4_95_Cutoff->Checked = false;
	TB_4_96_Cutoff->Checked = false;
	TB_4_97_Cutoff->Checked = false;
	TB_4_98_Cutoff->Checked = false;

	TB_4_1->Position = 0;
	TB_4_2->Position = 0;
	TB_4_3->Position = 0;
	TB_4_4->Position = 0;
	TB_4_5->Position = 0;
	TB_4_6->Position = 0;
	TB_4_7->Position = 0;
	TB_4_8->Position = 0;
	TB_4_9->Position = 0;
	TB_4_10->Position = 0;
	TB_4_11->Position = 0;
	TB_4_12->Position = 0;
	TB_4_13->Position = 0;
	TB_4_14->Position = 0;
	TB_4_15->Position = 0;
	TB_4_16->Position = 0;
	TB_4_17->Position = 0;
	TB_4_18->Position = 0;
	TB_4_19->Position = 0;
	TB_4_20->Position = 0;
	TB_4_21->Position = 0;
	TB_4_22->Position = 0;
	TB_4_23->Position = 0;
	TB_4_24->Position = 0;
	TB_4_25->Position = 0;
	TB_4_26->Position = 0;
	TB_4_27->Position = 0;
	TB_4_28->Position = 0;
	TB_4_29->Position = 0;
	TB_4_30->Position = 0;
	TB_4_31->Position = 0;
	TB_4_32->Position = 0;
	TB_4_33->Position = 0;
	TB_4_34->Position = 0;
	TB_4_35->Position = 0;
	TB_4_36->Position = 0;
	TB_4_37->Position = 0;
	TB_4_38->Position = 0;
	TB_4_39->Position = 0;
	TB_4_40->Position = 0;
	TB_4_41->Position = 0;
	TB_4_42->Position = 0;
	TB_4_43->Position = 0;
	TB_4_44->Position = 0;
	TB_4_45->Position = 0;
	TB_4_46->Position = 0;
	TB_4_47->Position = 0;
	TB_4_48->Position = 0;
	TB_4_49->Position = 0;
	TB_4_50->Position = 0;
	TB_4_51->Position = 0;
	TB_4_52->Position = 0;
	TB_4_53->Position = 0;
	TB_4_54->Position = 0;
	TB_4_55->Position = 0;
	TB_4_56->Position = 0;
	TB_4_57->Position = 0;
	TB_4_58->Position = 0;
	TB_4_59->Position = 0;
	TB_4_60->Position = 0;
	TB_4_61->Position = 0;
	TB_4_62->Position = 0;
	TB_4_63->Position = 0;
	TB_4_64->Position = 0;
	TB_4_65->Position = 0;
	TB_4_66->Position = 0;
	TB_4_67->Position = 0;
	TB_4_68->Position = 0;
	TB_4_69->Position = 0;
	TB_4_70->Position = 0;
	TB_4_71->Position = 0;
	TB_4_72->Position = 0;
	TB_4_73->Position = 0;
	TB_4_74->Position = 0;
	TB_4_75->Position = 0;
	TB_4_76->Position = 0;
	TB_4_77->Position = 0;
	TB_4_78->Position = 0;
	TB_4_79->Position = 0;
	TB_4_80->Position = 0;
	TB_4_81->Position = 0;
	TB_4_82->Position = 0;
	TB_4_83->Position = 0;
	TB_4_84->Position = 0;
	TB_4_85->Position = 0;
	TB_4_86->Position = 0;
	TB_4_87->Position = 0;
	TB_4_88->Position = 0;
	TB_4_89->Position = 0;
	TB_4_90->Position = 0;
	TB_4_91->Position = 0;
	TB_4_92->Position = 0;
	TB_4_93->Position = 0;
	TB_4_94->Position = 0;
	TB_4_95->Position = 0;
	TB_4_96->Position = 0;
	TB_4_97->Position = 0;
	TB_4_98->Position = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type4_all_upClick(TObject *Sender)
{
	int range_from,range_to;
	int cutoff_flag;

	range_from = -1;
	range_to   = 999;
	if (cmbT4_range_from->ItemIndex > 0 && cmbT4_range_to->ItemIndex > 0) {
		if (cmbT4_range_from->ItemIndex <= cmbT4_range_to->ItemIndex) {
			range_from = cmbT4_range_from->ItemIndex;
			range_to   = cmbT4_range_to->ItemIndex;
		} else {
			range_to = cmbT4_range_to->ItemIndex;
			range_from   = cmbT4_range_from->ItemIndex;
		}
	} else {
		if (cmbT4_range_from->ItemIndex > 0) {
			range_from = cmbT4_range_from->ItemIndex;
		}
		if (cmbT4_range_to->ItemIndex > 0) {
			range_to = cmbT4_range_to->ItemIndex;
		}
	}

	cutoff_flag = 0;
	if (range_from <= 1  && range_to >= 1 && TB_4_1_Cutoff->Checked == true) {TB_4_1->Tag = 14;TB_4_1_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 2  && range_to >= 2 && TB_4_2_Cutoff->Checked == true) {TB_4_2->Tag = 14;TB_4_2_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 3  && range_to >= 3 && TB_4_3_Cutoff->Checked == true) {TB_4_3->Tag = 14;TB_4_3_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 4  && range_to >= 4 && TB_4_4_Cutoff->Checked == true) {TB_4_4->Tag = 14;TB_4_4_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 5  && range_to >= 5 && TB_4_5_Cutoff->Checked == true) {TB_4_5->Tag = 14;TB_4_5_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 6  && range_to >= 6 && TB_4_6_Cutoff->Checked == true) {TB_4_6->Tag = 14;TB_4_6_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 7  && range_to >= 7 && TB_4_7_Cutoff->Checked == true) {TB_4_7->Tag = 14;TB_4_7_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 8  && range_to >= 8 && TB_4_8_Cutoff->Checked == true) {TB_4_8->Tag = 14;TB_4_8_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 9  && range_to >= 9 && TB_4_9_Cutoff->Checked == true) {TB_4_9->Tag = 14;TB_4_9_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 10  && range_to >= 10 && TB_4_10_Cutoff->Checked == true) {TB_4_10->Tag = 14;TB_4_10_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 11  && range_to >= 11 && TB_4_11_Cutoff->Checked == true) {TB_4_11->Tag = 14;TB_4_11_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 12  && range_to >= 12 && TB_4_12_Cutoff->Checked == true) {TB_4_12->Tag = 14;TB_4_12_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 13  && range_to >= 13 && TB_4_13_Cutoff->Checked == true) {TB_4_13->Tag = 14;TB_4_13_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 36  && range_to >= 36 && TB_4_36_Cutoff->Checked == true) {TB_4_36->Tag = 14;TB_4_36_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 37  && range_to >= 37 && TB_4_37_Cutoff->Checked == true) {TB_4_37->Tag = 14;TB_4_37_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 38  && range_to >= 38 && TB_4_38_Cutoff->Checked == true) {TB_4_38->Tag = 14;TB_4_38_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 39  && range_to >= 39 && TB_4_39_Cutoff->Checked == true) {TB_4_39->Tag = 14;TB_4_39_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 40  && range_to >= 40 && TB_4_40_Cutoff->Checked == true) {TB_4_40->Tag = 14;TB_4_40_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 41  && range_to >= 41 && TB_4_41_Cutoff->Checked == true) {TB_4_41->Tag = 14;TB_4_41_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 42  && range_to >= 42 && TB_4_42_Cutoff->Checked == true) {TB_4_42->Tag = 14;TB_4_42_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 43  && range_to >= 43 && TB_4_43_Cutoff->Checked == true) {TB_4_43->Tag = 14;TB_4_43_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 44  && range_to >= 44 && TB_4_44_Cutoff->Checked == true) {TB_4_44->Tag = 14;TB_4_44_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 45  && range_to >= 45 && TB_4_45_Cutoff->Checked == true) {TB_4_45->Tag = 14;TB_4_45_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 46  && range_to >= 46 && TB_4_46_Cutoff->Checked == true) {TB_4_46->Tag = 14;TB_4_46_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 47  && range_to >= 47 && TB_4_47_Cutoff->Checked == true) {TB_4_47->Tag = 14;TB_4_47_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 48  && range_to >= 48 && TB_4_48_Cutoff->Checked == true) {TB_4_48->Tag = 14;TB_4_48_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 49  && range_to >= 49 && TB_4_49_Cutoff->Checked == true) {TB_4_49->Tag = 14;TB_4_49_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 50  && range_to >= 50 && TB_4_50_Cutoff->Checked == true) {TB_4_50->Tag = 14;TB_4_50_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 51  && range_to >= 51 && TB_4_51_Cutoff->Checked == true) {TB_4_51->Tag = 14;TB_4_51_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 52  && range_to >= 52 && TB_4_52_Cutoff->Checked == true) {TB_4_52->Tag = 14;TB_4_52_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 53  && range_to >= 53 && TB_4_53_Cutoff->Checked == true) {TB_4_53->Tag = 14;TB_4_53_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 54  && range_to >= 54 && TB_4_54_Cutoff->Checked == true) {TB_4_54->Tag = 14;TB_4_54_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 55  && range_to >= 55 && TB_4_55_Cutoff->Checked == true) {TB_4_55->Tag = 14;TB_4_55_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 56  && range_to >= 56 && TB_4_56_Cutoff->Checked == true) {TB_4_56->Tag = 14;TB_4_56_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 57  && range_to >= 57 && TB_4_57_Cutoff->Checked == true) {TB_4_57->Tag = 14;TB_4_57_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 58  && range_to >= 58 && TB_4_58_Cutoff->Checked == true) {TB_4_58->Tag = 14;TB_4_58_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 59  && range_to >= 59 && TB_4_59_Cutoff->Checked == true) {TB_4_59->Tag = 14;TB_4_59_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 60  && range_to >= 60 && TB_4_60_Cutoff->Checked == true) {TB_4_60->Tag = 14;TB_4_60_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 61  && range_to >= 61 && TB_4_61_Cutoff->Checked == true) {TB_4_61->Tag = 14;TB_4_61_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 62  && range_to >= 62 && TB_4_62_Cutoff->Checked == true) {TB_4_62->Tag = 14;TB_4_62_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 63  && range_to >= 63 && TB_4_63_Cutoff->Checked == true) {TB_4_63->Tag = 14;TB_4_63_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 64  && range_to >= 64 && TB_4_64_Cutoff->Checked == true) {TB_4_64->Tag = 14;TB_4_64_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 65  && range_to >= 65 && TB_4_65_Cutoff->Checked == true) {TB_4_65->Tag = 14;TB_4_65_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 66  && range_to >= 66 && TB_4_66_Cutoff->Checked == true) {TB_4_66->Tag = 14;TB_4_66_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 67  && range_to >= 67 && TB_4_67_Cutoff->Checked == true) {TB_4_67->Tag = 14;TB_4_67_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 68  && range_to >= 68 && TB_4_68_Cutoff->Checked == true) {TB_4_68->Tag = 14;TB_4_68_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 69  && range_to >= 69 && TB_4_69_Cutoff->Checked == true) {TB_4_69->Tag = 14;TB_4_69_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 70  && range_to >= 70 && TB_4_70_Cutoff->Checked == true) {TB_4_70->Tag = 14;TB_4_70_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 71  && range_to >= 71 && TB_4_71_Cutoff->Checked == true) {TB_4_71->Tag = 14;TB_4_71_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 72  && range_to >= 72 && TB_4_72_Cutoff->Checked == true) {TB_4_72->Tag = 14;TB_4_72_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 73  && range_to >= 73 && TB_4_73_Cutoff->Checked == true) {TB_4_73->Tag = 14;TB_4_73_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 74  && range_to >= 74 && TB_4_74_Cutoff->Checked == true) {TB_4_74->Tag = 14;TB_4_74_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 75  && range_to >= 75 && TB_4_75_Cutoff->Checked == true) {TB_4_75->Tag = 14;TB_4_75_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 76  && range_to >= 76 && TB_4_76_Cutoff->Checked == true) {TB_4_76->Tag = 14;TB_4_76_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 77  && range_to >= 77 && TB_4_77_Cutoff->Checked == true) {TB_4_77->Tag = 14;TB_4_77_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 78  && range_to >= 78 && TB_4_78_Cutoff->Checked == true) {TB_4_78->Tag = 14;TB_4_78_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 79  && range_to >= 79 && TB_4_79_Cutoff->Checked == true) {TB_4_79->Tag = 14;TB_4_79_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 80  && range_to >= 80 && TB_4_80_Cutoff->Checked == true) {TB_4_80->Tag = 14;TB_4_80_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 81  && range_to >= 81 && TB_4_81_Cutoff->Checked == true) {TB_4_81->Tag = 14;TB_4_81_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 82  && range_to >= 82 && TB_4_82_Cutoff->Checked == true) {TB_4_82->Tag = 14;TB_4_82_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 83  && range_to >= 83 && TB_4_83_Cutoff->Checked == true) {TB_4_83->Tag = 14;TB_4_83_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 84  && range_to >= 84 && TB_4_84_Cutoff->Checked == true) {TB_4_84->Tag = 14;TB_4_84_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 85  && range_to >= 85 && TB_4_85_Cutoff->Checked == true) {TB_4_85->Tag = 14;TB_4_85_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 86  && range_to >= 86 && TB_4_86_Cutoff->Checked == true) {TB_4_86->Tag = 14;TB_4_86_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 87  && range_to >= 87 && TB_4_87_Cutoff->Checked == true) {TB_4_87->Tag = 14;TB_4_87_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 88  && range_to >= 88 && TB_4_88_Cutoff->Checked == true) {TB_4_88->Tag = 14;TB_4_88_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 89  && range_to >= 89 && TB_4_89_Cutoff->Checked == true) {TB_4_89->Tag = 14;TB_4_89_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 90  && range_to >= 90 && TB_4_90_Cutoff->Checked == true) {TB_4_90->Tag = 14;TB_4_90_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 91  && range_to >= 91 && TB_4_91_Cutoff->Checked == true) {TB_4_91->Tag = 14;TB_4_91_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 92  && range_to >= 92 && TB_4_92_Cutoff->Checked == true) {TB_4_92->Tag = 14;TB_4_92_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 93  && range_to >= 93 && TB_4_93_Cutoff->Checked == true) {TB_4_93->Tag = 14;TB_4_93_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 94  && range_to >= 94 && TB_4_94_Cutoff->Checked == true) {TB_4_94->Tag = 14;TB_4_94_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 95  && range_to >= 95 && TB_4_95_Cutoff->Checked == true) {TB_4_95->Tag = 14;TB_4_95_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 96  && range_to >= 96 && TB_4_96_Cutoff->Checked == true) {TB_4_96->Tag = 14;TB_4_96_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 97  && range_to >= 97 && TB_4_97_Cutoff->Checked == true) {TB_4_97->Tag = 14;TB_4_97_Cutoff->Checked = false;cutoff_flag = 1;}
	if (range_from <= 98  && range_to >= 98 && TB_4_98_Cutoff->Checked == true) {TB_4_98->Tag = 14;TB_4_98_Cutoff->Checked = false;cutoff_flag = 1;}
	if (cutoff_flag == 1) {
		return;
	}

	if (range_from <= 1  && range_to >= 1) 		TB_4_1->Position = TB_4_1->Position -1;
	if (range_from <= 2  && range_to >= 2) 		TB_4_2->Position = TB_4_2->Position -1;
	if (range_from <= 3  && range_to >= 3) 		TB_4_3->Position = TB_4_3->Position -1;
	if (range_from <= 4  && range_to >= 4) 		TB_4_4->Position = TB_4_4->Position -1;
	if (range_from <= 5  && range_to >= 5) 		TB_4_5->Position = TB_4_5->Position -1;
	if (range_from <= 6  && range_to >= 6) 		TB_4_6->Position = TB_4_6->Position -1;
	if (range_from <= 7  && range_to >= 7) 		TB_4_7->Position = TB_4_7->Position -1;
	if (range_from <= 8  && range_to >= 8) 		TB_4_8->Position = TB_4_8->Position -1;
	if (range_from <= 9  && range_to >= 9) 		TB_4_9->Position = TB_4_9->Position -1;
	if (range_from <= 10  && range_to >= 10)	TB_4_10->Position = TB_4_10->Position -1;
	if (range_from <= 11  && range_to >= 11)	TB_4_11->Position = TB_4_11->Position -1;
	if (range_from <= 12  && range_to >= 12)	TB_4_12->Position = TB_4_12->Position -1;
	if (range_from <= 13  && range_to >= 13)	TB_4_13->Position = TB_4_13->Position -1;
	if (range_from <= 14  && range_to >= 14)	TB_4_14->Position = TB_4_14->Position -1;
	if (range_from <= 15  && range_to >= 15)	TB_4_15->Position = TB_4_15->Position -1;
	if (range_from <= 16  && range_to >= 16)	TB_4_16->Position = TB_4_16->Position -1;
	if (range_from <= 17  && range_to >= 17)	TB_4_17->Position = TB_4_17->Position -1;
	if (range_from <= 18  && range_to >= 18)	TB_4_18->Position = TB_4_18->Position -1;
	if (range_from <= 19  && range_to >= 19)	TB_4_19->Position = TB_4_19->Position -1;
	if (range_from <= 20  && range_to >= 20)	TB_4_20->Position = TB_4_20->Position -1;
	if (range_from <= 21  && range_to >= 21)	TB_4_21->Position = TB_4_21->Position -1;
	if (range_from <= 22  && range_to >= 22)	TB_4_22->Position = TB_4_22->Position -1;
	if (range_from <= 23  && range_to >= 23)	TB_4_23->Position = TB_4_23->Position -1;
	if (range_from <= 24  && range_to >= 24)	TB_4_24->Position = TB_4_24->Position -1;
	if (range_from <= 25  && range_to >= 25)	TB_4_25->Position = TB_4_25->Position -1;
	if (range_from <= 26  && range_to >= 26)	TB_4_26->Position = TB_4_26->Position -1;
	if (range_from <= 27  && range_to >= 27)	TB_4_27->Position = TB_4_27->Position -1;
	if (range_from <= 28  && range_to >= 28)	TB_4_28->Position = TB_4_28->Position -1;
	if (range_from <= 29  && range_to >= 29)	TB_4_29->Position = TB_4_29->Position -1;
	if (range_from <= 30  && range_to >= 30)	TB_4_30->Position = TB_4_30->Position -1;
	if (range_from <= 31  && range_to >= 31)	TB_4_31->Position = TB_4_31->Position -1;
	if (range_from <= 32  && range_to >= 32)	TB_4_32->Position = TB_4_32->Position -1;
	if (range_from <= 33  && range_to >= 33)	TB_4_33->Position = TB_4_33->Position -1;
	if (range_from <= 34  && range_to >= 34)	TB_4_34->Position = TB_4_34->Position -1;
	if (range_from <= 35  && range_to >= 35)	TB_4_35->Position = TB_4_35->Position -1;
	if (range_from <= 36  && range_to >= 36)	TB_4_36->Position = TB_4_36->Position -1;
	if (range_from <= 37  && range_to >= 37)	TB_4_37->Position = TB_4_37->Position -1;
	if (range_from <= 38  && range_to >= 38)	TB_4_38->Position = TB_4_38->Position -1;
	if (range_from <= 39  && range_to >= 39)	TB_4_39->Position = TB_4_39->Position -1;
	if (range_from <= 40  && range_to >= 40)	TB_4_40->Position = TB_4_40->Position -1;
	if (range_from <= 41  && range_to >= 41)	TB_4_41->Position = TB_4_41->Position -1;
	if (range_from <= 42  && range_to >= 42)	TB_4_42->Position = TB_4_42->Position -1;
	if (range_from <= 43  && range_to >= 43)	TB_4_43->Position = TB_4_43->Position -1;
	if (range_from <= 44  && range_to >= 44)	TB_4_44->Position = TB_4_44->Position -1;
	if (range_from <= 45  && range_to >= 45)	TB_4_45->Position = TB_4_45->Position -1;
	if (range_from <= 46  && range_to >= 46)	TB_4_46->Position = TB_4_46->Position -1;
	if (range_from <= 47  && range_to >= 47)	TB_4_47->Position = TB_4_47->Position -1;
	if (range_from <= 48  && range_to >= 48)	TB_4_48->Position = TB_4_48->Position -1;
	if (range_from <= 49  && range_to >= 49)	TB_4_49->Position = TB_4_49->Position -1;
	if (range_from <= 50  && range_to >= 50)	TB_4_50->Position = TB_4_50->Position -1;
	if (range_from <= 51  && range_to >= 51)	TB_4_51->Position = TB_4_51->Position -1;
	if (range_from <= 52  && range_to >= 52)	TB_4_52->Position = TB_4_52->Position -1;
	if (range_from <= 53  && range_to >= 53)	TB_4_53->Position = TB_4_53->Position -1;
	if (range_from <= 54  && range_to >= 54)	TB_4_54->Position = TB_4_54->Position -1;
	if (range_from <= 55  && range_to >= 55)	TB_4_55->Position = TB_4_55->Position -1;
	if (range_from <= 56  && range_to >= 56)	TB_4_56->Position = TB_4_56->Position -1;
	if (range_from <= 57  && range_to >= 57)	TB_4_57->Position = TB_4_57->Position -1;
	if (range_from <= 58  && range_to >= 58)	TB_4_58->Position = TB_4_58->Position -1;
	if (range_from <= 59  && range_to >= 59)	TB_4_59->Position = TB_4_59->Position -1;
	if (range_from <= 60  && range_to >= 60)	TB_4_60->Position = TB_4_60->Position -1;
	if (range_from <= 61  && range_to >= 61)	TB_4_61->Position = TB_4_61->Position -1;
	if (range_from <= 62  && range_to >= 62)	TB_4_62->Position = TB_4_62->Position -1;
	if (range_from <= 63  && range_to >= 63)	TB_4_63->Position = TB_4_63->Position -1;
	if (range_from <= 64  && range_to >= 64)	TB_4_64->Position = TB_4_64->Position -1;
	if (range_from <= 65  && range_to >= 65)	TB_4_65->Position = TB_4_65->Position -1;
	if (range_from <= 66  && range_to >= 66)	TB_4_66->Position = TB_4_66->Position -1;
	if (range_from <= 67  && range_to >= 67)	TB_4_67->Position = TB_4_67->Position -1;
	if (range_from <= 68  && range_to >= 68)	TB_4_68->Position = TB_4_68->Position -1;
	if (range_from <= 69  && range_to >= 69)	TB_4_69->Position = TB_4_69->Position -1;
	if (range_from <= 70  && range_to >= 70)	TB_4_70->Position = TB_4_70->Position -1;
	if (range_from <= 71  && range_to >= 71)	TB_4_71->Position = TB_4_71->Position -1;
	if (range_from <= 72  && range_to >= 72)	TB_4_72->Position = TB_4_72->Position -1;
	if (range_from <= 73  && range_to >= 73)	TB_4_73->Position = TB_4_73->Position -1;
	if (range_from <= 74  && range_to >= 74)	TB_4_74->Position = TB_4_74->Position -1;
	if (range_from <= 75  && range_to >= 75)	TB_4_75->Position = TB_4_75->Position -1;
	if (range_from <= 76  && range_to >= 76)	TB_4_76->Position = TB_4_76->Position -1;
	if (range_from <= 77  && range_to >= 77)	TB_4_77->Position = TB_4_77->Position -1;
	if (range_from <= 78  && range_to >= 78)	TB_4_78->Position = TB_4_78->Position -1;
	if (range_from <= 79  && range_to >= 79)	TB_4_79->Position = TB_4_79->Position -1;
	if (range_from <= 80  && range_to >= 80)	TB_4_80->Position = TB_4_80->Position -1;
	if (range_from <= 81  && range_to >= 81)	TB_4_81->Position = TB_4_81->Position -1;
	if (range_from <= 82  && range_to >= 82)	TB_4_82->Position = TB_4_82->Position -1;
	if (range_from <= 83  && range_to >= 83)	TB_4_83->Position = TB_4_83->Position -1;
	if (range_from <= 84  && range_to >= 84)	TB_4_84->Position = TB_4_84->Position -1;
	if (range_from <= 85  && range_to >= 85)	TB_4_85->Position = TB_4_85->Position -1;
	if (range_from <= 86  && range_to >= 86)	TB_4_86->Position = TB_4_86->Position -1;
	if (range_from <= 87  && range_to >= 87)	TB_4_87->Position = TB_4_87->Position -1;
	if (range_from <= 88  && range_to >= 88)	TB_4_88->Position = TB_4_88->Position -1;
	if (range_from <= 89  && range_to >= 89)	TB_4_89->Position = TB_4_89->Position -1;
	if (range_from <= 90  && range_to >= 90)	TB_4_90->Position = TB_4_90->Position -1;
	if (range_from <= 91  && range_to >= 91)	TB_4_91->Position = TB_4_91->Position -1;
	if (range_from <= 92  && range_to >= 92)	TB_4_92->Position = TB_4_92->Position -1;
	if (range_from <= 93  && range_to >= 93)	TB_4_93->Position = TB_4_93->Position -1;
	if (range_from <= 94  && range_to >= 94)	TB_4_94->Position = TB_4_94->Position -1;
	if (range_from <= 95  && range_to >= 95)	TB_4_95->Position = TB_4_95->Position -1;
	if (range_from <= 96  && range_to >= 96)	TB_4_96->Position = TB_4_96->Position -1;
	if (range_from <= 97  && range_to >= 97)	TB_4_97->Position = TB_4_97->Position -1;
	if (range_from <= 98  && range_to >= 98)	TB_4_98->Position = TB_4_98->Position -1;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::btn_type4_all_downClick(TObject *Sender)
{
	int range_from,range_to;
	range_from = -1;
	range_to   = 999;
	if (cmbT4_range_from->ItemIndex > 0 && cmbT4_range_to->ItemIndex > 0) {
		if (cmbT4_range_from->ItemIndex <= cmbT4_range_to->ItemIndex) {
			range_from = cmbT4_range_from->ItemIndex;
			range_to   = cmbT4_range_to->ItemIndex;
		} else {
			range_to = cmbT4_range_to->ItemIndex;
			range_from   = cmbT4_range_from->ItemIndex;
		}
	} else {
		if (cmbT4_range_from->ItemIndex > 0) {
			range_from = cmbT4_range_from->ItemIndex;
		}
		if (cmbT4_range_to->ItemIndex > 0) {
			range_to = cmbT4_range_to->ItemIndex;
		}
	}

	if (range_from <= 1  && range_to >= 1 && TB_4_1->Position == 14) TB_4_1_Cutoff->Checked = true;
	if (range_from <= 2  && range_to >= 2 && TB_4_2->Position == 14) TB_4_2_Cutoff->Checked = true;
	if (range_from <= 3  && range_to >= 3 && TB_4_3->Position == 14) TB_4_3_Cutoff->Checked = true;
	if (range_from <= 4  && range_to >= 4 && TB_4_4->Position == 14) TB_4_4_Cutoff->Checked = true;
	if (range_from <= 5  && range_to >= 5 && TB_4_5->Position == 14) TB_4_5_Cutoff->Checked = true;
	if (range_from <= 6  && range_to >= 6 && TB_4_6->Position == 14) TB_4_6_Cutoff->Checked = true;
	if (range_from <= 7  && range_to >= 7 && TB_4_7->Position == 14) TB_4_7_Cutoff->Checked = true;
	if (range_from <= 8  && range_to >= 8 && TB_4_8->Position == 14) TB_4_8_Cutoff->Checked = true;
	if (range_from <= 9  && range_to >= 9 && TB_4_9->Position == 14) TB_4_9_Cutoff->Checked = true;
	if (range_from <= 10  && range_to >= 10 && TB_4_10->Position == 14) TB_4_10_Cutoff->Checked = true;
	if (range_from <= 11  && range_to >= 11 && TB_4_11->Position == 14) TB_4_11_Cutoff->Checked = true;
	if (range_from <= 12  && range_to >= 12 && TB_4_12->Position == 14) TB_4_12_Cutoff->Checked = true;
	if (range_from <= 13  && range_to >= 13 && TB_4_13->Position == 14) TB_4_13_Cutoff->Checked = true;
	if (range_from <= 36  && range_to >= 36 && TB_4_36->Position == 14) TB_4_36_Cutoff->Checked = true;
	if (range_from <= 37  && range_to >= 37 && TB_4_37->Position == 14) TB_4_37_Cutoff->Checked = true;
	if (range_from <= 38  && range_to >= 38 && TB_4_38->Position == 14) TB_4_38_Cutoff->Checked = true;
	if (range_from <= 39  && range_to >= 39 && TB_4_39->Position == 14) TB_4_39_Cutoff->Checked = true;
	if (range_from <= 40  && range_to >= 40 && TB_4_40->Position == 14) TB_4_40_Cutoff->Checked = true;
	if (range_from <= 41  && range_to >= 41 && TB_4_41->Position == 14) TB_4_41_Cutoff->Checked = true;
	if (range_from <= 42  && range_to >= 42 && TB_4_42->Position == 14) TB_4_42_Cutoff->Checked = true;
	if (range_from <= 43  && range_to >= 43 && TB_4_43->Position == 14) TB_4_43_Cutoff->Checked = true;
	if (range_from <= 44  && range_to >= 44 && TB_4_44->Position == 14) TB_4_44_Cutoff->Checked = true;
	if (range_from <= 45  && range_to >= 45 && TB_4_45->Position == 14) TB_4_45_Cutoff->Checked = true;
	if (range_from <= 46  && range_to >= 46 && TB_4_46->Position == 14) TB_4_46_Cutoff->Checked = true;
	if (range_from <= 47  && range_to >= 47 && TB_4_47->Position == 14) TB_4_47_Cutoff->Checked = true;
	if (range_from <= 48  && range_to >= 48 && TB_4_48->Position == 14) TB_4_48_Cutoff->Checked = true;
	if (range_from <= 49  && range_to >= 49 && TB_4_49->Position == 14) TB_4_49_Cutoff->Checked = true;
	if (range_from <= 50  && range_to >= 50 && TB_4_50->Position == 14) TB_4_50_Cutoff->Checked = true;
	if (range_from <= 51  && range_to >= 51 && TB_4_51->Position == 14) TB_4_51_Cutoff->Checked = true;
	if (range_from <= 52  && range_to >= 52 && TB_4_52->Position == 14) TB_4_52_Cutoff->Checked = true;
	if (range_from <= 53  && range_to >= 53 && TB_4_53->Position == 14) TB_4_53_Cutoff->Checked = true;
	if (range_from <= 54  && range_to >= 54 && TB_4_54->Position == 14) TB_4_54_Cutoff->Checked = true;
	if (range_from <= 55  && range_to >= 55 && TB_4_55->Position == 14) TB_4_55_Cutoff->Checked = true;
	if (range_from <= 56  && range_to >= 56 && TB_4_56->Position == 14) TB_4_56_Cutoff->Checked = true;
	if (range_from <= 57  && range_to >= 57 && TB_4_57->Position == 14) TB_4_57_Cutoff->Checked = true;
	if (range_from <= 58  && range_to >= 58 && TB_4_58->Position == 14) TB_4_58_Cutoff->Checked = true;
	if (range_from <= 59  && range_to >= 59 && TB_4_59->Position == 14) TB_4_59_Cutoff->Checked = true;
	if (range_from <= 60  && range_to >= 60 && TB_4_60->Position == 14) TB_4_60_Cutoff->Checked = true;
	if (range_from <= 61  && range_to >= 61 && TB_4_61->Position == 14) TB_4_61_Cutoff->Checked = true;
	if (range_from <= 62  && range_to >= 62 && TB_4_62->Position == 14) TB_4_62_Cutoff->Checked = true;
	if (range_from <= 63  && range_to >= 63 && TB_4_63->Position == 14) TB_4_63_Cutoff->Checked = true;
	if (range_from <= 64  && range_to >= 64 && TB_4_64->Position == 14) TB_4_64_Cutoff->Checked = true;
	if (range_from <= 65  && range_to >= 65 && TB_4_65->Position == 14) TB_4_65_Cutoff->Checked = true;
	if (range_from <= 66  && range_to >= 66 && TB_4_66->Position == 14) TB_4_66_Cutoff->Checked = true;
	if (range_from <= 67  && range_to >= 67 && TB_4_67->Position == 14) TB_4_67_Cutoff->Checked = true;
	if (range_from <= 68  && range_to >= 68 && TB_4_68->Position == 14) TB_4_68_Cutoff->Checked = true;
	if (range_from <= 69  && range_to >= 69 && TB_4_69->Position == 14) TB_4_69_Cutoff->Checked = true;
	if (range_from <= 70  && range_to >= 70 && TB_4_70->Position == 14) TB_4_70_Cutoff->Checked = true;
	if (range_from <= 71  && range_to >= 71 && TB_4_71->Position == 14) TB_4_71_Cutoff->Checked = true;
	if (range_from <= 72  && range_to >= 72 && TB_4_72->Position == 14) TB_4_72_Cutoff->Checked = true;
	if (range_from <= 73  && range_to >= 73 && TB_4_73->Position == 14) TB_4_73_Cutoff->Checked = true;
	if (range_from <= 74  && range_to >= 74 && TB_4_74->Position == 14) TB_4_74_Cutoff->Checked = true;
	if (range_from <= 75  && range_to >= 75 && TB_4_75->Position == 14) TB_4_75_Cutoff->Checked = true;
	if (range_from <= 76  && range_to >= 76 && TB_4_76->Position == 14) TB_4_76_Cutoff->Checked = true;
	if (range_from <= 77  && range_to >= 77 && TB_4_77->Position == 14) TB_4_77_Cutoff->Checked = true;
	if (range_from <= 78  && range_to >= 78 && TB_4_78->Position == 14) TB_4_78_Cutoff->Checked = true;
	if (range_from <= 79  && range_to >= 79 && TB_4_79->Position == 14) TB_4_79_Cutoff->Checked = true;
	if (range_from <= 80  && range_to >= 80 && TB_4_80->Position == 14) TB_4_80_Cutoff->Checked = true;
	if (range_from <= 81  && range_to >= 81 && TB_4_81->Position == 14) TB_4_81_Cutoff->Checked = true;
	if (range_from <= 82  && range_to >= 82 && TB_4_82->Position == 14) TB_4_82_Cutoff->Checked = true;
	if (range_from <= 83  && range_to >= 83 && TB_4_83->Position == 14) TB_4_83_Cutoff->Checked = true;
	if (range_from <= 84  && range_to >= 84 && TB_4_84->Position == 14) TB_4_84_Cutoff->Checked = true;
	if (range_from <= 85  && range_to >= 85 && TB_4_85->Position == 14) TB_4_85_Cutoff->Checked = true;
	if (range_from <= 86  && range_to >= 86 && TB_4_86->Position == 14) TB_4_86_Cutoff->Checked = true;
	if (range_from <= 87  && range_to >= 87 && TB_4_87->Position == 14) TB_4_87_Cutoff->Checked = true;
	if (range_from <= 88  && range_to >= 88 && TB_4_88->Position == 14) TB_4_88_Cutoff->Checked = true;
	if (range_from <= 89  && range_to >= 89 && TB_4_89->Position == 14) TB_4_89_Cutoff->Checked = true;
	if (range_from <= 90  && range_to >= 90 && TB_4_90->Position == 14) TB_4_90_Cutoff->Checked = true;
	if (range_from <= 91  && range_to >= 91 && TB_4_91->Position == 14) TB_4_91_Cutoff->Checked = true;
	if (range_from <= 92  && range_to >= 92 && TB_4_92->Position == 14) TB_4_92_Cutoff->Checked = true;
	if (range_from <= 93  && range_to >= 93 && TB_4_93->Position == 14) TB_4_93_Cutoff->Checked = true;
	if (range_from <= 94  && range_to >= 94 && TB_4_94->Position == 14) TB_4_94_Cutoff->Checked = true;
	if (range_from <= 95  && range_to >= 95 && TB_4_95->Position == 14) TB_4_95_Cutoff->Checked = true;
	if (range_from <= 96  && range_to >= 96 && TB_4_96->Position == 14) TB_4_96_Cutoff->Checked = true;
	if (range_from <= 97  && range_to >= 97 && TB_4_97->Position == 14) TB_4_97_Cutoff->Checked = true;
	if (range_from <= 98  && range_to >= 98 && TB_4_98->Position == 14) TB_4_98_Cutoff->Checked = true;

	if (range_from <= 1  && range_to >= 1) 		TB_4_1->Position = TB_4_1->Position +1;
	if (range_from <= 2  && range_to >= 2) 		TB_4_2->Position = TB_4_2->Position +1;
	if (range_from <= 3  && range_to >= 3) 		TB_4_3->Position = TB_4_3->Position +1;
	if (range_from <= 4  && range_to >= 4) 		TB_4_4->Position = TB_4_4->Position +1;
	if (range_from <= 5  && range_to >= 5) 		TB_4_5->Position = TB_4_5->Position +1;
	if (range_from <= 6  && range_to >= 6) 		TB_4_6->Position = TB_4_6->Position +1;
	if (range_from <= 7  && range_to >= 7) 		TB_4_7->Position = TB_4_7->Position +1;
	if (range_from <= 8  && range_to >= 8) 		TB_4_8->Position = TB_4_8->Position +1;
	if (range_from <= 9  && range_to >= 9) 		TB_4_9->Position = TB_4_9->Position +1;
	if (range_from <= 10  && range_to >= 10)	TB_4_10->Position = TB_4_10->Position +1;
	if (range_from <= 11  && range_to >= 11)	TB_4_11->Position = TB_4_11->Position +1;
	if (range_from <= 12  && range_to >= 12)	TB_4_12->Position = TB_4_12->Position +1;
	if (range_from <= 13  && range_to >= 13)	TB_4_13->Position = TB_4_13->Position +1;
	if (range_from <= 14  && range_to >= 14)	TB_4_14->Position = TB_4_14->Position +1;
	if (range_from <= 15  && range_to >= 15)	TB_4_15->Position = TB_4_15->Position +1;
	if (range_from <= 16  && range_to >= 16)	TB_4_16->Position = TB_4_16->Position +1;
	if (range_from <= 17  && range_to >= 17)	TB_4_17->Position = TB_4_17->Position +1;
	if (range_from <= 18  && range_to >= 18)	TB_4_18->Position = TB_4_18->Position +1;
	if (range_from <= 19  && range_to >= 19)	TB_4_19->Position = TB_4_19->Position +1;
	if (range_from <= 20  && range_to >= 20)	TB_4_20->Position = TB_4_20->Position +1;
	if (range_from <= 21  && range_to >= 21)	TB_4_21->Position = TB_4_21->Position +1;
	if (range_from <= 22  && range_to >= 22)	TB_4_22->Position = TB_4_22->Position +1;
	if (range_from <= 23  && range_to >= 23)	TB_4_23->Position = TB_4_23->Position +1;
	if (range_from <= 24  && range_to >= 24)	TB_4_24->Position = TB_4_24->Position +1;
	if (range_from <= 25  && range_to >= 25)	TB_4_25->Position = TB_4_25->Position +1;
	if (range_from <= 26  && range_to >= 26)	TB_4_26->Position = TB_4_26->Position +1;
	if (range_from <= 27  && range_to >= 27)	TB_4_27->Position = TB_4_27->Position +1;
	if (range_from <= 28  && range_to >= 28)	TB_4_28->Position = TB_4_28->Position +1;
	if (range_from <= 29  && range_to >= 29)	TB_4_29->Position = TB_4_29->Position +1;
	if (range_from <= 30  && range_to >= 30)	TB_4_30->Position = TB_4_30->Position +1;
	if (range_from <= 31  && range_to >= 31)	TB_4_31->Position = TB_4_31->Position +1;
	if (range_from <= 32  && range_to >= 32)	TB_4_32->Position = TB_4_32->Position +1;
	if (range_from <= 33  && range_to >= 33)	TB_4_33->Position = TB_4_33->Position +1;
	if (range_from <= 34  && range_to >= 34)	TB_4_34->Position = TB_4_34->Position +1;
	if (range_from <= 35  && range_to >= 35)	TB_4_35->Position = TB_4_35->Position +1;
	if (range_from <= 36  && range_to >= 36)	TB_4_36->Position = TB_4_36->Position +1;
	if (range_from <= 37  && range_to >= 37)	TB_4_37->Position = TB_4_37->Position +1;
	if (range_from <= 38  && range_to >= 38)	TB_4_38->Position = TB_4_38->Position +1;
	if (range_from <= 39  && range_to >= 39)	TB_4_39->Position = TB_4_39->Position +1;
	if (range_from <= 40  && range_to >= 40)	TB_4_40->Position = TB_4_40->Position +1;
	if (range_from <= 41  && range_to >= 41)	TB_4_41->Position = TB_4_41->Position +1;
	if (range_from <= 42  && range_to >= 42)	TB_4_42->Position = TB_4_42->Position +1;
	if (range_from <= 43  && range_to >= 43)	TB_4_43->Position = TB_4_43->Position +1;
	if (range_from <= 44  && range_to >= 44)	TB_4_44->Position = TB_4_44->Position +1;
	if (range_from <= 45  && range_to >= 45)	TB_4_45->Position = TB_4_45->Position +1;
	if (range_from <= 46  && range_to >= 46)	TB_4_46->Position = TB_4_46->Position +1;
	if (range_from <= 47  && range_to >= 47)	TB_4_47->Position = TB_4_47->Position +1;
	if (range_from <= 48  && range_to >= 48)	TB_4_48->Position = TB_4_48->Position +1;
	if (range_from <= 49  && range_to >= 49)	TB_4_49->Position = TB_4_49->Position +1;
	if (range_from <= 50  && range_to >= 50)	TB_4_50->Position = TB_4_50->Position +1;
	if (range_from <= 51  && range_to >= 51)	TB_4_51->Position = TB_4_51->Position +1;
	if (range_from <= 52  && range_to >= 52)	TB_4_52->Position = TB_4_52->Position +1;
	if (range_from <= 53  && range_to >= 53)	TB_4_53->Position = TB_4_53->Position +1;
	if (range_from <= 54  && range_to >= 54)	TB_4_54->Position = TB_4_54->Position +1;
	if (range_from <= 55  && range_to >= 55)	TB_4_55->Position = TB_4_55->Position +1;
	if (range_from <= 56  && range_to >= 56)	TB_4_56->Position = TB_4_56->Position +1;
	if (range_from <= 57  && range_to >= 57)	TB_4_57->Position = TB_4_57->Position +1;
	if (range_from <= 58  && range_to >= 58)	TB_4_58->Position = TB_4_58->Position +1;
	if (range_from <= 59  && range_to >= 59)	TB_4_59->Position = TB_4_59->Position +1;
	if (range_from <= 60  && range_to >= 60)	TB_4_60->Position = TB_4_60->Position +1;
	if (range_from <= 61  && range_to >= 61)	TB_4_61->Position = TB_4_61->Position +1;
	if (range_from <= 62  && range_to >= 62)	TB_4_62->Position = TB_4_62->Position +1;
	if (range_from <= 63  && range_to >= 63)	TB_4_63->Position = TB_4_63->Position +1;
	if (range_from <= 64  && range_to >= 64)	TB_4_64->Position = TB_4_64->Position +1;
	if (range_from <= 65  && range_to >= 65)	TB_4_65->Position = TB_4_65->Position +1;
	if (range_from <= 66  && range_to >= 66)	TB_4_66->Position = TB_4_66->Position +1;
	if (range_from <= 67  && range_to >= 67)	TB_4_67->Position = TB_4_67->Position +1;
	if (range_from <= 68  && range_to >= 68)	TB_4_68->Position = TB_4_68->Position +1;
	if (range_from <= 69  && range_to >= 69)	TB_4_69->Position = TB_4_69->Position +1;
	if (range_from <= 70  && range_to >= 70)	TB_4_70->Position = TB_4_70->Position +1;
	if (range_from <= 71  && range_to >= 71)	TB_4_71->Position = TB_4_71->Position +1;
	if (range_from <= 72  && range_to >= 72)	TB_4_72->Position = TB_4_72->Position +1;
	if (range_from <= 73  && range_to >= 73)	TB_4_73->Position = TB_4_73->Position +1;
	if (range_from <= 74  && range_to >= 74)	TB_4_74->Position = TB_4_74->Position +1;
	if (range_from <= 75  && range_to >= 75)	TB_4_75->Position = TB_4_75->Position +1;
	if (range_from <= 76  && range_to >= 76)	TB_4_76->Position = TB_4_76->Position +1;
	if (range_from <= 77  && range_to >= 77)	TB_4_77->Position = TB_4_77->Position +1;
	if (range_from <= 78  && range_to >= 78)	TB_4_78->Position = TB_4_78->Position +1;
	if (range_from <= 79  && range_to >= 79)	TB_4_79->Position = TB_4_79->Position +1;
	if (range_from <= 80  && range_to >= 80)	TB_4_80->Position = TB_4_80->Position +1;
	if (range_from <= 81  && range_to >= 81)	TB_4_81->Position = TB_4_81->Position +1;
	if (range_from <= 82  && range_to >= 82)	TB_4_82->Position = TB_4_82->Position +1;
	if (range_from <= 83  && range_to >= 83)	TB_4_83->Position = TB_4_83->Position +1;
	if (range_from <= 84  && range_to >= 84)	TB_4_84->Position = TB_4_84->Position +1;
	if (range_from <= 85  && range_to >= 85)	TB_4_85->Position = TB_4_85->Position +1;
	if (range_from <= 86  && range_to >= 86)	TB_4_86->Position = TB_4_86->Position +1;
	if (range_from <= 87  && range_to >= 87)	TB_4_87->Position = TB_4_87->Position +1;
	if (range_from <= 88  && range_to >= 88)	TB_4_88->Position = TB_4_88->Position +1;
	if (range_from <= 89  && range_to >= 89)	TB_4_89->Position = TB_4_89->Position +1;
	if (range_from <= 90  && range_to >= 90)	TB_4_90->Position = TB_4_90->Position +1;
	if (range_from <= 91  && range_to >= 91)	TB_4_91->Position = TB_4_91->Position +1;
	if (range_from <= 92  && range_to >= 92)	TB_4_92->Position = TB_4_92->Position +1;
	if (range_from <= 93  && range_to >= 93)	TB_4_93->Position = TB_4_93->Position +1;
	if (range_from <= 94  && range_to >= 94)	TB_4_94->Position = TB_4_94->Position +1;
	if (range_from <= 95  && range_to >= 95)	TB_4_95->Position = TB_4_95->Position +1;
	if (range_from <= 96  && range_to >= 96)	TB_4_96->Position = TB_4_96->Position +1;
	if (range_from <= 97  && range_to >= 97)	TB_4_97->Position = TB_4_97->Position +1;
	if (range_from <= 98  && range_to >= 98)	TB_4_98->Position = TB_4_98->Position +1;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_type4_copy_from_type3Click(TObject *Sender)
{
	double d;

	PRINT_FN_LOG("btn_type4_copy_from_type3Click","Start");
	TB_4_1->Position  = TB_3_1->Position;
	TB_4_2->Position  = TB_3_2->Position;
	TB_4_3->Position  = TB_3_3->Position;
	TB_4_4->Position  = TB_3_4->Position;
	TB_4_5->Position  = TB_3_5->Position;
	TB_4_6->Position  = TB_3_6->Position;
	TB_4_7->Position  = TB_3_7->Position;
	TB_4_8->Position  = TB_3_8->Position;
	TB_4_9->Position  = TB_3_9->Position;
	TB_4_10->Position = TB_3_10->Position;
	TB_4_11->Position = TB_3_11->Position;
	TB_4_12->Position = TB_3_12->Position;
	TB_4_13->Position = TB_3_13->Position;
	TB_4_14->Position = TB_3_14->Position;
	TB_4_15->Position = TB_3_15->Position;
	TB_4_16->Position = TB_3_16->Position;
	TB_4_17->Position = TB_3_17->Position;
	TB_4_18->Position = TB_3_18->Position;
	TB_4_19->Position = TB_3_19->Position;
	TB_4_20->Position = TB_3_20->Position;
	TB_4_21->Position = TB_3_21->Position;
	TB_4_22->Position = TB_3_22->Position;
	TB_4_23->Position = TB_3_23->Position;
	TB_4_24->Position = TB_3_24->Position;
	TB_4_25->Position = TB_3_25->Position;
	TB_4_26->Position = TB_3_26->Position;
	TB_4_27->Position = TB_3_27->Position;
	TB_4_28->Position = TB_3_28->Position;
	TB_4_29->Position = TB_3_29->Position;
	TB_4_30->Position = TB_3_30->Position;
	TB_4_31->Position = TB_3_31->Position;
	TB_4_32->Position = TB_3_32->Position;
	TB_4_33->Position = TB_3_33->Position;
	TB_4_34->Position = TB_3_34->Position;
	TB_4_35->Position = TB_3_35->Position;
	TB_4_36->Position = TB_3_36->Position;
	TB_4_37->Position = TB_3_37->Position;
	TB_4_38->Position = TB_3_38->Position;
	TB_4_39->Position = TB_3_39->Position;
	TB_4_40->Position = TB_3_40->Position;
	TB_4_41->Position = TB_3_41->Position;
	TB_4_42->Position = TB_3_42->Position;
	TB_4_43->Position = TB_3_43->Position;
	TB_4_44->Position = TB_3_44->Position;
	TB_4_45->Position = TB_3_45->Position;
	TB_4_46->Position = TB_3_46->Position;
	TB_4_47->Position = TB_3_47->Position;
	TB_4_48->Position = TB_3_48->Position;
	TB_4_49->Position = TB_3_49->Position;
	TB_4_50->Position = TB_3_50->Position;
	TB_4_50->Position = TB_3_50->Position;
	TB_4_51->Position = TB_3_51->Position;
	TB_4_52->Position = TB_3_52->Position;
	TB_4_53->Position = TB_3_53->Position;
	TB_4_54->Position = TB_3_54->Position;
	TB_4_55->Position = TB_3_55->Position;
	TB_4_56->Position = TB_3_56->Position;
	TB_4_57->Position = TB_3_57->Position;
	TB_4_58->Position = TB_3_58->Position;
	TB_4_59->Position = TB_3_59->Position;
	TB_4_60->Position = TB_3_60->Position;
	TB_4_61->Position = TB_3_61->Position;
	TB_4_62->Position = TB_3_62->Position;
	TB_4_63->Position = TB_3_63->Position;
	TB_4_64->Position = TB_3_64->Position;
	TB_4_65->Position = TB_3_65->Position;
	TB_4_66->Position = TB_3_66->Position;
	TB_4_67->Position = TB_3_67->Position;
	TB_4_68->Position = TB_3_68->Position;
	TB_4_69->Position = TB_3_69->Position;
	TB_4_70->Position = TB_3_70->Position;
	TB_4_71->Position = TB_3_71->Position;
	TB_4_72->Position = TB_3_72->Position;
	TB_4_73->Position = TB_3_73->Position;
	TB_4_74->Position = TB_3_74->Position;
	TB_4_75->Position = TB_3_75->Position;
	TB_4_76->Position = TB_3_76->Position;
	TB_4_77->Position = TB_3_77->Position;
	TB_4_78->Position = TB_3_78->Position;
	TB_4_79->Position = 0;
	TB_4_80->Position = 0;
	TB_4_81->Position = 0;
	TB_4_82->Position = 0;
	TB_4_83->Position = 0;
	TB_4_84->Position = 0;
	TB_4_85->Position = 0;
	TB_4_86->Position = 0;
	TB_4_87->Position = 0;
	TB_4_88->Position = 0;
	TB_4_89->Position = 0;
	TB_4_90->Position = 0;
	TB_4_91->Position = 0;
	TB_4_92->Position = 0;
	TB_4_93->Position = 0;
	TB_4_94->Position = 0;
	TB_4_95->Position = 0;
	TB_4_96->Position = 0;
	TB_4_97->Position = 0;
	TB_4_98->Position = 0;
	PRINT_FN_LOG("btn_type4_copy_from_type2Click","End");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbT1_range_fromChange(TObject *Sender)
{
	AnsiString range[] = {"","20Hz","25Hz","31Hz","40Hz","50Hz","63Hz","80Hz","100Hz","125Hz","160Hz","200Hz","250Hz","315Hz","400Hz","500Hz","630Hz","800Hz","1kHz","1.25kHz","1.6kHz","2kHz","2.5kHz","3.15kHz","4kHz","5kHz","6.3kHz","8kHz","10kHz","12.5kHz","16kHz","20kHz","25kHz","31.5KHz","40kHz","50kHz"};

	T1_range_label->Caption = range[cmbT1_range_from->ItemIndex] + " - " + range[cmbT1_range_to->ItemIndex];

}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbT1_range_toChange(TObject *Sender)
{
	AnsiString range[] = {"","20Hz","25Hz","31Hz","40Hz","50Hz","63Hz","80Hz","100Hz","125Hz","160Hz","200Hz","250Hz","315Hz","400Hz","500Hz","630Hz","800Hz","1kHz","1.25kHz","1.6kHz","2kHz","2.5kHz","3.15kHz","4kHz","5kHz","6.3kHz","8kHz","10kHz","12.5kHz","16kHz","20kHz","25kHz","31.5KHz","40kHz","50kHz"};

	T1_range_label->Caption = range[cmbT1_range_from->ItemIndex] + " - " + range[cmbT1_range_to->ItemIndex];
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbT2_range_fromChange(TObject *Sender)
{
	AnsiString range[] = {"","20Hz","25Hz","31Hz","40Hz","50Hz","63Hz","80Hz","100Hz","125Hz","160Hz","200Hz","250Hz","315Hz","400Hz","500Hz","630Hz","800Hz","1kHz","1.25kHz","1.6kHz","2kHz","2.5kHz","3.15kHz","4kHz","5kHz","6kHz","7kHz","8kHz","9kHz","10kHz","11kHz","12kHz","13kHz","14KHz","15kHz","16kHz","17kHz","18kHz","19kHz","20kHz","21kHz","22kHz","23kHz","24kHz","25kHz","30kHz","35kHz","40kHz","45kHz","50kHz"};

	T2_range_label->Caption = range[cmbT2_range_from->ItemIndex] + " - " + range[cmbT2_range_to->ItemIndex];
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbT2_range_toChange(TObject *Sender)
{
	AnsiString range[] = {"","20Hz","25Hz","31Hz","40Hz","50Hz","63Hz","80Hz","100Hz","125Hz","160Hz","200Hz","250Hz","315Hz","400Hz","500Hz","630Hz","800Hz","1kHz","1.25kHz","1.6kHz","2kHz","2.5kHz","3.15kHz","4kHz","5kHz","6kHz","7kHz","8kHz","9kHz","10kHz","11kHz","12kHz","13kHz","14KHz","15kHz","16kHz","17kHz","18kHz","19kHz","20kHz","21kHz","22kHz","23kHz","24kHz","25kHz","30kHz","35kHz","40kHz","45kHz","50kHz"};

	T2_range_label->Caption = range[cmbT2_range_from->ItemIndex] + " - " + range[cmbT2_range_to->ItemIndex];
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbT3_range_fromChange(TObject *Sender)
{
	AnsiString range[] = {"","5Hz","6Hz","8Hz","10Hz","12Hz","16Hz","20Hz","25Hz","31Hz","40Hz","50Hz","63Hz","80Hz","100Hz","125Hz","160Hz","200Hz","250Hz","315Hz","400Hz","500Hz","630Hz","800Hz","1kHz","1.25kHz","1.6kHz","2kHz","2.5kHz","3.15kHz","4kHz","5kHz","6kHz","7kHz","8kHz","9kHz","10kHz","11kHz","12kHz","13kHz","14KHz","15kHz","16kHz","17kHz","18kHz","19kHz","20kHz","21kHz","22kHz","23kHz","24kHz","25kHz","26kHz","27kHz","28kHz","29kHz","30kHz","31kHz","32kHz","33kHz","34kHz","35kHz","36kHz","37kHz","38kHz","39kHz","40kHz","45kHz","50kHz","55kHz","60kHz","65kHz","70kHz","75kHz","80kHz","85kHz","90kHz","95kHz","100kHz"};

	T3_range_label->Caption = range[cmbT3_range_from->ItemIndex] + " - " + range[cmbT3_range_to->ItemIndex];

}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbT3_range_toChange(TObject *Sender)
{
	AnsiString range[] = {"","5Hz","6Hz","8Hz","10Hz","12Hz","16Hz","20Hz","25Hz","31Hz","40Hz","50Hz","63Hz","80Hz","100Hz","125Hz","160Hz","200Hz","250Hz","315Hz","400Hz","500Hz","630Hz","800Hz","1kHz","1.25kHz","1.6kHz","2kHz","2.5kHz","3.15kHz","4kHz","5kHz","6kHz","7kHz","8kHz","9kHz","10kHz","11kHz","12kHz","13kHz","14KHz","15kHz","16kHz","17kHz","18kHz","19kHz","20kHz","21kHz","22kHz","23kHz","24kHz","25kHz","26kHz","27kHz","28kHz","29kHz","30kHz","31kHz","32kHz","33kHz","34kHz","35kHz","36kHz","37kHz","38kHz","39kHz","40kHz","45kHz","50kHz","55kHz","60kHz","65kHz","70kHz","75kHz","80kHz","85kHz","90kHz","95kHz","100kHz"};

	T3_range_label->Caption = range[cmbT3_range_from->ItemIndex] + " - " + range[cmbT3_range_to->ItemIndex];

}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbT4_range_fromChange(TObject *Sender)
{
	AnsiString range[] = {"","5Hz","6Hz","8Hz","10Hz","12Hz","16Hz","20Hz","25Hz","31Hz","40Hz","50Hz","63Hz","80Hz","100Hz","125Hz","160Hz","200Hz","250Hz","315Hz","400Hz","500Hz","630Hz","800Hz","1kHz","1.25kHz","1.6kHz","2kHz","2.5kHz","3.15kHz","4kHz","5kHz","6kHz","7kHz","8kHz","9kHz","10kHz","11kHz","12kHz","13kHz","14KHz","15kHz","16kHz","17kHz","18kHz","19kHz","20kHz","21kHz","22kHz","23kHz","24kHz","25kHz","26kHz","27kHz","28kHz","29kHz","30kHz","31kHz","32kHz","33kHz","34kHz","35kHz","36kHz","37kHz","38kHz","39kHz","40kHz","45kHz","50kHz","55kHz","60kHz","65kHz","70kHz","75kHz","80kHz","85kHz","90kHz","95kHz","100kHz","105kHz","110kHz","115kHz","120kHz","125kHz","130kHz","135kHz","140kHz","145kHz","150kHz","155kHz","160kHz","165kHz","170kHz","175kHz","180kHz","185kHz","190kHz","195kHz","200kHz"};

	T4_range_label->Caption = range[cmbT4_range_from->ItemIndex] + " - " + range[cmbT4_range_to->ItemIndex];

}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbT4_range_toChange(TObject *Sender)
{
	AnsiString range[] = {"","5Hz","6Hz","8Hz","10Hz","12Hz","16Hz","20Hz","25Hz","31Hz","40Hz","50Hz","63Hz","80Hz","100Hz","125Hz","160Hz","200Hz","250Hz","315Hz","400Hz","500Hz","630Hz","800Hz","1kHz","1.25kHz","1.6kHz","2kHz","2.5kHz","3.15kHz","4kHz","5kHz","6kHz","7kHz","8kHz","9kHz","10kHz","11kHz","12kHz","13kHz","14KHz","15kHz","16kHz","17kHz","18kHz","19kHz","20kHz","21kHz","22kHz","23kHz","24kHz","25kHz","26kHz","27kHz","28kHz","29kHz","30kHz","31kHz","32kHz","33kHz","34kHz","35kHz","36kHz","37kHz","38kHz","39kHz","40kHz","45kHz","50kHz","55kHz","60kHz","65kHz","70kHz","75kHz","80kHz","85kHz","90kHz","95kHz","100kHz","105kHz","110kHz","115kHz","120kHz","125kHz","130kHz","135kHz","140kHz","145kHz","150kHz","155kHz","160kHz","165kHz","170kHz","175kHz","180kHz","185kHz","190kHz","195kHz","200kHz"};

	T4_range_label->Caption = range[cmbT4_range_from->ItemIndex] + " - " + range[cmbT4_range_to->ItemIndex];

}
//---------------------------------------------------------------------------



void __fastcall TForm1::TB_4_1_CutoffClick(TObject *Sender)
{
	if (TB_4_1_Cutoff->Checked == false) {
		TB_4_1->Position = TB_4_1->Tag;
	} else {
		TB_4_1->Tag = TB_4_1->Position;
		TB_4_1->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_2_CutoffClick(TObject *Sender)
{
	if (TB_4_2_Cutoff->Checked == false) {
		TB_4_2->Position = TB_4_2->Tag;
	} else {
		TB_4_2->Tag = TB_4_2->Position;
		TB_4_2->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_3_CutoffClick(TObject *Sender)
{
	if (TB_4_3_Cutoff->Checked == false) {
		TB_4_3->Position = TB_4_3->Tag;
	} else {
		TB_4_3->Tag = TB_4_3->Position;
		TB_4_3->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_4_CutoffClick(TObject *Sender)
{
	if (TB_4_4_Cutoff->Checked == false) {
		TB_4_4->Position = TB_4_4->Tag;
	} else {
		TB_4_4->Tag = TB_4_4->Position;
		TB_4_4->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_5_CutoffClick(TObject *Sender)
{
	if (TB_4_5_Cutoff->Checked == false) {
		TB_4_5->Position = TB_4_5->Tag;
	} else {
		TB_4_5->Tag = TB_4_5->Position;
		TB_4_5->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_6_CutoffClick(TObject *Sender)
{
	if (TB_4_6_Cutoff->Checked == false) {
		TB_4_6->Position = TB_4_6->Tag;
	} else {
		TB_4_6->Tag = TB_4_6->Position;
		TB_4_6->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_7_CutoffClick(TObject *Sender)
{
	if (TB_4_7_Cutoff->Checked == false) {
		TB_4_7->Position = TB_4_7->Tag;
	} else {
		TB_4_7->Tag = TB_4_7->Position;
		TB_4_7->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_8_CutoffClick(TObject *Sender)
{
	if (TB_4_8_Cutoff->Checked == false) {
		TB_4_8->Position = TB_4_8->Tag;
	} else {
		TB_4_8->Tag = TB_4_8->Position;
		TB_4_8->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_9_CutoffClick(TObject *Sender)
{
	if (TB_4_9_Cutoff->Checked == false) {
		TB_4_9->Position = TB_4_9->Tag;
	} else {
		TB_4_9->Tag = TB_4_9->Position;
		TB_4_9->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_10_CutoffClick(TObject *Sender)
{
	if (TB_4_10_Cutoff->Checked == false) {
		TB_4_10->Position = TB_4_10->Tag;
	} else {
		TB_4_10->Tag = TB_4_10->Position;
		TB_4_10->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_11_CutoffClick(TObject *Sender)
{
	if (TB_4_11_Cutoff->Checked == false) {
		TB_4_11->Position = TB_4_11->Tag;
	} else {
		TB_4_11->Tag = TB_4_11->Position;
		TB_4_11->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_12_CutoffClick(TObject *Sender)
{
	if (TB_4_12_Cutoff->Checked == false) {
		TB_4_12->Position = TB_4_12->Tag;
	} else {
		TB_4_12->Tag = TB_4_12->Position;
		TB_4_12->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_13_CutoffClick(TObject *Sender)
{
	if (TB_4_13_Cutoff->Checked == false) {
		TB_4_13->Position = TB_4_13->Tag;
	} else {
		TB_4_13->Tag = TB_4_13->Position;
		TB_4_13->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_36_CutoffClick(TObject *Sender)
{
	if (TB_4_36_Cutoff->Checked == false) {
		TB_4_36->Position = TB_4_36->Tag;
	} else {
		TB_4_36->Tag = TB_4_36->Position;
		TB_4_36->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_37_CutoffClick(TObject *Sender)
{
	if (TB_4_37_Cutoff->Checked == false) {
		TB_4_37->Position = TB_4_37->Tag;
	} else {
		TB_4_37->Tag = TB_4_37->Position;
		TB_4_37->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_38_CutoffClick(TObject *Sender)
{
	if (TB_4_38_Cutoff->Checked == false) {
		TB_4_38->Position = TB_4_38->Tag;
	} else {
		TB_4_38->Tag = TB_4_38->Position;
		TB_4_38->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_39_CutoffClick(TObject *Sender)
{
	if (TB_4_39_Cutoff->Checked == false) {
		TB_4_39->Position = TB_4_39->Tag;
	} else {
		TB_4_39->Tag = TB_4_39->Position;
		TB_4_39->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_40_CutoffClick(TObject *Sender)
{
	if (TB_4_40_Cutoff->Checked == false) {
		TB_4_40->Position = TB_4_40->Tag;
	} else {
		TB_4_40->Tag = TB_4_40->Position;
		TB_4_40->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_41_CutoffClick(TObject *Sender)
{
	if (TB_4_41_Cutoff->Checked == false) {
		TB_4_41->Position = TB_4_41->Tag;
	} else {
		TB_4_41->Tag = TB_4_41->Position;
		TB_4_41->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_42_CutoffClick(TObject *Sender)
{
	if (TB_4_42_Cutoff->Checked == false) {
		TB_4_42->Position = TB_4_42->Tag;
	} else {
		TB_4_42->Tag = TB_4_42->Position;
		TB_4_42->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_43_CutoffClick(TObject *Sender)
{
	if (TB_4_43_Cutoff->Checked == false) {
		TB_4_43->Position = TB_4_43->Tag;
	} else {
		TB_4_43->Tag = TB_4_43->Position;
		TB_4_43->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_44_CutoffClick(TObject *Sender)
{
	if (TB_4_44_Cutoff->Checked == false) {
		TB_4_44->Position = TB_4_44->Tag;
	} else {
		TB_4_44->Tag = TB_4_44->Position;
		TB_4_44->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_45_CutoffClick(TObject *Sender)
{
	if (TB_4_45_Cutoff->Checked == false) {
		TB_4_45->Position = TB_4_45->Tag;
	} else {
		TB_4_45->Tag = TB_4_45->Position;
		TB_4_45->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_46_CutoffClick(TObject *Sender)
{
	if (TB_4_46_Cutoff->Checked == false) {
		TB_4_46->Position = TB_4_46->Tag;
	} else {
		TB_4_46->Tag = TB_4_46->Position;
		TB_4_46->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_47_CutoffClick(TObject *Sender)
{
	if (TB_4_47_Cutoff->Checked == false) {
		TB_4_47->Position = TB_4_47->Tag;
	} else {
		TB_4_47->Tag = TB_4_47->Position;
		TB_4_47->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_48_CutoffClick(TObject *Sender)
{
	if (TB_4_48_Cutoff->Checked == false) {
		TB_4_48->Position = TB_4_48->Tag;
	} else {
		TB_4_48->Tag = TB_4_48->Position;
		TB_4_48->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_49_CutoffClick(TObject *Sender)
{
	if (TB_4_49_Cutoff->Checked == false) {
		TB_4_49->Position = TB_4_49->Tag;
	} else {
		TB_4_49->Tag = TB_4_49->Position;
		TB_4_49->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_50_CutoffClick(TObject *Sender)
{
	if (TB_4_50_Cutoff->Checked == false) {
		TB_4_50->Position = TB_4_50->Tag;
	} else {
		TB_4_50->Tag = TB_4_50->Position;
		TB_4_50->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_51_CutoffClick(TObject *Sender)
{
	if (TB_4_51_Cutoff->Checked == false) {
		TB_4_51->Position = TB_4_51->Tag;
	} else {
		TB_4_51->Tag = TB_4_51->Position;
		TB_4_51->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_52_CutoffClick(TObject *Sender)
{
	if (TB_4_52_Cutoff->Checked == false) {
		TB_4_52->Position = TB_4_52->Tag;
	} else {
		TB_4_52->Tag = TB_4_52->Position;
		TB_4_52->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_53_CutoffClick(TObject *Sender)
{
	if (TB_4_53_Cutoff->Checked == false) {
		TB_4_53->Position = TB_4_53->Tag;
	} else {
		TB_4_53->Tag = TB_4_53->Position;
		TB_4_53->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_54_CutoffClick(TObject *Sender)
{
	if (TB_4_54_Cutoff->Checked == false) {
		TB_4_54->Position = TB_4_54->Tag;
	} else {
		TB_4_54->Tag = TB_4_54->Position;
		TB_4_54->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_55_CutoffClick(TObject *Sender)
{
	if (TB_4_55_Cutoff->Checked == false) {
		TB_4_55->Position = TB_4_55->Tag;
	} else {
		TB_4_55->Tag = TB_4_55->Position;
		TB_4_55->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_56_CutoffClick(TObject *Sender)
{
	if (TB_4_56_Cutoff->Checked == false) {
		TB_4_56->Position = TB_4_56->Tag;
	} else {
		TB_4_56->Tag = TB_4_56->Position;
		TB_4_56->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_57_CutoffClick(TObject *Sender)
{
	if (TB_4_57_Cutoff->Checked == false) {
		TB_4_57->Position = TB_4_57->Tag;
	} else {
		TB_4_57->Tag = TB_4_57->Position;
		TB_4_57->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_58_CutoffClick(TObject *Sender)
{
	if (TB_4_58_Cutoff->Checked == false) {
		TB_4_58->Position = TB_4_58->Tag;
	} else {
		TB_4_58->Tag = TB_4_58->Position;
		TB_4_58->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_59_CutoffClick(TObject *Sender)
{
	if (TB_4_59_Cutoff->Checked == false) {
		TB_4_59->Position = TB_4_59->Tag;
	} else {
		TB_4_59->Tag = TB_4_59->Position;
		TB_4_59->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_60_CutoffClick(TObject *Sender)
{
	if (TB_4_60_Cutoff->Checked == false) {
		TB_4_60->Position = TB_4_60->Tag;
	} else {
		TB_4_60->Tag = TB_4_60->Position;
		TB_4_60->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_61_CutoffClick(TObject *Sender)
{
	if (TB_4_61_Cutoff->Checked == false) {
		TB_4_61->Position = TB_4_61->Tag;
	} else {
		TB_4_61->Tag = TB_4_61->Position;
		TB_4_61->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_62_CutoffClick(TObject *Sender)
{
	if (TB_4_62_Cutoff->Checked == false) {
		TB_4_62->Position = TB_4_62->Tag;
	} else {
		TB_4_62->Tag = TB_4_62->Position;
		TB_4_62->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_63_CutoffClick(TObject *Sender)
{
	if (TB_4_63_Cutoff->Checked == false) {
		TB_4_63->Position = TB_4_63->Tag;
	} else {
		TB_4_63->Tag = TB_4_63->Position;
		TB_4_63->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_64_CutoffClick(TObject *Sender)
{
	if (TB_4_64_Cutoff->Checked == false) {
		TB_4_64->Position = TB_4_64->Tag;
	} else {
		TB_4_64->Tag = TB_4_64->Position;
		TB_4_64->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_65_CutoffClick(TObject *Sender)
{
	if (TB_4_65_Cutoff->Checked == false) {
		TB_4_65->Position = TB_4_65->Tag;
	} else {
		TB_4_65->Tag = TB_4_65->Position;
		TB_4_65->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_66_CutoffClick(TObject *Sender)
{
	if (TB_4_66_Cutoff->Checked == false) {
		TB_4_66->Position = TB_4_66->Tag;
	} else {
		TB_4_66->Tag = TB_4_66->Position;
		TB_4_66->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_67_CutoffClick(TObject *Sender)
{
	if (TB_4_67_Cutoff->Checked == false) {
		TB_4_67->Position = TB_4_67->Tag;
	} else {
		TB_4_67->Tag = TB_4_67->Position;
		TB_4_67->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_68_CutoffClick(TObject *Sender)
{
	if (TB_4_68_Cutoff->Checked == false) {
		TB_4_68->Position = TB_4_68->Tag;
	} else {
		TB_4_68->Tag = TB_4_68->Position;
		TB_4_68->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_69_CutoffClick(TObject *Sender)
{
	if (TB_4_69_Cutoff->Checked == false) {
		TB_4_69->Position = TB_4_69->Tag;
	} else {
		TB_4_69->Tag = TB_4_69->Position;
		TB_4_69->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_70_CutoffClick(TObject *Sender)
{
	if (TB_4_70_Cutoff->Checked == false) {
		TB_4_70->Position = TB_4_70->Tag;
	} else {
		TB_4_70->Tag = TB_4_70->Position;
		TB_4_70->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_71_CutoffClick(TObject *Sender)
{
	if (TB_4_71_Cutoff->Checked == false) {
		TB_4_71->Position = TB_4_71->Tag;
	} else {
		TB_4_71->Tag = TB_4_71->Position;
		TB_4_71->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_72_CutoffClick(TObject *Sender)
{
	if (TB_4_72_Cutoff->Checked == false) {
		TB_4_72->Position = TB_4_72->Tag;
	} else {
		TB_4_72->Tag = TB_4_72->Position;
		TB_4_72->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_73_CutoffClick(TObject *Sender)
{
	if (TB_4_73_Cutoff->Checked == false) {
		TB_4_73->Position = TB_4_73->Tag;
	} else {
		TB_4_73->Tag = TB_4_73->Position;
		TB_4_73->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_74_CutoffClick(TObject *Sender)
{
	if (TB_4_74_Cutoff->Checked == false) {
		TB_4_74->Position = TB_4_74->Tag;
	} else {
		TB_4_74->Tag = TB_4_74->Position;
		TB_4_74->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_75_CutoffClick(TObject *Sender)
{
	if (TB_4_75_Cutoff->Checked == false) {
		TB_4_75->Position = TB_4_75->Tag;
	} else {
		TB_4_75->Tag = TB_4_75->Position;
		TB_4_75->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_76_CutoffClick(TObject *Sender)
{
	if (TB_4_76_Cutoff->Checked == false) {
		TB_4_76->Position = TB_4_76->Tag;
	} else {
		TB_4_76->Tag = TB_4_76->Position;
		TB_4_76->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_77_CutoffClick(TObject *Sender)
{
	if (TB_4_77_Cutoff->Checked == false) {
		TB_4_77->Position = TB_4_77->Tag;
	} else {
		TB_4_77->Tag = TB_4_77->Position;
		TB_4_77->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_78_CutoffClick(TObject *Sender)
{
	if (TB_4_78_Cutoff->Checked == false) {
		TB_4_78->Position = TB_4_78->Tag;
	} else {
		TB_4_78->Tag = TB_4_78->Position;
		TB_4_78->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_79_CutoffClick(TObject *Sender)
{
	if (TB_4_79_Cutoff->Checked == false) {
		TB_4_79->Position = TB_4_79->Tag;
	} else {
		TB_4_79->Tag = TB_4_79->Position;
		TB_4_79->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_80_CutoffClick(TObject *Sender)
{
	if (TB_4_80_Cutoff->Checked == false) {
		TB_4_80->Position = TB_4_80->Tag;
	} else {
		TB_4_80->Tag = TB_4_80->Position;
		TB_4_80->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_81_CutoffClick(TObject *Sender)
{
	if (TB_4_81_Cutoff->Checked == false) {
		TB_4_81->Position = TB_4_81->Tag;
	} else {
		TB_4_81->Tag = TB_4_81->Position;
		TB_4_81->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_82_CutoffClick(TObject *Sender)
{
	if (TB_4_82_Cutoff->Checked == false) {
		TB_4_82->Position = TB_4_82->Tag;
	} else {
		TB_4_82->Tag = TB_4_82->Position;
		TB_4_82->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_83_CutoffClick(TObject *Sender)
{
	if (TB_4_83_Cutoff->Checked == false) {
		TB_4_83->Position = TB_4_83->Tag;
	} else {
		TB_4_83->Tag = TB_4_83->Position;
		TB_4_83->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_84_CutoffClick(TObject *Sender)
{
	if (TB_4_84_Cutoff->Checked == false) {
		TB_4_84->Position = TB_4_84->Tag;
	} else {
		TB_4_84->Tag = TB_4_84->Position;
		TB_4_84->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_85_CutoffClick(TObject *Sender)
{
	if (TB_4_85_Cutoff->Checked == false) {
		TB_4_85->Position = TB_4_85->Tag;
	} else {
		TB_4_85->Tag = TB_4_85->Position;
		TB_4_85->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_86_CutoffClick(TObject *Sender)
{
	if (TB_4_86_Cutoff->Checked == false) {
		TB_4_86->Position = TB_4_86->Tag;
	} else {
		TB_4_86->Tag = TB_4_86->Position;
		TB_4_86->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_87_CutoffClick(TObject *Sender)
{
	if (TB_4_87_Cutoff->Checked == false) {
		TB_4_87->Position = TB_4_87->Tag;
	} else {
		TB_4_87->Tag = TB_4_87->Position;
		TB_4_87->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_88_CutoffClick(TObject *Sender)
{
	if (TB_4_88_Cutoff->Checked == false) {
		TB_4_88->Position = TB_4_88->Tag;
	} else {
		TB_4_88->Tag = TB_4_88->Position;
		TB_4_88->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_89_CutoffClick(TObject *Sender)
{
	if (TB_4_89_Cutoff->Checked == false) {
		TB_4_89->Position = TB_4_89->Tag;
	} else {
		TB_4_89->Tag = TB_4_89->Position;
		TB_4_89->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_90_CutoffClick(TObject *Sender)
{
	if (TB_4_90_Cutoff->Checked == false) {
		TB_4_90->Position = TB_4_90->Tag;
	} else {
		TB_4_90->Tag = TB_4_90->Position;
		TB_4_90->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_91_CutoffClick(TObject *Sender)
{
	if (TB_4_91_Cutoff->Checked == false) {
		TB_4_91->Position = TB_4_91->Tag;
	} else {
		TB_4_91->Tag = TB_4_91->Position;
		TB_4_91->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_92_CutoffClick(TObject *Sender)
{
	if (TB_4_92_Cutoff->Checked == false) {
		TB_4_92->Position = TB_4_92->Tag;
	} else {
		TB_4_92->Tag = TB_4_92->Position;
		TB_4_92->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_93_CutoffClick(TObject *Sender)
{
	if (TB_4_93_Cutoff->Checked == false) {
		TB_4_93->Position = TB_4_93->Tag;
	} else {
		TB_4_93->Tag = TB_4_93->Position;
		TB_4_93->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_94_CutoffClick(TObject *Sender)
{
	if (TB_4_94_Cutoff->Checked == false) {
		TB_4_94->Position = TB_4_94->Tag;
	} else {
		TB_4_94->Tag = TB_4_94->Position;
		TB_4_94->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_95_CutoffClick(TObject *Sender)
{
	if (TB_4_95_Cutoff->Checked == false) {
		TB_4_95->Position = TB_4_95->Tag;
	} else {
		TB_4_95->Tag = TB_4_95->Position;
		TB_4_95->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_96_CutoffClick(TObject *Sender)
{
	if (TB_4_96_Cutoff->Checked == false) {
		TB_4_96->Position = TB_4_96->Tag;
	} else {
		TB_4_96->Tag = TB_4_96->Position;
		TB_4_96->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_97_CutoffClick(TObject *Sender)
{
	if (TB_4_97_Cutoff->Checked == false) {
		TB_4_97->Position = TB_4_97->Tag;
	} else {
		TB_4_97->Tag = TB_4_97->Position;
		TB_4_97->Position = 14;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TB_4_98_CutoffClick(TObject *Sender)
{
	if (TB_4_98_Cutoff->Checked == false) {
		TB_4_98->Position = TB_4_98->Tag;
	} else {
		TB_4_98->Tag = TB_4_98->Position;
		TB_4_98->Position = 14;
	}
}
//---------------------------------------------------------------------------
int __fastcall TForm1::CheckModifyEQ()
{
	int modify = 0;

	// EQ Type1
	if (TB_1_1->Position != TB_1_1->Tag) modify = 1;
	if (TB_1_2->Position != TB_1_2->Tag) modify = 1;
	if (TB_1_3->Position != TB_1_3->Tag) modify = 1;
	if (TB_1_4->Position != TB_1_4->Tag) modify = 1;
	if (TB_1_5->Position != TB_1_5->Tag) modify = 1;
	if (TB_1_6->Position != TB_1_6->Tag) modify = 1;
	if (TB_1_7->Position != TB_1_7->Tag) modify = 1;
	if (TB_1_8->Position != TB_1_8->Tag) modify = 1;
	if (TB_1_9->Position != TB_1_9->Tag) modify = 1;
	if (TB_1_10->Position != TB_1_10->Tag) modify = 1;
	if (TB_1_11->Position != TB_1_11->Tag) modify = 1;
	if (TB_1_12->Position != TB_1_12->Tag) modify = 1;
	if (TB_1_13->Position != TB_1_13->Tag) modify = 1;
	if (TB_1_14->Position != TB_1_14->Tag) modify = 1;
	if (TB_1_15->Position != TB_1_15->Tag) modify = 1;
	if (TB_1_16->Position != TB_1_16->Tag) modify = 1;
	if (TB_1_17->Position != TB_1_17->Tag) modify = 1;
	if (TB_1_18->Position != TB_1_18->Tag) modify = 1;
	if (TB_1_19->Position != TB_1_19->Tag) modify = 1;
	if (TB_1_20->Position != TB_1_20->Tag) modify = 1;
	if (TB_1_21->Position != TB_1_21->Tag) modify = 1;
	if (TB_1_22->Position != TB_1_22->Tag) modify = 1;
	if (TB_1_23->Position != TB_1_23->Tag) modify = 1;
	if (TB_1_24->Position != TB_1_24->Tag) modify = 1;
	if (TB_1_25->Position != TB_1_25->Tag) modify = 1;
	if (TB_1_26->Position != TB_1_26->Tag) modify = 1;
	if (TB_1_27->Position != TB_1_27->Tag) modify = 1;
	if (TB_1_28->Position != TB_1_28->Tag) modify = 1;
	if (TB_1_29->Position != TB_1_29->Tag) modify = 1;
	if (TB_1_30->Position != TB_1_30->Tag) modify = 1;
	if (TB_1_31->Position != TB_1_31->Tag) modify = 1;
	if (TB_1_32->Position != TB_1_32->Tag) modify = 1;
	if (TB_1_33->Position != TB_1_33->Tag) modify = 1;
	if (TB_1_34->Position != TB_1_34->Tag) modify = 1;
	if (TB_1_35->Position != TB_1_35->Tag) modify = 1;

	// EQ Type2
	if (TB_2_1->Position != TB_2_1->Tag) modify = 1;
	if (TB_2_2->Position != TB_2_2->Tag) modify = 1;
	if (TB_2_3->Position != TB_2_3->Tag) modify = 1;
	if (TB_2_4->Position != TB_2_4->Tag) modify = 1;
	if (TB_2_5->Position != TB_2_5->Tag) modify = 1;
	if (TB_2_6->Position != TB_2_6->Tag) modify = 1;
	if (TB_2_7->Position != TB_2_7->Tag) modify = 1;
	if (TB_2_8->Position != TB_2_8->Tag) modify = 1;
	if (TB_2_9->Position != TB_2_9->Tag) modify = 1;
	if (TB_2_10->Position != TB_2_10->Tag) modify = 1;
	if (TB_2_11->Position != TB_2_11->Tag) modify = 1;
	if (TB_2_12->Position != TB_2_12->Tag) modify = 1;
	if (TB_2_13->Position != TB_2_13->Tag) modify = 1;
	if (TB_2_14->Position != TB_2_14->Tag) modify = 1;
	if (TB_2_15->Position != TB_2_15->Tag) modify = 1;
	if (TB_2_16->Position != TB_2_16->Tag) modify = 1;
	if (TB_2_17->Position != TB_2_17->Tag) modify = 1;
	if (TB_2_18->Position != TB_2_18->Tag) modify = 1;
	if (TB_2_19->Position != TB_2_19->Tag) modify = 1;
	if (TB_2_20->Position != TB_2_20->Tag) modify = 1;
	if (TB_2_21->Position != TB_2_21->Tag) modify = 1;
	if (TB_2_22->Position != TB_2_22->Tag) modify = 1;
	if (TB_2_23->Position != TB_2_23->Tag) modify = 1;
	if (TB_2_24->Position != TB_2_24->Tag) modify = 1;
	if (TB_2_25->Position != TB_2_25->Tag) modify = 1;
	if (TB_2_26->Position != TB_2_26->Tag) modify = 1;
	if (TB_2_27->Position != TB_2_27->Tag) modify = 1;
	if (TB_2_28->Position != TB_2_28->Tag) modify = 1;
	if (TB_2_29->Position != TB_2_29->Tag) modify = 1;
	if (TB_2_30->Position != TB_2_30->Tag) modify = 1;
	if (TB_2_31->Position != TB_2_31->Tag) modify = 1;
	if (TB_2_32->Position != TB_2_32->Tag) modify = 1;
	if (TB_2_33->Position != TB_2_33->Tag) modify = 1;
	if (TB_2_34->Position != TB_2_34->Tag) modify = 1;
	if (TB_2_35->Position != TB_2_35->Tag) modify = 1;
	if (TB_2_36->Position != TB_2_36->Tag) modify = 1;
	if (TB_2_37->Position != TB_2_37->Tag) modify = 1;
	if (TB_2_38->Position != TB_2_38->Tag) modify = 1;
	if (TB_2_39->Position != TB_2_39->Tag) modify = 1;
	if (TB_2_40->Position != TB_2_40->Tag) modify = 1;
	if (TB_2_41->Position != TB_2_41->Tag) modify = 1;
	if (TB_2_42->Position != TB_2_42->Tag) modify = 1;
	if (TB_2_43->Position != TB_2_43->Tag) modify = 1;
	if (TB_2_44->Position != TB_2_44->Tag) modify = 1;
	if (TB_2_45->Position != TB_2_45->Tag) modify = 1;
	if (TB_2_46->Position != TB_2_46->Tag) modify = 1;
	if (TB_2_47->Position != TB_2_47->Tag) modify = 1;
	if (TB_2_48->Position != TB_2_48->Tag) modify = 1;
	if (TB_2_49->Position != TB_2_49->Tag) modify = 1;
	if (TB_2_50->Position != TB_2_50->Tag) modify = 1;

	// EQ Type3
	if (TB_3_1->Position != TB_3_1->Tag) modify = 1;
	if (TB_3_2->Position != TB_3_2->Tag) modify = 1;
	if (TB_3_3->Position != TB_3_3->Tag) modify = 1;
	if (TB_3_4->Position != TB_3_4->Tag) modify = 1;
	if (TB_3_5->Position != TB_3_5->Tag) modify = 1;
	if (TB_3_6->Position != TB_3_6->Tag) modify = 1;
	if (TB_3_7->Position != TB_3_7->Tag) modify = 1;
	if (TB_3_8->Position != TB_3_8->Tag) modify = 1;
	if (TB_3_9->Position != TB_3_9->Tag) modify = 1;
	if (TB_3_10->Position != TB_3_10->Tag) modify = 1;
	if (TB_3_11->Position != TB_3_11->Tag) modify = 1;
	if (TB_3_12->Position != TB_3_12->Tag) modify = 1;
	if (TB_3_13->Position != TB_3_13->Tag) modify = 1;
	if (TB_3_14->Position != TB_3_14->Tag) modify = 1;
	if (TB_3_15->Position != TB_3_15->Tag) modify = 1;
	if (TB_3_16->Position != TB_3_16->Tag) modify = 1;
	if (TB_3_17->Position != TB_3_17->Tag) modify = 1;
	if (TB_3_18->Position != TB_3_18->Tag) modify = 1;
	if (TB_3_19->Position != TB_3_19->Tag) modify = 1;
	if (TB_3_20->Position != TB_3_20->Tag) modify = 1;
	if (TB_3_21->Position != TB_3_21->Tag) modify = 1;
	if (TB_3_22->Position != TB_3_22->Tag) modify = 1;
	if (TB_3_23->Position != TB_3_23->Tag) modify = 1;
	if (TB_3_24->Position != TB_3_24->Tag) modify = 1;
	if (TB_3_25->Position != TB_3_25->Tag) modify = 1;
	if (TB_3_26->Position != TB_3_26->Tag) modify = 1;
	if (TB_3_27->Position != TB_3_27->Tag) modify = 1;
	if (TB_3_28->Position != TB_3_28->Tag) modify = 1;
	if (TB_3_29->Position != TB_3_29->Tag) modify = 1;
	if (TB_3_30->Position != TB_3_30->Tag) modify = 1;
	if (TB_3_31->Position != TB_3_31->Tag) modify = 1;
	if (TB_3_32->Position != TB_3_32->Tag) modify = 1;
	if (TB_3_33->Position != TB_3_33->Tag) modify = 1;
	if (TB_3_34->Position != TB_3_34->Tag) modify = 1;
	if (TB_3_35->Position != TB_3_35->Tag) modify = 1;
	if (TB_3_36->Position != TB_3_36->Tag) modify = 1;
	if (TB_3_37->Position != TB_3_37->Tag) modify = 1;
	if (TB_3_38->Position != TB_3_38->Tag) modify = 1;
	if (TB_3_39->Position != TB_3_39->Tag) modify = 1;
	if (TB_3_40->Position != TB_3_40->Tag) modify = 1;
	if (TB_3_41->Position != TB_3_41->Tag) modify = 1;
	if (TB_3_42->Position != TB_3_42->Tag) modify = 1;
	if (TB_3_43->Position != TB_3_43->Tag) modify = 1;
	if (TB_3_44->Position != TB_3_44->Tag) modify = 1;
	if (TB_3_45->Position != TB_3_45->Tag) modify = 1;
	if (TB_3_46->Position != TB_3_46->Tag) modify = 1;
	if (TB_3_47->Position != TB_3_47->Tag) modify = 1;
	if (TB_3_48->Position != TB_3_48->Tag) modify = 1;
	if (TB_3_49->Position != TB_3_49->Tag) modify = 1;
	if (TB_3_50->Position != TB_3_50->Tag) modify = 1;
	if (TB_3_51->Position != TB_3_51->Tag) modify = 1;
	if (TB_3_52->Position != TB_3_52->Tag) modify = 1;
	if (TB_3_53->Position != TB_3_53->Tag) modify = 1;
	if (TB_3_54->Position != TB_3_54->Tag) modify = 1;
	if (TB_3_55->Position != TB_3_55->Tag) modify = 1;
	if (TB_3_56->Position != TB_3_56->Tag) modify = 1;
	if (TB_3_57->Position != TB_3_57->Tag) modify = 1;
	if (TB_3_58->Position != TB_3_58->Tag) modify = 1;
	if (TB_3_59->Position != TB_3_59->Tag) modify = 1;
	if (TB_3_60->Position != TB_3_60->Tag) modify = 1;
	if (TB_3_61->Position != TB_3_61->Tag) modify = 1;
	if (TB_3_62->Position != TB_3_62->Tag) modify = 1;
	if (TB_3_63->Position != TB_3_63->Tag) modify = 1;
	if (TB_3_64->Position != TB_3_64->Tag) modify = 1;
	if (TB_3_65->Position != TB_3_65->Tag) modify = 1;
	if (TB_3_66->Position != TB_3_66->Tag) modify = 1;
	if (TB_3_67->Position != TB_3_67->Tag) modify = 1;
	if (TB_3_68->Position != TB_3_68->Tag) modify = 1;
	if (TB_3_69->Position != TB_3_69->Tag) modify = 1;
	if (TB_3_70->Position != TB_3_70->Tag) modify = 1;
	if (TB_3_71->Position != TB_3_71->Tag) modify = 1;
	if (TB_3_72->Position != TB_3_72->Tag) modify = 1;
	if (TB_3_73->Position != TB_3_73->Tag) modify = 1;
	if (TB_3_74->Position != TB_3_74->Tag) modify = 1;
	if (TB_3_75->Position != TB_3_75->Tag) modify = 1;
	if (TB_3_76->Position != TB_3_76->Tag) modify = 1;
	if (TB_3_77->Position != TB_3_77->Tag) modify = 1;
	if (TB_3_78->Position != TB_3_78->Tag) modify = 1;

	// EQ Type4
	if (TB_4_1->Position !=  TB_4_1->Tag) modify = 1;
	if (TB_4_2->Position !=  TB_4_2->Tag) modify = 1;
	if (TB_4_3->Position !=  TB_4_3->Tag) modify = 1;
	if (TB_4_4->Position !=  TB_4_4->Tag) modify = 1;
	if (TB_4_5->Position !=  TB_4_5->Tag) modify = 1;
	if (TB_4_6->Position !=  TB_4_6->Tag) modify = 1;
	if (TB_4_7->Position !=  TB_4_7->Tag) modify = 1;
	if (TB_4_8->Position !=  TB_4_8->Tag) modify = 1;
	if (TB_4_9->Position !=  TB_4_9->Tag) modify = 1;
	if (TB_4_10->Position != TB_4_10->Tag) modify = 1;
	if (TB_4_11->Position != TB_4_11->Tag) modify = 1;
	if (TB_4_12->Position != TB_4_12->Tag) modify = 1;
	if (TB_4_13->Position != TB_4_13->Tag) modify = 1;
	if (TB_4_14->Position != TB_4_14->Tag) modify = 1;
	if (TB_4_15->Position != TB_4_15->Tag) modify = 1;
	if (TB_4_16->Position != TB_4_16->Tag) modify = 1;
	if (TB_4_17->Position != TB_4_17->Tag) modify = 1;
	if (TB_4_18->Position != TB_4_18->Tag) modify = 1;
	if (TB_4_19->Position != TB_4_19->Tag) modify = 1;
	if (TB_4_20->Position != TB_4_20->Tag) modify = 1;
	if (TB_4_21->Position != TB_4_21->Tag) modify = 1;
	if (TB_4_22->Position != TB_4_22->Tag) modify = 1;
	if (TB_4_23->Position != TB_4_23->Tag) modify = 1;
	if (TB_4_24->Position != TB_4_24->Tag) modify = 1;
	if (TB_4_25->Position != TB_4_25->Tag) modify = 1;
	if (TB_4_26->Position != TB_4_26->Tag) modify = 1;
	if (TB_4_27->Position != TB_4_27->Tag) modify = 1;
	if (TB_4_28->Position != TB_4_28->Tag) modify = 1;
	if (TB_4_29->Position != TB_4_29->Tag) modify = 1;
	if (TB_4_30->Position != TB_4_30->Tag) modify = 1;
	if (TB_4_31->Position != TB_4_31->Tag) modify = 1;
	if (TB_4_32->Position != TB_4_32->Tag) modify = 1;
	if (TB_4_33->Position != TB_4_33->Tag) modify = 1;
	if (TB_4_34->Position != TB_4_34->Tag) modify = 1;
	if (TB_4_35->Position != TB_4_35->Tag) modify = 1;
	if (TB_4_36->Position != TB_4_36->Tag) modify = 1;
	if (TB_4_37->Position != TB_4_37->Tag) modify = 1;
	if (TB_4_38->Position != TB_4_38->Tag) modify = 1;
	if (TB_4_39->Position != TB_4_39->Tag) modify = 1;
	if (TB_4_40->Position != TB_4_40->Tag) modify = 1;
	if (TB_4_41->Position != TB_4_41->Tag) modify = 1;
	if (TB_4_42->Position != TB_4_42->Tag) modify = 1;
	if (TB_4_43->Position != TB_4_43->Tag) modify = 1;
	if (TB_4_44->Position != TB_4_44->Tag) modify = 1;
	if (TB_4_45->Position != TB_4_45->Tag) modify = 1;
	if (TB_4_46->Position != TB_4_46->Tag) modify = 1;
	if (TB_4_47->Position != TB_4_47->Tag) modify = 1;
	if (TB_4_48->Position != TB_4_48->Tag) modify = 1;
	if (TB_4_49->Position != TB_4_49->Tag) modify = 1;
	if (TB_4_50->Position != TB_4_50->Tag) modify = 1;
	if (TB_4_51->Position != TB_4_51->Tag) modify = 1;
	if (TB_4_52->Position != TB_4_52->Tag) modify = 1;
	if (TB_4_53->Position != TB_4_53->Tag) modify = 1;
	if (TB_4_54->Position != TB_4_54->Tag) modify = 1;
	if (TB_4_55->Position != TB_4_55->Tag) modify = 1;
	if (TB_4_56->Position != TB_4_56->Tag) modify = 1;
	if (TB_4_57->Position != TB_4_57->Tag) modify = 1;
	if (TB_4_58->Position != TB_4_58->Tag) modify = 1;
	if (TB_4_59->Position != TB_4_59->Tag) modify = 1;
	if (TB_4_60->Position != TB_4_60->Tag) modify = 1;
	if (TB_4_61->Position != TB_4_61->Tag) modify = 1;
	if (TB_4_62->Position != TB_4_62->Tag) modify = 1;
	if (TB_4_63->Position != TB_4_63->Tag) modify = 1;
	if (TB_4_64->Position != TB_4_64->Tag) modify = 1;
	if (TB_4_65->Position != TB_4_65->Tag) modify = 1;
	if (TB_4_66->Position != TB_4_66->Tag) modify = 1;
	if (TB_4_67->Position != TB_4_67->Tag) modify = 1;
	if (TB_4_68->Position != TB_4_68->Tag) modify = 1;
	if (TB_4_69->Position != TB_4_69->Tag) modify = 1;
	if (TB_4_70->Position != TB_4_70->Tag) modify = 1;
	if (TB_4_71->Position != TB_4_71->Tag) modify = 1;
	if (TB_4_72->Position != TB_4_72->Tag) modify = 1;
	if (TB_4_73->Position != TB_4_73->Tag) modify = 1;
	if (TB_4_74->Position != TB_4_74->Tag) modify = 1;
	if (TB_4_75->Position != TB_4_75->Tag) modify = 1;
	if (TB_4_76->Position != TB_4_76->Tag) modify = 1;
	if (TB_4_77->Position != TB_4_77->Tag) modify = 1;
	if (TB_4_78->Position != TB_4_78->Tag) modify = 1;
	if (TB_4_79->Position != TB_4_79->Tag) modify = 1;
	if (TB_4_80->Position != TB_4_80->Tag) modify = 1;
	if (TB_4_81->Position != TB_4_81->Tag) modify = 1;
	if (TB_4_82->Position != TB_4_82->Tag) modify = 1;
	if (TB_4_83->Position != TB_4_83->Tag) modify = 1;
	if (TB_4_84->Position != TB_4_84->Tag) modify = 1;
	if (TB_4_85->Position != TB_4_85->Tag) modify = 1;
	if (TB_4_86->Position != TB_4_86->Tag) modify = 1;
	if (TB_4_87->Position != TB_4_87->Tag) modify = 1;
	if (TB_4_88->Position != TB_4_88->Tag) modify = 1;
	if (TB_4_89->Position != TB_4_89->Tag) modify = 1;
	if (TB_4_90->Position != TB_4_90->Tag) modify = 1;
	if (TB_4_91->Position != TB_4_91->Tag) modify = 1;
	if (TB_4_92->Position != TB_4_92->Tag) modify = 1;
	if (TB_4_93->Position != TB_4_93->Tag) modify = 1;
	if (TB_4_94->Position != TB_4_94->Tag) modify = 1;
	if (TB_4_95->Position != TB_4_95->Tag) modify = 1;
	if (TB_4_96->Position != TB_4_96->Tag) modify = 1;
	if (TB_4_97->Position != TB_4_97->Tag) modify = 1;
	if (TB_4_98->Position != TB_4_98->Tag) modify = 1;

	return modify;
}
void __fastcall TForm1::SetPositionToTag()
{
	TB_1_1->Tag = TB_1_1->Position;
	TB_1_2->Tag = TB_1_2->Position;
	TB_1_3->Tag = TB_1_3->Position;
	TB_1_4->Tag = TB_1_4->Position;
	TB_1_5->Tag = TB_1_5->Position;
	TB_1_6->Tag = TB_1_6->Position;
	TB_1_7->Tag = TB_1_7->Position;
	TB_1_8->Tag = TB_1_8->Position;
	TB_1_9->Tag = TB_1_9->Position;
	TB_1_10->Tag = TB_1_10->Position;
	TB_1_11->Tag = TB_1_11->Position;
	TB_1_12->Tag = TB_1_12->Position;
	TB_1_13->Tag = TB_1_13->Position;
	TB_1_14->Tag = TB_1_14->Position;
	TB_1_15->Tag = TB_1_15->Position;
	TB_1_16->Tag = TB_1_16->Position;
	TB_1_17->Tag = TB_1_17->Position;
	TB_1_18->Tag = TB_1_18->Position;
	TB_1_19->Tag = TB_1_19->Position;
	TB_1_20->Tag = TB_1_20->Position;
	TB_1_21->Tag = TB_1_21->Position;
	TB_1_22->Tag = TB_1_22->Position;
	TB_1_23->Tag = TB_1_23->Position;
	TB_1_24->Tag = TB_1_24->Position;
	TB_1_25->Tag = TB_1_25->Position;
	TB_1_26->Tag = TB_1_26->Position;
	TB_1_27->Tag = TB_1_27->Position;
	TB_1_28->Tag = TB_1_28->Position;
	TB_1_29->Tag = TB_1_29->Position;
	TB_1_30->Tag = TB_1_30->Position;
	TB_1_31->Tag = TB_1_31->Position;
	TB_1_32->Tag = TB_1_32->Position;
	TB_1_33->Tag = TB_1_33->Position;
	TB_1_34->Tag = TB_1_34->Position;
	TB_1_35->Tag = TB_1_35->Position;

		// EQ Type2
	TB_2_1->Tag = TB_2_1->Position;
	TB_2_2->Tag = TB_2_2->Position;
	TB_2_3->Tag = TB_2_3->Position;
	TB_2_4->Tag = TB_2_4->Position;
	TB_2_5->Tag = TB_2_5->Position;
	TB_2_6->Tag = TB_2_6->Position;
	TB_2_7->Tag = TB_2_7->Position;
	TB_2_8->Tag = TB_2_8->Position;
	TB_2_9->Tag = TB_2_9->Position;
	TB_2_10->Tag = TB_2_10->Position;
	TB_2_11->Tag = TB_2_11->Position;
	TB_2_12->Tag = TB_2_12->Position;
	TB_2_13->Tag = TB_2_13->Position;
	TB_2_14->Tag = TB_2_14->Position;
	TB_2_15->Tag = TB_2_15->Position;
	TB_2_16->Tag = TB_2_16->Position;
	TB_2_17->Tag = TB_2_17->Position;
	TB_2_18->Tag = TB_2_18->Position;
	TB_2_19->Tag = TB_2_19->Position;
	TB_2_20->Tag = TB_2_20->Position;
	TB_2_21->Tag = TB_2_21->Position;
	TB_2_22->Tag = TB_2_22->Position;
	TB_2_23->Tag = TB_2_23->Position;
	TB_2_24->Tag = TB_2_24->Position;
	TB_2_25->Tag = TB_2_25->Position;
	TB_2_26->Tag = TB_2_26->Position;
	TB_2_27->Tag = TB_2_27->Position;
	TB_2_28->Tag = TB_2_28->Position;
	TB_2_29->Tag = TB_2_29->Position;
	TB_2_30->Tag = TB_2_30->Position;
	TB_2_31->Tag = TB_2_31->Position;
	TB_2_32->Tag = TB_2_32->Position;
	TB_2_33->Tag = TB_2_33->Position;
	TB_2_34->Tag = TB_2_34->Position;
	TB_2_35->Tag = TB_2_35->Position;
	TB_2_36->Tag = TB_2_36->Position;
	TB_2_37->Tag = TB_2_37->Position;
	TB_2_38->Tag = TB_2_38->Position;
	TB_2_39->Tag = TB_2_39->Position;
	TB_2_40->Tag = TB_2_40->Position;
	TB_2_41->Tag = TB_2_41->Position;
	TB_2_42->Tag = TB_2_42->Position;
	TB_2_43->Tag = TB_2_43->Position;
	TB_2_44->Tag = TB_2_44->Position;
	TB_2_45->Tag = TB_2_45->Position;
	TB_2_46->Tag = TB_2_46->Position;
	TB_2_47->Tag = TB_2_47->Position;
	TB_2_48->Tag = TB_2_48->Position;
	TB_2_49->Tag = TB_2_49->Position;
	TB_2_50->Tag = TB_2_50->Position;

		// EQ Type3
	TB_3_1->Tag = TB_3_1->Position;
	TB_3_2->Tag = TB_3_2->Position;
	TB_3_3->Tag = TB_3_3->Position;
	TB_3_4->Tag = TB_3_4->Position;
	TB_3_5->Tag = TB_3_5->Position;
	TB_3_6->Tag = TB_3_6->Position;
	TB_3_7->Tag = TB_3_7->Position;
	TB_3_8->Tag = TB_3_8->Position;
	TB_3_9->Tag = TB_3_9->Position;
	TB_3_10->Tag = TB_3_10->Position;
	TB_3_11->Tag = TB_3_11->Position;
	TB_3_12->Tag = TB_3_12->Position;
	TB_3_13->Tag = TB_3_13->Position;
	TB_3_14->Tag = TB_3_14->Position;
	TB_3_15->Tag = TB_3_15->Position;
	TB_3_16->Tag = TB_3_16->Position;
	TB_3_17->Tag = TB_3_17->Position;
	TB_3_18->Tag = TB_3_18->Position;
	TB_3_19->Tag = TB_3_19->Position;
	TB_3_20->Tag = TB_3_20->Position;
	TB_3_21->Tag = TB_3_21->Position;
	TB_3_22->Tag = TB_3_22->Position;
	TB_3_23->Tag = TB_3_23->Position;
	TB_3_24->Tag = TB_3_24->Position;
	TB_3_25->Tag = TB_3_25->Position;
	TB_3_26->Tag = TB_3_26->Position;
	TB_3_27->Tag = TB_3_27->Position;
	TB_3_28->Tag = TB_3_28->Position;
	TB_3_29->Tag = TB_3_29->Position;
	TB_3_30->Tag = TB_3_30->Position;
	TB_3_31->Tag = TB_3_31->Position;
	TB_3_32->Tag = TB_3_32->Position;
	TB_3_33->Tag = TB_3_33->Position;
	TB_3_34->Tag = TB_3_34->Position;
	TB_3_35->Tag = TB_3_35->Position;
	TB_3_36->Tag = TB_3_36->Position;
	TB_3_37->Tag = TB_3_37->Position;
	TB_3_38->Tag = TB_3_38->Position;
	TB_3_39->Tag = TB_3_39->Position;
	TB_3_40->Tag = TB_3_40->Position;
	TB_3_41->Tag = TB_3_41->Position;
	TB_3_42->Tag = TB_3_42->Position;
	TB_3_43->Tag = TB_3_43->Position;
	TB_3_44->Tag = TB_3_44->Position;
	TB_3_45->Tag = TB_3_45->Position;
	TB_3_46->Tag = TB_3_46->Position;
	TB_3_47->Tag = TB_3_47->Position;
	TB_3_48->Tag = TB_3_48->Position;
	TB_3_49->Tag = TB_3_49->Position;
	TB_3_50->Tag = TB_3_50->Position;
	TB_3_51->Tag = TB_3_51->Position;
	TB_3_52->Tag = TB_3_52->Position;
	TB_3_53->Tag = TB_3_53->Position;
	TB_3_54->Tag = TB_3_54->Position;
	TB_3_55->Tag = TB_3_55->Position;
	TB_3_56->Tag = TB_3_56->Position;
	TB_3_57->Tag = TB_3_57->Position;
	TB_3_58->Tag = TB_3_58->Position;
	TB_3_59->Tag = TB_3_59->Position;
	TB_3_60->Tag = TB_3_60->Position;
	TB_3_61->Tag = TB_3_61->Position;
	TB_3_62->Tag = TB_3_62->Position;
	TB_3_63->Tag = TB_3_63->Position;
	TB_3_64->Tag = TB_3_64->Position;
	TB_3_65->Tag = TB_3_65->Position;
	TB_3_66->Tag = TB_3_66->Position;
	TB_3_67->Tag = TB_3_67->Position;
	TB_3_68->Tag = TB_3_68->Position;
	TB_3_69->Tag = TB_3_69->Position;
	TB_3_70->Tag = TB_3_70->Position;
	TB_3_71->Tag = TB_3_71->Position;
	TB_3_72->Tag = TB_3_72->Position;
	TB_3_73->Tag = TB_3_73->Position;
	TB_3_74->Tag = TB_3_74->Position;
	TB_3_75->Tag = TB_3_75->Position;
	TB_3_76->Tag = TB_3_76->Position;
	TB_3_77->Tag = TB_3_77->Position;
	TB_3_78->Tag = TB_3_78->Position;

		// EQ Type4
	TB_4_1->Tag = TB_4_1->Position;
	TB_4_2->Tag = TB_4_2->Position;
	TB_4_3->Tag = TB_4_3->Position;
	TB_4_4->Tag = TB_4_4->Position;
	TB_4_5->Tag = TB_4_5->Position;
	TB_4_6->Tag = TB_4_6->Position;
	TB_4_7->Tag = TB_4_7->Position;
	TB_4_8->Tag = TB_4_8->Position;
	TB_4_9->Tag = TB_4_9->Position;
	TB_4_10->Tag = TB_4_10->Position;
	TB_4_11->Tag = TB_4_11->Position;
	TB_4_12->Tag = TB_4_12->Position;
	TB_4_13->Tag = TB_4_13->Position;
	TB_4_14->Tag = TB_4_14->Position;
	TB_4_15->Tag = TB_4_15->Position;
	TB_4_16->Tag = TB_4_16->Position;
	TB_4_17->Tag = TB_4_17->Position;
	TB_4_18->Tag = TB_4_18->Position;
	TB_4_19->Tag = TB_4_19->Position;
	TB_4_20->Tag = TB_4_20->Position;
	TB_4_21->Tag = TB_4_21->Position;
	TB_4_22->Tag = TB_4_22->Position;
	TB_4_23->Tag = TB_4_23->Position;
	TB_4_24->Tag = TB_4_24->Position;
	TB_4_25->Tag = TB_4_25->Position;
	TB_4_26->Tag = TB_4_26->Position;
	TB_4_27->Tag = TB_4_27->Position;
	TB_4_28->Tag = TB_4_28->Position;
	TB_4_29->Tag = TB_4_29->Position;
	TB_4_30->Tag = TB_4_30->Position;
	TB_4_31->Tag = TB_4_31->Position;
	TB_4_32->Tag = TB_4_32->Position;
	TB_4_33->Tag = TB_4_33->Position;
	TB_4_34->Tag = TB_4_34->Position;
	TB_4_35->Tag = TB_4_35->Position;
	TB_4_36->Tag = TB_4_36->Position;
	TB_4_37->Tag = TB_4_37->Position;
	TB_4_38->Tag = TB_4_38->Position;
	TB_4_39->Tag = TB_4_39->Position;
	TB_4_40->Tag = TB_4_40->Position;
	TB_4_41->Tag = TB_4_41->Position;
	TB_4_42->Tag = TB_4_42->Position;
	TB_4_43->Tag = TB_4_43->Position;
	TB_4_44->Tag = TB_4_44->Position;
	TB_4_45->Tag = TB_4_45->Position;
	TB_4_46->Tag = TB_4_46->Position;
	TB_4_47->Tag = TB_4_47->Position;
	TB_4_48->Tag = TB_4_48->Position;
	TB_4_49->Tag = TB_4_49->Position;
	TB_4_50->Tag = TB_4_50->Position;
	TB_4_51->Tag = TB_4_51->Position;
	TB_4_52->Tag = TB_4_52->Position;
	TB_4_53->Tag = TB_4_53->Position;
	TB_4_54->Tag = TB_4_54->Position;
	TB_4_55->Tag = TB_4_55->Position;
	TB_4_56->Tag = TB_4_56->Position;
	TB_4_57->Tag = TB_4_57->Position;
	TB_4_58->Tag = TB_4_58->Position;
	TB_4_59->Tag = TB_4_59->Position;
	TB_4_60->Tag = TB_4_60->Position;
	TB_4_61->Tag = TB_4_61->Position;
	TB_4_62->Tag = TB_4_62->Position;
	TB_4_63->Tag = TB_4_63->Position;
	TB_4_64->Tag = TB_4_64->Position;
	TB_4_65->Tag = TB_4_65->Position;
	TB_4_66->Tag = TB_4_66->Position;
	TB_4_67->Tag = TB_4_67->Position;
	TB_4_68->Tag = TB_4_68->Position;
	TB_4_69->Tag = TB_4_69->Position;
	TB_4_70->Tag = TB_4_70->Position;
	TB_4_71->Tag = TB_4_71->Position;
	TB_4_72->Tag = TB_4_72->Position;
	TB_4_73->Tag = TB_4_73->Position;
	TB_4_74->Tag = TB_4_74->Position;
	TB_4_75->Tag = TB_4_75->Position;
	TB_4_76->Tag = TB_4_76->Position;
	TB_4_77->Tag = TB_4_77->Position;
	TB_4_78->Tag = TB_4_78->Position;
	TB_4_79->Tag = TB_4_79->Position;
	TB_4_80->Tag = TB_4_80->Position;
	TB_4_81->Tag = TB_4_81->Position;
	TB_4_82->Tag = TB_4_82->Position;
	TB_4_83->Tag = TB_4_83->Position;
	TB_4_84->Tag = TB_4_84->Position;
	TB_4_85->Tag = TB_4_85->Position;
	TB_4_86->Tag = TB_4_86->Position;
	TB_4_87->Tag = TB_4_87->Position;
	TB_4_88->Tag = TB_4_88->Position;
	TB_4_89->Tag = TB_4_89->Position;
	TB_4_90->Tag = TB_4_90->Position;
	TB_4_91->Tag = TB_4_91->Position;
	TB_4_92->Tag = TB_4_92->Position;
	TB_4_93->Tag = TB_4_93->Position;
	TB_4_94->Tag = TB_4_94->Position;
	TB_4_95->Tag = TB_4_95->Position;
	TB_4_96->Tag = TB_4_96->Position;
	TB_4_97->Tag = TB_4_97->Position;
	TB_4_98->Tag = TB_4_98->Position;
}
