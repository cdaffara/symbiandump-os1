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
// The receiver part of the SUPL Push API.
// 
//

#include <e32base.h>
#include <lbs/lbssuplpushreceiver.h>
#include <lbs/lbssuplpushcommon.h>
#include "lbsdevloggermacros.h"

#include "lbssuplpushreceiverimpl.h"

//=============================================================================
// MLbsSuplPushRecObserver
//=============================================================================
/**
Provides the version of the observer interface. This can be used by the library 
to determine the functionality supported by the client application.

@return Version of the observer interface.
*/  
EXPORT_C TVersion MLbsSuplPushRecObserver::Version() const
	{
	LBSLOG(ELogP1, "MLbsSuplPushRecObserver::Version() Begin\n");
	LBSLOG(ELogP1, "MLbsSuplPushRecObserver::Version() End\n");
	return TVersion(1, 0, 0);
	}


/**
Should not be used. Provides for future expansion of the observer interface. 
*/  
/*virtual*/ EXPORT_C TAny* MLbsSuplPushRecObserver::ExtendedInterface
	(
	TInt  /*aFunctionNumber*/,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	LBSLOG(ELogP1, "MLbsSuplPushRecObserver::Version() Begin\n");
	LBSLOG(ELogP1, "MLbsSuplPushRecObserver::Version() End\n");
	return NULL;
	}


//=============================================================================
// CLbsSuplPushRec
//=============================================================================
/**
Opens all the SUPL Push channels and starts listening for incoming SUPL INIT messages.

@param aObserver [In] A reference to an observer waiting for SUPL INIT messages.

@return an Instance of the interface. The calling application becomes the
owner of the returned instance and is responsible its disposal.

@capability NetworkServices
@capability ReadDeviceData
*/
EXPORT_C CLbsSuplPushRec* CLbsSuplPushRec::NewL(MLbsSuplPushRecObserver& aObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPushRec::NewL() Begin\n");
	LBSLOG(ELogP8, "->S CLbsSuplPushRec::NewL() SuplPush\n");
	CLbsSuplPushRec* newObj = new (ELeave) CLbsSuplPushRec();
	CleanupStack::PushL(newObj);
	newObj->ConstructL(aObserver);
	CleanupStack::Pop(newObj);
	LBSLOG(ELogP1, "CLbsSuplPushRec::NewL() End\n");
	return newObj;
	}
	
/**
Closes the interface and disposes all open or used resources.
*/
CLbsSuplPushRec::~CLbsSuplPushRec()
	{
	LBSLOG(ELogP1, "CLbsSuplPushRec::~CLbsSuplPushRec() Begin\n");
	LBSLOG(ELogP8, "->S CLbsSuplPushRec::~CLbsSuplPushRec() SuplPush\n");
	delete iImpl;
	iImpl = NULL;
	LBSLOG(ELogP1, "CLbsSuplPushRec::~CLbsSuplPushRec() End\n");
	}
	
/**
Default constructor.
@internalComponent
*/
CLbsSuplPushRec::CLbsSuplPushRec()
	{
	LBSLOG(ELogP1, "CLbsSuplPushRec::CLbsSuplPushRec() Begin\n");
	LBSLOG(ELogP1, "CLbsSuplPushRec::CLbsSuplPushRec() End\n");
	}

/**
2nd phase constructor. 
Creates and assigns all the required internal resources.

@param aObserver [In] A reference to an observer waiting for SUPL INIT messages.

@internalComponent
*/
void CLbsSuplPushRec::ConstructL(MLbsSuplPushRecObserver& aObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPushRec::ConstructL() Begin\n");
	iImpl = CLbsSuplPushRecImpl::NewL(aObserver);
	LBSLOG(ELogP1, "CLbsSuplPushRec::ConstructL() End\n");
	}

/**
Sends a confirmation of receiving a SUPL INIT message.
The function is not intended to report any details about the success or failure
of an MT-LR request or server connection.
It is only to confirm that the SUPL INIT message has been received by the SUPL
Protocol Module.

@param aError    [In] KErrNone if successful. KErrArgument if the structure 
                      or content of the received SUPL INIT message was incorrect. 
@param aChannel  [In] The channel Id.
@param aReqId    [In] An Id of the request.
@param aReserved [In] Reserved for future use.

@return KErrNone if successful, otherwise another of the system-wide error codes.
	
@see MLbsSuplPushRecObserver::OnSuplInit()

@capability NetworkServices
@capability ReadDeviceData
*/
EXPORT_C TInt CLbsSuplPushRec::SuplInitComplete(TLbsSuplPushChannel aChannel, 
												TLbsSuplPushRequestId aReqId, 
												TInt aError, 
												TInt aReserved)
	{
	LBSLOG(ELogP1, "CLbsSuplPushRec::SuplInitComplete() Begin\n");
	LBSLOG(ELogP9, "->S CLbsSuplPushRec::SuplInitComplete() SuplPush\n");
	TInt err = iImpl->SuplInitComplete(aChannel, aReqId, aError, aReserved);
	LBSLOG2(ELogP9, "  > TLbsSuplPushChannel aChannel = %d\n", aChannel);
	LBSLOG2(ELogP9, "  > TLbsSuplPushRequestId aReqId = %d\n", aReqId);
	LBSLOG2(ELogP9, "  > TInt aError = %d\n", aError);
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	LBSLOG(ELogP1, "CLbsSuplPushRec::SuplInitComplete() End\n");
	return err;
	}

