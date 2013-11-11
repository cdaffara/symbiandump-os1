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
// This file contains the Data Call class definitions.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMDATACALL_H__
#define __CSIMDATACALL_H__

#include "CSimCall.h"
#include "csimtimer.h"
#include "CSimNtras.h"

/**
* CSimDataCall.h Handles data specific Call functionality
*
* Created and owned by the CSimLine class.
*/
class MTimerCallBack;
class CSimDataCall : public CSimCall, MTimerCallBack, MNTRasCallBack
	{
public:
	static CSimDataCall* NewL(CSimLine* aLine,const TDesC& aName, CSimPhone* aPhone);
	CSimDataCall(CSimLine* aLine,const TName& aName, CSimPhone* aPhone);
	~CSimDataCall();

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

	virtual TInt GetMobileDataCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aParams);
	virtual TInt GetMobileDataCallCapsCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt GetMobileDataCallRLPRange(const TTsyReqHandle aTsyReqHandle, TInt* aRLPVersion, TDes8* aRLPRange);
	virtual TInt GetMobileDataCallRLPRangeCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt NotifyMobileDataCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps);
	virtual TInt NotifyMobileDataCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt SetDynamicHSCSDParams(const TTsyReqHandle aTsyReqHandle, RMobileCall::TMobileCallAiur* aAiur, TInt* aRxTimeslots);
	virtual TInt SetDynamicHSCSDParamsCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt GetCurrentHSCSDInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aHSCSDInfo);
	virtual TInt NotifyHSCSDInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aHSCSDInfo);
	virtual TInt NotifyHSCSDInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);

	virtual TInt LoanDataPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aCommPort);
	virtual TInt LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt RecoverDataPort(const TTsyReqHandle aTsyReqHandle);

	virtual void TimerCallBack(TInt aId);
	virtual void NTRasCallBack(TInt aStatus);
	virtual TInt ActionEvent(TCallEvent aEvent,TInt aStatus);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);

	virtual TUint Caps();

private:
	void ConstructL();
	void StartTimer(TInt aDuration);
	TInt ProcessAnswerIncomingCallL();
	TInt ProcessRemoteHangupL();
	void PopulateCallParams(const TDesC8* aCallParams);
	void ChangeDynamicInfo();
	TInt SearchRetrieveRlp(TInt* aRLPVersion, TDes8* aRLPRange);

private:
	class TMobileCallRLPItem
	{
	public:
		TInt iRlpVersion;
		RMobileCall::TMobileDataRLPRangesV1 iMobileCallRLP;
	};

private:
	CSimTimer* iTimer;
	CSimNtRas* iNtRas;
	TInt iDiallingPause;		//< duration spent in dialling state, before entering connecting state.
	TInt iConnectingPause;		//< duration spent in connecting state, before entering connected state.
	TInt iDisconnectingPause;   //< duration spent in disconnecting state, before entering disconnected state.
	TInt iAnswerIncomingPause;  //< duration spent in Answering state, before entering connected state.
	TInt iRemoteHangupPause;  	//< duration spent in connected state, before entering disconnecting state.

	TName iCsyName;				//< The CSY Name associated with the NTRas port.
	TName iPortName;			//< The Port Name associated with the NTRas port.
	TCommConfigV01 iConfig;		//< The configuration to be used with the NTRas port.
	TBool iCommPortLoaned;		//< A flag indicating whether the CommPort is loaned.
	TBool iHscsdCall;			//< Is this a Hscsd call?
	
	RCall::TBearerService iBearerService;	//< The bearer information associated with a data call.
	RMobileCall::TMobileDataCallParamsV1 iMobileCallParams; //< The current datacall params
	RMobileCall::TMobileCallDataCapsV1 iMobileCallCaps;		//< The current hscsd params
	RMobileCall::TMobileHscsdCallParamsV1 iHscsdSettings;	//< The current hscsd dynamic params
	RMobileCall::TMobileCallHscsdInfoV1  iHscsdInfo;		//< The current hscsd dynamic info

	CArrayFixFlat<TMobileCallRLPItem>* iMobileCallRLPList;  //< A list of all RLP versions supported and ranges 
	TNotifyData	iNotifyInfo;	//< Holds the notification data when Hscsd info changes	
	};

#endif // CSimDataCall_H__
