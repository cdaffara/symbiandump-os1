// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CSIMLINE_H__
#define __CSIMLINE_H__


#include <et_phone.h>
#include "utils.h"
#include <etelmm.h>
#include "CSimPubSub.h"
#include "csimtimer.h"

/**
 * @file
 * Declares the base class for voice and data lines - CSimLine
 * @internalAll
 */

const RMobileCall::TMobileCallStatus KStateTableTerminator=(RMobileCall::TMobileCallStatus)999;

struct TStateChangeValidity
	{
	RMobileCall::TMobileCallStatus iOldState;
	RMobileCall::TMobileCallStatus iNewState;
	TInt iError;
	};

/**
 * This table is used to test the validity of potential line state changes.
 * If the state change is not included in the table, the error KErrGeneral will be generated.
 * A potential error value is included as the third item in each entry to enable errors
 * other than KErrGeneral to be generated for specific erroneous state changes.
 */
const struct TStateChangeValidity KLineStateChangeValidity[]= {
	{ RMobileCall::EStatusIdle,			RMobileCall::EStatusDialling,	KErrNone },
	{ RMobileCall::EStatusDialling,		RMobileCall::EStatusConnecting,	KErrNone },
	{ RMobileCall::EStatusConnecting,	RMobileCall::EStatusConnected,	KErrNone },

	{ RMobileCall::EStatusIdle,			RMobileCall::EStatusAnswering,	KErrNone },
	{ RMobileCall::EStatusIdle,			RMobileCall::EStatusRinging,	KErrNone },
	{ RMobileCall::EStatusRinging,		RMobileCall::EStatusAnswering,	KErrNone },
	{ RMobileCall::EStatusRinging,		RMobileCall::EStatusRinging,	KErrNone },
	{ RMobileCall::EStatusRinging,		RMobileCall::EStatusHold,	KErrNone },
	{ RMobileCall::EStatusAnswering,	RMobileCall::EStatusConnected,	KErrNone },

	{ RMobileCall::EStatusConnected,	RMobileCall::EStatusHold,		KErrNone },
	{ RMobileCall::EStatusHold,			RMobileCall::EStatusConnected,	KErrNone },
	{ RMobileCall::EStatusHold,			RMobileCall::EStatusDialling,	KErrNone },
	{ RMobileCall::EStatusHold,			RMobileCall::EStatusAnswering,	KErrNone },
	{ RMobileCall::EStatusHold,			RMobileCall::EStatusDisconnecting,	KErrNone },

	{ RMobileCall::EStatusConnected,	RMobileCall::EStatusDisconnecting,	KErrNone },
	{ RMobileCall::EStatusConnected,	RMobileCall::EStatusRinging,	KErrNone }, // new AA
	{ RMobileCall::EStatusHold,	RMobileCall::EStatusRinging,	KErrNone }, 
	{ RMobileCall::EStatusDisconnecting,	RMobileCall::EStatusIdle,		KErrNone },
	
	{ RMobileCall::EStatusDisconnecting,	RMobileCall::EStatusConnected,		KErrNone },//HangUp Cancel

	{ KStateTableTerminator,			KStateTableTerminator,			KStateTableTerminator}
	};

//
// CSimLine - General Line Functionality
//
/**
* This class 'CSimLine' is the Line class for the Simulator TSY.
* It is the base class for Voice (CSimVoiceLine) and Data (CSimDataLine) lines. 
* It is an abstract class and is therefore not going to be instantiated. 
* It will handle all functionality common to voice and data lines.
*/
class CSimPhone;
class CSimCall;
class CTestConfigSection;

class CSimLine : public CLineBase, public MPSSimObserver, public MTimerCallBack
	{
	friend class CSimVoiceCall;
public:
	CSimLine(CSimPhone* aPhone);
	~CSimLine();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	static void CloseLine(TAny* aObj);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
	// MTelObjectTSY pure virtuals
	virtual void Init();
	
	// Functionality associated with the ETel Core API
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle,RLine::TLineInfo* aLineInfo);

	virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps);
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RLine::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName);
	virtual TInt NotifyIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus);
	virtual TInt NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus);
	virtual TInt GetHookStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::THookStatus* aHookStatus);
	virtual TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aLineStatus);
	virtual TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	// Functionality associated with the ETel Multimode API
	virtual TInt NotifyMobileLineStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aLineStatus);
	virtual TInt NotifyMobileLineStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt NotifyCallAdded(const TTsyReqHandle aTsyReqHandle,TName* aName);
	virtual TInt NotifyCallAddedCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetMobileLineStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

	const CTestConfigSection* CfgFile();					// < Accessor function for the configuration file.
	TInt ChangeStateL(RMobileCall::TMobileCallStatus aNewState,TBool aSwap,CSimCall* aOriginatingCall);	// < Attempt to change the line state.
	virtual void SimPSEvent(const CSimPubSub::TPubSubProperty aProperty, TInt aStatus);		// < MPSSimObserver  pure virtual
	TInt SetAutoAnswerCallObject(CSimCall* aCallObject);			// < Set the call object that will be used to answer the next incoming call.
	void ResetAutoAnswerCallObject(CSimCall* aCallObject);			// < Reset the call object that would have been used to answer the next incoming call.
	TInt SetRemoteHangupCallObject(CSimCall* aCallObject);			// < Set the call object that will be remotely hung up.
	void ResetRemoteHangupCallObject(CSimCall* aCallObject);		// < Reset the call object that would have been remotely hung up.
	void CallDestructor(CSimCall* aCall);
	TBool IsAnswerCallObjectSpare();
	virtual TUint Caps()=0;
	enum TCallType
		{
		ECallTypeSpareCall,
		ECallTypeNormalCall
		};
	virtual CSimCall* CreateNewCallL(TDes& aNewName,TCallType aCallType)=0;
	virtual TInt FindActiveCall(CSimCall*& aCall)=0;


	// Implement the MTimerCallBack interface
	void TimerCallBack(TInt /*aId*/);

protected:
	void ConstructL(const TName& aName);
	void HandleNewCallAddedNotification(const TDesC& aNewName);

private:
	void ProcessIncomingCallEvent();
	void ProcessNotifyIncomingCallEvent();
	void ProcessRemoteHangupEvent();
	RCall::TStatus GetCoreLineStatus();	//< Converts Multimode Line status to Core line status

public:
	CSimPhone* iPhone;							//< Pointer to the phone object
	TName iLineName;							//< Current Line name
	RMobileCall::TMobileCallStatus iState;		//< Holds the current line status
	void UpdatePhoneNotifiers(CSimCall*,TUint);

protected:
	CArrayFixFlat<CSimCall*>* iCalls;			//< Array of all the call objects created
	TInt iCallCnt;
	TInt iNameIndex;							//< Call index in the pool of calls created
	TName iNameOfLastCallAdded;					//< Name of last call added
	CSimCall* iAnswerNextIncomingCall;			//< Pointer for handling AnswerIncomingCall request
	CSimCall* iSpareCall;						//< Pointer to the spare call, used for incoming calls if none is allocated.
	CSimCall* iRemoteHangupCall;				//< Pointer for handling RemoteHangupCall request

	CSimPubSub::TPubSubProperty iICProperty;
	CSimPubSub::TPubSubProperty iRHProperty;
	
	TBool iAnswerIncomingCallReqPending;		//< There is an answer incoming call request pending on a call on this line.
	TBool iRemoteHangupCallReqPending;			//< There is a remote hangup call request pending on a call on this line.

	TNotifyData		iNotifyMobileStatusChange;	//< Holds the notification data (a flag, TsyReqHandle and any raw data) for the mobile line status
	TNotifyData		iNotifyStatusChange;		//< Holds the notification data (a flag, TsyReqHandle and any raw data) for the line status
	RCall::THookStatus	iHookState;				//< The current line hook state.
	TNotifyData		iNotifyHookChange;			//< Holds the notification data (a flag, TsyReqHandle and any raw data) for the line hook status
	TNotifyData		iNotifyCallAdded;			//< Holds the notification data (a flag, TsyReqHandle and any raw data) for the newly added call
	TNotifyData		iNotifyIncomingCall;		//< Holds the notification data (a flag, TsyReqHandle and any raw data) for IncomingCall Notification
	TNotifyData		iNotifyCapsChange;			//< Holds the notification data (a flag, TsyReqHandle and any raw data) for Line Capability Notifications
	TUint			iCaps;						//< Current line capability information
private:
	CSimPubSub*		iSimPubSubIC;				//< Pointer to the Publish & Subscribe object for the incoming call
	CSimPubSub*		iSimPubSubRH;				//< Pointer to the Publish & Subscribe object for the incoming call
	
	/** 
	Duration between receiving an incoming call and sending the 
	incoming call notification.
	*/
	TInt iNotifyIncomingCallPause;
	/**
	Timer to delay sending the notification of the incoming call.
	*/
	CSimTimer* iTimer;
	};

#endif // __CSIMLINE_H__
