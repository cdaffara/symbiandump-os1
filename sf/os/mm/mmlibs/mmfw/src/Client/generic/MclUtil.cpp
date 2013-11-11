// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Public Media Server includes
#include <mda/client/utility.h>

#include "CompatAids.h"

#if defined(__PANIC_COMPAT_IS_ACTIVE)
_LIT(KMdaServerName, "CMdaServer");	
#endif // __PANIC_COMPAT_IS_ACTIVE			

// --------------------------------------------------------------
// CMdaServer member functions
// Public

/**
Connects to and starts the media server.

@return The connected session with the media server.
*/
EXPORT_C CMdaServer* CMdaServer::NewL()
	{
	return new(ELeave) CMdaServer();
	}

/**
Destructor. Closes the session with the media server.

@panic  EMdaUtilServerListenerListNotEmpty 
        Not all event listeners that were added to the server have been removed.
@panic  EMdaClientBaseSessionHasOpenObjects 
        The client has resources still open in the server.
*/
EXPORT_C CMdaServer::~CMdaServer()
	{
	}

/**
Adds an object event listener, so that events can be passed to it.

@param  aListener
        The object event listener to add to the list.
*/
EXPORT_C void CMdaServer::AddListenerL(MMdaObjectEventListener& /*aListener*/)
	{
	__PANIC_COMPAT(User::Panic(KMdaServerName, KDummyMdaServerNotImplemented));
	User::Leave(KErrNotSupported);
	}

/**
Removes an object event listener so that events can no longer be passed to it. 
The specified listener must be in the list, or a panic occurs.

@param  aListener
        The object event listener to remove from the list.
*/
EXPORT_C void CMdaServer::RemoveListener(MMdaObjectEventListener& /*aListener*/)
	{
	__PANIC_COMPAT(User::Panic(KMdaServerName, KDummyMdaServerNotImplemented));
	}
