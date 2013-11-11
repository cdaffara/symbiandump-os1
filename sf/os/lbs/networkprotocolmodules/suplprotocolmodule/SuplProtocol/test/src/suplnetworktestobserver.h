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
// SUPL Protocol Module network observer class definition
// 
//

#ifndef __CSUPLNETWORKTESTOBSERVER_H__
#define __CSUPLNETWORKTESTOBSERVER_H__
#include <e32base.h>
#include <e32cmn.h>
#include "lbstimer.h"
#include <lbspositioninfo.h>
#include "suplconnectionmanager.h"
#include "suplpospayload.h"


/** Network observer class definition.

This class is used to capture calls made from the Protocol Module
to the Network.
*/
class CSuplNetworkTestObserver : public CBase, public MSuplConnectionManagerTestObserver, public MLbsCallbackTimerObserver
	{
public:

	/** Callback method result enumeration.
	This is used to show which observer method was called.
	*/
	enum TCallBackResult
		{
		ENone,
		EConnectReq,
		ESuplStartSendReq,
		ESendSuplPosInitSendReq,
		ESendSuplPosSendReq,
		ESendSuplEndSendReq,
		EDisconnectReq,
		ETimeout,
		ETimerError
		};

public:

	static CSuplNetworkTestObserver* NewL();
	~CSuplNetworkTestObserver();
	
	void ResetWasObserverCalled();
	void WaitForCallBack();
	TBool WasObserverCalled() const;
	TInt CallBackResult() const;
	TBool IsMoreObserverActivity();

	// MSuplConnectionManagerObserver methods (Connection Manager callbacks)
	void ConnectRequestReceived(const TSuplLocalSessionId& aSessionId, const CSuplSessionRecord::TServiceType& aServiceType, const TLbsHostSettingsId& aSettingsId);
	void DisconnectRequestReceived(const TSuplLocalSessionId& aSessionId);
	void SendMessageRequestReceived(const CSuplMessageBase* aMessage, const TSuplLocalSessionId& aSessionId);

	// Methods for the test code to access the parameters
	// provided by the callbacks
	TSuplLocalSessionId SessionId() const;
	CSuplMessageBase* ReceivedMessage();
	const TPosition& MobilePos() const;
	TLbsAssistanceDataGroup AssistDataReq();
	CSuplPosPayload::TSuplPosPayloadType RrlpMessageType();
	TLbsHostSettingsId HostSettingsId();
	
	// Methods for the test code to instruct the Connection Manager what to report to
	// the SUPL state machines (MLbsConnectionManagerObserver wrapper methods)
	// 
	void InjectSuplMessage(const TSuplLocalSessionId& sessionId, CSuplMessageBase* aSuplMessage);
	void InjectConnectedIndication(const TSuplLocalSessionId& aSessionId);
	void InjectConnectionErrorIndication(const TSuplLocalSessionId& aSessionId, const MSuplConnectionManagerObserver::TSuplConnectionError& aError);

	// MTestTimerObserver methods
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);

private:

	CSuplNetworkTestObserver();
	void ConstructL();

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

	/** Pointer to the network interface.
	*/
	CSuplConnectionManager* iConnectionManager;

	/** Mobile Location position
	*/
	TPosition iMobilePos;

	/** Session Id notified by the Connection Manager
	*/
	TSuplLocalSessionId iSessionId;
	
	/** SUPL Message notified by the Connection Manager
	*/
	CSuplMessageBase* iReceivedMessage;

	/** Test timer
	*/
	CLbsCallbackTimer* iTimer;
	
	/** Assistance Data Request Mask sent from SPM to
	SLP
	*/
	TLbsAsistanceDataGroup iRxAssistData;
	
	/** Type of RRLP message received in a SUPL POS
	*/
	CSuplPosPayload::TSuplPosPayloadType iRrlpMessageType;
	
	/** HostSettingsId expected to be used for the session
	*/
	TLbsHostSettingsId iSettingsId;
	};


#endif // __CSUPLNETWORKTESTOBSERVER_H__

