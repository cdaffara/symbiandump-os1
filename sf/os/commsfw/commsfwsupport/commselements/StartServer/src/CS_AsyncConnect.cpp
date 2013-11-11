// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RAsyncConnect Client side header
// Declares the Symbian OS RAsyncConnectBase class as a base for client side API 
// of servers supporting startup synchronisation using Rendezvous
// 
//

/**
 @file 
 @internalTechnology
*/

#include "CS_AsyncConnect.h"
#include <e32math.h>


EXPORT_C void RAsyncConnectBase::CreateSession(const TDesC& aServerName, const TVersion& aVersion, TInt aAsyncMessageSlots, TRequestStatus& aStatus)
/**
 * The RAsyncConnectBase::Connect method
 *
 * Connect the Handle to the Server
 *
 * @internalTechnology
 *
 * @param aServerName name of the daemon, no file extension
 * @param aVersion version of the API
 * @param aAsyncMessageSlots number of slots in server queue (apply to all clients)
 * @param aStatus user request
 */
	{
	TInt err = RSessionBase::CreateSession(aServerName, aVersion, aAsyncMessageSlots, EIpcSession_Unsharable, NULL, &aStatus);
   	if (KErrNone != err)
   		{
		TRequestStatus* p = &aStatus;
		User::RequestComplete(p, err);
   		}
	}

