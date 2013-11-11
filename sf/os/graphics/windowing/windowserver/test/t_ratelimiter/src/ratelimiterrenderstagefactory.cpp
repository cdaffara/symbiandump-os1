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

#include "ratelimiterrenderstagefactory.h"
#include "ratelimiterrenderstage.h"
#include "panics.h"

_LIT(KRateLimiterRenderStageName, "ratelimiter");

CRateLimiterRenderStageFactory* CRateLimiterRenderStageFactory::CreateL()
	{
	return new(ELeave) CRateLimiterRenderStageFactory;
	}
	
void CRateLimiterRenderStageFactory::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	}
	
CRateLimiterRenderStageFactory::CRateLimiterRenderStageFactory()
	{
	}
	
CRateLimiterRenderStageFactory::~CRateLimiterRenderStageFactory()
	{
	}

TAny* CRateLimiterRenderStageFactory::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
	case KMWsRenderStageFactory:
		return static_cast<MWsRenderStageFactory*>(this);
		}

	return NULL;
	}

CWsRenderStage* CRateLimiterRenderStageFactory::CreateFinalRenderStageL(MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, TInt /*aScreenNumber*/)
	{
	Panic(ERateLimiterPanicRateLimiterMustNotBeFinalRenderStage);
	return NULL;
	}

CWsRenderStage* CRateLimiterRenderStageFactory::CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage)
	{
	return CRateLimiterRenderStage::NewL(&Env(), aScreen, aScreenRedraw, aNextStage);
	}

const TDesC& CRateLimiterRenderStageFactory::PluginName() const
	{
	return KRateLimiterRenderStageName;
	}
