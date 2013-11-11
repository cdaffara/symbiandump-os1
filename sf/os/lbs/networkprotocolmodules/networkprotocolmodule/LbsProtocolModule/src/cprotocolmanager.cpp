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
// This file provides the implementation of the class that manages
// protocol aspects of Test Protocol Module operation.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32base.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "cconfigmanager.h"
#include "cmolrstatemachine.h"
#include "cmtlrstatemachine.h"
#include "cx3pstatemachine.h"
#include "cnetlocstatemachine.h"
#include "cprotocolmanager.h"
#include "cassistdatamgr.h"
#include "lbsdevloggermacros.h"

#ifdef ENABLE_LBS_DEV_LOGGER
#include <lbs/lbsnetclasstypes.h>
#endif

/** The unique ID of this plug-in implementation.
This corresponds to the implementation UID specified in the .rss file
for this protocol module.
*/
const TInt KPluginUidValue = 0x10281D70;
const TUid KPluginUid = { KPluginUidValue };


/** Static constructor.
@param aGateway A reference to the protocol manager observer
@return A new instance of the CProtocolManager class
*/
CProtocolManager* CProtocolManager::NewL(MProtocolMgrObserver& aGateway)
	{
	CProtocolManager* self = new (ELeave) CProtocolManager(aGateway);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aGateway A reference to the protocol manager observer
*/
CProtocolManager::CProtocolManager(MProtocolMgrObserver& aGateway)
: iGateway(aGateway), iLbsStatus(CLbsNetworkProtocolBase::ESystemStatusNone),
	iActiveServiceMask(MLbsNetworkProtocolObserver::EServiceNone)
	{
	iInternalSessionId.SetSessionOwner(KPluginUid);
	iInternalSessionId.SetSessionNum(0);

	LBSLOG(ELogP2, "CProtocolManager::CProtocolManager() CurrentOp --> EOpNone\n");
	iCurrentOp.iOperation = CConfigManager::EOpNone;
	iCurrentOp.iPriority = CConfigManager::EPriorityNone;
	}


/** Standard destructor.
*/
CProtocolManager::~CProtocolManager()
	{
	delete iAssistMgr;
	delete iNetLoc;
	delete iX3p;
	delete iMtLr;
	delete iMoLr;
	delete iConfig;
	delete iNetwork;
	}
	
	
/** Private second-stage constructor.
*/
void CProtocolManager::ConstructL()
	{
	iNetwork = CNetworkInterface::NewL(*this);
	iNetwork->Connect();
	
	iConfig = CConfigManager::NewL();
	iMoLr = CMoLrStateMachine::NewL(*this);
	iMtLr = CMtLrStateMachine::NewL(*this);
	iX3p = CX3pStateMachine::NewL(*this);
	iNetLoc = CNetLocStateMachine::NewL(*this);
	iAssistMgr = CAssistDataMgr::NewL();
	}


/** Identify active state machine
@param aActiveMachine A reference to a pointer that identifies the active
	state machine. This pointer is set to NULL if there is no active machine.
*/
void CProtocolManager::GetActiveStateMachine(CStateMachineBase*& aActiveMachine) const
	{
	//LBSLOG(ELogP2, "CProtocolManager::GetActiveStateMachine() \n");
	if (CStateMachineBase::EStateActive == iMtLr->State())
		{
		LBSLOG(ELogP2, "CProtocolManager::GetActiveStateMachine() Active Machine = MTLR\n");
		aActiveMachine = iMtLr;
		}
	else if (CStateMachineBase::EStateActive == iMoLr->State())
		{
		LBSLOG(ELogP2, "CProtocolManager::GetActiveStateMachine() Active Machine = MOLR\n");
		aActiveMachine = iMoLr;
		}
	else if (CStateMachineBase::EStateActive == iX3p->State())
		{
		LBSLOG(ELogP2, "CProtocolManager::GetActiveStateMachine() Active Machine = X3P\n");
		aActiveMachine = iX3p;
		}
	else if (CStateMachineBase::EStateActive == iNetLoc->State())
		{
		LBSLOG(ELogP2, "CProtocolManager::GetActiveStateMachine() Active Machine = NBLR\n");
		aActiveMachine = iNetLoc;
		}
	else
		{
		LBSLOG(ELogP2, "CProtocolManager::GetActiveStateMachine() No Active Machine\n");
		aActiveMachine = NULL;
		}

	}


/** Identify a state machine that is currently cancelling.
@param aCancellingMachine A reference to a pointer that identifies the cancelling
	state machine. This pointer is set to NULL if there is no cancelling machine.
*/
void CProtocolManager::GetCancellingStateMachine(CStateMachineBase*& aCancellingMachine) const
	{

	if (CStateMachineBase::EStateCancelling == iMtLr->State())
		{
		aCancellingMachine = iMtLr;
		}
	else if (CStateMachineBase::EStateCancelling == iMoLr->State())
		{
		aCancellingMachine = iMoLr;
		}
	else if (CStateMachineBase::EStateCancelling == iX3p->State())
		{
		aCancellingMachine = iX3p;
		}
	else if (CStateMachineBase::EStateCancelling == iNetLoc->State())
		{
		aCancellingMachine = iNetLoc;
		}
	else
		{
		aCancellingMachine = NULL;
		}
	}


/** Identify a state machine that is currently queued.
This will identify a state machine that is queued and ready to be started.
Priority is given to MT-LR because whilst another machine is cancelling
an MT-LR emergency request may be briefly queued.

@param aQueuedMachine A reference to a pointer that identifies a queued
	state machine. This pointer is set to NULL if there is no queued machine.
*/
void CProtocolManager::GetQueuedStateMachine(CStateMachineBase*& aQueuedMachine) const
	{
	if (iMtLr->IsMachineQueued())
		{
		aQueuedMachine = iMtLr;
		}
	else if (iMoLr->IsMachineQueued())
		{
		aQueuedMachine = iMoLr;
		}
	else if (iX3p->IsMachineQueued())
		{
		aQueuedMachine = iX3p;
		}
	else if (iNetLoc->IsMachineQueued())
		{
		aQueuedMachine = iNetLoc;
		}
	else
		{
		aQueuedMachine = NULL;
		}

	}


/** Resolve the conflict between an active procedure and a new request.
@param aNewOp The type of operation that is represented by the new request.
@param aNewPriority The priority of the new request - optional.
@return TBool An indication if the 
*/
CConfigManager::TConflictResult CProtocolManager::ResolveConflict(
			CConfigManager::TConflictOp aNewOp, 
			CConfigManager::TConflictPriority aNewPriority)
	{
	CConfigManager::TConflictResult conflictResult = CConfigManager::EConflictNone;
	CStateMachineBase* activeMachine;
	CStateMachineBase* cancellingMachine;
	GetActiveStateMachine(activeMachine);
	GetCancellingStateMachine(cancellingMachine);

	LBSLOG3(ELogP2, "CProtocolManager::ResolveConflict() Current op = %d, New op = %d\n", iCurrentOp.iOperation, aNewOp);
	
	// One special case relates to an existing MT-LR that is awaiting the 
	// measurement control after a privacy reject. This MT-LR sequence can be
	// cancelled once a new request arrives.
	if ((iMtLr == activeMachine) && (iMtLr->IsRejectedWaitingMeasureControl()))
		{
		iMtLr->CancelMachine(CStateMachineBase::ECancelNone);
		conflictResult = CConfigManager::EConflictCancelCurrent;
		}
	// There is an operation already in the process of being cancelled, 
	// so queue the new request until this completes.
	else if (NULL != cancellingMachine)
		{
		LBSLOG(ELogP2, "CProtocolManager::ResolveConflict() Queing new request while previous op cancelled\n");
		conflictResult = CConfigManager::EConflictQueueNew;
		}
	else
		{
		CConfigManager::TConflictOperation newOp;
		newOp.iOperation = aNewOp;
		newOp.iPriority = aNewPriority;

		// A special case is when tracking is set on and it is not a new MO-LR request.
		// If there is not a current active operation, or MOLR is active, then
		// there may be a need to cancel a tracking 'session'.
		if ((iLbsStatus & CLbsNetworkProtocolBase::ESystemStatusClientTracking) &&
			(CConfigManager::EOpMoLr != aNewOp) &&
			((NULL == activeMachine) || (iMoLr == activeMachine)))
			{
			// Use a fake MO-LR to obtain a conflict decision.
			CConfigManager::TConflictOperation fakeMoLrOp;
			fakeMoLrOp.iOperation = CConfigManager::EOpMoLr;
			fakeMoLrOp.iPriority = CConfigManager::EPriorityNone;
			conflictResult = iConfig->ConflictDecision(fakeMoLrOp, newOp);

			// Should tracking be cancelled?
			if ((CConfigManager::EConflictCancelCurrent == conflictResult) ||
				(CConfigManager::EConflictQueueCurrent == conflictResult))
				{
				// Turn tracking off
				iLbsStatus = CLbsNetworkProtocolBase::ESystemStatusNone;
				if (NULL == activeMachine)
					{
					// Cancel tracking session if we have no active operation and
					// there has been a preceding MOLR session
					if ((iMoLr->SessionId().SessionOwner().iUid != 0) ||
						(iMoLr->SessionId().SessionNum() != 0))
						{
						Gateway()->SessionCompleteInd(iMoLr->SessionId(), KErrPositionHighPriorityReceive);
						}
					conflictResult = CConfigManager::EConflictNone;
					}
				}
			}
		// If we have an active operation which needs a conflict decision
		else if( (NULL != activeMachine) /*&& (iCurrentOp.iOperation != CConfigManager::EOpNone)*/)
			{
			conflictResult = iConfig->ConflictDecision(iCurrentOp, newOp);
			}
		else
			{
			// no tracking and no active operation = no conflict
			}

		// Perform any initial action arising from conflict decision
		switch (conflictResult)
			{
		// Current operation must be cancelled
		case CConfigManager::EConflictCancelCurrent:
			// Indicate if the source of cancellation is the network or LBS
			if ((CConfigManager::EOpMtLr == aNewOp) || 
				(CConfigManager::EOpNiLr == aNewOp))
				{
				LBSLOG(ELogP2, "CProtocolManager::ResolveConflict() Cancelling state machine, client\n");
				activeMachine->CancelMachine(CStateMachineBase::ECancelNetworkCancel, KErrPositionHighPriorityReceive);
				}
			else
				{
				LBSLOG(ELogP2, "CProtocolManager::ResolveConflict() Cancelling state machine, network\n");
				activeMachine->CancelMachine(CStateMachineBase::ECancelClientCancel, KErrPositionHighPriorityReceive);
				}
			break;

		// Queue the current operation before performing new operation
		// Note: At present we only have a requirement to queue an X3P timer
		//       and the functionality to support this is as follows:
		//       (a) cancel current operation BUT without cancelling LBS session
		//       (b) silently consume any LBS responses that relate to this session
		//       (c) proceed with the new operation
		//       (d) wait for LBS (NRH) to repeat the X3P timer request
		case CConfigManager::EConflictQueueCurrent:
			activeMachine->CancelMachine(CStateMachineBase::ECancelClientCancelSilent, KErrPositionHighPriorityReceive);
			break;

		// Other conflict results require no further action here
		default:
			break;
			};
		}

	LBSLOG2(ELogP2, "CProtocolManager::ResolveConflict() returning conflict result %d\n", conflictResult);
	return conflictResult;
	}


/** Handle LBS request for Assistance Data
@param aDataRequestMask A mask identifying the set of data requested.
*/
void CProtocolManager::AssistanceDataReq(TLbsAsistanceDataGroup aDataRequestMask)
	{
	// Pass request to the assistance data manager to process
	TBool noAssistanceNeeded = iAssistMgr->ProcessDataRequest(aDataRequestMask);

	// No assistance needed?
	if (noAssistanceNeeded)
		{
		// Do nothing!
		}
	// Check if the requested data mask was entirely invalid
	else if (iAssistMgr->RequestErrorMask() == aDataRequestMask)
		{
		// Report the request error now
		DoAssistanceDataActions();
		}
	else
		{
		// Check if we have an active state machine
		CStateMachineBase* activeMachine;
		GetActiveStateMachine(activeMachine);
		if (NULL != activeMachine)
			{
			// Advise state machine of assistance data request
			activeMachine->AssistanceDataReq();
			}
		else
			{
			// Store assistance data error
			iAssistMgr->Error(KErrGeneral);

			// Report the error
			DoAssistanceDataActions();
			}
		}
	}


/** Handle LBS request for self locate
@param aSessionId The session ID supplied by LBS.
@param aDataRequestMask A mask identifying the set of data requested.
*/
void CProtocolManager::SelfLocationReq(const TLbsNetSessionId& aSessionId, 
				const TLbsNetPosRequestOptionsBase& aOptions)
	{
	
	LBSLOG(ELogP2, "CProtocolManager::SelfLocationReq()\n");	
	const TLbsNetPosRequestOptionsAssistance options = reinterpret_cast<const TLbsNetPosRequestOptionsAssistance&> (aOptions);
	// Pass assistance data mask to the assistance data manager to process
	TBool noAssistanceNeeded = iAssistMgr->ProcessDataRequest(options.DataRequestMask());

	// Was assistance data request entirely invalid
	if (iAssistMgr->RequestErrorMask() == options.DataRequestMask() &&
		!noAssistanceNeeded)
		{
		// Ensure this error is reported
		DoAssistanceDataActions();
		noAssistanceNeeded = ETrue;
		}

	// Is there already an MO-LR active?
	if (CStateMachineBase::EStateActive == iMoLr->State())
		{
		LBSLOG(ELogP2, "CProtocolManager::SelfLocationReq() Previously we would've ignored this request!!!!!!\n");
		}

	CConfigManager::TConflictResult conflictResult = ResolveConflict(CConfigManager::EOpMoLr);
	// Resolve any conflicts arising
	switch (conflictResult)
		{
	// Start MO-LR
	case CConfigManager::EConflictNone:
		LBSLOG(ELogP2, "CProtocolManager::SelfLocationReq() No conflict\n");
		// Network interaction is not required under these circumstances:
		// (a) GPS mode is autonomous OR
		// (b) this is an old client AND no assistance data was requested
		if (TPositionModuleInfo::ETechnologyTerminal == options.PosMode() ||
			(!options.NewClientConnected() && noAssistanceNeeded))
			{
			// End the LBS session
			LBSLOG(ELogP2, "CProtocolManager::SelfLocationReq() Ending session due to autonomous OR old client not requiring ass data\n");
			Gateway()->SessionCompleteInd(aSessionId, KErrNone);
			}
		else
			{
			// Status update sent to LBS
			StatusUpdate(CConfigManager::EOpMoLr, ETrue); // ETrue=new operation
			// Need to set the assistance data manager as if no data
			// has been received yet, and all previous data has been reported,
			// to ensure that any existing set of data is reported to LBS.
			iAssistMgr->DataReported();
			iAssistMgr->SetDataNotReceived();
			iMoLr->MoLrReq(aSessionId);
			LBSLOG(ELogP2, "CProtocolManager::SelfLocationReq() CurrentOp --> EOpMoLr\n");
			iCurrentOp.iOperation = CConfigManager::EOpMoLr;
			iCurrentOp.iPriority = CConfigManager::EPriorityNone;
			}
		break;

	// Reject the new request
	case CConfigManager::EConflictRejectNew:
		LBSLOG(ELogP2, "CProtocolManager::SelfLocationReq() Conflict: Rejecting new request\n");
		Gateway()->SessionCompleteInd(aSessionId, KErrServerBusy);
		break;

	// New request is queued whilst current operation continues
	// or current operation is being cancelled.
	case CConfigManager::EConflictQueueNew:
	case CConfigManager::EConflictCancelCurrent:
	case CConfigManager::EConflictQueueCurrent:
		LBSLOG2(ELogP2, "CProtocolManager::SelfLocationReq() Conflict %d: Queueing new request\n", conflictResult);
		iMoLr->QueueMoLrReq(aSessionId);
		break;

	default:
		break;
		};
	}


/** Handle LBS completion of self locate
@param aSessionId The session to be completed.
@param aReason Completion reason value.
*/
void CProtocolManager::SelfLocationCompleteInd(const TLbsNetSessionId& aSessionId, 
				TInt aReason)
	{
	// Check we have an active self locate and valid session ID,
	// otherwise silently consume this completion indication.
	if ((CStateMachineBase::EStateActive == iMoLr->State()) &&
		(aSessionId == iMoLr->SessionId()))
		{
		iMoLr->CancelMachine(CStateMachineBase::ECancelClientCancel, aReason);
		}
	}


/** Handle LBS system status indication
@param aStatus The status indication from LBS.
*/
void CProtocolManager::SystemStatusInd(CLbsNetworkProtocolBase::TLbsSystemStatus aStatus)
	{
	iLbsStatus = aStatus;
	}


/** Handle LBS Location Response
@param aSessionId The session ID for which this response is provided.
@param aReason An error response; KErrNone if position is okay.
@param aPosInfo The location response information
*/
void CProtocolManager::LocationResp(const TLbsNetSessionId& aSessionId, TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	// Check we have an active state machine to handle this
	CStateMachineBase* activeMachine;
	GetActiveStateMachine(activeMachine);

	// Special requirement to allow MT-LR to receive late responses
	if ((NULL == activeMachine) && 
		(CStateMachineBase::EStateCancelling == iMtLr->State()))
		{
		activeMachine = iMtLr;
		}

	// Check there is an active state machine and that session ID is legitimate,
	// otherwise silently consume this location response.
	if ((NULL != activeMachine) && (aSessionId == activeMachine->SessionId()))
		{
		activeMachine->LocationResp(aReason, aPosInfo);
		}

	}


/** Handle LBS Privacy Response
@param aResponse Privacy response value.
*/
void CProtocolManager::PrivacyResp(const TLbsNetSessionId& aSessionId, 
			const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	// Check we have an active MT-LR state machine and valid session ID,
	// otherwise silently consume this privacy response.
	if ((CStateMachineBase::EStateActive == iMtLr->State()) &&
		(aSessionId == iMtLr->SessionId()))
		{
		iMtLr->PrivacyResp(aResponse);
		}
	}


/** Handle LBS Network Based Location Request
*/
void CProtocolManager::NetworkBasedLocationReq(const TLbsNetSessionId& aSessionId, 
					const TLbsNetPosRequestOptionsBase& /*aOptions*/)
	{
	LBSLOG2(ELogP2, "CProtocolManager::NetworkBasedLocationReq() session id %d\n", aSessionId.SessionNum());
	// Resolve any conflicts arising
	switch (ResolveConflict(CConfigManager::EOpNbLr)) 
		{
	// Start Network Based Location
	case CConfigManager::EConflictNone:
		// Status update sent to LBS
		StatusUpdate(CConfigManager::EOpNbLr, ETrue); // ETrue=new operation
		iNetLoc->NetLocReq(aSessionId);
		LBSLOG(ELogP2, "CProtocolManager::NetworkBasedLocationReq() CurrentOp --> EOpNbLr\n");
		iCurrentOp.iOperation = CConfigManager::EOpNbLr;
		iCurrentOp.iPriority = CConfigManager::EPriorityNone;
		break;

	// Reject the new request
	case CConfigManager::EConflictRejectNew:
		Gateway()->SessionCompleteInd(aSessionId, KErrServerBusy);
		break;

	// New request is queued whilst current operation continues
	// or current operation is being cancelled.
	case CConfigManager::EConflictQueueNew:
	case CConfigManager::EConflictCancelCurrent:
	case CConfigManager::EConflictQueueCurrent:
		LBSLOG2(ELogP2, "CProtocolManager::NetworkBasedLocationReq() Queueing NetLocrequest with session id %d\n", aSessionId.SessionNum());		
		iNetLoc->QueueNetLocReq(aSessionId);
		break;

	default:
		break;
		};
	}


/** Handle LBS Completion of a Network Based Location Request
*/
void CProtocolManager::NetworkBasedLocationCompleteInd(const TLbsNetSessionId& aSessionId, 
					TInt aReason)
	{
	LBSLOG2(ELogP2, "CProtocolManager::NetworkBasedLocationCompleteInd() session id %d\n", aSessionId.SessionNum());
	// Check we have an active Network Based Loc state machine and valid session ID,
	// otherwise silently consume this completion indication.
	if ((CStateMachineBase::EStateActive == iNetLoc->State()) &&
		(aSessionId == iNetLoc->SessionId()))
		{
		iNetLoc->CancelMachine(CStateMachineBase::ECancelClientCancel, aReason);
		}
	}


/** Handle LBS Transmit Location Request
*/
void CProtocolManager::TransmitLocationReq(const TLbsNetSessionId& aSessionId, const TDesC& aDest, TInt aPriority)
	{
	CConfigManager::TConflictPriority priority;
	switch(aPriority)
		{
	case 0x10: // Timer
		priority = CConfigManager::EPriorityLow;
		break;

	case 0x08: // Menu
		priority = CConfigManager::EPriorityMedium;
		break;

	case 0x02: // Push
		priority = CConfigManager::EPriorityHigh;
		break;

	default:
		priority = CConfigManager::EPriorityNone;
		break;
		};

	// Resolve any conflicts arising
	switch (ResolveConflict(CConfigManager::EOpX3p, priority))
		{
	// Start X3P
	case CConfigManager::EConflictNone:
		// Status update sent to LBS
		StatusUpdate(CConfigManager::EOpX3p, ETrue); // ETrue=new operation
		iX3p->X3pReq(aSessionId, aDest);
		LBSLOG(ELogP2, "CProtocolManager::TransmitLocationReq() CurrentOp --> EOpX3p\n");
		iCurrentOp.iOperation = CConfigManager::EOpX3p;
		iCurrentOp.iPriority = priority;
		break;

	// Reject the new request
	case CConfigManager::EConflictRejectNew:
		Gateway()->SessionCompleteInd(aSessionId, KErrServerBusy);
		break;

	// New request is queued whilst current operation continues
	// or current operation is being cancelled.
	case CConfigManager::EConflictQueueNew:
	case CConfigManager::EConflictCancelCurrent:
	case CConfigManager::EConflictQueueCurrent:
		iX3p->QueueX3pReq(aSessionId, aDest);
		break;

	default:
		break;
		};
	}


/** Handle LBS Completion of a Transmit Location Request
*/
void CProtocolManager::TransmitLocationCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	// Check we have an active X3P state machine and valid session ID,
	// otherwise silently consume this completion indication
	if ((CStateMachineBase::EStateActive == iX3p->State()) &&
		(aSessionId == iX3p->SessionId()))
		{
		// Cancel
		iX3p->CancelMachine(CStateMachineBase::ECancelClientCancel, aReason);
		}
	}


//
// MNetworkObserver methods
//


/** Get a new session ID
@see MNetworkObserver::NewSession()
*/
const TLbsNetSessionId& CProtocolManager::NewSessionId()
	{
	iInternalSessionId.IncrSession();
	return iInternalSessionId;
	}


/** Handle Network Measurement Control indication
@see MNetworkObserver
*/
void CProtocolManager::MeasurementControlInd(const TPositionInfoBase& aPosInfo,
		const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality,
		const TLbsNetPosRequestMethod& aPosMethod)
	{
	LBSLOG(ELogP2, "CProtocolManager::MeasurementControlInd()\n");
	// Store assistance data
	iAssistMgr->StoreData(aData);

	// Check if we have an active state machine to handle this
	CStateMachineBase* activeMachine;
	GetActiveStateMachine(activeMachine);
	CStateMachineBase* cancellingMachine;
	GetCancellingStateMachine(cancellingMachine);
	if (NULL != activeMachine)
		{
		// Is this the first measurement control received?
		if (!activeMachine->IsLocReqReceived())
			{
			// Forward the measurement and control to the state machine
			activeMachine->MeasurementControlInd(aPosInfo, aQuality, aPosMethod);
			}
		else
			{
			// Advise state machine of pending assistance data
			activeMachine->AdditionalAssistDataInd();
			}
		}
	// Any cancelling state machine?
	else if (NULL != cancellingMachine)
		{
		// Forward the measurement and control to the state machine
		cancellingMachine->MeasurementControlInd(aPosInfo, aQuality, aPosMethod);
		}
	else
		{
		// If there is no active or cancelling operation then this can be 
		// interpreted as a network induced location request (NI-LR).

		// We must perform conflict decision-making before handling this
		CConfigManager::TConflictPriority priority;
		priority = (iMtLr->IsEmergency()) ? CConfigManager::EPriorityEmergency : CConfigManager::EPriorityNone;
		switch (ResolveConflict(CConfigManager::EOpNiLr, priority))
			{
		// Start NI-LR
		case CConfigManager::EConflictNone:
			{
			// Status update sent to LBS
			StatusUpdate(CConfigManager::EOpNiLr, ETrue); // ETrue=new operation
			// The NI-LR requires a new session
			iMtLr->SessionId(NewSessionId());
			LBSLOG(ELogP2, "CProtocolManager::MeasurementControlInd() CurrentOp --> EOpNiLr\n");
			iCurrentOp.iOperation = CConfigManager::EOpNiLr;
			iCurrentOp.iPriority = priority;

			// Forward the measurement and control to the MT-LR state machine
			iMtLr->MeasurementControlInd(aPosInfo, aQuality, aPosMethod);
			}
			break;

		// Reject the new request
		case CConfigManager::EConflictRejectNew:
			{			
			CNetworkInterface::TNetworkError netError =
				Network()->LocationResp(KErrServerBusy);
			// Handle network-related error
			if (CNetworkInterface::ENone != netError)
				{
				NetworkErrorReported();
				}
			}
			break;

		// We don't do anything for other conflict responses because we
		// could only be in the process of cancelling an active operation
		// for which this measurement control becomes redundant.
		default:
			break;
			};
		}
	}


/** Handle Network Measurement Control error indication
@see MNetworkObserver
*/
void CProtocolManager::MeasurementControlErrorInd(TInt aReason)
	{
	LBSLOG2(ELogP2, "CProtocolManager::NetworkErrorInd(%d)\n", aReason);
	// Store assistance data error
	iAssistMgr->Error(aReason);

	// Check if we have an active state machine to handle this,
	// otherwise silently consume this error indication.
	CStateMachineBase* activeMachine;
	GetActiveStateMachine(activeMachine);
	if (NULL != activeMachine)
		{
		// Advise state machine of error
		activeMachine->MeasurementControlErrorInd(aReason);
		}
	}


/** Handle Network MT-LR request
@see MNetworkObserver
*/
void CProtocolManager::MtLrReq(const TLbsExternalRequestInfo& aReqInfo,
		const TLbsNetPosRequestPrivacy& aPrivacy)
	{
	// Resolve any conflicts arising
	CConfigManager::TConflictPriority priority;
	priority = (iMtLr->IsEmergency()) ? CConfigManager::EPriorityEmergency : CConfigManager::EPriorityNone;
	switch (ResolveConflict(CConfigManager::EOpMtLr, priority))
		{
	// Start MT-LR
	case CConfigManager::EConflictNone:
		// Status update sent to LBS
		StatusUpdate(CConfigManager::EOpMtLr, ETrue); // ETrue=new operation
		iMtLr->MtLrReq(aReqInfo, aPrivacy);
		LBSLOG(ELogP2, "CProtocolManager::MtLrReq() CurrentOp --> EOpMtLr\n");
		iCurrentOp.iOperation = CConfigManager::EOpMtLr;
		iCurrentOp.iPriority = priority;
		break;

	// Reject the new request
	case CConfigManager::EConflictRejectNew:
		Network()->MtLrResp(CLbsNetworkProtocolBase::EPrivacyResponseRejected);
		break;

	// New request is queued whilst current operation continues
	// or current operation is being cancelled.
	case CConfigManager::EConflictQueueNew:
	case CConfigManager::EConflictCancelCurrent:
	case CConfigManager::EConflictQueueCurrent:
		iMtLr->QueueMtLrReq(aReqInfo, aPrivacy);
		break;

	default:
		break;
		};
	}


/** Handle Network MT-LR cancel indication
@see MNetworkObserver
*/
void CProtocolManager::MtLrCancelInd(TInt aReason)
	{
	// If we have an active state machine then forward the indication,
	// otherwise silently consume this cancel indication.
	if (CStateMachineBase::EStateActive == iMtLr->State())
		{
		iMtLr->MtLrCancelInd(aReason);
		}
	}


/** Handle Network session result indication
@see MNetworkObserver
*/
void CProtocolManager::NetworkResultInd(TInt aResult, const TPositionInfoBase* aPosInfo)
	{
	LBSLOG2(ELogP2, "CProtocolManager::NetworkErrorInd() Result = %d\n", aResult);
	// Check we have an active state machine to handle this,
	// otherwise silently consume this indication.
	CStateMachineBase* activeMachine;
	GetActiveStateMachine(activeMachine);

    if (NULL != activeMachine)
    	{
    	// Pass the indication on to the active state machine
    	if (CStateMachineBase::EStateActive == iMoLr->State())
			{
			iMoLr->SessionResult(aResult, aPosInfo);
			}

		if (CStateMachineBase::EStateActive == iX3p->State())
			{
			iX3p->SessionResult(aResult, aPosInfo);
			}
    	}
	}


/** Handle Network error indication
@see MNetworkObserver
*/
void CProtocolManager::NetworkErrorInd(TInt aReason)
	{
	LBSLOG2(ELogP2, "CProtocolManager::NetworkErrorInd(%d)\n", aReason);
	// Ensure any active state machine is informed of this problem
	CStateMachineBase* stateMachine;
	GetActiveStateMachine(stateMachine);
	if (NULL != stateMachine)
		{
		// Advise state machine of error
		stateMachine->NetworkErrorInd();

		// Store assistance data error. This is only reported if the
		// state machine indicates that assistance data actions should
		// be performed.
		if (stateMachine->IsAssistanceDataActionReq())
			{
			iAssistMgr->Error(aReason);
			}
		}
	}


/** Network request for LBS capabilities
@see MNetworkObserver
*/
void CProtocolManager::GetCapabilities(TLbsNetPosCapabilities& aCapabilities)
	{
	Gateway()->GetCapabilities(aCapabilities);
	}

/** Network request for reseting of assistance data
@see MNetworkObserver
*/
void CProtocolManager::ResetAssistanceData(TLbsAssistanceDataGroup aMask)
	{
	iAssistMgr->ResetData();
	Gateway()->ResetAssistanceData(aMask, iAssistMgr->ValidData());
	}

// MStateMachineObserver methods

/** Gateway interface pointer.
@see MStateMachineObserver
*/
MProtocolMgrObserver* CProtocolManager::Gateway()
	{
	return &iGateway;
	}


/** Network interface pointer.
@see MStateMachineObserver
*/
CNetworkInterface* CProtocolManager::Network()
	{
	return iNetwork;
	}


/** State machine procedure complete notification
@see MStateMachineObserver
*/
void CProtocolManager::ProcedureCompleteInd()
	{
	LBSLOG(ELogP2, "CProtocolManager::ProcedureCompleteInd() CurrentOp --> None\n");
	// Status update sent to LBS
	StatusUpdate(iCurrentOp.iOperation, EFalse); // EFalse=operation ended

	iCurrentOp.iOperation = CConfigManager::EOpNone;
	iCurrentOp.iPriority = CConfigManager::EPriorityNone;

	// Identify if any request was queued and if this is the case
	// start the relevant state machine.
	CStateMachineBase* queuedMachine;
	GetQueuedStateMachine(queuedMachine);
	if (NULL != queuedMachine)
		{
		// Set the current operation appropriately:
		if(queuedMachine == iMtLr)
			{
			iCurrentOp.iOperation = CConfigManager::EOpMtLr;
			}
		else if(queuedMachine == iMoLr)
			{
			iCurrentOp.iOperation = CConfigManager::EOpMoLr;
			}
		else if(queuedMachine == iX3p)
			{
			// TO DO - how do we know what priority to use here!?
			iCurrentOp.iOperation = CConfigManager::EOpX3p;
			}
		else
			{
			ASSERT(queuedMachine == iNetLoc);
			iCurrentOp.iOperation = CConfigManager::EOpNbLr;
			}
			
		// Set Current Operation appropriately:
		queuedMachine->StartQueuedMachine();
		}
	}


/** Handle Measurement Control timeout
@see MStateMachineObserver
*/
void CProtocolManager::MeasurementControlTimeout()
	{
	// Store assistance data error
	iAssistMgr->Error(KErrTimedOut);

	// Check if we have an active state machine to handle this,
	// otherwise silently consume this indication.
	CStateMachineBase* activeMachine;
	GetActiveStateMachine(activeMachine);
	if (NULL != activeMachine)
		{
		// Advise state machine of error
		activeMachine->MeasurementControlTimeout();
		}
	}


/** Handle network error reported
@see MStateMachineObserver
*/
void CProtocolManager::NetworkErrorReported()
	{
	NetworkErrorInd(KErrDisconnected);
	}


/** Perform assistance data request action.
This is called by state machines when they are in suitable state to
handle assistance data actions.

The actions will arises from preceding assistance data requests and responses
which have been processed by the assistance data manager.
@see CAssistDataMgr::ProcessDataRequest()
@see CAssistDataMgr::StoreData()
@see CAssistDataMgr::Error()

@see MStateMachineObserver
*/
void CProtocolManager::DoAssistanceDataActions()
	{
	// Is there a data response error to be reported to LBS?
	if (iAssistMgr->IsErrorToBeReported())
		{
		// note: send any available valid data anyway
		Gateway()->AssistanceDataInd(iAssistMgr->ErrorGroupMask(), iAssistMgr->ValidData(), iAssistMgr->ErrorReason());
		iAssistMgr->ErrorReported();
		}

	// Is there valid data to be reported to LBS?
	if (iAssistMgr->IsDataToBeReported())
		{
		Gateway()->AssistanceDataInd(iAssistMgr->ValidGroupMask(), iAssistMgr->ValidData(), KErrNone);
		iAssistMgr->DataReported();
		}

	// Is there a request error to be reported to LBS?
	if (iAssistMgr->IsRequestErrorToBeReported())
		{
		Gateway()->AssistanceDataInd(iAssistMgr->RequestErrorMask(), iAssistMgr->ValidData(), KErrArgument);
		iAssistMgr->RequestErrorReported();
		}

	// Is data required from the network?
	if (iAssistMgr->IsDataToBeRequested())
		{
		iAssistMgr->SendingRequest();
		Network()->MeasurementControlReq(iAssistMgr->RequestMask());
		// Start the timer for receiving additional assistance data
		CStateMachineBase* activeMachine;
		GetActiveStateMachine(activeMachine);
		if (NULL != activeMachine)
			{
			activeMachine->StartAssistDataTimer();
			}
		iAssistMgr->RequestSent();
		}
	}



/** Inform LBS of status update.
This is called when a new operation starts or when a current operation ends.

@param aOperation Indicates the type of operation
@param aIsOperationStarting ETrue if operation is starting
*/
void CProtocolManager::StatusUpdate(
					CConfigManager::TConflictOp aOperation, 
					TBool aIsOperationStarting)
	{
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask mask = MLbsNetworkProtocolObserver::EServiceNone;
	switch (aOperation)
		{
	case CConfigManager::EOpMoLr:
		mask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
		break;
	case CConfigManager::EOpNbLr:
		mask = MLbsNetworkProtocolObserver::EServiceNetworkLocation;
		break;
	case CConfigManager::EOpMtLr:
		mask = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
		break;
	case CConfigManager::EOpNiLr:
		mask = MLbsNetworkProtocolObserver::EServiceNetworkInduced;
		break;
	case CConfigManager::EOpX3p:
		mask = MLbsNetworkProtocolObserver::EServiceTransmitThirdParty;
		break;

	case CConfigManager::EOpNone:
	default:
		break;
		};

	if (aIsOperationStarting)
		{
		iActiveServiceMask |= mask;
		}
	else
		{
		iActiveServiceMask &= ~mask;
		}
#pragma message ("Status updates ommitted for now")
	// Gateway()->StatusUpdate(iActiveServiceMask);
	}
