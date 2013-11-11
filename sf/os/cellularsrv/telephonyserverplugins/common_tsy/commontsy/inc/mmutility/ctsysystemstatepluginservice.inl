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

#include <ecom/ecom.h>

// CONSTANTS
// Plugin name must match the default_data field in the resource file (case sensitive!)
_LIT8(KPluginName, "CtsySystemStatePluginPlugin");
const TUid KCtsySystemStatePluginServiceImplementationUid = { 0x102859CE }; 

// -----------------------------------------------------------------------------
// CCtsySystemStatePluginService::NewL
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
inline CCtsySystemStatePluginService* CCtsySystemStatePluginService::NewL()
	{
    TAny* interface = REComSession::CreateImplementationL(KCtsySystemStatePluginServiceImplementationUid,
													_FOFF(CCtsySystemStatePluginService, iDestructorIDKey));

	CCtsySystemStatePluginService* service = reinterpret_cast<CCtsySystemStatePluginService*>(interface);

    return service;
	}
	
// -----------------------------------------------------------------------------
// CCtsySystemStatePluginService::~CCtsySystemStatePluginService
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
inline CCtsySystemStatePluginService::~CCtsySystemStatePluginService()
	{			
	REComSession::DestroyedImplementation(iDestructorIDKey);		
	}


// End of File
