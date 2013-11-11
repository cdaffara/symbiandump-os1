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

#include "lbsnetsim.h"
#include "lbsnetsimtest.h"
#include "lbsnetsimtestobserver.h"

#include <lbs/lbsassistancedatabuilderset.h>
#include <e32property.h>

/**
Constructor
*/
EXPORT_C RLbsNetSimTest::RLbsNetSimTest() :
	iObserver(NULL), iNotificationConnect(NULL), iNotificationDisconnect(NULL),
	iNotificationRegisterLcsMoLr(NULL), iNotificationReleaseLcsMoLr(NULL), 
	iNotificationMeasurementReport(NULL), 
	iNotificationMeasurementReportRequestMoreAssitanceData(NULL), iNotificationMeasurementReportControlFailure(NULL), 
	iNotificationMeasurementControlLocation(NULL), iNotificationFacilityLcsMoLrResult(NULL),
	iMessageReleaseLcsLocationNotification(NULL), iMessageStartNetworkLocationRequest(NULL)
	{
	}
	

/**
Destructor
*/
EXPORT_C RLbsNetSimTest::~RLbsNetSimTest()
	{
	}

/** 
Connect to the server

@param aObserver	observer to use for callbacks
*/
EXPORT_C TInt RLbsNetSimTest::ConnectL(MLbsNetSimTestObserver* aObserver)
	{
	TInt err = KErrNone;

	// Attempt to start the server and create a session
	for (TInt x = 0; ((x < 2) && (err == KErrNone)); x++)
		{
		err = CreateSession(KLbsNetSimServerName, TVersion(0,0,0));
		if (err == KErrNone)
			{
			break; // We have a session
			}
		
		// Cant create a session
		if ((err == KErrNotFound) || (err == KErrServerTerminated))
			{
			// We need to start the server
			err = StartServer();
			} // if
		} // for
		
	// Leave if we have no session
	if (err == KErrNone)
		{
		TIpcArgs args(ETestHandler);
		err = SendReceive(EInitialise, args);
	
		// Again if we cant initialise then leave
		if (err == KErrNone)
			{
			iObserver = aObserver;
			if (iObserver != NULL)
				{
				// Create callback handlers
				iNotificationConnect = new(ELeave) CNotificationConnect(this, iObserver);
				iNotificationDisconnect = new(ELeave) CNotificationDisconnect(this, iObserver);
				iNotificationRegisterLcsMoLr = new(ELeave) CNotificationRegisterLcsMoLr(this, iObserver);
				iNotificationReleaseLcsMoLr = new(ELeave) CNotificationReleaseLcsMoLr(this, iObserver);
				iMessageReleaseLcsLocationNotification = new(ELeave) CMessageReleaseLcsLocationNotification(this, iObserver);
				iNotificationMeasurementReport = new(ELeave) CNotificationMeasurementReport(this, iObserver);
				iNotificationMeasurementReportRequestMoreAssitanceData = new(ELeave) CNotificationMeasurementReportRequestMoreAssitanceData(this, iObserver);
				iNotificationMeasurementReportControlFailure = new(ELeave) CNotificationMeasurementReportControlFailure(this, iObserver);
				iNotificationMeasurementControlLocation = CNotificationMeasurementControlLocation::NewL(this, iObserver);
				iNotificationFacilityLcsMoLrResult = new(ELeave) CNotificationFacilityLcsMoLrResult(this, iObserver);
				iMessageStartNetworkLocationRequest = new(ELeave) CMessageStartNetworkLocationRequest(this, iObserver);
				
				// Set-up property for step mode
				err = RProperty::Define(TUid::Uid(KLbsNetSimClient), KLbsStepModeKey, RProperty::EInt, 
								  ECapability_None, ECapability_None);
								  
				if (err == KErrAlreadyExists)
					err = KErrNone; //we have already setup NetSim and property before
				}
			}
		}
	
	return err;
	}


/**
Close the connection to the server
*/
EXPORT_C void RLbsNetSimTest::Close()
	{
	// Cleanup here as they are created in the connect
	delete iNotificationConnect;
	delete iNotificationDisconnect;
	delete iNotificationRegisterLcsMoLr;
	delete iNotificationReleaseLcsMoLr;		
	delete iMessageReleaseLcsLocationNotification;
	delete iNotificationMeasurementReport;
	delete iNotificationMeasurementReportRequestMoreAssitanceData;
	delete iNotificationMeasurementReportControlFailure;
	delete iNotificationMeasurementControlLocation;
	delete iNotificationFacilityLcsMoLrResult;
	delete iMessageStartNetworkLocationRequest;
	
	RSessionBase::Close();
	}


/**
Set the reference location to use

The reference location is passed to the assitance data supply module when 
retrieving assistance data.

@param aLocation the reference location
*/
EXPORT_C TBool RLbsNetSimTest::SetReferenceLocation(TPosition& aPosition)
	{
	TPckg<TPosition> positionPkg(aPosition);
	
	TIpcArgs args(&positionPkg);
	return (SendReceive(ETSetReferenceLocation, args) == KErrNone);
	}


/**
Set the reference location to use

The reference location is passed to the assitance data supply module when 
retrieving assistance data.

@param aLocation the reference location
*/
EXPORT_C TBool RLbsNetSimTest::SetReferenceLocation(TDesC8& aLocation)
	{
	TIpcArgs args(&aLocation);
	return (SendReceive(ETSetReferenceLocationData, args) == KErrNone);
	}


/**
Set the time taken to respond to a request

@param aTime 	the time in milliseconds to wait before responding
@param aSticky	continue using this delay until further notice
*/
EXPORT_C TBool RLbsNetSimTest::SetResponseTime(TInt32 aTime, TBool aSticky)
	{
	TIpcArgs args(aTime, aSticky);
	return (SendReceive(ETSetResponseTime, args) == KErrNone);
	}


/**
Set the error to respond to a request

@param aError 	any error value
@param aSticky	continue using this delay until further notice
*/
EXPORT_C TBool RLbsNetSimTest::SetResponseError(TInt32 aError, TBool aSticky)
	{
	TIpcArgs args(aError, aSticky);
	return (SendReceive(ETSetResponseError, args) == KErrNone);
	}


/**
Set the default assitance data returned

@param aFilter the filter
*/
EXPORT_C TBool RLbsNetSimTest::SetDefaultAssitanceDataFilter(TLbsAssistanceDataGroup& aFilter)
	{
	TPckg<TLbsAssistanceDataGroup> filterPkg(aFilter);
	TIpcArgs args(&filterPkg);
	return (SendReceive(ETSetDefaultAssitanceDataFilter, args) == KErrNone);
	}


/**
Set the default assitance data returned if more assitance data is required

@param aFilter the filter
*/
EXPORT_C TBool RLbsNetSimTest::SetMoreAssitanceDataFilter(TLbsAssistanceDataGroup& aFilter)
	{
	TPckg<TLbsAssistanceDataGroup> filterPkg(aFilter);
	TIpcArgs args(&filterPkg);
	return (SendReceive(ETSetMoreDefaultAssitanceDataFilter, args) == KErrNone);
	}


/**
Clear the filters back to there default states
*/
EXPORT_C TBool RLbsNetSimTest::ClearAssistanceDataFilters()
	{
	return (SendReceive(ETClearAssitanceDataFilters) == KErrNone);
	}


/**
Select which assitance data provider to use

@param aProvider the TUid of the provider to use
*/
EXPORT_C TBool RLbsNetSimTest::SetAssistanceDataProvider(TUid aProvider)
	{
	
	TIpcArgs args(aProvider.iUid);
	return (SendReceive(ETSetAssistanceDataProvider, args) == KErrNone);
	}


/**
Set the emergeny status of the phone

@param aEmergency ETrue if the phone should be in emergeny status
*/
EXPORT_C TBool RLbsNetSimTest::SetEmergenyStatus(TBool aEmergency)
	{
	TIpcArgs args(aEmergency);
	return (SendReceive(ETSetEmergenyStatus, args) == KErrNone);
	}


/**
Set the roaming status of the phone

@param aRoaming ETrue if the phone should be in roaming status
*/
EXPORT_C TBool RLbsNetSimTest::SetRoamingStatus(TBool aRoaming)
	{
	TIpcArgs args(aRoaming);
	return (SendReceive(ETSetRoamingStatus, args) == KErrNone);
	}


/**
Set the quality

@param aQuality The quality of the data
*/
EXPORT_C TBool RLbsNetSimTest::SetQuality(TLbsNetPosRequestQuality& aQuality)
	{
	TPckg<TLbsNetPosRequestQuality> qualityPkg(aQuality);
	TIpcArgs args(&qualityPkg);
	return (SendReceive(ETSetRequestQuality, args) == KErrNone);
	}

EXPORT_C TBool RLbsNetSimTest::SendResetAssistanceData(TLbsAssistanceDataGroup aMask)
	{
	TIpcArgs args(aMask);
	return (SendReceive(ETSendResetAssistanceData, args) == KErrNone);
	}



/**
Start a network privacy request

@param aType
@param aRequest
*/
EXPORT_C void RLbsNetSimTest::StartNetworkPrivacyRequest(TLbsNetPosRequestPrivacy aType, 
														 TLbsExternalRequestInfo& aRequest)
	{
	if (iObserver != NULL)
		{
		iMessageReleaseLcsLocationNotification->Send(aType, aRequest);
		}
	}


/**
Cancel an outstanding network privacy request


*/
EXPORT_C void RLbsNetSimTest::CancelNetworkPrivacyRequest()
	{
	if (iObserver != NULL)
		{
		iMessageReleaseLcsLocationNotification->Cancel();
		}
	}


/**
Start a network location request
*/
EXPORT_C void RLbsNetSimTest::StartNetworkLocationRequest()
	{
	if (iObserver != NULL)
		{
		iMessageStartNetworkLocationRequest->Send();	
		}
	}


/**
Cancels an outstanding network location request

*/
EXPORT_C void RLbsNetSimTest::CancelNetworkLocationRequest()
	{
	if (iObserver != NULL)
		{
		iMessageStartNetworkLocationRequest->Cancel();
		}
	}

	
/**
Enable the simulators step mode

@param aStepMode ETrue to enter step mode
*/
EXPORT_C TBool RLbsNetSimTest::SetStepMode(TBool aStepMode)
	{
	if (iObserver != NULL)
		{
		TIpcArgs args(aStepMode);
		return (SendReceive(ETSetStepMode, args) == KErrNone);
		}
	else
		{
		return EFalse;
		}
	}


/**
Enable the simulator to peform the next step

@param aError The error to use, KErrNone for normal behaviour

*/
EXPORT_C TBool RLbsNetSimTest::Next(TInt aError)
	{
	if (iObserver != NULL)
		{
		RProperty::Set(TUid::Uid(KLbsNetSimClient), KLbsStepModeKey, aError);
	
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
	
	
//
// Private methods

/**
*/
TInt RLbsNetSimTest::StartServer()
	{
	RProcess server;

 	// Create the process/server
 	TInt err = server.Create(KLBSNetSimServerBinary, KNullDesC);

 	if (err == KErrNone)
 		{
		TRequestStatus status;
 	
 		// Start the server
 		server.Rendezvous(status);
 		if (status != KRequestPending)
 			{
 			server.Kill(KErrNone);
 			} // if
 		else
 			{
 			server.Resume();
 			} // else
 			
	 	// Wait for server to start
	 	User::WaitForRequest(status);
	 		
	 	// Result
	 	err = (server.ExitType() == EExitPanic) ? KErrGeneral : status.Int();
 			
	 	// Cleanup
	 	server.Close();
 		} // if
 		
 	return err;
	}

//
// CNotifications: CNotificationConnect
RLbsNetSimTest::CNotificationConnect::CNotificationConnect(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);	

	TIpcArgs args(ENotificationConnect);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}
	
RLbsNetSimTest::CNotificationConnect::~CNotificationConnect()
	{
	Cancel();
	}

void RLbsNetSimTest::CNotificationConnect::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->Connected();
	
		TIpcArgs args(ENotificationConnect);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		} // if
	}

void RLbsNetSimTest::CNotificationConnect::DoCancel()
	{
	TIpcArgs args(ENotificationConnectCancel);
	iTest->Send(ECallbacks, args);
	}
	

//
// CNotifications: CNotificationDisconnect
RLbsNetSimTest::CNotificationDisconnect::CNotificationDisconnect(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);	

	TIpcArgs args(ENotificationDisconnect);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}
	
RLbsNetSimTest::CNotificationDisconnect::~CNotificationDisconnect()
	{
	Cancel();
	}
	

void RLbsNetSimTest::CNotificationDisconnect::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->Disconnected();
		
		TIpcArgs args(ENotificationDisconnect);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		} // if
	}

void RLbsNetSimTest::CNotificationDisconnect::DoCancel()
	{
	TIpcArgs args(ENotificationDisconnectCancel);
	iTest->Send(ECallbacks, args);
	}	
	

//
// CNotifications: CNotificationRegisterLcsMoLr
RLbsNetSimTest::CNotificationRegisterLcsMoLr::CNotificationRegisterLcsMoLr(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);	

	TIpcArgs args(ENotificationRegisterLcsMoLr, &iData);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}
	
RLbsNetSimTest::CNotificationRegisterLcsMoLr::~CNotificationRegisterLcsMoLr()
	{
	Cancel();
	}
	

void RLbsNetSimTest::CNotificationRegisterLcsMoLr::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->NotifyRegisterLcsMoLr(iData);
		
		TIpcArgs args(ENotificationRegisterLcsMoLr, &iData);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		} // if
	}

void RLbsNetSimTest::CNotificationRegisterLcsMoLr::DoCancel()
	{
	TIpcArgs args(ENotificationRegisterLcsMoLrCancel);
	iTest->Send(ECallbacks, args);
	}		
	

//
// CNotifications: CNotificationReleaseLcsMoLr
//
// Priority of this object is low as it should be the last notification fired if more than
// one notification is active at the same time
RLbsNetSimTest::CNotificationReleaseLcsMoLr::CNotificationReleaseLcsMoLr(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityMuchLess), iTest(aTest), iObserver(aObserver), iReasonPkg(iReason)
	{
	CActiveScheduler::Add(this);	

	TIpcArgs args(ENotificationReleaseLcsMoLr, &iReasonPkg);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}
	
RLbsNetSimTest::CNotificationReleaseLcsMoLr::~CNotificationReleaseLcsMoLr()
	{
	Cancel();
	}
	

void RLbsNetSimTest::CNotificationReleaseLcsMoLr::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->NotifyReleaseLcsMoLr(iReason);
		
		TIpcArgs args(ENotificationReleaseLcsMoLr, &iReasonPkg);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		} // if
	}

void RLbsNetSimTest::CNotificationReleaseLcsMoLr::DoCancel()
	{
	TIpcArgs args(ENotificationReleaseLcsMoLrCancel);
	iTest->Send(ECallbacks, args);
	}		
//
// CNotifications: CNotificationMeasurementReport
RLbsNetSimTest::CNotificationMeasurementReport::CNotificationMeasurementReport(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver), iPositionPkg(iPosition)
	{
	CActiveScheduler::Add(this);	

	TIpcArgs args(ENotificationMeasurementReport, &iPositionPkg);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}
	
RLbsNetSimTest::CNotificationMeasurementReport::~CNotificationMeasurementReport()
	{
	Cancel();
	}
	

void RLbsNetSimTest::CNotificationMeasurementReport::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->NotifyMeasurementReportLocation(iPosition);
		
		TIpcArgs args(ENotificationMeasurementReport, &iPositionPkg);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		} // if
	}

void RLbsNetSimTest::CNotificationMeasurementReport::DoCancel()
	{
	TIpcArgs args(ENotificationMeasurementReportCancel);
	iTest->Send(ECallbacks, args);
	}
	
//
// CNotifications: CNotificationMeasurementReportRequestMoreAssitanceData
RLbsNetSimTest::CNotificationMeasurementReportRequestMoreAssitanceData::CNotificationMeasurementReportRequestMoreAssitanceData(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver), iFilterPkg(iFilter)
	{
	CActiveScheduler::Add(this);	

	TIpcArgs args(ENotificationMeasurementReportRequestMoreAssitanceData, &iFilterPkg);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}
	
RLbsNetSimTest::CNotificationMeasurementReportRequestMoreAssitanceData::~CNotificationMeasurementReportRequestMoreAssitanceData()
	{
	Cancel();
	}
	

void RLbsNetSimTest::CNotificationMeasurementReportRequestMoreAssitanceData::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->NotifyMeasurementReportRequestMoreAssistanceData(iFilter);
		
		TIpcArgs args(ENotificationMeasurementReportRequestMoreAssitanceData, &iFilterPkg);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		} // if
	}

void RLbsNetSimTest::CNotificationMeasurementReportRequestMoreAssitanceData::DoCancel()
	{
	TIpcArgs args(ENotificationMeasurementReportRequestMoreAssitanceDataCancel);
	iTest->Send(ECallbacks, args);
	}	
	
//
// CNotifications: CNotificationMeasurementReportControlFailure
RLbsNetSimTest::CNotificationMeasurementReportControlFailure::CNotificationMeasurementReportControlFailure(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver), iReasonPkg(iReason)
	{
	CActiveScheduler::Add(this);	

	TIpcArgs args(ENotificationMeasurementReportControlFailure, &iReasonPkg);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}
	
RLbsNetSimTest::CNotificationMeasurementReportControlFailure::~CNotificationMeasurementReportControlFailure()
	{
	Cancel();
	}
	

void RLbsNetSimTest::CNotificationMeasurementReportControlFailure::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->NotifyMeasurementReportControlFailure(iReason);
		
		TIpcArgs args(ENotificationMeasurementReportControlFailure, &iReasonPkg);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		} // if
	}

void RLbsNetSimTest::CNotificationMeasurementReportControlFailure::DoCancel()
	{
	TIpcArgs args(ENotificationMeasurementReportControlFailureCancel);
	iTest->Send(ECallbacks, args);
	}		
	
//
// CNotifications: CNotificationMeasurementControlLocation
RLbsNetSimTest::CNotificationMeasurementControlLocation* RLbsNetSimTest::CNotificationMeasurementControlLocation::NewL(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver)
	{
	CNotificationMeasurementControlLocation* self = new (ELeave) CNotificationMeasurementControlLocation(aTest, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}
	
void RLbsNetSimTest::CNotificationMeasurementControlLocation::ConstructL()
	{
	iAssistanceData = HBufC8::NewL(RLbsAssistanceDataBuilderSet::MaxExternalizedBufferSize());
	iPtr.Set(iAssistanceData->Des());

	TIpcArgs args(ENotificationMeasurementControlLocation, &iPositionPkg, &iQualityPkg, &iPtr);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}

RLbsNetSimTest::CNotificationMeasurementControlLocation::CNotificationMeasurementControlLocation(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver), 
	iPositionPkg(iPosition), iQualityPkg(iQuality), iPtr(NULL, 0)
	{
	CActiveScheduler::Add(this);	
	}
	
RLbsNetSimTest::CNotificationMeasurementControlLocation::~CNotificationMeasurementControlLocation()
	{
	Cancel();
	delete iAssistanceData;
	}
	

void RLbsNetSimTest::CNotificationMeasurementControlLocation::RunL()
	{
	if (iStatus == KErrNone)
		{
		// Read the assistance data back out
		RLbsAssistanceDataBuilderSet assistanceData;
		CleanupClosePushL(assistanceData);
		assistanceData.OpenL();
		RDesReadStream stream(iPtr);
		CleanupClosePushL(stream);
		assistanceData.InternalizeL(stream);
		
		iObserver->NotifyMeasurementControlLocation(iPosition, assistanceData, iQuality);
		
		
		TIpcArgs args(ENotificationMeasurementControlLocation, &iPositionPkg, &iQualityPkg, &iPtr);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		
		// Cleanup
		CleanupStack::PopAndDestroy(2);
		} // if
	}

void RLbsNetSimTest::CNotificationMeasurementControlLocation::DoCancel()
	{
	TIpcArgs args(ENotificationMeasurementControlLocationCancel);
	iTest->Send(ECallbacks, args);
	}		
		
//
// CNotifications: CNotificationFacilityLcsMoLrResult
RLbsNetSimTest::CNotificationFacilityLcsMoLrResult::CNotificationFacilityLcsMoLrResult(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver), iReasonPkg(iReason), iPositionPkg(iPosition)
	{
	CActiveScheduler::Add(this);	
	
	TIpcArgs args(ENotificationFacilityLcsMoLrResult, &iReasonPkg, &iPositionPkg);
	iTest->SendReceive(ECallbacks, args, iStatus);
	SetActive();
	}
	
RLbsNetSimTest::CNotificationFacilityLcsMoLrResult::~CNotificationFacilityLcsMoLrResult()
	{
	Cancel();
	}
	

void RLbsNetSimTest::CNotificationFacilityLcsMoLrResult::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->NotifyFacilityLcsMoLrResult(iReason, iPosition);
		
		TIpcArgs args(ENotificationFacilityLcsMoLrResult, &iReasonPkg, &iPositionPkg);
		iTest->SendReceive(ECallbacks, args, iStatus);
		SetActive();
		} // if
	}

void RLbsNetSimTest::CNotificationFacilityLcsMoLrResult::DoCancel()
	{
	TIpcArgs args(ENotificationFacilityLcsMoLrResultCancel);
	iTest->Send(ECallbacks, args);
	}
//
// CNotifications: CMessageReleaseLcsLocationNotification
RLbsNetSimTest::CMessageReleaseLcsLocationNotification::CMessageReleaseLcsLocationNotification(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver), iTypePkg(iType),
	iRequestInfoPkg(iRequestInfo), iResponsePkg(iResponse)
	{
	CActiveScheduler::Add(this);	
	}
	
RLbsNetSimTest::CMessageReleaseLcsLocationNotification::~CMessageReleaseLcsLocationNotification()
	{
	Cancel();
	}

void RLbsNetSimTest::CMessageReleaseLcsLocationNotification::Send(TLbsNetPosRequestPrivacy& aType, TLbsExternalRequestInfo& aRequestInfo)
	{
	iType = aType;
	iRequestInfo = aRequestInfo;
	TIpcArgs args(&iTypePkg, &iRequestInfoPkg, &iResponsePkg);
	iTest->SendReceive(ETReleaseLcsLocationNotification, args, iStatus);
	SetActive();
	}

void RLbsNetSimTest::CMessageReleaseLcsLocationNotification::RunL()
	{
	if (iStatus == KErrNone)
		{
		iObserver->NotifyReleaseLcsLocationNotification(iResponse);
		} // if
	else
		{
		iObserver->NotifyError(MLbsNetSimTestObserver::EStartNetworkPrivacyRequest, iStatus.Int());
		}
	}

void RLbsNetSimTest::CMessageReleaseLcsLocationNotification::DoCancel()
	{
	iTest->Send(ETReleaseLcsLocationNotificationCancel);
	}		
//
// CNotifications: CMessageReleaseLcsLocationNotification
RLbsNetSimTest::CMessageStartNetworkLocationRequest::CMessageStartNetworkLocationRequest(RLbsNetSimTest* aTest, MLbsNetSimTestObserver* aObserver) :
	CActive(EPriorityStandard), iTest(aTest), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);	
	}
	
RLbsNetSimTest::CMessageStartNetworkLocationRequest::~CMessageStartNetworkLocationRequest()
	{
	Cancel();
	}

void RLbsNetSimTest::CMessageStartNetworkLocationRequest::Send()
	{
	iTest->SendReceive(ETStartNetworkLocationRequest, iStatus);
	SetActive();
	}

void RLbsNetSimTest::CMessageStartNetworkLocationRequest::RunL()
	{
	if (iStatus != KErrNone)
		{
		iObserver->NotifyError(MLbsNetSimTestObserver::EStartNetworkLocationRequest, iStatus.Int());
		} // if
	}

void RLbsNetSimTest::CMessageStartNetworkLocationRequest::DoCancel()
	{
	iTest->Send(ETStartNetworkLocationRequestCancel);
	}		
