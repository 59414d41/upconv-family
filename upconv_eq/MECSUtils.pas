{ **************************************************************************** }
{                                                                              }
{                         MECS Utilities Unit  ver1.57                         }
{               Copyright (c) 2008-2016  Hideaki Tominaga (DEKO)               }
{                                                                              }
{ ---------------------------------------------------------------------------- }
{                         http://ht-deko.minim.ne.jp/                          }
{                           deko@ht-deko.minim.ne.jp                           }
{ ---------------------------------------------------------------------------- }
{                    Reference manual available (Japanese).                    }
{                    http://ht-deko.minim.ne.jp/tech021.html                   }
{ **************************************************************************** }

unit MECSUtils;

interface

uses
  {$IF CompilerVersion >= 23.0}
    {$DEFINE XE2_OR_LATER}
    System.SysUtils, System.Classes
    {$IFDEF MSWINDOWS}
    , Winapi.Windows, Vcl.Graphics, System.Win.Registry
    {$ENDIF}
  {$ELSE}
    SysUtils, Classes, Windows, Graphics, Registry
  {$IFEND}
  ;

type
  TLeadBytes = set of AnsiChar;
  UTF8Char =  type AnsiChar;
  PUTF8Char = type PAnsiChar;

  {$IFDEF UNICODE}
  UnicodeChar = type Char;
  PUnicodeChar = type PChar;
  {$ENDIF}

  {$IFNDEF UNICODE}
  {$IFNDEF VER185}
  TBytes = array of Byte;
  {$ENDIF}
  {$ENDIF}

  {$IFDEF UNICODE}
  ByteString = type RawByteString;
  {$ELSE}
  ByteString = type AnsiString;
  {$ENDIF}

  TElementType = (etSingle, etLead, etTrail);

  TEastAsianWidth = (eawNeutral, eawFullwidth, eawHalfwidth, eawWide, eawNarrow, eawAmbiguous);
  TCombiningType = (ctBase, ctCombining);

  TIndexCount =
    record
      StartIndex: Integer;
      EndIndex: Integer;
      Count: Integer;
    end;

  // Normalization forms
  TNORM_FORM =
    (
      NormalizationOther = $00,
      NormalizationC     = $01, // Normalization Form Canonical Composition(NFC)
      NormalizationD     = $02, // Normalization Form Canonical Decomposition(NFD)
      NormalizationKC    = $05, // Normalization Form Compatibility Composition(NFKC)
      NormalizationKD    = $06  // Normalization Form Compatibility Decomposition(NFKD)
    );

// for Codepage
// -----------------------------------------------------------------------------
// CodePageToFontCharset
function CodePageToFontCharset(Codepage: DWORD): TFontCharset;

// FontCharsetToCodePage
function FontCharsetToCodePage(FontCharset: TFontCharset): DWORD;

// IsFarEastCharSet
function IsFarEastCharSet(CharSet: TFontCharSet): Boolean;

// IsFarEastLCID
function IsFarEastLCID(Locale: LCID): Boolean;

// for TBytes
// -----------------------------------------------------------------------------

// MecsAnsiBytesOf
function MecsAnsiBytesOf(const Val: ByteString): TBytes; overload;

// MecsAnsiStringOf
function MecsAnsiStringOf(const Bytes: TBytes): ByteString;

// MecsBytesOf
function MecsBytesOf(const Val: ByteString): TBytes; overload;
{$IFDEF UNICODE}
function MecsBytesOf(const Val: UnicodeString): TBytes; overload;
{$ENDIF}
function MecsBytesOf(const Val: WideString): TBytes; overload;

// MecsStringOf
procedure MecsStringOf(const Bytes: TBytes; var S: ByteString); overload;
{$IFDEF UNICODE}
procedure MecsStringOf(const Bytes: TBytes; var S: UnicodeString); overload;
{$ENDIF}
procedure MecsStringOf(const Bytes: TBytes; var S: WideString); overload;

// MecsWideBytesOf
{$IFDEF UNICODE}
function MecsWideBytesOf(const Val: UnicodeString): TBytes; overload;
{$ELSE}
function MecsWideBytesOf(const Val: WideString): TBytes; overload;
{$ENDIF}

// MecsWideStringOf
{$IFDEF UNICODE}
function MecsWideStringOf(const Bytes: TBytes): UnicodeString;
{$ELSE}
function MecsWideStringOf(const Bytes: TBytes): WideString;
{$ENDIF}

// for AnsiString
// -----------------------------------------------------------------------------
// MecsGetCodePage
function MecsGetCodePage(const S: ByteString): DWORD;

// MecsGetLeadBytes
function MecsGetLeadBytes(Codepage: DWORD = 0): TLeadBytes;

// MecsShrinkElement
function MecsShrinkElement(const S: WideString): ByteString; overload;

// MecsStretchElement
function MecsStretchElement(const S: ByteString): WideString; overload;

// for UnicodeString/WideString
// -----------------------------------------------------------------------------
// MecsEastAsianWidth
function MecsEastAsianWidth(U: UCS4Char): TEastAsianWidth;

// MecsIsNormalized
function MecsIsNormalized(const AText: WideString; NormForm: TNORM_FORM): Boolean;

// MecsMappingFix_CP932ToUnicode
{$IFDEF UNICODE}
function MecsMappingFix_CP932ToUnicode(const S: UnicodeString): UnicodeString; overload;
{$ENDIF}
function MecsMappingFix_CP932ToUnicode(const S: WideString): WideString; overload;

// MecsMappingFix_JISX0208ToJISX0213
{$IFDEF UNICODE}
function MecsMappingFix_JISX0208ToJISX0213(const S: UnicodeString): UnicodeString; overload;
{$ENDIF}
function MecsMappingFix_JISX0208ToJISX0213(const S: WideString): WideString; overload;

// MecsMappingFix_JISX0208ToUnicode
{$IFDEF UNICODE}
function MecsMappingFix_JISX0208ToUnicode(const S: UnicodeString): UnicodeString; overload;
{$ENDIF}
function MecsMappingFix_JISX0208ToUnicode(const S: WideString): WideString; overload;

// MecsMappingFix_JISX0213ToJISX0208
{$IFDEF UNICODE}
function MecsMappingFix_JISX0213ToJISX0208(const S: UnicodeString): UnicodeString; overload;
{$ENDIF}
function MecsMappingFix_JISX0213ToJISX0208(const S: WideString): WideString; overload;

// MecsMappingFix_UnicodeToCP932
{$IFDEF UNICODE}
function MecsMappingFix_UnicodeToCP932(const S: UnicodeString): UnicodeString; overload;
{$ENDIF}
function MecsMappingFix_UnicodeToCP932(const S: WideString): WideString; overload;

// MecsMappingFix_UnicodeToJISX0208
{$IFDEF UNICODE}
function MecsMappingFix_UnicodeToJISX0208(const S: UnicodeString): UnicodeString; overload;
{$ENDIF}
function MecsMappingFix_UnicodeToJISX0208(const S: WideString): WideString; overload;
function MecsNormalize(const Src: WideString; var Dst: WideString; NormForm: TNORM_FORM): Boolean;

// MecsStringWidth
{$IFDEF UNICODE}
function MecsStringWidth(const S: UnicodeString; EAW: Boolean = True): Integer; overload;
{$ENDIF}
function MecsStringWidth(const S: WideString; EAW: Boolean = True): Integer; overload;

// StrLen
{$IFNDEF UNICODE}
function StrLen(const Str: PWideChar): Cardinal;
{$ENDIF}

// StrPos
{$IFNDEF UNICODE}
function StrPos(const Str1, Str2: PWideChar): PWideChar;
{$ENDIF}

// for AnsiString/UnicodeString/WideString (Common)
// -----------------------------------------------------------------------------
// MecsAnsiPos
function MecsAnsiPos(const SubStr, S: ByteString; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsAnsiPos(const SubStr, S: UnicodeString): Integer; overload;
{$ENDIF}
function MecsAnsiPos(const SubStr, S: WideString): Integer; overload;

// MeceAnsiPosEx
function MecsAnsiPosEx(const SubStr, S: ByteString; Offset: Integer = 1; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsAnsiPosEx(const SubStr, S: UnicodeString; Offset: Integer = 1): Integer; overload;
{$ENDIF}
function MecsAnsiPosEx(const SubStr, S: WideString; Offset: Integer = 1): Integer; overload;

// MecsAnsiStrPos
function MecsAnsiStrPos(Str, SubStr: PAnsiChar; Codepage: DWORD = 0): PAnsiChar; overload;
function MecsAnsiStrPos(Str, SubStr: PWideChar): PWideChar; overload;

// MecsCharToElementIndex
function MecsCharToElementIndex(const S: ByteString; Index: Integer; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsCharToElementIndex(const S: UnicodeString; Index: Integer): Integer; overload;
{$ENDIF}
function MecsCharToElementIndex(const S: WideString; Index: Integer): Integer; overload;

// MecsCharToElementIndexCount
function MecsCharToElementIndexCount(const S: ByteString; Index, Count: Integer; Codepage: DWORD = 0): TIndexCount; overload;
{$IFDEF UNICODE}
function MecsCharToElementIndexCount(const S: UnicodeString; Index, Count: Integer): TIndexCount; overload;
{$ENDIF}
function MecsCharToElementIndexCount(const S: WideString; Index, Count: Integer): TIndexCount; overload;

// MecsCharToElementLen
function MecsCharToElementLen(const S: ByteString; MaxLen: Integer; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsCharToElementLen(const S: UnicodeString; MaxLen: Integer): Integer; overload;
{$ENDIF}
function MecsCharToElementLen(const S: WideString; MaxLen: Integer): Integer; overload;

// MecsCharLength
function MecsCharLength(const S: ByteString; Index: Integer; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsCharLength(const S: UnicodeString; Index: Integer): Integer; overload;
{$ENDIF}
function MecsCharLength(const S: WideString; Index: Integer): Integer; overload;

// MecsCopy
function MecsCopy(const S: ByteString; Index, Count: Integer; Codepage: DWORD = 0): ByteString; overload;
{$IFDEF UNICODE}
function MecsCopy(const S: UnicodeString; Index, Count: Integer): UnicodeString; overload;
{$ENDIF}
function MecsCopy(const S: WideString; Index, Count: Integer): WideString; overload;

// MecsDelete
procedure MecsDelete(var S: ByteString; Index, Count: Integer; Codepage: DWORD = 0); overload;
{$IFDEF UNICODE}
procedure MecsDelete(var S: UnicodeString; Index, Count: Integer); overload;
{$ENDIF}
procedure MecsDelete(var S: WideString; Index, Count: Integer); overload;

// MecsElementToCharIndex
function MecsElementToCharIndex(const S: ByteString; Index: Integer; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsElementToCharIndex(const S: UnicodeString; Index: Integer): Integer; overload;
{$ENDIF}
function MecsElementToCharIndex(const S: WideString; Index: Integer): Integer; overload;

// MecsElementToCharLen
function MecsElementToCharLen(const S: ByteString; MaxLen: Integer; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsElementToCharLen(const S: UnicodeString; MaxLen: Integer): Integer; overload;
{$ENDIF}
function MecsElementToCharLen(const S: WideString; MaxLen: Integer): Integer; overload;

// MecsElementType
function MecsElementType(const S: ByteString; Index: Integer; Codepage: DWORD = 0): TElementType; overload;
{$IFDEF UNICODE}
function MecsElementType(const S: UnicodeString; Index: Integer): TElementType; overload;
{$ENDIF}
function MecsElementType(const S: WideString; Index: Integer): TElementType; overload;

// MecsInsert
procedure MecsInsert(const Source: ByteString; var S: ByteString; Index: Integer; Codepage: DWORD = 0); overload;
{$IFDEF UNICODE}
procedure MecsInsert(const Source: UnicodeString; var S: UnicodeString; Index: Integer); overload;
{$ENDIF}
procedure MecsInsert(const Source: WideString; var S: WideString; Index: Integer); overload;

// MecsIsFullWidth
function MecsIsFullWidth(const S: ByteString; CharIndex: Integer): Boolean; overload;
function MecsIsFullWidth(const S: ByteString; CharIndex: Integer; FarEast: Boolean; CodePage: DWORD = 0): Boolean; overload;
{$IFDEF UNICODE}
function MecsIsFullWidth(const S: UnicodeString; CharIndex: Integer): Boolean; overload;
function MecsIsFullWidth(const S: UnicodeString; CharIndex: Integer; FarEast: Boolean): Boolean; overload;
{$ENDIF}
function MecsIsFullWidth(const S: WideString; CharIndex: Integer): Boolean; overload;
function MecsIsFullWidth(const S: WideString; CharIndex: Integer; FarEast: Boolean): Boolean; overload;
function MecsIsFullWidth(const CodePoint: UCS4Char): Boolean; overload;
function MecsIsFullWidth(const CodePoint: UCS4Char; FarEast: Boolean): Boolean; overload;

// MecsIsLeadElement
function MecsIsLeadElement(TestChar: AnsiChar; Codepage: DWORD = 0): Boolean; overload;
{$IFDEF UNICODE}
function MecsIsLeadElement(TestChar: UnicodeChar): Boolean; overload;
{$ELSE}
function MecsIsLeadElement(TestChar: WideChar): Boolean; overload;
{$ENDIF}

// MecsIsMECElement
function MecsIsMECElement(TestChar: AnsiChar; Codepage: DWORD = 0): Boolean; overload;
{$IFDEF UNICODE}
function MecsIsMECElement(TestChar: UnicodeChar): Boolean; overload;
{$ELSE}
function MecsIsMECElement(TestChar: WideChar): Boolean; overload;
{$ENDIF}

// MecsIsTrailElement
function MecsIsTrailElement(TestChar: AnsiChar; Codepage: DWORD = 0): Boolean; overload;
{$IFDEF UNICODE}
function MecsIsTrailElement(TestChar: UnicodeChar): Boolean; overload;
{$ELSE}
function MecsIsTrailElement(TestChar: WideChar): Boolean; overload;
{$ENDIF}

// MecsLeftStr
function MecsLeftStr(const AText: ByteString; const ACount: Integer; Codepage: DWORD = 0): ByteString; overload;
{$IFDEF UNICODE}
function MecsLeftStr(const AText: UnicodeString; const ACount: Integer): UnicodeString; overload;
{$ENDIF}
function MecsLeftStr(const AText: WideString; const ACount: Integer): WideString; overload;

// MecsLength
function MecsLength(const S: ByteString; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsLength(const S: UnicodeString): Integer; overload;
{$ENDIF}
function MecsLength(const S: WideString): Integer; overload;

// MecsMidStr
function MecsMidStr(const AText: ByteString; const AStart, ACount: Integer; Codepage: DWORD = 0): ByteString; overload;
{$IFDEF UNICODE}
function MecsMidStr(const AText: UnicodeString; const AStart, ACount: Integer): UnicodeString; overload;
{$ENDIF}
function MecsMidStr(const AText: WideString; const AStart, ACount: Integer): WideString; overload;

// MecsNextCharIndex
function MecsNextCharIndex(const S: ByteString; Index: Integer; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsNextCharIndex(const S: UnicodeString; Index: Integer): Integer; overload;
{$ENDIF}
function MecsNextCharIndex(const S: WideString; Index: Integer): Integer; overload;

// MecsReverseString
function MecsReverseString(const AText: ByteString; Codepage: DWORD = 0): ByteString; overload;
{$IFDEF UNICODE}
function MecsReverseString(const AText: UnicodeString): UnicodeString; overload;
{$ENDIF}
function MecsReverseString(const AText: WideString): WideString; overload;

// MecsRightStr
function MecsRightStr(const AText: ByteString; const ACount: Integer; Codepage: DWORD = 0): ByteString; overload;
{$IFDEF UNICODE}
function MecsRightStr(const AText: UnicodeString; const ACount: Integer): UnicodeString; overload;
{$ENDIF}
function MecsRightStr(const AText: WideString; const ACount: Integer): WideString; overload;

// MecsStrCharLength
function MecsStrCharLength(const Str: PAnsiChar; Codepage: DWORD = 0): Integer; overload;
{$IFDEF UNICODE}
function MecsStrCharLength(const Str: PUnicodeChar): Integer; overload;
{$ENDIF}
function MecsStrCharLength(const Str: PWideChar): Integer; overload;

// MecsStrConv
function MecsStrConv(const str: ByteString; Conversion: DWORD; Optional: LCID = 0): ByteString; overload;
{$IFDEF UNICODE}
function MecsStrConv(const str: UnicodeString; Conversion: DWORD; Optional: LCID = 0): UnicodeString; overload;
{$ELSE}
function MecsStrConv(const str: WideString; Conversion: DWORD; Optional: LCID = 0): WideString; overload;
{$ENDIF}

// MecsStrElementType
function MecsStrElementType(const Str: PAnsiChar; Index: Cardinal; Codepage: DWORD = 0): TElementType; overload;
{$IFDEF UNICODE}
function MecsStrElementType(const Str: PUnicodeChar; Index: Cardinal): TElementType; overload;
{$ENDIF}
function MecsStrElementType(const Str: PWideChar; Index: Cardinal): TElementType; overload;

// MecsStrLen
function MecsStrLen(const Str: PAnsiChar; Codepage: DWORD = 0): Cardinal; overload;
{$IFDEF UNICODE}
function MecsStrLen(const Str: PUnicodeChar): Cardinal; overload;
{$ENDIF}
function MecsStrLen(const Str: PWideChar): Cardinal; overload;

// MecsStrNextChar
function MecsStrNextChar(const Str: PAnsiChar; Codepage: DWORD = 0): PAnsiChar; overload;
{$IFDEF UNICODE}
function MecsStrNextChar(const Str: PUnicodeChar): PUnicodeChar; overload;
{$ENDIF}
function MecsStrNextChar(const Str: PWideChar): PWideChar; overload;

// for UnicodeString/WideString (Combining Character Sequence)
// -----------------------------------------------------------------------------
// MecsCombiningType
function MecsCombiningType(U: UCS4Char): TCombiningType;

// MecsCCSLength
{$IFDEF UNICODE}
function MecsCCSLength(const S: UnicodeString; Index: Integer): Integer; overload;
{$ENDIF}
function MecsCCSLength(const S: WideString; Index: Integer): Integer; overload;

// MecsCCSToElementIndex
{$IFDEF UNICODE}
function MecsCCSToElementIndex(const S: UnicodeString; Index: Integer): Integer; overload;
{$ENDIF}
function MecsCCSToElementIndex(const S: WideString; Index: Integer): Integer; overload;

// MecsCCSToElementIndexCount
{$IFDEF UNICODE}
function MecsCCSToElementIndexCount(const S: UnicodeString; Index, Count: Integer): TIndexCount; overload;
{$ENDIF}
function MecsCCSToElementIndexCount(const S: WideString; Index, Count: Integer): TIndexCount; overload;

// MecsCCSToElementLen
{$IFDEF UNICODE}
function MecsCCSToElementLen(const S: UnicodeString; MaxLen: Integer): Integer; overload;
{$ENDIF}
function MecsCCSToElementLen(const S: WideString; MaxLen: Integer): Integer; overload;

// MecsDeleteC
{$IFDEF UNICODE}
procedure MecsDeleteC(var S: UnicodeString; Index, Count: Integer); overload;
{$ENDIF}
procedure MecsDeleteC(var S: WideString; Index, Count: Integer); overload;

// MecsElementToCCSIndex
{$IFDEF UNICODE}
function MecsElementToCCSIndex(const S: UnicodeString; Index: Integer): Integer; overload;
{$ENDIF}
function MecsElementToCCSIndex(const S: WideString; Index: Integer): Integer; overload;

// MecsElementToCCSLen
{$IFDEF UNICODE}
function MecsElementToCCSLen(const S: UnicodeString; MaxLen: Integer): Integer; overload;
{$ENDIF}
function MecsElementToCCSLen(const S: WideString; MaxLen: Integer): Integer; overload;

// MecsLengthC
{$IFDEF UNICODE}
function MecsLengthC(const S: UnicodeString): Integer; overload;
{$ENDIF}
function MecsLengthC(const S: WideString): Integer; overload;

// MecsCopyC
{$IFDEF UNICODE}
function MecsCopyC(const S: UnicodeString; Index, Count: Integer): UnicodeString; overload;
{$ENDIF}
function MecsCopyC(const S: WideString; Index, Count: Integer): WideString; overload;

// MecsInsertC
{$IFDEF UNICODE}
procedure MecsInsertC(const Source: UnicodeString; var S: UnicodeString; Index: Integer); overload;
{$ENDIF}
procedure MecsInsertC(const Source: WideString; var S: WideString; Index: Integer); overload;

// MecsLeftStrC
{$IFDEF UNICODE}
function MecsLeftStrC(const AText: UnicodeString; const ACount: Integer): UnicodeString; overload;
{$ENDIF}
function MecsLeftStrC(const AText: WideString; const ACount: Integer): WideString; overload;

// MecsMidStrC
{$IFDEF UNICODE}
function MecsMidStrC(const AText: UnicodeString; const AStart, ACount: Integer): UnicodeString; overload;
{$ENDIF}
function MecsMidStrC(const AText: WideString; const AStart, ACount: Integer): WideString; overload;

// MecsNextCCSIndex
{$IFDEF UNICODE}
function MecsNextCCSIndex(const S: UnicodeString; Index: Integer): Integer; overload;
{$ENDIF}
function MecsNextCCSIndex(const S: WideString; Index: Integer): Integer; overload;

// MecsReverseStringC
{$IFDEF UNICODE}
function MecsReverseStringC(const AText: UnicodeString): UnicodeString; overload;
{$ENDIF}
function MecsReverseStringC(const AText: WideString): WideString; overload;

// MecsRightStrC
{$IFDEF UNICODE}
function MecsRightStrC(const AText: UnicodeString; const ACount: Integer): UnicodeString; overload;
{$ENDIF}
function MecsRightStrC(const AText: WideString; const ACount: Integer): WideString; overload;

// MecsStrCCSLength
{$IFDEF UNICODE}
function MecsStrCCSLength(const Str: PUnicodeChar): Integer; overload;
{$ENDIF}
function MecsStrCCSLength(const Str: PWideChar): Integer; overload;

// MecsStrLenC
{$IFDEF UNICODE}
function MecsStrLenC(const Str: PUnicodeChar): Cardinal; overload;
{$ENDIF}
function MecsStrLenC(const Str: PWideChar): Cardinal; overload;

// MecsStrNextCCS
{$IFDEF UNICODE}
function MecsStrNextCCS(const Str: PUnicodeChar): PUnicodeChar; overload;
{$ENDIF}
function MecsStrNextCCS(const Str: PWideChar): PWideChar; overload;

// for Convert
// -----------------------------------------------------------------------------
// AnsiToUTF32
function AnsiToUTF32(const AText: ByteString; Codepage: DWORD = 0): UCS4String;

// AnsiToUTF16
function AnsiToUTF16(const AText: ByteString; Codepage: DWORD = 0): WideString;

// AnsiToUTF8
{$IFDEF UNICODE}
function AnsiToUTF8(const AText: ByteString; Codepage: DWORD = 0): ByteString;
{$ELSE}
function AnsiToUTF8(const AText: ByteString; Codepage: DWORD = 0): UTF8String;
{$ENDIF}

// CodePointToUTF16
function CodePointToUTF16(const CodePoint: UCS4Char): WideString;

// CodePointToUTF8
{$IFDEF UNICODE}
function CodePointToUTF8(const CodePoint: UCS4Char): ByteString;
{$ELSE}
function CodePointToUTF8(const CodePoint: UCS4Char): UTF8String;
{$ENDIF}

// ConvertMultiByteToUnicode
function ConvertMultiByteToUnicode(SrcCodepage: DWORD; const SrcStr: ByteString; var DstStr: WideString): Boolean;

// ConvertString
function ConvertString(SrcCodepage, DstCodepage: DWORD; const SrcStr: ByteString; var DstStr: ByteString): Boolean;

// TrimNullTerm
procedure TrimNullTerm(var S: WideString); overload;
procedure TrimNullTerm(var S: UCS4String); overload;
procedure TrimNullTerm(var S: TBytes); overload;

// ConvertUnicodeToMultiByte
function ConvertUnicodeToMultiByte(DstCodepage: DWORD; const SrcStr: WideString; var DstStr: ByteString): Boolean;

// UTF32ToAnsi
function UTF32ToAnsi(const UCS4Text: UCS4String; Codepage: DWORD = 0): ByteString;

// UTF32ToUTF16
function UTF32ToUTF16(const UCS4Text: UCS4String): WideString;

// UTF32ToUTF8
{$IFDEF UNICODE}
function UTF32ToUTF8(const UCS4Text: UCS4String): ByteString;
{$ELSE}
function UTF32ToUTF8(const UCS4Text: UCS4String): UTF8String;
{$ENDIF}

// UTF16ToAnsi
function UTF16ToAnsi(const WText: WideString; Codepage: DWORD = 0): ByteString;

// UTF16ToUTF32
function UTF16ToUTF32(const WText: WideString): UCS4String;

// UTF16ToUTF32
{$IFDEF UNICODE}
function UTF16ToUTF8(const WText: WideString): ByteString;
{$ELSE}
function UTF16ToUTF8(const WText: WideString): UTF8String;
{$ENDIF}

// UTF8ToAnsi
{$IFDEF UNICODE}
function UTF8ToAnsi(const UTF8Text: ByteString; Codepage: DWORD): ByteString;
{$ELSE}
function UTF8ToAnsi(const UTF8Text: UTF8String; Codepage: DWORD): ByteString;
{$ENDIF}

// UTF8ToUTF32
{$IFDEF UNICODE}
function UTF8ToUTF32(const UTF8Text: ByteString): UCS4String;
{$ELSE}
function UTF8ToUTF32(const UTF8Text: UTF8String): UCS4String;
{$ENDIF}

// UTF8ToUTF16
{$IFDEF UNICODE}
function UTF8ToUTF16(const UTF8Text: ByteString): WideString;
{$ELSE}
function UTF8ToUTF16(const UTF8Text: UTF8String): WideString;
{$ENDIF}

// for HTML
// -----------------------------------------------------------------------------
// MecsCodepageToCharset
{$IFDEF UNICODE}
function MecsCodePageToCharset(const Codepage: WORD): UnicodeString;
{$ELSE}
function MecsCodePageToCharset(const Codepage: WORD): WideString;
{$ENDIF}

// MecsCharsetToCodepage
{$IFDEF UNICODE}
function MecsCharsetToCodepage(const Charset: UnicodeString): WORD;
{$ELSE}
function MecsCharsetToCodepage(const Charset: WideString): WORD;
{$ENDIF}

// MecsHTMLDecode
{$IFDEF UNICODE}
function MecsHTMLDecode(const s: UnicodeString): UnicodeString;
{$ELSE}
function MecsHTMLDecode(const s: WideString): WideString;
{$ENDIF}

// MecsHTMLEncode
{$IFDEF UNICODE}
function MecsHTMLEncode(const s: UnicodeString): UnicodeString;
{$ELSE}
function MecsHTMLEncode(const s: WideString): WideString;
{$ENDIF}

var
  DefaultAnsiCodePage : DWORD;
  DefaultOEMCodePage  : DWORD;
  DefaultLCID         : LCID;

implementation

// for Codepage/Locale/CharSet
// -----------------------------------------------------------------------------

// CodePageToFontCharset
function CodePageToFontCharset(Codepage: DWORD): TFontCharset;
var
  lpSrc: DWORD;
  lpCs: TCharsetInfo;
begin
  lpSrc := CodePage;
  if TranslateCharsetInfo(lpSrc, lpCs, TCI_SRCCODEPAGE) then
    result := lpCs.ciCharset
  else
    result := DEFAULT_CHARSET;
end;

// FontCharsetToCodePage
function FontCharsetToCodePage(FontCharset: TFontCharset): DWORD;
var
  lpSrc: DWORD;
  lpCs: TCharsetInfo;
begin
  lpSrc := FontCharset;
  if TranslateCharsetInfo(lpSrc, lpCs, TCI_SRCCHARSET) then
    result := lpCs.ciACP
  else
    result := DefaultAnsiCodePage;
end;

// IsFarEastCharSet
function IsFarEastCharSet(CharSet: TFontCharSet): Boolean;
begin
  case CharSet of
    SHIFTJIS_CHARSET,
    GB2312_CHARSET,
    CHINESEBIG5_CHARSET,
    HANGEUL_CHARSET,
    JOHAB_CHARSET:
      result := True;
  else
    result := False;
  end;
end;

// IsFarEastLCID
function IsFarEastLCID(Locale: LCID): Boolean;
begin
  case Locale of
    1028, // Taiwan
    1041, // Japan
    1042, // Korea
    2052, // China
    3076, // Hong Kong
    4100: // Singapore
      result := True;
  else
    result := False;
  end;
end;


// for TBytes
// -----------------------------------------------------------------------------

// MecsAnsiBytesOf
function MecsAnsiBytesOf(const Val: ByteString): TBytes; overload;
begin
  result := MecsBytesOf(Val);
end;

// MecsAnsiStringOf
function MecsAnsiStringOf(const Bytes: TBytes): ByteString;
begin
  MecsStringOf(Bytes, result);
end;

// MecsBytesOf
function MecsBytesOf(const Val: ByteString): TBytes;
begin
  SetLength(result, Length(Val) + 1);
  Move(Val[1], result[0], Length(Val));
  result[Length(Val)] := $00;
end;

{$IFDEF UNICODE}
function MecsBytesOf(const Val: UnicodeString): TBytes;
begin
  SetLength(result, Length(Val) * 2 + 2);
  Move(Val[1], result[0], Length(Val) * 2);
  result[Length(Val) * 2    ] := $00;
  result[Length(Val) * 2 + 1] := $00;
end;
{$ENDIF}

function MecsBytesOf(const Val: WideString): TBytes;
begin
  SetLength(result, Length(Val) * 2 + 2);
  Move(Val[1], result[0], Length(Val) * 2);
  result[Length(Val) * 2    ] := $00;
  result[Length(Val) * 2 + 1] := $00;
end;

// MecsStringOf
procedure MecsStringOf(const Bytes: TBytes; var S: ByteString);
begin
  if Length(Bytes) = 0 then
    S := ''
  else
    S := PAnsiChar(@Bytes[0]);
end;

{$IFDEF UNICODE}
procedure MecsStringOf(const Bytes: TBytes; var S: UnicodeString);
begin
  if Length(Bytes) = 0 then
    S := ''
  else
    S := PWideChar(@Bytes[0]);
end;
{$ENDIF}

procedure MecsStringOf(const Bytes: TBytes; var S: WideString);
begin
  if Length(Bytes) = 0 then
    S := ''
  else
    S := PWideChar(@Bytes[0]);
end;

// MecsWideBytesOf
{$IFDEF UNICODE}
function MecsWideBytesOf(const Val: UnicodeString): TBytes; overload;
{$ELSE}
function MecsWideBytesOf(const Val: WideString): TBytes; overload;
{$ENDIF}
begin
  result := MecsBytesOf(Val);
end;

// MecsWideStringOf
{$IFDEF UNICODE}
function MecsWideStringOf(const Bytes: TBytes): UnicodeString;
{$ELSE}
function MecsWideStringOf(const Bytes: TBytes): WideString;
{$ENDIF}
begin
  MecsStringOf(Bytes, result);
end;

// for AnsiString
// -----------------------------------------------------------------------------

// MecsGetCodePage
function MecsGetCodePage(const S: ByteString): DWORD;
begin
  {$IFDEF UNICODE}
  result := StringCodePage(S);
  if result = CP_ACP then
  {$ENDIF}
    result := DefaultAnsiCodePage;
end;

// MecsGetLeadBytes
function MecsGetLeadBytes(Codepage: DWORD): TLeadBytes;
var
  i:Integer;
  MBCSCPInfo: TCPInfo;
  LB: Byte;
  dCodePage: DWORD;
begin
  result := [];
  if CodePage = CP_UTF8 then
    Exit;
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  GetCPInfo(dCodePage, MBCSCPInfo);
  with MBCSCPInfo do
    begin
      i := 0;
      while (i < MAX_LEADBYTES) and ((LeadByte[i] or LeadByte[i + 1]) <> 0) do
        begin
          for LB := LeadByte[i] to LeadByte[i + 1] do
            Include(result, AnsiChar(LB));
          Inc(i, 2);
        end;
    end;
end;

// MecsShrinkElement
function MecsShrinkElement(const S: WideString): ByteString;
var
  i: Integer;
begin
  SetLength(result, Length(S));
  for i:=1 to Length(S) do
    result[i] := AnsiChar(S[i]);
end;

// MecsStretchElement
function MecsStretchElement(const S: ByteString): WideString;
var
  i: Integer;
begin
  SetLength(result, Length(S));
  for i:=1 to Length(S) do
    result[i] := WideChar(S[i]);
end;

// for UnicodeString/WideString
// -----------------------------------------------------------------------------

// MecsEastAsianWidth
function MecsEastAsianWidth(U: UCS4Char): TEastAsianWidth;
// http://unicode.org/Public/UNIDATA/MecsEastAsianWidth.txt
begin
  case U of
    // Fullwidth
    $3000,
    $FF01..$FF60,
    $FFE0..$FFE6:
      result := eawFullwidth;
    // Halfwidth
    $20A9,
    $FF61..$FFBE,
    $FFC2..$FFC7,
    $FFCA..$FFCF,
    $FFD2..$FFD7,
    $FFDA..$FFDC,
    $FFE8..$FFEE:
      result := eawHalfwidth;
    // Wide
    $1100..$1159,
    $115F,
    $2329..$232A,
    $2E80..$2E99,
    $2E9B..$2EF3,
    $2F00..$2FD5,
    $2FF0..$2FFB,
    $3001..$303E,
    $3041..$3096,
    $3099..$30FF,
    $3105..$312D,
    $3131..$318E,
    $3190..$31B7,
    $31C0..$31E3,
    $31F0..$321E,
    $3220..$3243,
    $3250..$32FE,
    $3300..$33FF,
    $3400..$4DB5,
    $4E00..$9FC3,
    $A000..$A48C,
    $A490..$A4C6,
    $AC00..$D7A3,
    $F900..$FA2D,
    $FA30..$FA6A,
    $FA70..$FAD9,
    $FE10..$FE19,
    $FE30..$FE52,
    $FE54..$FE66,
    $FE68..$FE6B,
    $20000..$2A6D6,
    $2A6D7..$2F7FF,
    $2F800..$2FA1D,
    $2FA1E..$2FFFD,
    $30000..$3FFFD:
      result := eawWide;
    // Narrow
    $0020..$007E,
    $00A2..$00A3,
    $00A5..$00A6,
    $00AC,
    $00AF,
    $27E6..$27ED,
    $2985..$2986:
      result := eawNarrow;
    // Ambiguous
    $00A1,
    $00A4,
    $00A7..$00A8,
    $00AA,
    $00AD..$00AE,
    $00B0..$00B4,
    $00B6..$00BA,
    $00BC..$00BF,
    $00C6,
    $00D0,
    $00D7..$00D8,
    $00DE..$00E1,
    $00E6,
    $00E8..$00EA,
    $00EC..$00ED,
    $00F0,
    $00F2..$00F3,
    $00F7..$00FA,
    $00FC,
    $00FE,
    $0101,
    $0111,
    $0113,
    $011B,
    $0126..$0127,
    $012B,
    $0131..$0133,
    $0138,
    $013F..$0142,
    $0144,
    $0148..$014B,
    $014D,
    $0152..$0153,
    $0166..$0167,
    $016B,
    $01CE,
    $01D0,
    $01D2,
    $01D4,
    $01D6,
    $01D8,
    $01DA,
    $01DC,
    $0251,
    $0261,
    $02C4,
    $02C7,
    $02C9..$02CB,
    $02CD,
    $02D0,
    $02D8..$02DB,
    $02DD,
    $02DF,
    $0300..$036F,
    $0391..$03A1,
    $03A3..$03A9,
    $03B1..$03C1,
    $03C3..$03C9,
    $0401,
    $0410..$044F,
    $0451,
    $2010,
    $2013..$2016,
    $2018..$2019,
    $201C..$201D,
    $2020..$2022,
    $2024..$2027,
    $2030,
    $2032..$2033,
    $2035,
    $203B,
    $203E,
    $2074,
    $207F,
    $2081..$2084,
    $20AC,
    $2103,
    $2105,
    $2109,
    $2113,
    $2116,
    $2121..$2122,
    $2126,
    $212B,
    $2153..$2154,
    $215B..$215E,
    $2160..$216B,
    $2170..$2179,
    $2190..$2199,
    $21B8..$21B9,
    $21D2,
    $21D4,
    $21E7,
    $2200,
    $2202..$2203,
    $2207..$2208,
    $220B,
    $220F,
    $2211,
    $2215,
    $221A,
    $221D..$2220,
    $2223,
    $2225,
    $2227..$222C,
    $222E,
    $2234..$2237,
    $223C..$223D,
    $2248,
    $224C,
    $2252,
    $2260..$2261,
    $2264..$2267,
    $226A..$226B,
    $226E..$226F,
    $2282..$2283,
    $2286..$2287,
    $2295,
    $2299,
    $22A5,
    $22BF,
    $2312,
    $2460..$24E9,
    $24EB..$254B,
    $2550..$2573,
    $2580..$258F,
    $2592..$2595,
    $25A0..$25A1,
    $25A3..$25A9,
    $25B2..$25B3,
    $25B6..$25B7,
    $25BC..$25BD,
    $25C0..$25C1,
    $25C6..$25C8,
    $25CB,
    $25CE..$25D1,
    $25E2..$25E5,
    $25EF,
    $2605..$2606,
    $2609,
    $260E..$260F,
    $2614..$2615,
    $261C,
    $261E,
    $2640,
    $2642,
    $2660..$2661,
    $2663..$2665,
    $2667..$266A,
    $266C..$266D,
    $266F,
    $273D,
    $2776..$277F,
    $E000..$F8FF,
    $FE00..$FE0F,
    $FFFD,
    $E0100..$E01EF:
      result := eawAmbiguous;
  else
    result := eawNeutral;
  end;
end;

// MecsCombiningType (written by Mae)
function MecsCombiningType(U: UCS4Char): TCombiningType;
begin
  case U of
    $0300..$036F,
    $0483..$0489,
    $0591..$05BD,
    $05BF..$05BF,
    $05C1..$05C2,
    $05C4..$05C5,
    $05C7..$05C7,
    $0610..$061A,
    $064B..$065E,
    $0670..$0670,
    $06D6..$06DC,
    $06DE..$06E4,
    $06E7..$06E8,
    $06EA..$06ED,
    $0711..$0711,
    $0730..$074A,
    $07A6..$07B0,
    $07EB..$07F3,
    $0901..$0903,
    $093C..$093C,
    $093E..$094D,
    $0951..$0954,
    $0962..$0963,
    $0981..$0983,
    $09BC..$09BC,
    $09BE..$09C4,
    $09C7..$09C8,
    $09CB..$09CD,
    $09D7..$09D7,
    $09E2..$09E3,
    $0A01..$0A03,
    $0A3C..$0A3C,
    $0A3E..$0A42,
    $0A47..$0A48,
    $0A4B..$0A4D,
    $0A51..$0A51,
    $0A70..$0A71,
    $0A75..$0A75,
    $0A81..$0A83,
    $0ABC..$0ABC,
    $0ABE..$0AC5,
    $0AC7..$0AC9,
    $0ACB..$0ACD,
    $0AE2..$0AE3,
    $0B01..$0B03,
    $0B3C..$0B3C,
    $0B3E..$0B44,
    $0B47..$0B48,
    $0B4B..$0B4D,
    $0B56..$0B57,
    $0B62..$0B63,
    $0B82..$0B82,
    $0BBE..$0BC2,
    $0BC6..$0BC8,
    $0BCA..$0BCD,
    $0BD7..$0BD7,
    $0C01..$0C03,
    $0C3E..$0C44,
    $0C46..$0C48,
    $0C4A..$0C4D,
    $0C55..$0C56,
    $0C62..$0C63,
    $0C82..$0C83,
    $0CBC..$0CBC,
    $0CBE..$0CBE,
    $0CC0..$0CC4,
    $0CC7..$0CC8,
    $0CCA..$0CCD,
    $0CD5..$0CD6,
    $0CE2..$0CE3,
    $0D02..$0D03,
    $0D3E..$0D44,
    $0D46..$0D48,
    $0D4A..$0D4D,
    $0D57..$0D57,
    $0D62..$0D63,
    $0D82..$0D83,
    $0DCA..$0DCA,
    $0DCF..$0DD4,
    $0DD6..$0DD6,
    $0DD8..$0DDF,
    $0DF2..$0DF3,
    $0E31..$0E31,
    $0E34..$0E3A,
    $0E47..$0E4E,
    $0EB1..$0EB1,
    $0EB4..$0EB9,
    $0EBB..$0EBC,
    $0EC8..$0ECD,
    $0F18..$0F19,
    $0F35..$0F35,
    $0F37..$0F37,
    $0F39..$0F39,
    $0F3E..$0F3F,
    $0F71..$0F84,
    $0F86..$0F87,
    $0F90..$0F97,
    $0F99..$0FBC,
    $0FC6..$0FC6,
    $102B..$103E,
    $1056..$1059,
    $105E..$1060,
    $1062..$1064,
    $1067..$106D,
    $1071..$1074,
    $1082..$108D,
    $108F..$108F,
    $135F..$135F,
    $1712..$1714,
    $1732..$1734,
    $1752..$1753,
    $1772..$1773,
    $17B6..$17D3,
    $17DD..$17DD,
    $180B..$180D,
    $18A9..$18A9,
    $1920..$192B,
    $1930..$193B,
    $19B0..$19C0,
    $19C8..$19C9,
    $1A17..$1A1B,
    $1B00..$1B04,
    $1B34..$1B44,
    $1B6B..$1B73,
    $1B80..$1B82,
    $1BA1..$1BAA,
    $1C24..$1C37,
    $1DC0..$1DE6,
    $1DFE..$1DFF,
    $20D0..$20F0,
    $2DE0..$2DFF,
    $302A..$302F,
    $3099..$309A,
    $A66F..$A672,
    $A67C..$A67D,
    $A802..$A802,
    $A806..$A806,
    $A80B..$A80B,
    $A823..$A827,
    $A880..$A881,
    $A8B4..$A8C4,
    $A926..$A92D,
    $A947..$A953,
    $AA29..$AA36,
    $AA43..$AA43,
    $AA4C..$AA4D,
    $FB1E..$FB1E,
    $FE00..$FE0F,
    $FE20..$FE26,
    $101FD..$101FD,
    $10A01..$10A03,
    $10A05..$10A06,
    $10A0C..$10A0F,
    $10A38..$10A3A,
    $10A3F..$10A3F,
    $1D165..$1D169,
    $1D16D..$1D172,
    $1D17B..$1D182,
    $1D185..$1D18B,
    $1D1AA..$1D1AD,
    $1D242..$1D244,
    $E0100..$E01EF:
      result := ctCombining;
  else
    result := ctBase;
  end;
end;

// MecsIsNormalized
function MecsIsNormalized(const AText: WideString; NormForm: TNORM_FORM): Boolean;
// -----------------------------------------------------------------------------
// Requires(XP/2003 or later):
// Microsoft Internationalized Domain Names (IDN) Mitigation APIs 1.1
// http://www.microsoft.com/downloads/details.aspx?FamilyID=ad6158d7-ddba-416a-9109-07607425a815&displaylang=en
// (or Internet Explorer 7 or later)
// -----------------------------------------------------------------------------
var
  FP: TFarProc;
  DLLWnd: THandle;
  // http://msdn2.microsoft.com/en-us/library/ms776382(VS.85).aspx
  IsNormalizedString: function(NormForm: Integer; lpString: LPCWSTR; cwLength: Integer):Boolean; stdcall;
begin
  result := False;
  if Length(AText) = 0 then
    begin
      result := True;
      Exit;
    end;
  {$IFDEF UNICODE}
  DLLWnd := LoadLibraryW('normaliz.dll');
  {$ELSE}
  DLLWnd := LoadLibraryA('normaliz.dll');
  {$ENDIF}
  try
    FP := GetProcAddress(DLLWnd, 'NormalizedString');
    if FP <> nil then
      begin
        @IsNormalizedString := FP;
        result := IsNormalizedString(Integer(NormForm), PWideChar(AText), Length(AText));
      end;
  finally
    if DLLWnd > 0 then
      FreeLibrary(DLLWnd);
  end;
end;

// MecsMappingFix_CP932ToUnicode
{$IFDEF UNICODE}
function MecsMappingFix_CP932ToUnicode(const S: UnicodeString): UnicodeString; overload;
begin
  result := MecsMappingFix_CP932ToUnicode(WideString(S));
end;
{$ENDIF}
function MecsMappingFix_CP932ToUnicode(const S: WideString): WideString; overload;
var
  i: Integer;
  P: PWideChar;
begin
  result := S;
  P := PWideChar(result);
  for i:=1 to Length(result) do
    begin
      case P^ of
        #$2015: // U+2015: HORIZONTAL BAR   -> U+2014: EM DASH
          P^ := #$2014;
        #$FFE3: // U+FFE3: FULLWIDTH MACRON -> U+203E: OVERLINE
          P^ := #$203E;
      end;
      Inc(P);
    end;
end;

// MecsMappingFix_JISX0208ToJISX0213
{$IFDEF UNICODE}
function MecsMappingFix_JISX0208ToJISX0213(const S: UnicodeString): UnicodeString; overload;
begin
  result := MecsMappingFix_JISX0208ToJISX0213(WideString(S));
end;
{$ENDIF}
function MecsMappingFix_JISX0208ToJISX0213(const S: WideString): WideString; overload;
const
  NullTerm = 1;
var
  i: Integer;
  UCS4: UCS4String;
  P: PUCS4Char;
begin
  UCS4 := UTF16ToUTF32(S);
  P := PUCS4Char(UCS4);
  for i:=0 to Length(UCS4) - NullTerm - 1 do
    begin
      case P^ of
        $5265: // U+5265: peel                        -> U+525D: peel, peel off, to shell, strip
          P^ := $525D;
        $53F1: // U+53F1: scold, shout at, bawl out   -> U+20B9F: U+20B9F
          P^ := $20B9F;
        $586B: // U+586B: fill in, fill up; make good -> U+5861: fill in, fill up; make good
          P^ := $5861;
        $982C: // U+982C: cheeks, jaw                 -> U+9830: cheeks, jaw
          P^ := $9830;
      end;
      Inc(P);
    end;
  result := UTF32ToUTF16(UCS4);
end;

// MecsMappingFix_JISX0208ToUnicode
{$IFDEF UNICODE}
function MecsMappingFix_JISX0208ToUnicode(const S: UnicodeString): UnicodeString; overload;
begin
  result := MecsMappingFix_JISX0208ToUnicode(WideString(S));
end;
{$ENDIF}
function MecsMappingFix_JISX0208ToUnicode(const S: WideString): WideString; overload;
var
  i: Integer;
  P: PWideChar;
begin
  result := S;
  P := PWideChar(result);
  for i:=1 to Length(result) do
    begin
      case P^ of
        // Vender dependence
        #$2014: // U+2014: EM DASH              -> U+2015: HORIZONTAL BAR
          P^ := #$2015;
        #$203E: // U+203E: OVERLINE             -> U+FFE3: FULLWIDTH MACRON
          P^ := #$FFE3;
        // Mapping Fix
        #$2016: // U+2016: DOUBLE VERTICAL LINE -> U+2225: PARALLEL TO
          P^ := #$2225;
        #$2212: // U+2212: MINUS SIGN           -> U+FF0D: FULLWIDTH HYPHEN-MINUS
          P^ := #$FF0D;
        #$301C: // U+301C: WAVE DASH            -> U+FF5E: FULLWIDTH TILDE
          P^ := #$FF5E;
        #$00A2: // U+00A2: CENT SIGN            -> U+FFE0: FULLWIDTH CENT SIGN
          P^ := #$FFE0;
        #$00A3: // U+00A3: POUND SIGN           -> U+FFE1: FULLWIDTH POUND SIGN
          P^ := #$FFE1;
        #$00AC: // U+00AC: NOT SIGN             -> U+FFE2: FULLWIDTH NOT SIGN
          P^ := #$FFE2;
      end;
      Inc(P);
    end;
end;

// MecsMappingFix_JISX0213ToJISX0208
{$IFDEF UNICODE}
function MecsMappingFix_JISX0213ToJISX0208(const S: UnicodeString): UnicodeString; overload;
begin
  result := MecsMappingFix_JISX0213ToJISX0208(WideString(S));
end;
{$ENDIF}
function MecsMappingFix_JISX0213ToJISX0208(const S: WideString): WideString; overload;
const
  NullTerm = 1;
var
  i: Integer;
  UCS4: UCS4String;
  P: PUCS4Char;
begin
  UCS4 := UTF16ToUTF32(S);
  P := PUCS4Char(UCS4);
  for i:=0 to Length(UCS4) - NullTerm - 1 do
    begin
      case P^ of
        $525D: // U+525D: peel, peel off, to shell, strip  -> U+5265: peel
          P^ := $5265;
        $5861: // U+5861: fill in, fill up; make good      -> U+586B: fill in, fill up; make good
          P^ := $586B;
        $9830: // U+9830: cheeks, jaw                      -> U+982C: cheeks, jaw
          P^ := $982C;
        $20B9F:// U+20B9F: U+20B9F                         -> U+53F1: scold, shout at, bawl out
          P^ := $53F1;
      end;
      Inc(P);
    end;
  result := UTF32ToUTF16(UCS4);
end;

// MecsMappingFix_UnicodeToCP932
{$IFDEF UNICODE}
function MecsMappingFix_UnicodeToCP932(const S: UnicodeString): UnicodeString; overload;
begin
  result := MecsMappingFix_UnicodeToCP932(WideString(S));
end;
{$ENDIF}
function MecsMappingFix_UnicodeToCP932(const S: WideString): WideString; overload;
var
  i: Integer;
  P: PWideChar;
begin
  result := S;
  P := PWideChar(result);
  for i:=1 to Length(result) do
    begin
      case P^ of
        // Vender dependence
        #$2014: // U+2014: EM DASH                -> U+2015: HORIZONTAL BAR
          P^ := #$2015;
        #$203E: // U+203E: OVERLINE               -> U+FFE3: FULLWIDTH MACRON
          P^ := #$FFE3;
        // Mapping Fix
        #$2011, // U+2011: NON-BREAKING HYPHEN    -> U+002D: HYPHEN-MINUS
        #$2012, // U+2012: FIGURE DASH            -> U+002D: HYPHEN-MINUS
        #$2013: // U+2013: EN DASH                -> U+002D: HYPHEN-MINUS
          P^ := #$002D;
        #$2212, // U+2212: MINUS SIGN             -> U+FF0D: FULLWIDTH HYPHEN-MINUS
        #$FE58, // U+FE58: SMALL EM DASH          -> U+FF0D: FULLWIDTH HYPHEN-MINUS
        #$FE63: // U+FE63: SMALL HYPHEN-MINUS     -> U+FF0D: FULLWIDTH HYPHEN-MINUS
          P^ := #$FF0D;
      end;
      Inc(P);
    end;
  result := MecsMappingFix_JISX0213ToJISX0208(result);
end;

// MecsMappingFix_UnicodeToJISX0208
{$IFDEF UNICODE}
function MecsMappingFix_UnicodeToJISX0208(const S: UnicodeString): UnicodeString; overload;
begin
  result := MecsMappingFix_UnicodeToJISX0208(WideString(S));
end;
{$ENDIF}
function MecsMappingFix_UnicodeToJISX0208(const S: WideString): WideString; overload;
var
  i: Integer;
  P: PWideChar;
begin
  result := S;
  P := PWideChar(result);
  for i:=1 to Length(result) do
    begin
      case P^ of
        // Vender dependence
        #$2014: // U+2014: EM DASH                -> U+2015: HORIZONTAL BAR
          P^ := #$2015;
        #$203E: // U+203E: OVERLINE               -> U+FFE3: FULLWIDTH MACRON
          P^ := #$FFE3;
        // Mapping Fix
        #$2011, // U+2011: NON-BREAKING HYPHEN    -> U+002D: HYPHEN-MINUS
        #$2012, // U+2012: FIGURE DASH            -> U+002D: HYPHEN-MINUS
        #$2013: // U+2013: EN DASH                -> U+002D: HYPHEN-MINUS
          P^ := #$002D;
        #$2225: // U+2225: PARALLEL TO            -> U+2016: DOUBLE VERTICAL LINE
          P^ := #$2016;
        #$FE58, // U+FE58: SMALL EM DASH          -> U+2212: MINUS SIGN
        #$FE63, // U+FE63: SMALL HYPHEN-MINUS     -> U+2212: MINUS SIGN
        #$FF0D: // U+FF0D: FULLWIDTH HYPHEN-MINUS -> U+2212: MINUS SIGN
          P^ := #$2212;
        #$FF5E: // U+FF5E: FULLWIDTH TILDE        -> U+301C: WAVE DASH
          P^ := #$301C;
        #$FFE0: // U+FFE0: FULLWIDTH CENT SIGN    -> U+00A2: CENT SIGN
          P^ := #$00A2;
        #$FFE1: // U+FFE1: FULLWIDTH POUND SIGN   -> U+00A3: POUND SIGN
          P^ := #$00A3;
        #$FFE2: // U+FFE2: FULLWIDTH NOT SIGN     -> U+00AC: NOT SIGN
          P^ := #$00AC;
      end;
      Inc(P);
    end;
  result := MecsMappingFix_JISX0213ToJISX0208(result);
end;

// MecsNormalize
function MecsNormalize(const Src: WideString; var Dst: WideString; NormForm: TNORM_FORM): Boolean;
// -----------------------------------------------------------------------------
// NormalizeString Requires(XP/2003 or later):
// Microsoft Internationalized Domain Names (IDN) Mitigation APIs 1.1
// http://www.microsoft.com/downloads/details.aspx?FamilyID=ad6158d7-ddba-416a-9109-07607425a815&displaylang=en
// (or Internet Explorer 7 or later)
// -----------------------------------------------------------------------------
// FoldString requires Windows NT3.1 or later;
// -----------------------------------------------------------------------------
//
// Please examine U+03D3.
//  - NFC  U+03D3
//  - NFD  U+03D2 (U+0020) U+0301
//  - NFKC U+038E
//  - NFKC U+03A5 (U+0020) U+0301
//
// -----------------------------------------------------------------------------
var
  i: Integer;
  O,P :PWideChar;
  BufSize: Integer;
  FP: TFarProc;
  DLLWnd: THandle;
  MapFlags: DWORD;
  // http://msdn2.microsoft.com/en-us/library/ms776395(VS.85).aspx
  NormalizeString: function(NormForm: Integer; lpSrcString: LPCWSTR; cwSrMecsLength: Integer; lpDstString: LPWSTR; cwDstLength: Integer):Integer; stdcall;
begin
  result := False;
  Dst := '';
  if Length(Src) = 0 then
    begin
      result := True;
      Exit;
    end;
  {$IFDEF UNICODE}
  DLLWnd := LoadLibraryW('normaliz.dll');
  {$ELSE}
  DLLWnd := LoadLibraryA('normaliz.dll');
  {$ENDIF}
  try
    FP := GetProcAddress(DLLWnd, 'NormalizeString');
    if FP <> nil then
      begin
        @NormalizeString := FP;
        BufSize := NormalizeString(Integer(NormForm), PWideChar(Src), Length(Src), nil, 0);
        if (GetLastError <> 0) then
          Exit;
        if (BufSize = 0) then
          begin
            result := True;
            Exit;
          end;
        SetLength(Dst, BufSize);
        P := PWideChar(Dst);
        for i:=1 to BufSize do
          begin
            P^ := #$0000;
            Inc(P);
          end;
        NormalizeString(Integer(NormForm), PWideChar(Src), Length(Src), PWideChar(Dst), Length(Dst));
        if (GetLastError <> 0) then
          Exit;
        P := PWideChar(Dst);
        O := P;
        while (P^ <> #$0000) do
          Inc(P);
        SetLength(Dst, P - O);
        result := True;
      end;
  finally
    if DLLWnd > 0 then
      FreeLibrary(DLLWnd);
  end;
  if not result then
    begin
      case NormForm of
        NormalizationC:
          MapFlags := MAP_PRECOMPOSED;
        NormalizationD:
          MapFlags := MAP_COMPOSITE;
        NormalizationKC:
          MapFlags := MAP_FOLDCZONE or MAP_PRECOMPOSED;
        NormalizationKD:
          MapFlags := MAP_FOLDCZONE or MAP_COMPOSITE;
      else
        MapFlags := 0;
      end;
      BufSize := FoldStringW(MapFlags, PWideChar(Src), Length(Src), nil, 0);
      if (GetLastError <> 0) then
        Exit;
      if (BufSize = 0) then
        begin
          result := True;
          Exit;
        end;
      SetLength(Dst, BufSize);
      P := PWideChar(Dst);
      for i:=1 to BufSize do
        begin
          P^ := #$0000;
          Inc(P);
        end;
      FoldStringW(MapFlags, PWideChar(Src), Length(Src), PWideChar(Dst), Length(Dst));
      if (GetLastError <> 0) then
        Exit;
      P := PWideChar(Dst);
      O := P;
      while (P^ <> #$0000) do
        Inc(P);
      SetLength(Dst, P - O);
      result := True;
    end;
end;

// StrLen
{$IFNDEF UNICODE}
function StrLen(const Str: PWideChar): Cardinal;
begin
  result := Length(Str);
end;
{$ENDIF}

// StrPos
{$IFNDEF UNICODE}
function StrPos(const Str1, Str2: PWideChar): PWideChar;
var
  MatchStart, LStr1, LStr2: PWideChar;
begin
  result := nil;
  if (Str1^ = #0) or (Str2^ = #0) then
    Exit;
  if (MECSUtils.StrLen(Str1) < MECSUtils.StrLen(Str2)) then
    Exit;
  MatchStart := Str1;
  while MatchStart^ <> #0 do
    begin
      if MatchStart^ = Str2^ then
        begin
          LStr1 := MatchStart + 1;
          LStr2 := Str2 + 1;
          while True do
            begin
              if LStr2^ = #0 then
                begin
                  result := MatchStart;
                  Exit;
                end;
              if (LStr1^ <> LStr2^) or (LStr1^ = #0) then
                Break;
              Inc(LStr1);
              Inc(LStr2);
            end;
        end;
    Inc(MatchStart);
  end;
end;
{$ENDIF}

// for AnsiString/UnicodeString/WideString (Common)
// -----------------------------------------------------------------------------

// MecsAnsiPos
function MecsAnsiPos(const SubStr, S: ByteString; Codepage: DWORD): Integer;
begin
  result := MecsAnsiPosEx(SubStr, S, 1, Codepage);
end;

{$IFDEF UNICODE}
function MecsAnsiPos(const SubStr, S: UnicodeString): Integer;
begin
  result := MecsAnsiPosEx(SubStr, S, 1);
end;
{$ENDIF}

function MecsAnsiPos(const SubStr, S: WideString): Integer;
begin
  result := MecsAnsiPosEx(SubStr, S, 1);
end;

// MeceAnsiPosEx
function MecsAnsiPosEx(const SubStr, S: ByteString; Offset: Integer; Codepage: DWORD): Integer;
var
  P: PAnsiChar;
  Src: PAnsiChar;
  dCodepage: DWORD;
begin
  result := 0;
  if (Offset > Length(S)) or (Offset < 1) then
    Exit;
  if Codepage <> 0 then
    dCodepage := Codepage
  else
    dCodepage := MecsGetCodePage(SubStr);
  Src := PAnsiChar(S);
  Inc(Src, Offset - 1);
  P := MecsAnsiStrPos(Src, PAnsiChar(SubStr), dCodepage);
  if P <> nil then
    result := (Integer(P) - Integer(PAnsiChar(S))) div SizeOf(AnsiChar) + 1;
end;

{$IFDEF UNICODE}
function MecsAnsiPosEx(const SubStr, S: UnicodeString; Offset: Integer): Integer;
begin
  result := MecsAnsiPosEx(WideString(SubStr), WideString(S), Offset);
end;
{$ENDIF}

function MecsAnsiPosEx(const SubStr, S: WideString; Offset: Integer): Integer;
var
  P: PWideChar;
  Src: PWideChar;
begin
  result := 0;
  if (Offset > Length(S)) or (Offset < 1) then
    Exit;
  Src := PWideChar(S);
  Inc(Src, Offset - 1);
  P := MecsAnsiStrPos(Src, PWideChar(SubStr));
  if P <> nil then
    result := (Integer(P) - Integer(PWideChar(S))) div SizeOf(WideChar) + 1;
end;

// MecsAnsiStrPos
function MecsAnsiStrPos(Str, SubStr: PAnsiChar; Codepage: DWORD): PAnsiChar;
var
  L1, L2: Cardinal;
  ElementType : TElementType;
begin
  if CodePage = CP_UTF8 then
    result := {$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrPos(Str, SubStr)
  else
    begin
      result := nil;
      if (Str = nil) or (Str^ = #0) or (SubStr = nil) or (SubStr^ = #0) then
        Exit;
      L1 := {$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrLen(Str);
      L2 := {$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrLen(SubStr);
      result := {$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrPos(Str, SubStr);
      while (result <> nil) and ((L1 - Cardinal(result - Str)) >= L2) do
        begin
          ElementType := MecsStrElementType(Str, Integer(result-Str), Codepage);
          if (ElementType <> etTrail) and (StrLComp(Result, SubStr, L2) = 0) then
            Exit;
          if (ElementType = etLead) then
            Inc(result);
          Inc(result);
          result := {$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrPos(result, SubStr);
        end;
      result := nil;
    end;
end;

function MecsAnsiStrPos(Str, SubStr: PWideChar): PWideChar;
begin
{$IFDEF UNICODE}
  result := {$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrPos(Str, SubStr);
{$ELSE}
  result := MECSUtils.StrPos(Str, SubStr);
{$ENDIF}
end;

// MecsCharToElementIndex
function MecsCharToElementIndex(const S: ByteString; Index: Integer; Codepage: DWORD): Integer;
var
  P, O: PAnsiChar;
  L, ChrCnt: Integer;
  LeadBytes: TLeadBytes;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  result := 0;
  if dCodePage = CP_UTF8 then
    begin
      ChrCnt := 0;
      P := PAnsiChar(S);
      O := P;
      while (P^ <> #$00) and (result = 0) do
        begin
          if (Byte(P^) and $80) = $00 then
            Inc(ChrCnt)
          else if (Byte(P^) and $C0) = $C0 then
            Inc(ChrCnt);
          if ChrCnt = Index then
            begin
              result := P - O + 1;
              Exit;
            end;
          Inc(P);
        end;
    end
  else
    begin
      LeadBytes := MecsGetLeadBytes(dCodePage);
      if LeadBytes = [] then
        result := Index
      else
        begin
          ChrCnt := 0;
          P := PAnsiChar(S);
          O := P;
          while (P^ <> #$00) and (result = 0) do
            begin
              Inc(ChrCnt);
              if ChrCnt = Index then
                begin
                  result := P - O + 1;
                  Exit;
                end;
              if P^ in LeadBytes then
                begin
                  Inc(P);
                  if P^ = #$00 then
                    Break;
                end;
              Inc(P);
            end;
        end;
    end;
end;

{$IFDEF UNICODE}
function MecsCharToElementIndex(const S: UnicodeString; Index: Integer): Integer;
var
  P, O: PUnicodeChar;
  L, ChrCnt: Integer;
begin
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  ChrCnt := 0;
  P := PUnicodeChar(S);
  O := P;
  while (P^ <> #$0000) and (result = 0) do
    begin
      Inc(ChrCnt);
      if ChrCnt = Index then
        begin
          result := P - O + 1;
          Break;
        end;
      if MecsIsLeadElement(P^) then
        begin
          Inc(P);
          if P^ = #$0000 then
            Break;
        end;
      Inc(P);
    end;
end;
{$ENDIF}

function MecsCharToElementIndex(const S: WideString; Index: Integer): Integer;
var
  P, O: PWideChar;
  L, ChrCnt: Integer;
begin
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  ChrCnt := 0;
  P := PWideChar(S);
  O := P;
  while (P^ <> #$0000) and (result = 0) do
    begin
      Inc(ChrCnt);
      if ChrCnt = Index then
        begin
          result := P - O + 1;
          Break;
        end;
      if MecsIsLeadElement(P^) then
        begin
          Inc(P);
          if P^ = #$0000 then
            Break;
        end;
      Inc(P);
    end;
end;

// MecsCharToElementIndexCount
function MecsCharToElementIndexCount(const S: ByteString; Index, Count: Integer; Codepage: DWORD): TIndexCount;
var
  P, O: PAnsiChar;
  L, J, ChrCnt: Integer;
  LeadBytes: TLeadBytes;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  result.StartIndex := Index;
  result.EndIndex   := Index + Count - 1;
  result.Count      := Count;
  L := System.Length(S);
  if L = 0 then
    Exit;
  if dCodePage = CP_UTF8 then
    begin
      result.StartIndex := 0;
      result.EndIndex   := 0;
      result.Count      := 0;
      ChrCnt := 0;
      P := PAnsiChar(S);
      O := P;
      while (P^ <> #$00) and (result.EndIndex = 0) and ((ChrCnt - Index) < Count) do
        begin
          if (Byte(P^) and $80) = $00 then
            begin
              Inc(ChrCnt);
              if ChrCnt = Index then
                result.StartIndex := P - O + 1;
              if ChrCnt = (Index + Count - 1) then
                result.EndIndex := P - O + 1;
            end
          else if (Byte(P^) and $C0) = $C0 then
            begin
              Inc(ChrCnt);
              if ChrCnt = Index then
                result.StartIndex := P - O + 1;
              if ChrCnt = (Index + Count - 1) then
                begin
                  for J:=2 to 6 do
                    begin
                      if (Byte(P^) and ($80 shr J)) = 0 then
                        begin
                          result.EndIndex := P - O + J;
                          Break;
                        end;
                    end;
                end;
            end;
          Inc(P);
        end;
      if (result.EndIndex = 0) and ((ChrCnt - Index) < Count) then
        result.EndIndex := L;
      result.Count := result.EndIndex - result.StartIndex + 1;
    end
  else
    begin
      LeadBytes := MecsGetLeadBytes(dCodePage);
      if LeadBytes = [] then
        begin
          result.StartIndex := Index;
          result.EndIndex   := Index + Count - 1;
          result.Count      := Count;
        end
      else
        begin
          result.StartIndex := 0;
          result.EndIndex   := 0;
          result.Count      := 0;
          ChrCnt := 0;
          P := PAnsiChar(S);
          O := P;
          while (P^ <> #$00) and (result.EndIndex = 0) and ((ChrCnt - Index) < Count) do
            begin
              Inc(ChrCnt);
              if ChrCnt = Index then
                result.StartIndex := P - O + 1;
              if P^ in LeadBytes then
                begin
                  if ChrCnt = (Index + Count - 1) then
                    result.EndIndex := P - O + 2;
                  Inc(P);
                  if P^ = #$00 then
                    Break;
                end
              else if ChrCnt = (Index + Count - 1) then
                result.EndIndex := P - O + 1;
              Inc(P);
            end;
          if (result.EndIndex = 0) and ((ChrCnt - Index) < Count) then
            result.EndIndex := L;
          result.Count := result.EndIndex - result.StartIndex + 1;
        end;
    end;
end;

{$IFDEF UNICODE}
function MecsCharToElementIndexCount(const S: UnicodeString; Index, Count: Integer): TIndexCount;
var
  P, O: PUnicodeChar;
  L, ChrCnt: Integer;
begin
  result.StartIndex := Index;
  result.EndIndex   := Index + Count - 1;
  result.Count      := Count;
  L := System.Length(S);
  if L = 0 then
    Exit;
  result.StartIndex := 0;
  result.EndIndex   := 0;
  result.Count      := 0;
  ChrCnt := 0;
  P := PUnicodeChar(S);
  O := P;
  while (P^ <> #$0000) and (result.EndIndex = 0) and ((ChrCnt - Index) < Count) do
    begin
      Inc(ChrCnt);
      if ChrCnt = Index then
        result.StartIndex := P - O + 1;
      if MecsIsLeadElement(P^) then
        begin
          if ChrCnt = (Index + Count - 1) then
            result.EndIndex := P - O + 2;
          Inc(P);
          if P^ = #$0000 then
            Break;
        end
      else if ChrCnt = (Index + Count - 1) then
        result.EndIndex := P - O + 1;
      Inc(P);
    end;
  if (result.EndIndex = 0) and ((ChrCnt - Index) < Count) then
    result.EndIndex := L;
  result.Count := result.EndIndex - result.StartIndex + 1;
end;
{$ENDIF}

function MecsCharToElementIndexCount(const S: WideString; Index, Count: Integer): TIndexCount;
var
  P, O: PWideChar;
  L, ChrCnt: Integer;
begin
  result.StartIndex := Index;
  result.EndIndex   := Index + Count - 1;
  result.Count      := Count;
  L := System.Length(S);
  if L = 0 then
    Exit;
  result.StartIndex := 0;
  result.EndIndex   := 0;
  result.Count      := 0;
  ChrCnt := 0;
  P := PWideChar(S);
  O := P;
  while (P^ <> #$0000) and (result.EndIndex = 0) and ((ChrCnt - Index) < Count) do
    begin
      Inc(ChrCnt);
      if ChrCnt = Index then
        result.StartIndex := P - O + 1;
      if MecsIsLeadElement(P^) then
        begin
          if ChrCnt = (Index + Count - 1) then
            result.EndIndex := P - O + 2;
          Inc(P);
          if P^ = #$0000 then
            Break;
        end
      else if ChrCnt = (Index + Count - 1) then
        result.EndIndex := P - O + 1;
      Inc(P);
    end;
  if (result.EndIndex = 0) and ((ChrCnt - Index) < Count) then
    result.EndIndex := L;
  result.Count := result.EndIndex - result.StartIndex + 1;
end;


// MecsCharLength
function MecsCharLength(const S: ByteString; Index: Integer; Codepage: DWORD): Integer;
var
  Idx: Integer;
  J: Byte;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  result := 1;
  if S = '' then
    Exit;
  Idx := MecsCharToElementIndex(S, Index, dCodepage);
  if Idx = 0 then
    Exit;
  if dCodepage = CP_UTF8 then
    begin
      if (Byte(S[Idx]) and $C0) = $C0 then
        begin
          for J:=2 to 6 do
            begin
              if (Byte(S[Idx]) and ($80 shr J)) = 0 then
                begin
                  result := J;
                  Break;
                end;
            end;
        end;
    end
  else
    begin
      if MecsIsLeadElement(S[Idx], dCodepage) then
        result := 2;
    end;
end;

{$IFDEF UNICODE}
function MecsCharLength(const S: UnicodeString; Index: Integer): Integer;
begin
  result := 1;
  if S = '' then
    Exit;
  if Index <= 0 then
    Exit;
  if MecsIsLeadElement(S[Index]) then
    result := 2;
end;
{$ENDIF}

function MecsCharLength(const S: WideString; Index: Integer): Integer;
begin
  result := 1;
  if S = '' then
    Exit;
  if Index <= 0 then
    Exit;
  if MecsIsLeadElement(S[Index]) then
    result := 2;
end;

// MecsCharToElementLen
function MecsCharToElementLen(const S: ByteString; MaxLen: Integer; Codepage: DWORD): Integer;
var
  dCodePage: DWORD;
  function _MecsCharToElementLen(const S: ByteString; MaxLen: Integer; Codepage: DWORD): Integer;
  var
    P, O: PAnsiChar;
    L, ChrCnt: Integer;
    J: Byte;
    LeadBytes: TLeadBytes;
  begin
    result := 0;
    L := System.Length(S);
    if L = 0 then
      Exit;
    if CodePage = CP_UTF8 then
      begin
        ChrCnt := 0;
        P := PAnsiChar(S);
        O := P;
        while (P^ <> #$00) and (result = 0) do
          begin
            if (Byte(P^) and $80) = $00 then
              begin
                Inc(ChrCnt);
                if ChrCnt = MaxLen then
                  begin
                    result := P - O + 1;
                    Exit;
                  end;
              end
            else if (Byte(P^) and $C0) = $C0 then
              begin
                Inc(ChrCnt);
                if ChrCnt = MaxLen then
                  begin
                    for J:=2 to 6 do
                      begin
                        if (Byte(P^) and ($80 shr J)) = 0 then
                          begin
                            result := P - O + J;
                            Break;
                          end;
                      end;
                    Exit;
                  end;
              end;
            Inc(P);
          end;
      end
    else
      begin
        LeadBytes := MecsGetLeadBytes(CodePage);
        if LeadBytes = [] then
          result := MaxLen
        else
          begin
            ChrCnt := 0;
            P := PAnsiChar(S);
            O := P;
            while (P^ <> #$00) and (result = 0) do
              begin
                Inc(ChrCnt);
                if ChrCnt = MaxLen then
                  result := P - O + 1;
                if P^ in LeadBytes then
                  begin
                    Inc(P);
                    if ChrCnt = MaxLen then
                      begin
                        result := P - O + 1;
                        Break;
                      end;
                    if P^ = #$00 then
                      Break;
                  end;
                Inc(P);
              end;
          end;
      end;
  end;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  result := _MecsCharToElementLen(S, MaxLen, dCodePage);
  if result < MaxLen then
    result := System.Length(S);
end;

{$IFDEF UNICODE}
function MecsCharToElementLen(const S: UnicodeString; MaxLen: Integer): Integer;
begin
  result := MecsCharToElementIndex(S, MaxLen + 1) - 1;
  if result < MaxLen then
    result := System.Length(S);
end;
{$ENDIF}

function MecsCharToElementLen(const S: WideString; MaxLen: Integer): Integer;
begin
  result := MecsCharToElementIndex(S, MaxLen + 1) - 1;
  if result < MaxLen then
    result := System.Length(S);
end;

// MecsCopy
function MecsCopy(const S: ByteString; Index, Count: Integer; Codepage: DWORD): ByteString;
var
  AIC: TIndexCount;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  AIC := MecsCharToElementIndexCount(S, Index, Count, dCodePage);
  if AIC.StartIndex > 0 then
    result := Copy(S, AIC.StartIndex, AIC.Count)
  else
    result := '';
end;

{$IFDEF UNICODE}
function MecsCopy(const S: UnicodeString; Index, Count: Integer): UnicodeString;
var
  WIC: TIndexCount;
begin
  WIC := MecsCharToElementIndexCount(S, Index, Count);
  if WIC.StartIndex > 0 then
    result := Copy(S, WIC.StartIndex, WIC.Count)
  else
    result := '';
end;
{$ENDIF}

function MecsCopy(const S: WideString; Index, Count: Integer): WideString;
var
  WIC: TIndexCount;
begin
  WIC := MecsCharToElementIndexCount(S, Index, Count);
  if WIC.StartIndex > 0 then
    result := Copy(S, WIC.StartIndex, WIC.Count)
  else
    result := '';
end;

// MecsDelete
procedure MecsDelete(var S: ByteString; Index, Count: Integer; Codepage: DWORD);
var
  AIC: TIndexCount;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  AIC := MecsCharToElementIndexCount(S, Index, Count, dCodePage);
  if AIC.StartIndex > 0 then
    Delete(S, AIC.StartIndex, AIC.Count);
end;

{$IFDEF UNICODE}
procedure MecsDelete(var S: UnicodeString; Index, Count: Integer);
var
  UIC: TIndexCount;
begin
  UIC := MecsCharToElementIndexCount(S, Index, Count);
  if UIC.StartIndex > 0 then
    Delete(S, UIC.StartIndex, UIC.Count);
end;
{$ENDIF}

procedure MecsDelete(var S: WideString; Index, Count: Integer);
var
  WIC: TIndexCount;
begin
  WIC := MecsCharToElementIndexCount(S, Index, Count);
  if WIC.StartIndex > 0 then
    Delete(S, WIC.StartIndex, WIC.Count);
end;

// MecsElementToCharIndex
function MecsElementToCharIndex(const S: ByteString; Index: Integer; Codepage: DWORD): Integer;
var
  P, O: PAnsiChar;
  L, ChrCnt: Integer;
  LeadBytes: TLeadBytes;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  result := 0;
  if dCodePage = CP_UTF8 then
    begin
      ChrCnt := 0;
      P := PAnsiChar(S);
      O := P;
      while (P^ <> #$00) and (result = 0) do
        begin
          if (Byte(P^) and $80) = $00 then
            Inc(ChrCnt)
          else if (Byte(P^) and $C0) = $C0 then
            Inc(ChrCnt);
          if Index = (P - O + 1) then
            begin
              result := ChrCnt;
              Break;
            end;
          Inc(P);
        end;
    end
  else
    begin
      LeadBytes := MecsGetLeadBytes(dCodePage);
      if LeadBytes = [] then
        result := Index
      else
        begin
          ChrCnt := 0;
          P := PAnsiChar(S);
          O := P;
          while (P^ <> #$00) and (result = 0) do
            begin
              Inc(ChrCnt);
              if Index = (P - O + 1) then
                begin
                  result := ChrCnt;
                  Break;
                end;
              if P^ in LeadBytes then
                begin
                  Inc(P);
                  if P^ = #$00 then
                    Break;
                  if Index = (P - O + 1) then
                    begin
                      result := ChrCnt;
                      Break;
                    end;
                end;
              Inc(P);
            end;
        end;
    end;
end;

{$IFDEF UNICODE}
function MecsElementToCharIndex(const S: UnicodeString; Index: Integer): Integer;
var
  P, O: PUnicodeChar;
  L, ChrCnt: Integer;
begin
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  ChrCnt := 0;
  P := PUnicodeChar(S);
  O := P;
  while (P^ <> #$0000) and (result = 0) do
    begin
      Inc(ChrCnt);
      if Index = (P - O + 1) then
        begin
          result := ChrCnt;
          Break;
        end;
      if MecsIsLeadElement(P^) then
        begin
          Inc(P);
          if P^ = #$0000 then
            Break;
          if Index = (P - O + 1) then
            begin
              result := ChrCnt;
              Break;
            end;
        end;
      Inc(P);
    end;
end;
{$ENDIF}

function MecsElementToCharIndex(const S: WideString; Index: Integer): Integer;
var
  P, O: PWideChar;
  L, ChrCnt: Integer;
begin
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  ChrCnt := 0;
  P := PWideChar(S);
  O := P;
  while (P^ <> #$0000) and (result = 0) do
    begin
      Inc(ChrCnt);
      if Index = (P - O + 1) then
        begin
          result := ChrCnt;
          Break;
        end;
      if MecsIsLeadElement(P^) then
        begin
          Inc(P);
          if P^ = #$0000 then
            Break;
          if Index = (P - O + 1) then
            begin
              result := ChrCnt;
              Break;
            end;
        end;
      Inc(P);
    end;
end;

// MecsElementToCharLen
function MecsElementToCharLen(const S: ByteString; MaxLen: Integer; Codepage: DWORD): Integer;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  result := MecsElementToCharIndex(S, MaxLen, dCodePage);
  if result = 0 then
    result := MecsLength(S, dCodePage);
end;

{$IFDEF UNICODE}
function MecsElementToCharLen(const S: UnicodeString; MaxLen: Integer): Integer;
begin
  result := MecsElementToCharIndex(S, MaxLen);
  if result = 0 then
    result := MecsLength(S);
end;
{$ENDIF}

function MecsElementToCharLen(const S: WideString; MaxLen: Integer): Integer;
begin
  result := MecsElementToCharIndex(S, MaxLen);
  if result = 0 then
    result := MecsLength(S);
end;

// MecsElementType
function MecsElementType(const S: ByteString; Index: Integer; Codepage: DWORD): TElementType;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  result := MecsStrElementType(PAnsiChar(S), Index - 1, dCodePage);
end;

{$IFDEF UNICODE}
function MecsElementType(const S: UnicodeString; Index: Integer): TElementType;
begin
  result := MecsStrElementType(PUnicodeChar(S), Index - 1);
end;
{$ENDIF}

function MecsElementType(const S: WideString; Index: Integer): TElementType;
begin
  result := MecsStrElementType(PWideChar(S), Index - 1);
end;

// MecsInsert
procedure MecsInsert(const Source: ByteString; var S: ByteString; Index: Integer; Codepage: DWORD);
var
  Idx: Integer;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  Idx := MecsCharToElementIndex(S, Index, dCodePage);
  if Idx > 0 then
    Insert(Source, S, Idx)
  else
    S := S + Source;
end;

{$IFDEF UNICODE}
procedure MecsInsert(const Source: UnicodeString; var S: UnicodeString; Index: Integer);
var
  Idx: Integer;
begin
  Idx := MecsCharToElementIndex(S, Index);
  if Idx > 0 then
    Insert(Source, S, Idx)
  else
    S := S + Source;
end;
{$ENDIF}

procedure MecsInsert(const Source: WideString; var S: WideString; Index: Integer);
var
  Idx: Integer;
begin
  Idx := MecsCharToElementIndex(S, Index);
  if Idx > 0 then
    Insert(Source, S, Idx)
  else
    S := S + Source;
end;

// MecsIsFullWidth
function MecsIsFullWidth(const S: ByteString; CharIndex: Integer): Boolean;
begin
  result := MecsIsFullWidth(S, CharIndex, SysLocale.FarEast, MecsGetCodePage(S));
end;

function MecsIsFullWidth(const S: ByteString; CharIndex: Integer; FarEast: Boolean; Codepage: DWORD): Boolean;
var
  IC: TIndexCount;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  if dCodePage = CP_UTF8 then
    {$IFDEF UNICODE}
    result := MecsIsFullWidth(UTF8ToUnicodeString(S), CharIndex, FarEast)
    {$ELSE}
    result := MecsIsFullWidth(MECSUtils.UTF8ToUTF16(S), CharIndex, FarEast)
    {$ENDIF}
  else
    begin
      IC := MecsCharToElementIndexCount(S, CharIndex, 1, dCodepage);
      result := (IC.Count > 1);
    end;
end;

{$IFDEF UNICODE}
function MecsIsFullWidth(const S: UnicodeString; CharIndex: Integer): Boolean;
begin
  result := MecsIsFullWidth(S, CharIndex, SysLocale.FarEast);
end;
{$ENDIF}

{$IFDEF UNICODE}
function MecsIsFullWidth(const S: UnicodeString; CharIndex: Integer; FarEast: Boolean): Boolean;
const
  NullTerm = 1;
var
  UCS4: UCS4String;
begin
  result := False;
  UCS4 := UTF16ToUTF32(S);
  if Length(UCS4) - NullTerm <= 0 then
    Exit;
  if CharIndex < 1 then
    Exit;
  if CharIndex > Length(UCS4) - NullTerm then
    Exit;
  case MecsEastAsianWidth(UCS4[CharIndex - 1]) of
    eawWide,
    eawFullwidth:
      result := True;
    eawAmbiguous:
      result := FarEast;
  else
    result := False;
  end;
end;
{$ENDIF}

function MecsIsFullWidth(const S: WideString; CharIndex: Integer): Boolean;
begin
  result := MecsIsFullWidth(S, CharIndex, SysLocale.FarEast);
end;

function MecsIsFullWidth(const S: WideString; CharIndex: Integer; FarEast: Boolean): Boolean;
const
  NullTerm = 1;
var
  UCS4: UCS4String;
begin
  result := False;
  UCS4 := UTF16ToUTF32(S);
  if Length(UCS4) - NullTerm < 0 then
    Exit;
  if CharIndex < 1 then
    Exit;
  if CharIndex > Length(UCS4) - NullTerm then
    Exit;
  case MecsEastAsianWidth(UCS4[CharIndex - 1]) of
    eawWide,
    eawFullwidth:
      result := True;
    eawAmbiguous:
      result := FarEast;
  else
    result := False;
  end;
end;

function MecsIsFullWidth(const CodePoint: UCS4Char): Boolean; overload;
begin
  result := MecsIsFullWidth(CodePoint, SysLocale.FarEast);
end;

function MecsIsFullWidth(const CodePoint: UCS4Char; FarEast: Boolean): Boolean; overload;
begin
  case MecsEastAsianWidth(CodePoint) of
    eawWide,
    eawFullwidth:
      result := True;
    eawAmbiguous:
      result := FarEast;
  else
    result := False;
  end;
end;

// MecsIsLeadElement
function MecsIsLeadElement(TestChar: AnsiChar; Codepage: DWORD): Boolean;
var
  LeadBytes: TLeadBytes;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  result := False;
  if TestChar = '' then
    Exit;
  if dCodePage = CP_UTF8 then
    result := ((Byte(TestChar) and $C0) = $C0)
  else
    begin
      LeadBytes := MecsGetLeadBytes(dCodePage);
      if LeadBytes = [] then
        Exit;
      result := (TestChar in LeadBytes);
    end;
end;

{$IFDEF UNICODE}
function MecsIsLeadElement(TestChar: UnicodeChar): Boolean;
{$ELSE}
function MecsIsLeadElement(TestChar: WideChar): Boolean;
{$ENDIF}
begin
  result := ((WORD(TestChar) shr 8) in [$D8..$DB]);
end;

// MecsIsMECElement
function MecsIsMECElement(TestChar: AnsiChar; Codepage: DWORD): Boolean; overload;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  result := (MecsElementType(TestChar, 1, dCodePage) <> etSingle);
end;

{$IFDEF UNICODE}
function MecsIsMECElement(TestChar: UnicodeChar): Boolean;
{$ELSE}
function MecsIsMECElement(TestChar: WideChar): Boolean;
{$ENDIF}
begin
  result := ((WORD(TestChar) and $D800) = $D800);
end;

// MecsIsTrailElement
function MecsIsTrailElement(TestChar: AnsiChar; Codepage: DWORD): Boolean;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  result := (MecsElementType(TestChar, 1, dCodePage) = etTrail);
end;

{$IFDEF UNICODE}
function MecsIsTrailElement(TestChar: UnicodeChar): Boolean;
{$ELSE}
function MecsIsTrailElement(TestChar: WideChar): Boolean;
{$ENDIF}
begin
  result := ((WORD(TestChar) shr 8) in [$DC..$DF]);
end;

// MecsLeftStr
function MecsLeftStr(const AText: ByteString; const ACount: Integer; Codepage: DWORD): ByteString;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(AText)
  else
    dCodePage := Codepage;
  result := MecsCopy(AText, 1, ACount, dCodePage);
end;

{$IFDEF UNICODE}
function MecsLeftStr(const AText: UnicodeString; const ACount: Integer): UnicodeString;
begin
  result := MecsCopy(AText, 1, ACount);
end;
{$ENDIF}

function MecsLeftStr(const AText: WideString; const ACount: Integer): WideString;
begin
  result := MecsCopy(AText, 1, ACount);
end;

// MecsLength
function MecsLength(const S: ByteString; Codepage: DWORD): Integer;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  result := MecsStrLen(PAnsiChar(S), dCodePage);
end;

{$IFDEF UNICODE}
function MecsLength(const S: UnicodeString): Integer;
begin
  result := MecsStrLen(PUnicodeChar(S));
end;
{$ENDIF}

function MecsLength(const S: WideString): Integer;
begin
  result := MecsStrLen(PWideChar(S));
end;

// MecsMidStr
function MecsMidStr(const AText: ByteString; const AStart, ACount: Integer; Codepage: DWORD): ByteString;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(AText)
  else
    dCodePage := Codepage;
  result := MecsCopy(AText, AStart, ACount, dCodePage);
end;

{$IFDEF UNICODE}
function MecsMidStr(const AText: UnicodeString; const AStart, ACount: Integer): UnicodeString;
begin
  result := MecsCopy(AText, AStart, ACount);
end;
{$ENDIF}

function MecsMidStr(const AText: WideString; const AStart, ACount: Integer): WideString;
begin
  result := MecsCopy(AText, AStart, ACount);
end;

// MecsNextCharIndex
function MecsNextCharIndex(const S: ByteString; Index: Integer; Codepage: DWORD): Integer;
var
  Idx: Integer;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(S)
  else
    dCodePage := Codepage;
  Idx := MecsElementToCharIndex(S, Index, dCodePage);
  result := MecsCharToElementLen(S, Idx, dCodePage) + 1;
end;

{$IFDEF UNICODE}
function MecsNextCharIndex(const S: UnicodeString; Index: Integer): Integer;
var
  Idx: Integer;
begin
  Idx := MecsElementToCharIndex(S, Index);
  result := MecsCharToElementLen(S, Idx) + 1;
end;
{$ENDIF}

function MecsNextCharIndex(const S: WideString; Index: Integer): Integer;
var
  Idx: Integer;
begin
  Idx := MecsElementToCharIndex(S, Index);
  result := MecsCharToElementLen(S, Idx) + 1;
end;

// MecsReverseString
function MecsReverseString(const AText: ByteString; Codepage: DWORD): ByteString;
var
  i: Integer;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(AText)
  else
    dCodePage := Codepage;
  result := '';
  for i:=1 to MecsLength(AText, dCodePage) do
    result := MecsCopy(AText, i, 1, dCodePage) + result;
end;

{$IFDEF UNICODE}
function MecsReverseString(const AText: UnicodeString): UnicodeString;
var
  i: Integer;
begin
  result := '';
  for i:=1 to System.Length(AText) do
    begin
      case MecsElementType(AText, i) of
        etLead:;
        etTrail:
          result := AText[i-1] + AText[i] + result;
      else
        result := AText[i] + result;
      end;
    end;
end;
{$ENDIF}

function MecsReverseString(const AText: WideString): WideString;
var
  i: Integer;
begin
  result := '';
  for i:=1 to System.Length(AText) do
    begin
      case MecsElementType(AText, i) of
        etLead:;
        etTrail:
          result := WideString(AText[i-1]) + WideString(AText[i]) + result;
      else
        result := AText[i] + result;
      end;
    end;
end;

// MecsRightStr
function MecsRightStr(const AText: ByteString; const ACount: Integer; Codepage: DWORD): ByteString;
var
  Idx: Integer;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(AText)
  else
    dCodePage := Codepage;
  Idx :=  MecsLength(AText, dCodePage);
  if (Idx - ACount + 1) > 0 then
    result := MecsCopy(AText, Idx - ACount + 1, ACount, dCodePage)
  else
    result := AText;
end;

{$IFDEF UNICODE}
function MecsRightStr(const AText: UnicodeString; const ACount: Integer): UnicodeString;
var
  Idx: Integer;
begin
  Idx := MecsLength(AText);
  if (Idx - ACount + 1) > 0 then
    result := MecsCopy(AText, Idx - ACount + 1, ACount)
  else
    result := AText;
end;
{$ENDIF}

function MecsRightStr(const AText: WideString; const ACount: Integer): WideString;
var
  Idx: Integer;
begin
  Idx := MecsLength(AText);
  if (Idx - ACount + 1) > 0 then
    result := MecsCopy(AText, Idx - ACount + 1, ACount)
  else
    result := AText;
end;

// MecsStrCharLength
function MecsStrCharLength(const Str: PAnsiChar; Codepage: DWORD): Integer;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  if (Str = nil) or (Str = '') then
    result := 0
  else
    result := MecsCharLength(StrPas(Str), 1, dCodepage);
end;

{$IFDEF UNICODE}
function MecsStrCharLength(const Str: PUnicodeChar): Integer;
begin
  if (Str = nil) or (Str = '') then
    result := 0
  else
    result := MecsCharLength(Str^, 1);
end;
{$ENDIF}

function MecsStrCharLength(const Str: PWideChar): Integer;
begin
  if (Str = nil) or (Str = '') then
    result := 0
  else
    result := MecsCharLength(Str^, 1);
end;

// MecsStrConv
{$IFDEF UNICODE}
function MecsStrConv(const str: UnicodeString; Conversion: DWORD; Optional: LCID): UnicodeString;
{$ELSE}
function MecsStrConv(const str: WideString; Conversion: DWORD; Optional: LCID): WideString;
{$ENDIF}
var
  Size: Integer;
  dLocale: LCID;
begin
  if Optional = 0 then
    dLocale := DefaultLCID
  else
    dLocale := Optional;
  result := '';
  Size := LCMapStringW(dLocale, Conversion, PWideChar(str), Length(str), nil, 0);
  if Size > 0 then
    begin
      SetLength(result, Size);
      LCMapStringW(dLocale, Conversion, PWideChar(str), Length(str), PWideChar(Result), Size);
    end
  else
    result := str;
end;

function MecsStrConv(const str: ByteString; Conversion: DWORD; Optional: LCID): ByteString;
var
  Size: Integer;
  dLocale: LCID;
begin
  if Optional = 0 then
    dLocale := DefaultLCID
  else
    dLocale := Optional;
  result := '';
  Size := LCMapStringA(dLocale, Conversion, PAnsiChar(str), Length(str), nil, 0);
  if Size > 0 then
    begin
      SetLength(result, Size);
      LCMapStringA(dLocale, Conversion, PAnsiChar(str), Length(str), PAnsiChar(Result), Size);
    end
  else
    result := str;
end;

// MecsStrElementType
function MecsStrElementType(const Str: PAnsiChar; Index: Cardinal; Codepage: DWORD): TElementType;
var
  P: PAnsiChar;
  i: Integer;
  LeadBytes: TLeadBytes;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;  result := etSingle;
  if Str = nil then
    Exit;
  if (Index <= 0) or (Index + 1 > {$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrLen(Str)) then
    Exit;
  P := Str;
  if dCodePage = CP_UTF8 then
    begin
      Inc(P, Index);
      if (Byte(P^) and $80) = $00 then
        result := etSingle
      else if (Byte(P^) and $C0) = $C0 then
        result := etLead
      else
        result := etTrail;
    end
  else
    begin
      LeadBytes := MecsGetLeadBytes(dCodePage);
      if LeadBytes = [] then
        Exit;
      if (P = nil) or ((P + Index)^ = #$00) then
        Exit;
      if (Index = 0) then
        begin
          if P^ in LeadBytes then
            result := etLead;
        end
      else
        begin
          i := Index - 1;
          while (i >= 0) and ((P + i)^ in LeadBytes) do
            Dec(i);
          if ((Index - Cardinal(i)) mod 2) = 0 then
            result := etTrail
          else if (P + Index)^ in LeadBytes then
            result := etLead;
        end;
    end;
end;

{$IFDEF UNICODE}
function MecsStrElementType(const Str: PUnicodeChar; Index: Cardinal): TElementType;
begin
  result := etSingle;
  if Str = nil then
    Exit;
  if (Index <= 0) or ((Index + 1) > Cardinal({$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrLen(PWideChar(Str)))) then
    Exit;
  if MecsIsLeadElement(Str[Index]) then
    result := etLead
  else if MecsIsTrailElement(Str[Index]) then
    result := etTrail;
end;
{$ENDIF}

function MecsStrElementType(const Str: PWideChar; Index: Cardinal): TElementType;
begin
  result := etSingle;
  if Str = nil then
    Exit;
  {$IFDEF UNICODE}
  if (Index <= 0) or ((Index + 1) > Cardinal({$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrLen(Str))) then
  {$ELSE}
  if (Index <= 0) or ((Index + 1) > Cardinal(MECSUtils.StrLen(Str))) then
  {$ENDIF}
    Exit;
  if MecsIsLeadElement(Str[Index]) then
    result := etLead
  else if MecsIsTrailElement(Str[Index]) then
    result := etTrail;
end;

// MecsStringWidth
{$IFDEF UNICODE}
function MecsStringWidth(const S: UnicodeString; EAW: Boolean = True): Integer;
var
  i: Integer;
begin
  result := 0;
  for i:=1 to MecsLength(s) do
    if MecsIsFullWidth(s, i, EAW) then
      Inc(result, 2)
    else
      Inc(result, 1);
end;
{$ENDIF}

function MecsStringWidth(const S: WideString; EAW: Boolean = True): Integer;
var
  i: Integer;
begin
  result := 0;
  for i:=1 to MecsLength(s) do
    if MecsIsFullWidth(s, i, EAW) then
      Inc(result, 2)
    else
      Inc(result, 1);
end;

// MecsStrLen
function MecsStrLen(const Str: PAnsiChar; Codepage: DWORD): Cardinal;
var
  P: PAnsiChar;
  L, ChrCnt: Integer;
  LeadBytes: TLeadBytes;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  result := 0;
  L := {$IFDEF XE2_OR_LATER}System.{$ENDIF}SysUtils.StrLen(Str);
  if L = 0 then
    Exit;
  if dCodePage = CP_UTF8 then
    begin
      ChrCnt := 0;
      P := Str;
      while P^ <> #$00 do
        begin
          if (Byte(P^) and $80) = $00 then
            Inc(ChrCnt)
          else if (Byte(P^) and $C0) = $C0 then
            Inc(ChrCnt);
          Inc(P);
        end;
    end
  else
    begin
      LeadBytes := MecsGetLeadBytes(dCodePage);
      if LeadBytes = [] then
        ChrCnt := L
      else
        begin
          ChrCnt := 0;
          P := Str;
          while P^ <> #$00 do
            begin
              Inc(ChrCnt);
              if P^ in LeadBytes then
                begin
                  Inc(P);
                  if P^ = #$00 then
                    Break;
                end;
              Inc(P);
            end;
        end;
    end;
  result := ChrCnt;
end;

{$IFDEF UNICODE}
function MecsStrLen(const Str: PUnicodeChar): Cardinal;
var
  P: PUnicodeChar;
  ChrCnt: Integer;
begin
  ChrCnt := 0;
  P := Str;
  while P^ <> #$0000 do
    begin
      if MecsIsLeadElement(P^) then
        Inc(ChrCnt)
      else if not MecsIsTrailElement(P^) then
        Inc(ChrCnt);
      Inc(P);
    end;
  result := ChrCnt;
end;
{$ENDIF}

function MecsStrLen(const Str: PWideChar): Cardinal;
var
  P: PWideChar;
  ChrCnt: Integer;
begin
  ChrCnt := 0;
  P := Str;
  while P^ <> #$0000 do
    begin
      if MecsIsLeadElement(P^) then
        Inc(ChrCnt)
      else if not MecsIsTrailElement(P^) then
        Inc(ChrCnt);
      Inc(P);
    end;
  result := ChrCnt;
end;

// MecsStrNextChar
function MecsStrNextChar(const Str: PAnsiChar; Codepage: DWORD): PAnsiChar;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  result := str + MecsStrCharLength(str, dCodePage);
end;

{$IFDEF UNICODE}
function MecsStrNextChar(const Str: PUnicodeChar): PUnicodeChar;
begin
  result := str + MecsStrCharLength(str);
end;
{$ENDIF}

function MecsStrNextChar(const Str: PWideChar): PWideChar;
begin
  result := str + MecsStrCharLength(str);
end;

// for UnicodeString/WideString (Combining Character Sequence)
// -----------------------------------------------------------------------------

// MecsCCSLength
{$IFDEF UNICODE}
function MecsCCSLength(const S: UnicodeString; Index: Integer): Integer; overload;
var
  IC: TIndexCount;
begin
  IC := MecsCCSToElementIndexCount(S, Index, 1);
  if IC.StartIndex = 0 then
    result := 1
  else
    result := IC.Count;
end;
{$ENDIF}

function MecsCCSLength(const S: WideString; Index: Integer): Integer; overload;
var
  IC: TIndexCount;
begin
  IC := MecsCCSToElementIndexCount(S, Index, 1);
  if IC.StartIndex = 0 then
    result := 1
  else
    result := IC.Count;
end;

// MecsCCSToElementIndex
{$IFDEF UNICODE}
function MecsCCSToElementIndex(const S: UnicodeString; Index: Integer): Integer; overload;
const
  NullTerm = 1;
var
  L, ChrCnt: Integer;
  UCS4 : UCS4String;
  i: Integer;
begin
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  ChrCnt := 0;
  UCS4 := UTF16toUTF32(S);
  for i:=0 to Length(UCS4) - NullTerm - 1 do
    begin
      if MecsCombiningType(UCS4[i]) = ctBase then
        Inc(ChrCnt);
      if (ChrCnt = Index) then
        begin
          result := MecsCharToElementIndex(S, i + 1);
          break;
        end;
    end;
end;
{$ENDIF}

function MecsCCSToElementIndex(const S: WideString; Index: Integer): Integer; overload;
const
  NullTerm = 1;
var
  L, ChrCnt: Integer;
  UCS4 : UCS4String;
  i: Integer;
begin
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  ChrCnt := 0;
  SetLength(UCS4, 0);
  UCS4 := UTF16toUTF32(S);
  for i:=0 to Length(UCS4) - NullTerm - 1 do
    begin
      if MecsCombiningType(UCS4[i]) = ctBase then
        Inc(ChrCnt);
      if (ChrCnt = Index) then
        begin
          result := MecsCharToElementIndex(S, i + 1);
          break;
        end;
    end;
end;

// MecsCCSToElementIndexCount
{$IFDEF UNICODE}
function MecsCCSToElementIndexCount(const S: UnicodeString; Index, Count: Integer): TIndexCount; overload;
const
  NullTerm = 1;
var
  ChrCnt, L: Integer;
  UCS4 : UCS4String;
  i: Integer;
begin
  result.StartIndex := Index;
  result.EndIndex   := Index + Count - 1;
  result.Count      := Count;
  L := System.Length(S);
  if L = 0 then
    Exit;

  result.StartIndex := 0;
  result.EndIndex   := 0;
  result.Count      := 0;
  ChrCnt := 0;
  UCS4 := UTF16toUTF32(S);
  for i:=0 to Length(UCS4) - NullTerm - 1 do
    begin
      if MecsCombiningType(UCS4[i]) = ctBase then
        Inc(ChrCnt);
      if (ChrCnt = Index) and (result.StartIndex = 0) then
        result.StartIndex := MecsCharToElementIndex(S, i + 1);
      if (ChrCnt = (Index + Count - 1)) and (result.StartIndex > 0) then
        result.EndIndex := MecsCharToElementLen(S, i + 1)
      else if ChrCnt > (Index + Count - 1) then
        break;
    end;
  if (result.EndIndex = 0) and ((ChrCnt - Index) < Count) then
    result.EndIndex := L;
  result.Count := result.EndIndex - result.StartIndex + 1;
end;

{$ENDIF}
function MecsCCSToElementIndexCount(const S: WideString; Index, Count: Integer): TIndexCount; overload;
const
  NullTerm = 1;
var
  ChrCnt, L: Integer;
  UCS4 : UCS4String;
  i: Integer;
begin
  result.StartIndex := Index;
  result.EndIndex   := Index + Count - 1;
  result.Count      := Count;
  L := System.Length(S);
  if L = 0 then
    Exit;

  result.StartIndex := 0;
  result.EndIndex   := 0;
  result.Count      := 0;
  ChrCnt := 0;

  SetLength(UCS4, 0);
  UCS4 := UTF16toUTF32(S);
  for i:=0 to Length(UCS4) - NullTerm - 1 do
    begin
      if MecsCombiningType(UCS4[i]) = ctBase then
        Inc(ChrCnt);
      if (ChrCnt = Index) and (result.StartIndex = 0) then
        result.StartIndex := MecsCharToElementIndex(S, i + 1);
      if (ChrCnt = (Index + Count - 1)) and (result.StartIndex > 0) then
        result.EndIndex := MecsCharToElementLen(S, i + 1)
      else if ChrCnt > (Index + Count - 1) then
        break;
    end;
  if (result.EndIndex = 0) and ((ChrCnt - Index) < Count) then
    result.EndIndex := L;
  result.Count := result.EndIndex - result.StartIndex + 1;
end;

// MecsCCSToElementLen
{$IFDEF UNICODE}
function MecsCCSToElementLen(const S: UnicodeString; MaxLen: Integer): Integer; overload;
begin
  result := MecsCCSToElementIndex(S, MaxLen + 1) - 1;
  if result < MaxLen then
    result := System.Length(S);
end;
{$ENDIF}

function MecsCCSToElementLen(const S: WideString; MaxLen: Integer): Integer; overload;
begin
  result := MecsCCSToElementIndex(S, MaxLen + 1) - 1;
  if result < MaxLen then
    result := System.Length(S);
end;

// MecsElementToCCSIndex
{$IFDEF UNICODE}
function MecsElementToCCSIndex(const S: UnicodeString; Index: Integer): Integer; overload;
const
  NullTerm = 1;
var
  L, ChrCnt: Integer;
  UCS4: UCS4String;
  i, Idx: Integer;
begin
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  Idx := MecsElementToCharIndex(S, Index);
  ChrCnt := 0;
  UCS4 := UTF16ToUTF32(S);
  for i:=0 to Length(UCS4) - NullTerm - 1 do
    begin
      if MecsCombiningType(UCS4[i]) = ctBase then
        Inc(ChrCnt);
      if i = (Idx - 1) then
        begin
          result := ChrCnt;
          Break;
        end;
    end;
end;
{$ENDIF}

function MecsElementToCCSIndex(const S: WideString; Index: Integer): Integer; overload;
const
  NullTerm = 1;
var
  L, ChrCnt: Integer;
  UCS4: UCS4String;
  i, Idx: Integer;
begin
  result := 0;
  L := System.Length(S);
  if L = 0 then
    Exit;
  Idx := MecsElementToCharIndex(S, Index);
  ChrCnt := 0;
  SetLength(UCS4, 0);
  UCS4 := UTF16ToUTF32(S);
  for i:=0 to Length(UCS4) - NullTerm - 1 do
    begin
      if MecsCombiningType(UCS4[i]) = ctBase then
        Inc(ChrCnt);
      if i = (Idx - 1) then
        begin
          result := ChrCnt;
          Break;
        end;
    end;
end;

// MecsElementToCCSLen
{$IFDEF UNICODE}
function MecsElementToCCSLen(const S: UnicodeString; MaxLen: Integer): Integer; overload;
begin
  result := MecsElementToCCSIndex(S, MaxLen);
  if result = 0 then
    result := MecsLengthC(S);
end;
{$ENDIF}

function MecsElementToCCSLen(const S: WideString; MaxLen: Integer): Integer; overload;
begin
  result := MecsElementToCCSIndex(S, MaxLen);
  if result = 0 then
    result := MecsLengthC(S);
end;

// MecsDeleteC
{$IFDEF UNICODE}
procedure MecsDeleteC(var S: UnicodeString; Index, Count: Integer); overload;
var
  AIC: TIndexCount;
begin
  AIC := MecsCCSToElementIndexCount(S, Index, Count);
  if AIC.StartIndex > 0 then
    Delete(S, AIC.StartIndex, AIC.Count);
end;
{$ENDIF}

procedure MecsDeleteC(var S: WideString; Index, Count: Integer); overload;
var
  AIC: TIndexCount;
begin
  AIC := MecsCCSToElementIndexCount(S, Index, Count);
  if AIC.StartIndex > 0 then
    Delete(S, AIC.StartIndex, AIC.Count);
end;

// MecsLengthC
{$IFDEF UNICODE}
function MecsLengthC(const S: UnicodeString): Integer; overload;
const
  NullTerm = 1;
var
  Cnt: Integer;
  UCS4 : UCS4String;
  i: Integer;
begin
  Cnt := 0;
  if Length(S) > 0 then
    begin
      UCS4 := UTF16toUTF32(S);
      Cnt := Length(UCS4) - NullTerm - 1;
      for i:=0 to Length(UCS4) - NullTerm - 1 do
        begin
          if MecsCombiningType(UCS4[i]) = ctCombining then
            Dec(Cnt);
        end;
    end;
  result := Cnt;
end;
{$ENDIF}

function MecsLengthC(const S: WideString): Integer; overload;
const
  NullTerm = 1;
var
  Cnt: Integer;
  UCS4 : UCS4String;
  i: Integer;
begin
  Cnt := 0;
  if Length(S) > 0 then
    begin
      SetLength(UCS4, 0);
      UCS4 := UTF16toUTF32(S);
      Cnt := Length(UCS4) - NullTerm - 1;
      for i:=0 to Length(UCS4) - NullTerm - 1 do
        begin
          if MecsCombiningType(UCS4[i]) = ctCombining then
            Dec(Cnt);
        end;
    end;
  result := Cnt;
end;

// MecsCopyC
{$IFDEF UNICODE}
function MecsCopyC(const S: UnicodeString; Index, Count: Integer): UnicodeString; overload;
var
  AIC: TIndexCount;
begin
  AIC := MecsCCSToElementIndexCount(S, Index, Count);
  if AIC.StartIndex > 0 then
    result := Copy(S, AIC.StartIndex, AIC.Count)
  else
    result := '';
end;
{$ENDIF}

function MecsCopyC(const S: WideString; Index, Count: Integer): WideString; overload;
var
  AIC: TIndexCount;
begin
  AIC := MecsCCSToElementIndexCount(S, Index, Count);
  if AIC.StartIndex > 0 then
    result := Copy(S, AIC.StartIndex, AIC.Count)
  else
    result := '';
end;

// MecsInsertC
{$IFDEF UNICODE}
procedure MecsInsertC(const Source: UnicodeString; var S: UnicodeString; Index: Integer); overload;
var
  Idx: Integer;
begin
  Idx := MecsCCSToElementIndex(S, Index);
  if Idx > 0 then
    Insert(Source, S, Idx)
  else
    S := S + Source;
end;
{$ENDIF}

procedure MecsInsertC(const Source: WideString; var S: WideString; Index: Integer); overload;
var
  Idx: Integer;
begin
  Idx := MecsCCSToElementIndex(S, Index);
  if Idx > 0 then
    Insert(Source, S, Idx)
  else
    S := S + Source;
end;

// MecsLeftStrC
{$IFDEF UNICODE}
function MecsLeftStrC(const AText: UnicodeString; const ACount: Integer): UnicodeString; overload;
begin
  result := MecsCopyC(AText, 1, ACount);
end;
{$ENDIF}

function MecsLeftStrC(const AText: WideString; const ACount: Integer): WideString; overload;
begin
  result := MecsCopyC(AText, 1, ACount);
end;

// MecsMidStr
{$IFDEF UNICODE}
function MecsMidStrC(const AText: UnicodeString; const AStart, ACount: Integer): UnicodeString; overload;
begin
  result := MecsCopyC(AText, AStart, ACount);
end;
{$ENDIF}

function MecsMidStrC(const AText: WideString; const AStart, ACount: Integer): WideString; overload;
begin
  result := MecsCopyC(AText, AStart, ACount);
end;

// MecsNextCCSIndex
{$IFDEF UNICODE}
function MecsNextCCSIndex(const S: UnicodeString; Index: Integer): Integer; overload;
var
  Idx: Integer;
begin
  Idx := MecsElementToCCSIndex(S, Index);
  result := MecsCCSToElementLen(S, Idx) + 1;
end;
{$ENDIF}

function MecsNextCCSIndex(const S: WideString; Index: Integer): Integer; overload;
var
  Idx: Integer;
begin
  Idx := MecsElementToCCSIndex(S, Index);
  result := MecsCCSToElementLen(S, Idx) + 1;
end;

// MecsReverseStringC
{$IFDEF UNICODE}
function MecsReverseStringC(const AText: UnicodeString): UnicodeString; overload;
var
  i: Integer;
begin
  result := '';
  for i := MECSLengthC(AText) downto 1 do
    result := result + MecsCopyC(AText, i, 1);
end;
{$ENDIF}

function MecsReverseStringC(const AText: WideString): WideString; overload;
var
  i: Integer;
begin
  result := '';
  for i := MecsLengthC(AText) downto 1 do
    result := result + MecsCopyC(AText, i, 1);
end;

// MecsRightStrC
{$IFDEF UNICODE}
function MecsRightStrC(const AText: UnicodeString; const ACount: Integer): UnicodeString; overload;
var
  Idx: Integer;
begin
  Idx := MecsLengthC(AText);
  if (Idx - ACount + 1) > 0 then
    result := MecsCopyC(AText, Idx - ACount + 1, ACount)
  else
    result := AText;
end;
{$ENDIF}

function MecsRightStrC(const AText: WideString; const ACount: Integer): WideString; overload;
var
  Idx: Integer;
begin
  Idx := MecsLengthC(AText);
  if (Idx - ACount + 1) > 0 then
    result := MecsCopyC(AText, Idx - ACount + 1, ACount)
  else
    result := AText;
end;

// MecsStrCCSLength
{$IFDEF UNICODE}
function MecsStrCCSLength(const Str: PUnicodeChar): Integer; overload;
begin
  result := MecsCCSLength(StrPas(PWideChar(Str)), 1);
end;
{$ENDIF}

function MecsStrCCSLength(const Str: PWideChar): Integer; overload;
begin
  result := MecsCCSLength(WideString(Str), 1);
end;

// MecsStrLenC
{$IFDEF UNICODE}
function MecsStrLenC(const Str: PUnicodeChar): Cardinal; overload;
begin
  result := MECSLengthC(StrPas(PWideChar(Str)));
end;
{$ENDIF}

function MecsStrLenC(const Str: PWideChar): Cardinal; overload;
begin
  result := MECSLengthC(WideString(Str));
end;

// MecsStrNextCCS
{$IFDEF UNICODE}
function MecsStrNextCCS(const Str: PUnicodeChar): PUnicodeChar; overload;
begin
  result := str + MecsStrCCSLength(str);
end;
{$ENDIF}

function MecsStrNextCCS(const Str: PWideChar): PWideChar; overload;
begin
  result := str + MecsStrCCSLength(str);
end;

// for Convert
// -----------------------------------------------------------------------------

// IsWin95
function IsWin95: Boolean;
begin
  result := (Win32Platform = VER_PLATFORM_WIN32_WINDOWS) and (Win32MinorVersion = 0);
end;

// IsWin2000
function IsWin2000: Boolean;
begin
  result := (Win32Platform = VER_PLATFORM_WIN32_NT) and (Win32MajorVersion = 5) and (Win32MinorVersion = 0);
end;

// AnsiToUTF16
function AnsiToUTF16(const AText: ByteString; Codepage: DWORD): WideString;
// MultiByteToWideChar
// http://msdn2.microsoft.com/en-us/library/ms776413(VS.85).aspx
// CP_UTF8(CP_UTF7) is *not* supported in Win95.
var
  BufSize: Integer;
  dwFlags: DWORD;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(AText)
  else
    dCodePage := Codepage;
  result := '';
  if Length(AText) = 0 then
    Exit;

  case dCodePage of
    932: // shift_jis (cp932)
      dwFlags := MB_PRECOMPOSED;
  else
    dwFlags := 0;
  end;

  if IsWin95 and (dwFlags = 0) then
    ConvertMultiByteToUnicode(dCodepage, AText, result)
  else
    begin
      BufSize := MultiByteToWideChar(dCodePage, dwFlags, PAnsiChar(AText), Length(AText), nil, 0);
      SetLength(result, BufSize);
      MultiByteToWideChar(dCodePage, dwFlags, PANsiChar(AText), Length(AText), PWideChar(result), BufSize);
    end;
end;

// AnsiToUTF32
function AnsiToUTF32(const AText: ByteString; Codepage: DWORD): UCS4String;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(AText)
  else
    dCodePage := Codepage;
  result := UTF16ToUTF32(AnsiToUTF16(AText, dCodePage));
end;

// AnsiToUTF8
{$IFDEF UNICODE}
function AnsiToUTF8(const AText: ByteString; Codepage: DWORD): ByteString;
{$ELSE}
function AnsiToUTF8(const AText: ByteString; Codepage: DWORD): UTF8String;
{$ENDIF}
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := MecsGetCodePage(AText)
  else
    dCodePage := Codepage;
  if dCodePage = CP_UTF8 then
    result := AText
  else
    begin
      if IsWin95 then
        ConvertString(dCodepage, CP_UTF8, AText, ByteString(result))
      else
        result := UTF16ToAnsi(AnsiToUTF16(AText, dCodePage), CP_UTF8);
    end;
end;

function ConvertINetString(lpdwMode: LPDWORD; dwSrcEncoding: DWORD; dwDstEncoding: DWORD; lpSrcStr: LPCSTR; lpnSrcSize: PInteger; lpDstStr: Pointer; lpnDstSize: PInteger):Hresult;
var
  FP: TFarProc;
  DLLWnd: THandle;
  // http://msdn2.microsoft.com/en-us/library/aa741106(VS.85).aspx
  // Win95 and IE5.5 or later
  _ConvertINetString: function(lpdwMode: LPDWORD; dwSrcEncoding: DWORD; dwDstEncoding: DWORD; lpSrcStr: LPCSTR; lpnSrcSize: PInteger; lpDstStr: Pointer; lpnDstSize: PInteger):Hresult; stdcall;
begin
  result := S_FALSE;
  {$IFDEF UNICODE}
  DLLWnd := LoadLibraryW('mlang.dll');
  {$ELSE}
  DLLWnd := LoadLibraryA('mlang.dll');
  {$ENDIF}
  try
    FP := GetProcAddress(DLLWnd, 'ConvertINetString');
    if FP <> nil then
      begin
        @_ConvertINetString := FP;
        result := _ConvertINetString(lpdwMode, dwSrcEncoding, dwDstEncoding, lpSrcStr, lpnSrcSize, lpDstStr, lpnDstSize);
      end;
  finally
    if DLLWnd > 0 then
      FreeLibrary(DLLWnd);
  end;
end;

// CodePointToUTF16
function CodePointToUTF16(const CodePoint: UCS4Char): WideString;
var
  U: UCS4String;
begin
  SetLength(U, 2);
  U[0] := CodePoint;
  U[1] := $00000000;
  result := UTF32ToUTF16(U);
end;

// CodePointToUTF8
{$IFDEF UNICODE}
function CodePointToUTF8(const CodePoint: UCS4Char): ByteString;
{$ELSE}
function CodePointToUTF8(const CodePoint: UCS4Char): UTF8String;
{$ENDIF}
var
  U: UCS4String;
begin
  SetLength(U, 2);
  U[0] := CodePoint;
  U[1] := $00000000;
  result := UTF32ToUTF8(U);
end;

// ConvertString
function ConvertString(SrcCodepage, DstCodepage: DWORD; const SrcStr: ByteString; var DstStr: ByteString): Boolean;
var
  dwMode: DWORD;
  SrcSize, DstSize: Integer;
begin
  dwMode := 0;
  SrcSize := Length(SrcStr);
  DstSize := Length(SrcStr) * 3 + 3;
  SetLength(DstStr, DstSize);
  result := (ConvertINetString(@dwMode, SrcCodepage, DstCodepage, PAnsiChar(SrcStr), @SrcSize, PAnsiChar(DstStr), @DstSize) = S_OK);
  if result then
    SetLength(DstStr, DstSize)
  else
    DstStr := '';
end;

function ConvertINetUnicodeToMultiByte(lpdwMode: LPDWORD; dwEncoding: DWORD; lpSrcStr: LPCWSTR; lpnWideCharCount: PInteger; lpDstStr: LPSTR; lpnMultiCharCount: PInteger):Hresult;
var
  FP: TFarProc;
  DLLWnd: THandle;
  // http://msdn2.microsoft.com/en-us/library/aa741107(VS.85).aspx
  // Win95 and IE5.5 or later
  _ConvertINetUnicodeToMultiByte: function(lpdwMode: LPDWORD; dwEncoding: DWORD; lpSrcStr: LPCWSTR; lpnWideCharCount: PInteger; lpDstStr: LPSTR; lpnMultiCharCount: PInteger):Hresult; stdcall;
begin
  result := S_FALSE;
  {$IFDEF UNICODE}
  DLLWnd := LoadLibraryW('mlang.dll');
  {$ELSE}
  DLLWnd := LoadLibraryA('mlang.dll');
  {$ENDIF}
  try
    FP := GetProcAddress(DLLWnd, 'ConvertINetUnicodeToMultiByte');
    if FP <> nil then
      begin
        @_ConvertINetUnicodeToMultiByte := FP;
        result := _ConvertINetUnicodeToMultiByte(lpdwMode, dwEncoding, lpSrcStr, lpnWideCharCount, lpDstStr, lpnMultiCharCount);
      end;
  finally
    if DLLWnd > 0 then
      FreeLibrary(DLLWnd);
  end;
end;

// TrimNullTerm
procedure TrimNullTerm(var S: WideString); overload;
var
  i: Integer;
  Cnt: Integer;
begin
  Cnt := 0;
  for i:=Length(S) downto 1 do
    begin
      if Word(S[i]) <> $0000 then
        Break;
      Inc(Cnt);
    end;
  if Cnt > 0 then
    SetLength(S, Length(S) - Cnt);
end;

procedure TrimNullTerm(var S: UCS4String); overload;
var
  i: Integer;
  Cnt: Integer;
begin
  Cnt := 0;
  for i:=Length(S) - 1 downto 0 do
    begin
      if S[i] <> $00000000 then
        Break;
      Inc(Cnt);
    end;
  if Cnt > 0 then
    SetLength(S, Length(S) - Cnt);
end;

procedure TrimNullTerm(var S: TBytes); overload;
var
  i: Integer;
  Cnt: Integer;
begin
  Cnt := 0;
  for i:=Length(S) - 1 downto 0 do
    begin
      if S[i] <> $00 then
        Break;
      Inc(Cnt);
    end;
  if Cnt > 0 then
    SetLength(S, Length(S) - Cnt);
end;

// ConvertUnicodeToMultiByte
function ConvertUnicodeToMultiByte(DstCodepage: DWORD; const SrcStr: WideString; var DstStr: ByteString): Boolean;
var
  dwMode: DWORD;
  SrcSize, DstSize: Integer;
begin
  dwMode := 0;
  SetLength(DstStr, Length(SrcStr) * 5 + 3);
  SrcSize := Length(SrcStr);
  DstSize := Length(DstStr);
  result := (ConvertINetUnicodeToMultiByte(@dwMode, DstCodepage, PWideChar(SrcStr), @SrcSize, PAnsiChar(DstStr), @DstSize) = S_OK);
  if result then
    SetLength(DstStr, DstSize)
  else
    DstStr := '';
end;

function ConvertINetMultiByteToUnicode(lpdwMode: LPDWORD; dwSrcEncoding: DWORD; lpSrcStr: LPCSTR; lpnMultiCharCount: PInteger; lpDstStr: LPWSTR; lpnWideCharCount: PInteger):Hresult;
var
  FP: TFarProc;
  DLLWnd: THandle;
  // http://msdn2.microsoft.com/en-us/library/aa741105(VS.85).aspx
  // Win95 and IE5.5 or later
  _ConvertINetMultiByteToUnicode: function(lpdwMode: LPDWORD; dwSrcEncoding: DWORD; lpSrcStr: LPCSTR; lpnMultiCharCount: PInteger; lpDstStr: LPWSTR; lpnWideCharCount: PInteger):Hresult; stdcall;
begin
  result := S_FALSE;
  {$IFDEF UNICODE}
  DLLWnd := LoadLibraryW('mlang.dll');
  {$ELSE}
  DLLWnd := LoadLibraryA('mlang.dll');
  {$ENDIF}
  try
    FP := GetProcAddress(DLLWnd, 'ConvertINetMultiByteToUnicode');
    if FP <> nil then
      begin
        @_ConvertINetMultiByteToUnicode := FP;
        result := _ConvertINetMultiByteToUnicode(lpdwMode, dwSrcEncoding, lpSrcStr, lpnMultiCharCount, lpDstStr, lpnWideCharCount);
      end;
  finally
    if DLLWnd > 0 then
      FreeLibrary(DLLWnd);
  end;
end;

// ConvertMultiByteToUnicode
function ConvertMultiByteToUnicode(SrcCodepage: DWORD; const SrcStr: ByteString; var DstStr: WideString): Boolean;
var
  dwMode: DWORD;
  SrcSize, DstSize: Integer;
begin
  dwMode := 0;
  SetLength(DstStr, Length(SrcStr) * 3);
  SrcSize := Length(SrcStr);
  DstSize := Length(DstStr);
  result := (ConvertINetMultiByteToUnicode(@dwMode, SrcCodepage, PAnsiChar(SrcStr), @SrcSize, PWideChar(DstStr), @DstSize) = S_OK);
  if result then
    SetLength(DstStr, DstSize)
  else
    DstStr := '';
end;

// UTF32ToAnsi
function UTF32ToAnsi(const UCS4Text: UCS4String; Codepage: DWORD): ByteString;
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  result := UTF16ToAnsi(UTF32ToUTF16(UCS4Text), dCodePage);
end;

// UTF32ToUTF16
function UTF32ToUTF16(const UCS4Text: UCS4String): WideString;
var
  Counter,
  i: Integer;
  U: UCS4Char;
  PU, OU: PUCS4Char;
  PW: PWideChar;
  NullTerm: Integer;
begin
  result := '';
  if Length(UCS4Text) = 0 then
    Exit;
  if UCS4Text[0] = $00000000 then
    Exit;
  NullTerm := Integer(UCS4Text[Length(UCS4Text) - 1] = $00000000);

  Counter := 0;
  PU := PUCS4Char(UCS4Text);
  OU := PU;
  for i := 0 to Length(UCS4Text) - NullTerm - 1 do
    begin
      if PU^ < $10000 then
        Inc(Counter)
      else
        Inc(Counter, 2);
      Inc(PU);
    end;
  SetLength(result, Counter);

  PU := OU;
  PW := PWideChar(result);
  for i := 0 to Length(UCS4Text) - NullTerm - 1 do
    begin
      if PU^ < $10000 then
        begin
          PW^ := WideChar(PU^);
          Inc(PW);
        end
      else
        begin
          // Surrogate Pair
          U := PU^ - $10000;
          PW^ := WideChar((U shr 10) or $D800);
          Inc(PW);
          PW^ := WideChar((U and $000003FF) or $DC00);
          Inc(PW);
        end;
      Inc(PU);
    end;
end;

// UTF32ToUTF8
{$IFDEF UNICODE}
function UTF32ToUTF8(const UCS4Text: UCS4String): ByteString;
{$ELSE}
function UTF32ToUTF8(const UCS4Text: UCS4String): UTF8String;
{$ENDIF}
const
  NullTerm = 1;
var
  Counter: Integer;
  SrcLen, SrcLen2, i: Integer;
  J, CharLen: Byte;
  U: UCS4Char;
  PU, PO: PUCS4Char;
  PR: PUTF8Char;
begin
  result := '';
  SrcLen := Length(UCS4Text);
  SrcLen2 := SrcLen - NullTerm;
  if SrcLen = 0 then
    Exit;

  Counter := 0;
  PU := PUCS4Char(UCS4Text);
  PO := PU;
  for i:=0 to SrcLen2-1 do
    begin
      U := PU^;
      case U of
        $00..$7F:
          CharLen := 1;
        $80..$7FF:
          CharLen := 2;
        $800..$FFFF:
          CharLen := 3;
        $10000..$1FFFFF:
          CharLen := 4;
        $200000..$3FFFFFF:
          CharLen := 5;
        $4000000..$7FFFFFFF:
          CharLen := 6;
      else
        CharLen := 0;
      end;
      if CharLen > 0 then
        Inc(Counter, CharLen);
      Inc(PU);
    end;
  SetLength(result, Counter);

  PU := PO;
  PR := PUTF8Char(result);
  for i:=0 to SrcLen2-1 do
    begin
      U := PU^;
      case U of
        $00..$7F:
          begin
            PR^ := UTF8Char(Byte(U));
            Inc(PR);
            CharLen := 1;
          end;
        $80..$7FF:
          CharLen := 2;
        $800..$FFFF:
          CharLen := 3;
        $10000..$1FFFFF:
          CharLen := 4;
        $200000..$3FFFFFF:
          CharLen := 5;
        $4000000..$7FFFFFFF:
          CharLen := 6;
      else
        CharLen := 0;
      end;
      if CharLen >= 2 then
        begin
          Inc(PR, CharLen - 1);
          for J := CharLen downto 2 do
            begin
              PR^ := UTF8Char(Byte((U and $3F) or $80));
              U := U shr 6;
              Dec(PR);
            end;
          PR^ := UTF8Char(Byte(U or ($FE shl (7 - CharLen))));
          Inc(PR, CharLen);
        end;
      Inc(PU);
    end;
end;

// UTF16ToAnsi
function UTF16ToAnsi(const WText: WideString; Codepage: DWORD): ByteString;
// WideCharToMultiByte
// http://msdn2.microsoft.com/en-us/library/ms776420(VS.85).aspx
// CP_UTF8(CP_UTF7) is *not* supported in Win95.
var
  BufSize: Integer;
  dwFlags: DWORD;
  {$IFDEF UNICODE}
  dmyCodePage: WORD;
  {$ENDIF}
  NeedFix: Boolean;
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  {$IFDEF UNICODE}
  dmyCodePage := StringCodePage(result);
  {$ENDIF}
  result := '';
  if Length(WText) = 0 then
    Exit;

  case dCodePage of
    932: // shift_jis (cp932)
      dwFlags := WC_COMPOSITECHECK;
  else
    dwFlags := 0;
  end;

  if IsWin95 and (dwFlags = 0) then
    ConvertUnicodeToMultiByte(dCodepage, WText, result)
  else
    begin
      BufSize := 0;
      NeedFix := False;
      if IsWin2000 then
        case dCodepage of
          50220, // ISO-2022-JP
          50221, // csISO2022JP
          50222, // iso-2022-jp
          50225, // iso-2022-kr
          50227, // x-cp50227
          50229: // iso-2022-tw
            NeedFix := True;
        end;
      if not NeedFix then
        begin
          BufSize := WideCharToMultiByte(dCodePage, dwFlags, PWideChar(WText), -1 , nil, 0, nil, nil);
          NeedFix := (BufSize = 0);
        end;
      if NeedFix then
        begin
          BufSize := (Length(WText) * 5) + 4 - (Length(WText) div 2);
          SetLength(result, BufSize - 1);
          BufSize := WideCharToMultiByte(dCodePage, dwFlags, PWideChar(WText), -1 , PAnsiChar(result), BufSize, nil, nil);
          SetLength(result, BufSize - 1);
        end
      else
        begin
          SetLength(result, BufSize - 1);
          WideCharToMultiByte(dCodePage, dwFlags, PWideChar(WText), -1 , PAnsiChar(result), BufSize, nil, nil);
        end;

      {$IFDEF UNICODE}
      case dCodePage of
        CP_UTF7,
        CP_UTF8:
          SetCodePage(RawByteString(result), dCodePage, False);
      else
        if dCodePage <> dmyCodePage then
          SetCodePage(RawByteString(result), dmyCodePage, True);
      end;
      {$ENDIF}
    end;
end;

// UTF16ToUTF32
function UTF16ToUTF32(const WText: WideString): UCS4String;
var
  Counter,
  i: Integer;
  PW, OW: PWideChar;
  PR: PUCS4Char;
  SrcLen: Integer;
  SrcLen2: Integer;
begin
  result := nil;
  SrcLen := Length(WText);
  SrcLen2 := SrcLen - 1;
  if SrcLen = 0 then
    Exit;

  Counter := 0;
  PW := PWideChar(WText);
  OW := PW;
  for i := 0 to SrcLen2 do
    begin
      if MecsIsLeadElement(PW^) then
        begin
          // Surrogate Pair
          if MecsIsTrailElement((PW + 1)^) then
            Inc(Counter);
        end
      else if not MecsIsTrailElement(PW^) then
        Inc(Counter);
      Inc(PW);
    end;
  SetLength(result, Counter + 1);

  PW := OW;
  PR := PUCS4Char(result);
  for i := 0 to SrcLen2 do
    begin
      if MecsIsLeadElement(PW^) then
        begin
          // Surrogate Pair
          if MecsIsTrailElement((PW + 1)^) then
            begin
              PR^ := ((WORD(PW^)       and $03FF) shl 10) +
                     ((WORD((PW + 1)^) and $03FF) + $10000);
              Inc(PR);
            end;
        end
      else if not MecsIsTrailElement(PW^) then
        begin
          PR^ := UCS4Char(PW^);
          Inc(PR);
        end;
      Inc(PW);
    end;
  PR^ := 0;
end;

// UTF16ToUTF8
{$IFDEF UNICODE}
function UTF16ToUTF8(const WText: WideString): ByteString;
{$ELSE}
function UTF16ToUTF8(const WText: WideString): UTF8String;
{$ENDIF}
begin
  if IsWin95 then
    ConvertUnicodeToMultiByte(CP_UTF8, WText, ByteString(result))
  else
    result := UTF16ToAnsi(WText, CP_UTF8);
end;

// UTF8ToAnsi
{$IFDEF UNICODE}
function UTF8ToAnsi(const UTF8Text: ByteString; Codepage: DWORD): ByteString;
{$ELSE}
function UTF8ToAnsi(const UTF8Text: UTF8String; Codepage: DWORD): ByteString;
{$ENDIF}
var
  dCodePage: DWORD;
begin
  if Codepage = 0 then
    dCodePage := DefaultAnsiCodePage
  else
    dCodePage := Codepage;
  if dCodePage = CP_UTF8 then
    result := UTF8Text
  else
    begin
      if IsWin95 then
        ConvertString(CP_UTF8, dCodepage, UTF8Text, result)
      else
        result := UTF16ToAnsi(AnsiToUTF16(UTF8Text, CP_UTF8), dCodePage);
    end;
end;

// UTF8ToUTF16
{$IFDEF UNICODE}
function UTF8ToUTF16(const UTF8Text: ByteString): WideString;
{$ELSE}
function UTF8ToUTF16(const UTF8Text: UTF8String): WideString;
{$ENDIF}
begin
  if IsWin95 then
    ConvertMultiByteToUnicode(CP_UTF8, UTF8Text, result)
  else
    result := AnsiToUTF16(UTF8Text, CP_UTF8);
end;

// UTF8ToUTF32
{$IFDEF UNICODE}
function UTF8ToUTF32(const UTF8Text: ByteString): UCS4String;
{$ELSE}
function UTF8ToUTF32(const UTF8Text: UTF8String): UCS4String;
{$ENDIF}
var
  P, O: PUTF8Char;
  J, CharLen: Byte;
  Counter, SrcLen: Integer;
  U: UCS4Char;
  PU: PUCS4Char;
begin
  result := nil;
  SrcLen := Length(UTF8Text);
  if SrcLen = 0 then
    Exit;

  Counter := 0;
  P := PUTF8Char(UTF8Text);
  O := P;
  while (P - O) < SrcLen do
    begin
      if (Byte(P^) and $80) = $00 then
        Inc(Counter)
      else if(Byte(P^) and $C0) = $C0 then
        begin
          CharLen := 0;
          for J := 2 to 6 do
            begin
              if (Byte(P^) and ($80 shr J)) = 0 then
                begin
                  CharLen := J;
                  Break;
                end;
            end;
          if (P - O) <= (SrcLen - CharLen) then
            begin
              for J := 2 to CharLen do
                Inc(P);
              Inc(Counter);
            end;
        end;
      Inc(P);
    end;
  SetLength(result, Counter + 1);

  P := O;
  PU := PUCS4Char(result);
  while (P - O) < SrcLen do
    begin
      if (Byte(P^) and $80) = $00 then
        begin
          PU^ := Byte(P^);
          Inc(PU);
        end
      else if(Byte(P^) and $C0) = $C0 then
        begin
          CharLen := 0;
          for J := 2 to 6 do
            begin
              if (Byte(P^) and ($80 shr J)) = 0 then
                begin
                  CharLen := J;
                  Break;
                end;
            end;
          if (P - O) <= (SrcLen - CharLen) then
            begin
              U := Byte((Byte(P^) shl (CharLen + 1)));
              U := U shr (CharLen + 1);
              for J := 2 to CharLen do
                begin
                  Inc(P);
                  U := (U shl 6);
                  U := U or (Byte(P^) and $3F);
                end;
              PU^ := U;
              Inc(PU);
            end;
        end;
      Inc(P);
    end;
  PU^ := $00000000;
end;


// for HTML
// -----------------------------------------------------------------------------

// MecsCodePageToCharset
{$IFDEF UNICODE}
function MecsCodePageToCharset(const Codepage: WORD): UnicodeString;
{$ELSE}
function MecsCodePageToCharset(const Codepage: WORD): WideString;
{$ENDIF}
const
  Key_Encoding = '\Mime\Database\Codepage\';
  key_Charset  = 'WebCharset';
var
  Reg: TRegistry;
begin
  result := '';
  Reg := TRegistry.Create;
  try
    Reg.RootKey := HKEY_CLASSES_ROOT;
    if not Reg.KeyExists(Key_Encoding + IntToStr(Codepage)) then
      Exit;
    Reg.OpenKeyReadOnly(Key_Encoding + IntToStr(Codepage));
    try
      if Reg.ValueExists(key_Charset) then
        result := Reg.ReadString(key_Charset);
    finally
      Reg.CloseKey;
    end;
  finally
    Reg.Free;
  end;
end;

// MecsCharsetToCodePage
{$IFDEF UNICODE}
function MecsCharsetToCodePage(const Charset: UnicodeString): WORD;
{$ELSE}
function MecsCharsetToCodePage(const Charset: WideString): WORD;
{$ENDIF}
const
  key_Charset  = '\Mime\Database\Charset\';
  Key_Encoding = 'InternetEncoding';
  key_Alias    = 'AliasForCharset';
var
  Reg: TRegistry;
  Names: TStringList;
  Idx: Integer;
  AliasName: string;
begin
  result := 0;
  if Charset = '' then
    Exit;
  Reg := TRegistry.Create;
  Names := TStringList.Create;
  try
    Reg.RootKey := HKEY_CLASSES_ROOT;
    Reg.OpenKeyReadOnly(key_Charset);
    try
      Reg.GetKeyNames(Names);
      Names.Sort;
      if not Names.Find(Charset, Idx) then
        Exit;
    finally
      Reg.CloseKey;
    end;
    AliasName := '';
    Reg.OpenKeyReadOnly(key_Charset + Names[Idx]);
    try
      if Reg.ValueExists(Key_Encoding) then
        begin
          result := Reg.ReadInteger(Key_Encoding);
          Exit;
        end
      else if Reg.ValueExists(key_Alias) then
        AliasName := Reg.ReadString(key_Alias);
    finally
      Reg.CloseKey;
    end;
    if AliasName = '' then
      Exit;
    Reg.OpenKeyReadOnly(key_Charset + AliasName);
    try
      if Reg.ValueExists(Key_Encoding) then
        result := Reg.ReadInteger(Key_Encoding);
    finally
      Reg.CloseKey;
    end;
  finally
    Names.Free;
    Reg.Free;
  end;
end;

{$IFDEF UNICODE}
function MecsHTMLDecode(const s: UnicodeString): UnicodeString;
{$ELSE}
function MecsHTMLDecode(const s: WideString): WideString;
{$ENDIF}
var
  U4: UCS4String;
  P, dP: PUCS4Char;
  dLen, v, code: Integer;
  ret, Dmy: String;
begin
  ret := '';
  if s = '' then
    Exit;
  U4 := UTF16ToUTF32(s);
  P := @U4[0];
  while P^ <> $00000000 do
    begin
      if P^ = $00000026 then // &
        begin
          Dmy := UTF32toUTF16(Copy(UCS4String(P), 0, 10)); // Max = '&#1114111;'
          if      Pos('&amp;', LowerCase(Dmy)) = 1 then
            begin
              ret := ret + '&';
              Inc(P, 4);
            end
          else if Pos('&lt;', LowerCase(Dmy)) = 1 then
            begin
              ret := ret + '<';
              Inc(P, 3);
            end
          else if Pos('&gt;', LowerCase(Dmy)) = 1 then
            begin
              ret := ret + '>';
              Inc(P, 3);
            end
          else if Pos('&quot;', LowerCase(Dmy)) = 1 then
            begin
              ret := ret + '"';
              Inc(P, 5);
            end
          else if Pos('&#', Dmy) = 1 then
            begin
              dP := P;
              dLen := 0;
              while (dP^ <> $0000003B) and (dP^ <> $00000000) do
                begin
                  Inc(dLen);
                  Inc(dp);
                end;
              Dmy := UTF32toUTF16(Copy(UCS4String(P), 2, dLen - 2));
              Val(Dmy, v, code);
              if code = 0 then
                begin
                  ret := ret + CodePointToUTF16(v);
                  Inc(P, dLen);
                end
              else
                ret := ret + CodePointToUTF16(P^);
            end
          else
            ret := ret + CodePointToUTF16(P^);
        end
      else
        ret := ret + CodePointToUTF16(P^);
      Inc(P);
    end;
  result := ret;
end;

{$IFDEF UNICODE}
function MecsHTMLEncode(const s: UnicodeString): UnicodeString;
{$ELSE}
function MecsHTMLEncode(const s: WideString): WideString;
{$ENDIF}
var
  U4: UCS4String;
  i: Integer;
  ret: string;
begin
  ret := '';
  if s = '' then
    Exit;
  U4 := UTF16ToUTF32(s);
  for i:=0 to Length(U4)-2 do
    case U4[i] of
      $00000022: // "
        ret := ret + '&quot;';
      $00000026: // &
        ret := ret + '&amp;';
      $0000003C: // <
        ret := ret + '&lt;';
      $0000003E: // >
        ret := ret + '&gt;';
    else
      if U4[i] < $00010000 then
        ret := ret + CodePointToUTF16(U4[i])    // BMP
      else
        ret := ret + Format('&#x%x;', [U4[i]]);
    end;
  result := ret;
end;

initialization
  DefaultAnsiCodePage := GetACP;
  DefaultOEMCodePage  := GetOEMCP;
  DefaultLCID         := GetUserDefaultLCID;
end.


