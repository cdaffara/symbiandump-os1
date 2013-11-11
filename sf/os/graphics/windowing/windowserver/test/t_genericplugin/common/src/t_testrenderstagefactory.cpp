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
#ifndef GRAPHICS_TEST_PLUGIN_NGA
_LIT(KTestRenderStageInvalidName, "renderstageInvalid");
_LIT(KTestRenderStageInvalidROIName, "renderstageROI");
#endif //GRAPHICS_TEST_PLUGIN_NGA

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

/**
Initialisation phase of two phase construction.
@param aEnv The environment for a graphic drawer 
@param aData The data for construction
*/
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
#ifdef GRAPHICS_TEST_PLUGIN_NGA

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

#else //GRAPHICS_TEST_PLUGIN_NGA
/**
Creates a new CTestRenderStage object. 
@param	aScreen A pointer points to the interface for drawing occasion of a CWsGraphicDrawer 
		used by animation schedulers to update screens 
@param	aScreenRedraw A pointer points to the interface for handling the animation aspects of 
		the redraw sequence.
@return A pointer to the newly created object.
*/ 
CWsRenderStage* CTestRenderStageFactory::CreateStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw)
	{
	CTestRenderStage* stage = CTestRenderStage::NewL(&Env(), aScreen, aScreenRedraw);
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

/**********************************************************************
CTestRenderStageFactory_Invalid
***********************************************************************/
/**
Creates a new CTestRenderStageFactory_Invalid object.
*/
CTestRenderStageFactory_Invalid* CTestRenderStageFactory_Invalid::CreateL()
	{
	return new(ELeave) CTestRenderStageFactory_Invalid;
	}

/**
Initialisation phase of two phase construction.
@param aEnv The environment for a graphic drawer 
@param aData The data for construction
*/
void CTestRenderStageFactory_Invalid::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("TestRenderStageFactory_Invalid is created."));	
	CleanupStack::PopAndDestroy(log);
	}

/**
Creates a new RenderStage object. 
@param	aScreen A pointer points to the interface for drawing occasion of a CWsGraphicDrawer 
		used by animation schedulers to update screens 
@param	aScreenRedraw A pointer points to the interface for handling the animation aspects of 
		the redraw sequence.
@return For nagative test purpose, it simply returns NULL.
*/ 
CWsRenderStage* CTestRenderStageFactory_Invalid::CreateStageL(MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/)
	{
	return NULL;
	}

/**
Gets the plugin name
@return The Plugin name.
*/
const TDesC& CTestRenderStageFactory_Invalid::PluginName() const
	{
	return KTestRenderStageInvalidName;
	}

/**********************************************************************
CTestRenderStageFactory_InvalidROI
***********************************************************************/
/**
Creates a new CTestRenderStageFactory_InvalidROI object
*/
CTestRenderStageFactory_InvalidROI* CTestRenderStageFactory_InvalidROI::CreateL()
	{
	return new(ELeave) CTestRenderStageFactory_InvalidROI;
	}

/**
Initialisation phase of two phase construction.
@param aEnv The environment for a graphic drawer 
@param aData The data for construction
*/
void CTestRenderStageFactory_InvalidROI::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("TestRenderStageFactory_InvalidROI is created."));	
	CleanupStack::PopAndDestroy(log);
	}

/**
Overides MWsObjectProvider. Resolve an instance of an interface
@param aTypeId The interface UID.
@return For negative test purpose, it simply returns NULL.
*/
TAny* CTestRenderStageFactory_InvalidROI::ResolveObjectInterface(TUint /*aTypeId*/)
	{
	return NULL;
	}

/**
Gets the plugin name
@return The Plugin name.
*/
const TDesC& CTestRenderStageFactory_InvalidROI::PluginName() const
	{
	return KTestRenderStageInvalidROIName;
	}

#endif //GRAPHICS_TEST_PLUGIN_NGA
