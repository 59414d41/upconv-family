// CodeGear C++Builder
// Copyright (c) 1995, 2013 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'MECSUtils.pas' rev: 26.00 (Windows)

#ifndef MecsutilsHPP
#define MecsutilsHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.SysUtils.hpp>	// Pascal unit
#include <System.Classes.hpp>	// Pascal unit
#include <Winapi.Windows.hpp>	// Pascal unit
#include <Vcl.Graphics.hpp>	// Pascal unit
#include <System.Win.Registry.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Mecsutils
{
//-- type declarations -------------------------------------------------------
typedef System::Set<char, _DELPHI_SET_CHAR(0), _DELPHI_SET_CHAR(255)> TLeadBytes;

typedef char UTF8Char;

typedef char *PUTF8Char;

typedef System::WideChar UnicodeChar;

typedef System::WideChar *PUnicodeChar;

typedef System::RawByteString ByteString;

enum DECLSPEC_DENUM TElementType : unsigned char { etSingle, etLead, etTrail };

enum DECLSPEC_DENUM TEastAsianWidth : unsigned char { eawNeutral, eawFullwidth, eawHalfwidth, eawWide, eawNarrow, eawAmbiguous };

enum DECLSPEC_DENUM TCombiningType : unsigned char { ctBase, ctCombining };

struct DECLSPEC_DRECORD TIndexCount
{
public:
	int StartIndex;
	int EndIndex;
	int Count;
};


enum DECLSPEC_DENUM TNORM_FORM : unsigned char { NormalizationOther, NormalizationC, NormalizationD, NormalizationKC = 5, NormalizationKD };

//-- var, const, procedure ---------------------------------------------------
extern DELPHI_PACKAGE unsigned DefaultAnsiCodePage;
extern DELPHI_PACKAGE unsigned DefaultOEMCodePage;
extern DELPHI_PACKAGE unsigned DefaultLCID;
extern DELPHI_PACKAGE System::Uitypes::TFontCharset __fastcall CodePageToFontCharset(unsigned Codepage);
extern DELPHI_PACKAGE unsigned __fastcall FontCharsetToCodePage(System::Uitypes::TFontCharset FontCharset);
extern DELPHI_PACKAGE bool __fastcall IsFarEastCharSet(System::Uitypes::TFontCharset CharSet);
extern DELPHI_PACKAGE bool __fastcall IsFarEastLCID(unsigned Locale);
extern DELPHI_PACKAGE System::DynamicArray<System::Byte> __fastcall MecsAnsiBytesOf(const ByteString Val)/* overload */;
extern DELPHI_PACKAGE ByteString __fastcall MecsAnsiStringOf(const System::DynamicArray<System::Byte> Bytes);
extern DELPHI_PACKAGE System::DynamicArray<System::Byte> __fastcall MecsBytesOf(const ByteString Val)/* overload */;
extern DELPHI_PACKAGE System::DynamicArray<System::Byte> __fastcall MecsBytesOf(const System::UnicodeString Val)/* overload */;
extern DELPHI_PACKAGE System::DynamicArray<System::Byte> __fastcall MecsBytesOf(const System::WideString Val)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsStringOf(const System::DynamicArray<System::Byte> Bytes, ByteString &S)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsStringOf(const System::DynamicArray<System::Byte> Bytes, System::UnicodeString &S)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsStringOf(const System::DynamicArray<System::Byte> Bytes, System::WideString &S)/* overload */;
extern DELPHI_PACKAGE System::DynamicArray<System::Byte> __fastcall MecsWideBytesOf(const System::UnicodeString Val)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsWideStringOf(const System::DynamicArray<System::Byte> Bytes);
extern DELPHI_PACKAGE unsigned __fastcall MecsGetCodePage(const ByteString S);
extern DELPHI_PACKAGE TLeadBytes __fastcall MecsGetLeadBytes(unsigned Codepage = (unsigned)(0x0));
extern DELPHI_PACKAGE ByteString __fastcall MecsShrinkElement(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsStretchElement(const ByteString S)/* overload */;
extern DELPHI_PACKAGE TEastAsianWidth __fastcall MecsEastAsianWidth(System::UCS4Char U);
extern DELPHI_PACKAGE TCombiningType __fastcall MecsCombiningType(System::UCS4Char U);
extern DELPHI_PACKAGE bool __fastcall MecsIsNormalized(const System::WideString AText, TNORM_FORM NormForm);
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsMappingFix_CP932ToUnicode(const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsMappingFix_CP932ToUnicode(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsMappingFix_JISX0208ToJISX0213(const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsMappingFix_JISX0208ToJISX0213(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsMappingFix_JISX0208ToUnicode(const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsMappingFix_JISX0208ToUnicode(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsMappingFix_JISX0213ToJISX0208(const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsMappingFix_JISX0213ToJISX0208(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsMappingFix_UnicodeToCP932(const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsMappingFix_UnicodeToCP932(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsMappingFix_UnicodeToJISX0208(const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsMappingFix_UnicodeToJISX0208(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsNormalize(const System::WideString Src, System::WideString &Dst, TNORM_FORM NormForm);
extern DELPHI_PACKAGE int __fastcall MecsAnsiPos(const ByteString SubStr, const ByteString S, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsAnsiPos(const System::UnicodeString SubStr, const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsAnsiPos(const System::WideString SubStr, const System::WideString S)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsAnsiPosEx(const ByteString SubStr, const ByteString S, int Offset = 0x1, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsAnsiPosEx(const System::UnicodeString SubStr, const System::UnicodeString S, int Offset = 0x1)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsAnsiPosEx(const System::WideString SubStr, const System::WideString S, int Offset = 0x1)/* overload */;
extern DELPHI_PACKAGE char * __fastcall MecsAnsiStrPos(char * Str, char * SubStr, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE System::WideChar * __fastcall MecsAnsiStrPos(System::WideChar * Str, System::WideChar * SubStr)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharToElementIndex(const ByteString S, int Index, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharToElementIndex(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharToElementIndex(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE TIndexCount __fastcall MecsCharToElementIndexCount(const ByteString S, int Index, int Count, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE TIndexCount __fastcall MecsCharToElementIndexCount(const System::UnicodeString S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE TIndexCount __fastcall MecsCharToElementIndexCount(const System::WideString S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharLength(const ByteString S, int Index, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharLength(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharLength(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharToElementLen(const ByteString S, int MaxLen, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharToElementLen(const System::UnicodeString S, int MaxLen)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCharToElementLen(const System::WideString S, int MaxLen)/* overload */;
extern DELPHI_PACKAGE ByteString __fastcall MecsCopy(const ByteString S, int Index, int Count, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsCopy(const System::UnicodeString S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsCopy(const System::WideString S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsDelete(ByteString &S, int Index, int Count, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsDelete(System::UnicodeString &S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsDelete(System::WideString &S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCharIndex(const ByteString S, int Index, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCharIndex(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCharIndex(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCharLen(const ByteString S, int MaxLen, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCharLen(const System::UnicodeString S, int MaxLen)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCharLen(const System::WideString S, int MaxLen)/* overload */;
extern DELPHI_PACKAGE TElementType __fastcall MecsElementType(const ByteString S, int Index, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE TElementType __fastcall MecsElementType(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE TElementType __fastcall MecsElementType(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsInsert(const ByteString Source, ByteString &S, int Index, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsInsert(const System::UnicodeString Source, System::UnicodeString &S, int Index)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsInsert(const System::WideString Source, System::WideString &S, int Index)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsFullWidth(const ByteString S, int CharIndex)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsFullWidth(const ByteString S, int CharIndex, bool FarEast, unsigned CodePage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsFullWidth(const System::UnicodeString S, int CharIndex)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsFullWidth(const System::UnicodeString S, int CharIndex, bool FarEast)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsFullWidth(const System::WideString S, int CharIndex)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsFullWidth(const System::WideString S, int CharIndex, bool FarEast)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsFullWidth(const System::UCS4Char CodePoint)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsFullWidth(const System::UCS4Char CodePoint, bool FarEast)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsLeadElement(char TestChar, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsLeadElement(UnicodeChar TestChar)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsMECElement(char TestChar, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsMECElement(UnicodeChar TestChar)/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsTrailElement(char TestChar, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE bool __fastcall MecsIsTrailElement(UnicodeChar TestChar)/* overload */;
extern DELPHI_PACKAGE ByteString __fastcall MecsLeftStr(const ByteString AText, const int ACount, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsLeftStr(const System::UnicodeString AText, const int ACount)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsLeftStr(const System::WideString AText, const int ACount)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsLength(const ByteString S, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsLength(const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsLength(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE ByteString __fastcall MecsMidStr(const ByteString AText, const int AStart, const int ACount, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsMidStr(const System::UnicodeString AText, const int AStart, const int ACount)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsMidStr(const System::WideString AText, const int AStart, const int ACount)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsNextCharIndex(const ByteString S, int Index, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsNextCharIndex(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsNextCharIndex(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE ByteString __fastcall MecsReverseString(const ByteString AText, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsReverseString(const System::UnicodeString AText)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsReverseString(const System::WideString AText)/* overload */;
extern DELPHI_PACKAGE ByteString __fastcall MecsRightStr(const ByteString AText, const int ACount, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsRightStr(const System::UnicodeString AText, const int ACount)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsRightStr(const System::WideString AText, const int ACount)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsStrCharLength(const char * Str, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsStrCharLength(const PUnicodeChar Str)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsStrCharLength(const System::WideChar * Str)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsStrConv(const System::UnicodeString str, unsigned Conversion, unsigned Optional = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE ByteString __fastcall MecsStrConv(const ByteString str, unsigned Conversion, unsigned Optional = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE TElementType __fastcall MecsStrElementType(const char * Str, unsigned Index, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE TElementType __fastcall MecsStrElementType(const PUnicodeChar Str, unsigned Index)/* overload */;
extern DELPHI_PACKAGE TElementType __fastcall MecsStrElementType(const System::WideChar * Str, unsigned Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsStringWidth(const System::UnicodeString S, bool EAW = true)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsStringWidth(const System::WideString S, bool EAW = true)/* overload */;
extern DELPHI_PACKAGE unsigned __fastcall MecsStrLen(const char * Str, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE unsigned __fastcall MecsStrLen(const PUnicodeChar Str)/* overload */;
extern DELPHI_PACKAGE unsigned __fastcall MecsStrLen(const System::WideChar * Str)/* overload */;
extern DELPHI_PACKAGE char * __fastcall MecsStrNextChar(const char * Str, unsigned Codepage = (unsigned)(0x0))/* overload */;
extern DELPHI_PACKAGE PUnicodeChar __fastcall MecsStrNextChar(const PUnicodeChar Str)/* overload */;
extern DELPHI_PACKAGE System::WideChar * __fastcall MecsStrNextChar(const System::WideChar * Str)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCCSLength(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCCSLength(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCCSToElementIndex(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCCSToElementIndex(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE TIndexCount __fastcall MecsCCSToElementIndexCount(const System::UnicodeString S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE TIndexCount __fastcall MecsCCSToElementIndexCount(const System::WideString S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCCSToElementLen(const System::UnicodeString S, int MaxLen)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsCCSToElementLen(const System::WideString S, int MaxLen)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCCSIndex(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCCSIndex(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCCSLen(const System::UnicodeString S, int MaxLen)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsElementToCCSLen(const System::WideString S, int MaxLen)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsDeleteC(System::UnicodeString &S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsDeleteC(System::WideString &S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsLengthC(const System::UnicodeString S)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsLengthC(const System::WideString S)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsCopyC(const System::UnicodeString S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsCopyC(const System::WideString S, int Index, int Count)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsInsertC(const System::UnicodeString Source, System::UnicodeString &S, int Index)/* overload */;
extern DELPHI_PACKAGE void __fastcall MecsInsertC(const System::WideString Source, System::WideString &S, int Index)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsLeftStrC(const System::UnicodeString AText, const int ACount)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsLeftStrC(const System::WideString AText, const int ACount)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsMidStrC(const System::UnicodeString AText, const int AStart, const int ACount)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsMidStrC(const System::WideString AText, const int AStart, const int ACount)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsNextCCSIndex(const System::UnicodeString S, int Index)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsNextCCSIndex(const System::WideString S, int Index)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsReverseStringC(const System::UnicodeString AText)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsReverseStringC(const System::WideString AText)/* overload */;
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsRightStrC(const System::UnicodeString AText, const int ACount)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall MecsRightStrC(const System::WideString AText, const int ACount)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsStrCCSLength(const PUnicodeChar Str)/* overload */;
extern DELPHI_PACKAGE int __fastcall MecsStrCCSLength(const System::WideChar * Str)/* overload */;
extern DELPHI_PACKAGE unsigned __fastcall MecsStrLenC(const PUnicodeChar Str)/* overload */;
extern DELPHI_PACKAGE unsigned __fastcall MecsStrLenC(const System::WideChar * Str)/* overload */;
extern DELPHI_PACKAGE PUnicodeChar __fastcall MecsStrNextCCS(const PUnicodeChar Str)/* overload */;
extern DELPHI_PACKAGE System::WideChar * __fastcall MecsStrNextCCS(const System::WideChar * Str)/* overload */;
extern DELPHI_PACKAGE System::WideString __fastcall AnsiToUTF16(const ByteString AText, unsigned Codepage = (unsigned)(0x0));
extern DELPHI_PACKAGE System::UCS4String __fastcall AnsiToUTF32(const ByteString AText, unsigned Codepage = (unsigned)(0x0));
extern DELPHI_PACKAGE ByteString __fastcall AnsiToUTF8(const ByteString AText, unsigned Codepage = (unsigned)(0x0));
extern DELPHI_PACKAGE System::WideString __fastcall CodePointToUTF16(const System::UCS4Char CodePoint);
extern DELPHI_PACKAGE ByteString __fastcall CodePointToUTF8(const System::UCS4Char CodePoint);
extern DELPHI_PACKAGE bool __fastcall ConvertString(unsigned SrcCodepage, unsigned DstCodepage, const ByteString SrcStr, ByteString &DstStr);
extern DELPHI_PACKAGE void __fastcall TrimNullTerm(System::WideString &S)/* overload */;
extern DELPHI_PACKAGE void __fastcall TrimNullTerm(System::UCS4String &S)/* overload */;
extern DELPHI_PACKAGE void __fastcall TrimNullTerm(System::DynamicArray<System::Byte> &S)/* overload */;
extern DELPHI_PACKAGE bool __fastcall ConvertUnicodeToMultiByte(unsigned DstCodepage, const System::WideString SrcStr, ByteString &DstStr);
extern DELPHI_PACKAGE bool __fastcall ConvertMultiByteToUnicode(unsigned SrcCodepage, const ByteString SrcStr, System::WideString &DstStr);
extern DELPHI_PACKAGE ByteString __fastcall UTF32ToAnsi(const System::UCS4String UCS4Text, unsigned Codepage = (unsigned)(0x0));
extern DELPHI_PACKAGE System::WideString __fastcall UTF32ToUTF16(const System::UCS4String UCS4Text);
extern DELPHI_PACKAGE ByteString __fastcall UTF32ToUTF8(const System::UCS4String UCS4Text);
extern DELPHI_PACKAGE ByteString __fastcall UTF16ToAnsi(const System::WideString WText, unsigned Codepage = (unsigned)(0x0));
extern DELPHI_PACKAGE System::UCS4String __fastcall UTF16ToUTF32(const System::WideString WText);
extern DELPHI_PACKAGE ByteString __fastcall UTF16ToUTF8(const System::WideString WText);
extern DELPHI_PACKAGE ByteString __fastcall UTF8ToAnsi(const ByteString UTF8Text, unsigned Codepage);
extern DELPHI_PACKAGE System::WideString __fastcall UTF8ToUTF16(const ByteString UTF8Text);
extern DELPHI_PACKAGE System::UCS4String __fastcall UTF8ToUTF32(const ByteString UTF8Text);
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsCodePageToCharset(const System::Word Codepage);
extern DELPHI_PACKAGE System::Word __fastcall MecsCharsetToCodepage(const System::UnicodeString Charset);
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsHTMLDecode(const System::UnicodeString s);
extern DELPHI_PACKAGE System::UnicodeString __fastcall MecsHTMLEncode(const System::UnicodeString s);
}	/* namespace Mecsutils */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_MECSUTILS)
using namespace Mecsutils;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// MecsutilsHPP
