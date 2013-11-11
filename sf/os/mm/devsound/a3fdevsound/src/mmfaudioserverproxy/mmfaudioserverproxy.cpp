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
//



#include <f32file.h>
#include <e32math.h>
#include <s32mem.h>

#include "mmfaudioserverproxy.h"
#include "mmfaudioserverdecs.h"
#include <a3f/mmfaudiosvruids.hrh>


static const TUid KUidAudioServer = {KUidA3fAudioServerUID};
const TInt KRetries = 5; 
const TInt KInitialTime = 100000;  //100ms
const TInt KTimeIncrement = 50000; // 50ms 

_LIT_SECURITY_POLICY_C1(KServerHasDRMRights, ECapabilityDRM);

// -----------------------------------------------------------------------------
// RMMFAudioServerProxy::OpenSessionToTrustedAudioServer
//
// -----------------------------------------------------------------------------
//	
TInt RMMFAudioServerProxy::OpenSessionToTrustedAudioServer()
	{
	return CreateSession(KAudioServerName,
                                TVersion(KMMFAudioServerVersion,
                                KMMFAudioServerMinorVersionNumber,
                                KMMFAudioServerBuildVersionNumber),
                                -1, // Global pool
                                EIpcSession_Unsharable,
                                &KServerHasDRMRights,
                                NULL); // NULL required for synchronous behaviour
	}	

// -----------------------------------------------------------------------------
// RMMFAudioServerProxy::Open
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFAudioServerProxy::Open()
	{
	const TUidType serverUid(KNullUid,KNullUid,KUidAudioServer);
	// Assume the server is already running and attempt to create a session
	TInt err = OpenSessionToTrustedAudioServer();
	
	if(err == KErrNotFound)
		{
		// Server not running
		// Construct the server binary name
		RProcess server;

		_LIT(KEmpty,"");

		err = server.Create(KAudioServerFileName, KEmpty, serverUid);
		if(err != KErrNone)
			{
			return err;
			}
		// Synchronise with the server
		TRequestStatus reqStatus;
		server.Rendezvous(reqStatus);

		if (reqStatus!=KRequestPending)
			{
			server.Kill(0);
			}
		else
			{
			// Start the test harness
			server.Resume();
			// Server will call the reciprocal static synchronise call
			}

		User::WaitForRequest(reqStatus); // wait for rendezvous or death
		server.Close();
		TInt reqStatusValue = reqStatus.Int();

		if(reqStatusValue != KErrNone)
			{
			if(reqStatusValue == KErrNoMemory || reqStatusValue == KErrNotFound)
				{
				// All error codes except KErrNoMemory and KErrNotFound are assumed
				// to be a duplicate server instance dying, then keep trying
				// connection to the server. This can happen when two servers
				// attempt to start at the same time.
				return reqStatusValue;
				}
			//Wait for sometime, then retry and creation session
			TInt waitTime = KInitialTime;
			for(TInt retries=0;retries<KRetries;retries++)
				{
				User::After(waitTime);
				waitTime+=KTimeIncrement;
				err = OpenSessionToTrustedAudioServer();				
				
				if(err==KErrNone)
					{
					//Session created successfully
					break;
					}	
				}			
			}
		else
			{
			// Create the root server session
            err = OpenSessionToTrustedAudioServer();
			}
		}
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFAudioServerProxy::SetDevSoundInfo
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFAudioServerProxy::SetDevSoundInfo()
	{
	return SendReceive(EMMFAudioLaunchRequests);
	}

// -----------------------------------------------------------------------------
// RMMFAudioServerProxy::GetDevSoundSessionHandle
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFAudioServerProxy::GetDevSoundSessionHandle()
	{
	return SendReceive(EMMFDevSoundSessionHandle);
	}

// End of File
