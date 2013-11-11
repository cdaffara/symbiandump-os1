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
// Protocol Module gateway observer class definition
// 
//

#ifndef __CSUPLGATEWAYOBSERVER_H__
#define __CSUPLGATEWAYOBSERVER_H__

#include <e32cmn.h>
#include <ecom/ecom.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "lbstimer.h"


/** Gateway observer class definition.

This class is used to capture calls made from the Protocol Module
to the Network Gateway. The callback methods are all defined by the 
MLbsNetworkProtocolObserver interface.
*/
class CSuplGatewayObserver : public CBase , public MLbsNetworkProtocolObserver2,
	public MLbsCallbackTimerObserver
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
		ETimeout,
		ETimerError
		};

public:

	/** Static constructor
	*/
	static CSuplGatewayObserver* NewL();

	/** Destructor
	*/
	~CSuplGatewayObserver();
	
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

	/** Retrieve assistance data error mask
	*/
	TLbsAsistanceDataGroup AssistanceDataErrorMask() const;
	
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

	/** Has Reference Location been
		reported?
	*/
	TBool IsRefLocNotified();

	/** Privacy notification type
	*/
	const TLbsNetPosRequestPrivacy& PrivacyNotifType() const;

	/** Retrieve privacy requestor info ID
	*/
	void RequesterId(TDes8& aId) const;
	
	/** Is privacy request emergency related
	*/
	TBool IsPrivReqReceived();

	/** Check if more observer activity takes place
	*/
	TBool IsMoreObserverActivity();
	
	/** Set the default set of positioning capabilities, as returned by
		GetCurrentCapabilities()
	*/
	void SetDefaultCapabilities();

	/** Modify the set of positioning capabilities, as returned by
		GetCurrentCapabilities()
	*/
	void SetCurrentCapabilities(const TLbsNetPosCapabilities& aCapabilities);

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
					const RLbsAssistanceDataBuilderSet& aData, TInt aReason, const TLbsNetSessionIdArray& aSessionIdArray);
	
	/** Inform the gateway of the arrival of the network-based location.
	
	@param aSessionId The Id of the location session.
	@param aLocation The network-based location, including any
							  relevant accuracy data.
	*/
	void ProcessLocationUpdate(const TLbsNetSessionId&  aSessionId, 
					const TPositionInfoBase& aLocation);

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
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);

private:

	/** Constructor
	*/
	CSuplGatewayObserver();

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
	This is the mask reported by the module when
	the error is KErrNone (available assistance data)
	*/
	TLbsAsistanceDataGroup iAssistanceDataSetMask;
	
	/** Provided data set mask
	This mask is derived from the actual provided data content
	*/
	TLbsAsistanceDataGroup iProvidedDataSetMask;
	
	/** Assistance Data Error Mask
	This is the mask reported by the module to signal
	that some requested assistance data types couldn't be
	obtained from the network.
	*/
	TLbsAsistanceDataGroup iAssistanceDataErrorMask;
	
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

	/** True when reference position
		has been notified from SUPL
	*/
	TBool iRefLocNotified;

	/** Privacy Notification Type
	*/
	TLbsNetPosRequestPrivacy iPrivacyNotifType;
	
	/** Location Request Method
	*/
	TLbsNetPosRequestMethod iMethod;

	/** Privacy Requestor Information
	*/
	TLbsExternalRequestInfo iRequestInfo;

	/** Flag to show if privacy request has been received
	*/
	TBool iPrivReqReceived;
	
	/** Positioning Capabilities
	*/
	TLbsNetPosCapabilities iCapabilities;
	
	/** Test timer
	*/
	CLbsCallbackTimer* iTimer;

	};


#endif // __CSUPLGATEWAYOBSERVER_H__

