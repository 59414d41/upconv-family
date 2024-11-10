//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnExitClick(TObject *Sender)
{
	HWND hParent;
	int no_send_msg;

	no_send_msg = 0;
	hParent = FindWindow(NULL, L"Upconv Frontend 1.0.x");
	if (hParent == NULL) {
		hParent = FindWindow(NULL, L"Upconv Frontend 0.8.6 (Test)");
		if (hParent == NULL) {
			hParent = FindWindow(NULL, L"Upconv Frontend 0.8.5");
			if (hParent == NULL) {
				hParent = FindWindow(NULL, L"Upconv Frontend 0.8.4");
				if (hParent == NULL) {
					hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2k");
					if (hParent == NULL) {
						hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2k (x64)");
						if (hParent == NULL) {
							hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2j");
							if (hParent == NULL) {
								hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2j (x64)");
								if (hParent == NULL) {
									hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2i");
									if (hParent == NULL) {
										hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2i (x64)");
										if (hParent == NULL) {
											no_send_msg = 1;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (no_send_msg == 0) {
		PostMessage(hParent, WM_APP + WM_RELOAD_DP, 0, 0);
	}

	Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	int argc;
	int index;
	int work_int;
	wchar_t file_path[2048];
	AnsiString sStr;

	portableMode = 0;
	changePortableMode = 0;
	saveFlag = 0;
	cmbDefaultParameter->ItemIndex = 0;

	argc = ParamCount();
	if (argc == 3) {
		sStr = ParamStr(2);
		if (sscanf(sStr.c_str(),"portable=%d",&work_int) == 1) {
			if (work_int == 0 || work_int == 1) {
				portableMode = work_int;
			}
		}
		sStr = ParamStr(3);
		if (sscanf(sStr.c_str(),"change=%d",&work_int) == 1) {
			if (work_int == 0 || work_int == 1) {
				changePortableMode = work_int;
			}
		}
	}
	if (portableMode == 0 && changePortableMode == 0) {
		//rd_defaultParameterPath = "C:\\ProgramData\\upconv";
		//rd_defaultEQPath = "C:\\ProgramData\\upconv_eq";
		SHGetSpecialFolderPath(Form1->Handle,file_path,CSIDL_COMMON_APPDATA,false);
		rd_defaultParameterPath = file_path;
		rd_defaultParameterPath += L"\\upconv";
		rd_defaultEQPath = file_path;
		rd_defaultEQPath += L"\\upconv_eq";
		wr_defaultParameterPath = rd_defaultParameterPath;
		wr_defaultEQPath        = rd_defaultEQPath;
		btnDelete->Tag = 0;
	} else if (portableMode == 1 && changePortableMode == 0) {
		rd_defaultParameterPath = ExtractFilePath(Application->ExeName);
		rd_defaultEQPath        = ExtractFilePath(Application->ExeName);
		wr_defaultParameterPath = rd_defaultParameterPath;
		wr_defaultEQPath        = rd_defaultEQPath;
		btnDelete->Tag = 0;
	} else if (portableMode == 0 && changePortableMode == 1) {
		rd_defaultParameterPath = ExtractFilePath(Application->ExeName);
		rd_defaultEQPath        = ExtractFilePath(Application->ExeName);
		SHGetSpecialFolderPath(Form1->Handle,file_path,CSIDL_COMMON_APPDATA,false);
		wr_defaultParameterPath = file_path;
		wr_defaultParameterPath += L"\\upconv";
		wr_defaultEQPath = file_path;
		wr_defaultEQPath += L"\\upconv_eq";
		btnDelete->Tag = 1;
	} else if (portableMode == 1 && changePortableMode == 1) {
		SHGetSpecialFolderPath(Form1->Handle,file_path,CSIDL_COMMON_APPDATA,false);
		rd_defaultParameterPath = file_path;
		rd_defaultParameterPath += L"\\upconv";
		rd_defaultEQPath = file_path;
		rd_defaultEQPath += L"\\upconv_eq";
		wr_defaultParameterPath = ExtractFilePath(Application->ExeName);
		wr_defaultEQPath        = ExtractFilePath(Application->ExeName);
		btnDelete->Tag = 1;
	}
	LoadDefaultParameter();
	sStr = ParamStr(1);
	if (sscanf(sStr.c_str(),"param=%d",&index) == 1) {
		if ((index >= 1 && index < cmbDefaultParameter->Items->Count) || (index == cmbDefaultParameter->Items->Count && index == 99)) {
			cmbDefaultParameter->ItemIndex = index - 1;
			btnLoad->Click();
			if (index >= 3) {
				btnSave->Enabled = true;
				if (btnDelete->Tag == 0) {
					btnDelete->Enabled = true;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LoadDefaultParameter(void)
{
	FILE *fp;
	wchar_t file_path[2048];
	UnicodeString uFileName;
	UnicodeString uStr;
	AnsiString sFileName;
	AnsiString sStr,sStr2,sTitle;
	char str[8192];
	char title[8192];
	int error;
	int i,ii;
	char *p,*pp;
	int d1,d2,d3,d4,d5,d6,d7,d8,d9,d10;

	chkPreEQ->Checked = false;
	chkPreEQ->Enabled = false;
	chkPostEQ->Checked = false;
	chkPostEQ->Enabled = false;
	cmbPreEQ->Enabled = false;
	cmbPostEQ->Enabled = false;
	cmbPreEQ->ItemIndex = -1;
	cmbPostEQ->ItemIndex = -1;
	cmbPreEQ->Items->Clear();
	cmbPostEQ->Items->Clear();
	memo_eq_title->Lines->Clear();
	edtParameter->Lines->Clear();
	edtParameter2->Lines->Clear();
	edtPTitle->Lines->Clear();

	for (i = 1;i <= 99;i++) {
		error = 1;
		sStr = L"";
		sTitle = L"";
		uFileName.printf(L"%s\\upconv_default_%02d.txt",rd_defaultParameterPath,i);
		sFileName = (AnsiString)uFileName;
		fp = fopen(sFileName.c_str(),"r");
		if (fp) {
			memset(str,0,8000);
			if (fgets(str,8000,fp) != NULL) {
				p = strrchr(str,'\n');if (p != NULL) *p = '\0';
				sStr = str;
				if (fgets(str,8000,fp) != NULL) {
					p = strrchr(str,'\n');if (p != NULL) *p = '\0';
					edtWorkPath->Text = (UnicodeString)str;
					if (edtWorkPath->Text.Length() > 0) edtWorkPath->RightButton->Visible = true;

					if (fgets(str,8000,fp) != NULL) {
						p = strrchr(str,'\n');if (p != NULL) *p = '\0';
						edtEncorder_FLAC->Text = (UnicodeString)str;

						if (fgets(str,8000,fp) != NULL) {
							p = strrchr(str,'\n');if (p != NULL) *p = '\0';
							edtEncorder_WavPack->Text = (UnicodeString)str;

							if (fgets(str,8000,fp) != NULL) {
								p = strrchr(str,'\n');if (p != NULL) *p = '\0';
								edtEncorder_MP3->Text = (UnicodeString)str;

								if (fgets(str,8000,fp) != NULL) {
									memset(title,0,8000);
									p = strrchr(str,'\n');if (p != NULL) *p = '\0';
									strcpy(title,str);

									if (fgets(str,8000,fp) != NULL) {
										p = strrchr(str,'\n');if (p != NULL) *p = '\0';
										edtEncorder_OPUS->Text = (UnicodeString)str;
									}

									if (fgets(str,8000,fp) != NULL) {
										p = strrchr(str,'\n');if (p != NULL) *p = '\0';
										edtEncorder_M4A->Text = (UnicodeString)str;
									}

									sStr = sStr + "|" + edtEncorder_FLAC->Text + "|" + edtEncorder_WavPack->Text + "|" + edtEncorder_MP3->Text + "|" + edtEncorder_OPUS->Text + "|" + edtEncorder_M4A->Text;
									sTitle= (AnsiString)title;
									error = 0;
								}
							}
						}
					}
				}
			}
			fclose(fp);
		} else {
			break;
		}
		if (error == 0) {
			if (sStr.Length() <= 1000) {
				edtParameter->Lines->Add((UnicodeString)sStr);
				edtParameter2->Lines->Add("");
			} else {
				// sStr の文字数が1000以下になるようにsStr,sStr2に分割する
				char del_c,del_c2;
				sStr2 = "";
				strcpy(str,sStr.c_str());
				pp = NULL;
				del_c  = '\0';
				del_c2 = '\0';
				do {
					if (del_c == '\0') {
						p = strchr(str,'|');
					} else {
						p = strrchr(str,' ');
					}

					if (p != NULL) {
						del_c2 = del_c;
						del_c = *p;
						*p = '\0';
						if (pp != NULL && del_c2 != '\0') {
							*pp = del_c2;
						}
						sStr  = (AnsiString)str;
						sStr2 = (AnsiString)del_c + (AnsiString)(p + 1);
						if (sStr.Length() <= 1000) {
							// 分割できた場合
							break;
						} else {
							pp = p;
						}
					} else {
						// 分割できない場合はやめる
						if (pp != NULL && del_c2 != '\0') {
							*pp = del_c2;
						}
						break;
					}
				} while (1);
				edtParameter->Lines->Add((UnicodeString)sStr);
				edtParameter2->Lines->Add((UnicodeString)sStr2);
			}
			edtPTitle->Lines->Add((UnicodeString)sTitle);
		}
	}

	for (i = 1;i <= 999;i++) {
		sStr = L"";
		uFileName.printf(L"%s\\upconv_eq_%03d.txt",rd_defaultEQPath,i);
		sFileName = (AnsiString)uFileName;
		fp = fopen(sFileName.c_str(),"r");
		if (fp) {
			double dp;
			for (ii = 0;ii < 192000;ii++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
				} else {
					break;
				}
			}
			if (fscanf(fp,"%lf",&dp) == 1) {
				ii++;
				for (;ii < 384000;ii++) {
					if (fscanf(fp,"%lf",&dp) == 1) {
					} else {
						break;
					}
				}
				fgets(str,4096,fp);
				fgets(str,4096,fp);
				fgets(str,4096,fp);
			}
			if (ii == 192000 || ii == 384000) {
//				fgets(str,4096,fp);
				if (fgets(str,4096,fp) != NULL) {					/* Active(EQ) */
					if (fgets(str,4096,fp) != NULL) {   			/* EQ(Type1) */
						if (fgets(str,4096,fp) != NULL) {			/* EQ(Type2) */
							if (fgets(str,4096,fp) != NULL) {		/* EQ(Type3) */
								if (fgets(str,4096,fp) != NULL) {	/* Title */
									p = strrchr(str,'\n');if (p) *p = '\0';
									sStr.printf("%03d.%s",i,str);
									memo_eq_title->Lines->Add(sStr);
									sStr.printf("%03d",i);
									cmbPreEQ->Items->Add(sStr);
									cmbPostEQ->Items->Add(sStr);
								}
							}
						}
					}
				}
			}
			fclose(fp);
		}
	}
	if (cmbPreEQ->Items->Count > 0) {
		chkPreEQ->Enabled = true;
		cmbPreEQ->Enabled = true;
	}
	if (cmbPostEQ->Items->Count > 0) {
		chkPostEQ->Enabled = true;
		cmbPostEQ->Enabled = true;
	}

	for (i = edtParameter->Lines->Count - 1;i >= 0;i--) {
		uStr = edtParameter->Lines->Strings[i];
		uStr = uStr.Trim();
		if (uStr.Length() == 0) {
			edtParameter->Lines->Delete(i);
		}
	}

	for (i = 0;i < edtPTitle->Lines->Count;i++) {
		uStr.printf(L"%d",i + 1);
		if (edtParameter->Lines->Strings[i].Length() == 0) break;
		cmbDefaultParameter->Items->Add(uStr);
	}
	uStr.printf(L"%d",i + 1);
	cmbDefaultParameter->Items->Add(uStr);
	uFileName.printf(L"%s\\preset.dat",rd_defaultParameterPath);
	sFileName = (AnsiString)uFileName;
	fp = fopen(sFileName.c_str(),"r");
	if (fp) {
		for (i = 1;i <= 10;i++) {
			if (fgets(str,4000,fp) != NULL) {
				if (sscanf(str,"%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d",title,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&d8,&d9) == 10) {
					p = strrchr(str,'\n');if (p != NULL) *p = '\0';
					edtHFA_Preset->Lines->Add((UnicodeString)str);
				}
			}
		}
		fclose(fp);
	}
	LoadEncoderCustomOption(-1);
}
void __fastcall TForm1::btnLoadClick(TObject *Sender)
{
	int index;

	index = cmbDefaultParameter->Text.ToInt();
	ParseDefaultParameter(index);
	LoadEncoderCustomOption(0);
	if (cmbEncoderCustom->Items->Count > 0) {
		cmbEncoderCustom->ItemIndex = 0;
	}

}
//---------------------------------------------------------------------------
void __fastcall TForm1::ParseDefaultParameter(int index)
{
	UnicodeString uStr,uStr2;
	UnicodeString uTitle;
	AnsiString    sStr;
	char work[8192];
	char title[8192];
	char *p,*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
	int i,ii,d1,d2,d3,d4,d5,d6;
	int real_index;
	double LRC_CC_Phase,LRC_CC_Level;
	int fft_window_lookup[]={1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11};
	int fft_lvl_lookup[]={1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5};

	edtEncorder_FLAC->Text = L"-s -f --best";
	edtEncorder_WavPack->Text = L"-hh -q";
	edtEncorder_MP3->Text = L"-V 0 -b 256";
	edtEncorder_OPUS->Text = L"--vbr";
	edtEncorder_M4A->Text  = L"-b:a 256k";

	uStr  = edtParameter->Lines->Strings[index - 1];
	uStr2 = edtParameter2->Lines->Strings[index - 1];
	if (uStr2.Length() > 0) {
		uStr = uStr + uStr2;
	}
	sStr = (AnsiString)uStr;
	strcpy(work,sStr.c_str());
	p1 = work;
	p2 = NULL;
	p3 = NULL;
	p4 = NULL;
	p5 = NULL;
	p6 = NULL;
	p2 = strchr(work,(int)'|');
	if (p2 != NULL) {
		*p2 = '\0';p2++;
		p3 = strchr(p2,(int)'|');
		if (p3 != NULL) {
			*p3 = '\0';p3++;
			p4 = strchr(p3,(int)'|');
			if (p4 != NULL) {
				*p4 = '\0';p4++;
				p5 = strchr(p4,(int)'|');
				if (p5 != NULL) {
					*p5 = '\0';p5++;
					p6 = strchr(p5,(int)'|');
					if (p6 != NULL) {
						*p6 = '\0';p6++;
//						p7 = strchr(p6,(int)'|');
//						if (p7 != NULL) {
//							*p7 = '\0';p7++;
//						}
					}
				}
			}
		}
	}
	if (p2 != NULL) {
		edtEncorder_FLAC->Text = (UnicodeString)p2;
	}
	if (p3 != NULL) {
		edtEncorder_WavPack->Text = (UnicodeString)p3;
	}
	if (p4 != NULL) {
		edtEncorder_MP3->Text = (UnicodeString)p4;
	}
	if (p5 != NULL) {
		edtEncorder_OPUS->Text = (UnicodeString)p5;
	}
	if (p6 != NULL) {
		edtEncorder_M4A->Text = (UnicodeString)p6;
	}
	uStr = edtPTitle->Lines->Strings[index - 1];
	if (uStr != "") {
		uTitle = uStr;
	} else {
		uTitle = L"";
	}

	edtTitle->Text = uTitle;
	chkABE->Checked = false;
	chkPostABE->Checked = false;
	chkPostNR->Checked  = false;
	chkCutHighPassDither->Checked = false;
	chkLowAdjust->Checked = false;
	chkHFC_Auto->Checked  = false;
	chk_LRC_process->Checked = false;
	cmbOverSamp->ItemIndex = 0;
	chkPreEQ->Checked = false;
	chkPostEQ->Checked = false;
	cmbPreEQ->ItemIndex = -1;
	cmbPostEQ->ItemIndex = -1;
	chkEnableHFC->Checked = false;
	edtHFC->Clear();
	chkEnableLFC->Checked = false;
	edtLFC->Clear();
	chkNREnable->Checked = false;
	edtNR_Freq->Clear();
	upD_NR_Level->Position = 1;
	cmbOutputDither->ItemIndex = 0;
	UpD_OutLv->Position = 1;
	rdoDeE1->Checked = true;
	chkABE_CutLowLevel->Checked = false;
	chkSmoothingLowLevel->Checked = false;
	UpD_ABE_CutDitherLv->Position = 0;
	rdoABE_0774->Checked  = true;
	rdoABE_080->Checked   = false;
	rdoABE_086->Checked   = false;
	chkABE_DeClip->Checked = false;
	chkAdFilter->Checked = false;
	edtCLLD_Level->Enabled = false;
	UpD_CLLD_Level->Enabled = false;
	chkClickNoiseReduction->Checked = false;
	UpD_CLLD_Level->Position = 8;
	cmbHFA_Level->ItemIndex = 0;
	rdoHFA3_0774->Checked = true;
	rdoHFA3_082->Checked  = false;
	rdoHFA3_084->Checked  = false;
	cmbHFC_AutoAdjust->ItemIndex = 0;
	UpDHFA_filter1->Position = 6;
	UpDHFA_filter2->Position = 3;
	UpDHFA_filter3->Position = 5;
	UpDHFA_filter4->Position = 5;
	cmbHFA_Preset->ItemIndex = 0;
	cmbVolumeLevelAdjust->ItemIndex = 0;
	track_hfa3_ala->Position = 0;
	upd_hfa_level_adj_width->Position = 2000;
	upd_hfa_level_adj_nx->Position = 95;
	tb_hfa3_sig_level_limit->Position = 12;
	upd_hfa_level_adj_width_LRC->Position = 2000;
	upd_hfa_level_adj_nx_LRC->Position = 95;
	tb_hfa3_sig_level_limit_LRC->Position = 12;
	chkMC_Center->Checked = false;
	chkMC_Surround->Checked = false;
	chkMC_LFE->Checked = false;
	chkMC_Mono->Checked = false;
	chkMC_DownMix->Checked = false;
	chkMC_EchoEffect->Checked = false;
	UpDMC_EchoEffect->Position = 1;
	chkSuffix->Checked = false;
	edtSuffix->Text = L"upconv";

	chkConvOptionOverWrite->Checked = false;
	chkHFC_OverWrite->Checked       = false;
	chkNormalizeOverWrite->Checked  = false;
	cmbSamplingOverWrite->ItemIndex = 0;
	cmbBitWidthOverWrite->ItemIndex = 0;
	cmbHFA->ItemIndex               = 0;
	cmbNormalize->ItemIndex         = 0;
	chk_ms_process->Checked = false;
	cmbMidSide->ItemIndex = 0;

	cmb_HFA3_Freq_Start->ItemIndex = 6;
	cmb_HFA3_Freq_Len->ItemIndex = 3;
	edt_hfa3_ana_step->Text = 273;
	rdo_hfa3_pp_3->Checked = true;

	cmb_HFA3_Freq_Start_LRC->ItemIndex = 6;
	cmb_HFA3_Freq_Len_LRC->ItemIndex = 3;
	edt_hfa3_ana_step_LRC->Text = 273;
	rdo_hfa3_pp_3_LRC->Checked = true;
	cmb_LRC_CC_Phase->ItemIndex = 17;
	cmb_LRC_CC_Level->ItemIndex = 11;

	cmbHFA3_FFT_Window_Size->ItemIndex = 0;
	cmbHFA3_FFT_Window_Lvl->ItemIndex = 0;

	chkEnableCenterHFC->Checked = false;
	edtCenterHFC->Text = "";

//	UpD_WltCutLevel->Position = 2;
	UpD_CutLevel->Position = 10;
	cmbCutMode->ItemIndex = 0;

	p1 = work;
	p2 = strchr(p1,(int)' ');
	for (;p1 != NULL;) {
		if (p2 != NULL) {
			*p2 = '\0';
		}
		// Upconv
		if (strcmp(p1,"-abe") == 0) chkABE->Checked = true;
		if (strcmp(p1,"-post_abe") == 0) chkPostABE->Checked = true;
		if (strcmp(p1,"-post_nr") == 0)  chkPostNR->Checked  = true;
		if (strcmp(p1,"-cut_high_dither") == 0) chkCutHighPassDither->Checked = true;
		if (strcmp(p1,"-low_adjust") == 0) chkLowAdjust->Checked = true;
		if (strcmp(p1,"-hfc_auto") == 0) chkHFC_Auto->Checked  = true;
		if (strcmp(p1,"-LRC_process:1") == 0) chk_LRC_process->Checked = true;
		if (strcmp(p1,"-MS_LRC_mode:1") == 0) cmb_MS_LRC->ItemIndex = 0;
		if (strcmp(p1,"-MS_LRC_mode:2") == 0) cmb_MS_LRC->ItemIndex = 1;

		if (sscanf(p1,"-oversamp:%d",&d1) == 1 && d1 >= 0 && d1 <= 6) {
			if (d1 == 0) {
				cmbOverSamp->ItemIndex = 0;
			} else if (d1 == 1) {
				cmbOverSamp->ItemIndex = 1;
			} else if (d1 == 2) {
				cmbOverSamp->ItemIndex = 3;
			} else if (d1 == 3) {
				cmbOverSamp->ItemIndex = 5;
			} else if (d1 == 4) {
				cmbOverSamp->ItemIndex = 2;
			} else if (d1 == 5) {
				cmbOverSamp->ItemIndex = 4;
			} else if (d1 == 6) {
				cmbOverSamp->ItemIndex = 6;
			}
		}
		if (sscanf(p1,"-pre_eq:%d",&d1) == 1 && (d1 == -1 || (d1 >= 0 && d1 < 1000))) {
			if (d1 >= 0) {
				int n;
				for (i = 0;i < memo_eq_title->Lines->Count;i++) {
					sStr = memo_eq_title->Lines->Strings[i];
					if (sscanf(sStr.c_str(),"%d.%[^\n]",&n,title) == 2) {
						if (d1 == n) {
							cmbPreEQ->ItemIndex = i;
							edt_PreEQ_Title->Text = (UnicodeString)title;
							break;
						}
					}
				}
			}
		}
		if (sscanf(p1,"-enable_pre_eq:%d",&d1) == 1 && d1 == 1) chkPreEQ->Checked = true;

		if (sscanf(p1,"-post_eq:%d",&d1) == 1 && (d1 == -1 || (d1 >= 0 && d1 < 1000))) {
			if (d1 >= 0) {
				int n;
				for (i = 0;i < memo_eq_title->Lines->Count;i++) {
					sStr = memo_eq_title->Lines->Strings[i];
					if (sscanf(sStr.c_str(),"%d.%[^\n]",&n,title) == 2) {
						if (d1 == n) {
							cmbPostEQ->ItemIndex = i;
							edt_PostEQ_Title->Text = (UnicodeString)title;
							break;
						}
					}
				}
			}
		}
		if (sscanf(p1,"-enable_post_eq:%d",&d1) == 1 && d1 == 1) chkPostEQ->Checked = true;

		if (sscanf(p1,"-enable_hfc:%d",&d1) == 1 && d1 >= 0 && d1 <= 1) chkEnableHFC->Checked = d1;
		if (sscanf(p1,"-enable_lfc:%d",&d1) == 1 && d1 >= 0 && d1 <= 1) chkEnableLFC->Checked = d1;
		if (sscanf(p1,"-hfc:%d",&d1) == 1 && d1 >= 1000) edtHFC->Text = d1;
		if (sscanf(p1,"-lfc:%d",&d1) == 1 && d1 >= 20) edtLFC->Text = d1;
		if (sscanf(p1,"-enable_nr:%d",&d1) == 1 && d1 >= 0 && d1 <= 1 && d1 == 1) chkNREnable->Checked = true;
		if (sscanf(p1,"-nr:%d",&d1) == 1 && d1 >= 100 && d1 <= 20000) edtNR_Freq->Text = d1;
		if (sscanf(p1,"-nr_option:%d",&d1) == 1 && d1 >= 1 && d1 <= 5) upD_NR_Level->Position = d1;
		if (sscanf(p1,"-output_dither:%d",&d1) == 1 && d1 >= 0 && d1 <= 3) cmbOutputDither->ItemIndex = d1;
		if (sscanf(p1,"-output_dither_option:%d",&d1) == 1 && d1 >= 0 && d1 <= 16) UpD_OutLv->Position = d1;
		if (sscanf(p1,"-abe_option:%d,%d,%d",&d1,&d2,&d3) == 3 && (d1 >= 0 && d1 <= 1) && (d2 >= 0 && d2 <= 1) && (d3 >= 0 && d3 <= 5)) {
			if (d1) chkABE_CutLowLevel->Checked = true;
			if (d2) chkSmoothingLowLevel->Checked = true;
			UpD_ABE_CutDitherLv->Position = d3;
		}
		if (sscanf(p1,"-abe_version:%d",&d1) == 1 && (d1 >= 1 && d1 <= 4)) {
			if (d1 == 1) {
				rdoABE_0774->Checked = true;
				edtCLLD_Level->Enabled = false;
				UpD_CLLD_Level->Enabled = false;
				track_hfa3_ala->Enabled = false;
				track_hfa3_ala->SliderVisible = false;
			}
			if (d1 == 2) rdoABE_080->Checked  = true;
			if (d1 == 3) rdoABE_086->Checked  = true;
			if (d1 == 4) rdoABE_10x->Checked  = true;
		}
		if (sscanf(p1,"-abe_declip:%d",&d1) == 1 && (d1 >= 0 && d1 <= 1)) {
			if (d1 == 1) chkABE_DeClip->Checked = true;
		}
		if (sscanf(p1,"-abe_declick:%d",&d1) == 1 && (d1 >= 0 && d1 <= 1)) {
			if (d1 == 1) chkClickNoiseReduction->Checked = true;
		}
		if (sscanf(p1,"-abe_ad_filter:%d",&d1) == 1 && (d1 >= 0 && d1 <= 1)) {
			if (d1 == 1) chkAdFilter->Checked = true;
		}

//		if (sscanf(p1,"-abe_10x_wlt_level:%d",&d1) == 1 && (d1 >= 0 && d1 <= 5)) {
//			UpD_WltCutLevel->Position = d1;
//		}
		if (sscanf(p1,"-abe_10x_cut_level:%d",&d1) == 1 && (d1 >= 0 && d1 <= 20)) {
			UpD_CutLevel->Position = d1;
		}
		if (sscanf(p1,"-abe_10x_cut_mode:%d",&d1) == 1 && (d1 >= 0 && d1 <= 2)) {
			cmbCutMode->ItemIndex = d1;
		}

		if (sscanf(p1,"-abe_clld_level:%d",&d1) == 1 && (d1 >= 1 && d1 <= 10)) {
			UpD_CLLD_Level->Position = d1;
		}

		if (sscanf(p1,"-hfc_auto_adjust:%d",&d1) == 1 && (d1 >= 0 && d1 <= 9)) {
			cmbHFC_AutoAdjust->ItemIndex = d1;
		}
		if (sscanf(p1,"-hfa3_analysis_limit_adjust:%d",&d1) == 1 && (d1 >= -4 && d1 <= 4)) {
			track_hfa3_ala->Position = d1;
		}
		if (sscanf(p1,"-hfa_level_adj_width:%d",&d1) == 1 && (d1 >= 1000 && d1 <= 4000)) {
			upd_hfa_level_adj_width->Position = d1;
		}
		if (sscanf(p1,"-hfa_level_adj_nx:%d",&d1) == 1 && (d1 >= 10 && d1 <= 100)) {
			upd_hfa_level_adj_nx->Position = d1;
		}
		if (sscanf(p1,"-hfa3_sig_level_limit:%d",&d1) == 1 && (d1 == 0 || (d1 >= 10  && d1 <= 16))) {
			tb_hfa3_sig_level_limit->Position = d1;
		}
		if (sscanf(p1,"-hfa3_pp:%d",&d1) == 1 && (d1 >= 1 && d1 <= 2)) {
			if (d1 == 1) {
				rdo_hfa3_pp_1->Checked = true;
			} else {
				rdo_hfa3_pp_2->Checked = true;
			}
		}
		if (sscanf(p1,"-hfa3_fft_window_size:%d",&d1) == 1 && (d1 == 0 || d1 >= 10 && d1 <= 30)) {
			if (d1 == 0) {
				cmbHFA3_FFT_Window_Size->ItemIndex = 0;
			} else {
				cmbHFA3_FFT_Window_Size->ItemIndex = fft_window_lookup[d1 - 10];
			}
		}
		if (sscanf(p1,"-hfa3_fft_window_lvl:%d",&d1) == 1 && (d1 == 0 || d1 >= 4 && d1 <= 20)) {
			if (d1 == 0) {
				cmbHFA3_FFT_Window_Lvl->ItemIndex = 0;
			} else {
				cmbHFA3_FFT_Window_Lvl->ItemIndex = fft_lvl_lookup[d1 - 4];
			}
		}

		if (sscanf(p1,"-deE:%d",&d1) == 1 && d1 >= 0 && d1 <= 2) {
			if (d1 == 0) rdoDeE1->Checked = true;
			if (d1 == 1) rdoDeE2->Checked = true;
			if (d1 == 2) rdoDeE3->Checked = true;
		}
		if (sscanf(p1,"-ovwrite_s:%d",&d1) == 1 && d1 >= 0 && d1 <= 10) {
			cmbSamplingOverWrite->ItemIndex = d1;
		}
		if (sscanf(p1,"-ovwrite_w:%d",&d1) == 1 && d1 >= 0 && d1 <= 3) {
			cmbBitWidthOverWrite->ItemIndex = d1;
		}
		if (sscanf(p1,"-enable_ovwrite_sw:%d",&d1) == 1 && d1 >= 0 && d1 <= 1) {
			if (d1 == 1) chkConvOptionOverWrite->Checked = true;
		}
		if (sscanf(p1,"-ovwrite_hfa:%d",&d1) == 1 && d1 >= 0 && d1 <= 3) {
			cmbHFA->ItemIndex = d1;
		}
		if (sscanf(p1,"-enable_ovwrite_hfa:%d",&d1) == 1 && d1 >= 0 && d1 <= 1) {
			if (d1 == 1) chkHFC_OverWrite->Checked = true;
		}
		if (sscanf(p1,"-ovwrite_norm:%d",&d1) == 1 && d1 >= 0 && d1 <= 2) {
			cmbNormalize->ItemIndex = d1;
		}
		if (sscanf(p1,"-enable_ovwrite_norm:%d",&d1) == 1 && d1 >= 0 && d1 <= 1) {
			if (d1 == 1) chkNormalizeOverWrite->Checked = true;
		}
		if (sscanf(p1,"-ms_process:%d",&d1) == 1 && d1 >= 1 && d1 <= 10) {
			chk_ms_process->Checked = true;
			cmbMidSide->ItemIndex = d1 - 1;
		}
		if (sscanf(p1,"-ms_process_save:%d",&d1) == 1 && d1 >= 1 && d1 <= 10) {
			chk_ms_process->Checked = false;
			cmbMidSide->ItemIndex = d1 - 1;
		}

		// HFA2/3 Option
		if (sscanf(p1,"-hfa_preset:%d",&d1) == 1 && d1 >= 0 && d1 <= 10) {
			if (d1 == 0) d1 = 1;
			cmbHFA_Preset->ItemIndex = d1 - 1;
			ChangeHFAPreset();
		}
		if (sscanf(p1,"-hfa3_option:%d",&d1) == 1 && d1 >= 1 && d1 <= 3) {
			if (d1 == 1) rdoHFA3_0774->Checked = true;
			if (d1 == 2) rdoHFA3_082->Checked  = true;
			if (d1 == 3) rdoHFA3_084->Checked  = true;
		}
		if (sscanf(p1,"-hfa_filter:%d,%d,%d,%d",&d1,&d2,&d3,&d4) == 4 && (d1 >= 1 && d1 <= 9) && (d2 >= 1 && d2 <= 9) && (d3 >= 1 && d3 <= 9) && (d4 >= 1 && d4 <= 9)) {
			UpDHFA_filter1->Position = d1;
			UpDHFA_filter2->Position = d2;
			UpDHFA_filter3->Position = d3;
			UpDHFA_filter4->Position = d4;
		}
		if (rdoHFA3_0774->Checked) {
			edtHFA_Filter1->Enabled = false;
			edtHFA_Filter2->Enabled = false;
			edtHFA_Filter3->Enabled = false;
			edtHFA_Filter4->Enabled = false;
		}
		if (sscanf(p1,"-hfa_level:%d",&d1) == 1 && d1 >= 1 && d1 <= 6) {
			cmbHFA_Level->ItemIndex = d1;
		}
		if (sscanf(p1,"-hfa3_freq_start:%d",&d1) == 1 && d1 >= 4000 && d1 <= 12000) {
			int iii;
			for (iii = 0;iii < cmb_HFA3_Freq_Start->Items->Count;iii++) {
				if (cmb_HFA3_Freq_Start->Items->Strings[iii].ToInt() == d1) break;
			}
			if (iii < cmb_HFA3_Freq_Start->Items->Count) {
				cmb_HFA3_Freq_Start->ItemIndex = iii;
			}
		}
		if (sscanf(p1,"-hfa3_freq_len:%d",&d1) == 1 && d1 >= 1500 && d1 <= 4000) {
			int iii;
			for (iii = 0;iii < cmb_HFA3_Freq_Len->Items->Count;iii++) {
				if (cmb_HFA3_Freq_Len->Items->Strings[iii].ToInt() == d1) break;
			}
			if (iii < cmb_HFA3_Freq_Len->Items->Count) {
				cmb_HFA3_Freq_Len->ItemIndex = iii;
			}
		}
		if (sscanf(p1,"-hfa3_analyze_step:%d",&d1) == 1 && d1 >= 70 && d1 <= 300) {
			upd_hfa3_ana_step->Position = d1;
		}

		if (sscanf(p1,"-hfa3_freq_start_LRC:%d",&d1) == 1 && d1 >= 4000 && d1 <= 12000) {
			int iii;
			for (iii = 0;iii < cmb_HFA3_Freq_Start_LRC->Items->Count;iii++) {
				if (cmb_HFA3_Freq_Start_LRC->Items->Strings[iii].ToInt() == d1) break;
			}
			if (iii < cmb_HFA3_Freq_Start_LRC->Items->Count) {
				cmb_HFA3_Freq_Start_LRC->ItemIndex = iii;
			}
		}
		if (sscanf(p1,"-hfa3_freq_len_LRC:%d",&d1) == 1 && d1 >= 1500 && d1 <= 4000) {
			int iii;
			for (iii = 0;iii < cmb_HFA3_Freq_Len_LRC->Items->Count;iii++) {
				if (cmb_HFA3_Freq_Len_LRC->Items->Strings[iii].ToInt() == d1) break;
			}
			if (iii < cmb_HFA3_Freq_Len_LRC->Items->Count) {
				cmb_HFA3_Freq_Len_LRC->ItemIndex = iii;
			}
		}
		if (sscanf(p1,"-hfa3_analyze_step_LRC:%d",&d1) == 1 && d1 >= 70 && d1 <= 300) {
			upd_hfa3_ana_step_LRC->Position = d1;
		}

		if (sscanf(p1,"-hfa_level_adj_width_LRC:%d",&d1) == 1 && (d1 >= 1000 && d1 <= 4000)) {
			upd_hfa_level_adj_width_LRC->Position = d1;
		}

		if (sscanf(p1,"-hfa_level_adj_nx_LRC:%d",&d1) == 1 && (d1 >= 10 && d1 <= 100)) {
			upd_hfa_level_adj_nx_LRC->Position = d1;
		}
		if (sscanf(p1,"-hfa3_sig_level_limit_LRC:%d",&d1) == 1 && (d1 == 0 || (d1 >= 10  && d1 <= 16))) {
			tb_hfa3_sig_level_limit_LRC->Position = d1;
		}
		if (sscanf(p1,"-hfa3_pp_LRC:%d",&d1) == 1 && (d1 >= 1 && d1 <= 2)) {
			if (d1 == 1) {
				rdo_hfa3_pp_1_LRC->Checked = true;
			} else {
				rdo_hfa3_pp_2_LRC->Checked = true;
			}
		}
		if (sscanf(p1,"-hfa3_LRC_CC_Phase:%lf",&LRC_CC_Phase) == 1) {
			Memo1->Lines->Add("hfa3_LRC_CC_Phase");
			Memo1->Lines->Add(LRC_CC_Phase);
			if (LRC_CC_Phase >= 0.01 && LRC_CC_Phase <= 0.35) {
				Memo1->Lines->Add("-hfa3_LRC_CC_Phase for");
				for (i = 0;i < cmb_LRC_CC_Phase->Items->Count;i++) {
					Memo1->Lines->Add(cmb_LRC_CC_Phase->Items->Strings[i] + ":" + LRC_CC_Phase);
					uStr.printf(L"%.02lf",LRC_CC_Phase);
					if (cmb_LRC_CC_Phase->Items->Strings[i] == uStr) break;
				}
				if (i < cmb_LRC_CC_Phase->Items->Count) {
					cmb_LRC_CC_Phase->ItemIndex = i;
				} else {
					cmb_LRC_CC_Phase->ItemIndex = 17;
				}
			} else {
				Memo1->Lines->Add("else");
			}
		}

		if (sscanf(p1,"-hfa3_LRC_CC_Level:%lf",&LRC_CC_Level) == 1) {
			Memo1->Lines->Add("-hfa3_LRC_CC_Level sscanf");
			Memo1->Lines->Add(LRC_CC_Level);
			if (LRC_CC_Level >= 0.75 && LRC_CC_Level <= 1.00) {
				for (i = 0;i < cmb_LRC_CC_Level->Items->Count;i++) {
					uStr.printf(L"%.02lf",LRC_CC_Level);
					if (cmb_LRC_CC_Level->Items->Strings[i] == uStr) break;
				}
				if (i < cmb_LRC_CC_Level->Items->Count) {
					cmb_LRC_CC_Level->ItemIndex = i;
				} else {
					cmb_LRC_CC_Level->ItemIndex = 11;
				}
			} else {
				Memo1->Lines->Add("else");
			}
		}

		if (sscanf(p1,"-enable_center_hfc:%d",&d1) == 1 && (d1 == 0 || d1 == 1)) {
			if (d1 == 1) {
				chkEnableCenterHFC->Checked = true;
			}
		}
		if (sscanf(p1,"-center_hfc:%d",&d1) == 1 && d1 >= 1000) {
			edtCenterHFC->Text = d1;
		}

		if (strcmp(p1,"-vla:auto") == 0) {
			cmbVolumeLevelAdjust->ItemIndex = 11;
		} else if (sscanf(p1,"-vla:%d",&d1) == 1 && d1 >= 0 && d1 <= 10) {
			cmbVolumeLevelAdjust->ItemIndex = d1;
		}

		// Multi Channel
		if (strcmp(p1,"-C") == 0) {
			chkMC_Center->Checked = true;
		}
		if (strcmp(p1,"-SLR") == 0) {
			chkMC_Surround->Checked = true;
		}
		if (strcmp(p1,"-LFE") == 0) {
			chkMC_LFE->Checked = true;
		}
		if (sscanf(p1,"-MC_Option:%d,%d,%d,%d",&d1,&d2,&d3,&d4) == 4 && (d1 >= 0 && d1 <= 1) && (d2 >= 0 && d2 <= 1) && (d3 >= 0 && d3 <= 1) && (d4 >= 1 && d4 <= 5)) {
			if (d1) chkMC_Mono->Checked = true;
			if (d2) chkMC_DownMix->Checked = true;
			if (d3) chkMC_EchoEffect->Checked = true;
			UpDMC_EchoEffect->Position = d4;
		}
		if (sscanf(p1,"-enable_suffix:%d",&d1) == 1 && d1 >= 0 && d1 <= 1) {
			if (d1 == 1) chkSuffix->Checked = true;
		}
		if (sscanf(p1,"-suffix:%[^| ]",work) == 1) {
			if (strlen(p1) > 0) edtSuffix->Text = work;
		}

		if (p2 == NULL) break;
		p1 = p2 + 1;
		p2 = strchr(p1,(int)' ');
	}

}
void __fastcall TForm1::ChangeHFAPreset(void)
{
	UnicodeString uStr;
	AnsiString    sStr;
	char title[4096];
	int d1,d2,d3,d4,d5,d6,d7,d8,d9;
	int index;

	index = cmbHFA_Preset->ItemIndex;
	if (index >= 0 && index < 10) {
		uStr = edtHFA_Preset->Lines->Strings[index];
		sStr = (AnsiString)uStr;
		chkHFA_Sig1_Enable->Checked = false;
		chkHFA_Sig2_Enable->Checked = false;
		chkHFA_Sig3_Enable->Checked = false;
		if (sscanf(sStr.c_str(),"%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d",title,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&d8,&d9) == 10) {
			if (d1 == 1) chkHFA_Sig1_Enable->Checked = true;
			if (d2 >= 1 && d2 <= 25) UpD_HFASig1_LineX->Position = d2;
			if (d3 >= -44 && d3 <= 44) UpD_HFA_Sig1Phase->Position = d3;
			if (d4 == 1) chkHFA_Sig2_Enable->Checked = true;
			if (d5 >= -44 && d5 <= 44) UpD_HFA_Sig2Phase->Position = d5;
			if (d8 == 1) chkHFA_Sig3_Enable->Checked = true;
			if (d9 >= -44 && d9 <= 44) UpD_HFA_Sig3Phase->Position = d9;
			if (d7 >= 0 && d7 <= 100) UpDHFA_NoiseBlend->Position  = d7;
			edtHFA_PresetTitle->Text = (UnicodeString)title;
		}
	}
}

void __fastcall TForm1::cmbHFA_PresetChange(TObject *Sender)
{
	ChangeHFAPreset();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnHFA_PresetSaveClick(TObject *Sender)
{
	UnicodeString uStr;
	int index = cmbHFA_Preset->ItemIndex;
	int d1,d2,d3,d4,d5,d6,d7,d8,d9;
	d1 = chkHFA_Sig1_Enable->Checked == true ? 1 : 0;
	d2 = edtHFA_Sig1_LineX->Text.ToInt();
	d3 = edtHFA_Sig1_Phase->Text.ToInt();
	d4 = chkHFA_Sig2_Enable->Checked == true ? 1 : 0;
	d5 = UpD_HFA_Sig2Phase->Position;
	d6 = 0;
	d7 = UpDHFA_NoiseBlend->Position;
	d8 = chkHFA_Sig3_Enable->Checked == true ? 1 : 0;
	d9 = UpD_HFA_Sig3Phase->Position;

	uStr.printf(L"%s,%d,%d,%d,%d,%d,%d,%d,%d,%d",edtHFA_PresetTitle->Text,d1,d2,d3,d4,d5,d6,d7,d8,d9);
	edtHFA_Preset->Lines->Strings[index] = uStr;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	FILE *fp;
	UnicodeString uFileName;
	AnsiString sFileName;
	AnsiString sStr,sStr2;
	AnsiString sTitle;
	char work[8192],title[8192];
	char *p;

	int i;

	if (saveFlag) {
		for (i = 1;i <= 99;i++) {
			uFileName.sprintf(L"%s\\upconv_default_%02d.txt",wr_defaultParameterPath,i);
			sFileName = (AnsiString)uFileName;
			unlink(sFileName.c_str());
		}
		for (i = 0;i < cmbDefaultParameter->Items->Count - 1;i++) {
			uFileName.sprintf(L"%s\\upconv_default_%02d.txt",wr_defaultParameterPath,i+1);
			sFileName = (AnsiString)uFileName;
			if (edtParameter->Lines->Strings[i].Length() > 0) {
				sStr = edtParameter->Lines->Strings[i];
				strcpy(work,sStr.c_str());
				if (edtParameter2->Lines->Strings[i].Length() > 0) {
					sStr2 = edtParameter2->Lines->Strings[i];
					strcat(work," ");
					strcat(work,sStr2.c_str());
				}
				sTitle = edtPTitle->Lines->Strings[i];
				if (sTitle.Length() == 0) {
					sTitle.sprintf("Upconv Parameter %02d",i + 1);
				}
				p = strrchr(work,'|');
				if (p != NULL) *p = '\0';
				p = strrchr(work,'|');
				if (p != NULL) *p = '\0';
				p = strrchr(work,'|');
				if (p != NULL) *p = '\0';
				p = strrchr(work,'|');
				if (p != NULL) *p = '\0';
				p = strrchr(work,'|');
				if (p != NULL) *p = '\0';
				fp = fopen(sFileName.c_str(),"w");
				if (fp) {
					fprintf(fp,"%s\n",work);

					sStr = edtWorkPath->Text;
					fprintf(fp,"%s\n",sStr.c_str());

					sStr = edtEncorder_FLAC->Text;
					fprintf(fp,"%s\n",sStr.c_str());

					sStr = edtEncorder_WavPack->Text;
					fprintf(fp,"%s\n",sStr.c_str());

					sStr = edtEncorder_MP3->Text;
					fprintf(fp,"%s\n",sStr.c_str());

					fprintf(fp,"%s\n",sTitle.c_str());

					sStr = edtEncorder_OPUS->Text;
					fprintf(fp,"%s\n",sStr.c_str());

					sStr = edtEncorder_M4A->Text;
					fprintf(fp,"%s\n",sStr.c_str());
					fclose(fp);
				}
			}
		}
	}
	uFileName.sprintf(L"%s\\preset.dat",wr_defaultParameterPath);
	edtHFA_Preset->Lines->SaveToFile(uFileName);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::btnSaveClick(TObject *Sender)
{
	UnicodeString uStr;
	UnicodeString uTemp;
	AnsiString sStr,sStr2;
	uStr = L"";
	int index;
	int deE;
	int abe_ver;

	if (edtHFC->Text.Length() > 0 && edtHFC->Text.ToInt() < 4000) {
		Application->MessageBoxW(L"HFC >= 4000",L"",MB_OK);
		edtHFC->Text = L"";
		return;
	}
	if (edtLFC->Text.Length() > 0 && edtLFC->Text.ToInt() < 20) {
		Application->MessageBoxW(L"HFC >= 20",L"",MB_OK);
		edtLFC->Text = L"";
		return;
	}

	index = cmbDefaultParameter->ItemIndex;
	deE = 0;
	uStr = "";
	if (rdoDeE2->Checked == true) deE = 1;
	if (rdoDeE3->Checked == true) deE = 2;

	if (edtTitle->Text.Length() == 0) edtTitle->Text.sprintf(L"Upconv Parameter %02d",index + 1);
	// Upconv
	if (chkABE->Checked     == true) uStr += " -abe";
	if (chkPostABE->Checked == true) uStr += " -post_abe";
	if (chkPostNR->Checked  == true) uStr += " -post_nr";
	if (chkCutHighPassDither->Checked == true) uStr += " -cut_high_dither";
	if (chkLowAdjust->Checked == true) uStr += " -low_adjust";
	if (chkHFC_Auto->Checked  == true) uStr += " -hfc_auto";
	if (chk_LRC_process->Checked == true) uStr += " -LRC_process:1";
	if (cmb_MS_LRC->ItemIndex == 0) uStr += " -MS_LRC_mode:1";
	if (cmb_MS_LRC->ItemIndex == 1) uStr += " -MS_LRC_mode:2";
	if (cmbOverSamp->ItemIndex == 0) {
		uTemp.sprintf(L" -oversamp:0");uStr += uTemp;
	} else if (cmbOverSamp->ItemIndex == 1) {
		uTemp.sprintf(L" -oversamp:1");uStr += uTemp;
	} else if (cmbOverSamp->ItemIndex == 2) {
		uTemp.sprintf(L" -oversamp:4");uStr += uTemp;
	} else if (cmbOverSamp->ItemIndex == 3) {
		uTemp.sprintf(L" -oversamp:2");uStr += uTemp;
	} else if (cmbOverSamp->ItemIndex == 4) {
		uTemp.sprintf(L" -oversamp:5");uStr += uTemp;
	} else if (cmbOverSamp->ItemIndex == 5) {
		uTemp.sprintf(L" -oversamp:3");uStr += uTemp;
	} else if (cmbOverSamp->ItemIndex == 6) {
		uTemp.sprintf(L" -oversamp:6");uStr += uTemp;
	}
	if (chkPreEQ->Checked == true) {
		uStr += " -enable_pre_eq:1";
	}
	if (cmbPreEQ->ItemIndex >= 0) {
		uTemp.sprintf(L" -pre_eq:%s",cmbPreEQ->Items->Strings[cmbPreEQ->ItemIndex]);
		uStr += uTemp;
	}
	if (chkPostEQ->Checked == true) {
		uStr += " -enable_post_eq:1";
	}
	if (cmbPostEQ->ItemIndex >= 0) {
		uTemp.sprintf(L" -post_eq:%s",cmbPostEQ->Items->Strings[cmbPostEQ->ItemIndex]);
		uStr += uTemp;
	}
	uTemp.sprintf(L" -enable_hfc:%d",chkEnableHFC->Checked == true ? 1 : 0);uStr += uTemp;
	uTemp.sprintf(L" -enable_lfc:%d",chkEnableLFC->Checked == true ? 1 : 0);uStr += uTemp;
	if (edtHFC->Text.Length() > 0) {uTemp.sprintf(L" -hfc:%d",edtHFC->Text.ToInt());uStr += uTemp;}
	if (edtLFC->Text.Length() > 0) {uTemp.sprintf(L" -lfc:%d",edtLFC->Text.ToInt());uStr += uTemp;}
	uTemp.sprintf(L" -enable_nr:%d",chkNREnable->Checked == true ? 1 : 0);uStr += uTemp;
	if (edtNR_Freq->Text.Length() > 0) {uTemp.sprintf(L" -nr:%d",edtNR_Freq->Text.ToInt());uStr += uTemp;}
	uTemp.sprintf(L" -nr_option:%d",upD_NR_Level->Position);uStr += uTemp;
	uTemp.sprintf(L" -output_dither:%d",cmbOutputDither->ItemIndex);uStr += uTemp;
	uTemp.sprintf(L" -output_dither_option:%d",UpD_OutLv->Position);uStr += uTemp;
	uTemp.sprintf(L" -abe_option:%d,%d,%d",chkABE_CutLowLevel->Checked == true ? 1 : 0,chkSmoothingLowLevel->Checked == true ? 1 : 0,UpD_ABE_CutDitherLv->Position);uStr += uTemp;
	uTemp.sprintf(L" -abe_ad_filter:%d",chkAdFilter->Checked == true ? 1 : 0);uStr += uTemp;

	abe_ver = 1;
	if (rdoABE_080->Checked == true) abe_ver = 2;
	if (rdoABE_086->Checked == true) abe_ver = 3;
	if (rdoABE_10x->Checked == true) abe_ver = 4;
	uTemp.sprintf(L" -abe_version:%d",abe_ver);uStr += uTemp;
	uTemp.sprintf(L" -abe_declip:%d",chkABE_DeClip->Checked == true ? 1 : 0);uStr += uTemp;
	uTemp.sprintf(L" -abe_declick:%d",chkClickNoiseReduction->Checked == true ? 1 : 0);uStr += uTemp;
	uTemp.sprintf(L" -abe_clld_level:%d",UpD_CLLD_Level->Position);uStr += uTemp;
//	uTemp.sprintf(L" -abe_10x_wlt_level:%d",UpD_WltCutLevel->Position);uStr += uTemp;
	uTemp.sprintf(L" -abe_10x_cut_level:%d",UpD_CutLevel->Position);uStr += uTemp;
    uTemp.sprintf(L" -abe_10x_cut_mode:%d",cmbCutMode->ItemIndex);uStr += uTemp;

//	uTemp.sprintf(L" -abe_wlt_level:%d",UpD_WltCutLevel->Position);uStr += uTemp;

	uTemp.sprintf(L" -deE:%d",deE);uStr += uTemp;
	uTemp.sprintf(L" -hfc_auto_adjust:%d",cmbHFC_AutoAdjust->ItemIndex);uStr += uTemp;
	if (cmbVolumeLevelAdjust->ItemIndex == 11) {
		uTemp.sprintf(L" -vla:auto");uStr += uTemp;
	} else {
		uTemp.sprintf(L" -vla:%d",cmbVolumeLevelAdjust->ItemIndex);uStr += uTemp;
	}
	// HFA2/3 Option
	uTemp.sprintf(L" -hfa_preset:%d",cmbHFA_Preset->ItemIndex + 1);uStr += uTemp;
	if (rdoHFA3_0774->Checked == true) {
		uTemp.sprintf(L" -hfa3_option:%d",1);uStr += uTemp;
	}
	if (rdoHFA3_082->Checked == true) {
		uTemp.sprintf(L" -hfa3_option:%d",2);uStr += uTemp;
	}
	if (rdoHFA3_084->Checked == true) {
		uTemp.sprintf(L" -hfa3_option:%d",3);uStr += uTemp;
	}
	uTemp.sprintf(L" -hfa_filter:%d,%d,%d,%d",UpDHFA_filter1->Position,UpDHFA_filter2->Position,UpDHFA_filter3->Position,UpDHFA_filter4->Position);uStr += uTemp;
	uTemp.sprintf(L" -hfa3_analysis_limit_adjust:%d",track_hfa3_ala->Position);uStr += uTemp;
	uTemp.sprintf(L" -hfa_level:%d",cmbHFA_Level->ItemIndex); uStr += uTemp;
	uTemp.sprintf(L" -hfa3_freq_start:%d",cmb_HFA3_Freq_Start->Text.ToInt());uStr += uTemp;
	uTemp.sprintf(L" -hfa3_freq_len:%d",cmb_HFA3_Freq_Len->Text.ToInt());uStr += uTemp;
	uTemp.sprintf(L" -hfa3_analyze_step:%d",upd_hfa3_ana_step->Position);uStr += uTemp;
	uTemp.sprintf(L" -hfa_level_adj_width:%d",upd_hfa_level_adj_width->Position);uStr += uTemp;
	uTemp.sprintf(L" -hfa_level_adj_nx:%d",upd_hfa_level_adj_nx->Position);uStr += uTemp;
	if (tb_hfa3_sig_level_limit->Position > 9) {
		uTemp.sprintf(L" -hfa3_sig_level_limit:%d",tb_hfa3_sig_level_limit->Position);uStr += uTemp;
	} else {
		uTemp.sprintf(L" -hfa3_sig_level_limit:0");uStr += uTemp;
	}
	if (rdo_hfa3_pp_1->Checked == true) {
		uTemp.sprintf(L" -hfa3_pp:%d",1);uStr += uTemp;
	}
	if (rdo_hfa3_pp_2->Checked == true) {
		uTemp.sprintf(L" -hfa3_pp:%d",2);uStr += uTemp;
	}

	uTemp.sprintf(L" -hfa3_freq_start_LRC:%d",cmb_HFA3_Freq_Start_LRC->Text.ToInt());uStr += uTemp;
	uTemp.sprintf(L" -hfa3_freq_len_LRC:%d",cmb_HFA3_Freq_Len_LRC->Text.ToInt());uStr += uTemp;
	uTemp.sprintf(L" -hfa3_analyze_step_LRC:%d",upd_hfa3_ana_step_LRC->Position);uStr += uTemp;
	uTemp.sprintf(L" -hfa_level_adj_width_LRC:%d",upd_hfa_level_adj_width_LRC->Position);uStr += uTemp;
	uTemp.sprintf(L" -hfa_level_adj_nx_LRC:%d",upd_hfa_level_adj_nx_LRC->Position);uStr += uTemp;
	if (tb_hfa3_sig_level_limit_LRC->Position > 9) {
		uTemp.sprintf(L" -hfa3_sig_level_limit_LRC:%d",tb_hfa3_sig_level_limit_LRC->Position);uStr += uTemp;
	} else {
		uTemp.sprintf(L" -hfa3_sig_level_limit_LRC:0");uStr += uTemp;
	}
	if (rdo_hfa3_pp_1_LRC->Checked == true) {
		uTemp.sprintf(L" -hfa3_pp_LRC:%d",1);uStr += uTemp;
	}
	if (rdo_hfa3_pp_2_LRC->Checked == true) {
		uTemp.sprintf(L" -hfa3_pp_LRC:%d",2);uStr += uTemp;
	}
	uTemp.sprintf(L" -hfa3_LRC_CC_Phase:%.02lf",cmb_LRC_CC_Phase->Items->Strings[cmb_LRC_CC_Phase->ItemIndex].ToDouble());uStr += uTemp;
	uTemp.sprintf(L" -hfa3_LRC_CC_Level:%.02lf",cmb_LRC_CC_Level->Items->Strings[cmb_LRC_CC_Level->ItemIndex].ToDouble());uStr += uTemp;
	if (cmbHFA3_FFT_Window_Size->ItemIndex == 0) {
		uTemp.sprintf(L" -hfa3_fft_window_size:0");uStr += uTemp;
	} else {
		uTemp.sprintf(L" -hfa3_fft_window_size:%s",cmbHFA3_FFT_Window_Size->Text);uStr += uTemp;
	}
	if (cmbHFA3_FFT_Window_Lvl->ItemIndex == 0) {
		uTemp.sprintf(L" -hfa3_fft_window_lvl:0");uStr += uTemp;
	} else {
		uTemp.sprintf(L" -hfa3_fft_window_lvl:%s",cmbHFA3_FFT_Window_Lvl->Text);uStr += uTemp;
	}
	if (chkEnableCenterHFC->Checked == true) {
		uTemp.sprintf(L" -enable_center_hfc:1");uStr += uTemp;
	}
	if (edtCenterHFC->Text.Length() > 0) {
		uTemp.sprintf(L" -center_hfc:%d",edtCenterHFC->Text.ToInt());uStr += uTemp;
	}

	// Multi Channel
	if (chkMC_Center->Checked == true) uStr += L" -C";
	if (chkMC_Surround->Checked == true) uStr += L" -SLR";
	if (chkMC_LFE->Checked == true) uStr += L" -LFE";
	uTemp.sprintf(L" -MC_Option:%d,%d,%d,%d",chkMC_Mono->Checked == true ? 1 : 0,chkMC_DownMix->Checked == true ? 1 : 0,chkMC_EchoEffect->Checked == true ? 1 : 0,UpDMC_EchoEffect->Position);uStr += uTemp;

	// Overwrite Option
	uTemp.sprintf(L" -ovwrite_s:%d",cmbSamplingOverWrite->ItemIndex);uStr += uTemp;
	uTemp.sprintf(L" -ovwrite_w:%d",cmbBitWidthOverWrite->ItemIndex);uStr += uTemp;
	uTemp.sprintf(L" -enable_ovwrite_sw:%d",chkConvOptionOverWrite->Checked == true ? 1 : 0);uStr += uTemp;
	uTemp.sprintf(L" -ovwrite_hfa:%d",cmbHFA->ItemIndex);uStr += uTemp;
	uTemp.sprintf(L" -enable_ovwrite_hfa:%d",chkHFC_OverWrite->Checked == true ? 1 : 0);uStr += uTemp;
	uTemp.sprintf(L" -ovwrite_norm:%d",cmbNormalize->ItemIndex);uStr += uTemp;
	uTemp.sprintf(L" -enable_ovwrite_norm:%d",chkNormalizeOverWrite->Checked == true ? 1 : 0);uStr += uTemp;
	if (chk_ms_process->Checked) {
		uTemp.sprintf(L" -ms_process:%d",cmbMidSide->ItemIndex + 1);uStr += uTemp;
	} else {
		uTemp.sprintf(L" -ms_process_save:%d",cmbMidSide->ItemIndex + 1);uStr += uTemp;
	}

	uTemp.sprintf(L" -enable_suffix:%d",chkSuffix->Checked == true ? 1 : 0);uStr += uTemp;
	uTemp.sprintf(L" -suffix:%s",edtSuffix->Text);uStr += uTemp;

	sStr = uStr.TrimLeft() + L"|" + edtEncorder_FLAC->Text + L"|" + edtEncorder_WavPack->Text + L"|" + edtEncorder_MP3->Text + L"|" + edtEncorder_OPUS->Text + L"|" + edtEncorder_M4A->Text;
	if (sStr.Length() <= 1000) {
		edtParameter->Lines->Strings[index]  = (UnicodeString)sStr;
		edtParameter2->Lines->Strings[index] = "";
	} else {
		// sStr の文字数が1000以下になるようにsStr,sStr2に分割する
		char del_c,del_c2;
		char str[8192];
		char *p,*pp;
		sStr2 = "";
		strcpy(str,sStr.c_str());
		pp = NULL;
		del_c  = '\0';
		del_c2 = '\0';
		do {
			if (del_c == '\0') {
				p = strchr(str,'|');
			} else {
				p = strrchr(str,' ');
			}

			if (p != NULL) {
				del_c2 = del_c;
				del_c = *p;
				*p = '\0';
				if (pp != NULL && del_c2 != '\0') {
					*pp = del_c2;
				}
				sStr  = (AnsiString)str;
				sStr2 = (AnsiString)del_c + (AnsiString)(p + 1);
				if (sStr.Length() <= 1000) {
					// 分割できた場合
					break;
				} else {
					pp = p;
				}
			} else {
				// 分割できない場合はやめる
				if (pp != NULL && del_c2 != '\0') {
					*pp = del_c2;
				}
				break;
			}
		} while (1);
		edtParameter->Lines->Strings[index]  = (UnicodeString)sStr;
		edtParameter2->Lines->Strings[index] = (UnicodeString)sStr2;
	}
	edtPTitle->Lines->Strings[index] = edtTitle->Text;
	saveFlag = 1;

	if (index + 1 == cmbDefaultParameter->Items->Count && index + 1 <= 98) {
		uStr.sprintf(L"%d",index + 2);
		cmbDefaultParameter->Items->Add(uStr);
		if (index + 1 == edtParameter->Lines->Count) {
			edtParameter->Lines->Add("");
		}
		if (index + 1 == edtParameter2->Lines->Count) {
			edtParameter2->Lines->Add("");
		}
		if (index + 1 == edtPTitle->Lines->Count) {
			edtPTitle->Lines->Add("");
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtWorkPathLeftButtonClick(TObject *Sender)
{
	WideString root = "";
	String dir = "";
	dir = edtWorkPath->Text;
	if (dir.Length() == 0) dir = "c:\\Windows\\Temp";
	if (SelectDirectory("Upconv",root,dir)) {
		edtWorkPath->Text = dir;
		edtWorkPath->RightButton->Visible = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtWorkPathRightButtonClick(TObject *Sender)
{
	edtWorkPath->Text = L"";
}
//---------------------------------------------------------------------------


void __fastcall TForm1::cmbDefaultParameterChange(TObject *Sender)
{
	if (cmbDefaultParameter->ItemIndex >= 2) {
		btnSave->Enabled = true;
		if (btnDelete->Tag == 0) {
			btnDelete->Enabled = true;
		} else {
			btnDelete->Enabled = false;
		}
	} else {
		btnSave->Enabled = false;
		btnDelete->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnDeleteClick(TObject *Sender)
{
	int i,index;
	HWND hParent;

	UnicodeString uStr,uStr2;
	index = cmbDefaultParameter->ItemIndex;
	for (i = index; i < edtParameter->Lines->Count;i++) {
		uStr = edtParameter->Lines->Strings[i + 1];
		edtParameter->Lines->Strings[i] = uStr;
		uStr = edtParameter2->Lines->Strings[i + 1];
		edtParameter2->Lines->Strings[i] = uStr;
		uStr = edtPTitle->Lines->Strings[i + 1];
		edtPTitle->Lines->Strings[i] = uStr;
		uStr.sprintf(L"%s\\upconv_default_%02d.txt",wr_defaultParameterPath,i);
		uStr2.sprintf(L"%s\\upconv_default_%02d.txt",wr_defaultParameterPath,i + 1);
		DeleteFile(uStr.c_str());
		MoveFile(uStr2.c_str(),uStr.c_str());
	}
	edtParameter->Lines->Strings[i] = L"";
	edtParameter2->Lines->Strings[i] = L"";
	edtPTitle->Lines->Strings[i] = L"";
	cmbDefaultParameter->Items->Clear();
	uStr.sprintf(L"%s\\upconv_default_%02d.txt",wr_defaultParameterPath,i);
	DeleteFile(uStr.c_str());
	for (i = 0;i < edtParameter->Lines->Count;i++) {
		uStr.sprintf(L"%d",i + 1);
		if (edtParameter->Lines->Strings[i].Length() == 0) break;
		cmbDefaultParameter->Items->Add(uStr);
	}
	cmbDefaultParameter->Items->Add(uStr);
	edtTitle->Text = L"";
	hParent = FindWindow(NULL, L"Upconv Frontend 1.0.x");
	if (hParent == NULL) {
		hParent = FindWindow(NULL, L"Upconv Frontend 0.8.6");
		if (hParent == NULL) {
			hParent = FindWindow(NULL, L"Upconv Frontend 0.8.5");
			if (hParent == NULL) {
				hParent = FindWindow(NULL, L"Upconv Frontend 0.8.4");
				if (hParent == NULL) {
					hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2k");
					if (hParent == NULL) {
						hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2k (x64)");
						if (hParent == NULL) {
							hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2j");
							if (hParent == NULL) {
								hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2j (x64)");
								if (hParent == NULL) {
									hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2i");
									if (hParent == NULL) {
										hParent = FindWindow(NULL, L"Upconv Frontend 0.8.2i (x64)");
										if (hParent == NULL) {
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	PostMessage(hParent, WM_APP + WM_RESET_DP, 0, 0);

	saveFlag = 1;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::edtTitleKeyPress(TObject *Sender, System::WideChar &Key)
{
	if (Key == '|') Key = '\0';
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtEncorder_FLACKeyPress(TObject *Sender, System::WideChar &Key)

{
	if (Key == '|') Key = '\0';
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtEncorder_WavPackKeyPress(TObject *Sender, System::WideChar &Key)

{
	if (Key == '|') Key = '\0';
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtEncorder_MP3KeyPress(TObject *Sender, System::WideChar &Key)

{
	if (Key == '|') Key = '\0';
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtSuffixKeyPress(TObject *Sender, System::WideChar &Key)

{
	if (Key == '|' || Key == ' ') Key = '\0';
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtSuffixRightButtonClick(TObject *Sender)
{
	edtSuffix->Text = L"upconv";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rdoHFA3_0774Click(TObject *Sender)
{
	edtHFA_Filter1->Enabled = false;
	edtHFA_Filter2->Enabled = false;
	edtHFA_Filter3->Enabled = false;
	edtHFA_Filter4->Enabled = false;
	track_hfa3_ala->Enabled = false;
	track_hfa3_ala->SliderVisible = false;
	cmb_HFA3_Freq_Start->Enabled = false;
	cmb_HFA3_Freq_Len->Enabled = false;
	edt_hfa3_ana_step->Enabled = false;
	upd_hfa3_ana_step->Enabled = false;
	GroupBox5->Enabled = false;
	edtHFA_LevelAdjWidth->Enabled = false;
	upd_hfa_level_adj_width->Enabled = false;
	GroupBox7->Enabled = false;
	edtHFA_LevelAdjNX->Enabled = false;
	upd_hfa_level_adj_nx->Enabled = false;
	GroupBox8->Enabled = false;
	GroupBox9->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rdoHFA3_082Click(TObject *Sender)
{
	edtHFA_Filter1->Enabled = true;
	edtHFA_Filter2->Enabled = true;
	edtHFA_Filter3->Enabled = true;
	edtHFA_Filter4->Enabled = true;
	track_hfa3_ala->Enabled = true;
	track_hfa3_ala->SliderVisible = true;
	cmb_HFA3_Freq_Start->Enabled = false;
	cmb_HFA3_Freq_Len->Enabled = false;
	edt_hfa3_ana_step->Enabled = false;
	upd_hfa3_ana_step->Enabled = false;
	GroupBox5->Enabled = false;
	edtHFA_LevelAdjWidth->Enabled = false;
	upd_hfa_level_adj_width->Enabled = false;
	GroupBox7->Enabled = false;
	edtHFA_LevelAdjNX->Enabled = false;
	upd_hfa_level_adj_nx->Enabled = false;
	GroupBox8->Enabled = false;
	GroupBox9->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rdoHFA3_084Click(TObject *Sender)
{
	edtHFA_Filter1->Enabled = true;
	edtHFA_Filter2->Enabled = true;
	edtHFA_Filter3->Enabled = true;
	edtHFA_Filter4->Enabled = true;
	track_hfa3_ala->Enabled = true;
	track_hfa3_ala->SliderVisible = true;
	cmb_HFA3_Freq_Start->Enabled = true;
	cmb_HFA3_Freq_Len->Enabled = true;
	edt_hfa3_ana_step->Enabled = true;
	upd_hfa3_ana_step->Enabled = true;
	GroupBox5->Enabled = true;
	edtHFA_LevelAdjWidth->Enabled = true;
	upd_hfa_level_adj_width->Enabled = true;
	GroupBox7->Enabled = true;
	edtHFA_LevelAdjNX->Enabled = true;
	upd_hfa_level_adj_nx->Enabled = true;
	GroupBox8->Enabled = true;
	GroupBox9->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdoABE_0774Click(TObject *Sender)
{
	edtCLLD_Level->Enabled = false;
	UpD_CLLD_Level->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdoABE_080Click(TObject *Sender)
{
	edtCLLD_Level->Enabled = true;
	UpD_CLLD_Level->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::track_hfa3_alaChange(TObject *Sender)
{
	UnicodeString uStr;
	uStr.sprintf(L"%d kHz",track_hfa3_ala->Position);
	lbl_hfa3_ala->Caption = uStr;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btn_folder_openClick(TObject *Sender)
{
	UnicodeString uStr;

	uStr = L"explorer.exe \"" + wr_defaultParameterPath + L"\"";

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = {};

	if (CreateProcess(NULL,uStr.c_str(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) {
		CloseHandle(pi.hThread);
	}
}

void __fastcall TForm1::rdo_ana_step_1Click(TObject *Sender)
{
	upd_hfa3_ana_step->Position = 77;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_ana_step_2Click(TObject *Sender)
{
	upd_hfa3_ana_step->Position = 273;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_ana_step_3Click(TObject *Sender)
{
	upd_hfa3_ana_step->Position = 203;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_width_1Click(TObject *Sender)
{
	upd_hfa_level_adj_width->Position = 2000;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_width_2Click(TObject *Sender)
{
	upd_hfa_level_adj_width->Position = 3000;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_nx_1Click(TObject *Sender)
{
	upd_hfa_level_adj_nx->Position = 95;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_nx_2Click(TObject *Sender)
{
	upd_hfa_level_adj_nx->Position = 85;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::tb_hfa3_sig_level_limitChange(TObject *Sender)
{
	UnicodeString uStr;
	if (tb_hfa3_sig_level_limit->Position > 9) {
		uStr.sprintf(L"%d kHz",tb_hfa3_sig_level_limit->Position);
	} else {
		uStr.sprintf(L"Off");
	}
	lbl_hfa3_sig_level_limit->Caption = uStr;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cmbPreEQChange(TObject *Sender)
{
	FILE *fp;
	UnicodeString uFileName;
	UnicodeString uStr;
	AnsiString sFileName;
	AnsiString sStr;
	char str[4096];
	char title[4096];
	int error;
	int ii,index;
	char *p;

	index = cmbPreEQ->ItemIndex + 1;
	sStr = L"";
	uFileName.printf(L"%s\\upconv_eq_%03d.txt",rd_defaultEQPath,index);
	sFileName = (AnsiString)uFileName;
	fp = fopen(sFileName.c_str(),"r");
	if (fp) {
		double dp;
		for (ii = 0;ii < 192000;ii++) {
			if (fscanf(fp,"%lf",&dp) == 1) {
			} else {
				break;
			}
		}
		if (fscanf(fp,"%lf",&dp) == 1) {
			ii++;
			for (;ii < 384000;ii++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
				} else {
					break;
				}
			}
			fgets(str,4096,fp);
			fgets(str,4096,fp);
			fgets(str,4096,fp);
		}
		if (ii == 192000 || ii == 384000) {
			if (fgets(str,4096,fp) != NULL) {					/* Active(EQ) */
				if (fgets(str,4096,fp) != NULL) {   			/* EQ(Type1) */
					if (fgets(str,4096,fp) != NULL) {			/* EQ(Type2) */
						if (fgets(str,4096,fp) != NULL) {		/* EQ(Type3) */
							if (fgets(str,4096,fp) != NULL) {	/* Title */
								p = strrchr(str,'\n');if (p) *p = '\0';
								edt_PreEQ_Title->Text = (UnicodeString)str;
							}
						}
					}
				}
			}
		}
		fclose(fp);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CategoryPanel1Expand(TObject *Sender)
{
	CategoryPanel2->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel4->Collapsed = true;
	CategoryPanel8->Collapsed = true;
	//CategoryPanel9->Collapsed = true;
	CategoryPanel10->Collapsed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel2Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel4->Collapsed = true;
	CategoryPanel8->Collapsed = true;
	//CategoryPanel9->Collapsed = true;
	CategoryPanel10->Collapsed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel3Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel2->Collapsed = true;
	CategoryPanel4->Collapsed = true;
	CategoryPanel8->Collapsed = true;
	//CategoryPanel9->Collapsed = true;
	CategoryPanel10->Collapsed = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel4Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel2->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel8->Collapsed = true;
	//CategoryPanel9->Collapsed = true;
	CategoryPanel10->Collapsed = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CategoryPanel10Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel2->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel4->Collapsed = true;
   	CategoryPanel8->Collapsed = true;
	//CategoryPanel9->Collapsed = true;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel1Collapse(TObject *Sender)
{
	//CategoryPanel1->Collapsed = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel2Collapse(TObject *Sender)
{
	//CategoryPanel2->Collapsed = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel3Collapse(TObject *Sender)
{
	//CategoryPanel3->Collapsed = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel4Collapse(TObject *Sender)
{
	//CategoryPanel4->Collapsed = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel5Collapse(TObject *Sender)
{
	CategoryPanel5->Collapsed = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel6Collapse(TObject *Sender)
{
	CategoryPanel6->Collapsed = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CategoryPanel8Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel2->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel4->Collapsed = true;
	//CategoryPanel9->Collapsed = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CategoryPanel10Collapse(TObject *Sender)
{
//	CategoryPanel10->Collapsed = false;
}

void __fastcall TForm1::cmbPostEQChange(TObject *Sender)
{
	FILE *fp;
	UnicodeString uFileName;
	UnicodeString uStr;
	AnsiString sFileName;
	AnsiString sStr;
	char str[4096];
	char title[4096];
	int error;
	int ii,index;
	char *p;

	index = cmbPostEQ->ItemIndex + 1;
	sStr = L"";
	uFileName.printf(L"%s\\upconv_eq_%03d.txt",rd_defaultEQPath,index);
	sFileName = (AnsiString)uFileName;
	fp = fopen(sFileName.c_str(),"r");
	if (fp) {
		double dp;
		for (ii = 0;ii < 192000;ii++) {
			if (fscanf(fp,"%lf",&dp) == 1) {
			} else {
				break;
			}
		}
		if (fscanf(fp,"%lf",&dp) == 1) {
			ii++;
			for (;ii < 384000;ii++) {
				if (fscanf(fp,"%lf",&dp) == 1) {
				} else {
					break;
				}
			}
			fgets(str,4096,fp);
			fgets(str,4096,fp);
			fgets(str,4096,fp);
		}
		if (ii == 192000 || ii == 384000) {
			if (fgets(str,4096,fp) != NULL) {					/* Active(EQ) */
				if (fgets(str,4096,fp) != NULL) {   			/* EQ(Type1) */
					if (fgets(str,4096,fp) != NULL) {			/* EQ(Type2) */
						if (fgets(str,4096,fp) != NULL) {		/* EQ(Type3) */
							if (fgets(str,4096,fp) != NULL) {	/* Title */
								p = strrchr(str,'\n');if (p) *p = '\0';
								edt_PostEQ_Title->Text = (UnicodeString)str;
							}
						}
					}
				}
			}
		}
		fclose(fp);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rdo_ana_step_2_LRCClick(TObject *Sender)
{
	upd_hfa3_ana_step_LRC->Position = 273;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_ana_step_3_LRCClick(TObject *Sender)
{
	upd_hfa3_ana_step_LRC->Position = 203;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_width_1_LRCClick(TObject *Sender)
{
	upd_hfa_level_adj_width_LRC->Position = 2000;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_width_2_LRCClick(TObject *Sender)
{
	upd_hfa_level_adj_width_LRC->Position = 3000;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_nx_1_LRCClick(TObject *Sender)
{
	upd_hfa_level_adj_nx_LRC->Position = 95;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_nx_2_LRCClick(TObject *Sender)
{
	upd_hfa_level_adj_nx_LRC->Position = 85;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::tb_hfa3_sig_level_limit_LRCChange(TObject *Sender)
{
	UnicodeString uStr;
	if (tb_hfa3_sig_level_limit_LRC->Position > 9) {
		uStr.sprintf(L"%d kHz",tb_hfa3_sig_level_limit_LRC->Position);
	} else {
		uStr.sprintf(L"Off");
	}
	lbl_hfa3_sig_level_limit_LRC->Caption = uStr;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::CategoryPanel9Expand(TObject *Sender)
{
	CategoryPanel1->Collapsed = true;
	CategoryPanel2->Collapsed = true;
	CategoryPanel3->Collapsed = true;
	CategoryPanel4->Collapsed = true;
	CategoryPanel8->Collapsed = true;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::rdo_ana_step_1_LRCClick(TObject *Sender)
{
	upd_hfa3_ana_step_LRC->Position = 77;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_ana_step_4Click(TObject *Sender)
{
upd_hfa3_ana_step->Position = 113;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_nx_3Click(TObject *Sender)
{
	upd_hfa_level_adj_nx->Position = 80;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_ana_step_4_LRCClick(TObject *Sender)
{
	upd_hfa3_ana_step_LRC->Position = 193;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::rdo_level_nx_3_LRCClick(TObject *Sender)
{
	upd_hfa_level_adj_nx_LRC->Position = 75;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LoadEncoderCustomOption(int index)
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

	if (index == -1) {
		uFileName.printf(L"%s\\upconvfe_ext.txt",rd_defaultParameterPath);
		sFileName = (AnsiString)uFileName;
		fp = fopen(sFileName.c_str(),"rt");
		TStringList *tsl = NULL;
		tsl = new TStringList;
		tsl->LoadFromFile(uFileName);
		for (i = 0; i < tsl->Count;i++) {
			UnicodeString ext = tsl->Strings[i];
			ext = ext.LowerCase();
			ext = ext.Trim();
			if (ext.Length() > 0 && ext.Length() <= 10) {
				for (ii = 0;ii < cmbEncoderCustomExt->Items->Count;ii++) {
					if (cmbEncoderCustomExt->Items->Strings[ii] == ext) break;
				}
				if (ii == cmbEncoderCustomExt->Items->Count) {
					cmbEncoderCustomExt->Items->Add(ext);
				}
			}
		}
		delete tsl;
		if (cmbEncoderCustomExt->Items->Count == 0) {
			cmbEncoderCustomExt->Items->Add("mp3");
		}

		if (fp != NULL) fclose(fp);

		uFileName.printf(L"%s\\upconv_encoder.txt",rd_defaultParameterPath);
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
								cnt++;
							}
						}
					}
				} else {
					break;
				}
			}
			fclose(fp);
		}
		if (cnt == 0) {
			fp = fopen(sFileName.c_str(),"wt");
			fprintf(fp,"%s\n","lame(mp3)    96k|lame.exe|mp3|-V 0 -b 96");
			fclose(fp);
			// デフォルト値を指定しておく
			cnt = 1;
		}
		cmbEncoderCustom->Items->Clear();
		for (i = 1;i <= cnt + 1;i++) {
			sprintf(work,"%02d",i);
			cmbEncoderCustom->Items->Add(work);
		}
		index = 0;
	} else {
		edtEncoderCustomTitle->Text = "";
		cmbEncoderCustomPg->ItemIndex = -1;
		cmbEncoderCustomExt->ItemIndex = -1;
		edtEncoderCustomParam->Text = "";
		btnEncoderCustomSave->Enabled = true;
		btnEncoderCustomDelete->Enabled = true;
		if (index + 1 == cmbEncoderCustom->Items->Count) {
			btnEncoderCustomDelete->Enabled = false;
		}

		uFileName.printf(L"%s\\upconv_encoder.txt",rd_defaultParameterPath);
		sFileName = (AnsiString)uFileName;
		fp = fopen(sFileName.c_str(),"rt");
		if (fp != NULL) {
			for (i = 0;i < 100;i++) {
				work[0] = '\0';
				if (fgets(work,500,fp) != NULL) {
					if (i == index && strlen(work) > 0) {
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
								edtEncoderCustomTitle->Text = (UnicodeString)e_p1;
								if (strcmp(e_p2,"ffmpeg.exe") == 0) {
									cmbEncoderCustomPg->ItemIndex = 0;
								}
								if (strcmp(e_p2,"lame.exe") == 0) {
									cmbEncoderCustomPg->ItemIndex = 1;
								}
								for (ii = 0;ii < cmbEncoderCustomExt->Items->Count;ii++) {
									if (cmbEncoderCustomExt->Items->Strings[ii] == (AnsiString)e_p3) {
										cmbEncoderCustomExt->ItemIndex = ii;
										break;
									}
								}
								edtEncoderCustomParam->Text = (AnsiString)e_p4;
							}
						}
					}
				} else {
					break;
				}
			}
			fclose(fp);
		}
		if (edtEncoderCustomTitle->Text == "" || cmbEncoderCustomPg->ItemIndex == -1 || cmbEncoderCustomExt->ItemIndex == -1 || edtEncoderCustomParam->Text == "") {
			btnEncoderCustomSave->Enabled = false;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SaveEncoderCustomOption(int index)
{

}
//---------------------------------------------------------------------------



void __fastcall TForm1::cmbEncoderCustomChange(TObject *Sender)
{
	LoadEncoderCustomOption(cmbEncoderCustom->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtEncoderCustomTitleChange(TObject *Sender)
{
	if (edtEncoderCustomTitle->Text.Length() > 0) {
		btnEncoderCustomSave->Enabled = true;
	} else {
		btnEncoderCustomSave->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbEncoderCustomPgChange(TObject *Sender)
{
	btnEncoderCustomSave->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cmbEncoderCustomExtChange(TObject *Sender)
{
	btnEncoderCustomSave->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::edtEncoderCustomParamChange(TObject *Sender)
{
	if (edtEncoderCustomParam->Text.Length() > 0) {
		btnEncoderCustomSave->Enabled = true;
	} else {
		btnEncoderCustomSave->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnEncoderCustomSaveClick(TObject *Sender)
{
	UnicodeString uFileName;
	UnicodeString uItemString;
	AnsiString sFileName;
	AnsiString itemString;

	char work[512];
	char *e_p1,*e_p2,*e_p3,*e_p4;
	int i,ii,cnt;
	e_p1 = e_p2 = e_p3 = e_p4;
	cnt = 0;

	uFileName.printf(L"%s\\upconv_encoder.txt",rd_defaultParameterPath);
	if (FileExists(uFileName) == true) {
		TStringList *tsl = NULL;
		tsl = new TStringList;
		tsl->LoadFromFile(uFileName);
		if (cmbEncoderCustom->ItemIndex < tsl->Count) {
			edtEncoderCustomTitle->Text = StringReplace(edtEncoderCustomTitle->Text,"|","",TReplaceFlags() << rfReplaceAll);
			uItemString = edtEncoderCustomTitle->Text + L"|" + cmbEncoderCustomPg->Text + (UnicodeString)"|" + cmbEncoderCustomExt->Text + (UnicodeString)"|" + edtEncoderCustomParam->Text;
			uItemString = StringReplace(uItemString,"\n","",TReplaceFlags() << rfReplaceAll);
			tsl->Strings[cmbEncoderCustom->ItemIndex] = uItemString;
		}
		tsl->SaveToFile(uFileName);
		if (cmbEncoderCustom->ItemIndex + 1 == cmbEncoderCustom->Items->Count) {
			sprintf(work,"%02d",cmbEncoderCustom->Items->Count + 1);
			cmbEncoderCustom->Items->Add(work);

		}
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::btnEncoderCustomDeleteClick(TObject *Sender)
{
	UnicodeString uFileName;
	UnicodeString uItemString;
	AnsiString sFileName;
	AnsiString itemString;

	char work[512];
	char *e_p1,*e_p2,*e_p3,*e_p4;
	int i,ii,cnt;
	e_p1 = e_p2 = e_p3 = e_p4;
	cnt = 0;

	uFileName.printf(L"%s\\upconv_encoder.txt",rd_defaultParameterPath);
	if (FileExists(uFileName) == true) {
		TStringList *tsl = NULL;
		tsl = new TStringList;
		tsl->LoadFromFile(uFileName);
		for (i = cmbEncoderCustom->ItemIndex;i + 2 < tsl->Count;i++) {
			tsl->Strings[i] = tsl->Strings[i + 1];
		}
		tsl->Delete(i);
		cmbEncoderCustom->Items->Delete(cmbEncoderCustom->Items->Count - 1);
		tsl->SaveToFile(uFileName);
		LoadEncoderCustomOption(cmbEncoderCustom->ItemIndex);
	}
}
//---------------------------------------------------------------------------

