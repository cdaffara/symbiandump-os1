// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares the two main notification interfaces for the protocol.
// 
//

/**
 @file
 @internalAll
*/

#if !defined(SMSPCOMM_H__)
#define SMSPCOMM_H__

#include <e32std.h>
#include "smspmain.h"
#include "smsuact.h"
#include "smsuaddr.H"
#include "Gsmumsg.h"


#include <etel.h>

//
// Forward declarations
//

class TSmsAddr;
class CSmsMessage;

//
// Class declarations
//


/**
 *  The notification interface used by the protocol to notify Service Access
 *  Points (SAPs, eg CSmsProvider) or WAP SMS Protocol of completion of a
 *  previous request.
 *  
 *  It is also used to request and set the local address of a SAP.
 *  @internalComponent
 */
class MSmsMessageObserver
	{
public:
	// Used by the protocol to ask a SAP for its local address.
	virtual const TSmsAddr& GetLocalAddress() const=0;

	// Used by the protocol to bind (set) the local address for the SAP.
	virtual void SetLocalAddress(const TSmsAddr& aSmsAddr)=0;

	// Used to notify a SAP that the modem connection status has changed.
	virtual void ModemNotificationCompleted(TInt aStatus)=0;

	// Used to notify a SAP that sending of a message has completed.
	// @param aStatus is EPOC error or -(KFailureCauseOffset+failurecause).
	virtual void MessageSendCompleted(TInt aStatus)=0;

	// Used to notify a SAP that a message has been successfully received.
	// @param aSmsMessage the SMS message received.
	// @return aDes a buffer for the deliver report which can be filled in by the observer.
	// @return EPOC error or -(KFailureCauseOffset+failurecause) must be returned by the observer.
	virtual TInt MessageReceived(const CSmsMessage& aSmsMessage,TDes& aDes)=0;

	// Used to notify a SAP that a phone enumeration has completed.
	virtual void EnumeratePhoneCompleted(TInt aStatus)=0;

	// Used to notify a SAP that writing a message to the phone has completed.
	virtual void MessageWriteCompleted(TInt aStatus, const CSmsMessage* aSmsMessage = NULL)=0;

	// Used to notify a SAP that a message deletion has completed.
	virtual void MessageDeleteCompleted(TInt aStatus)=0;

	// Used to notify a SAP that a read all SMS parameters has completed.
	virtual void ReadSmsParamsCompleted(TInt aStatus,CMobilePhoneSmspList* aSmspList)=0;

    // Used to notify a SAP that a write all SMS parameters has completed.
    virtual void WriteSmsParamsCompleted(TInt aStatus)=0;

	// Used to figure out whether entry in reassembly store can be deleted, or confirmation of
	// successfull receipt from client is needed
    virtual TBool ClientConfirmsMessage()const=0;
	virtual TBool SmsAddrIsDuplicate(const MSmsMessageObserver* aObserver,const TSmsAddr& aAddr)const=0;
	};


/**
 *  The notification interface used by the active object derived state machines
 *  to notify the protocol of an event or to make a request.
 *  @internalComponent
 */
class MSmsComm
	{
public:
	// Callback to allow the protocol to notify a SAP of a new (valid) message.
	// @param aSmsMessage the received message.
	// @return EPOC error or -(KFailureCauseOffset+failurecause) must be returned by the observed.
	virtual TInt ProcessMessageL(const CSmsMessage& aSmsMessage,const TSmsAddr* aOriginalSmsAddr,const CSmsMessage* aOriginalSmsMessage,TDes& aDes)=0;

	// Callback to request deletion of a number of SMS PDUs from the phone.
	// @param aSlotArray the array of SMS PDU slots to delete.
	// @param aObserver a reference to an observer if the request is from a SAP.
	virtual void DeletePDUs(const CArrayFix<TGsmSmsSlotEntry>& aSlotArray,MSmsMessageObserver* aObserver)=0;

	// Callback when the state machine for sending a message has completed.
	virtual void MessageSendCompleted(TInt aStatus)=0;

	// Callback to notify the protocol that the modem detection status has changed.
	virtual void ModemNotificationCompleted(TInt aStatus,RPhone::TModemDetection aState)=0;

	// Callback when the state machine for enumerating the phone message stores has completed
	virtual void PhoneEnumerationCompleted(TInt aStatus)=0;

	// Callback when the state machine for deleting a PDU from the phone has completed
	virtual TBool ClosingDown() const =0;
	virtual void CloseNowWrap() = 0;

    // Check if observer is still present.
    // @param a reference to an observer.
    virtual TBool ObserverIsPresent(const MSmsMessageObserver& aObserver) const =0;
	// Callback to get network info
	virtual const RMobilePhone::TMobilePhoneNetworkInfoV1& NetworkInfo()const=0;
	virtual TBool NetworkInfoAvailable()const=0;

	// Callback to notify the protocol that the Disk Space Monitor has changed state.
	virtual void DiskSpaceMonitorStateChange(TSmsDiskSpaceMonitorStatus aStatus)=0;
	};

#endif // SMSPCOMM_H__
