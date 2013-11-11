// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMUTILITY_H_
#define __SSMUTILITY_H_

#include <e32def.h>
#include <e32cmn.h>

/**
 The UID value for System Utility Plugin DLLs.
 @publishedPartner
 @released
 */
const TInt KSsmUtilityPluginDllTypeUidValue=0x2000D767;

/**
 The type UID for System Utility Plugin DLLs.
 @publishedPartner
 @released
 */
const TUid KSsmUtilityPluginDllTypeUid={KSsmUtilityPluginDllTypeUidValue};


/**
 This interface is used for plug-ins in the SSM Utility Server that are intended
 to monitor some aspect of the system. 
 
 @see SSM_START_SSM_UTILITY_PLUGIN
 @released
 @publishedPartner
 */
class MSsmUtility
	{
public:
	/**
	 Expected to install an active object in the SSM
	 Utility Server's active scheduler.
	 */
	virtual void InitializeL() = 0;
	
	/**
	 Expected to start running an active object in the SSM
	 Utility Server's active scheduler.
	 */
	virtual void StartL() = 0;
	
	/**
	 Expected to stop, remove and delete the active object that was installed 
	 and started with the @c InitialiseL and @c StartL members of this interface.
	 Should also free resources allocated for this derived M class.
	 */
	virtual void Release() = 0;
	};

#endif
