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

#include <bitstd.h>
#include <graphics/wsgraphicdrawer.h>
#include "ratelimitdrawer.h"
#include "wsratelimitdrawer.h"
#include <graphics/wsgraphicscontext.h>

//Constant message handle, which is used to send count value between client and plug-in
const TUint8 KCmdCount=0;
// First Frame
const TInt KFirstFrame = 0;

/**
Creates new test framerate object.
*/
CWsRateLimitGraphicDrawer* CWsRateLimitGraphicDrawer::NewL()
	{
	return new(ELeave) CWsRateLimitGraphicDrawer;
	}

CWsRateLimitGraphicDrawer::CWsRateLimitGraphicDrawer()
	{
	InitialiseMembers();
	}

void CWsRateLimitGraphicDrawer::InitialiseMembers()
	{
	iColors[0] = TRgb(0x000000);
	iColors[1] = TRgb(0x555555);
	iColors[2] = TRgb(0x000080);
	iColors[3] = TRgb(0x008000);
	iColors[4] = TRgb(0x008080);
	iColors[5] = TRgb(0x800000);
	iColors[6] = TRgb(0x800080);
	iColors[7] = TRgb(0x808000);
	iColors[8] = TRgb(0x0000ff);
	iColors[9] = TRgb(0x00ff00);
	iColors[10] = TRgb(0x000000);
	iColors[11] = TRgb(0x555555);
	iColors[12] = TRgb(0x000080);
	iColors[13] = TRgb(0x008000);
	iColors[14] = TRgb(0x008080);
	iColors[15] = TRgb(0x800000);
	iColors[16] = TRgb(0x800080);
	iColors[17] = TRgb(0x808000);
	iColors[18] = TRgb(0x0000ff);
	iColors[19] = TRgb(0x00ff00);
	iColors[20] = TRgb(0x000000);
	iColors[21] = TRgb(0x555555);
	iColors[22] = TRgb(0x000080);
	iColors[23] = TRgb(0x008000);
	iColors[24] = TRgb(0x008080);
	iColors[25] = TRgb(0x800000);
	iColors[26] = TRgb(0x800080);
	iColors[27] = TRgb(0x808000);
	iColors[28] = TRgb(0x0000ff);
	iColors[29] = TRgb(0x00ff00);
	iColors[30] = TRgb(0x000000);
	iColors[31] = TRgb(0x555555);
	iColors[32] = TRgb(0x000080);
	iColors[33] = TRgb(0x008000);
	iColors[34] = TRgb(0x008080);
	iColors[35] = TRgb(0x800000);
	iColors[36] = TRgb(0x800080);
	iColors[37] = TRgb(0x808000);
	iColors[38] = TRgb(0x0000ff);
	iColors[39] = TRgb(0x00ff00);
	
	iFrameRate = 0.0;
	}


CWsRateLimitGraphicDrawer::~CWsRateLimitGraphicDrawer()
	{
	}

void CWsRateLimitGraphicDrawer::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv,aId,aOwner);
	}

/**	
Simulate two animations of different frame rate with their respective schedule.

@param MWsGc Window server graphic context to draw the animation
@param TRect Rectangle are required to draw the animation
@param TDesC Parameter value to use inside this function.
*/
void CWsRateLimitGraphicDrawer::DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& /*aData*/) const
	{
	TPoint KWinPos = aRect.iTl + TPoint(10,10);
	const TSize KWinSize(100,100);
	const TRect KWinRect(KWinPos,KWinSize);
	
	MWsGraphicsContext* context = static_cast<MWsGraphicsContext*>(aGc.ResolveObjectInterface(KMWsGraphicsContext));
	
	//Get the time the animation starts
	if(iFrame == KFirstFrame)
		{
		iStartTime = aGc.Now();
		}
	
	//Draw a filled rect with the chosen color
	context->Push();
	context->SetBrushStyle(MWsGraphicsContext::ESolidBrush);
	context->SetBrushColor(iColors[iFrame]);
	context->DrawRect(KWinRect);
	context->Pop();
	
	if(iFrame < KMaxFrames)
		{
		aGc.ScheduleAnimation(KWinRect,0);
		}
	else
		{
		TTimeIntervalMicroSeconds animLength = aGc.Now().MicroSecondsFrom(iStartTime);
		
		iFrameRate = static_cast<TReal>(KMaxFrames)/(static_cast<TReal>(animLength.Int64())/KOneSecondInMicroSecs);
		}

	iFrame++;
	}

/**	
Handles message between client and plug-in.

@param TDesC Constant message command.
*/
void CWsRateLimitGraphicDrawer::HandleMessage(const TDesC8& aData)
	{
	switch (aData[0])
		{
		case KCmdCount:
			TPckgBuf<TAnimData> buf;
			buf().iFrameRate = iFrameRate;
			TInt err = SendMessage(buf);
			__ASSERT_DEBUG(err>=KErrNone, User::Invariant());
			break;
		}
	}
	
