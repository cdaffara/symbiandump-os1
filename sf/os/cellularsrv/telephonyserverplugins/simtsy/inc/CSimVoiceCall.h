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
// This file contains the class definitions for the Simulator Voice Call classes.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMVOICECALL_H__
#define __CSIMVOICECALL_H__

#include "CSimCall.h"
#include "csimtimer.h"


/**
* CSimVoiceCall.h Handles voice specific Call functionality
*
* Created and owned by the CSimLine class.
*/
class CSimDtmf;

class CSimVoiceCall : public CSimCall, MTimerCallBack
	{
public:
	static CSimVoiceCall* NewL(CSimLine* aLine,const TDesC& aName, CSimPhone* aPhone);
	CSimVoiceCall(CSimLine* aLine,const TDesC& aName, CSimPhone* aPhone);
	~CSimVoiceCall();

// MCallBaseTSY pure virtuals

	virtual TInt Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* aTelNumber);
	virtual TInt DialCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt ConnectCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	virtual TInt AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt HangUp(const TTsyReqHandle aTsyReqHandle);
	virtual TInt HangUpCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt RelinquishOwnership();

    virtual TInt GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* aService);
	virtual TInt GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aParams);

	virtual TInt LoanDataPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aCommPort);
	virtual TInt LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RecoverDataPort(const TTsyReqHandle aTsyReqHandle);

	virtual void TimerCallBack(TInt aId);
	virtual TInt ActionEvent(TCallEvent aEvent,TInt aOtherArgument);
	TInt HandleSwap(TCallEvent aEvent);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);

	void SetDtmfSession(CSimDtmf* aSimDtmf);
	CSimDtmf* GetDtmfSession();
	
	virtual TUint Caps();

private:
	void ConstructL();
	void StartTimer(TInt aDuration);
	TInt ProcessAnswerIncomingCallL();
	TInt ProcessRemoteHangupL();
	TInt DialEmergencyCall(const TTsyReqHandle aTsyReqHandle,TDesC* aTelNumber);
	TInt DialEmergencyCallCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Hold(const TTsyReqHandle);
	TInt HoldCancel(const TTsyReqHandle);
	TInt Resume(const TTsyReqHandle);
	TInt ResumeCancel(const TTsyReqHandle);
	TInt Swap(const TTsyReqHandle);
	TInt SwapCancel(const TTsyReqHandle);
	void UpdateRemotePartyInfoDirection(RMobileCall::TMobileCallStatus aPreviousStatus);
	TInt DialISV(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* aTelNumber);
	TInt DialISVCancel(const TTsyReqHandle aTsyReqHandle);
	TInt AnswerIncomingCallISVL(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams);
	TInt AnswerIncomingCallISVCancel(const TTsyReqHandle aTsyReqHandle);
	

	// CDMA Mode
	TInt NotifyConnectConfirmationCancel(const TTsyReqHandle aReqHandle);
	TInt ResumeConnectCancel(const TTsyReqHandle);
	TInt ResumeConnect(const TTsyReqHandle aReqHandle, const TBool * aConnectContinue );

private:
	CSimTimer* iTimer;
	TInt iDiallingPause;		//< duration spent in dialling state, before entering connecting state.
	TInt iConnectingPause;		//< duration spent in connecting state, before entering connected state.
	TInt iDisconnectingPause;   //< duration spent in disconnecting state, before entering disconnected state.
	TInt iAnswerIncomingPause;  //< duration spent in Answering state, before entering connected state.
	TInt iRemoteHangupPause; 	//< duration spent in connected state, before entering disconnecting state.
	CSimDtmf* iSimDtmf;			//< DTMF session pointer.
	
	// CDMA Mode
	TBool iWaitForConnectConfirm; //< flag to indicate a meta-state between IDLE and DIALING status
	TTsyReqHandle iNotifyConnectConfirmReqHandle; //< Notify Connect Confirm request handle
	TTsyReqHandle iHoldResumeRequestHandle;  //< The TSY request handle associated with a hold or resume.

	};

#endif // CSIMVOICECALL_H__
