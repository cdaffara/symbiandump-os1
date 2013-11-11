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

#include "hwrmuipluginservice.h"
#include "hwrmuipluginresolver.h"

// CONSTANTS
// Plugin name must match the default_data field in the resource file (case sensitive!)
_LIT8( KPluginName, "HwrmUiPlugin" );


// -----------------------------------------------------------------------------
// CHwrmUiPluginService::NewL
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
CHwrmUiPluginService* CHwrmUiPluginService::NewL()
	{
	TUid uiPluginUid(TUid::Null());
	TAny* interface(NULL);

#ifdef _DEBUG
		{
		CHwrmUiPluginResolver* uiPluginResolver = CHwrmUiPluginResolver::NewLC();
		TRAP_IGNORE(uiPluginResolver->GetDesiredPluginL(uiPluginUid));
		CleanupStack::PopAndDestroy(uiPluginResolver);
		}
#endif

	if (uiPluginUid == TUid::Null())
		{
		
		// Set resolve parameters to find correct implementation    		            
	    TEComResolverParams resolverParams;     
	    resolverParams.SetDataType(KPluginName);
	    resolverParams.SetWildcardMatch(EFalse);
	                   
	    interface = REComSession::CreateImplementationL(KHwrmUiPluginServiceInterfaceUid,
														_FOFF(CHwrmUiPluginService, iDestructorIDKey),
	                                        			resolverParams,
														KRomOnlyResolverUid);
		}
	else
		{
	    interface = REComSession::CreateImplementationL(uiPluginUid,
	    												_FOFF(CHwrmUiPluginService, iDestructorIDKey));
		
		}
		
	User::LeaveIfNull(interface);
	CHwrmUiPluginService* service = reinterpret_cast<CHwrmUiPluginService*>(interface); 										    
       
    return service;	        	

	}
	
// End of File
