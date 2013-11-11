// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test - Test plug-in to test wsgraphic animation artwork framerate
 @internalComponent - Internal Symbian test code
*/

#ifndef __GRAPHICDRAWERTESTFRAMERATE_H__
#define __GRAPHICDRAWERTESTFRAMERATE_H__

#include <e32std.h>
#include "graphics/WSGRAPHICDRAWER.H"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>

//Maximum number of animations at a time.
#define KMaxAnim 100

class MWsGraphicDrawerContext;

/**
Drawer plug-in class used to draw the animation and scheduling.
*/
NONSHARABLE_CLASS(CGraphicDrawerTestFrameRate): public CWsGraphicDrawer
	{
public:
	enum 
		{
		EImplUid = 0x1028292A
		};
	static CGraphicDrawerTestFrameRate* CreateL();
	~CGraphicDrawerTestFrameRate();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData);
	void HandleMessage(const TDesC8& aData);
private:
	void DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const;
private:
	CGraphicDrawerTestFrameRate();
	mutable TInt iCounter[KMaxAnim];
	MWsGraphicDrawerContext* iContext;
	};

#endif //__GRAPHICDRAWERTESTFRAMERATE_H__
