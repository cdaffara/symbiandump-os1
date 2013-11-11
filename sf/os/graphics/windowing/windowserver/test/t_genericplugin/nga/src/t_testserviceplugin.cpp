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
#include <graphics/lookuptable.h>

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
	
	// Delete log file from previous test run
	CLogFile::DeleteLogFileL();
	
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("testserviceplugin is created."));		
	CleanupStack::PopAndDestroy(log);
	//Default in BitGdi was 128 for the blackMap and 255 for the whiteMap
	//SetFadingParameters shows how the fade color is computed
	iFadeColor.SetInternal(0x80FFFFFF);
	}

/**
Constructor for CTestServicePlugin
*/
CTestServicePlugin::CTestServicePlugin() : iLut(PtrTo16BitNormalisationTable())
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
@param	aFadeColor the fading color
*/
void CTestServicePlugin::SetFadingParamsL(TUint8 aBlackMap, TUint8 aWhiteMap)
	{
	//Situations where blackMap > whiteMap are NOT supported
	if (aBlackMap > aWhiteMap)
		{
		TUint8 oldMap = aBlackMap;
		aBlackMap = aWhiteMap;
		aWhiteMap = oldMap;
		}
	
	//CFbsBitGc::FadeArea() does the following per color component:
	//   dst = dst * (whiteMap - blackMap) + blackMap;

	//To achieve the same effect using MWsGraphicsContext we draw a rectangle
	//with specific intensity and alpha values:
	//   dst = dst * (1 - alpha) + intensity * alpha;
	//Thus:
	//   alpha = 1 - whiteMap + blackMap;
	//   intensity = blackMap / alpha;

	// alpha = 1 - whiteMap + blackMap;
	TInt alpha = 255 - aWhiteMap + aBlackMap;
	// intensity = blackMap / alpha;
	TInt i = (aBlackMap * iLut[alpha]) >> 8;

	iFadeColor.SetInternal(i << 16 | i << 8 | i | alpha << 24);

	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("Fading parameters have been set."));
	CleanupStack::PopAndDestroy(log);
	}

/**
Gets the fade color parameter
@return fade color parameter
*/
TRgb CTestServicePlugin::GetFadeColorL()
	{
	CLogFile* log = CLogFile::NewL();
	CleanupStack::PushL(log);
	log->WriteToLogL(_L("Returned fade color."));
	CleanupStack::PopAndDestroy(log);
	return iFadeColor;
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
