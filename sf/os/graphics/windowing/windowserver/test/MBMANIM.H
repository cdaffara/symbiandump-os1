// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Demonstration handwriting class
// 
//

#if !defined(__MBMANIM_H__)
#define __MBMANIM_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__FBS_H__)
#include <fbs.h>
#endif
#if !defined(__W32ADLL_H__)
#include <w32adll.h>
#endif


class CAnimateMbmAnimDll : public CAnimDll 
	{
public:
	CAnim *CreateInstanceL(TInt aType);
	};

class MAnimTimer
	{
public:
	virtual void Animate()=0;
	};

class CAnimTimer : public CTimer
	{
public:
	inline CAnimTimer(MAnimTimer* aAnimator) :CTimer(0), iAnimator(aAnimator) {}
	void ConstructL();
	//pure virtual functions from CActive
	void DoCancel();
	void RunL();
private:
	MAnimTimer* iAnimator;
	};

class CAnimateMbm : public CFreeTimerWindowAnim, MAnimTimer
	{
public:
	~CAnimateMbm();
	//pure virtual functions from MAnimTimer
	void Animate();
	//pure virtual functions from MEventHandler
	TBool OfferRawEvent(const TRawEvent &aRawEvent);
	//pure virtual functions from CAnim
	void ConstructL(TAny *aArgs, TBool aHasFocus);
	void Animate(TDateTime *aDateTime);
	void Redraw();
	void Command(TInt aOpcode, TAny *aArgs);
	TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	void FocusChanged(TBool aState);
private:
	inline TInt LoadBitmap() {return LoadBitmap(iIndex);}
	inline TInt LoadBitmap(TInt aIndex) {return iBitmap->Load(iName,aIndex);}
private:
	CAnimTimer* iTimer;
	CFbsBitmap* iBitmap;
	TBuf<32> iName;
	TInt iIndex;
	TBool iRunning;
	TTimeIntervalMicroSeconds32 iInterval;
	};

#endif
