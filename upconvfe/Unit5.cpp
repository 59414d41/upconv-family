//---------------------------------------------------------------------------

#include <vcl.h>
#include <Jpeg.hpp>
#include <PNGImage.hpp>
#pragma hdrstop

#include "Unit5.h"
#include "..\\upconv_src\\upconv\\upconv.h"
#include "..\\upconv_src\\PLG_AUDIO_IO\\PLG_AudioIO.h"
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm5 *Form5;
//---------------------------------------------------------------------------
__fastcall TForm5::TForm5(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm5::FormShow(TObject *Sender)
{
	int retValue;
	char *info;
	char *pic;
	char pic_tag[4];
	char *p,*cr;
	int  pic_size;
	TTreeNode *parent;
	TTreeNode *n1;
	TTreeNode *n2;
	TTreeNode *n3;
	TTreeNode *n4;
	TTreeNode *n5;
	TTreeNode *n;
	TStringList *tsList;
	int level;
	parent = n1 = n2 = n3 = n4 = n5 = n = NULL;
	level  = 0;
	Image1->Picture = NULL;
	memset(pic_tag,0,4);
	tvTree->Items->Clear();
	while (tvDesc->RowCount > 2) {
		tvDesc->DeleteRow(2);
	}


	pic_size = 4 * 1024 * 1024L;
	info = (char *)HeapAlloc(GetProcessHeap(),0,4 * 1024 * 1024L);
	pic  = (char *)HeapAlloc(GetProcessHeap(),0,pic_size);
	if (info != NULL && pic != NULL) {
		if (strlen(filename.c_str()) > 0) {
			memset(info,0,4 * 1024 * 1024L);
			memset(pic,0,pic_size);
			retValue = PLG_GetChunkInfo(filename.c_str(),info,4 * 1024 * 1024L,pic,&pic_size,pic_tag);
			if (retValue == STATUS_SUCCESS) {
				p = info;
				do {
					cr = strchr(p,'\n');
					if (cr) {
						*cr = '\0';
					}
					if (p == NULL || strlen(p) == 0) {
						break;
					}
					if (p[0] == 'A') {
						switch (level) {
							case 0:
								n = tvTree->Items->Add(NULL,(UnicodeString)(p + 1));
								parent = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
							case 1:
								n = tvTree->Items->AddChild(parent,(UnicodeString)(p + 1));
								n1 = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
							case 2:
								n = tvTree->Items->AddChild(n1,(UnicodeString)(p + 1));
								n2 = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
							case 3:
								n = tvTree->Items->AddChild(n2,(UnicodeString)(p + 1));
								n3 = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
							case 4:
								n = tvTree->Items->AddChild(n3,(UnicodeString)(p + 1));
								n4 = n;
								tsList = new TStringList;
								n->Data = tsList;
								break;
						}
					} else if (p[0] == 'D') {
						level++;
					} else if (p[0] == 'I') {
						switch (level) {
							case 0:
								tsList = (TStringList*)parent->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
							case 1:
								tsList = (TStringList*)n1->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
							case 2:
								tsList = (TStringList*)n2->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
							case 3:
								tsList = (TStringList*)n3->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
							case 4:
								tsList = (TStringList*)n4->Data;
								tsList->Add((UnicodeString)(p + 1));
								break;
						}
					}
					if (cr) {
						p = cr + 1;
					}
				} while (cr != NULL);
				tvTree->FullExpand();
				if (strcmp(pic_tag,"JPG") == 0 && pic_size > 0) {
					TMemoryStream *imem = new TMemoryStream;
					imem->Write(pic,pic_size);
					imem->Position = 0;
					TJPEGImage *jpg = new TJPEGImage;
					jpg->LoadFromStream(imem);
					Image1->Picture->Bitmap->Assign(jpg);
					delete imem;
					delete jpg;
				}
				if (strcmp(pic_tag,"PNG") == 0 && pic_size > 0) {
					TMemoryStream *imem = new TMemoryStream;
					imem->Write(pic,pic_size);
					imem->Position = 0;
					TPngImage *png = new TPngImage;
					png->LoadFromStream(imem);
					Image1->Picture->Bitmap->Assign(png);
					delete imem;
					delete png;
				}
			}
		}
		HeapFree(GetProcessHeap(),0,info);
		HeapFree(GetProcessHeap(),0,pic);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm5::tvTreeClick(TObject *Sender)
{
	TStringList *tsList;
	TTreeNode *Node;
	UnicodeString s;
	UnicodeString k,v;
	int i,pos,n;

	tvDesc->Visible = true;
	while (tvDesc->RowCount > 2) {
		tvDesc->DeleteRow(2);
	}
	Node = tvTree->Selected;
	if (Node) {
		tsList = (TStringList *)Node->Data;
		for (i = 0;i < tsList->Count;i++) {
			s = tsList->Strings[i];
			pos = s.LastDelimiter("\t");
			n = s.Length();
			k = s.SubString(0,pos - 1);
			v = s.SubString(pos + 1,n);
			if (k == "Offset") {
				tvDesc->Values["Offset"] = v;
			} else if (k == "Size") {
				tvDesc->Values["Size"] = v;
			} else {
				tvDesc->InsertRow(k,v,true);
			}
			tvDesc->ItemProps[i]->ReadOnly = true;
		}
	}

}
//---------------------------------------------------------------------------

