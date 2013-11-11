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
// STDGRAPHICTESTDRAWER.CPP
// 
//

#ifndef __STDGRAPHICDRAWERTEST_H__
#define __STDGRAPHICDRAWERTEST_H__

#include <e32std.h>
#include "graphics/WSGRAPHICDRAWER.H"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>

class MWsGraphicDrawerContext;

NONSHARABLE_CLASS(CFrameRate): public CBase
	{
public:
	TInt CountSamples() const;
	void Sample() const;
	TInt Fps() const;
private:
	static const TInt KMaxSamples = 25;
	mutable TInt iNumSamples;
	mutable TInt64 iSamples[KMaxSamples];
	};

NONSHARABLE_CLASS(CWsGraphicDrawerTestFrameRate): public CWsGraphicDrawer
	{
public:
	enum 
		{
		EImplUid = 0x10281AB2
		};
	static CWsGraphicDrawerTestFrameRate* CreateL();
	~CWsGraphicDrawerTestFrameRate();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData);
	void HandleMessage(const TDesC8& aData);
private:
	void DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const;
private:
	CWsGraphicDrawerTestFrameRate();
	CFrameRate iFps;
	MWsGraphicDrawerContext* iContext;
	};

#endif //__STDGRAPHICDRAWERTEST_H__
