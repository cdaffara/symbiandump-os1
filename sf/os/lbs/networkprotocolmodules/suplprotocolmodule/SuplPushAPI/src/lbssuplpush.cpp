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
// Location Based Services SUPL Push API
// 
//

/**
 @file
 @deprecated
*/



#include <e32base.h>
#include <lbs/lbssuplpush.h>
#include "lbsdevloggermacros.h"

#include "lbssuplpushimpl.h"


//=============================================================================
// MLbsSuplPushObserver
//=============================================================================
/**
Provides the version of the observer interface. This can be used by the library 
to determine the functionality supported by the client application.

@return Version of the observer interface.
*/  
EXPORT_C TVersion MLbsSuplPushObserver::Version() const
	{
	LBSLOG(ELogP1, "MLbsSuplPushObserver::Version() Begin\n");
	LBSLOG(ELogP1, "MLbsSuplPushObserver::Version() End\n");
	return TVersion(1, 0, 0);
	}


/**
Should not be used. Provides for future expansion of the observer interface. 
*/  
/*virtual*/ EXPORT_C TAny* MLbsSuplPushObserver::ExtendedInterface
	(
	TInt  /*aFunctionNumber*/,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	LBSLOG(ELogP1, "MLbsSuplPushObserver::ExtendedInterface() Begin\n");
	LBSLOG(ELogP1, "MLbsSuplPushObserver::ExtendedInterface() End\n");
	return NULL;
	}


/**
Opens a new SUPL Push channel to the LBS sub-system.

@param aChannel  [In] The id of the channel to be opened.
@param aObserver [In] A reference to an observer waiting for request completion call-backs.

@return an Instance of the interface. The calling application becomes the
owner of the returned instance and is responsible for its disposal.

@capability NetworkServices
@capability ReadDeviceData
*/
EXPORT_C CLbsSuplPush* CLbsSuplPush::NewL(TLbsSuplPushChannel aChannel, MLbsSuplPushObserver& aObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPush::NewL() Begin\n");
	LBSLOG(ELogP8, "->S CLbsSuplPush::NewL() SuplPush\n");
	CLbsSuplPush* newObj = new (ELeave) CLbsSuplPush();
	CleanupStack::PushL(newObj);
	newObj->ConstructL(aChannel, aObserver);
	LBSLOG(ELogP1, "CLbsSuplPush::NewL() End\n");
	CleanupStack::Pop(newObj);
	return newObj;
	}
	
/**
Closes the interface and disposes all open or used resources.
*/
CLbsSuplPush::~CLbsSuplPush()
	{
	LBSLOG(ELogP1, "CLbsSuplPush::~CLbsSuplPush() Begin\n");
	LBSLOG(ELogP8, "->S CLbsSuplPush::~CLbsSuplPush() SuplPush\n");
	delete iImpl;
	iImpl = NULL;
	LBSLOG(ELogP1, "CLbsSuplPush::~CLbsSuplPush() End\n");
	}
	
/**
Default constructor.
@internalComponent
*/
CLbsSuplPush::CLbsSuplPush()
	{
	LBSLOG(ELogP1, "CLbsSuplPush::CLbsSuplPush() Begin\n");
	LBSLOG(ELogP1, "CLbsSuplPush::CLbsSuplPush() End\n");
	}

/**
2nd phase constructor. 
Creates and assigns all the required internal resources.

@param aChannel  The id of the channel to be opened.
@param aObserver A reference to an observer waiting for request completion call-backs.

@internalComponent
*/
void CLbsSuplPush::ConstructL(TLbsSuplPushChannel aChannel, MLbsSuplPushObserver& aObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPush::ConstructL() Begin\n");
	iImpl = CLbsSuplPushImpl::NewL(aChannel, aObserver);
	LBSLOG(ELogP1, "CLbsSuplPush::ConstructL() End\n");
	}

/**
Sends a SUPL INIT message to the LBS sub-system. If the channel is busy
sending a previous message the new one will be queued. The request completion code 
is received through a call-back.

Please note that only one thread can actively use a channel at any time. Simultaneous
use of a channel by multiple threads may cause some of the requests to time out.

A system requiring concurrent access to a channel must provide its own access 
control mechanism.

@param aReqId [Out] Channel unique request Id.
@param aMsg   [In]  A buffer containing a SUPL INIT message.
@param aReserved [In] Reserved for future use.

@return An error code related to the synchronous part of the request - KErrNone if successful, KErrArgument if 
the message has a wrong size, KErrNotReady if the LBS can't be started, KErrPermissionDenied if the calling process
has not enough capabilities, or another system error code.

@see MLbsSuplPushObserver::OnSuplInitComplete()

@capability NetworkServices
@capability ReadDeviceData
*/
EXPORT_C TInt CLbsSuplPush::SuplInit(TLbsSuplPushRequestId& aReqId, const TDesC8& aMsg, TInt aReserved)
	{
	LBSLOG(ELogP1, "CLbsSuplPush::SuplInit() Begin\n");
	LBSLOG(ELogP9, "->A CLbsSuplPush::SuplInit() SuplPush\n");
	
	TInt res = iImpl->SuplInit(aReqId, aMsg, aReserved);
	
	LBSLOG2(ELogP9, "  < TLbsSuplPushRequestId& aReqId = %d\n", aReqId);
	LBSLOG2(ELogP9, "  Return  = %d\n", res);
	LBSLOG(ELogP1, "CLbsSuplPush::SuplInit() End\n");
	return res;
	}
