// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header containing the structs TTwoTInt and TDrawTextInContextInternal which are included into gdi.h
//

#ifndef GDISTRUCTS_H
#define GDISTRUCTS_H

/*
Structure used as a package for GetUnderlineMetrics to pass to the relevant
section of APIExtension.
@internalTechnology
*/
struct TTwoTInt
{
	TInt iTop;
	TInt iBottom;
};
	
/*
Structure used as a package for DrawText to pass to relevant section of 
APIExtension
@internalTechnology
*/
struct TDrawTextInContextInternal
	{
	CGraphicsContext::TTextParameters iParam;
	TPtrC iText;
	TPoint iPosition;
	TRect iBox;
	TInt iBaselineOffset;
	CGraphicsContext::TTextAlign iAlign;
	TInt iMargin;
	TInt iTextWidth;
	TBool iUp;	// For vertical drawing
	};

/*
Structure used as a package for TextWidthInPixels to pass to the relevant
section of APIExtension
@internalTechnology
*/
struct TTextWidthInternal
	{
	CFont::TMeasureTextInput iParam;
	TPtrC iText;
	};


/*
Structure used to pass input/output parameters between RFontTable and CFbsFont.
@internalTechnology
*/
class TGetFontTableParam 
    {
public:
    TUint32 iTag;
    TAny *iContent;
    TInt iLength;
    };

/*
Structure used to pass input/output parameters between RGlyphOutlineIterator and CFbsFont.
Used when opening an outline iterator.
@internalTechnology
*/
class TGetGlyphOutlineParam 
    {
public:
    TInt iCount;
    const TUint *iCodes; 
    TBool iHinted;
    TAny **iOutlines;
    TInt *iLengths; 
    };

/*
Structure used to pass input/output parameters between RGlyphOutlineIterator and CFbsFont.
Used when closing an outline iterator.
@internalTechnology
*/
class TReleaseGlyphOutlineParam 
    {
public:
    TInt iCount;
    const TUint *iCodes;
    TBool iHinted;
    };

#endif /* GDISTRUCTS_H */
