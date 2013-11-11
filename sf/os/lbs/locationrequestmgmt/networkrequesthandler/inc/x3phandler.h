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
// Definition of network position (reference/final) handler sub-component of the NRH
// 
//

/**
 @file
 @internalComponent
*/

#ifndef _X3PHANDLER_H 
#define _X3PHANDLER_H

#include <lbs/lbsloccommon.h>
#include <lbs/lbsadmin.h>
#include "LbsInternalInterface.h"
#include "ngmessageswitch.h"
#include "x3prequest.h"
#include "agpsinterfacehandler.h"

/* Observer class used to inform of the status of an ongoing X3P
*/
class MX3pHandlerObserver
	{
public:
	/* Notification of the completion of an X3P.
	
	Note that the X3P may have completed because of an error, 
	in which case aPositionInfo will not be valid.
	
	@param aSessionId SessionId of the completed session
	@param aPositionInfo Contains the position sent to the network. 
						 If there was an error during the transmit
						 location service then this value will be
						 invalid
	@param aReason Reason the session was completed. KErrNone if the
				   the session completed normally. KErrCancel if the 
				   session was cancelled (either by the client of the
				   network). KErrTimedOut if the session timed out
				   before it could be completed.
	*/
	virtual void OnTransmitLocationComplete(const TLbsNetSessionIdInt& aSessionId,
											const TPositionInfoBase& aPositionInfo,
											TInt aReason) = 0;
											
	/* Notification of network-provided Reference Location.
	
	Note that aReason may show an error has happened, 
	in which case aPositionInfo will not be valid.
	
	@param aSessionId SessionId of the session where the request originated.
	@param aPositionInfo Contains the reference position provided by the network. 
						 If there was an error during the transmit
						 location service then this value will be
						 invalid.
	@param aReason Reason the location is being notified. KErrNone if the
				   the location has been normally provided by the network.
				   KErrCancel if the session was cancelled (either by the
				   client of the network). KErrTimedOut if the session timed
				   out before it could be completed.
	*/									
	virtual void OnReferenceLocationAvailable(const TLbsNetSessionIdInt& aSessionId,
									  const TPositionInfoBase& aPositionInfo) = 0;
	};
	
/*
 * Defines functions called by the server's subsessions to pass on
 * requests from an X3P client
 */ 
class MX3pHandlerNotifier
	{
public:
	virtual TInt OnTransmitPosition(const TDesC& aDestinationId,
									TUint aTransmitPriority,
									const TLbsTransmitPositionOptions& aTransmitOptions,
									TLbsNetSessionIdInt& aSessionId) = 0;
	virtual void OnCancelTransmitPosition(const TLbsNetSessionIdInt& aSessionId) = 0;
	
    virtual void AddObserverL(MX3pHandlerObserver* aObserver) = 0;
    virtual void RemoveObserver(MX3pHandlerObserver* aObserver) = 0;
	};
	
/* 
*/
class CX3pHandler : public CBase, 
                    public MX3pRequestObserver,
                    public MNGMessageSwitchObserver,
                    public MX3pHandlerNotifier
    {
public:
	static CX3pHandler* NewL(CNGMessageSwitch& aMessageSwitch, MX3pStatusHandler& aX3pStatusHandler, CLbsAdmin& aLbsAdmin);
	~CX3pHandler(); 
    
public: // From class MNGMessageSwitchObserver
	void OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
					   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
					   TBool aIsEmergency,
					   const TLbsExternalRequestInfo& aExternalRequestInfo,
					   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy);
	
	void OnSessionComplete(const TLbsNetSessionIdInt& aSessionId, TInt aReason);

	void OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
						 const TLbsNetPosRequestMethodInt& aNetPosMethod,
						 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
						 TBool aIsEmergency,
						 const TLbsNetPosRequestQualityInt& aQuality);
	void OnNetLocResponse(const TLbsNetSessionIdInt& aSessionId,
								 const TLbsNetPosRequestQualityInt& aQuality);
	
	void OnNetLocReferenceUpdate(const TLbsNetSessionIdInt& aSessionId, 
								 const TPositionInfoBase& aPosInfo);

	void OnNetLocFinalUpdate(const TLbsNetSessionIdInt& aSessionId, 
							 const TPositionInfoBase& aPosInfo);
    
public: // From MX3pRequestObserver
	void OnRequestTimeout(const TLbsNetSessionIdInt& aTimedOutRequestId);

protected:
	CX3pHandler(CNGMessageSwitch& aMessageSwitch, MX3pStatusHandler& aX3pStatusHandler, CLbsAdmin& aLbsAdmin);
    void ConstructL();
    
protected: // From MX3pServerHandler 
	TInt OnTransmitPosition(const TDesC& aDestinationId,
							TUint aTransmitPriority,
							const TLbsTransmitPositionOptions& aTransmitOptions,
							TLbsNetSessionIdInt& aSessionId);
	void OnCancelTransmitPosition(const TLbsNetSessionIdInt& aSessionId);
	
    void AddObserverL(MX3pHandlerObserver* aObserver);
    void RemoveObserver(MX3pHandlerObserver* aObserver);
    
private:
	// Request a new X3P
	void TransmitLocationRequestL(const TDesC& aDestinationID,
								  TUint aTransmitPriority, 
								  const TLbsTransmitPositionOptions& aTransmitOptions,
								  TLbsNetSessionIdInt& aSessionId);
	// Cancel a current X3P session
	void TransmitLocationCancel(const TLbsNetSessionIdInt& aSessionId);

private:
	void HandleTimedOutRequest(const TLbsNetSessionIdInt& aRequestId);
	void HandleCompletedRequest(const TLbsNetSessionIdInt& aRequestId, TInt aReason);
	void ActivateRequest();
	TInt HighestPriorityIndex();
	void RecalculateActiveIndex(const TLbsNetSessionIdInt& aSessionId);
	TInt FindSessionById(const TLbsNetSessionIdInt& aSessionId);
	void HandleReferenceLocation(const TLbsNetSessionIdInt& aRequestId, const TPositionInfo& aRefPosInfo);
	void UpdateX3pStatus();
	void GetAdminTechnologyType(TPositionModuleInfo::TTechnologyType& aMode);
    
private:
	CNGMessageSwitch&			iMessageSwitch;
	TInt						iNextSessionId;
	CLbsAdmin&					iAdmin;
	TInt						iActiveRequestIndex;
	CX3pRequest*				iActiveRequest;
	RPointerArray<CX3pRequest>	iX3pRequests;
	RPointerArray<MX3pHandlerObserver>	iX3pObservers;

	TLbsNetSessionIdInt			iTimedOutRequestId;
	TPositionInfo				iRefPosInfo;
	
	TBool						iReceivedFinalNetPosInfo;
	TPositionInfo				iFinalNetPosInfo;
	
	MX3pStatusHandler& iX3pStatusHandler;
	CLbsAdmin::TLbsBehaviourMode iLbsBehaviourMode;
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities iDeviceGpsModeCaps;	
    };


#endif // _X3PHANDLER_H
