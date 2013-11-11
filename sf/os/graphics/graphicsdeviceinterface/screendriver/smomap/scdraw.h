// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This module define the class and constants used for the different display
// mode supported.
// Include files                                                   
// 
//

/**
 @file
*/
/********************************************************************/
#if !defined__SCDRAW_H__
#define __SCDRAW_H__

#if !defined(__E32SVR_H__)
    #include <e32svr.h>
#endif /* __E32SVR_H__ */

#include "BMDRAW.H"

/**
 *  Define the palette constants size
 * 
 */ 
const TInt		KEightBppPaletteEntries		= 256 ;

/*
 *  Define the constants of the Bpp mode to set in the 
 *  palette entry 0
 *
 */
const TUint16	KEightBppPixelBitSize		= 3 << 12 ;
const TUint16	KSixteenBppPixelBitSize		= 4 << 12 ;	// In active mode (TFT)

/**
 *  Define the CDrawEightBppScreenBitmapColor class
 *  8 bpp Color
 */
NONSHARABLE_CLASS(CDrawEightBppScreenBitmapColor) : public CDrawEightBppBitmapColor
	{
public:
	virtual TInt  InitScreen() ;
	virtual TInt  ConstructScreenL(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize) ;
	virtual void  OrientationsAvailable(TBool aOrientation[4]) ;
	virtual TBool SetOrientation(TOrientation aOrientation) ;
    TInt SetCustomPalette(const CPalette* aPalette);
    virtual void SetDisplayMode(CFbsDrawDevice*);
   	virtual TInt HorzTwipsPerThousandPixels() const;
	virtual TInt VertTwipsPerThousandPixels() const;
protected:
	virtual void SetSize(const TSize& aSize);	
	TUint16* iPaletteAddress ;
    TInt iScreenNo;
	};

/**
 *  Define the CDrawSixteenBppScreenBitmap class
 *  16 bpp Color
 *
 */
class CDrawSixteenBppScreenBitmap : public CDrawSixteenBppBitmap
	{
public:
	virtual TInt  InitScreen() ;
	virtual TInt  ConstructScreenL(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize) ;
	virtual void  OrientationsAvailable(TBool aOrientation[4]) ;
	virtual TBool SetOrientation(TOrientation aOrientation) ;
    virtual void SetDisplayMode(CFbsDrawDevice*);
    virtual TInt HorzTwipsPerThousandPixels() const;
	virtual TInt VertTwipsPerThousandPixels() const;
protected:
	virtual void SetSize(const TSize& aSize);	
    TInt iScreenNo;
	};


/**
 *  Define the CDrawUTwentyFourBppScreenBitmap class
 *  24 bpp Color
 *
 */
class CDrawUTwentyFourBppScreenBitmap : public CDrawUTwentyFourBppBitmap
	{
public:
	virtual TInt  InitScreen() ;
	virtual TInt  ConstructScreenL(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize) ;
	virtual void  OrientationsAvailable(TBool aOrientation[4]) ;
	virtual TBool SetOrientation(TOrientation aOrientation) ;
    virtual void SetDisplayMode(CFbsDrawDevice*);
    virtual TInt HorzTwipsPerThousandPixels() const;
	virtual TInt VertTwipsPerThousandPixels() const;
protected:
	virtual void SetSize(const TSize& aSize);	
	TUint16* iPaletteAddress ;
    TInt iScreenNo;	
    };

/**
 *  Define the CDrawThirtyTwoBppScreenBitmapAlpha class
 *  32 bpp Color
 *
 */
class CDrawThirtyTwoBppScreenBitmapAlpha : public CDrawThirtyTwoBppBitmapAlpha
	{
public:
	virtual TInt  InitScreen() ;
	virtual TInt  ConstructScreenL(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize) ;
	virtual void  OrientationsAvailable(TBool aOrientation[4]) ;
	virtual TBool SetOrientation(TOrientation aOrientation) ;
    virtual void SetDisplayMode(CFbsDrawDevice*);
    virtual TInt HorzTwipsPerThousandPixels() const;
	virtual TInt VertTwipsPerThousandPixels() const;
protected:
	virtual void SetSize(const TSize& aSize);	
	TUint16* iPaletteAddress ;
    TInt iScreenNo;
    };

/**
 *  Define the CDrawThirtyTwoBppScreenBitmapAlphaPM class
 *  32 bpp Color
 *
 */
class CDrawThirtyTwoBppScreenBitmapAlphaPM : public CDrawThirtyTwoBppBitmapAlphaPM
	{
public:
	virtual TInt  InitScreen() ;
	virtual TInt  ConstructScreenL(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize) ;
	virtual void  OrientationsAvailable(TBool aOrientation[4]) ;
	virtual TBool SetOrientation(TOrientation aOrientation) ;
    virtual void SetDisplayMode(CFbsDrawDevice*);
    virtual TInt HorzTwipsPerThousandPixels() const;
	virtual TInt VertTwipsPerThousandPixels() const;
protected:
	virtual void SetSize(const TSize& aSize);	
	TUint16* iPaletteAddress ;
    TInt iScreenNo;
    };

#endif /* __SCDRAW_H__ */

