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
// Protocol Module gateway observer class definition
// 
//

#ifndef __CGATEWAYOBSERVER_H__
#define __CGATEWAYOBSERVER_H__

#include <e32cmn.h>
#include <ecom/ecom.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "ctesttimer.h"


/** Gateway observer class definition.

This class is used to capture calls made from the Protocol Module
to the Network Gateway. The callback methods are all defined by the 
MLbsNetworkProtocolObserver interface.
*/
class CGatewayObserver : public CBase , public MLbsNetworkProtocolObserver,
	public MTestTimerObserver
	{
public:

	/** Callback method result enumeration.
	This is used to show which observer method was called.
	*/
	enum TCallBackResult
		{
		ENone,
		EProcessLocationRequest,
		EProcessAssistanceData,
		EProcessLocationUpdate,
		EProcessPrivacyRequest,
		EProcessSessionComplete,
		ETimeout
		};

public:

	/** Static constructor
	*/
	static CGatewayObserver* NewL();

	/** Destructor
	*/
	~CGatewayObserver();
	
	/** Reset the observer's called status
	*/
	void ResetWasObserverCalled();
	
	/** Wait for an observer callback
	*/
	void WaitForCallBack();
	
	/** Check if the observer was called
	*/
	TBool WasObserverCalled() const;
	
	/** Retrieve callback result
	*/
	TInt CallBackResult() const;
	
	/** Retrieve assistance data error
	*/
	TInt AssistanceDataError() const;
	
	/** Session ID
	*/
	const TLbsNetSessionId& SessionIdValue() const;
	
	/** Retrieve assistance data set mask
	*/
	TLbsAsistanceDataGroup AssistanceDataSetMask() const;
	
	/** Retrieve provided data group mask
	*/
	TLbsAsistanceDataGroup ProvidedDataSetMask() const;
	
	/** Session close reason
	*/
	TInt SessionCloseReason() const;
	
	/** Is location request emergency related
	*/
	TBool IsLocEmergency() const;

	/** Location requestor type
	*/
	const MLbsNetworkProtocolObserver::TLbsNetProtocolService& LocType() const;

	/** Location request quality
	*/
	const TLbsNetPosRequestQuality& LocQuality() const;

	/** Reference location position
	*/
	const TPosition& ReferencePos() const;

	/** Privacy notification type
	*/
	const TLbsNetPosRequestPrivacy& PrivacyNotifType() const;

	/** Retrieve privacy requestor info ID
	*/
	void RequesterId(TDes8& aId) const;
	
	/** Is privacy request emergency related
	*/
	TBool IsPrivEmergency() const;

	/** Check if more observer activity takes place
	*/
	TBool IsMoreObserverActivity();


	// MLbsNetworkProtocolObserver methods
	
	/** Get LBS capabilities
	*/
	void GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const;

	/** Advise LBS of status update
	*/
	void ProcessStatusUpdate(TLbsNetProtocolServiceMask aActiveServiceMask);

	/** Ask the gateway to process a location request arriving from the network.
	
	@param aSessionId The Id of the location session within the LBS sub-system.
	@param aEmergency ETrue if this is an emergency request, EFalse otherwise.
	@param aType The type of location request. The type is determined by preceding
				 requests and other context information that only the network
				 protocol module knows about. So it is the responsibility
				 of the network protocol module to set the type of each request.
	@param aQuality The network-requested quality of service (QoS) of the 
					location calculation.
	@param aMethod Location request means and method to be employed.
	*/
	void ProcessLocationRequest(const TLbsNetSessionId&  aSessionId, TBool aEmergency,
			TLbsNetProtocolService aType,
			const TLbsNetPosRequestQuality& aQuality,
			const TLbsNetPosRequestMethod& aMethod);
	
	/** Tell the gateway that the network has finished a location session.
	
	@param aSessionId The Id of the session.
	@param aReason Reason for the completion. KErrNone if the  request is successfully 
				   completed, or one of a range of error codes otherwise.
	*/
	void ProcessSessionComplete(const TLbsNetSessionId&  aSessionId, TInt aReason);
	
	/** Inform the gateway of the arrival of new assistance data.
	
	@param aDataMask The data group mask relevant for the delivered data.
	@param aData A container that holds all of the GPS assistance data to be delivered.
	@param aReason The nature of any error related to this assistance data.
	*/
	void ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
					const RLbsAssistanceDataBuilderSet& aData, TInt aReason);
	
	/** Inform the gateway of the arrival of the network-based location.
	
	@param aSessionId The Id of the location session.
	@param aReferenceLocation The network-based location, including any
							  relevant accuracy data.
	*/
	void ProcessLocationUpdate(const TLbsNetSessionId&  aSessionId, 
					const TPositionInfoBase& aReferenceLocation);

	/** Tell the gateway to process a privacy request from the network.
	
	@param aSessionId The Id of the location session within the LBS sub-system.
	@param aEmergency ETrue if this is an emergency privacy request, EFalse
					  otherwise.
	@param aNotification Type of user notification specified by the network.
	@param aRequestInfo Data about the external requestor of this handsets location.
	*/
	void ProcessPrivacyRequest(const TLbsNetSessionId&  aSessionId, TBool aEmergency, 
								const TLbsNetPosRequestPrivacy& aPrivacy,
								const TLbsExternalRequestInfo& aRequestInfo);


	// MTestTimerObserver methods

	/** Timer expiry event
	This is used to identify a timer expiry and is called by the timer.
	@param aEventId The identity of the timer that expired
	*/
	void OnTimerEvent(TInt aEventId);

private:

	/** Constructor
	*/
	CGatewayObserver();

	/** Second phase Constructor
	*/
	void ConstructL();
	
	/** Signal that observer has received a callback
	@param aResult Enumeration showing which observer method was called.
	*/
	void SignalObserverCallBack(const TCallBackResult aResult);

private:

	/** Flag to show if observer has received a callback
	*/
	TBool iWasObserverCalled;

	/** Value relating to the observer method that was called.
	*/
	TCallBackResult iCallBackResult;
	
	/** Local (nested) active scheduler used to wait for callbacks
	*/
	CActiveSchedulerWait* iScheduler;
	
	/** Assistance data error
	*/
	TInt iAssistanceDataError;
	
	/** Assistance data set mask
	This is the mask reported by the module
	*/
	TLbsAsistanceDataGroup iAssistanceDataSetMask;
	
	/** Provided data set mask
	This mask is derived from the actual provided data content
	*/
	TLbsAsistanceDataGroup iProvidedDataSetMask;
	
	/** Session ID
	*/
	TLbsNetSessionId iSessionIdValue;
	
	/** Session close reason
	*/
	TInt iSessionCloseReason;

	/** Flag to show if location request is emergency related
	*/
	TBool iLocEmergency;

	/** Location requestor type
	*/
	TLbsNetProtocolService iLocReqType;

	/** Location requestor quality
	*/
	TLbsNetPosRequestQuality iLocQuality;

	/** Reference Location position
	*/
	TPosition iReferencePos;

	/** Privacy Notification Type
	*/
	TLbsNetPosRequestPrivacy iPrivacyNotifType;

	/** Privacy Requestor Information
	*/
	TLbsExternalRequestInfo iRequestInfo;

	/** Flag to show if privacy request is emergency related
	*/
	TBool iPrivEmergency;
	
	/** Test timer
	*/
	CTestTimer* iTimer;

	};


#endif // __CGATEWAYOBSERVER_H__

