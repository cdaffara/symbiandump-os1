/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Header for NetDial ETel interface.
* 
*
*/



/**
 @file Nd_etel.h
 @internalComponent
*/


#ifndef __ND_ETEL_H__
#define __ND_ETEL_H__

#include <cdblen.h>
#include <etelmm.h>

// Forward declarations

class CCommsDbNetDialAccess;

class MNetDialEtelObserver
/**
MNetDialEtelObserver class is used by CTelServerProcessor to complete the request of the calling state.

@internalComponent
*/
	{
public:
	virtual void TelFunctionComplete(TInt aError) =0;
	};

class CTelServerProcessor : public CActive
/**
CTelServerProcessor, one of these per CNetDial object. Implements calls to ETEL.

@internalComponent
*/
	{
private:
	enum {
		EIdle,
		EDialUp,
		EWait,
		EHangUpDialOut,
		EHangUpDialIn
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT		
		,EListenForStatusChange
#endif		
		} iState;
public:
	static CTelServerProcessor* NewL(CCommsDbNetDialAccess* aDb, TInt aPriority =EPriorityStandard);
	CTelServerProcessor(CCommsDbNetDialAccess* aDb, TInt aPriority);
	~CTelServerProcessor();
	void StartDialUpL(MNetDialEtelObserver& aObserver);
	void StartReconnectL(MNetDialEtelObserver& aObserver);
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT	
	void ListenForStatusChange(MNetDialEtelObserver& aObserver);	
#endif
	void WaitForIncomingCallL(MNetDialEtelObserver& aObserver);
	void StartHangUpAfterDialOut(MNetDialEtelObserver& aObserver);
	void StartHangUpAfterDialIn(MNetDialEtelObserver& aObserver);
	void GetCommPortL(RCall::TCommPort& aCommPort);
	void ReturnCommPortL();
	void CloseCall();
	TBool CommPortLoaned() const;
	TBool CallActive() const;
private:
	void ConstructL();
	void OpenNewCallL();
	void GetPhoneInfoL(RTelServer::TPhoneInfo& aInfo);
        void GetLineInfoL(RPhone::TLineInfo& aInfo,const TDesC& aPhoneName, TUint aLineType);
	void SetMmParametersL();
	void DialL();
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT	
	void HandleStatusChange();
	void CancelListenForStatusChange();
#endif	
	void StartHangUp();
	static void DialUpCompleted(TAny* aPtr,TInt aStatus);
	static void HangUpCompleted(TAny* aPtr,TInt aStatus);
	// from CActive
	virtual void DoCancel();
	virtual void RunL();
	//
	TBool RepeatDialL();
	void Assertions();
	RCall& ActiveCall();
private:
	enum {
		EUnknown =0,
		ECoreCallOnly,
		EMmDataCall,
		EMmHscsdCall

		} iCallType;

	CCommsDbNetDialAccess* iDb;
	RTelServer iTelServer;
	// Core call
	RCall iCall;
	RCall::TCallParams iCallParams;
	RCall::TCallParamsPckg iCallParamsPckg;
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT	
	RCall::TStatus iCallStatus;
#endif	

	// MM API phone object
	RMobilePhone								iMmPhone;
	// MM API call object
	RMobileCall 								iMmCall;
	// MM API call parameters
	RMobileCall::TMobileCallParamsV1			iMmCallParams;
	RMobileCall::TMobileCallParamsV1Pckg		iMmCallParamsPckg;
	// MM API data call parameters
	RMobileCall::TMobileDataCallParamsV1		iMmDataCallParams;
	RMobileCall::TMobileDataCallParamsV1Pckg	iMmDataCallParamsPckg;
	// MM API hscsd call parameters
	RMobileCall::TMobileHscsdCallParamsV1		iMmHscsdParams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg	iMmHscsdParamsPckg;

	TBool iTsyLoaded;
	TDialString iTelNum;
	MNetDialEtelObserver* iCurrentObserver;
	TBool iLoaned;
	TBool iCallOpen;
	TUint32 iDialCounter;
	TBuf<KCommsDbSvrMaxFieldLength> iTsyName;
	TUint32 iMaxDialAttempts;
	};

#endif
