// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Common Fax/Hayes definitions
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __NOTIFY_H__
#define __NOTIFY_H__

#include "ATBASE.H"
/**
@internalComponent
*/
enum TNotifications
	{
	EModemDetection,
	EIncomingCall,
	ELineHookChange,
	ECallHookChange,
	ELineStatusChange,
	ENewCallAdded,
	ECallStatusChange,
	ECallDurationChange,
	ECallCaps,
	EReadOrWriteFax,
	EEndOfFaxPage,	// used for both Rx and Tx modes
	ERegistrationStatus,
	ECurrentNetwork,
	EMobileCallStatusChange,
	ECallMobileCaps,
	EMobileLineStatusChange,
	// GPRS
	EPacketContextAdded,
	EPacketStatusChange,
	EPacketNtwkRegStatusChange,	
	// GPRS Context
	EPacketContextConfigChange,
	EPacketContextStatusChange,
	// GPRS QoS
	EPacketQoSProfileChange
	};
/**
@internalComponent
*/
enum TEvent
	{
	ENoEvent,
	ERingOccurred,
	ERingStopped,
	EPhoneDetected,
	EPhoneNotDetected,
	EBegunConnecting,
	EConnected,
	EBegunHangingUp,
	EBecomeIdle,
	ECallAdded,
	ETimePeriodElapsed,
	EDataPortLoaned,
	EDataPortRecovered,
	EFaxReadOrWriteCompleted,
	EEndOfFaxPageCompleted,
	EFaxProgressChanged,
	EFaxSessionTerminated,
	EAddressAddedToPhoneBookCompleted,
	EAddressRemovedFromPhoneBookCompleted,
	ERegistrationStatusChanged,
	ECurrentNetworkChanged,
	// GPRS
	EPacketContextAddedChanged,
	EPacketStatusChanged,
	// GPRS Context
	EPacketContextConfigChanged,
	EPacketContextStatusChanged,
	// GPRS QoS
	EPacketQoSProfileChanged
	};
/**
@internalComponent
*/
struct TLastEvent
	{
	TEvent iLastEvent;
	CTelObject* iTelObject;
	};

class CFaxSession;
class CNotifyBase;
class CNotifications : public CBase
/**
@internalComponent
*/
	{
public:
	static CNotifications* NewL();
	CNotifications();
	void ConstructL();
	~CNotifications();
	void RegisterNotification(TNotifications aWhichNotification,
							  TTsyReqHandle aTsyReqHandle,
							  CTelObject* aTelObject,
							  TAny* aParams = NULL);
	void CheckNotification(CTelObject* aTelObject,TEvent aEvent);
	void CheckNotification(CCallBase* aCallObject,TEvent aEvent);
	void CheckNotification(CFaxSession* aETelFaxObject,TEvent aEvent,
						   TInt aError,TAny* aParams=NULL);
	void CheckNotification(CTelObject* aTelObject, TNotifications aNotification);
	TBool CheckForEIncomingSmsNotification();
	void RemoveNotification(TTsyReqHandle aTsyReqHandle);
	void CompleteNotificationsWithError(TInt aError);
	void RemoveClientFromLastEvents(CTelObject* aTelObject);
	void RemoveEventFromLastEvents(TEvent aEvent);
protected:
	TInt AddLastEvent(CTelObject* aTelObject,TEvent aLastEvent);
	TInt FindLastEvent(CTelObject* aTelObject);
	TInt GetLastEvent(CTelObject* aTelObject, TLastEvent& aLastEvent);
	void RegisterNotificationL(CNotifyBase* aNotify);
protected:
	CArrayPtrFlat<CNotifyBase>* iNotifications;
	CArrayFixFlat<TLastEvent>* iLastEvents;	// each TelObject has a last event
	TBool iAlreadyChecking;
	};

class CCallMobileFax;
class CATBase;
class CNotifyBase : public CBase
/**
@internalComponent
*/
	{
protected:
	CNotifyBase(TTsyReqHandle aReqHandle,CTelObject* aTelObject);
public:
	~CNotifyBase();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent aLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent);
	virtual TBool CheckAndCompleteNotification(CFaxSession* aETelFaxObject,TEvent aEvent,TInt aError,TAny* aParams=NULL);
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject, TNotifications aNotification);
public:
	TTsyReqHandle TsyReqHandle();
	CTelObject* TelObject();
protected:
	TTsyReqHandle iReqHandle;
	CTelObject* iTelObject;
private:
	CArrayFixFlat<CNotifyBase*>* iNotifications;
	};

class CNotifyModemDetected : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyModemDetected* NewL(RPhone::TModemDetection* aDetection,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyModemDetected(RPhone::TModemDetection* aDetection,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyModemDetected();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent iLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent aLastEvent);
private:
	RPhone::TModemDetection* iDetection;
	};

class CNotifyIncomingCall : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyIncomingCall* NewL(TDes* aName,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyIncomingCall(TDes* aName,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyIncomingCall();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent iLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallBase,TEvent aEvent,TEvent aLastEvent);
private:
	TDes* iName;
	};

class CNotifyLineHookChange : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyLineHookChange* NewL(RCall::THookStatus* aHookStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyLineHookChange(RCall::THookStatus* aHookStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyLineHookChange();
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	RCall::THookStatus* iHookStatus;
	};

class CNotifyCallHookChange : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyCallHookChange* NewL(RCall::THookStatus* aHookStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyCallHookChange(RCall::THookStatus* aHookStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyCallHookChange();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent iLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	RCall::THookStatus* iHookStatus;
	};

class CNotifyLineStatusChange : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyLineStatusChange* NewL(RCall::TStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyLineStatusChange(RCall::TStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyLineStatusChange();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent iLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	RCall::TStatus* iStatus;
	};

class CNotifyMobileLineStatusChange : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyMobileLineStatusChange* NewL(RMobileCall::TMobileCallStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyMobileLineStatusChange(RMobileCall::TMobileCallStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyMobileLineStatusChange();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent iLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	RMobileCall::TMobileCallStatus* iStatus;
	};

class CNotifyLineNewCallAdded : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyLineNewCallAdded* NewL(TDes* aName,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyLineNewCallAdded(TDes* aName,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyLineNewCallAdded();
	virtual TBool CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent iLastEvent);
private:
	TDes* iName;
	};

class CNotifyCallStatusChange : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyCallStatusChange* NewL(RCall::TStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyCallStatusChange(RCall::TStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyCallStatusChange();
//	virtual TBool CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	RCall::TStatus* iStatus;
	};

class CNotifyMobileCallStatusChange : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyMobileCallStatusChange* NewL(RMobileCall::TMobileCallStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyMobileCallStatusChange(RMobileCall::TMobileCallStatus* aStatus,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyMobileCallStatusChange();
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	RMobileCall::TMobileCallStatus* iStatus;
	};

class CNotifyCallDurationChange : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyCallDurationChange* NewL(TTimeIntervalSeconds* aTime,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyCallDurationChange(TTimeIntervalSeconds* aTime,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyCallDurationChange();
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	TTimeIntervalSeconds* iTime;
	};

class CNotifyCallCaps : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyCallCaps* NewL(RCall::TCaps* aCaps,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyCallCaps(RCall::TCaps* aCaps,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyCallCaps();
	virtual TBool CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	RCall::TCaps* iCaps;
	};

class CNotifyMobileCallCaps : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyMobileCallCaps* NewL(TDes8* aCaps,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyMobileCallCaps(TDes8* aCaps,TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyMobileCallCaps();
	virtual TBool CheckAndCompleteNotification(CTelObject* /*aTelObject*/,TEvent aEvent,TEvent aLastEvent);
	virtual TBool CheckAndCompleteNotification(CCallBase* aCallObject,TEvent aEvent,TEvent iLastEvent);
private:
	RMobileCall::TMobileCallCapsV1Pckg* iCapsPckg;
	};

class CNotifyFaxReadOrWrite : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyFaxReadOrWrite* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyFaxReadOrWrite(TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyFaxReadOrWrite();
	virtual TBool CheckAndCompleteNotification(CFaxSession* aETelFaxObject,TEvent aEvent,TInt aError,TAny* aParams);
	};

class CNotifyFaxEndOfPage : public CNotifyBase
/**
@internalComponent
*/
	{
public:
	static CNotifyFaxEndOfPage* NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	CNotifyFaxEndOfPage(TTsyReqHandle aReqHandle,CTelObject* aTelObject);
	~CNotifyFaxEndOfPage();
	virtual TBool CheckAndCompleteNotification(CFaxSession* aETelFaxObject,TEvent aEvent,TInt aError,TAny* aParams);
	};

#endif
