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

#ifndef __DEBUGBAR_H__
#define __DEBUGBAR_H__

#include <e32base.h>
#include <fbs.h>

class CScreen;

class CDebugBar : public CBase
	{
	public:
		static CDebugBar* NewL(CScreen * aScreen,const TRect& aScreenRect,TTimeIntervalMicroSeconds32 aRefreshInterval);
		const TRect& Rect() const;
		~CDebugBar();
		void OnKeyEvent();
		void RedrawDebugBar() const;
	private:
		CDebugBar(CScreen * aScreen,const TRect& aScreenRect);
		void ConstructL(TTimeIntervalMicroSeconds32 aRefreshInterval);
		static TBool RedrawDebugBarCallback(TAny* aAny);
	private:
		CPeriodic* iTimer;
		CScreen* iScreen;
		CFbsFont* iFont;
		TRect iScreenRect;
		TInt iBaseline;
		mutable TBuf<128> iBuf;
		TInt iKeyEvents;
		TInt iPolls;
		mutable TInt64 iLastTotal;
		mutable TInt64 iLastIdle;
		mutable TInt64 iLastDrawing;
		mutable TInt64 iLastReclaimedIdleTime;
	};

#endif
