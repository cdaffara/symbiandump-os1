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

#include <bluetooth/eirclient.h>
#include <e32uid.h>
#include <e32base.h>
#include <bluetooth/eirmanshared.h>
#include "eirmanclientlogger.h"

// Don't allocate memory for dedicated message slots, just use the ones from
// the global pool.
const TInt KAsyncMessageSlots = -1;

/* REirMan Client side class
REirMan provides access to EIR Manager API
*/
REirMan::REirMan() 
	: iBytesAvailablePckg(0)
	{
	LOG_FUNC
	}

/**
Connect the Handle to the Server
Must be called before all other methods except Version()

@return KErrNone if successful, otherwise the error that occurred
*/
void REirMan::Connect(TRequestStatus& aStatus)
	{
	LOG_FUNC
	
	// Don't start the Bluetooth Protocol Stack. It's either loaded and we'll use it, or else this will fail
	TInt err = CreateSession(KEirManServerName, Version(), KAsyncMessageSlots, EIpcSession_Unsharable, NULL, &aStatus);

	LOG1(_L8("CreateSession returned %d"), err);

	if(err != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		*status = KRequestPending;
		User::RequestComplete(status, err);
		}
	}

/**

*/
void REirMan::SpaceAvailableNotification(TUint& aBytesAvailable, TRequestStatus& aStatus) 
	{
	LOG_FUNC
	
	iBytesAvailablePckg.Set((TUint8*)&aBytesAvailable, sizeof(TUint), sizeof(TUint));

	SendReceive(EEirManSpaceAvailableNotification, TIpcArgs(&iBytesAvailablePckg), aStatus);
	}

/**

*/
void REirMan::CancelSpaceAvailableNotification()
	{
	LOG_FUNC
	SendReceive(EEirManCancelSpaceAvailableNotification);
	}

/**

*/
void REirMan::SetData(const TDesC8& aData, TEirDataMode aDataMode, TRequestStatus& aStatus) 
	{
	LOG_FUNC

	SendReceive(EEirManSetData, TIpcArgs(&aData, aDataMode), aStatus);
	}

/**

*/
void REirMan::RegisterTag(TEirTag aEirTag, TRequestStatus& aStatus)
	{
	LOG_FUNC

	SendReceive(EEirManRegisterTag, TIpcArgs(aEirTag), aStatus);
	}

// private
/**
Extract the version of the server providing the REirMan API

@return Version of the server
*/
TVersion REirMan::Version() const
	{
	return(TVersion(KEirManSrvMajorVersionNumber,KEirManSrvMinorVersionNumber,KEirManSrvBuildVersionNumber));
	}

TInt REirMan::RegisterTag(TEirTag aEirTag)
	{
	LOG_FUNC

	TInt ret = SendReceive(EEirManRegisterTag, TIpcArgs(aEirTag));

	return ret;
	}

void REirMan::NewData(TInt aRequiredLength, TRequestStatus& aStatus)
	{
	LOG_FUNC
	
	SendReceive(EEirManNewData, TIpcArgs(aRequiredLength), aStatus);
	}

