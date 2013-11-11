// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined(__FONTCOMP_H__)
#define __FONTCOMP_H__
#include <stdlib.h>
#include <string.h>
#include <io.h>

#ifdef __MSVCDOTNET__
#include <fstream>
#include <iostream>
using namespace std;
#else //!__MSVCDOTNET__
#include <fstream.h>
#endif //__MSVCDOTNET__

/**
@internalComponent
*/
#define MAX_CHARS 256
#define FONT_NAME_LEN 16
/**
@internalComponent
*/
#define MAX_CHAR_WID 256
#define MAX_HEIGHT 256
#define MAX_LABEL_LENGTH 16		/* Max font label name */
#define MAX_LEN_IN 255	/* Max input line length */
#define MAXLINE 600	/* Max line length for writing to screen */
#define FONT_MAX_HEADER_LEN 128
#define P_FNAMESIZE 128 /* Maximum file name size */
/**
@internalComponent
*/
enum Errors
	{
	NoError=0,
	NoMemory=-1,
	NoFile=-2,
	FileRead=-3,
	FileWrite=-4,
	FileFormat=-5,
	Parameter=-6
	};
/**
@internalComponent
*/
struct FcmCharHead
	{
	int xOffset;
	int yOffset;
	int width;
	int height;
	int move;
	int ByteWid;
	int offset;
	};
/**
@internalComponent
*/
struct LetterTableData
	{
	short int offset;
	short int width; /* x16 in version 1*/
	};
/**
@internalComponent
*/
struct LetterData
	{
	char blx;
	char bly;
	unsigned char rWid; /* Enclosing rectangle width */
	unsigned char rHgt; /* Enclosing rectangle height */
	};
/**
@internalComponent
*/
struct EffVariousData
	{
	short int version;
	short int blx;
	short int bly;
	short int Wid;
	short int Hgt;
	short int xSize; /* 16 x Point size */
	short int xRes;  /* DPI */
	short int ySize;
	short int yRes;
	unsigned char XHeight;
	unsigned char CapHeight;
	unsigned char Ascender;
	char Descender;
	unsigned char LineSpacing;
	char UnderLinePos;
	unsigned char UnderLineThickness;
	unsigned char filler[7];
	};
/**
@internalComponent
*/
const int EMaxFontNameLength=256;

class Fxf
/**
@internalComponent
*/
	{
public:
	FcmCharHead *chr[MAX_CHARS];
	char name[EMaxFontNameLength]; /* Font name */
	char typeface[EMaxFontNameLength]; /* Font typeface */
	int MaxChrWidth;
	int cell_height;      /* Height of character set */
	int nominal_ascent;   /* Max ascent of normal (ASCII) characters */
	int descent;         /* Descent of characters below base line */
	int chr_seg;
	int FirstChr;
	int n_chars;         /* counts total number of characters defined */
	unsigned int max_info_width;  /* Max char width to put in info file */
	unsigned short int flags;
	unsigned short int special;
	unsigned int ByteWid;
	int UseWords;
	int UlinePos;
	int UlineThickness;
	int iBold;
	int iItalic;
	int iProportional;
	int iSerif;
	int iSymbol;
	unsigned int iUid;
	};
/**
@internalComponent
*/
enum FontType
	{
	EFontTypeFsc,
	EFontTypeEff
	};

class FontCompiler
/**
@internalComponent
*/
	{
public:
	FontCompiler();
	int Read(FontType aInputType);
	int Init(char*,char*,char*);
	void RemoveBlankSpace();
	char* FontStore() const;
	virtual int WriteFont()=0;
protected: // general information
	fstream iInputFile;
	fstream iOutputFile;
	Fxf* iFxf;
protected: // font information
	char* iFontSpace;
	char* iWorkSpace; // misc. stuff
	short int* iMapSpace;
	int iWorkSpaceSize;
	unsigned int iHeaderDataLen;
	unsigned char iHeaderData[FONT_MAX_HEADER_LEN];
	};

class FontRead
/**
@internalComponent
*/
	{
public:
	FontRead(fstream& aFile,FontCompiler& aFontCompiler,Fxf* aFxf);
	virtual int ReadFont()=0;
private:
	FontRead& operator=(const FontRead&);
protected:
	fstream& iInputFile;
	FontCompiler* iFontCompiler;
	Fxf* iFxf;
	};

class EffRead : public FontRead
/**
@internalComponent
*/
	{
public:
	EffRead(fstream& aFile,FontCompiler &aFontCompiler,Fxf* aFxf,short int* aMapSpace);
	virtual int ReadFont();
private:
	EffRead& operator=(const EffRead&);
private:
	short int* iMapSpace;
	};

class FscRead : public FontRead
/**
@internalComponent
*/
	{
public:
	FscRead(fstream& aFile,FontCompiler &aFontCompiler,Fxf* aFxf);
	virtual int ReadFont();
private:
	FscRead& operator=(const FscRead&);
private:
	int ReadLine();
	int Pass1();
	int Pass2();
	char* ScanLine(int& aLen);
	int DoCom(int aSecondPass);
private:
	FcmCharHead *iChar;
	char iInputBuf[256];
	int iInputBufLen;
	char* iFileBuf;
	int iFileBufLen;
	int iFileBufPos;
	int iUnderHang;
	int iOverHang;
	};

#endif
