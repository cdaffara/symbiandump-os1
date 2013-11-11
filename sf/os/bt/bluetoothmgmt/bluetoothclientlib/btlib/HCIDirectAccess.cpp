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
// Generic functions for direct HCI access (licensee-specific)
// 
//

#include "HCIDirectAccess.h"

#include <bt_sock.h>
#include <bluetooth/hci/hciipc.h>

# include <bluetooth/hciserverclient.h>
#include <bthci.h>

/**
Constructor
*/
EXPORT_C RHCIDirectAccess::RHCIDirectAccess()
	: iHCIServerSession(NULL)
	, iSocketAccess(NULL)
	{
	}

/**	
Close the HCIDirectAccess.
*/
EXPORT_C void RHCIDirectAccess::Close()
	{
	if(iHCIServerSession)
		{
		iHCIServerSession->Close();
		delete iHCIServerSession;
		iHCIServerSession = NULL;
		}
	}

/**
Open a direct interface between the application layer and the HCI for
custom messages.
@return Error code
@publishedAll
@released
@capability LocalServices
*/
EXPORT_C TInt RHCIDirectAccess::Open()
	{
	TInt err = KErrNoMemory;	
	iHCIServerSession = new RHCIServerSession;
	if(iHCIServerSession)
		{
		err = iHCIServerSession->Open(KHCIDirectAccessManagerUid);
		if(err != KErrNone)
			{
			delete iHCIServerSession;
			iHCIServerSession = NULL;
			}
		}	
	return err;
	}

/**
Open a direct interface between the application layer and the HCI for
custom messages. This method exists for backwards compatability. The
ESock session is not used.
@param aSocketServ ESock session
@return Error code
@capability LocalServices
*/
EXPORT_C TInt RHCIDirectAccess::Open(RSocketServ& /*aSocketServ*/)
	{
	return Open();
	}

/** 
Applies an asynchronous control operation on the HCI.

Data may be passed and received if a descriptor address is provided as an argument.
Only one Ioctl() operation may be outstanding for each RHCIDirectAccess.

@see RSocket::Ioctl(TUint aCommand,TRequestStatus &aStatus,TDes8* aDesc,TUint aLevel)
@param aCommand Ioctl command.
@param aStatus Status parameter for asynchronous request
@param aDesc Pointer to a descriptor in which data may be sent and received on completion.
@param aLevel Control operation level. Should be KSolBtHCI for this API
*/
EXPORT_C void RHCIDirectAccess::Ioctl(TUint aCommand, TRequestStatus& aStatus, TDes8* aDesc,TUint aLevel)
	{
	if(!iHCIServerSession)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotSupported);
		return;
		}

	iHCIServerSession->SendAsync(aCommand, aDesc, NULL, aLevel, aStatus);
	}

/** 
Cancels the Ioctl() asynchronous control operation.
@see RSocket::CancelIoctl()
*/
EXPORT_C void RHCIDirectAccess::CancelIoctl()
	{
	if(!iHCIServerSession)
		{
		return;
		}

	iHCIServerSession->SendSync(KHCIDirectAccessCancel, NULL, NULL, 0);
	}

/** 
Applies an asynchronous control operation on the HCI.

Data may be passed and received if a descriptor address is provided as an argument.
Only one AsyncMessage operation may be outstanding for each RHCIDirectAccess.

@param aCommand Message identifier.
@param aStatus Status parameter for asynchronous request
@param aDesc Pointer to a descriptor in which data may be sent and received on completion.
*/
EXPORT_C void RHCIDirectAccess::AsyncMessage(TUint aCommand, TRequestStatus& aStatus, TDes8* aDesc)
	{
	if(!iHCIServerSession)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotSupported);
		return;
		}

	iHCIServerSession->SendAsync(aCommand, aDesc, NULL, 0, aStatus);
	}

/** 
Cancels the Ioctl() asynchronous control operation.
@see RSocket::CancelIoctl()
*/
EXPORT_C void RHCIDirectAccess::CancelAsyncMessage()
	{
	if(iHCIServerSession)
		{
		return;
		}

	iHCIServerSession->SendSync(KHCIDirectAccessCancel, NULL, NULL, 0);
	}

/**
Originally a getter for the sub-session handle.
This facility has now been removed.
@deprecated
@return Always return zero
*/
EXPORT_C TInt RHCIDirectAccess::SubSessionHandle()
	{
	return 0;
	}
