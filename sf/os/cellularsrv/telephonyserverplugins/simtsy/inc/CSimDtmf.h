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
// Contains definitions for classes that implement signal strength functionality.
// 
//

/**
 @file
 @internalAll
*/

#ifndef CSimDtmf_H__
#define CSimDtmf_H__

#include <et_phone.h>
#include "csimtimer.h"

class CSimPhone;
class CTestConfigSection;

/**
 * Implements the RMobilePhone based functions that constitute the signal strength
 * functionality provided by the SIM TSY.
 */
class CSimDtmf : public CBase, MTimerCallBack
	{
public:
	static CSimDtmf* NewL(CSimPhone* aPhone);
	CSimDtmf(CSimPhone* aPhone);
	~CSimDtmf();
	TInt GetDtmfCaps(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	TInt NotifyDtmfCapsChange(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	void NotifyDtmfCapsChangeCancel(TTsyReqHandle aReqHandle);
	TInt SendDTMFTones(TTsyReqHandle aReqHandle,TDes* aPckg1);
	TInt StartDTMFTone(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	TInt StopDTMFTone(TTsyReqHandle aReqHandle);
	TInt NotifyStopInDTMFString(TTsyReqHandle aReqHandle);
	void NotifyStopInDTMFStringCancel();
	TInt ContinueDtmfStringSending(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	void SendDTMFTonesCancel();
	void CallClosureCallback();
	const CTestConfigSection* CfgFile();

private:
	void ConstructL();
	enum TEvent
		{
		EEventStartDtmfString,
		EEventStartDtmfTone,
		EEventTimer,
		EEventStopDtmfTone,
		EEventContinueDtmf,
		EEventTerminateDtmf,
		EEventCallClosure
		};
	TInt ActionEvent(TEvent aEvent);
 	TInt ActionEvent(TEvent aEvent,const TChar& aTone);
 	TInt ProcessTone(const TChar& aTone,TBool aStartTimer);

	void CompleteDtmfStringTx(TInt aStatus);
	void CheckNotification();
	TInt SetDtmfCall();
	TBool CheckForActiveVoiceCall();
	void TimerCallBack(TInt aId);

private:
	CSimPhone* iPhone;		//< Pointer to the parent phone class.
	CSimTimer* iTimer;		//< Pointer to the owned timer class.
	HBufC* iDtmfData;		//< Pointer to buffer holding string of DTMF characters.

	TBool iNotifyStopChar;				//< Flag indicating that a DTMF Stop Notification is outstanding.
	TTsyReqHandle iNotifyStopReqHandle;	//< The request handle associated with an outstanding DTMF Stop Notification.
	enum TState
		{
		EIdle,
		ETxTone,
		EStopped
		};
	TState iState;						//< DTMF Tx engine state.
	TBool iDtmfString;					//< Flag indicating that a DTMF string tx is ongoing.
	TTsyReqHandle iDtmfStringReqPending;//< The request handle associated with an ongoing DTMF string transmission.
	TInt iDtmfStringIndex;				//< The index of the next DTMF character to be processed when tx'ing a DTMF string.
	};

#endif
