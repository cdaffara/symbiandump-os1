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

#include "wsgraphicdrawercontext.h"
#include "stdgraphictestdrawer.h"

CWsGraphicDrawerTestFrameRate* CWsGraphicDrawerTestFrameRate::CreateL()
	{
	return new(ELeave) CWsGraphicDrawerTestFrameRate;
	}

CWsGraphicDrawerTestFrameRate::CWsGraphicDrawerTestFrameRate()
	{
	}

CWsGraphicDrawerTestFrameRate::~CWsGraphicDrawerTestFrameRate()
	{
	if (iContext)
		{
		iContext->Destroy();
		iContext = NULL;
		}
	}

void CWsGraphicDrawerTestFrameRate::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv,aId,aOwner);
	if (!(aEnv.Screen(0)->ResolveObjectInterface(KMWsCompositionContext) || aEnv.Screen(0)->ResolveObjectInterface(KMWsScene)))
		{
		iContext = CWsGraphicDrawerNonNgaContext::NewL();
		}
	else
		{
		iContext = CWsGraphicDrawerNgaContext::NewL();
		}
	}

void CWsGraphicDrawerTestFrameRate::DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& /*aData*/) const
	{
	if (KErrNone != iContext->Push(aGc))
		{
		return;
		}
	//Simplification: treats all screens the same
	iContext->DrawFrameRate(aGc, aRect, iFps);
	//Count this draw		
	iFps.Sample();
	//Schedule a redraw
	iContext->ScheduleAnimation(aGc,aRect,0); // as fast as possible
	iContext->Pop(aGc);
	}

void CWsGraphicDrawerTestFrameRate::HandleMessage(const TDesC8& /*aData*/)
	{
	}
	
TInt CFrameRate::Fps() const
	{
	if(iNumSamples > 1)
		{
		const TUint count = Min(iNumSamples,KMaxSamples);		
		TInt64 earliest = iSamples[0];
		TInt64 latest = earliest;
		for(TInt i=1; i<count; i++)
			{
			const TInt64 sample = iSamples[i];
			earliest = Min(earliest,sample);
			latest = Max(latest,sample);
			}
		const TInt64 duration = latest - earliest;
		const TInt microspf = (duration / count);
		if(microspf)
			{
			const TInt millispf = (microspf / 1000);
			if(millispf)
				{
				const TInt fps = (1000 / millispf);
				return fps;
				}
			}
		}
	return 0;
	}
	
TInt CFrameRate::CountSamples() const
	{
	return iNumSamples;
	}

void CFrameRate::Sample() const
	{
	//Add the current tick to the stats for next time
	TTime now;
	now.UniversalTime();
	iSamples[iNumSamples % KMaxSamples] = now.Int64();
	iNumSamples++;
	}
