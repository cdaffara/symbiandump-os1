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
 @internalComponent
*/

#include "t_testrenderstagefactory.h"
#include "t_testrenderstage.h"
#include "t_logfile.h"

_LIT(KTestRenderStageName, "testrenderstage");

/**********************************************************************
CTestRenderStageFactory
***********************************************************************/
/**
Creates a new CTestRenderStageFactory object
*/
CTestRenderStageFactory* CTestRenderStageFactory::CreateL()
	{
	return new(ELeave) CTestRenderStageFactory;
	}

void CTestRenderStageFactory::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("TestRenderStageFactory is created."));
	CleanupStack::PopAndDestroy(log);
	}

/**
Constructor for CTestRenderStageFactory
*/
CTestRenderStageFactory::CTestRenderStageFactory()
	{
	}

/**
Destructor for CTestRenderStageFactory
*/
CTestRenderStageFactory::~CTestRenderStageFactory()
	{
	}

/**
Overides MWsObjectProvider. Resolve an instance of an interface
@param aTypeId The interface UID.
@return The pointer to the instance of an interface.
*/
TAny* CTestRenderStageFactory::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case KMWsRenderStageFactory:
			return static_cast<MWsRenderStageFactory*>(this);
		}
	return NULL;
	}

CWsRenderStage* CTestRenderStageFactory::CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, TInt /*aScreenNumber*/)
	{
	return CreateRenderStageL(aScreen, aScreenRedraw, NULL);
	}
	
CWsRenderStage* CTestRenderStageFactory::CreateRenderStageL(MWsScreen*aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage)
	{
	CTestRenderStage* stage = CTestRenderStage::NewL(&Env(), aScreen, aScreenRedraw, aNextStage);
	CleanupStack::PushL(stage);
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("TestRenderStage is created."));
	CleanupStack::PopAndDestroy(log);
	CleanupStack::Pop(stage);
	return stage;
	}

/**
Gets the plugin name
@return The Plugin name.
*/
const TDesC& CTestRenderStageFactory::PluginName() const
	{
	return KTestRenderStageName;
	}
