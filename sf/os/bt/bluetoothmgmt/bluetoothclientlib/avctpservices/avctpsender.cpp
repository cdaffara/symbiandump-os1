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

#include <bluetooth/logger.h>
#include "avctpsender.h"
#include "avctpbody.h"
#include "AvctpMessageParameters.h"
#include "avctppacketstatics.h"
#include "avctpPriorities.h"
#include "avctpserviceutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP_SERVICES);
#endif

using namespace SymbianAvctp;

/**
@param aNotify The class that provides access to the AVCTP client
@leave system wide reason
@return pointer to newly created object
*/
CAvctpSender* CAvctpSender::NewL(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, SymbianAvctp::TPid aPid)
	{
	LOG_STATIC_FUNC
	return new(ELeave) CAvctpSender(aNotify, aAvctpSocket, aPid);
	}

CAvctpSender::CAvctpSender(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, SymbianAvctp::TPid aPid) : 
	CActive(KDataPlanePriority),
	iNotify(aNotify),
	iAvctpSocket(aAvctpSocket)
	{
	LOG_FUNC
	iAddr.SetPid(aPid); // only need to set this once
	CActiveScheduler::Add(this);
	}

CAvctpSender::~CAvctpSender()
	{
	LOG_FUNC
	Cancel();
	}

/**
Sends AVCTP message
The Pid to send the message to was given when the client opened the RAvctp object
used to send this message.

@pre must be connected to remote
@param aBTDevice device address to which to send message
@param aTransactionLabel transaction label
@param aType the message type, i.e. Command or Response
@param aMessageInformation the contents of the message to send
@return  KErrInUse if the sender AO is active or
		 KErrInvalidTransactionLabel if the transaction label was invalid or
		 another system wide error code
@internalComponent
*/
TInt CAvctpSender::SendMessage(const TBTDevAddr& aBTDevice, 
				 TTransactionLabel aTransactionLabel,
				 TMessageType aType,
				 const TDesC8& aMessageInformation)
	{
	LOG_FUNC

	TInt ret = KErrNone;
	
	if (!IsActive())
		{
		// Sort out where we want to send stuff (the PID has already been set)
		iAddr.SetBTAddr(aBTDevice);

		// Use the flags parameter to pass on AVCTP specific information
		TAvctpMessageParameters parms;
		parms.SetType(aType);
		parms.iTransaction = aTransactionLabel;
		
		// Store the current transaction to use in MaenMessageSendComplete notification
		iCurrentTrans = aTransactionLabel;

		// Stick it all down the socket
		iAvctpSocket.SendTo(aMessageInformation, iAddr, parms.GetFlags(), iStatus);
		SetActive();
		}
	else 
		{
		ret = KErrInUse;
		}
		
	return ret;
	}

inline void CAvctpSender::NotifyMessageSent(const TBTDevAddr& aBTDevice, 
							       		  SymbianAvctp::TTransactionLabel aTransactionLabel,   
								  		  TInt aSendResult)
	{
	LOG_FUNC
	__ASSERT_BTADDR(aBTDevice);
	SymbianAvctp::AssertValidTransactionLabel(aTransactionLabel);
	iNotify.MaenMessageSendComplete(aBTDevice, aTransactionLabel, aSendResult);
	}
	
//
// Functions From CActive
	
void CAvctpSender::DoCancel()
	{
	LOG_FUNC

	iAvctpSocket.CancelSend();
	// Don't complete the client's request since they don't care about it anymore
	}

void CAvctpSender::RunL()
	{
	LOG_FUNC
	NotifyMessageSent(iAddr.BTAddr(), iCurrentTrans, iStatus.Int());
	}
	
//
// Other Functions
 
void CAvctpSender::ResetSendInfo()
	{
	LOG_FUNC

	iAddr = TAvctpSockAddr(0);
	iCurrentTrans = KUndefinedTransactionLabel;
	}

// EOF

