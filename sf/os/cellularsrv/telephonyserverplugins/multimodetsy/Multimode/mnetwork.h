// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MM TSY Network Header file.
// Describes the GSM Network & Operator information.
// 
//

/**
 @file
 @internalAll
*/
 
#ifndef __MNETWORK_H__
#define __MNETWORK_H__

#include "mmlist.h"
#include "ATBASE.H"
#include "NOTIFY.H"

/**
 * Determines GSM Network and Operator Information.
 *
 * CATNetworkInfo inherits from the CATCommands class. 
 * This class transmits the Set Network Operator Command (AT+COPS=x,x) in numeric, short 
 * alphanumeric and long alphanumeric formats. It also determines the current operator 
 * using the operator query command (AT+COPS?).
 */
class CATNetworkInfo : public CATCommands
	{
public:

	struct TTsyNetworkInfo
		{
		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* iNetworkInfoPckg;
		RMobilePhone::TMobilePhoneLocationAreaV1* iArea;
		};
	
	static CATNetworkInfo* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATNetworkInfo();

	void CheckOperator();
	void GetCurrentNetworkInfo(CATNetworkInfo::TTsyNetworkInfo* aNetworkInfo);


private:
	CATNetworkInfo(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	virtual void Complete(TInt aError, TEventSource aSource);

	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	
	void StartGetOperator();
	void InitializeNetworkInfo();		// Initialize TNetworkInfo struct
	void ParseOperatorResponseL();		// Parse Operator Response
	
	
	enum {
		EATNotInProgress,
		EATGetCurrentOperatorWriteComplete,
		EATGetCurrentOperatorReadComplete,
		EATNumericOperatorWaitForWriteComplete,		// Numeric Operator Format
		EATSetNumericOperatorReadComplete,
		EATShortOperatorWaitForWriteComplete,		// Short Alphanumeric Op. Format
		EATSetShortOperatorReadComplete,		
		EATLongOperatorWaitForWriteComplete,		// Long Alphanumeric Op. Format
		EATSetLongOperatorReadComplete,
		EATWaitForSettlingTimeout,
		EATWaitForStopState
		} iState;
		
	CPhoneGlobals* iPhoneGlobals;
	TTsyNetworkInfo iNetworkInfo;
	TBool iCancelled;
	};

// Notifications

/**
 * \class CNotifyNetworkRegistrationStatusChange mnetwork.h "mmtsy/multimode/mnetwork.h"
 * \brief This is a Notification class. Handles a Registration status notification.
 *
 * CNotifyNetworkRegistrationStatusChange inherits from the CNotifyBase class defined in
 * notify.h.
 */
class CNotifyNetworkRegistrationStatusChange : public CNotifyBase
	{
public:
	static CNotifyNetworkRegistrationStatusChange* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RMobilePhone::TMobilePhoneRegistrationStatus* aRegStatus);
	~CNotifyNetworkRegistrationStatusChange();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent);
private:
	CNotifyNetworkRegistrationStatusChange(TTsyReqHandle aReqHandle,CTelObject* aTelObject,RMobilePhone::TMobilePhoneRegistrationStatus* aRegStatus);
	RMobilePhone::TMobilePhoneRegistrationStatus* iRegStatus;
	};		

/**
 * \class CCurrentNetworkChangedNotify mnetwork.h "mmtsy/multimode/mnetwork.h"
 * \brief This is a Notification class. Handles a Current network change notification.
 *
 * CCurrentNetworkChangedNotify inherits from the CNotifyBase class defined in
 * notify.h. CNotifyCurrentNetworkChange
 */
class CNotifyCurrentNetworkChange : public CNotifyBase
	{
public:
	static CNotifyCurrentNetworkChange* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TInt* aInfo);
	~CNotifyCurrentNetworkChange();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent);
private:
	CNotifyCurrentNetworkChange(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TInt* aInfo);
	CATNetworkInfo::TTsyNetworkInfo iNetworkInfo;
	};		

/**
 * \class CATDetectNetwork mnetwork.h "mmtsy/multimode/mnetwork.h"
 * \brief Determines the Operators present in the GSM Network.
 *
 * CATDetectNetwork inherits from the CATCommands class. 
 * This class transmits the AT+COPS=? Command in order to determine all the Operators 
 * present in the GSM Network.
 */
class CATDetectNetwork : public CATCommands
	{
public:
	static CATDetectNetwork* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATDetectNetwork();

	void DetectNetworks(TTsyReqHandle aTsyReqHandle,TAny* aParams);
	TInt ListNetworks(TDes8& aBuffer) const;

	struct TTsyDetectedNetworks
		{
		TInt* iBufSize;		// Size of the streamed network list, in bytes.
		CBufBase** iBufPtr;  // Streamed network list
		};

private:
	CATDetectNetwork(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	virtual void Complete(TInt aError, TEventSource aSource);

	void ParseResponseL();
private:
	enum {
		EATNotInProgress,
		EATDetectWaitForWriteComplete,
		EATDetectReadComplete,
		EATDetectExtendedReadComplete,
		EATWaitForStopState
		} iState;

	TTsyDetectedNetworks iDetectedNetworks;
	};

#endif
