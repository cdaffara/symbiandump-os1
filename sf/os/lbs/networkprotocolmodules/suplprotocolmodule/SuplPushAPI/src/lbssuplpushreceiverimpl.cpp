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
// Internal implementation of the receiver's part of the SUPL Push API
// 
//

/**
 @file
 @internalComponent
 @deprecated
*/

#include <e32base.h>
#include <centralrepository.h>
#include "lbsdevloggermacros.h"

#include "lbssuplpushreceiverimpl.h"
#include "lbssuplpushreceiverchannel.h"
#include "lbsrootcenrepdefs.h"

//=============================================================================
// CLbsSuplPushRecImpl
//=============================================================================
/**
Opens all the SUPL Push channels and starts listening for incoming SUPL INIT messages.

@param aObserver [In] A reference to an observer waiting for SUPL INIT messages.

@return an instance of the interface. The calling object becomes the
owner of the returned instance and is responsible its disposal.

@see CLbsSuplPushRec::NewL
*/
CLbsSuplPushRecImpl* CLbsSuplPushRecImpl::NewL(MLbsSuplPushRecObserver& aObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecImpl::NewL() Begin\n");
	CLbsSuplPushRecImpl* newObj = new (ELeave) CLbsSuplPushRecImpl();
	CleanupStack::PushL(newObj);
	newObj->ConstructL(aObserver);
	CleanupStack::Pop(newObj);
	LBSLOG(ELogP1, "CLbsSuplPushRecImpl::NewL() End\n");
	return newObj;
	}
	
/**
Deletes all channels used. 

@see CLbsSuplPushRec::~CLbsSuplPushRec
*/
CLbsSuplPushRecImpl::~CLbsSuplPushRecImpl()
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecImpl::~CLbsSuplPushRecImpl() Begin\n");
	delete iSmsChannel;
	delete iWapPushChannel;
	LBSLOG(ELogP1, "CLbsSuplPushRecImpl::~CLbsSuplPushRecImpl() End\n");
	}
	

/**
The 2nd phase constructor. Constructs both channels, which start to listen for new messages.

@param aObserver [In] A reference to an observer waiting for SUPL INIT messages.
 
@see CLbsSuplPushRecImpl::NewL
*/
void CLbsSuplPushRecImpl::ConstructL(MLbsSuplPushRecObserver& aObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecImpl::ConstructL() Begin\n");
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	TInt err = rep->Get(KSuplPushAPIKey, category);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(rep);
	
	iSmsChannel = CLbsSuplPushRecChannel::NewL(ELbsSuplPushChannelSMS, aObserver,TSecureId(category));
	iWapPushChannel = CLbsSuplPushRecChannel::NewL(ELbsSuplPushChannelWAP, aObserver, TSecureId(category));
	LBSLOG(ELogP1, "CLbsSuplPushRecImpl::ConstructL() End\n");
	}


/**
Sends a confirmation of receiving a SUPL INIT message.
The function is not intended to report any details about the success or failure
of an MT-LR request or server connection.
It is only to confirm that the SUPL INIT message has been received by the SUPL
Protocol Module.

The current implementation of the SUPL Push API does not use this function, 
so its body is empty. 

@param aChannel  [In] The channel Id.
@param aError    [In] KErrNone if successful. KErrArgument if the structure 
                      or content of the received SUPL INIT message was incorrect. 
@param aReqId    [In] An Id of the request.
@param aReserved [In] Reserved for future use.

@return KErrNone if successful, otherwise another of the system-wide error codes.
	
@see MLbsSuplPushRecObserver::OnSuplInit  
@see CLbsSuplPushRec::SuplInitComplete
*/
TInt CLbsSuplPushRecImpl::SuplInitComplete(TLbsSuplPushChannel /*aChannel*/, 
												TLbsSuplPushRequestId /*aReqId*/, 
												TInt /*aError*/, 
												TInt /*aReserved*/)
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecImpl::ConstructL() Begin\n");
	LBSLOG(ELogP1, "CLbsSuplPushRecImpl::ConstructL() End\n");
	//Intentionally left blank
	return KErrNone;
	}

