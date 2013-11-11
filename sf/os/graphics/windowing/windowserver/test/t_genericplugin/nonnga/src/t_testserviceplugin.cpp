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

#include "t_testserviceplugin.h"
#include "t_logfile.h"

_LIT(KTestServicePluginName, "testserviceplugin");
/**
Creates a new CTestServicePlugin object
*/ 
CTestServicePlugin* CTestServicePlugin::CreateL()
	{
	return new(ELeave) CTestServicePlugin;
	}

/**
Initialisation phase of two phase construction.
@param aEnv The environment for a graphic drawer 
@param aData The data for construction
*/
void CTestServicePlugin::ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv);
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("testserviceplugin is created."));		
	CleanupStack::PopAndDestroy(log);
	iBlackMap = 128;
	iWhiteMap = 255;
	}

/**
Constructor for CTestServicePlugin
*/
CTestServicePlugin::CTestServicePlugin()
	{
	}

/**
Destructor for CTestServicePlugin
*/
CTestServicePlugin::~CTestServicePlugin()
	{
	}

/**
Sets the fading parameter
@param	aBlackMap Black map fading parameter
@param	aWhiteMap White map fading parameter
*/
void CTestServicePlugin::SetFadingParamsL(TUint8 aBlackMap, TUint8 aWhiteMap)
	{
	iBlackMap = aBlackMap;
	iWhiteMap = aWhiteMap;
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("Fading parameters have been set."));
	CleanupStack::PopAndDestroy(log);
	}

/**
Gets the white map fading parameter
@return White map fading parameter
*/
TUint8 CTestServicePlugin::GetWhiteMapL()
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("Returned white map."));
	CleanupStack::PopAndDestroy(log);
	return iWhiteMap;
	}

/**
Gets the black map fading parameter
@return Black map fading parameter
*/
TUint8 CTestServicePlugin::GetBlackMapL()
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("Returned black map."));
	CleanupStack::PopAndDestroy(log);
	return iBlackMap;
	}

/**
Overides MWsObjectProvider. Resolve an instance of an interface
@param aTypeId The interface UID.
@return The pointer to the instance of an interface.
*/
TAny* CTestServicePlugin::ResolveObjectInterface(TUint aTypeId)
	{
	switch (aTypeId)
		{
		case EImplUid:
			return static_cast<CWsPlugin*>(this);
		}
	return NULL;
	}

/**
Gets the plugin name
@return The Plugin name.
*/
const TDesC& CTestServicePlugin::PluginName() const
	{
	return KTestServicePluginName;
	}
