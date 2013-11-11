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

#include "displayrenderstagefactory.h"
#include "displayrenderstage.h"
#include "panic.h"

_LIT(KDisplayRenderStageName, "display");

CDisplayRenderStageFactory* CDisplayRenderStageFactory::CreateL()
	{
	return new(ELeave) CDisplayRenderStageFactory;
	}
	
void CDisplayRenderStageFactory::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	}
	
CDisplayRenderStageFactory::CDisplayRenderStageFactory()
	{
	}
	
CDisplayRenderStageFactory::~CDisplayRenderStageFactory()
	{
	}

TAny* CDisplayRenderStageFactory::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
	case KMWsRenderStageFactory:
		return static_cast<MWsRenderStageFactory*>(this);
		}

	return NULL;
	}

CWsRenderStage* CDisplayRenderStageFactory::CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* /*aScreenRedraw*/, TInt aScreenNumber)
	{
	CDisplayRenderStage* stage = CDisplayRenderStage::NewL(&Env(), aScreen, aScreenNumber);
	return stage;
	}
	
CWsRenderStage* CDisplayRenderStageFactory::CreateRenderStageL(MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, CWsRenderStage* /*aNextStage*/)
	{
	//if we arrive here there has been a configuration error. this render stage must be the final render stage
	Panic(EPluginPanicDisplayRenderStageMustBeFinal);
	User::Leave(EPluginPanicDisplayRenderStageMustBeFinal);//to keep LeaveScan happy 
	return NULL;
	}

const TDesC& CDisplayRenderStageFactory::PluginName() const
	{
	return KDisplayRenderStageName;
	}

