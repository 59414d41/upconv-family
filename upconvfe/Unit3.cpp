//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit3.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   �d�v: VCL �ɂ�����I�u�W�F�N�g�̃��\�b�h�ƃv���p�e�B�́ASynchronize ���g����
//   �Ăяo����郁�\�b�h�ł̂ݎg�p�ł��܂��B���Ƃ��΁A���̂悤�ɂȂ�܂��B
//
//      Synchronize(&UpdateCaption);
//
//   �����ŁAUpdateCaption �� ���̂悤�ȃR�[�h�ɂȂ�܂��B
//
//      void __fastcall Exec::UpdateCaption()
//      {
//        Form1->Caption = "�X���b�h�ōX�V����܂���";
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
