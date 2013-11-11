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

#include <e32std.h>
#include <lbscommon.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsprivacycontroller.h>
#include "lbsprivacycontrollerimpl.h"
#include "lbsdevloggermacros.h"
#include "lbsnrhserverdata.h"


//-----------------------------------------------------------------------------
// MLbsPrivacyObserver
//-----------------------------------------------------------------------------	

/**
Returns the current version of the interface.

@return The current version of the interface.
*/
EXPORT_C TVersion MLbsPrivacyObserver::Version()
	{
	LBSLOG(ELogP9, "<-S MLbsPrivacyObserver::Version() lbsnetworkrequesthandler.exe\n");
	LBSLOG4(ELogP9, "  Return TVersion  = (%d, %d, %d)\n", 
					KLbsPrivacyObserverVersionMajor,
					KLbsPrivacyObserverVersionMinor,
					KLbsPrivacyObserverVersionBuild);
	return TVersion(KLbsPrivacyObserverVersionMajor, 
			KLbsPrivacyObserverVersionMinor, 
			KLbsPrivacyObserverVersionBuild); 

	}

//-----------------------------------------------------------------------------
// MLbsPrivacyObserver2
//-----------------------------------------------------------------------------	

/**
Returns the current version of the interface.

@return The current version of the interface.
*/
EXPORT_C TVersion MLbsPrivacyObserver2::Version()
	{
	LBSLOG(ELogP9, "<-S MLbsPrivacyObserver2::Version() lbsnetworkrequesthandler.exe\n");
	LBSLOG4(ELogP9, "  Return TVersion  = (%d, %d, %d)\n", 
					KLbsPrivacyObserverVersion2Major,
					KLbsPrivacyObserverVersion2Minor,
					KLbsPrivacyObserverVersion2Build);
	return TVersion(KLbsPrivacyObserverVersion2Major, 
			KLbsPrivacyObserverVersion2Minor, 
			KLbsPrivacyObserverVersion2Build); 
	}

//-----------------------------------------------------------------------------
// CLbsPrivacyController
//-----------------------------------------------------------------------------	

/**
Standard Symbian OS two-phase constructor.

@param aObserver Reference to an observer that will be notified when a network
                 location request is recieved by the LBS system.
@return A new CLbsPrivacyController object.

@capability ECapabilityLocation, ECapabilityReadDeviceData
*/
EXPORT_C CLbsPrivacyController* CLbsPrivacyController::NewL(MLbsPrivacyObserver& aObserver)
	{
	LBSLOG(ELogP9, "->S CLbsPrivacyController::NewL() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > MLbsPrivacyObserver aObserver  = 0x%08X\n", &aObserver);
	CLbsPrivacyController* self = new (ELeave) CLbsPrivacyController;
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	CleanupStack::Pop();
	return self;
	}
	
/**
Standard ConstructL for CPrivacyController. It creates the object that
implements the functionality for CLbsPrivacyObserver.
@internalComponent
@param aObserver Reference to an observer that will be notified when a network
                 location request is recieved by the LBS system.
*/
void CLbsPrivacyController::ConstructL(MLbsPrivacyObserver& aObserver)
	{
	iImpl = CLbsPrivacyControllerImpl::NewL(&aObserver);
	}

/**
Standard C++ constructor.
*/
EXPORT_C CLbsPrivacyController::CLbsPrivacyController()
	{
	}
	
/**
Standard C++ destructor
@internalComponent
*/
CLbsPrivacyController::~CLbsPrivacyController()
	{
	LBSLOG(ELogP9, "->S CLbsPrivacyController::~CLbsPrivacyController() lbsnetworkrequesthandler.exe\n");
	delete iImpl;
	}

/**
The application that is listening to network location requests must use this
function to give the LBS system the response to the location request.

The application is first notified of a network location request when 
MLbsObserver::ProcessNetworkLocationRequest is called. Then it can ask the
user for verification if needed, or decide on its own. It must then use 
this function to tell the LBS system whether to accept or reject the network
location request.

The response can be one of the values of TRequestVerificationResult, usually
it will either be ERequestAccepted or ERequestRejected.

@see CLbsPrivacyController::TRequestVerificationResult
@see MLbsPrivacyObserver::ProcessNetworkLocationRequest

@param aRequestId Identifies the request within the LBS system.
@param aResult The response to the network location request.

@capability ECapabilityLocation, ECapabilityWriteDeviceData
*/
EXPORT_C void CLbsPrivacyController::RespondNetworkLocationRequest(TUint aRequestId, 
											TRequestVerificationResult aResult)
	{
	LBSLOG(ELogP9, "->A CLbsPrivacyController::RespondNetworkLocationRequest() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TUint aRequestId  = %u\n", aRequestId);
	LBSLOG2(ELogP9, "  > TRequestVerificationResult aResult  = %d\n", aResult);

	iImpl->RespondNetworkLocationRequest(aRequestId, aResult);
	}

/**
Force the LBS system to cancel a currently running network location request, or 
to reject a request which has not yet been answered.

If this function is called after CLbsPrivacyController::RespondNetworkLocationRequest()
then the LBS system will send a KErrCancel error in the next network 
Measurement Control message.

If this function is called before the request has been accepted 
(by a call to CLbsPrivacyController::RespondNetworkLocationRequest()) the 
effect is the same as a call to 
CLbsPrivacyController::RespondNetworkLocationRequest() with the value 
ERequestRejected.

If aRequestId is not a currently active external location request then the 
call is ignored.

@param aRequestId The Id of the network location request (MT-LR) to cancel.

@capability ECapabilityLocation, ECapabilityWriteDeviceData
*/
EXPORT_C void CLbsPrivacyController::CancelNetworkLocationRequest(TUint aRequestId)
	{
	LBSLOG(ELogP9, "->A CLbsPrivacyController::CancelNetworkLocationRequest() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TUint aRequestId  = %u\n", aRequestId);

	iImpl->CancelNetworkLocationRequest(aRequestId);
	}

