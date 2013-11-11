// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PDRBODY_H
#define PDRBODY_H

#include <e32std.h>
#include <s32file.h>
#include <pdrstore.h>

/**
@internalComponent
*/
const TInt KCInfoFontUidVal = 268435524;

/**
pdr store files are backwardly compatible to this value
@internalComponent
*/
const TInt KPdrtranVersion = 22;  

/**
50 points
@internalComponent
*/
const TInt KScalableWidthTableHeightInTwips = 1000;

/**
@internalComponent
*/
const TInt KMaxCommandStringMaxLength=0x100;

/**
@internalComponent
*/
enum TStyleIndex
	{
	EStyleNormal,
	EStyleBold,
	EStyleItalic,
	EStyleBoldItalic
	};

class TPdrResource
/**
@internalComponent
*/
	{
public:
	TPdrResource();
	void InternalizeL(RReadStream& aStream);
public:
	TInt iId;
	TCommandString iString;
	};

NONSHARABLE_CLASS(CPdrTranslation) : public CBase
/**
@internalComponent
*/
	{
public:
	CPdrTranslation();
	~CPdrTranslation();
	void InternalizeL(RReadStream& aStream);
public:
	TInt iFrom;
	HBufC8* iTo;
	};

NONSHARABLE_CLASS(CPdrTranslates) : public CBase
/**
@internalComponent
*/
	{
public:
	CPdrTranslates();
	void InternalizeL(RReadStream& aStream);
	~CPdrTranslates();
	HBufC8* TranslateStringL(const TDesC& aString) const;
public:
	TStreamId iStreamId;
	TInt iNumTranslations;
	CPdrTranslation** iTranslationList;
	};

NONSHARABLE_CLASS(CWidthsCodeSection) : public CBase
/**
@internalComponent
*/
	{
public:
	CWidthsCodeSection();
	void InternalizeL(RReadStream& aStream);
	~CWidthsCodeSection();
public:
	TCodeSection iCodeSection;
	TInt iNumWidths;
	TUint16* iWidthList;
	};

NONSHARABLE_CLASS(CFontInfo)	: public CBase
/**
@internalComponent
*/
	{
public:
	CFontInfo(TStreamId aStreamId);
	void InternalizeL(RReadStream &aStream);
	~CFontInfo();
	TInt CharWidthInPixels(TChar aChar) const;
	TInt NumCodeSections() const;
	TCodeSection CodeSection(TInt anIndex) const;
public:
	TStreamId iStreamId;
	TUint16 iAscentInPixels;
	TUint16 iMaxCharWidthInPixels;
	TUint16 iMaxNormalCharWidthInPixels;
	TInt iNumCodeSections;
	CWidthsCodeSection** iCodeSectionList;
	};

class TPdrStyle
/**
@internalComponent
*/
	{
public:
	TPdrStyle();
	void InternalizeL(RReadStream &aStream);
public:
	TBool iIsAvailable;
	TStreamId iFontInfoStreamId;
	};

class TPdrFontHeight
/**
@internalComponent
*/
	{
public:
	 TPdrFontHeight();
	 void InternalizeL(RReadStream& aStream);
public:
	TCommandString iCommandString;
	TInt iHeightInTwips;
	TInt iWidthScale;
	TPdrStyle iStyle[EStyleBoldItalic + 1];  // N.B. Don't use enums TFontPosture and TFontStrokeWeight as indices
	};

class TPdrScalableFontHeight
/**
@internalComponent
*/
	{
public:
	TPdrScalableFontHeight();
	void InternalizeL(RReadStream& aStream);
public:
	TCommandString iCommandString;
	TInt iHeightMinInTwips;
	TInt iHeightMaxInTwips;
	TInt iHeightDeltaInTwips;
	TPdrStyle iStyle[EStyleBoldItalic + 1];  // N.B. Don't use enums TFontPosture and TFontStrokeWeight as indices
	};

NONSHARABLE_CLASS(CTypefaceFonts) : public CBase
/**
@internalComponent
*/
	{
public:
	CTypefaceFonts();
	void InternalizeL(RReadStream& aStream);
	~CTypefaceFonts();
	TInt IsScalable() const;
	TInt NumFontHeights() const;
	TInt FontHeightInTwips(TInt aHeightIndex) const;
	TInt FontInfoHeightInTwips(TInt aHeightIndex) const;
	void CommandString(TDes8& aDes, TInt aHeightIndex) const;
	TPdrStyle* Style(TInt aHeightIndex, TFontStyle& aFontStyle) const;
	TTypeface Typeface();
private:
	TStyleIndex StyleIndex(TFontStyle& aFontStyle) const;
private:
	TTypeface iTypeface;
	TInt iNumFontHeights;  // Also used as flag	for !iIsScalable
	TPdrFontHeight* iFontHeightList;
	TPdrScalableFontHeight* iScalableFontHeight;
public:
	TSwizzle<CPdrTranslates> iTranslates;
	};

#endif
