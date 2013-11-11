/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* TelephonyActNetwork Active Object class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include <etel.h>
#include <etelmm.h>
#include "Etel3rdParty.h"

const TInt KOneSecond = 1000000;

class CTelephony;
class CTelephonyFunctions;	// forward reference
class CAsyncRequestBaseAct;
class MEventObserver;

/**
*/
class CDialNewCallAct : public CAsyncRequestBaseAct
	{
	
public:		
	static CDialNewCallAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CDialNewCallAct();
	void DialNewCall(TDes8& aId,const CTelephony::TTelNumber& aTelNumber, CTelephony::TCallId& aCallId,
					 CTelephony::TCallId& aTempCallId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CDialNewCallAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TCallId* iCallId;
	CTelephony::TCallId iTempCallId;
	CTelephony::TCallParamsV1* iISVcallParams; 
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 iMMcallParams;
	RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg iMMCallParamsPckg;
	};


class CGetCallDynamicCapsAct : public CAsyncRequestBaseAct
/**
*/
	{
	
public:		
	static CGetCallDynamicCapsAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetCallDynamicCapsAct();
	void GetCallDynamicCaps(TDes8* aId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CGetCallDynamicCapsAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	};

	
class CHoldAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CHoldAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CHoldAct();
	void Hold(const CTelephony::TCallId& aCallId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CHoldAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TCallId iCallId;
	};

class CResumeAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CResumeAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CResumeAct();
	void Resume(const CTelephony::TCallId& aCallId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CResumeAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TCallId iCallId;
	};

class CSwapAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CSwapAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CSwapAct();
	void Swap(const CTelephony::TCallId& aCallId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CSwapAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TCallId iCallId;
	};

class CHangupAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CHangupAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CHangupAct();
	void Hangup(const CTelephony::TCallId& aCallId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CHangupAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TCallId iCallId;
	};

class CAnswerIncomingCallAct : public CAsyncRequestBaseAct, public MEventObserver 
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CAnswerIncomingCallAct* NewL(CTelephonyFunctions* aTelephonyFunctions, MIncomingCallNameSubject& aIncomingCallNameSubject, RArray<RMobileCall>& aCallPool, RArray<CTelephonyFunctions::TCallPoolOperation>& aCallPoolStatus);
	~CAnswerIncomingCallAct();
	void AnswerIncomingCall(CTelephony::TCallId& aCallId, CTelephony::TCallId& aTempCallId);

	// Implementation of pure virtual function in MEventObserver.
	void EventCompleted();
	
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CAnswerIncomingCallAct(CTelephonyFunctions* aTelephonyFunctions, MIncomingCallNameSubject& aIncomingCallNameSubject, RArray<RMobileCall>& aCallPool, RArray<CTelephonyFunctions::TCallPoolOperation>& aCallPoolStatus);
	void ConstructL();
	
	CTelephony::TCallId* iCallId;
	CTelephony::TCallId iTempCallId;
	CTelephonyFunctions* iTelephonyFunctions;
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 iMMcallParams;
	RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg iMMCallParamsPckg;
	
	/**
	The state a CAnswerIncomingCallAct object is in.
	*/
	enum TAnswerIncomingCallState 
		{
		/** Idle. Default state when a CAnswerIncomingCallAct object is created.  */
		EAnswerIncomingCallStateIdle,
		/** Answering an incoming call. Enters this state when the line status is ringing and there is a valid call name stored in CNotifyIncomingCallAct object. */
		EAnswerIncomingCallStateAnswering,
		/** Waiting for CNotifyIncomingCallAct::NotifyIncomingCall() to complete. */
		EAnswerIncomingCallStateWaiting
		};

	/**
	State this active object is currently in.
	*/
	TAnswerIncomingCallState iState;
	/**
	Timer object used to wait for NotifyIncomingCall to complete.
	*/
	RTimer iTimer;
	/**
	Reference to active object owned by iTelephonyFunctions
	which implements MIncomingCallNameSubject interface. 
	Allows this AO to retrieve the call name of an incoming call.
	*/
	MIncomingCallNameSubject& iIncomingCallNameSubject;
	/**
	Reference to the call pool array owned by iTelephonyFunctions.
	*/
	RArray<RMobileCall>& iCallPool;
	/**
	Reference to the call pool status array owned by iTelephonyFunctions.
	*/
	RArray<CTelephonyFunctions::TCallPoolOperation>&	iCallPoolStatus;

	};
	
class CNotifyCallStatusAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CNotifyCallStatusAct* NewL(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation);
	~CNotifyCallStatusAct();
	void NotifyCallStatus(TDes8& aId, const CTelephony::TCallId& aCallId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CNotifyCallStatusAct(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TCallStatusV1* iISVCallStatus; 
	RMobileCall:: TMobileCallStatus iMMCallStatus;
	CTelephony::TCallId iCallId;
	CTelephonyFunctions::TOperations iPendingOperation;
	};
	
class CNotifyRemotePartyInfoAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CNotifyRemotePartyInfoAct* NewL(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation);
	~CNotifyRemotePartyInfoAct();
	void NotifyRemotePartyInfo(TDes8& aId, const CTelephony::TCallId& aCallId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CNotifyRemotePartyInfoAct(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TRemotePartyInfoV1* iISVRemotePartyInfo; 
	RMobileCall::TMobileCallRemotePartyInfoV1 iMMRemotePartyInfo;
	RMobileCall::TMobileCallRemotePartyInfoV1Pckg iMMRemotePartyInfoPckg;
	CTelephony::TCallId iCallId;
	CTelephonyFunctions::TOperations iPendingOperation;
	};

/**
This is an active object responsible for interacting with Multimode 
ETel.

This active object is owned by the CTelephonyFunctions object and 
continuously monitors for the the NotifyIncomingCall event. 
When an incoming call arrives, this AO stores the call
name and sends a notification of an incoming call to any observers
that want to be notified of this event.

One of the observers of this AO is CAnswerIncomingCallAct.
If a client requests to answer an incoming call, the request is only
passed on to the tsy (by CAnswerIncomingCallAct) if this AO has sent
the notification of an incoming call to it (and has a valid call name 
available). This ensures that an attempt to answer a call before the 
call object has been created is not made since the call object is 
only guaranteed to exist when an incoming call notification has been
sent.
*/
class CNotifyIncomingCallAct : public CAsyncRequestBaseAct, public MIncomingCallNameSubject
	{	
public:
	static CNotifyIncomingCallAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CNotifyIncomingCallAct();
	void NotifyIncomingCall();
	
	void ResetCallName();
	TInt CallName(TName&) const;
	
 	void RegisterObserver(MEventObserver* aObserver);
	void DeregisterObserver(MEventObserver* aObserver);
	
protected:
	void Complete();
	TInt RunError(/*TInt aLeaveCode*/);
	void DoCancel();
	
private:
	CNotifyIncomingCallAct(CTelephonyFunctions* aTelephonyFunctions);
	/**
	Pointer to the CTelephonyFunctions object which owns this object.
	*/
	CTelephonyFunctions* iTelephonyFunctions;
	/**
	Stores the call name retrieved from the completion of RLine::NotifyIncomingCall(). 
	Empty when there is no incoming call.
	*/
	TName iCallName;
	/**
	Stores pointers to objects which want to be notified when this 
	object completes.
	*/
	RPointerArray<MEventObserver> iIncomingCallCompletionObservers;
	/**
	Possible states that CNotifyIncomingCallAct can be in.
	*/
	enum TNotifyIncomingCallActState
		{
		ENotifyIncomingCallActStateOperating,
		ENotifyIncomingCallActStateCleanUp
		};
	/**
	The current state of this active object.
	*/
	TNotifyIncomingCallActState iState;
	};
	
