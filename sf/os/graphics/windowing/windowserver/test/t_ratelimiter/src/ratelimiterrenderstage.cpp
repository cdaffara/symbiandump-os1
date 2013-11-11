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

#include "ratelimiterrenderstage.h"
#include "constants.h"

CRateLimiterRenderStage* CRateLimiterRenderStage::NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, CWsRenderStage* aNextStage)
	{
	CRateLimiterRenderStage* stage = new(ELeave) CRateLimiterRenderStage(*aEnv);
	CleanupStack::PushL(stage);
	stage->ConstructL(aNextStage);
	CleanupStack::Pop(stage);
	return stage;
	}

CRateLimiterRenderStage::CRateLimiterRenderStage(MWsGraphicDrawerEnvironment& aEnv)
	: iEnv(aEnv)
	{
	}

void CRateLimiterRenderStage::ConstructL(CWsRenderStage* aNextStage)
	{
	BaseConstructL();
	
	iTick = CPeriodic::NewL(0);
	
	SetNext(aNextStage);
	}

CRateLimiterRenderStage::~CRateLimiterRenderStage()
	{

	}

TAny* CRateLimiterRenderStage::ResolveObjectInterface(TUint aTypeId)
	{
	return CWsRenderStage::ResolveObjectInterface(aTypeId);
	}

TBool CRateLimiterRenderStage::OnTickCallBack(TAny* aAny)
	{
	CRateLimiterRenderStage* self = reinterpret_cast<CRateLimiterRenderStage*>(aAny);
	User::RequestComplete(self->iCompositorReady, KErrNone);
	
	self->iTick->Cancel();
	
	return EFalse;
	}

void CRateLimiterRenderStage::Begin(const TRegion* aRegion)
	{
	if (Next())
		{
		Next()->Begin(aRegion);
		}
	}

void CRateLimiterRenderStage::End(TRequestStatus* aCompositorReady)
	{
	// rate limit
	*aCompositorReady = KRequestPending;
	iCompositorReady = aCompositorReady;
	iTick->Start(KFrameRateDelay,0,TCallBack(OnTickCallBack,this));
	
	if (Next())
		{
		Next()->End(NULL);
		}
	}
