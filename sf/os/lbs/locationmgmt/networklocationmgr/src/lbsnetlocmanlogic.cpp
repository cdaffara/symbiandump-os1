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
// Includes
// System
// 
//

#include <e32cons.h>

// Lbs
#include "lbsdevloggermacros.h"

// Component
#include "lbsnetlocmanlogic.h"


//****************************************************************
// Class CNetLocManLogic
//****************************************************************
/** Static constructor.
@released
@internalComponent
*/
CNetLocManLogic* CNetLocManLogic::NewL()
	{
	
	
	CNetLocManLogic* self = new(ELeave) CNetLocManLogic;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	CleanupStack::Pop(self);
	return self;
	};


/** Constructor.
@released
@internalComponent
*/
CNetLocManLogic::CNetLocManLogic()
	{
	}


/** Second phase constructor.
@released
@internalComponent
*/
void CNetLocManLogic::ConstructL()
	{
	
	
	iCurrentId.SetSessionOwner(KLbsNetLocManagerUid);
	iCurrentId.SetSessionNum(0);
	
	// create the monitor to handle the close down request signal from root process
	iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(this, KLbsNetLocManagerUid);
	
	// open the property for position update and module status
	iPositionUpdates.OpenL(KLbsNetLocManagerUid);

	// Set the initial device status, network location manager can only be EDeviceReady or EDeviceActive
	iModuleStatus.OpenL(KLbsNetLocManagerUid);
	iPreviousReportedModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceReady);
	iPreviousReportedModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityNormal);
	TPositionModuleStatusEventBase::TModuleEvent occurredEventsSinceLastSet = 
		TPositionModuleStatusEventBase::EEventDeviceStatus | TPositionModuleStatusEventBase::EEventDataQualityStatus;
	iModuleStatus.SetModuleStatus(&iPreviousReportedModuleStatus,sizeof(iPreviousReportedModuleStatus),occurredEventsSinceLastSet);

	const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierLS = 
		{
			{KLbsNetLocManagerUidValue},{KLbsLocServerUidValue}
		};

	// create netloc request handler and start the active object
	iLocSvrLocationRequestHandler = CNetLocManRequestHandler::NewL(KChannelIdentifierLS, *this); 
	iLocSvrLocationRequestHandler->StartGettingRequests();

	// create netloc response handler and start the active object
	iNetLocationResponseHandler = CNetLocManResponseHandler::NewL(*this); 
	iNetLocationResponseHandler->StartGettingRequests();
	}


/** Destructor.
@released
@internalComponent
*/
CNetLocManLogic::~CNetLocManLogic()
	{
	
	delete iNetLocationResponseHandler;
	iNetLocationResponseHandler = NULL;
	
	delete iLocSvrLocationRequestHandler;
	iLocSvrLocationRequestHandler = NULL;

	delete iCloseDownRequestDetector;
	iCloseDownRequestDetector = NULL;
	}



/** Process version
@released
@internalComponent
*/
TVersion CNetLocManLogic::Version()
	{
	return TVersion(1,0,0);
	}


/** Retrieve the current session id for the component
@param aSessionId A session id to return current id through
@released
@internalComponent
*/
void CNetLocManLogic::GetSessionId(TLbsNetSessionIdInt& aSessionId)
	{
	aSessionId = iCurrentId;
	}


/** Increment the current session
@released
@internalComponent
*/
void CNetLocManLogic::IncrementSessionId()
	{
	iCurrentId.IncrSession();
	}
	
	
/** Publish a position on the position request bus
This function is called from the response handler when it has
received a response from the NetworkGateway. If aStatus is different
from KErrNone an error has ocurred and the NetworkGateway has not
a valid position to the response handler and the information passed
in the other parameters is therefore disregarded. In any other case
the position info received is published.
@param aStatus An errorcode
@param aPosInfoArray A position array
@param aTargetTime The position target time
@see CNetLocManResponseHandler::RunL
@released
@internalComponent
*/
void CNetLocManLogic::UpdateLocation(TInt aStatus, TPositionInfoBase& aPosition, 
										 const TTime& aTargetTime)
	{
	// Module Id and update type
	TPositionModuleId id = {KLbsNetLocManagerUidValue};
	TPositionModuleInfo::TTechnologyType type = TPositionModuleInfo::ETechnologyNetwork;
	
	aPosition.SetModuleId(id);
	aPosition.SetUpdateType(EPositionUpdateGeneral);
	aPosition.SetPositionMode(type);
	aPosition.SetPositionModeReason(EPositionModeReasonNone);

	TInt size = aPosition.PositionClassSize();
	if ((aStatus == KErrServerBusy) || (aStatus == KErrPositionHighPriorityReceive))
		{
		iPositionUpdates.SetPositionInfo(aStatus, ETrue, &aPosition,size,0, 0);
		}
	else if (aStatus != KErrNone)
		{
		iPositionUpdates.SetPositionInfo(aStatus, EFalse, &aPosition,size,0, 0);		
		}
	
	else
		{ 
		TTime timeNow;
		timeNow.UniversalTime();

		// always publish new update regardless
		TInt size = aPosition.PositionClassSize();
		iPositionUpdates.SetPositionInfo(aStatus, EFalse, &aPosition,size,aTargetTime, timeNow);
		}
	// Request has been dealt with... We can say we're 'ready' again.
	UpdateDeviceStatus(TPositionModuleStatus::EDeviceReady);
	}


/** Process close down callback
@see MLbsCloseDownObserver
@released
@internalComponent
*/
void CNetLocManLogic::OnProcessCloseDown()
	{
	iLocSvrLocationRequestHandler->Cancel();
	iNetLocationResponseHandler->Cancel();
	CActiveScheduler::Stop();
	}
	
void CNetLocManLogic::UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus)
	{
	iPreviousReportedModuleStatus.SetDeviceStatus(aDeviceStatus);
	TPositionModuleStatusEventBase::TModuleEvent occurredEventsSinceLastSet = 
		TPositionModuleStatusEventBase::EEventDeviceStatus;
	iModuleStatus.SetModuleStatus(&iPreviousReportedModuleStatus,sizeof(TPositionModuleStatus),occurredEventsSinceLastSet);
	}

