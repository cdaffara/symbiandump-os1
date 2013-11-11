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
// This file defines the class that manages protocol aspects
// of Test Protocol Module operation.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CPROTOCOLMANAGER_H__
#define __CPROTOCOLMANAGER_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "cconfigmanager.h"
#include "cnetworkinterface.h"

class CStateMachineBase;
class CMoLrStateMachine;
class CMtLrStateMachine;
class CX3pStateMachine;
class CNetLocStateMachine;
class CAssistDataMgr;
class MProtocolMgrObserver;

/** State machine observer mixin definition.
This defines an interface to be implemented by an observer of state machines.
In this case it is only the Protocol Manager that implements this.
*/
class MStateMachineObserver
	{
public:
		
	/** Gateway interface pointer.
	@return MProtocolMgrObserver* A pointer to the gateway interface
	*/
	virtual MProtocolMgrObserver* Gateway() = 0;

	/** Network interface pointer.
	@return CNetworkInterface* A pointer to the Network interface
	*/
	virtual CNetworkInterface* Network() = 0;

	/** Do outstanding actions relating to assistance data.
	*/
	virtual void DoAssistanceDataActions() = 0;

	/** State machine procedure complete notification.
	*/
	virtual void ProcedureCompleteInd() = 0;

	/** State machine measurement control timeout notification.
	*/
	virtual void MeasurementControlTimeout() = 0;

	/** State machine reports network error.
	*/
	virtual void NetworkErrorReported() = 0;

	/** State machine requests a new session.
	A new ID is assigned when this new session is created. This is called
	when the protocol module needs to initiate a new session.
	@return const TLbsNetSessionId& The new session ID.
	*/
	virtual const TLbsNetSessionId& NewSessionId() = 0;

	};
	

/** Protocol Manager class.
This class manages protocol aspects of the protocol module operation.
It employs various state machines to oversee individual protocol
procedures and transfers requests, responses and indications to these
state machines.
@see CMoLrStateMachine
@see CMtLrStateMachine
@see CX3pStateMachine
@see CNetLocStateMachine

The protocol manager implements the MStateMachineObserver interface to
receive messages from state machines.
@see MStateMachineObserver

The protocol manager is created by the Gateway Interface and uses the
MProtocolMgrObserver interface to communicate with it.
@see MProtocolMgrObserver

The protocol manager implements the MNetworkObserver interface to receive
requests and responses from the Network Interface.
@see MNetworkObserver

The class also owns a CAssistDataMgr assistance data manager object.
This is used to oversee all assistance data requests and responses.
@see CAssistDataMgr
*/
NONSHARABLE_CLASS(CProtocolManager)
 : public CBase, public MStateMachineObserver, public MNetworkObserver
	{
public:

	static CProtocolManager* NewL(MProtocolMgrObserver& aGateway);
	~CProtocolManager();

	CConfigManager::TConflictResult ResolveConflict(
					CConfigManager::TConflictOp aNewOp, 
					CConfigManager::TConflictPriority aNewPriority = CConfigManager::EPriorityNone);

	void AssistanceDataReq(TLbsAsistanceDataGroup aDataRequestMask);
	void LocationResp(const TLbsNetSessionId& aSessionId, TInt aReason, const TPositionInfoBase& aPosInfo);
	void PrivacyResp(const TLbsNetSessionId& aSessionId, const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse);
	void NetworkBasedLocationReq(const TLbsNetSessionId& aSessionId, const TLbsNetPosRequestOptionsBase& aOptions);
	void NetworkBasedLocationCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason);
	void SelfLocationReq(const TLbsNetSessionId& aSessionId, const TLbsNetPosRequestOptionsBase& aOptions);
	void SelfLocationCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason);
	void TransmitLocationReq(const TLbsNetSessionId& aSessionId, const TDesC& aDest, TInt aPriority);
	void TransmitLocationCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason);
	void SystemStatusInd(CLbsNetworkProtocolBase::TLbsSystemStatus aStatus);


	// MStateMachineObserver derived methods
		
	MProtocolMgrObserver* Gateway();
	CNetworkInterface* Network();
	void DoAssistanceDataActions();
	void ProcedureCompleteInd();
	void MeasurementControlTimeout();
	void NetworkErrorReported();

	const TLbsNetSessionId& NewSessionId();

	// MNetworkObserver derived methods
	
	void MeasurementControlInd(const TPositionInfoBase& aPosInfo,
		const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality,
		const TLbsNetPosRequestMethod& aPosMethod);
	void MeasurementControlErrorInd(TInt aReason);
	void MtLrReq(const TLbsExternalRequestInfo& aReqInfo,
		const TLbsNetPosRequestPrivacy& aPrivacy);
	void MtLrCancelInd(TInt aReason);
	void NetworkResultInd(TInt aResult, const TPositionInfoBase* aPosInfo);
	void NetworkErrorInd(TInt aReason);
	void GetCapabilities(TLbsNetPosCapabilities& aCapabilities);
	void ResetAssistanceData(TLbsAssistanceDataGroup aMask);
	
private:

	CProtocolManager(MProtocolMgrObserver& aGateway);
	void ConstructL();

	void GetActiveStateMachine(CStateMachineBase*& aActiveMachine) const;
	void GetCancellingStateMachine(CStateMachineBase*& aCancellingMachine) const;
	void GetQueuedStateMachine(CStateMachineBase*& aQueuedMachine) const;

	void StatusUpdate(CConfigManager::TConflictOp aNewOp, 
					TBool aIsOperationStarting);

private:

	/** Gateway interface reference.
	*/
	MProtocolMgrObserver& iGateway;

	/** Configuration manager object pointer.
	This object is created and owned by this class
	*/
	CConfigManager* iConfig;


	/** Current active operation.
	*/
	CConfigManager::TConflictOperation iCurrentOp;

	/** Network interface object pointer.
	This object is created and owned by this class
	*/
	CNetworkInterface* iNetwork;

	/** MO-LR state machine object pointer.
	This object is created and owned by this class
	*/
	CMoLrStateMachine* iMoLr;

	/** MT-LR state machine object pointer.
	This object is created and owned by this class
	*/
	CMtLrStateMachine* iMtLr;

	/** X3P state machine object pointer.
	This object is created and owned by this class
	*/
	CX3pStateMachine* iX3p;

	/** Network Based Location state machine object pointer.
	This object is created and owned by this class
	*/
	CNetLocStateMachine* iNetLoc;

	/** Assistance data manager.
	This object is created and owned by this class
	*/
	CAssistDataMgr* iAssistMgr;

	/** Internal session ID
	This uniquely identifies the latest client session which has been
	initiated by the protocol module. 
	*/
	TLbsNetSessionId iInternalSessionId;

	/** System Status session ID
	This holds the latest status information reported by LBS and is
	used when performing conflict decisions.
	*/
	CLbsNetworkProtocolBase::TLbsSystemStatus iLbsStatus;

	/** Service status
	This holds the latest service status information known by the module.
	*/
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask iActiveServiceMask;
	};


/** Protocol Manager observer mixin definition.
This interface is implemented to handle calls from the protocol manager
and this is currently only implemented by the Gateway Interface.
*/
class MProtocolMgrObserver
	{
public:

	/** Send privacy request to the gateway observer.
	@param aSessionId Unique session ID for this request
	@param aEmergency Indicates an emergency request when ETrue
	@param aPrivacy Privacy request information
	@param aRequestInfo External requestor information
	*/
	virtual void PrivacyReq(const TLbsNetSessionId& aSessionId, TBool aEmergency, 
					const TLbsNetPosRequestPrivacy& aPrivacy,
					const TLbsExternalRequestInfo& aRequestInfo) = 0;

	/** Send network-based location to the gateway observer.
	@param aSessionId Unique session ID
	@param aReferenceLocation The reference location position
	*/
	virtual void NetworkLocationInd(const TLbsNetSessionId& aSessionId, 
					const TPositionInfoBase& aPosInfo) = 0;

	/** Send assistance data to the network observer.
	@param aGroupMask This identifies the data group relevant for this indication.
	@param aData Assistance data set
	@param aReason An error value for the assistance data
	*/
	virtual void AssistanceDataInd(const TLbsAsistanceDataGroup& aGroupMask,
					const RLbsAssistanceDataBuilderSet& aData, TInt aReason) = 0;

	/** Send location request to the gateway observer.
	@param aSessionId Unique session ID for this request
	@param aEmergency Indicates an emergency request when ETrue
	@param aType Identifies the type of request originator
	@param aQuality The required location quality value.
	@param aMethod The method to employ to obtain location
	*/
	virtual void LocationReq(const TLbsNetSessionId& aSessionId, TBool aEmergency,
					const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aType, 
					const TLbsNetPosRequestQuality& aQuality,
					const TLbsNetPosRequestMethod& aPosMethod) = 0;

	/** Send session complete indication to the gateway observer.
	@param aSessionId Unique session ID
	@param aReason An error or reason value for the session closing
	*/
	virtual void SessionCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason) = 0;

	/** Request to get LBS capabilities from the gateway observer.
	@param aCapabilities Object to hold capabilities returned by LBS
	*/
	virtual void GetCapabilities(TLbsNetPosCapabilities& aCapabilities) = 0;

	/** Send Status Update to the gateway observer.
	@param aActiveServiceMask The active services are represented by this mask.
	*/
	virtual void StatusUpdate(MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask aActiveServiceMask) = 0;
	
	/** Send a request to reset assistance data
	@param aMask The mask representing what assistance data to reset
	*/
	virtual void ResetAssistanceData(TLbsAssistanceDataGroup aMask, const RLbsAssistanceDataBuilderSet& aData) = 0;

	};
	
#endif // __CPROTOCOLMANAGER_H__
