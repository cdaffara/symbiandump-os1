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

#ifndef ANIMATED_H
#define ANIMATED_H

#include <w32std.h>
#include "compwin.h"
#include "test_step_conf.h"

_LIT8(KAnimatedWindowType, "Animated");

class CAnimatedWin : public CCompWin
	{
public:
	static CAnimatedWin* NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	static void LoadConfiguration(const MTestStepConfigurationContext* aContext);
	static TBool IsEnabled() { return iEnabled; }

	~CAnimatedWin();
	
	// from CCompWin
	void Redraw(const TRect& aRect);
	void DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	void SetSize(const TSize & aSize);
	void SetPos(const TPoint & aPos);
	virtual const TDesC8& TypeName() { return KAnimatedWindowType; }
	void DumpDetails(RFile & aFile, TInt aDepth);
	
protected:
	CAnimatedWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	void ConstructL();
	
	// from CCompWin
	TBool QueryReadyForVerification();

private:
	void SetAnimPos(const TPoint& aPos);
	void AppendTime(TDes& aTimeText,const TTime& aTime) const;
	
	static TBool iEnabled;
	static TBool iTransparent;
	static TBool iTransparentForegroundWindow;
	
private:
	class RStressAnim : public RAnim
	{ 
	public:
		RStressAnim (RAnimDll& aAnimDll);
		TInt Construct(const RWindowBase &aDevice, const TDesC8 &aParams);
		
		TInt RequestAnimThis(TInt aOpcode);
		TInt RequestAnimThis(TInt aOpcode, const TDesC8 &aParams);
			
		friend class CAnimatedWin;
	};
	
	enum TValues
	{
	ENofFrames = 2,
	EFrame1	   = 0,
	EFrame2	   = 1,
	EFrameSzX  = 64,
	EFrameSzY  = 64,
	EFrameSzXHalf = (EFrameSzX / 2),
	EFrameSzYHalf = (EFrameSzY / 2),
	EFrameSzXQtr = (EFrameSzX / 4),
	EFrameSzYQtr = (EFrameSzY / 4),
	EFrameSzXEgt = (EFrameSzX / 8),
	EFrameSzYEgt = (EFrameSzY / 8),
	EFrameDurMult = 100000,
	EFrameDurMaxTenthSec = 15,
	EWatchUrgentSync	= 100000,   	// 	1/10 sec
	EWatchLazySync		= 800000,	    // 	8/10 sec
	EWatchSafeGap		= 100000,
	EWatchMatchGap		= 900000
	};

	RAnimDll				iAnimDll;
	RStressAnim*			iAnim;

	CFbsBitmap* 			iAnimContent [ENofFrames];
	CFbsBitmap* 			iAnimMask;

	CFbsFont*				iAnimFont; 				
	
	CFbsBitmapDevice*		iAnimCntDevice [ENofFrames];
	CFbsBitmapDevice*		iAnimMskDevice;
	
	TBool					iConstructed;
	
	TRgb					iBgColor;

	TTimeIntervalMicroSeconds32
							iFrameDuration;
	
	TUint32 				iAnimStartup,
							iAnimLastKnownAction;
	TInt 					iKernelTicksPeriod;
	
	RBlankWindow*  			iForeWin; //transparent foreground window
	};


#endif // ANIMATED_H
