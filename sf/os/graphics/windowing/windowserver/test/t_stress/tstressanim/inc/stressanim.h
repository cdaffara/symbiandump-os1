// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/


#if !defined(__STRESSTESTANIM_H__)
#define __STRESSTESTANIM_H__

#include <e32std.h>
#include <w32std.h>
#include <w32adll.h>
#include <bitstd.h>


class CStressWindowAnim : public CWindowAnim
	{
	enum {ENumPolyPoints=9};
public:
	~CStressWindowAnim();
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Redraw();
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	void SetPolyList(const TRect &aRect);
	void DrawPolyLine();
	void DrawBitmap();
	void DrawText();
	void TweakPolyList(TInt aState);
	void FocusChanged(TBool aState);
	void InvalidateText();
	void InvalidateBitmap();
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	void AppendTime(TDes& aTimeText,const TTime& aTime) const;
private:
	TPoint iPos;
	TSize iSize;
	TInt iColor;
	TBool iMasked;
	TRect iPolyRect;
	TInt iPolyState;
	TInt iWiggleSize;
	TPoint iTextPos;
	TBool iHasFocus;
	CArrayFixFlat<TPoint> *iPolyList;
	CFbsBitmap iBitmap1;
	CFbsBitmap iBitmap2;
	CFbsBitmap iMask;
	CFbsFont *iFont;
	
	TUint32	iLastAnimShot;
	TBool iDoAnimation;
	TInt iKernelTicksPeriod;
	};
	
class CStressAnimDll : public CAnimDll 
	{
public:
	CAnim *CreateInstanceL(TInt aType);
private:
	};
	

#endif // __STRESSTESTANIM_H__
