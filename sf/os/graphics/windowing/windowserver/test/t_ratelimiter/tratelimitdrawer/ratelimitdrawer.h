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
 @internalComponent - Internal Symbian test code
*/

#ifndef RATELIMITDRAWER_H_
#define RATELIMITDRAWER_H_

#include <e32std.h>
#include "graphics/WSGRAPHICDRAWER.H"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "../inc/constants.h"

/**
Drawer plug-in class used to draw the animation and scheduling.
*/
NONSHARABLE_CLASS(CWsRateLimitGraphicDrawer): public CWsGraphicDrawer
	{
public:
	enum {EImplUid = 0x102869F6};
	
public:
	static CWsRateLimitGraphicDrawer* NewL();
	~CWsRateLimitGraphicDrawer();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData);
	void HandleMessage(const TDesC8& aData);
private:
	void DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const;
	void InitialiseMembers();
private:
	CWsRateLimitGraphicDrawer();
	
private:
	TRgb iColors[KMaxFrames];
	mutable TReal iFrameRate;
	mutable TInt iFrame;
	mutable TTime iStartTime;
	};

#endif /*RATELIMITDRAWER_H_*/
