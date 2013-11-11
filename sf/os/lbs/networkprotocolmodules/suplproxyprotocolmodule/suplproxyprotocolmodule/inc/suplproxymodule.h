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
//

#ifndef __SUPLPROXYMODULE_H__
#define __SUPLPROXYMODULE_H__

#include <e32base.h>

#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/supltiapi.h>

#include "suplproxyprotocol.h"
#include "suplproxyserver.h"
#include "suplproxyprivacyrequestinfo.h"

// Used to store the MOLR/MTLR TLbsNetSessionIds as they are created and info about any outstanding pos requests
class CLbsNetSession: public CBase
/**
This class is used to store an outstanding request, for completion when response received
@internal
@released
 */
	{
public:
	// the type of request
	enum TRequesttype
		{
		EPosRequestTypeNone,			// no async pos request outstanding 
		EPosRequestTypeGetPosition,		// getpositionl request outstanding
		EPosRequestTypeRrlpGetLocation,	// outstanding request to get location from rrlp
		EPosRequestTypeSuplInfoAssData, // getsuplinfoL request requiring assistance data mask outstanding
		};
	
	inline ~CLbsNetSession(){;}
	
	inline CLbsNetSession(const TLbsNetSessionId aSessionId, MLbsNetworkProtocolObserver2::TLbsNetProtocolService aService):iSessionId(aSessionId),iService(aService)
			{iResponseTimestamp.UniversalTime(); TTimeIntervalDays days(5); iResponseTimestamp -= days;}
	
	inline const TLbsNetSessionId SessionId() const {return iSessionId;}
	inline const RMessage2 Msg() {return iPosResponseMsg;}
	inline const TRequesttype Type() {return iPosRequestType;}
	inline const TLbsNetPosRequestQuality Quality() {return iQuality;}
	inline const MLbsNetworkProtocolObserver2::TLbsNetProtocolService Service() {return iService;}
	inline const TLbsAssistanceDataGroup GetAssistanceDataMask() {return iAssistanceDataMask;}
	inline const TLbsNetPosRequestMethod PosMethod() {return iRequestedPosMethod;}
	inline void SetType(TRequesttype aSetType) {iPosRequestType = aSetType;}
	inline void SetMsg(RMessage2 aMessage) {iPosResponseMsg = aMessage;}
	inline void SetQuality(const TLbsNetPosRequestQuality& aQuality) {iQuality = aQuality;}
	inline void SetAssistanceDataMask(TLbsAsistanceDataGroup aDataMask) {iAssistanceDataMask = aDataMask;}
	inline void SetPosMethod(const TLbsNetPosRequestMethod& aPosMethod) {iRequestedPosMethod = aPosMethod;}
	inline void SetPosProtocolResponseObserver(MPosProtocolResponseObserver* aObserver) {iPosProtocolResponseObserver = aObserver;}
	inline void SetAssistanceDataMaskRequested(){iAssistanceDataMaskRequested = ETrue;}
	inline const TBool AssistanceDataMaskRequested(){return iAssistanceDataMaskRequested;}
	inline MPosProtocolResponseObserver* PosProtocolResponseObserver() {return iPosProtocolResponseObserver;}
	inline void  SetSessionStarted() {iSessionStarted = ETrue;}
	inline TBool IsSessionStarted() {return iSessionStarted;}
	inline void  SetSessionCompleteStatus(TBool aStatus) 
		{
		iSessionEnded = aStatus;
		if(iSessionEnded)
			{
			iSessionStarted = EFalse;
			}
		}
	inline TBool SessionCompleteStatus() {return iSessionEnded;}
    inline void  SetSessionCompleteErr(TInt aResult) {iSessionCompleteErr = aResult;}
    inline TInt  SessionCompleteErr() {return iSessionCompleteErr;}
    inline TBool GetPositionCalled(){return iGetPositionCalled;}
    inline void SetGetPositionCalled(){iGetPositionCalled = ETrue;}
	inline void SetPosSessionStarted(TBool aStatus){iPosSessionStarted = aStatus;}
	inline TBool PosSessionStarted(){return iPosSessionStarted;}
    inline void SetResponseTimeStamp(const TTime aTimeStamp){iResponseTimestamp = aTimeStamp;}
    inline TTime GetResponseTimeStamp(){return iResponseTimestamp;}
    inline void SetExtRequestInfo(const TLbsExternalRequestInfo2& aExtRequestInfo){iExtRequestInfo = aExtRequestInfo;}
    inline TLbsExternalRequestInfo2 GetExtRequestInfo(){return iExtRequestInfo;}
    
	// match sessionid
	static TBool IsSessionMatch(
		const TLbsNetSessionId* aSessionId,
		const CLbsNetSession& aNetSession)
		{
		return (*aSessionId == aNetSession.SessionId());
		}	
	
	// match on sessionid number
	static TBool IsSessionNumMatch(
		const TInt* aSessionNum,
		const CLbsNetSession& aNetSession)
		{
		return (*aSessionNum == aNetSession.SessionId().SessionNum());
		}
private:
	// requested assistance data
	TLbsAsistanceDataGroup iAssistanceDataMask;
	// desired quality
	TLbsNetPosRequestQuality	iQuality;
	// the session id used by lbs subsystem (request id used by supl fw)
	TLbsNetSessionId iSessionId;
	// the type of service this session is for (eg EServiceSelfLocation)
	MLbsNetworkProtocolObserver2::TLbsNetProtocolService	iService;
	// the type of request outstanding, if any
	TRequesttype	iPosRequestType;	
	// the message to complete when we get an lbs location response (to a direct request using GetPositionL()) 
	RMessage2	iPosResponseMsg;
	// handle to the observer (subsession) for relaying an lbs rrlp location response/new assistance data req.
	MPosProtocolResponseObserver* iPosProtocolResponseObserver;
  	// positioning method requested by client (SUPL FW) if any
	TLbsNetPosRequestMethod	iRequestedPosMethod;
	// Flag which gets set on the session initialise call
	TBool iSessionStarted;
	// the status flag that indicates if SessionEnd has arrived before the position is obtained through the TI API callback. 
	TBool iSessionEnded;
	// holds the reason why a session is being completed
	TInt iSessionCompleteErr;
	// whether the assistance data mask has been requested for this session 
	// helps to determine that we are doing SUPL 2.0 use case
	TBool	iAssistanceDataMaskRequested;
	// whether GetPositionL has been called on this session
	TBool	iGetPositionCalled;
	// whether the positioning session has been started or not
	TBool   iPosSessionStarted;
	// time at which the SPPM received an 'accept' response from LBS for a privacy verification request 
	TTime	iResponseTimestamp;
	// Requestor id. Used to match a verification with a subsequent post-verify notification (SUPL FW always sends both for a single privacy request!)
	TLbsExternalRequestInfo2 iExtRequestInfo;
	};


/** 
LBS Network Protocol Interface 
This class implements the LBS Network Gateway plug-in interface which
allows the module to be accessed by, and to interact with, LBS.
@see CLbsNetworkProtocolBase
@see MLbsNetworkProtocolObserver2

The class also implements the internal MSuplProxyPrivacyProtocol interface
which is used to receive calls from network privacy API clients via the server session.
@see MSuplProxyPrivacyProtocol

and the class implements the internal MSuplProxyPosProtocol interface which is used to receive calls
 from the SUPL POS msg API clients via the server session
 @see MSuplProxyPosProtocol
 */
NONSHARABLE_CLASS(CSuplProxyProtocol) : public CLbsNetworkProtocolBase2,
        public MSuplProxyPrivacyProtocol,
        public MSuplProxyPosProtocol,
        public MLbsSuplTiObserver
                    
    {
	
public:

	static CSuplProxyProtocol* NewL(TAny* aParams);
	~CSuplProxyProtocol();

	// MSuplProxyPrivacyProtocol methods:
	void CancelVerifyLocationRequest(CCancelSuplProxyPrivacyRequestInfo* aCancelInfo);
	void GetNextSessionId(TLbsNetSessionId& aSessionId);
	void NotifyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo);
	void NotifyVerificationTimeout(CSuplProxyPrivacyRequestInfo* aRequestInfo);
	void VerifyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo);
	void PrivacyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo);
	void CompletePrivacyRequest(TUint32 aRequestId, TInt aReason);
	void PrivacySessionDisconnected(const CSession2* aSession, TInt aReason);
	void EndPrivacySession(TInt aRequestId, TInt aReason);
	
	// MSuplProxyPosProtocol methods:
	TInt NetworkGetPosition(const RMessage2& aMessage, TInt aRequestId);
	TInt RrlplLocationRequest(const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod, TInt aRequestId);
	TInt CancelNetworkGetPosition(const RMessage2& aMessage, TInt aRequestId);
	void NotifySubSessionOpen(MPosProtocolResponseObserver* aObserver, TInt aRequestId);
    void NotifySubSessionReInit(MPosProtocolResponseObserver* aObserver, TInt aRequestId);
    TInt NotifySubSessionClose(TInt aRequestId);
    TInt NotifySessionEnd(TInt aRequestId);
	void GetCurrentPosCapabilities(TLbsNetPosCapabilities& aPosCaps);
	TInt GetAssistanceDataMask(TInt aRequestId);
	void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, const RLbsAssistanceDataBuilderSet& aBuilderSet, TInt aReason, TInt aRequestId);
	void SetPosMethod(const TLbsNetPosRequestMethod& aMethod, TInt aRequestId);
	
	// MLbsSuplTiObserver method
	void RequestComplete(TInt aReason, const TLbsNetSessionId& aSessionId);
	
	// CLbsNetworkProtocolBase2 methods:
	void RequestTransmitLocation(const TLbsNetSessionId& aSessionId,
									const TDesC& aDestination,
									TInt aPriority,
									const TLbsNetPosRequestOptionsBase& aOptions); // new options param added

	void RespondPrivacyRequest(const TLbsNetSessionId& aSessionId,
									const TLbsPrivacyResponse& aResponse,
									TInt aReason);								// new reason param added

	void RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask,
									const TLbsNetSessionIdArray& aSessionIdArray);	// new sessionid array param added

	// CLbsNetworkProtocolBase methods:		
	void RespondLocationRequest(const TLbsNetSessionId& aSessionId,
								TInt  aReason,
								const TPositionInfoBase& aPosInfo);
									
	void CancelTransmitLocation(const TLbsNetSessionId& aSessionId,
								TInt  aReason);
	
	void RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelNetworkLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);

	void RequestSelfLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);

	void AdviceSystemStatus(TLbsSystemStatus aStatus);

private:

	CSuplProxyProtocol(MLbsNetworkProtocolObserver2& aObserver);
	
	void ConstructL();
	
	void ProcessRequest(CSuplProxyPrivacyRequestInfo* aRequest);

	void StatusUpdate(  const MLbsNetworkProtocolObserver2::TLbsNetProtocolService& aService, 
	                    const TBool& aIsOperationStarting);
	TInt CompleteMatchingRequest(CLbsNetSession::TRequesttype aRequestType, TInt aErrorCode);
	TInt CompleteMatchingRequest(TInt aRequestId, CLbsNetSession::TRequesttype aRequestType, TInt aErrorCode);
	void CompleteRequest(TInt index,TInt aErrorCode);
	TInt CompleteMatchingSession(const TLbsNetSessionId& aSessionId, TInt aReason);
	TInt CompleteMatchingSession(TInt aRequestId, TInt aReason);
	TBool HasRecentMatchingVerificationResponse(TLbsExternalRequestInfo2& aExtRequestInfo);
private:

	/** Stores the requests */
	CSuplProxyPrivacyRequestBuffer* iRequestBuffer;

	/** Protocol manager object pointer.
	This object is created and owned by this class and represents the
	internal manager for the protocol module. The object is used to
	make requests into the module in support of LBS interactions.
	*/
    CSuplProxyServer* iServer;

    /**
     * SUPL Terminal Initiation Module pointer - used
     * to forward requests to the SUPL FW in the middleware.
     */
    CLbsSuplTiApi* iSuplTiApiWrapper;
	
	/** Reference to this protocol module's observer.
	This is used to make callbacks into LBS from the protocol module.
	*/
    MLbsNetworkProtocolObserver2& iObserver;

    /** Service status
     This holds the latest service status information known by the module.
     */
    MLbsNetworkProtocolObserver2::TLbsNetProtocolServiceMask iActiveServiceMask;

    /** ID of the next session */
    TLbsNetSessionId iNextSessionId;
	
	/** Maximum number of requests that can be active at once */
	TInt iMaxSimultaneousActiveRequests;
	
	// (positioning) data related to LBS subsystem sessions / SUPL subsessions. We can have up to 5 of these, one for each potential SUPL session (MOLR/MTLR))
	RPointerArray<CLbsNetSession>	iLbsNetSessions;
	
	};


#endif // __SUPLPROXYMODULE_H__
