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
// This file defines the class that manages protocol aspects
// of the SUPL Protocol Module operation.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __SUPLPROTOCOLMANAGER_H__
#define __SUPLPROTOCOLMANAGER_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <etelmm.h>
#include <lbs/lbssuplpushreceiver.h>
#include "suplconflictmanager.h"
#include "suplmolrfsmsession.h"
#include "suplsessioncompleter.h"
#include "suplnetworkinfohandler.h"

class CSuplFsmSessionBase;
class CSuplMolrFsmSession;
class CSuplMtlrFsmSession;
class CSuplAssistanceDataManager;
class CSuplInit;

class MSuplFsmSessionObserver;

const TUint KSuplv1UidValue		= 0x10285A9C;

/** We only support SUPL V1 at the moment.
Future major revisions that are supported by this implementation will
need their version numbers stored in the array further down.
The definition for the version numbers can go here.
@see CSuplProtocolManager
*/
const TUint8 KSupportedSuplVersion_001 = 1;

/** SUPL Protocol Manager observer mixin definition.
This interface is implemented by observers to handle calls from
the Protocol Manager.
This is currently only implemented by the Gateway Interface.
@see CSuplProtocolManager
*/
class MSuplProtocolManagerObserver
	{
public:

	/** Send privacy request to the observer.
	@param aSessionId Unique session ID for this request
	@param aEmergency Indicates an emergency request when ETrue
	@param aPrivacy Privacy request information
	@param aRequestInfo External requestor information
	*/
	virtual void PrivacyReq(const TLbsNetSessionId& aSessionId, TBool aEmergency, 
					const TLbsNetPosRequestPrivacy& aPrivacy,
					const TLbsExternalRequestInfo& aRequestInfo) = 0;

	/** Send network-based location to the observer.
	@param aSessionId Unique session ID
	@param aReferenceLocation The reference location position
	*/
	virtual void NetworkLocationInd(const TLbsNetSessionId& aSessionId, 
					const TPositionInfoBase& aPosInfo) = 0;

	/** Send assistance data to the observer.
	@param aGroupMask This identifies the data group relevant for this indication.
	@param aData Assistance data set
	@param aReason An error value for the assistance data
	*/
	virtual void AssistanceDataInd(const TLbsAsistanceDataGroup& aGroupMask,
					const RLbsAssistanceDataBuilderSet& aData, TInt aReason) = 0;

	/** Send location request to the observer.
	@param aSessionId Unique session ID for this request
	@param aEmergency Indicates an emergency request when ETrue
	@param aType Identifies the type of request originator
	@param aQuality The required location quality value.
	@param aMethod The method to employ to obtain location
	*/
	virtual void LocationReq(const TLbsNetSessionId& aSessionId, 
					const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aType, 
					const TLbsNetPosRequestQuality& aQuality,
					const TLbsNetPosRequestMethod& aPosMethod) = 0;

	/** Send session complete indication to the observer.
	@param aSessionId Unique session ID
	@param aReason An error or reason value for the session closing
	*/
	virtual void SessionCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason) = 0;

	/** Request to get LBS capabilities from the observer.
	@param aCapabilities Object to hold capabilities returned by LBS
	*/
	virtual void GetCapabilities(TLbsNetPosCapabilities& aCapabilities) = 0;

	/** Send Status Update to the observer.
	@param aActiveServiceMask The active services are represented by this mask.
	*/
	virtual void StatusUpdate(MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask aActiveServiceMask) = 0;
	};
	
/** SUPL Protocol Manager class.
This class manages protocol aspects of the protocol module operation.
It employs various state machines to oversee individual protocol
procedures and transfers requests, responses and indications to these
state machines.
@see CSuplMolrFsmSession
@see CSuplMtlrFsmSession


The protocol manager implements the MSuplFsmSessionObserver interface to
receive messages from state machines.
@see MSuplFsmSessionObserver

The protocol manager is created by the Gateway Interface and uses the
MSuplProtocolManagerObserver interface to communicate with it.
@see MSuplProtocolManagerObserver

@see MNetworkObserver

The class also owns a CSuplAssistanceDataManager assistance data manager object.
This is used to oversee all assistance data requests and responses.
@see CSuplAssistanceDataManager
*/
NONSHARABLE_CLASS(CSuplProtocolManager): public CBase, public MSuplFsmSessionObserver,
										 public MSuplNetworkInfoObserver,
										 public MLbsSuplPushRecObserver
	{

	// SUPL sessions that can be conducted simultaneously. For SUPL V1 these are
	// MTLR and MOLR. Note that LBS may request two types of MOLR sessions: one of 
	// behalf of clients that want to use AGPS positioning and another on behalf of
	// clients that are only interested in CellBased positioning, therefore two
	// instance of the MOLR state machine may have to be instantiated.
	//
	enum TSuplSessionTypes
	{
		ESuplSessionMolr = 0,
		ESuplSessionCellBasedMolr,
		ESuplSessionMtlr,

		// Keep this last (index limit for array traversal)
		ESuplSessionLastType
	};

public:

	static CSuplProtocolManager* NewL(MSuplProtocolManagerObserver& aGateway);
	~CSuplProtocolManager();

    // --------------------------- Methods called by the Network Gateway Interface -----------------------

	void SelfLocationReq(const TLbsNetSessionId& aSessionId, const TLbsNetPosRequestOptionsBase& aOptions);
	void SelfLocationCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason);
	void SystemStatusInd(CLbsNetworkProtocolBase::TLbsSystemStatus aStatus);
	void LocationResp(const TLbsNetSessionId& aSessionId, TInt aReason, const TPositionInfoBase& aPosInfo);
    void PrivacyResp(const TLbsNetSessionId& aSessionId, const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse, TInt aReason);
	void NetworkBasedLocationReq(const TLbsNetSessionId& aSessionId, const TLbsNetPosRequestOptionsBase& aOptions);
	void NetworkBasedLocationCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason);
	void AssistanceDataReq(TLbsAsistanceDataGroup aDataRequestMask);
	void TransmitLocationReq(const TLbsNetSessionId& aSessionId, const TDesC& aDestination,
							 TInt aPriority, const TLbsNetPosRequestOptionsBase& aOptions);
	void TransmitLocationCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason);

	// ----- MSuplFsmSessionObserver derived methods (called by state machines and state handlers)--------

	MSuplProtocolManagerObserver& Gateway();
	CSuplConnectionManager& ConnectionManager();
	const TLbsNetSessionId& NewSessionId();
	void ProcedureCompleteInd(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aService);
	void LocationReq(const TLbsNetSessionId& aSessionId, 
					 const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aType, 
					 const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod);
	void PrivacyReq(const TLbsNetSessionId& aSessionId,
					const TLbsNetPosRequestPrivacy& aPrivacy,
				    const TLbsExternalRequestInfo& aRequestInfo);
	void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, 
							   const RLbsAssistanceDataBuilderSet& aData,const TInt& aReason,
							   const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aService);

	// ---------------------------- MLbsSuplPushRecObserver derived methods -------------------------------

	void OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId, TDesC8& aMsg);


	// ---------------------------- MSuplNetworkInfoObserver derived methods -------------------------------
	
	void NetInfoResults(const RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
						   const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	void HomeMccMncResult(TUint aMcc, TUint aMnc);
	void MsisdnResult(const TDesC& aTelNumber);
	void CellInfoResults(const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo);

private:

	CSuplProtocolManager(MSuplProtocolManagerObserver& aGateway);
	void ConstructL();

	void StatusUpdate(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aService,const TBool& aIsOperationStarting);

	CSuplConflictManager::TConflictResult ResolveConflict(
					const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aNewOperation, 
					const TLbsNetPosRequestOptionsBase& aNewOperationOptions);
	
	void ProcessSuplInitL(TDesC8& aMsg);
	void NetworkLocationReq(CSuplMessageBase* aSuplInit);
	
private:



	/** Gateway interface reference.
	*/
	MSuplProtocolManagerObserver& iGateway;

	/** Conflict Manager object pointer.
	This object is created and owned by this class
	*/
	CSuplConflictManager* iConflictManager;

	/** Connection Manager object pointer.
	This object is created and owned by this class
	*/
	CSuplConnectionManager* iConnectionManager;

	/** MO-LR state machine object pointer.
	This object is created and owned by this class
	*/
	CSuplMolrFsmSession* iMoLr;

	/** MT-LR state machine object pointer.
	This object is created and owned by this class
	*/
	CSuplMtlrFsmSession* iMtLr;
	
	/** SUPL Push Receiver
	Receives notification of SUPL messages that have
	arrived via WAP/SMS push, specifically SUPL INIT messages.
	This object is created and owned by this class
	*/
	CLbsSuplPushRec* iPushRec;
	
	/** Network Based Location state machine object pointer.
	This object is created and owned by this class
	*/
	CSuplMolrFsmSession* iNetLoc;

	/** Assistance data manager.
	This object is created and owned by this class
	*/
	CSuplAssistanceDataManager* iAssistMgr;
	
	/** Session Completer Active Object
	Used to asynchronously terminate session that
	don't really require SUPL protocol usage.
	*/
	CSuplSessionCompleter* iSessionCompleter;

	/** System Status session ID
	This holds the latest status information reported by LBS
	(tracking/non-tracking)
	*/
	CLbsNetworkProtocolBase::TLbsSystemStatus iLbsStatus;

	/** Service status
	This holds the latest service status information known by the module.
	*/
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask iActiveServiceMask;
	
	// New session IDs
	TLbsNetSessionId iInternalSessionId;
	
	/** Active object to obtain network infomation from ETEL
	*/
	CSuplNetworkInfoHandler* iNetInfoHandler;
	
	/** Supported Major Versions Array
	We only support SUPL V1 at the moment, the array below
	is to allow for future supported major version numbers
	to be recorded and tested against incoming packets.
	*/
	RArray<TUint8> iSupportedVersions;
	};



#endif // __SUPLPROTOCOLMANAGER_H__
