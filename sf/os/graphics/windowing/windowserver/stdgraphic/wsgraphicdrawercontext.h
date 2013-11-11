// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __WSGRAPHICDRAWERCONTEXT_H__
#define __WSGRAPHICDRAWERCONTEXT_H__

#include <e32base.h>
#include <e32std.h>

class MWsGc;
class CFbsBitmap;
class CFrameRate;
class MWsClient;
class MWsScreen;

class MWsGraphicDrawerContext
	{
public:
	virtual void Destroy() = 0;
	virtual MWsClient& Client(MWsGc& aGc) = 0;
	virtual MWsScreen& Screen(MWsGc& aGc) = 0;
	virtual const TTime& Now(MWsGc& aGc) const = 0;
	virtual void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow) = 0;
	virtual void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop) = 0;
	virtual void SetGcOrigin(MWsGc& aGc, const TPoint& aOrigin) = 0;
	virtual TInt Push(MWsGc& aGc) const = 0;
	virtual void Pop(MWsGc& aGc) const = 0;
	virtual void DrawBitmap(MWsGc& aGc, const TPoint& aDestPos, const CFbsBitmap* aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap) const = 0;
	virtual void DrawFrameRate(MWsGc& aGc, const TRect& aRect, const CFrameRate& aFps) const = 0;
	};

class CWsGraphicDrawerNgaContext : public CBase, public MWsGraphicDrawerContext
	{
public:
	static MWsGraphicDrawerContext* NewL();
public:
	void Destroy();
	MWsClient& Client(MWsGc& aGc);
	MWsScreen& Screen(MWsGc& aGc);
	const TTime& Now(MWsGc& aGc) const;
	void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow);
	void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop);
	void SetGcOrigin(MWsGc& aGc, const TPoint& aOrigin);
	TInt Push(MWsGc& aGc) const;
	void Pop(MWsGc& aGc) const;
	void DrawBitmap(MWsGc& aGc, const TPoint& aDestPos, const CFbsBitmap* aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap) const;
	void DrawFrameRate(MWsGc& aGc, const TRect& aRect, const CFrameRate& aFps) const;
	};

class CWsGraphicDrawerNonNgaContext : public CBase, public MWsGraphicDrawerContext
	{
public:
	static MWsGraphicDrawerContext* NewL();
public:
	void Destroy();
	MWsClient& Client(MWsGc& aGc);
	MWsScreen& Screen(MWsGc& aGc);
	const TTime& Now(MWsGc& aGc) const;
	void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow);
	void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop);
	void SetGcOrigin(MWsGc& aGc, const TPoint& aOrigin);
	TInt Push(MWsGc& aGc) const;
	void Pop(MWsGc& aGc) const;
	void DrawBitmap(MWsGc& aGc, const TPoint& aDestPos, const CFbsBitmap* aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap) const;
	void DrawFrameRate(MWsGc& aGc, const TRect& aRect, const CFrameRate& aFps) const;
	};

#endif // __WSGRAPHICDRAWERCONTEXT_H__
