// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32mem.h>
#include "lbsnetsim.h"
#include "lbsnetsimgateway.h"
#include "lbsnetsimgatewayobserver.h"
#include "lbsnetsimtest.h"
#include "lbsdevloggermacros.h"

/**
*/
EXPORT_C RLbsNetSim::RLbsNetSim() :
	iRegisterLcsMoLrData(NULL), 
	iReleaseLcsMoLrReasonPkg(iReleaseLcsMoLrReason),
	iRequestMoreAssistanceDataFilterPkg(iRequestMoreAssistanceDataFilter),
	iReleaseLcsLocationNotificationResponsePkg(iReleaseLcsLocationNotificationResponse),
	iMeasurementReportLocation(NULL), iNotificationMeasurementControlLocation(NULL), 
	iNotificationRegisterLcsLocation(NULL), iNotificationCancelPrivacyRequest(NULL), 
	iNotificationResetAssistanceData(NULL)
	{
	}
	
	
/**
*/
EXPORT_C RLbsNetSim::~RLbsNetSim()
	{
	delete iRegisterLcsMoLrData;
	}


/**
*/
EXPORT_C TInt RLbsNetSim::ConnectL(MLbsNetSimObserver* aObserver)
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
		// Initialise the session to the correct type.
		TIpcArgs args(EGatewayHandler);
		err = SendReceive(EInitialise, args);
		if (err == KErrNone)
			{
			iObserver = aObserver;
			
			// Create message handlers
			iMeasurementReportLocation = new (ELeave) CMeasurementReportLocation(this, iObserver);
			
			// Create notifications
			iNotificationMeasurementControlLocation = CNotificationMeasurementControlLocation::NewL(this, iObserver);
			iNotificationRegisterLcsLocation = new (ELeave) CNotificationRegisterLcsLocation(this, iObserver);
			iNotificationCancelPrivacyRequest = new (ELeave) CNotificationCancelPrivacyRequest(this, iObserver);
			iNotificationNetworkGone = new (ELeave) CNotificationNetworkGone(this, iObserver);
			iNotificationResetAssistanceData = new (ELeave) CNotificationResetAssistanceData(this, iObserver);
			}
		}
		
	return err;		
	}


/**
*/
EXPORT_C void RLbsNetSim::Close()
	{
	// Cleanup here as they are created in Connect
	delete iMeasurementReportLocation;
	delete iNotificationMeasurementControlLocation;
	delete iNotificationRegisterLcsLocation;
	delete iNotificationCancelPrivacyRequest;
	delete iNotificationNetworkGone;
	delete iNotificationResetAssistanceData;
	
	RSessionBase::Close();
	}


/**
*/
EXPORT_C void RLbsNetSim::RegisterLcsMoLrL(const TDesC& aData)
	{
	LBSLOG(ELogP1, "RLbsNetSim::RegisterLcsMoLrL()");
	if (iRegisterLcsMoLrData != NULL)
		{
		delete iRegisterLcsMoLrData;
		iRegisterLcsMoLrData = NULL;
		}
	
	iRegisterLcsMoLrData = aData.Alloc();
	TIpcArgs args(iRegisterLcsMoLrData);
	
	User::LeaveIfError(RSessionBase::Send(EGWRegisterLcsMoLr, args));
	}


/**
*/
EXPORT_C void RLbsNetSim::ReleaseLcsMoLrL(TInt aReason)
	{
	LBSLOG(ELogP1, "RLbsNetSim::ReleaseLcsMoLrL()");
	iReleaseLcsMoLrReason = aReason;
	TIpcArgs args(&iReleaseLcsMoLrReasonPkg);
	User::LeaveIfError(RSessionBase::Send(EGWReleaseLcsMoLr, args));
	}


/**
*/
EXPORT_C void RLbsNetSim::ReleaseLcsLocationNotificationL(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	LBSLOG(ELogP1, "RLbsNetSim::ReleaseLcsLocationNotificationL()");
	iReleaseLcsLocationNotificationResponse = aResponse;
	TIpcArgs args(&iReleaseLcsLocationNotificationResponsePkg);
	User::LeaveIfError(RSessionBase::Send(EGWReleaseLcsLocationNotification, args));
	
	}


/**
*/
EXPORT_C void RLbsNetSim::MeasurementReportLocationL(const TPositionInfoBase& aPosition)
	{
	LBSLOG(ELogP1, "RLbsNetSim::MeasurementReportLocationL()");
	iMeasurementReportLocation->SendL(reinterpret_cast<const TPositionInfo&>(aPosition));
	}


/**
*/
EXPORT_C void RLbsNetSim::MeasurementReportLocationRequestMoreAssistanceDataL(const TLbsAssistanceDataGroup& aFilter)
	{
	LBSLOG(ELogP1, "RLbsNetSim::MeasurementReportLocationRequestMoreAssistanceDataL()");
	iRequestMoreAssistanceDataFilter = aFilter;
	TIpcArgs args(&iRequestMoreAssistanceDataFilterPkg);
	User::LeaveIfError(RSessionBase::Send(EGWRequestMoreAssistanceData, args));
	}


/**
*/
EXPORT_C void RLbsNetSim::MeasurementControlFailureL(TInt aReason)
	{
	LBSLOG(ELogP1, "RLbsNetSim::MeasurementControlFailureL()");
	TIpcArgs args(aReason);
	User::LeaveIfError(RSessionBase::Send(EGWMeasurementControlFailure, args));
	}
	

//
// Private methods

/**
*/
TInt RLbsNetSim::StartServer()
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
// Callback Handler: CNotificationMeasurementControlLocation
RLbsNetSim::CNotificationMeasurementControlLocation* RLbsNetSim::CNotificationMeasurementControlLocation::NewL(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver)
	{
	CNotificationMeasurementControlLocation* self = new(ELeave) CNotificationMeasurementControlLocation(aNetSim, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}
	
void RLbsNetSim::CNotificationMeasurementControlLocation::ConstructL()
	{
	TInt size = RLbsAssistanceDataBuilderSet::MaxExternalizedBufferSize();
	iAssistanceDataBuffer = HBufC8::NewL(size);
	iPtr.Set(iAssistanceDataBuffer->Des());
	
	TIpcArgs args(&iPositionPkg, &iQualityPkg, &iPtr);
	iNetSim->SendReceive(EGWNotificationMeasurementControlLocation, args, iStatus);
	SetActive();
	}

RLbsNetSim::CNotificationMeasurementControlLocation::CNotificationMeasurementControlLocation(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver) :
	CActive(EPriorityStandard), iNetSim(aNetSim), iObserver(aObserver),
	iPositionPkg(iPosition), iQualityPkg(iQuality), iAssistanceDataBuffer(NULL), iPtr(NULL, 0)
	{
	CActiveScheduler::Add(this);
	}
	
RLbsNetSim::CNotificationMeasurementControlLocation::~CNotificationMeasurementControlLocation()
	{
	Cancel();

	delete iAssistanceDataBuffer;
	}
	
	
void RLbsNetSim::CNotificationMeasurementControlLocation::RunL()
	{
	LBSLOG(ELogP1, "RLbsNetSim::CNotificationMeasurementControlLocation::RunL()");
	if (iStatus == KErrNone)
		{
		// Read the assistance data
		RLbsAssistanceDataBuilderSet assistanceData;
		assistanceData.OpenL();
		CleanupClosePushL(assistanceData);
		RDesReadStream read(iPtr);
		CleanupClosePushL(read);
		assistanceData.InternalizeL(read);
		
		LBSLOG(ELogP2, "Calling iObserver->ProcessMeasurementControlLocation()");
		// A reference location must bo of "Network" type.
		iPosition.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
		iObserver->ProcessMeasurementControlLocation(iPositionPkg(), assistanceData, iQualityPkg());
		
		CleanupStack::PopAndDestroy(2);
		}
	else
		{
		if (iStatus.Int() == RLbsNetSimTest::KNetSimNetworkNotAvailable)
			{
			LBSLOG(ELogP2, "Calling iObserver->ProcessError(KErrNotReady)");
			iObserver->ProcessError(KErrNotReady);
			}
		else
			{
			LBSLOG2(ELogP2, "Calling iObserver->ProcessMeasurementControlLocationError(%d)", iStatus.Int());
			iObserver->ProcessMeasurementControlLocationError(iStatus.Int());
			}
		}
		
	// Restart
	TIpcArgs args(&iPositionPkg, &iQualityPkg, &iPtr);
	iNetSim->SendReceive(EGWNotificationMeasurementControlLocation, args, iStatus);
	SetActive();
	}

void RLbsNetSim::CNotificationMeasurementControlLocation::DoCancel()
	{
	iNetSim->Send(EGWNotificationMeasurementControlLocationCancel);
	}
	
//
// Callback Handler: CMeasurementReportLocation
RLbsNetSim::CMeasurementReportLocation::CMeasurementReportLocation(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver) :
	CActive(EPriorityStandard), iNetSim(aNetSim), iObserver(aObserver), 
	iReasonPkg(iReason), iPositionPkg(iPosition), iFireNotificationPkg(iFireNotification)
	{
	CActiveScheduler::Add(this);
	}

RLbsNetSim::CMeasurementReportLocation::~CMeasurementReportLocation()
	{
	Cancel();
	}
	
void RLbsNetSim::CMeasurementReportLocation::SendL(const TPositionInfo& aPosition)
	{
	if (!IsActive())
		{
		iPosition = aPosition;
		
		// Setup argument for data
		TIpcArgs args(&iReasonPkg, &iPositionPkg, &iFireNotificationPkg);
		iNetSim->SendReceive(EGWMeasurementReportLocation, args, iStatus);
		SetActive();
		}
	else
		{
		User::Leave(KErrInUse);
		}
		
	}
	
void RLbsNetSim::CMeasurementReportLocation::RunL()
	{
	LBSLOG(ELogP1, "RLbsNetSim::CMeasurementReportLocation::RunL()");
	if (iStatus == KErrNone)
		{
		if (iFireNotification) // This will only be true under MoLr
			{
			LBSLOG2(ELogP2, "Calling iObserver->ProcessFacilityLcsMoLrResult(%d, <TPositionInfo>)", iReason);
			// A final network location must be of "Assisted" type.
			// Even if no assistance data has been used when calculating the location.
			// It is because we use the technology type to distinguish between
			// the reference and the final location updates.
			iPosition.SetPositionMode(iPosition.PositionMode() | TPositionModuleInfo::ETechnologyAssisted);
			iObserver->ProcessFacilityLcsMoLrResult(iReason, &iPosition);
			} // if
		}
	else
		{
		if (iStatus.Int() == RLbsNetSimTest::KNetSimNetworkNotAvailable)
			{
			LBSLOG(ELogP2, "Calling iObserver->ProcessError(KErrNotReady)");
			iObserver->ProcessError(KErrNotReady);
			}
		else
			{
			if (iFireNotification)
				{
				LBSLOG2(ELogP2, "Calling iObserver->ProcessFacilityLcsMoLrResult(%d, NULL)", iStatus.Int());
				iObserver->ProcessFacilityLcsMoLrResult(iStatus.Int(), NULL);
				}
			}
		}
	}

void RLbsNetSim::CMeasurementReportLocation::DoCancel()
	{
	iNetSim->Send(EGWMeasurementReportLocationCancel);
	}

//
// Callback Handler: CNotificationRegisterLcsLocation
RLbsNetSim::CNotificationRegisterLcsLocation::CNotificationRegisterLcsLocation(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver) :
	CActive(EPriorityStandard), iNetSim(aNetSim), iObserver(aObserver),
	iRequestInfoPkg(iRequestInfo), iRequestPrivacyPkg(iRequestPrivacy)
	{
	CActiveScheduler::Add(this);

	TIpcArgs args(&iRequestInfoPkg, &iRequestPrivacyPkg);
	iNetSim->SendReceive(EGWNotificationRegisterLcsLocation, args, iStatus);
	SetActive();
	}

RLbsNetSim::CNotificationRegisterLcsLocation::~CNotificationRegisterLcsLocation()
	{
	Cancel();
	}
	
	
void RLbsNetSim::CNotificationRegisterLcsLocation::RunL()
	{
	LBSLOG(ELogP1, "RLbsNetSim::CNotificationRegisterLcsLocation::RunL()");
	if (iStatus == KErrNone)
		{
		LBSLOG(ELogP2, "Calling iObserver->ProcessRegisterLcsLocationNotification(<RequestInfo>, <RequestPrivacy>)");
		iObserver->ProcessRegisterLcsLocationNotification(iRequestInfo, iRequestPrivacy);
		}
	else
		{
		if (iStatus.Int() == RLbsNetSimTest::KNetSimNetworkNotAvailable)
			{
			LBSLOG(ELogP2, "Calling iObserver->ProcessError(KErrNotReady)");
			iObserver->ProcessError(KErrNotReady);
			}
		else
			{
			LBSLOG2(ELogP2, "Calling iObserver->ProcessError(%d)", iStatus.Int());
			iObserver->ProcessError(iStatus.Int());
			}
		}
		
	// Restart
	TIpcArgs args(&iRequestInfoPkg, &iRequestPrivacyPkg);
	iNetSim->SendReceive(EGWNotificationRegisterLcsLocation, args, iStatus);
	SetActive();
	}

void RLbsNetSim::CNotificationRegisterLcsLocation::DoCancel()
	{
	iNetSim->Send(EGWNotificationRegisterLcsLocationCancel);
	}

//
// Callback Handler: CNotificationCancelPrivacyRequest
RLbsNetSim::CNotificationCancelPrivacyRequest::CNotificationCancelPrivacyRequest(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver) :
	CActive(EPriorityStandard), iNetSim(aNetSim), iObserver(aObserver), iReason(0), iReasonPkg(iReason)
	{
	CActiveScheduler::Add(this);

	TIpcArgs args(&iReasonPkg);
	iNetSim->SendReceive(EGWNotificationCancelPrivacy, args, iStatus);
	SetActive();
	}

RLbsNetSim::CNotificationCancelPrivacyRequest::~CNotificationCancelPrivacyRequest()
	{
	Cancel();
	}
	
	
void RLbsNetSim::CNotificationCancelPrivacyRequest::RunL()
	{
	LBSLOG(ELogP1, "RLbsNetSim::CNotificationCancelPrivacyRequest::RunL()");
	
	LBSLOG2(ELogP2, "Calling iObserver->ProcessReleaseLcsLocationNotification(%d)", iReason);
	iObserver->ProcessReleaseLcsLocationNotification(iReason);
		
	// Restart
	TIpcArgs args(&iReasonPkg);
	iNetSim->SendReceive(EGWNotificationCancelPrivacy, args, iStatus);
	SetActive();
	}

void RLbsNetSim::CNotificationCancelPrivacyRequest::DoCancel()
	{
	iNetSim->Send(EGWNotificationCancelPrivacyCancel);
	}

//
// Callback Handler: CNotificationNetworkGone
RLbsNetSim::CNotificationNetworkGone::CNotificationNetworkGone(RLbsNetSim* aNetSim, MLbsNetSimObserver* aObserver) :
	CActive(EPriorityStandard), iNetSim(aNetSim), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);

	iNetSim->SendReceive(EGWNotificationNetworkGone, iStatus);
	SetActive();
	}

RLbsNetSim::CNotificationNetworkGone::~CNotificationNetworkGone()
	{
	Cancel();
	}
	
	
void RLbsNetSim::CNotificationNetworkGone::RunL()
	{
	LBSLOG(ELogP1, "RLbsNetSim::CNotificationNetworkGone::RunL()");
	if (iStatus != KErrCancel)
		{
		LBSLOG(ELogP2, "Calling iObserver->ProcessError(KErrNotReady)");
		iObserver->ProcessError(KErrNotReady);
		}
		
	// Restart
	iNetSim->SendReceive(EGWNotificationNetworkGone, iStatus);
	SetActive();
	}

void RLbsNetSim::CNotificationNetworkGone::DoCancel()
	{
	iNetSim->Send(EGWNotificationNetworkGoneCancel);
	}

//
// Callback Handler: CNotificationResetAssistanceData
RLbsNetSim::CNotificationResetAssistanceData::CNotificationResetAssistanceData(RLbsNetSim* aNetSim, 
								MLbsNetSimObserver* aObserver) :
	CActive(EPriorityStandard), iNetSim(aNetSim), iObserver(aObserver), iMask(0), iMaskPkg(iMask)
	{
	CActiveScheduler::Add(this);

	// Restart
	TIpcArgs args(&iMaskPkg);
	iNetSim->SendReceive(EGWNotificationResetAssistanceData, args, iStatus);
	SetActive();
	}

RLbsNetSim::CNotificationResetAssistanceData::~CNotificationResetAssistanceData()
	{
	Cancel();
	}
	
	
void RLbsNetSim::CNotificationResetAssistanceData::RunL()
	{
	LBSLOG(ELogP1, "RLbsNetSim::CNotificationResetAssistanceData::RunL()");
	if (iStatus == KErrNone)
		{
		LBSLOG2(ELogP2, "Calling iObserver->ProcessResetAssistanceData(0x%x)", iMask);
		iObserver->ProcessResetAssistanceData(iMask);
		}
	else
		{
		LBSLOG2(ELogP2, "Error reported when trying to send reset assistance data: %d", iStatus.Int());
		}
		
	// Restart
	TIpcArgs args(&iMaskPkg);
	iNetSim->SendReceive(EGWNotificationResetAssistanceData, args, iStatus);
	SetActive();
	}

void RLbsNetSim::CNotificationResetAssistanceData::DoCancel()
	{
	iNetSim->Send(EGWNotificationResetAssistanceDataCancel);
	}
