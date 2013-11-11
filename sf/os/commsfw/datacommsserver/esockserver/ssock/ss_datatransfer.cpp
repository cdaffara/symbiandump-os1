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

/**
 @file SS_DATATRANSFER.CPP
*/

#include "ss_datatransfer.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksdtrns, "ESockSSocksdtrns");
#endif

/**
 Constructor
 */
EXPORT_C CConnDataTransfer::CConnDataTransfer()
	{
	}

/**
 Calculate out how much data has been sent and received on the sub-connection
 
 @param aUplinkVolume On return, contains the amount of data sent on this subconnection
 @param aDownlinkVolume On return, contains the amount of data received on this subconnection
 @return KErrNone if successful, otherwise one of the system-wide error codes
 */	
EXPORT_C TInt CConnDataTransfer::DataTransferred(TUint& aUplinkVolume, TUint& aDownlinkVolume)
	{
	return DoDataTransferred( aUplinkVolume, aDownlinkVolume);
	}
	
/**
 Cancel a request for data transfer information
 
 @return ETrue to indicate that the operation is completed here
 */	
EXPORT_C TInt CConnDataTransfer::DataTransferredCancel()
	{
	return DoDataTransferredCancel();
	}

/**
 Receive a request for a notification after a given volume of data has been sent on the sub-connection
 
 @param aRequestedGranularity The amount of data to be sent after which the notification will occur (but see notes); 
 this is relative to the current volume of data sent
 @param aRequestedNotificationVolume The absolute amount of data that should be sent before we send a notification; only used if aRequestedGranularity is zero
 @return KErrNone, or one of the system-wide error codes
 */	
EXPORT_C TInt CConnDataTransfer::DataSentNotificationRequest(TUint aRequestedGranularity, TUint aRequestedNotificationVolume)
	{
	return DoDataSentNotificationRequest(aRequestedGranularity, aRequestedNotificationVolume);
	}

/**
 Remove the additional notification that this request would have generated
 
 @return KErrNone, or one of the system-wide error codes
 */	
EXPORT_C TInt CConnDataTransfer::DataSentNotificationCancel()
	{
	return DoDataSentNotificationCancel();
	}

/**
 Receive a request for a notification after a given volume of data has been received on the sub-connection
 
 @param aRequestedGranularity The amount of data to be received after which the notification will occur (but see notes); this is relative to the current volume of data received
 @param aRequestedNotificationVolume The absolute amount of data that should be received before we send a notification; only used if aRequestedGranularity is zero
 @return KErrNone, or one of the system-wide error codes
 */
EXPORT_C TInt CConnDataTransfer::DataReceivedNotificationRequest(TUint aRequestedGranularity, TUint aRequestedNotificationVolume)
	{
	return DoDataReceivedNotificationRequest(aRequestedGranularity, aRequestedNotificationVolume);
	}
	
/**
 Remove the additional notification that this request would have generated
 
 @return KErrNone, or one of the system-wide error codes
 */	
EXPORT_C TInt CConnDataTransfer::DataReceivedNotificationCancel()
	{
	return DoDataReceivedNotificationCancel();
	}

/**
 Register a client
 @param aClient Reference to the MConnDataTransferNotify derived class as the client
 */	
EXPORT_C void CConnDataTransfer::RegisterClientL( MConnDataTransferNotify& aClient )
	{
	__ASSERT_DEBUG(iClients.Find(&aClient) == KErrNotFound, User::Panic(KSpecAssert_ESockSSocksdtrns, 1));
	iClients.AppendL(&aClient);
	}

/**
 De-register a client
 @param aClient Reference to the MConnDataTransferNotify derived class as the client
 */	
EXPORT_C void CConnDataTransfer::DeRegisterClient( MConnDataTransferNotify& aClient )
	{
	TInt index = iClients.Find(&aClient);
	if ( index >= 0 )
		{
		iClients.Remove(index);
		}
	}

