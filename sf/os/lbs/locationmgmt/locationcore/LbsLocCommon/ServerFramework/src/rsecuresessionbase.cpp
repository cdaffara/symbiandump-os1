// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// rsecureasbase.cpp
// 
//

#include <e32debug.h>
#include "cserverlaunch.h"
#include "rsecuresessionbase.h"
#include "csecureasbase.h"
#include "lbsmessageenums.h"
#include "lbsdevloggermacros.h"

/** Client side session object connects to the server by starting server with specified parameter
keep trying to kick off the server!

@param aParams A reference to TProcessStartParams object
@return Symbian error code
@see TProcessStartParams
@internalTechnology
@released
 */
TInt RSecureSessionBase::Connect(TServerStartParams& aParams)
	{
	// be very cunning :) This has to slot into the existing RPositionServer::Connect
	// The parameter will have to morph into a local variable but that should be the only change
	TInt ret;
	
	TInt retry = 2;
	FOREVER
		{ 
		TVersion version = Version();

		ret = CreateSession(aParams.GetServerName(), version, aParams.GetNumberOfServerSlots());

		if (ret != KErrNotFound && ret != KErrServerTerminated)
			{
			break;
			}

		if (--retry == 0)
			{
			break;
			}

		ret = StartServer(aParams);

		if (ret != KErrAlreadyExists && ret != KErrNone)
			{
			break;
			}
		}
	return ret;
	}
	
/** Client side session object close the session handler

@internalTechnology
@released
 */
void RSecureSessionBase::Close()	
	{
	LBSLOG(ELogP1, "RSecureSessionBase::Close");
	RHandleBase::Close();
	}

/**
*/
TInt RSecureSessionBase::StartServer(TServerStartParams& aParams)
	{
	return (CServerLaunch::ServerLaunch(aParams));
	}
	

//------------------------------------------------------------------------------	
	
/** Client side subsession object issue the subsession close request.

@internalTechnology
@released
 */
void RSecureSubSessionBase::Close()	
	{
	CloseSubSession(ESecureSubSessionBaseClose);
	}
