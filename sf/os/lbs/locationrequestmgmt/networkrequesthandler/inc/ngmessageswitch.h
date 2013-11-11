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

/**
 @file
 @InternalComponent
 @released
*/

#ifndef NGMESSAGESWITCH_H 
#define NGMESSAGESWITCH_H

#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsextendedsatellite.h>
#include "lbscommoninternaldatatypes.h"
#include "privacyhandlerobserver.h"
#include "privacyhandler.h"
#include "lbsnetinternalapi.h"
#include "lbsnrhngmsgs.h"

 

class CNGMessageSendBufferBase;

/**
 *
 * Class which defines the observer interface for CNGMessageSwitch.
 * Objects which want to be informed of incoming messages should
 * inherit and implement this interface and register themselves by
 * calling CNGMessageSwitch::RegisterObserver();
 *
 */
class MNGMessageSwitchObserver
	{
public:
	/** Called when a privacy request arrives from the network.
	
	Note: a privacy request is often the start of an MT-LR, hence the function name.
	*/
	virtual void OnMTLRRequest(const TLbsNetSessionIdInt& aSessionId,
							   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
							   TBool aIsEmergency,
							   const TLbsExternalRequestInfo& aExternalRequestInfo,
							   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy) = 0;
	
	/** Called when a session complete arrives from the network.
	*/
	virtual void OnSessionComplete(const TLbsNetSessionIdInt& aSessionId, TInt aReason) = 0;

	/** Called when a location request arrives from the network.
	*/
	virtual void OnNetLocRequest(const TLbsNetSessionIdInt& aSessionId, 
								 const TLbsNetPosRequestMethodInt& aNetPosMethod,
								 TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 
								 TBool aIsEmergency,
								 const TLbsNetPosRequestQualityInt& aQuality) = 0;	

	/** Called when a location response is sent to the network.
	
	This is only needed for the X3P handler, so that it knows what quality
	parameters were used when the location request was sent to the A-GPS manager.
	*/
	virtual void OnNetLocResponse(const TLbsNetSessionIdInt& aSessionId,
								 const TLbsNetPosRequestQualityInt& aQuality) = 0;	
	
	/** Called when a reference location arrives from the network.
	*/
	virtual void OnNetLocReferenceUpdate(const TLbsNetSessionIdInt& aSessionId, 
										 const TPositionInfoBase& aPosInfo) = 0;

	/** Called when a final location arrives from the network.
	*/
	virtual void OnNetLocFinalUpdate(const TLbsNetSessionIdInt& aSessionId, 
									 const TPositionInfoBase& aPosInfo) = 0;
	};

/**
 *
 * Class which implements the interface between the Network Gateway and the 
 * Network Request Handler
 */
class CNGMessageSwitch : public CBase,
                         public MLbsNetChannelObserver
    {
public:
    static CNGMessageSwitch* NewL(CLbsAdmin& aAdmin);
    ~CNGMessageSwitch();
    
public:
	/*
	* Called by an observer to register itself with the 
	* message switch.
	*/
	TInt RegisterObserver(MNGMessageSwitchObserver* aPrvLocObserver);

	/**
	* Called by the Privacy and Location Request Handler to send the 
	* result of an MTLR to the Network
	*/
	void SendMTLRResponse(const TLbsNetSessionIdInt& aSessionId, 
						  TLbsNetworkEnumInt::TLbsPrivacyResponseInt aResult,
						  TInt aReason,
						  TBool aEmergency);
	/**
	* Called by the Privacy and Location Request Handler to send the 
	* result of a location measurement request to the Network
	*/
	void SendNetLocResponse(const TLbsNetSessionIdInt& aSessionId, 
							TInt aCompletionCode,
							const TLbsNetPosRequestQualityInt& aRequestQuality,
							const TPositionInfoBase& aPosInfo,
							const TTime& aTimeStamp,
							TBool aEmergency);
	/**
	* Called by the X3P Request Handler to send
	* an X3P request to the Message Gateway Protocol Handler
	*/
	void SendX3pRequest(const TLbsNetSessionIdInt& aSessionId,
						const TDesC& aDestId,
						TUint aTransmitPriority,
						const TLbsNetPosRequestOptionsTechnologyInt& aOptions);
	/**
	* Called by the X3P Request Handler to cancel
	* an X3P request to the Message Gateway Protocol Handler
	*/
	void SendX3pCancel(const TLbsNetSessionIdInt& aSessionId,
					   TInt aReason);
	
	/**
	* Called by the Privacy and Location Request Handler to cancel 
	* an External Location request
	*/
	void SendExternalLocateCancel(const TLbsNetSessionIdInt& aSessionId, 
						TInt aReason);

	TInt GetNetworkReferencePosition(const TLbsNetSessionIdInt& aSessionId,
									 TPositionInfoBase& aPosInfo);

	TInt GetNetworkFinalPosition(const TLbsNetSessionIdInt& aSessionId,
								 TPositionInfoBase& aPosInfo);

public: // From MLbsNetChannelObserver
    void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, 
    							  const TLbsNetInternalMsgBase& aMessage);


protected:
    void ConstructL();

private:
    CNGMessageSwitch(CLbsAdmin& aLbsAdmin);
    
private:

	class TNetPosItem
			{
	public:
		TLbsNetSessionIdInt iSessionId;
			TPositionInfo iPosInfo;
						
		};
	
	class TNetFNPPosItem
		{
	public:
		TLbsNetSessionIdInt iSessionId;
		TPositionExtendedSatelliteInfo iPosInfo;
		
		};

    static TBool IsSessionIdEqual(const TLbsNetSessionIdInt* aSessionId,
    							  const TNetPosItem& aItem);
    static TBool IsFNPSessionIdEqual(const TLbsNetSessionIdInt* aSessionId,
      							  const TNetFNPPosItem& aItem);
    
    void RemoveBufferedNetworkPositions(const TLbsNetSessionIdInt& aSessionId);

private:
	RPointerArray<MNGMessageSwitchObserver> iObservers;
	RLbsNetChannel iNetworkGateway;
	CNGMessageSendBufferBase* iNetworkGatewaySendBuffer;
	CLbsAdmin& iAdmin;

	RArray<TNetPosItem> iRefPosBuffer;
	RPointerArray<TNetFNPPosItem> iFNPPosBuffer;

    };
 
/** Abstract base class for buffer for sending messages to the Network Gateway.
*/
class CNGMessageSendBufferBase : public CActive
	{
public:
	~CNGMessageSendBufferBase();

    virtual void SendNetLocResponseToNetwork(
    	const TLbsNetSessionIdInt& aSessionId, 
		TInt aCompletionCode,
		const TPositionInfoBase& aPosInfo,
		const TTime& aTimeStamp,
		TBool aEmergency) = 0;
		
    virtual void SendMtlrResponseToNetwork(
    	const TLbsNetSessionIdInt& aSessionId, 
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt aResult,
		TInt aReason,
		TBool aEmergency) = 0;
		
	virtual void SendX3pRequestToNetwork(
		const TLbsNetSessionIdInt& aSessionId,
		const TDesC& aDestId,
		TUint aTransmitPriority,
		const TLbsNetPosRequestOptionsTechnologyInt& aOptions) = 0;
		
	virtual void SendX3pCancelToNetwork(
		const TLbsNetSessionIdInt& aSessionId,
		TInt aReason) = 0;
	
	virtual void SendExternalLocateCancelToNetwork(
		const TLbsNetSessionIdInt& aSessionId,
		TInt aReason) = 0;

protected:
	CNGMessageSendBufferBase(RLbsNetChannel& aGatewayChannel, CLbsAdmin& aAdmin);

	void RunL();
	void DoCancel();
	
	void SendMessageToNetwork(const TLbsNetInternalMsgBase& aMessage);
	
	virtual void BufferData(TLbsNetInternalMsgBase* aData) = 0;
	
protected:
	CLbsAdmin& iAdmin;

private:
	RLbsNetChannel& iNetGatewayChannel;
	};



/** The NRH uses this buffer used to send messages to network gateway.

This buffer has a special, non-dynamic  allocation that guarantees that the messages
can be sent. This avoiding Emergencies failing due to 
OOM situations. For non-emergencies messages the
memory is allocated off the heap to buffer messages. 
When this fails due to OOM then the result is that the messages are not sent.
*/
class CNGMessageSendBuffer : public CNGMessageSendBufferBase
	{
public:
	static CNGMessageSendBuffer* NewL(RLbsNetChannel& aGatewayChannel, CLbsAdmin& aAdmin);
	~CNGMessageSendBuffer();

    void SendNetLocResponseToNetwork(const TLbsNetSessionIdInt& aSessionId, 
									 TInt aCompletionCode,
									 const TPositionInfoBase& aPosInfo,
									 const TTime& aTimeStamp,
									 TBool aEmergency);
    void SendMtlrResponseToNetwork(const TLbsNetSessionIdInt& aSessionId, 
								   TLbsNetworkEnumInt::TLbsPrivacyResponseInt aResult,
								   TInt aReason,
								   TBool aEmergency);
	void SendX3pRequestToNetwork(const TLbsNetSessionIdInt& aSessionId,
								 const TDesC& aDestId,
								 TUint aTransmitPriority,
								 const TLbsNetPosRequestOptionsTechnologyInt& aOptions);
	void SendX3pCancelToNetwork(const TLbsNetSessionIdInt& aSessionId,
								TInt aReason);
	void SendExternalLocateCancelToNetwork(const TLbsNetSessionIdInt& aSessionId,
								TInt aReason);
private:
	CNGMessageSendBuffer(RLbsNetChannel& aGatewayChannel, CLbsAdmin& aAdmin);

	void RunL();
	TInt RunError(TInt aError);
	
	void BufferData(TLbsNetInternalMsgBase* aData);
	void BufferEmergencyData(TLbsNetInternalMsgBase* aData); // two spaces are pre-allocated in this array

	
private:
	RPointerArray<TLbsNetInternalMsgBase> iBuffer;
	RPointerArray<TLbsNetInternalMsgBase> iEmergencyBuffer;

	TLbsNetMtLrReponseMsg iEmergencyPrivacyResponseData;
	TLbsNetLocationResponseMsg iEmergencyLocationResponseData;

	};

#endif // NGMESSAGESWITCH_H

