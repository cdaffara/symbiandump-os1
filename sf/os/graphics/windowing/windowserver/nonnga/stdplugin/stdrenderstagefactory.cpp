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
//
// Description:
// STDRENDERFACTORY.CPP
// 
//

#include "stdrenderstagefactory.h"
#include "stdrenderstage.h"

_LIT(KStdRenderStageName, "std");

CStdRenderStageFactory* CStdRenderStageFactory::CreateL()
	{
	return new(ELeave) CStdRenderStageFactory;
	}
	
void CStdRenderStageFactory::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	}
	
CStdRenderStageFactory::CStdRenderStageFactory()
	{
	}
	
CStdRenderStageFactory::~CStdRenderStageFactory()
	{
	}

/**
Overidding MWsObjectProvider
*/
TAny* CStdRenderStageFactory::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case KMWsRenderStageFactory:
			return static_cast<MWsRenderStageFactory*>(this);
		}

	return NULL;
	}

CWsRenderStage * CStdRenderStageFactory::CreateStageL(MWsScreen * aScreen, MWsScreenRedraw *)
	{
	CStdRenderStage * stage = CStdRenderStage::NewL(&Env(), aScreen);
	return stage;
	}

const TDesC& CStdRenderStageFactory::PluginName() const
	{
	return KStdRenderStageName;
	}
