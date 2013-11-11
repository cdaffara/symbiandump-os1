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

#include "eventcheckerrenderstagefactory.h"
#include "eventcheckerrenderstage.h"
#include "panics.h"

_LIT(KEventCheckerRenderStageName, "eventchecker");

CEventCheckerRenderStageFactory* CEventCheckerRenderStageFactory::CreateL()
	{
	return new(ELeave) CEventCheckerRenderStageFactory;
	}
	
void CEventCheckerRenderStageFactory::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	}
	
CEventCheckerRenderStageFactory::CEventCheckerRenderStageFactory()
	{
	}
	
CEventCheckerRenderStageFactory::~CEventCheckerRenderStageFactory()
	{
	}

TAny* CEventCheckerRenderStageFactory::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
	case KMWsRenderStageFactory:
		return static_cast<MWsRenderStageFactory*>(this);
		}

	return NULL;
	}

CWsRenderStage* CEventCheckerRenderStageFactory::CreateFinalRenderStageL(MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, TInt /*aScreenNumber*/)
	{
	Panic(EEventCheckerPanicEventCheckerMustNotBeFinalRenderStage);
	return NULL;
	}

CWsRenderStage* CEventCheckerRenderStageFactory::CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage)
	{
	return CEventCheckerRenderStage::NewL(&Env(), aScreen, aScreenRedraw, aNextStage);
	}

const TDesC& CEventCheckerRenderStageFactory::PluginName() const
	{
	return KEventCheckerRenderStageName;
	}
