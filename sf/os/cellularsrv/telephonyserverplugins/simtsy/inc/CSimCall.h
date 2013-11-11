// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definitions of the Simulator TSY call classes.  The call classes
// process the call-based requests made by ETel clients and passed down to the TSY by the
// ETel Server.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMCALL_H__
#define __CSIMCALL_H__

#include <e32base.h>
#include <etelmm.h>
#include <et_phone.h>
#include "utils.h"
#include "SimConstants.h"
#include "csimtimer.h"

/**
* This class 'CSimCall' is the Call class for the Simulator TSY.
* This class is the base classs voice and data calls. It is an abstract class so it will 
* not be instantiated. It handles all the call-based ETel requests common to voice and data calls.
* Requests are passed down from the ETel Server through virtual functions.  
* Core ETel API requests appear as individual virtual functions, 
* while extension API requests are passed through the ExtFunc requests.
*/
class CSimLine;
class CSimCallDuration;
class CSimCallRemotePartyInfoChange;
class CSimCall : public CCallBase
	{
friend class CSimCallDuration;
friend class CSimCallRemotePartyInfoChange;

public:
	enum TCallEvent 
		{
		ECallEventDial,
		ECallEventHangUp,
		ECallEventAnswerIncoming,
		ECallEventIncomingCall,
		ECallEventTimeOut,
		ECallEventNtRasConnected,
		ECallEventHold,
		ECallEventResume,
		ECallEventSwap,
		ECallEventConnectContinue,
		ECallEventRemoteHangup
		};

public:
	CSimCall(CSimLine* aLine,const TName& aName, CSimPhone* aPhone);
	~CSimCall();
	void ConstructL();
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	static void CloseCall(TAny* aObj);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);

	// MCallBaseTSY pure virtuals
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle,RCall::TCallInfo* aCallInfo);
	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps* aCaps);
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RCall::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus);
    virtual TInt NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus);
	virtual TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyDurationChange(const TTsyReqHandle aTsyReqHandle,TTimeIntervalSeconds* aTime);
	virtual TInt NotifyDurationChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RecoverDataPortAndRelinquishOwnership();
    virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aCallStatus);
    virtual TInt TransferOwnership(const TTsyReqHandle aTsyReqHandle);
    virtual TInt AcquireOwnership(const TTsyReqHandle aTsyReqHandle);
	virtual TInt AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime);
	virtual TInt GetFaxSettings(const TTsyReqHandle aTsyReqHandle,RCall::TFaxSessionSettings* aSettings);
	virtual TInt SetFaxSettings(const TTsyReqHandle aTsyReqHandle,const RCall::TFaxSessionSettings* aSettings);
	virtual void UpdateNotifiers();
	//MM
	virtual TInt GetMobileCallStatus(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallStatus* aCallStatus);
	TInt GetMobileCallInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aMobileCallInfo);
	virtual TInt NotifyMobileCallStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	TInt NotifyMobileCallStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	TInt GetMobileCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt NotifyMobileCallCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	TInt NotifyMobileCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyRemotePartyInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aRemoteParty);
	virtual TInt NotifyRemotePartyInfoChangeCancel();

	// MTelObjectTSY pure virtuals 
	virtual CTelObject* OpenNewObjectByNameL(const TDesC&);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

	TBool Used();
	void SetUsed();
	void SetUnused();
	virtual TInt ActionEvent(TCallEvent aEvent,TInt aStatus);	//< The state machine for the calls
	TInt ChangeStateL(RMobileCall::TMobileCallStatus aState,TBool aSwap, TBool aNoPropagation);	//< Changes the state of the call
	void ResetIfRingingL();
	virtual TUint Caps()=0;

protected:
	void GenerateCoreCallCaps(TUint& aCaps);

private:
	RCall::TStatus GetCoreCallStatus();			//< Converts Multimode Line status to Core line status
	
public:
	TName iName;
	RMobileCall::TMobileCallStatus iState;		//< Holds the current state of the call
	
	CSimCallDuration* iCallDurationHandler;			// < Pointer to the owned timer class.

protected:
	TNotifyData		iAnswerIncomingCall;		//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
												//< for Answer incoming call notifications
	TTsyReqHandle iDialRequestHandle;			//< Request handle for the Dial request
	TTsyReqHandle iHangUpRequestHandle;			//< Request handle for the Hang up request
	CSimLine* iLine;							//< Pointer to the line class
	TNotifyData		iNotifyStatusChange;		//< Holds the notification data (a flag, TsyReqHandle and any raw data) for core call status change.
	TNotifyData		iMobileNotifyStatusChange;	//< Holds the notification data (a flag, TsyReqHandle and any raw data) for multimode call status change.
												//< for the line status
	RCall::THookStatus	iHookState;				//< The current call hook state.
	TNotifyData		iNotifyHookChange;			//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
												//< for the line hook status
	TNotifyData		iNotifyCapsChange;			//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
												//< for the core caps changes
	TNotifyData		iNotifyMobileCapsChange;	//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
												//< for the mobile caps changes
	TUint iCaps;								//< The capability flags associated with the call.
	CSimCallRemotePartyInfoChange* iNotifyRemotePartyInfoTimer;		//< active object that similates retrieval of remote party info after certain amount of time.

	CSimPhone* iPhone;
	};
	
class CSimCallDuration : public CBase, MTimerCallBack
	{
public:
	static CSimCallDuration* NewL(CSimCall* aCall);
	CSimCallDuration(CSimCall* aCall);
	~CSimCallDuration();
	void StartDuration();
	void StopDuration();
	void StartNotification(TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime);
	void StopNotification();	
	void GetDuration(TTimeIntervalSeconds* aTime);
	void TimerCallBack(TInt aId);
	TTimeIntervalSeconds iCallDuration;

private:
	void ConstructL();

private:
	CSimCall* iCall;	
	CSimTimer* iDurationTimer;			// < Pointer to the owned timer class.
						// <stores the call duration (since connected)
	TNotifyData iNotifyDurationChange;
	};
	
class CSimCallRemotePartyInfoChange : public CBase, MTimerCallBack
	{
public:
	static CSimCallRemotePartyInfoChange* NewL(CSimCall* aCall);
	CSimCallRemotePartyInfoChange(CSimCall* aCall);
	~CSimCallRemotePartyInfoChange();
	void TimerCallBack(TInt aId);
	void Start();
private:
	void ConstructL();
public:

	RMobileCall::TMobileCallRemotePartyInfoV1 iRemotePartyInfoV1;
	TNotifyData iNotifyRemotePartyInfo;
	CSimTimer* iRemoteInfoTimer;
	TInt iDelay;	
	CSimCall* iCall;	
	};

#endif // CSIMCALL_H__
