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

/**
 @file 
 @internalTechnology 
*/

#if !defined(__SS_DATATRANSFER_H__)
#define __SS_DATATRANSFER_H__

#include <e32def.h>
#include <e32base.h>

/**
 Interface that any sub-connection client wishing to get data notification must implement
 @released Since 9.1
 @internalTechnology
 */
class MConnDataTransferNotify
	{
public:
	/**
	 Override this to receive notification for data transfered, to allow any absolute volume 
	 notifications that may be outstanding to be completed if the required amount of data has been 
	 sent/received

	 @param aUplinkVolume The total volume of data sent on this subconnection
	 @param aDownlinkVolume The total volume of data received on this subconnection
	 @return KErrNone, or one of the system-wide error codes
	 @see CConnDataTransfer::DataTransferred
	 */
	virtual TInt NotifyDataTransferred(const TUint aUplinkVolume, const TUint aDownlinkVolume) = 0;
	
	/**	 	 
	 Override this to update the sent bytes count, and if necessary complete any outstanding RMessages
	 
	 @param aUplinkVolume The total number of bytes sent on this subconnection
	 @param aCurrentGranularity Requested granularity
	 @return KErrNone, or one of the system-wide error codes
	 */	 	
	virtual TInt NotifyDataSent(TUint aUplinkVolume, TUint aCurrentGranularity) = 0;
	
	/**
	 Override this to update the received bytes count, and if necessary complete any outstanding RMessages

	 @param aDownlinkVolume The total number of bytes sent on the sub-connection
	 @param aCurrentGranularity The currently set granularity of notifications
	 */
	virtual TInt NotifyDataReceived(TUint aDownlinkVolume, TUint aCurrentGranularity) = 0;
	};
	
/**
 Base class that any sub-connection client wishing to get sub-connection related data statitics must implement
 @released Since 9.1
 @internalTechnology
 */
class CConnDataTransfer : public CBase
	{
protected:
	IMPORT_C CConnDataTransfer();
	
public:
	IMPORT_C TInt DataTransferred(TUint& aUplinkVolume, TUint& aDownlinkVolume);
	IMPORT_C TInt DataTransferredCancel();
	IMPORT_C TInt DataSentNotificationRequest(TUint aRequestedGranularity, TUint aRequestedNotificationVolume);
	IMPORT_C TInt DataSentNotificationCancel();
	IMPORT_C TInt DataReceivedNotificationRequest(TUint aRequestedGranularity, TUint aRequestedNotificationVolume);
	IMPORT_C TInt DataReceivedNotificationCancel();

	IMPORT_C void RegisterClientL( MConnDataTransferNotify& aClient );
	IMPORT_C void DeRegisterClient( MConnDataTransferNotify& aClient );
	
protected:
	/**
	 Override this to register for data transfer notification on the sub-connection
	 
	 @param aUplinkVolume On return, contains the amount of data sent on this subconnection
	 @param aDownlinkVolume On return, contains the amount of data received on this subconnection
	 @return KErrNone if successful, otherwise one of the system-wide error codes
	 */
	virtual TInt DoDataTransferred(TUint& aUplinkVolume, TUint& aDownlinkVolume) = 0;
	
	/**
	 Override this to cancel a request for data transfer information
	 
	 @return ETrue to indicate that the operation is completed here
	 */
	virtual TInt DoDataTransferredCancel() = 0;
	
	/**
	 Override this to receive a request for a notification after a given volume of data has been sent on the sub-connection
	 
	 @param aRequestedGranularity The amount of data to be sent after which the notification will occur (but see notes); 
	 this is relative to the current volume of data sent
	 @param aRequestedNotificationVolume The absolute amount of data that should be sent before we send a notification; only used if aRequestedGranularity is zero
	 @return KErrNone, or one of the system-wide error codes
	 */
	virtual TInt DoDataSentNotificationRequest(TUint aRequestedGranularity, TUint aRequestedNotificationVolume) = 0;
	
	/**
	 Override this to remove the additional notification that this request would have generated
	 
	 @return KErrNone, or one of the system-wide error codes
	 */
	virtual TInt DoDataSentNotificationCancel() = 0;
	
	/**
	 Override this to receive a request for a notification after a given volume of data has been received on the sub-connection
	 
	 @param aRequestedGranularity The amount of data to be received after which the notification will occur (but see notes); this is relative to the current volume of data received
	 @param aRequestedNotificationVolume The absolute amount of data that should be received before we send a notification; only used if aRequestedGranularity is zero
	 @return KErrNone, or one of the system-wide error codes
	 */	
	virtual TInt DoDataReceivedNotificationRequest(TUint aRequestedGranularity, TUint aRequestedNotificationVolume) = 0;
	
	/**
	 Override this to remove the additional notification that this request would have generated
	 
	 @return KErrNone, or one of the system-wide error codes
	 */
	virtual TInt DoDataReceivedNotificationCancel() = 0;

protected:
	RPointerArray<MConnDataTransferNotify> iClients;
	};
	
#endif	// __SS_DATATRANSFER_H__

