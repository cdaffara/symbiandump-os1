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
// This file provides the implementation of the class that manages
// protocol aspects of Test Protocol Module operation.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include <e32base.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "suplconflictmanager.h"
#include "suplmolrfsmsession.h"
#include "suplmtlrfsmsession.h"
#include "suplprotocolmanager.h"
#include "suplassistancedatamgr.h"
#include "suplasn1decoder.h"
#include "suplinit.h"
#include <lbs/lbssuplpushreceiver.h>
#include "supldevloggermacros.h"

/** The unique ID of this plug-in implementation.
This corresponds to the implementation UID specified in the .rss file
for this protocol module.
*/
const TInt KPluginUidValue = 0x10285A9D;
const TUid KPluginUid = { KPluginUidValue };

/** Static constructor.
@param aGateway A reference to the protocol manager observer
@return A new instance of the CSuplProtocolManager class
*/
CSuplProtocolManager* CSuplProtocolManager::NewL(MSuplProtocolManagerObserver& aGateway)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::NewL() Begin\n");
	CSuplProtocolManager* self = new (ELeave) CSuplProtocolManager(aGateway);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplProtocolManager::NewL() End\n");
	return self;
	}

/** Standard constructor.
@param aGateway A reference to the protocol manager observer
*/
CSuplProtocolManager::CSuplProtocolManager(MSuplProtocolManagerObserver& aGateway)
: iGateway(aGateway), iLbsStatus(CLbsNetworkProtocolBase::ESystemStatusNone),
	iActiveServiceMask(MLbsNetworkProtocolObserver::EServiceNone)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::CSuplProtocolManager() Begin\n");
	iInternalSessionId.SetSessionOwner(KPluginUid);
	iInternalSessionId.SetSessionNum(0);
	SUPLLOG(ELogP1, "CSuplProtocolManager::CSuplProtocolManager() End\n");
	}
	
/** Private second-stage constructor.
*/
void CSuplProtocolManager::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::ConstructL() Begin\n");
	iConnectionManager = CSuplConnectionManager::NewL();
	iConflictManager = CSuplConflictManager::NewL();
	iMoLr = CSuplMolrFsmSession::NewL(*this);
	iNetLoc = CSuplMolrFsmSession::NewL(*this, CSuplMolrFsmSession::ESuplMolrCellBased);
	iMtLr = CSuplMtlrFsmSession::NewL(*this);	
	iAssistMgr = CSuplAssistanceDataManager::NewL(iGateway);
	iSessionCompleter = CSuplSessionCompleter::NewL(iGateway);
	iPushRec = CLbsSuplPushRec::NewL(*this);
	iNetInfoHandler = CSuplNetworkInfoHandler::NewL(*this);
	iNetInfoHandler->StartGettingNetworkInfoL();
	// Initialise array of supported SUPL versions.
	User::LeaveIfError(iSupportedVersions.Append(KSupportedSuplVersion_001));
	SUPLLOG(ELogP1, "CSuplProtocolManager::ConstructL() End\n");
	}

/** Standard destructor.
*/
CSuplProtocolManager::~CSuplProtocolManager()
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::~CSuplProtocolManager() Begin\n");
	iSupportedVersions.Close();
	delete iAssistMgr;
	delete iMoLr;
	delete iNetLoc;
	delete iMtLr;
	delete iConflictManager;
	delete iConnectionManager;
	delete iSessionCompleter;
    delete iNetInfoHandler;
	delete iPushRec;
	SUPLLOG(ELogP1, "CSuplProtocolManager::~CSuplProtocolManager() End\n");
	}
	
/** Handle LBS request for self locate
@param aSessionId The session ID supplied by LBS.
@param aOptions Parameters for the self location request (mode, assistance data mask, newclient flag,...)
*/
void CSuplProtocolManager::SelfLocationReq(const TLbsNetSessionId& aSessionId, 
				const TLbsNetPosRequestOptionsBase& aOptions)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::SelfLocationReq() Begin\n");
	
	const  TLbsNetPosRequestOptionsAssistance options = reinterpret_cast<const TLbsNetPosRequestOptionsAssistance&> (aOptions);
    TLbsAsistanceDataGroup dataRequestMask = options.DataRequestMask();
    
    // If the positioning method is autonomous, just complete the session
    //
    if (TPositionModuleInfo::ETechnologyTerminal == options.PosMode())
	    {
    	iSessionCompleter->CompleteSession(aSessionId, KErrNone);
 	   	}

 	// If not a new session, then just complete with no error - even if assistance data needed.
 	else if (!options.NewClientConnected())
 		{
 		iSessionCompleter->CompleteSession(aSessionId, KErrNone);
 		}
    else 
 	   {
		// Check if the conflict controller allows the self-locate
		// 
		switch (ResolveConflict(MLbsNetworkProtocolObserver::EServiceSelfLocation, aOptions))
			{
			case CSuplConflictManager::EConflictNone:
				StatusUpdate(MLbsNetworkProtocolObserver::EServiceSelfLocation, ETrue); // ETrue=new operation
				iMoLr->MoLrReq(aSessionId, options);
				iAssistMgr->ProcessDataRequest(dataRequestMask);
				break;

			case CSuplConflictManager::EConflictAdoptNewSessionId:
				// Continue with the existing MOLR network session but addopt new SessionId
				// in future communications with LBS due to the MOLR
				iMoLr->AdoptNewLbsSessionId(aSessionId);
				break;

			case CSuplConflictManager::EConflictCancelCurrent:
				// Cancel ongoing MTLR
				iMtLr->CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonNone);

				// Start new MOLR
				StatusUpdate(MLbsNetworkProtocolObserver::EServiceSelfLocation, ETrue); // ETrue=new operation
				iMoLr->MoLrReq(aSessionId, options);
				break;

			case CSuplConflictManager::EConflictRejectNew:
				iSessionCompleter->CompleteSession(aSessionId, KErrServerBusy);
				break;

			default :
				// No other conflict resolution is currently expected after
				// a SelfLocateRequest
				ASSERT(EFalse);
				Gateway().SessionCompleteInd(aSessionId, KErrGeneral);
				break;
			}
		}

	SUPLLOG(ELogP1, "CSuplProtocolManager::SelfLocationReq() End\n");
	}

/** Handle LBS completion of self locate
@param aSessionId The session to be completed.
@param aReason Completion reason value.
*/
void CSuplProtocolManager::SelfLocationCompleteInd(const TLbsNetSessionId& aSessionId, 
				TInt /*aReason*/)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::SelfLocationCompleteInd() Begin\n");
	// Check we have an active self locate and valid session ID,
	// otherwise silently consume this completion indication.
	if ((CSuplFsmSessionBase::EStateActive == iMoLr->State()) &&
		(aSessionId == iMoLr->LbsSessionId()))
		{
		iMoLr->CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonNone);
		}
	SUPLLOG(ELogP1, "CSuplProtocolManager::SelfLocationCompleteInd() End\n");
	}

/** Handle LBS system status indication
@param aStatus The status indication from LBS.
*/
void CSuplProtocolManager::SystemStatusInd(CLbsNetworkProtocolBase::TLbsSystemStatus aStatus)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::SystemStatusInd() Begin\n");
	iLbsStatus = aStatus;
	SUPLLOG(ELogP1, "CSuplProtocolManager::SystemStatusInd() End\n");
	}

/** Handle LBS Location Response
@param aSessionId The session ID for which this response is provided.
@param aReason An error response; KErrNone if position is okay.
@param aPosInfo The location response information
*/
void CSuplProtocolManager::LocationResp(const TLbsNetSessionId& aSessionId, TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::LocationResp() Begin\n");

	if (aSessionId == iMoLr->LbsSessionId())
		{
		iMoLr->LocationResp(aReason, aPosInfo);
		}
	else if (aSessionId == iMtLr->LbsSessionId())
		{
		iMtLr->LocationResp(aReason, aPosInfo);
		}
	else
		{
		// Unknown session ID
		ASSERT(EFalse);
		}
	SUPLLOG(ELogP1, "CSuplProtocolManager::LocationResp() End\n");
	}

/** Handle LBS Privacy Response
@param aResponse Privacy response value.
*/
void CSuplProtocolManager::PrivacyResp(const TLbsNetSessionId& aSessionId, 
			const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse,
			TInt /*aReason*/)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::PrivacyResp() Begin\n");
	
	
	
	
	
	iMtLr->PrivacyResp(aSessionId, aResponse);
	SUPLLOG(ELogP1, "CSuplProtocolManager::PrivacyResp() End\n");
	}


/** Handle LBS Network Based Location Request
*/
void CSuplProtocolManager::NetworkBasedLocationReq(const TLbsNetSessionId& aSessionId, 
										const TLbsNetPosRequestOptionsBase& aOptions)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::NetworkBasedLocationReq() Begin\n");
	
	switch (ResolveConflict(MLbsNetworkProtocolObserver::EServiceNetworkLocation, aOptions))
		{
		case CSuplConflictManager::EConflictNone:
			StatusUpdate(MLbsNetworkProtocolObserver::EServiceNetworkLocation, ETrue); // ETrue=new operation
			iNetLoc->MoLrReq(aSessionId, aOptions);
			break;

		case CSuplConflictManager::EConflictAdoptNewSessionId:
			// Continue with the existing MOLR network session but addopt new SessionId
			// in future communications with LBS due to the MOLR
			iNetLoc->AdoptNewLbsSessionId(aSessionId);
			break;

		case CSuplConflictManager::EConflictCancelCurrent:
			// Cancel ongoing MTLR
			iMtLr->CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonNone);

			// Start new MOLR
			StatusUpdate(MLbsNetworkProtocolObserver::EServiceNetworkLocation, ETrue); // ETrue=new operation
			iNetLoc->MoLrReq(aSessionId, aOptions);
			break;

		case CSuplConflictManager::EConflictRejectNew:
			// indicate server busy, continue with original request
			iSessionCompleter->CompleteSession(aSessionId, KErrServerBusy);
			break;

		default :
			// No other conflict resolution is currently expected after
			// a SelfLocateRequest
			ASSERT(EFalse);
			Gateway().SessionCompleteInd(aSessionId, KErrGeneral);
			break;
		}
	
	SUPLLOG(ELogP1, "CSuplProtocolManager::NetworkBasedLocationReq() End\n");
	}


/** Handle LBS Completion of a Network Based Location Request
*/
void CSuplProtocolManager::NetworkBasedLocationCompleteInd(const TLbsNetSessionId& aSessionId, 
														   TInt /*aReason*/)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::NetworkBasedLocationCompleteInd() Begin\n");
	// Check we have an active cell-based self locate and valid session ID,
	// otherwise silently consume this completion indication.
	if ((CSuplFsmSessionBase::EStateActive == iNetLoc->State()) &&
		(aSessionId == iNetLoc->LbsSessionId()))
		{
		iNetLoc->CancelMachine(CSuplFsmSessionBase::ECancelClient, CSuplFsmSessionBase::EReasonNone);
		}
	SUPLLOG(ELogP1, "CSuplProtocolManager::NetworkBasedLocationCompleteInd() End\n");
	}


/** Handle LBS request for Assistance Data
@param aDataRequestMask A mask identifying the set of data requested.
*/
void CSuplProtocolManager::AssistanceDataReq(TLbsAsistanceDataGroup aDataRequestMask)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::AssistanceDataReq() Begin\n");

	// Check with the Assistance Data Manager that the assistance data types requested by LBS
	// are supported. 
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask;
    serviceMask = iAssistMgr->ProcessDataRequest(aDataRequestMask);

	// Send the request to ALL of the state machines in 
	// the serviceMask.
	//
	if (serviceMask & MLbsNetworkProtocolObserver::EServiceSelfLocation)
		{
		iMoLr->AssistanceDataReq(aDataRequestMask);
		}
	if (serviceMask & MLbsNetworkProtocolObserver::EServiceMobileTerminated)
		{
		iMtLr->AssistanceDataReq(aDataRequestMask);
		}			

	SUPLLOG(ELogP1, "CSuplProtocolManager::AssistanceDataReq() End\n");
	}


/** Handle a request to transmit location to third party
*/
void CSuplProtocolManager::TransmitLocationReq(const TLbsNetSessionId& aSessionId,
								const TDesC& /*aDestination*/, TInt /*aPriority*/,
								const TLbsNetPosRequestOptionsBase& /*aOptions*/)
{
	SUPLLOG(ELogP1, "CSuplProtocolManager::TransmitLocationReq() Begin\n");
 // SUPL V1 does not support this service. Terminate session
 //
 iSessionCompleter->CompleteSession(aSessionId, KErrNotSupported);
 SUPLLOG(ELogP1, "CSuplProtocolManager::TransmitLocationReq() End\n");
}

/** Handle a request to cancel transmition of location to third party
*/
void CSuplProtocolManager::TransmitLocationCompleteInd(const TLbsNetSessionId& aSessionId, TInt /*aReason*/)
{
	SUPLLOG(ELogP1, "CSuplProtocolManager::TransmitLocationCompleteInd() Begin\n");
 // SUPL V1 does not support this service. Terminate session
 //
 iSessionCompleter->CompleteSession(aSessionId, KErrNotSupported);	
	SUPLLOG(ELogP1, "CSuplProtocolManager::TransmitLocationCompleteInd() End\n");
}

// --------------------------------   MSuplFsmSessionObserver methods ----------------------------------------------

/** Gateway interface pointer.
This method is called from state handlers to gain access to 
the Gateway Interface.
@see MSuplFsmSessionObserver
*/
MSuplProtocolManagerObserver& CSuplProtocolManager::Gateway()
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::Gateway() Begin\n");
	SUPLLOG(ELogP1, "CSuplProtocolManager::Gateway() End\n");
	return iGateway;
	}

/** Reference to the Connection Manager.
This method is called from state handlers to gain access to 
the Connection Manager.
@see MSuplFsmSessionObserver
*/
CSuplConnectionManager& CSuplProtocolManager::ConnectionManager()
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::ConnectionManager() Begin\n");
	SUPLLOG(ELogP1, "CSuplProtocolManager::ConnectionManager() End\n");
	return *iConnectionManager;
	}

/** Get a new session ID
*/
const TLbsNetSessionId& CSuplProtocolManager::NewSessionId()
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::NewSessionId() Begin\n");
	iInternalSessionId.IncrSession();
	SUPLLOG(ELogP1, "CSuplProtocolManager::NewSessionId() End\n");
	return iInternalSessionId;
	}

/** State machine procedure complete notification
Called by the state machines to notify the completion of a procedure.
@see MSuplFsmSessionObserver
*/
void CSuplProtocolManager::ProcedureCompleteInd(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aService)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::ProcedureCompleteInd() Begin\n");
	// Status update sent to LBS
	StatusUpdate(aService, EFalse); // EFalse=operation ended

	// Inform the Assistance Data manager that a state machine has completed in case
	// some assistance data types were expected from the machine
	iAssistMgr->MachineTerminated(aService);

	SUPLLOG(ELogP1, "CSuplProtocolManager::ProcedureCompleteInd() End\n");
	}

/** Handle a location request from the network. This can be due to a new
location request (i.e, after SUPL INIT) or it can be due to a change to the quality
of the position or to the positioning method requested by the remote SUPL server
either after a SUPL message (e.g. SUPL RESPONSE) or after a positioning message
(e.g. RRLP Measure Position Request).

@param aQuality Accuracy of the positioning request
@param aPosMethod Positioning methods requested by the remote SUPL server.
@param aType type of service (procedure) of the state machine that issued the request
@param aSessionId session ID of the state machine that issues the request
*/
void CSuplProtocolManager::LocationReq(const TLbsNetSessionId& aSessionId, 
				 const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aType, 
				const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::LocationReq() Begin\n");

	// Let the assistance data manager know that the machine doing service of
	// type aType is able to accept assistance data requests.
	//
	iAssistMgr->MachineReadyForAssistanceDataRequest(aType);

	// Fordward network-received location request to Gateway Interface
	iGateway.LocationReq(aSessionId, aType, aQuality, aPosMethod);
	SUPLLOG(ELogP1, "CSuplProtocolManager::LocationReq() End\n");
	}

/** Handle a Privacy Request after receiving a SUPL INIT

@param aSessionId session ID of the state machine that issues the request
@param aPrivacy Privacy advice based on received SUPL INIT
@param aRequestInfo Information about the requestor of the MT-LR from SUPL INIT
*/
void CSuplProtocolManager::PrivacyReq(const TLbsNetSessionId& aSessionId,
									   const TLbsNetPosRequestPrivacy& aPrivacy,
									   const TLbsExternalRequestInfo& aRequestInfo)
{
	SUPLLOG(ELogP1, "CSuplProtocolManager::PrivacyReq() Begin\n");
	// Fordward network-received location request to Gateway Interface
	iGateway.PrivacyReq(aSessionId, EFalse, // No Emergency MT-LR in SUPL v1.0 
					    aPrivacy, aRequestInfo);	
	SUPLLOG(ELogP1, "CSuplProtocolManager::PrivacyReq() End\n");
}


/** Send Assistance Data from the state machines to the Assistance Data Manager

This method is called by state machines when they have assistance data for LBS.

@see CSuplAssistanceDataManager
@see MSuplFsmSessionObserver
*/
void CSuplProtocolManager::ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, 
							   const RLbsAssistanceDataBuilderSet& aData,const TInt& aReason,
							   const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aService)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::ProcessAssistanceData() Begin\n");

	// Get the session Id of the machine reporting assistance data
	//
	TLbsNetSessionId sessionId;
	if (MLbsNetworkProtocolObserver::EServiceSelfLocation == aService)
		{
		sessionId = iMoLr->LbsSessionId();	
		}
	else if (MLbsNetworkProtocolObserver::EServiceMobileTerminated == aService)
		{
		sessionId = iMtLr->LbsSessionId();		
		}
	else
		{
		ASSERT(EFalse);
		}

	// Pass received assistance data to the assistance data manager
	//
	iAssistMgr->AssistanceDataReport(aData, aGroupMask, aReason, aService, sessionId);	
	SUPLLOG(ELogP1, "CSuplProtocolManager::ProcessAssistanceData() End\n");
	}




// --------------------------------   MSuplNetworkInfoObserver methods ----------------------------------------------

void CSuplProtocolManager::NetInfoResults(const RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
						   const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::NetInfoResults() Begin\n");
	iMoLr->StoreNetInfo(aNetworkInfo,aLocationArea);
	iNetLoc->StoreNetInfo(aNetworkInfo,aLocationArea);
	iMtLr->StoreNetInfo(aNetworkInfo,aLocationArea);
	SUPLLOG(ELogP1, "CSuplProtocolManager::NetInfoResults() End\n");
	}

void CSuplProtocolManager::HomeMccMncResult(TUint aMcc, TUint aMnc)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::HomeMccMncResult() Begin\n");
	iMoLr->StoreMccMnc(aMcc, aMnc);
	iNetLoc->StoreMccMnc(aMcc, aMnc);
	iMtLr->StoreMccMnc(aMcc, aMnc);
	SUPLLOG(ELogP1, "CSuplProtocolManager::HomeMccMncResult() End\n");
	}

void CSuplProtocolManager::MsisdnResult(const TDesC& aTelNumber)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::MsisdnResult() Begin\n");
	iMoLr->StoreMsisdn(aTelNumber);
	iNetLoc->StoreMsisdn(aTelNumber);
	iMtLr->StoreMsisdn(aTelNumber);
	SUPLLOG(ELogP1, "CSuplProtocolManager::MsisdnResult() End\n");
	}

void CSuplProtocolManager::CellInfoResults(const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::CellInfoResults() Begin\n");
	iMoLr->StoreCellInfo(aCellInfo);
	iNetLoc->StoreCellInfo(aCellInfo);
	iMtLr->StoreCellInfo(aCellInfo);
	SUPLLOG(ELogP1, "CSuplProtocolManager::CellInfoResults() End\n");
	}

//-------------------------------- Private methods ----------------------------------------------------------

/** Inform LBS of status update.
This is called when a new operation starts or when a current operation ends.

@param aService Indicates the type of service that has started or stopped
@param aIsOperationStarting ETrue if operation is starting
*/
void CSuplProtocolManager::StatusUpdate(
					const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aService, 
					const TBool& aIsOperationStarting)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::StatusUpdate() Begin\n");
	if (aIsOperationStarting)
		{
		iActiveServiceMask |= aService;
		}
	else
		{
		iActiveServiceMask &= ~aService;
		}

	 Gateway().StatusUpdate(iActiveServiceMask);
	SUPLLOG(ELogP1, "CSuplProtocolManager::StatusUpdate() End\n");
	}


/** Resolve the conflict between active procedures and a new request.

@param aNewOperation The type of operation that is represented by the new request.
@param aNewOperationOptions Options about the new operation that affect conflict control (posMethod, newClient,...)
@param aNoMoreAssistDataNeeded A boolean that is 'true' is no assistance data is needed by the new operation.
@return TConflictResult The action arising from the conflict resolution that is to be done.
*/
CSuplConflictManager::TConflictResult CSuplProtocolManager::ResolveConflict(
					const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aNewOperation, 
					const TLbsNetPosRequestOptionsBase& aNewOperationOptions)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::ResolveConflict() Begin\n");

	CSuplConflictManager::TConflictResult conflictResult = CSuplConflictManager::EConflictNone;

	if (iActiveServiceMask != 0)
		{
		conflictResult = iConflictManager->ConflictDecision(iActiveServiceMask, aNewOperation, aNewOperationOptions);
		}
	else
		{
		// no previous active operation = no conflict
		}

	SUPLLOG(ELogP1, "CSuplProtocolManager::ResolveConflict() End\n");
	return conflictResult;
	}

/** Receive notification about an incoming SUPL INIT message.

@param aChannel  [In] The channel the call-back is related to.
@param aReqId    [In] An Id of the request the call-back is related to.
@param aMsg      [In] A buffer containing a SUPL INIT message.
@see CLbsSuplPushRec::SuplInitComplete
@see CLbsSuplPush::SuplInit
*/
void CSuplProtocolManager::OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId, TDesC8& aMsg)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::OnSuplInit() Begin\n");
	TInt err = KErrNone;
	TInt reserved = KErrNone;
	
	TRAP(err, ProcessSuplInitL(aMsg));
	
	// notify the SUPL push receiver that the SUPL INIT has been received and processed.
	iPushRec->SuplInitComplete(aChannel, aReqId, err, reserved);
	SUPLLOG(ELogP1, "CSuplProtocolManager::OnSuplInit() End\n");
	}

void CSuplProtocolManager::ProcessSuplInitL(TDesC8& aMsg)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::ProcessSuplInitL() Begin\n");
	TInt error;
	// decode the received message
	CSuplAsn1Decoder* decoder = CSuplAsn1Decoder::NewL();
	CleanupStack::PushL(decoder);
	TPtrC8 msg(aMsg);
	
	// Check that the encoded length of the SUPL message matches 
	// the actual length of the encoded supl message descriptor
	TInt lengthField = msg[0] << 8;
	lengthField |= msg[1];
	if(msg.Length() != lengthField)
		{
		// Encoded message length does not match actual message length.
		SUPLLOG3(ELogP1, "CSuplProtocolManager::ProcessSuplInitL() msg length (%d) != length field (%d)", msg.Length(), lengthField);
		User::Leave(KErrCorrupt);
		}

	CSuplMessageBase* message = decoder->DecodeL(&msg, error);
	CleanupStack::PushL(message);
	
	// check for decode error 
	if (error!= KErrNone)
		{
		User::Leave(error);
		}
	
	// check message is in fact a SUPL INIT
	if (message->MessageType() != CSuplMessageBase::ESuplInit)
		{
		User::Leave(KErrNotSupported);
		}
	

	// Check if this supl packets major version is supported by
	// this implementation of the protocol

	// Get major version from message.
	CSuplVersion* version = CSuplVersion::NewL();
	CleanupStack::PushL(version);
	User::LeaveIfError(message->GetVersion(*version));

	// Check if version is supported.
	TBool versionMatchFound = EFalse;
	for (TInt i = 0; i < iSupportedVersions.Count(); i++)
		{
		if (version->iMaj == iSupportedVersions[i])
			{
			versionMatchFound = ETrue;
			break;
			}
		}

	// Version not supported.
	if (!versionMatchFound)
		{
		User::Leave(KErrNotSupported);
		}

	CleanupStack::PopAndDestroy(version);


	// pop message off the stack as ownership will be
	// transferred
	CleanupStack::Pop(message);
	NetworkLocationReq(message);
	
	CleanupStack::PopAndDestroy(decoder);
	SUPLLOG(ELogP1, "CSuplProtocolManager::ProcessSuplInitL() End\n");
	}

/** Handle Network Location Request (SUPL INIT)
*/
void CSuplProtocolManager::NetworkLocationReq(CSuplMessageBase* aSuplInit)
	{
	SUPLLOG(ELogP1, "CSuplProtocolManager::NetworkLocationReqL() Begin\n");
	TBool messageTransferred = EFalse;
	CSuplInit* suplInit = static_cast<CSuplInit*> (aSuplInit);
	
	// Set the Position Request options according to the received QoP element.
	TLbsNetPosRequestOptions options;
	options.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality;
	if (suplInit->QopPresent())
		{
		suplInit->GetQop(quality);
		options.SetRequestQuality(quality);
		}
		
	// Check if the conflict controller allows the network initiated location request
	switch (ResolveConflict(MLbsNetworkProtocolObserver::EServiceMobileTerminated, options))
		{
		// proceeed with the new request.
		case CSuplConflictManager::EConflictNone:
			// Status update sent to LBS
			StatusUpdate(MLbsNetworkProtocolObserver::EServiceMobileTerminated, ETrue); // ETrue=new operation

			// Start the MT-LR (passes ownership of CSuplInit object)
			iMtLr->MtLrReq(NewSessionId(), suplInit); 
			messageTransferred = ETrue;
			break;

		// Reject the new request
		case CSuplConflictManager::EConflictRejectNew:
			// Silently drop the SUPL INIT
			break;

		default:
			// No other conflict decisions are currently supported after
			// SUPL INIT
			//
			ASSERT(EFalse);
			break;
		}
	
	if (!messageTransferred)
		{
		delete suplInit;
		}
	
	SUPLLOG(ELogP1, "CSuplProtocolManager::NetworkLocationReqL() End\n");
	}

