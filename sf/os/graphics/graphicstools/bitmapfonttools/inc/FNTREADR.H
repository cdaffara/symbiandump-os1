/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Header FNTREADR.H
*
*/


#ifndef __FNTREADR_H__
#define __FNTREADR_H__

#include "FNTRECRD.H"
#include "LST.H"
#include "READER.H"
/**
@internalComponent
*/
const uint16 KMaxSizeCodeSectionBitmap = 0x7FFF;
const int KMaxBitmapWidth = 0x7f;
const int KMaxBitmapHeight = 0x7f;
const int KNumberOfBitsInByte = 8; 
/**
@internalComponent
*/
const int KLowestPermittedCharacterEncoding = 0x0000;
const int KHighestPermittedCharacterEncoding = 0xffff;
/**
@internalComponent
*/
const int KMaxNumberRepeatedLines = 15;	// maximum number of repeated lines in font compiler

/**
KFillCharacterOffset is a significant offset that is set when a character within a code section is 
not in the font. This means that for these fill characters nothing is stored within the binary
data part of the code section
@internalComponent
*/
const uint16 KFillCharacterOffset = 0x7FFF;

/**
Resource identifiers
@internalComponent
*/
const String IdentFontBitmap("FontBitmap");
const String IdentSTARTFONT("STARTFONT");
const String IdentTypeface("Typeface");
const String IdentFontStoreFile("FontStoreFile");
/**
Char identifiers
@internalComponent
*/
const String IdentAdjust("Adjust");
const String IdentEndChar("EndChar");

/**
Codesection identifiers
@internalComponent
*/
const String IdentEndCodeSection("EndCodeSection");
const String IdentChar("Char");

/**
Fontbitmap identifiers
@internalComponent
*/
const String IdentEndFontBitmap("EndFontBitmap");
const String IdentUid("Uid");
const String IdentBold("Bold");
const String IdentItalic("Italic");
const String IdentFontBitmapProportional("Proportional");
const String IdentCellHeight("CellHeight");
const String IdentAscent("Ascent");
const String IdentMaxNormalCharWidth("MaxNormalCharWidth");
const String IdentBitmapEncoding("BitmapEncoding");
const String IdentCodeSection("CodeSection");
const String IdentMaxConsecutiveFillChars("MaxConsecutiveFillChars"); 

/**
BDF CHAR identifiers
@internalComponent
*/
const String IdentBDFFileHeader("STARTFONT");
const String IdentBDFComment("COMMENT");
const String IdentBDFContentVersion("CONTENTVERSION");
const String IdentBDFFontBitmap("FONT"); 
const String IdentBDFPointSize("SIZE");
const String IdentBDFFontDesignBox("FONTBOUNDINGBOX");
const String IdentBDFWritingDirection("METRICSSET");
const String IdentBDFStartProperties("STARTPROPERTIES");
const String IdentBDFPropertyUid("UID");
const String IdentBDFPropertyBold("BOLD");
const String IdentBDFPropertyItalic("ITALIC");
const String IdentBDFPropertyFontAscent("FONTASCENT");
const String IdentBDFPropertyFontDescent("FONTDESCENT");
const String IdentBDFPropertyMaxNormalCharWidth("MAXNORMALCHARWIDTH");
const String IdentBDFPropertyMaxConsecutiveFillChars("MAXCONSECUTIVEFILLCHARS");
const String IdentBDFEndProperties("ENDPROPERTIES");
const String IdentBDFNumChars("CHARS");	// To be used as sanity check on BDF files
const String IdentBDFCharLabel("STARTCHAR");
const String IdentBDFChar("ENCODING");
const String IdentBDFCursorMove("DWIDTH");
const String IdentBDFBitmapSizeAndDisplacement("BBX");
const String IdentBDFStartBitmap("BITMAP");
const String IdentBDFEndChar("ENDCHAR");
const String IdentBDFEndFontBitmap("ENDFONT");

/**
Typeface identifiers
@internalComponent
*/
const String IdentEndTypeface("EndTypeface");
const String IdentName("Name");
const String IdentTypefaceProportional("Proportional");
const String IdentSerif("Serif");
const String IdentSymbol("Symbol");
const String IdentFontBitmaps("FontBitmaps");
const String IdentWidthFactor("WidthFactor");
const String IdentHeightFactor("HeightFactor");
const String IdentEndFontBitmaps("EndFontBitmaps");

/**
Fontstorefile identifiers
@internalComponent
*/
const String IdentEndFontStoreFile("EndFontStoreFile");
const String IdentFilename("Filename");
const String IdentCollectionUid("CollectionUid");
const String IdentKPixelAspectRatio("KPixelAspectRatio");
const String IdentTypefaces("Typefaces");
const String IdentEndTypefaces("EndTypefaces");
const String IdentExtraFontBitmaps("FontBitmaps");
const String IdentEndExtraFontBitmaps("EndFontBitmaps");
const String IdentCopyrightInfo("CopyrightInfo");
const String IdentEndCopyrightInfo("EndCopyrightInfo");

class CroppedValues;

class FontReader : public Reader
/**
@internalComponent
*/
	{
public:
	enum TReadFileFormat
		{
		ESymbianGDFFormat,
		EBDFFormat
		};
public:
	FontReader();
	boolean Read(const String& aFilename);
	boolean ReadBDFCharacter(int aCode);
	boolean ParseMetricsFromBDF(int aNumberCharsInFile, int aMaxConsecutiveFillChars);
	boolean ReadMetricFromBDFCharacter(CharacterMetrics* aMetric, CroppedValues* aCropped);
	boolean ReadBDFFontBitmap();
	boolean ReadTypeface();
	boolean ReadFontStoreFile();
	int Store(const String& aFilename);
	boolean CharLine(String& aCharLine);
private:
	boolean ReadBDFChars(const int aNumberOfGlyphsInFile, const int aMaxConsecutiveFillChars);
	boolean DoMetricAnalysis(int& aHeight, int& aWidth, int& aLeftAdjust, int& aRightAdjust, int& aAscent);
	void ErrorIdentifierExpected(const String& aIdentifier);
	boolean CompareBitmapLines(int aLine1, int aLine2);
	boolean BitmapLineEmpty(int aLine);
	boolean BitmapColumnEmpty(int aColumn);
	void WriteFillCharacters(int aNumberConsecutive);
	void PrintoutCodeSection(const BitmapCodeSection* aCodeSection) const;
private:
	FontStore iFontStore;
	FontStoreFile* iFontStoreFile;
	CharacterMetrics* iCharacterMetrics;
	BitmapCodeSection* iCodeSection;
	FontBitmap* iFontBitmap;
	FntTypeface* iTypeface;
	TReadFileFormat iReadFileFormat;
	int iBitArray[KMaxBitmapWidth][KMaxBitmapHeight];
	int iBitmapWidth;
	int iBitmapHeight;
	int iDefaultXMoveInPixels;
	int iDefaultYMoveInPixels;
	String iFileName;
	};

#endif
