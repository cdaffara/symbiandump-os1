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

/**
 @file
 @internalComponent
*/

#include <bluetooth/hciserverclient.h>

#include <bt_sock.h>
#include <bluetooth/hci/hciipc.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCISERVERCLIENT);
#endif

// Don't allocate memory for dedicated message slots, just use the ones from
// the global pool.
const TInt KAsyncMessageSlots = -1;

EXPORT_C RHCIServerSession::RHCIServerSession() 
	{
	}

/**
Connect the handle to the server.
Must be called before all other methods (except Version and Close).
@return KErrNone if successful, a system-wide error code otherwise.
*/
EXPORT_C TInt RHCIServerSession::Open(TInt aInterfaceUid)
	{
	TRAPD(err, OpenL(aInterfaceUid));
	return err;
	}

void RHCIServerSession::OpenL(TInt aInterfaceUid)
	{
	// Attempt to connect to the latest version number
	TVersion hciSvrVersion2(KHCISrvVersion2MajorVersionNumber, 
							KHCISrvVersion2MinorVersionNumber, 
							KHCISrvVersion2BuildNumber);

	TInt err = CreateSession(KHCIServerName, hciSvrVersion2, KAsyncMessageSlots);
	
	if (err != KErrNone)
		{
		// The connection failed for some reason, this could be due to the version number not
		// being supported, the server not being loaded or another reason. First attempt
		// to load the Bluetooth stack and in turn the server.
		LEAVEIFERRORL(iSocketServer.Connect());
		CleanupClosePushL(iSocketServer);

		TRequestStatus status;
		iSocketServer.StartProtocol(KBTAddrFamily,KSockSeqPacket,KBTLinkManager,status);
		User::WaitForRequest(status);
		TInt err = status.Int();
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			LEAVEIFERRORL(err);
			}

		// Now attempt the connection again.
		err = CreateSession(KHCIServerName, hciSvrVersion2, KAsyncMessageSlots);

		if (err != KErrNone)
			{
			// Finally try reverting to the old version number.
			TVersion hciSvrVersion1(KHCISrvMajorVersionNumber, 
									KHCISrvMinorVersionNumber, 
									KHCISrvBuildNumber);
			
			LEAVEIFERRORL(CreateSession(KHCIServerName, hciSvrVersion1, KAsyncMessageSlots));
			}
		
		// Now we have a server side session setup, so now we will cleanup the socketserver
		// on Close
		CleanupStack::Pop();
		}

	// Now try and connect to the requested service.
	CleanupClosePushL(*this);
	LEAVEIFERRORL(SendReceive(EConnectToService, TIpcArgs(aInterfaceUid)));
	CleanupStack::Pop();
	}

/**
Close the session.
*/
EXPORT_C void RHCIServerSession::Close()
	{
	if (iSocketServer.Handle())
		{
		// Release handle on the Bluetooth stack if we loaded it in OpenL
		TRequestStatus status;
		iSocketServer.StopProtocol(KBTAddrFamily,KSockSeqPacket,KBTLinkManager,status);
		User::WaitForRequest(status);
		
		iSocketServer.Close();
		}

	// Close the session
	RSessionBase::Close();
	}

/**
Synchronous request to be sent to HCI.

@param aInterfaceUid The type of request, for example power control, or direct access
@param aCommand The actual request, for example SetPower
@return KErrNone if successful, a system-wide error code otherwise.
*/
EXPORT_C TInt RHCIServerSession::SendSync(TUint aCommand)
	{
	return SendSync(aCommand, NULL, NULL, 0);
	}

/**
Asynchronous request to be sent to HCI.
	
@param aInterfaceUid The type of request, for example power control, or direct access
@param aCommand The actual request, for example SetPower
@param aStatus Used to alert user of completion of request.
*/
EXPORT_C void RHCIServerSession::SendAsync(TUint aCommand, TRequestStatus &aStatus)
	{
	SendAsync(aCommand, NULL, NULL, 0, aStatus);
	}

/**
Synchronous request to be sent to HCI.
	
@param aInterfaceUid The type of request, for example power control, or direct access
@param aCommand The actual request, for example SetPower
@param aDescIn The descriptor carries input information
@param aDescOut The descriptor carries output information
@param aValue A value for use with the command.
@return KErrNone if successful, a system-wide error code otherwise.
	*/
EXPORT_C TInt RHCIServerSession::SendSync(TUint aCommand, TDes8* aDescIn, TDes8* aDescOut, TUint aValue)
	{
	return SendReceive(EServiceSpecificRequest,TIpcArgs(aCommand,aDescIn,aDescOut,aValue));
	}

/**
Asynchronous request to be sent to HCI.
	
@param aInterfaceUid The type of request, for example power control, or direct access
@param aCommand The actual request, for example SetPower
@param aDescIn The descriptor carries input information
@param aDescOut The descriptor carries output information
@param aValue A value for use with the command.
@param aStatus Used to alert user of completion of request.
*/
EXPORT_C void RHCIServerSession::SendAsync(TUint aCommand, TDes8* aDescIn, TDes8* aDescOut, TUint aValue, TRequestStatus& aStatus)
	{
	SendReceive(EServiceSpecificRequest,TIpcArgs(aCommand,aDescIn,aDescOut,aValue),aStatus);
	}
