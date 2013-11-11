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

#include <bitstd.h>
#include <graphics/wsgraphicdrawer.h>
#include "framerate.h"
#include "wsgraphicdrawercontext.h"
#include "wsframerate.h"

//Constant message handle, which is used to send count value between client and plug-in
const TUint8 KCmdCount=0;

/**
Creates new test framerate object.
*/
CGraphicDrawerTestFrameRate* CGraphicDrawerTestFrameRate::CreateL()
	{
	return new(ELeave) CGraphicDrawerTestFrameRate;
	}

CGraphicDrawerTestFrameRate::CGraphicDrawerTestFrameRate()
	{
	}

CGraphicDrawerTestFrameRate::~CGraphicDrawerTestFrameRate()
	{
	}

void CGraphicDrawerTestFrameRate::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv,aId,aOwner);
	if (aEnv.Screen(0)->ResolveObjectInterface(KMWsScreenConfigInterfaceId))
		{
		iContext = CWsGraphicDrawerNonNgaContext::NewL();
		}
	else
		{
		iContext = CWsGraphicDrawerNgaContext::NewL();
		}
	}

/**	
Simulate two animations of different frame rate with their respective schedule.

@param MWsGc Window server graphic context to draw the animation
@param TRect Rectangle are required to draw the animation
@param TDesC Parameter value to use inside this function.
*/
void CGraphicDrawerTestFrameRate::DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const
	{
	TInt animId = aData[0];
	TInt requestedFps = aData[1];
	++iCounter[animId];
	//Schedule next redraw based on requested frame rate
	TTimeIntervalMicroSeconds nextTick = 1000000/requestedFps;
	iContext->ScheduleAnimation(aGc, aRect, nextTick);
	}

/**	
Handles message between client and plug-in.

@param TDesC Constant message command.
*/
void CGraphicDrawerTestFrameRate::HandleMessage(const TDesC8& aData)
	{
	switch (aData[0])
		{
		case KCmdCount:
			TPckgBuf<TAnimRate> buf;
			buf().iAnim1=iCounter[0];
			buf().iAnim2=iCounter[1];
			TInt err = SendMessage(buf);
			__ASSERT_DEBUG(err>=KErrNone, User::Invariant());
			break;
		}
	}
	
