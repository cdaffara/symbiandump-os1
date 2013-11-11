// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "renderstagefactory.h"
#include "renderstage.h"


_LIT(KRenderStageName, "eglendpointremotetest");


CRenderStageFactory* CRenderStageFactory::CreateL()
	{
	return new(ELeave) CRenderStageFactory;
	}


//aData contains data specific to the plugin that was included in wsini.ini.
//EglEndpointTestRenderStage has no such defined data. Therefore ignore it.
void CRenderStageFactory::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	}


CRenderStageFactory::CRenderStageFactory()
	{
	}


CRenderStageFactory::~CRenderStageFactory()
	{
	}


TAny* CRenderStageFactory::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case KMWsRenderStageFactory:
			return static_cast<MWsRenderStageFactory*>(this);
		default:
			return NULL;
		}
	}


//EglEndpointTestRenderStage cannot be used as the last render stage in the chain. Therefore return NULL here.
CWsRenderStage* CRenderStageFactory::CreateFinalRenderStageL(MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, TInt /*aScreenNumber*/)
	{
	return NULL;
	}


CWsRenderStage* CRenderStageFactory::CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage)
	{
	return CRenderStage::NewL(&Env(), aScreen, aScreenRedraw, aNextStage);
	}


const TDesC& CRenderStageFactory::PluginName() const
	{
	return KRenderStageName;
	}
