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

#include "screencapturestagefactory.h"
#include "screencapturerenderstage.h"
#include "panic.h"

_LIT(KCscRenderStageName, "tcsc");

CScreenCaptureStageFactory* CScreenCaptureStageFactory::CreateL()
	{
	return new(ELeave) CScreenCaptureStageFactory;
	}
	
void CScreenCaptureStageFactory::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	}
	
CScreenCaptureStageFactory::CScreenCaptureStageFactory()
	{
	}
	
CScreenCaptureStageFactory::~CScreenCaptureStageFactory()
	{
	}

/**
Overidding MWsObjectProvider
*/
TAny* CScreenCaptureStageFactory::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case KMWsRenderStageFactory:
			return static_cast<MWsRenderStageFactory*>(this);
		}

	return NULL;
	}

CWsRenderStage * CScreenCaptureStageFactory::CreateFinalRenderStageL(MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, TInt /*aScreenNumber*/)
	{
	Panic(EPluginPanicScreenCaptureRenderStageMustNotBeFinal);
	return NULL;
	}

CWsRenderStage * CScreenCaptureStageFactory::CreateRenderStageL(MWsScreen * /*aScreen*/, MWsScreenRedraw * /*aScreenRedraw*/, CWsRenderStage* aNextStage)
	{
	CScreenCaptureRenderStage * stage = CScreenCaptureRenderStage::NewL(&Env(), aNextStage);
	return stage;
	}

const TDesC& CScreenCaptureStageFactory::PluginName() const
	{
	return KCscRenderStageName;
	}
