// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MmfAudioServerProxy.h"
#include "MmfAudioServerStart.h"
#include "MmfBase.hrh"
#include "MmfAudioClientServer.h"

#define KMaxServerNameLength 256

static const TUid KUidAudioServer = {KUidMmfAudioServerDllUnicodeDefine};
const TInt KRetries = 5; 
const TInt KInitialTime = 100000;  //100ms
const TInt KTimeIncrement = 50000; // 50ms 


EXPORT_C TInt RMMFAudioServerProxy::Open()
	{

	const TUidType serverUid(KNullUid,KNullUid,KUidAudioServer);
		
	// Assume the server is already running and attempt to create a session
	// 4 message slots
	TInt err = CreateSession(KAudioServerName, TVersion(KMMFAudioServerVersion,
														KMMFAudioServerMinorVersionNumber,
														KMMFAudioServerBuildVersionNumber));
	if(err == KErrNotFound)
		{
		// Server not running
		// Construct the server binary name
		TBuf<KMaxServerNameLength> serverFile;
		RProcess server;

		_LIT(KEmpty,"");

		err = server.Create(KAudioServerFileName, KEmpty, serverUid);
		if(err != KErrNone)
			return err;
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
				// to be a duplicate server instance dying, then keep trying connection
				// to the server. This can happen when two servers attempt to start
				// at the same time.
				return reqStatusValue;
				}
			//Wait for sometime, then retry and creation session
			TInt waitTime = KInitialTime;
			for(TInt retries=0;retries<KRetries;retries++)
				{
				User::After(waitTime);
				waitTime+=KTimeIncrement;
				err = CreateSession(KAudioServerName, TVersion(KMMFAudioServerVersion,
														KMMFAudioServerMinorVersionNumber,
														KMMFAudioServerBuildVersionNumber));
				if(err==KErrNone)
					{
					//Session created successfully
					break;
					}	
				}			
			}
		else
			{
			//Create the root server session
			err = CreateSession(KAudioServerName, TVersion(KMMFAudioServerVersion,
														KMMFAudioServerMinorVersionNumber,
														KMMFAudioServerBuildVersionNumber));	
			}
		}
	return err;		
		
	}

EXPORT_C TInt RMMFAudioServerProxy::SetDevSoundInfo()
	{
	return SendReceive(EMMFAudioLaunchRequests);
	}

EXPORT_C TInt RMMFAudioServerProxy::GetDevSoundSessionHandle()
	{
	return SendReceive(EMMFDevSoundSessionHandle);
	}

