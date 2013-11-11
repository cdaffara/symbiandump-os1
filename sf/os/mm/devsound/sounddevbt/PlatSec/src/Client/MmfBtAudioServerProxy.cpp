// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MmfBtAudioServerProxy.h"
#include "MmfBtAudioServerStart.h"
#include "../../../inc/common/mmfBtBase.hrh"
#include "MmfBtAudioClientServer.h"

#define KMaxServerNameLength 256

static const TUid KUidAudioServer = {KUidMmfBtAudioServerDllUnicodeDefine};


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
		if(reqStatus.Int() != KErrNone)
			return reqStatus.Int();
		
		// Create the root server session
		err = CreateSession(KAudioServerName, TVersion(KMMFAudioServerVersion,
														KMMFAudioServerMinorVersionNumber,
														KMMFAudioServerBuildVersionNumber));
		}
	return err;		
		
	}

EXPORT_C TInt RMMFAudioServerProxy::SetDevSoundInfo()
	{
	return SendReceive(EMMFAudioLaunchRequests);
	}

EXPORT_C HBufC* RMMFAudioServerProxy::GetDevSoundServerNameL()
	{
	TInt len = GetDevSoundServerNameLengthL();
	len++;
	HBufC8* devSoundServerName = HBufC8::NewLC(len);
	TPtr8 devSoundServerNamePtr = devSoundServerName->Des();
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundServerName,
					                     KNullDesC8,
					                     KNullDesC8,
					                     devSoundServerNamePtr));

	RDesReadStream stream;
	stream.Open(*devSoundServerName);
	CleanupClosePushL(stream);

	HBufC* devSoundServerNameFlat = HBufC::NewL(stream, KMaxTInt);
	
	CleanupStack::PopAndDestroy();//stream
	CleanupStack::PopAndDestroy(devSoundServerName);

	return devSoundServerNameFlat;
	}


TInt RMMFAudioServerProxy::GetDevSoundServerNameLengthL()
	{
	TPckgBuf<TInt> descriptorSizePckg;
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundServerNameLength,
					                     KNullDesC8,
					                     KNullDesC8,
					                     descriptorSizePckg));

	return descriptorSizePckg();
	}


