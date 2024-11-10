//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit6.h"
#include "Unit1.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm6 *Form6;
//---------------------------------------------------------------------------
__fastcall TForm6::TForm6(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm6::FormShow(TObject *Sender)
{
	char *p1,*p2;
	long dp_len;
	int i;
	UnicodeString uStr;
	AnsiString sStr;
	FILE *fp;
	edt_DP_version->Text = dp_version;
	edt_dp_mtime->Text   = dp_mtime;
	p1 = dp_param;
	dp_len = strlen(p1);
	edt_default_parameter->Lines->Clear();

	for (i = 0;i < 6;i++) {
		p2 = strchr(p1,'\n');
		if (p2 != NULL) {
			*p2 = '\0';
		}
		uStr = p1;
		uStr = uStr.TrimRight();
		//if (uStr.Length() == 0) break;
		edt_default_parameter->Lines->Add(uStr);
		if (p2 == NULL) break;
		p1 = p2 + 1;
		dp_len = strlen(p1);
	}
	for (i = 1;i <= 99;i++) {
		uStr.printf(L"%s\\upconv_default_%02d.txt",Form1->defaultParameterPath,i);
		sStr = uStr;
		fp = fopen(sStr.c_str(),"r");
		if (fp == NULL) break;
		fclose(fp);
	}
	if (i <= 99) {
		uStr.printf(L"%02d",i);
		edt_dpn->Text = uStr;
	} else {
		btnSave->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm6::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------


void __fastcall TForm6::btnSaveClick(TObject *Sender)
{
	FILE *fp;
	UnicodeString uStr;
	AnsiString sStr;
	int i;

	if (edt_dpn->Text.Length() > 0 && edt_dpn->Text.ToInt() > 0) {
		uStr.printf(L"%s\\upconv_default_%02d.txt",Form1->defaultParameterPath,edt_dpn->Text.ToInt());
		sStr = uStr;
		fp = fopen(sStr.c_str(),"w");
		if (fp != NULL) {
			for (i = 0;i < 6;i++) {
				sStr = edt_default_parameter->Lines->Strings[i];
				fprintf(fp,"%s\n",sStr.c_str());
			}
			fclose(fp);
		} else {
			Application->MessageBoxW(L"Error:Save Default Parameter!",L"Upconv Frontend",MB_OK);
		}
	}
}
//---------------------------------------------------------------------------

