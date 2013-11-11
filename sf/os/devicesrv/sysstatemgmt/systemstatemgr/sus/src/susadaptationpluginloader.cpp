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

#include "susadaptationpluginloader.h"
#include "suspanic.h"

CSusAdaptationPluginLoader* CSusAdaptationPluginLoader::NewL()
	{
	CSusAdaptationPluginLoader* self = new (ELeave) CSusAdaptationPluginLoader();
	return self;
	}

CSusAdaptationPluginLoader::CSusAdaptationPluginLoader()
	{
	
	}

CSusAdaptationPluginLoader::~CSusAdaptationPluginLoader()
	{
	ilibEmergencyCallRfAdaptation.Close();
	ilibStateAdaptation.Close();
	ilibSimAdaptation.Close();
	ilibMiscAdaptation.Close();
	ilibRtcAdaptation.Close();
	}

MStateAdaptation* CSusAdaptationPluginLoader::CreateStateAdaptationL()
	{
	__ASSERT_DEBUG( KNullHandle == ilibStateAdaptation.Handle(),	User::Panic(KPanicSsmSus, ELibraryAlreadyOpen));
	MStateAdaptation* stateAdaptationPlugin = NULL;
	TInt err = ilibStateAdaptation.Load(KStateAdaptationPlugin);

	// if licensees have not provided the State Adaptation Plugin, we try to load the reference
	// State Adaptation Plugin provided by Symbian
	if(err == KErrNotFound)
		{
		err = ilibStateAdaptation.Load(KRefStateAdaptationPlugin);
		}
	User::LeaveIfError(err);
	stateAdaptationPlugin = (MStateAdaptation *)(ilibStateAdaptation.Lookup(1)());
	if(stateAdaptationPlugin == NULL)
		{
		User::Leave(KErrNotFound);
		}
	return stateAdaptationPlugin;			
	}

MSimAdaptation* CSusAdaptationPluginLoader::CreateSimAdaptationL()
	{
	__ASSERT_DEBUG( KNullHandle == ilibSimAdaptation.Handle(), User::Panic(KPanicSsmSus, ELibraryAlreadyOpen));
	MSimAdaptation* simAdaptationPlugin = NULL;
	TInt err = ilibSimAdaptation.Load(KSimAdaptationPlugin);

	// if licensees have not provided the Sim Adaptation Plugin, we try to load the reference
	// Sim Adaptation Plugin provided by Symbian
	if(err == KErrNotFound)
		{
		err = ilibSimAdaptation.Load(KRefSimAdaptationPlugin);
		}
	User::LeaveIfError(err);
	simAdaptationPlugin = (MSimAdaptation *)(ilibSimAdaptation.Lookup(1)());
	if(simAdaptationPlugin == NULL)	
		{
		User::Leave(KErrNotFound);						
		}		 
	return simAdaptationPlugin;
	}

MMiscAdaptation* CSusAdaptationPluginLoader::CreateMiscAdaptationL()
	{
	__ASSERT_DEBUG( KNullHandle == ilibMiscAdaptation.Handle(), User::Panic(KPanicSsmSus, ELibraryAlreadyOpen));
	MMiscAdaptation* miscAdaptationPlugin = NULL;
	TInt err = ilibMiscAdaptation.Load(KMiscAdaptationPlugin);

	// if licensees have not provided the Misc Adaptation Plugin, we try to load the reference
	// Misc Adaptation Plugin provided by Symbian
	if(err == KErrNotFound)
		{
		err = ilibMiscAdaptation.Load(KRefMiscAdaptationPlugin);
		}
	User::LeaveIfError(err);
	miscAdaptationPlugin =(MMiscAdaptation *)(ilibMiscAdaptation.Lookup(1)());
	if(miscAdaptationPlugin == NULL)
		{
		User::Leave(KErrNotFound);						
		}		 
	return miscAdaptationPlugin;
	}

MRtcAdaptation* CSusAdaptationPluginLoader::CreateRtcAdaptationL()
	{
	__ASSERT_DEBUG( KNullHandle == ilibRtcAdaptation.Handle(), User::Panic(KPanicSsmSus, ELibraryAlreadyOpen));
	MRtcAdaptation* rtcAdaptationPlugin = NULL;
	TInt err = ilibRtcAdaptation.Load(KRtcAdaptationPlugin);

	// if licensees have not provided the Rtc Adaptation Plugin, we try to load the reference
	// Rtc Adaptation Plugin provided by Symbian
	if(err == KErrNotFound)
		{
		err = ilibRtcAdaptation.Load(KRefRtcAdaptationPlugin);
		}
	User::LeaveIfError(err);
	rtcAdaptationPlugin = (MRtcAdaptation *)(ilibRtcAdaptation.Lookup(1)());
	if (rtcAdaptationPlugin == NULL) 
		{
		User::Leave(KErrNotFound);						
		}		 
	return rtcAdaptationPlugin;
	}

MEmergencyCallRfAdaptation* CSusAdaptationPluginLoader::CreateEmergencyCallRfAdaptationL()
	{
	__ASSERT_DEBUG( KNullHandle == ilibEmergencyCallRfAdaptation.Handle(), User::Panic(KPanicSsmSus, ELibraryAlreadyOpen));
	MEmergencyCallRfAdaptation* emergencyCallRfAdaptationPlugin = NULL;
	TInt err = ilibEmergencyCallRfAdaptation.Load(KEmergencyCallRfAdaptationPlugin);

	// if licensees have not provided the EmergencyCallRf Adaptation Plugin, we try to load the reference
	// EmergencyCallRf Adaptation Plugin provided by Symbian
	if(err == KErrNotFound)
		{
		err = ilibEmergencyCallRfAdaptation.Load(KRefEmergencyCallRfAdaptationPlugin);
		}
	User::LeaveIfError(err);
	emergencyCallRfAdaptationPlugin = (MEmergencyCallRfAdaptation *)(ilibEmergencyCallRfAdaptation.Lookup(1)());	
	if (emergencyCallRfAdaptationPlugin == NULL) 
		{
		User::Leave(KErrNotFound);						
		}		 
	return emergencyCallRfAdaptationPlugin;
	}
