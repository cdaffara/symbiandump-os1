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
class TRgb;

class MWsGraphicDrawerContext
	{
public:
	virtual void Destroy() = 0;
	virtual void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow) = 0;
	virtual void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop) = 0;
	};

class CWsGraphicDrawerNgaContext : public CBase, public MWsGraphicDrawerContext
	{
public:
	static MWsGraphicDrawerContext* NewL();
public:
	void Destroy();
	void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow);
	void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop);
	};

class CWsGraphicDrawerNonNgaContext : public CBase, public MWsGraphicDrawerContext
	{
public:
	static MWsGraphicDrawerContext* NewL();
public:
	void Destroy();
	void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow);
	void ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop);
	};

#endif // __WSGRAPHICDRAWERCONTEXT_H__
