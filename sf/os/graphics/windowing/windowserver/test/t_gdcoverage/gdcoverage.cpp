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
 @internalComponent - Internal Symbian test coverage code for GraphicDrawer related code
*/

#include "gdcoverage.h"
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>
#include <gdi.h>
#include <graphics/wsscreendevice.h>

const TInt KDefaultScreen = 0;

//
CWsGraphicDrawerCoverage* CWsGraphicDrawerCoverage::NewL()
	{
	return new(ELeave) CWsGraphicDrawerCoverage;
	}
	
CWsGraphicDrawerCoverage::~CWsGraphicDrawerCoverage()
	{
	}
	
void CWsGraphicDrawerCoverage::ConstructL(
	MWsGraphicDrawerEnvironment& aEnv,
	const TGraphicDrawerId& aId,
	MWsClient& aOwner,
	const TDesC8& aData)
	{
	BaseConstructL(aEnv, aId, aOwner);

	iScreenId = KDefaultScreen;
	if (aData.Length()>0)
		iScreenId = aData[0];
	MWsScreen* scr = aEnv.Screen(iScreenId);
	User::LeaveIfNull(scr);

	//NGA supports MWsScreenDevice and non-NGA supports MWsScreenConfig, MWsFrontBuffer, MWsBackBuffer
	MWsScreenDevice* screenDevice = scr->ObjectInterface<MWsScreenDevice>();
	if(screenDevice)
		{
		//empty for now
		}
	else
		{
		MWsScreenConfig* cfg = scr->ObjectInterface<MWsScreenConfig>();
		User::LeaveIfNull(cfg);
		TSize sizeInPixels = cfg->ScreenModeSizeInPixels();
		__ASSERT_ALWAYS(sizeInPixels.iWidth>0 && sizeInPixels.iHeight>0, User::Invariant());
		__ASSERT_ALWAYS(cfg->Stride()>0, User::Invariant());
		__ASSERT_ALWAYS(cfg->ScalingFactor()==TSize(1,1), User::Invariant());
		__ASSERT_ALWAYS(cfg->Origin()==TPoint(0,0), User::Invariant());
		
		MWsFrontBuffer* fBuffer = scr->ObjectInterface<MWsFrontBuffer>();
		User::LeaveIfNull(fBuffer);
		__ASSERT_ALWAYS(fBuffer->GetBits()!=NULL, User::Invariant());
		__ASSERT_ALWAYS(fBuffer->GetBitGc()!=NULL, User::Invariant());
		}

	MWsScreenRedraw* redraw = scr->ObjectInterface<MWsScreenRedraw>();
	User::LeaveIfNull(redraw);
	}

void CWsGraphicDrawerCoverage::HandleMessage(const TDesC8& aData)
	{
	// wserv already check data size, and won't invoke this handler if it's empty
	TBuf8<1> ack;
	ack.Append(KGdCoverageInfoSig);

	MWsGraphicDrawerEnvironment& env = Env();
	MWsScreen* scr = env.Screen(iScreenId);
	MWsScreenRedraw* redraw = scr->ObjectInterface<MWsScreenRedraw>();
	
	switch (aData[0])
		{
		case KGdCoverageCmdQuery:
			SendInfo();
			break;

		case KGdCoverageCmdCoverRedraw:
			{
			//Hint: for extra coverage in ScheduleRender find a way to call
			//CWsTop::WindowServer()->AnimationScheduler()->Animate(*scr);
		
			//cannot create a real observer, for coverage null is sufficient
			redraw->SetObserver(NULL);
			redraw->IsUpdatePending();
			TTimeIntervalMicroSeconds interval(1);
			redraw->ScheduleRender(interval);
			const TRegion *r = redraw->AnimationRegion();
			redraw->UpdateDevice();
			//although redraw is actually a CScreenRedraw, we cannot include
			//the ScreenRedraw.h here so the following methods are not available:
			//redraw->AddRedrawRegion(*r, ETrue, ERedrawTopOnly);
			//redraw->OnAnimation();
			break;
			}
			
		default:
			SendMessage(ack);
			break;
		}
	}
	
void CWsGraphicDrawerCoverage::DoDraw(MWsGc& /*aGc*/, const TRect& /*aRect*/, const TDesC8& /*aData*/) const
	{
	}

void CWsGraphicDrawerCoverage::SendInfo()
	{
	TPckgBuf<TGdCoverageInfo> buf;
	MWsScreen* scr = Env().Screen(iScreenId);
	if (scr)
		{
		buf().iSignature = KGdCoverageInfoSig;
		buf().iNumTests = 1;
		}
	TInt err = SendMessage(buf);
	__ASSERT_ALWAYS(err>=KErrNone, User::Invariant());
	}

