//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit3.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   重要: VCL におけるオブジェクトのメソッドとプロパティは、Synchronize を使って
//   呼び出されるメソッドでのみ使用できます。たとえば、次のようになります。
//
//      Synchronize(&UpdateCaption);
//
//   ここで、UpdateCaption は 次のようなコードになります。
//
//      void __fastcall Exec::UpdateCaption()
//      {
//        Form1->Caption = "スレッドで更新されました";
//      }
//---------------------------------------------------------------------------
__fastcall Exec::Exec(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall Exec::Execute()
{

	threadExec_c1 = NULL;
	threadExec_c2 = NULL;
	threadExec_c3 = NULL;
	threadExec_c4 = NULL;
	threadExec_c5 = NULL;
	threadExec_c6 = NULL;
	threadExec_c7 = NULL;
	threadExec_c8 = NULL;
	threadExec_c9 = NULL;
	threadExec_c10 = NULL;
	threadExec_c11 = NULL;
	threadExec_c12 = NULL;
	threadExec_c13 = NULL;
	threadExec_c14 = NULL;
	threadExec_c15 = NULL;
	threadExec_c16 = NULL;

}
//---------------------------------------------------------------------------
