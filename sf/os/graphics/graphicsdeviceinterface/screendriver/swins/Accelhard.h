// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ACCELHARD_H__
#define __ACCELHARD_H__

#include "GraphicsAccelerator.h"

/**
@internalComponent
*/
enum TGraphicsAcceleratorPanic
	{
	EGraphicsAcceleratorPanicInvalidOperationBuffer,
	EGraphicsAcceleratorAttemptedDrawOutsideBitmap,
	};

/**
@internalComponent
*/
GLREF_C void GraphicsAcceleratorPanic(TGraphicsAcceleratorPanic aPanicCode);

/**
@internalComponent
*/
class CHardwareGraphicsAcceleratorBase : public CHardwareGraphicsAccelerator
	{
protected:
	void ConstructL(RHardwareBitmap aBitmap);
public:
	// From CGraphicsAccelerator
	virtual const TGraphicsAcceleratorCaps* Capabilities();
	virtual TInt Operation(const TGraphicsOperation& aOperation);
	virtual TInt Operation(const TGraphicsOperation& aOperation,TInt aNumClipRects,TRect* aClipRects);
	virtual TInt Operation(TDes8& aBuffer);
	virtual TInt Operation(TDes8& aBuffer,TInt aNumClipRects,TRect* aClipRects);
	// From CGraphicsAccelerator
	virtual void Reserved_1();
	virtual void Reserved_2();
	virtual void Reserved_3();
	virtual void Reserved_4();
	
protected:
	// Generic pure virtual operations
	virtual void DoFillRect(const TRect& aRect,TUint32 aFillValue) = 0;
    virtual TInt DoFilledRectWithPattern(const TRect& aRect,TGopFillPattern aPattern) = 0;
    virtual void DoInvertRect(const TRect& aRect) = 0;
	virtual TInt DoBitBlt(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect) = 0;
    virtual TInt DoBitBltMasked(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aMaskSpec) = 0;
	virtual void DoFadeRect(const TRect& aRect,const TGopFadeParams aFade) = 0;
 	virtual TInt DoBitBltAlphaBitmap(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aAlphaBitmapSpec) = 0;
	virtual TInt DoAlphaBlendTwoBitmaps(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSrceBitmapSpec1,const TAcceleratedBitmapSpec& aSrceBitmapSpec2,const TRect& aSourceRect,const TPoint& aSrcPt2,const TAcceleratedBitmapSpec& aAlphaBitmapSpec,const TPoint& aAlphaPt) = 0;
	virtual TInt DoAlphaBlendOneBitmap(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSrceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aAlphaBitmapSpec,const TPoint& aAlphaPt) = 0;
	//

	// New pure virtual
	virtual TUint32 FillValue(TRgb aColor) = 0; // Return a TUint32 value which has all pixels set to the given color
	virtual void DoUpdateScreen(const TRect& aRect) = 0;
	inline void UpdateScreen(const TRect& aRect);
protected:
	TGraphicsAcceleratorCaps const*	iCaps;
	TAcceleratedBitmapInfo			iBitmapInfo;
	TBool							iIsScreen;
	};

inline void CHardwareGraphicsAcceleratorBase::UpdateScreen(const TRect& aRect)
	{
	if(iIsScreen)
		DoUpdateScreen(aRect);
	}

/**
@internalComponent
*/
class CColor256HardwareGraphicsAccelerator : public CHardwareGraphicsAcceleratorBase
	{
public:
	static CColor256HardwareGraphicsAccelerator* NewL(RHardwareBitmap aBitmap);
protected:
	void ConstructL(RHardwareBitmap aBitmap);
	// From CHardwareGraphicsAcceleratorBase
	virtual void DoFillRect(const TRect& aRect,TUint32 aFillValue);
	virtual TInt DoFilledRectWithPattern(const TRect& aRect,TGopFillPattern aPattern);
	virtual void DoInvertRect(const TRect& aRect);
	virtual TInt DoBitBlt(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect);
	virtual TInt DoBitBltMasked(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aMaskSpec);
	virtual void DoFadeRect(const TRect& aRect,const TGopFadeParams aFade);
	virtual TInt DoBitBltAlphaBitmap(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSourceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aAlphaBitmapSpec);
	virtual TInt DoAlphaBlendTwoBitmaps(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSrceBitmapSpec1,const TAcceleratedBitmapSpec& aSrceBitmapSpec2,const TRect& aSourceRect,const TPoint& aSrcPt2,const TAcceleratedBitmapSpec& aAlphaBitmapSpec,const TPoint& aAlphaPt);
	virtual TInt DoAlphaBlendOneBitmap(const TPoint& aDestination,const TAcceleratedBitmapSpec& aSrceBitmapSpec,const TRect& aSourceRect,const TAcceleratedBitmapSpec& aAlphaBitmapSpec,const TPoint& aAlphaPt);
	virtual TUint32 FillValue(TRgb aColor);
	virtual void DoUpdateScreen(const TRect& aRect);
private:
    TInt iScreenNo;
	};

#endif
