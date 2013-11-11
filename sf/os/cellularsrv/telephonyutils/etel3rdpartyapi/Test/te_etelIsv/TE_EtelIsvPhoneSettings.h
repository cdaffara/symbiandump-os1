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


#ifndef __TE_ETELISVPHONESETTINGS_H_
#define __TE_ETELISVPHONESETTINGS_H_

/**
 * This class implements a state machine that executes a series of asynchronous
 * requests.
 */

class CTestGetPhoneId : public CEtelIsvTestStep
	{
public:
	CTestGetPhoneId() ;
	~CTestGetPhoneId(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetPhoneIdError : public CEtelIsvTestStep
	{
public:
	CTestGetPhoneIdError() ;
	~CTestGetPhoneIdError(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetPhoneIdAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetPhoneIdAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros = ETrue);
	void ConstructL();
	~CTestGetPhoneIdAct();
	void GetPhoneId(TDes8& aExpectedPhoneIdPckg, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetPhoneIdAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros);
public:
	CTelephony::TPhoneIdV1 iPhoneId;
private:	// data
	TDes8* iExpectedPhoneIdPckg;
	TBool iUseMacros;
	TPckgBuf<CTelephony::TPhoneIdV1> iPhoneIdBuff;
	CTelephony::TPhoneIdV1Pckg iPhoneIdPckg;
	};

class CTestGetSubscriberId : public CEtelIsvTestStep
	{
public:
	CTestGetSubscriberId() ;
	~CTestGetSubscriberId(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetSubscriberIdError : public CEtelIsvTestStep
	{
public:
	CTestGetSubscriberIdError() ;
	~CTestGetSubscriberIdError(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetSubscriberIdAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetSubscriberIdAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetSubscriberIdAct();
	void GetSubscriberId(TDes8& aExpSubscriberId, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetSubscriberIdAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TSubscriberIdV1 iSubscriberId;
	TDes8* iExpSubscriberIdPckg;
	
	TPckgBuf<CTelephony::TSubscriberIdV1> iSubscriberIdBuff;
	CTelephony::TSubscriberIdV1Pckg iSubscriberIdPckg;
	};

class CTestGetIndicator : public CEtelIsvTestStep
	{
public:
	CTestGetIndicator() ;
	~CTestGetIndicator(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetIndicatorAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetIndicatorAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetIndicatorAct();
	void GetPhoneIndicator(TUint32 aExpIndCaps, TUint32 aExpInd,TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetIndicatorAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TIndicatorV1 iIndicator;
	TUint32 iExpIndCaps;
	TUint32 iExpInd;
	TRequestStatus iExpStat;
	TPckgBuf<CTelephony::TIndicatorV1> iIndicatorBuff;
	CTelephony::TIndicatorV1Pckg iIndicatorPckg;
	};

class CTestGetBatteryInfo : public CEtelIsvTestStep
	{
public:
	CTestGetBatteryInfo() ;
	~CTestGetBatteryInfo(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetBatteryInfoAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetBatteryInfoAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetBatteryInfoAct();
	void GetBatteryInfo(TDes8& aBatteryExpInfoPckg, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetBatteryInfoAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TBatteryInfoV1 iBatteryInfo;
	TDes8* iBatteryExpInfoPckg;	
	
	TPckgBuf<CTelephony::TBatteryInfoV1> iBatteryInfoBuff;
	CTelephony::TBatteryInfoV1Pckg iBatteryInfoPckg;
};

class CTestGetSignalStrength : public CEtelIsvTestStep
	{
public:
	CTestGetSignalStrength() ;
	~CTestGetSignalStrength(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetSignalStrengthAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetSignalStrengthAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros = ETrue);
	void ConstructL();
	~CTestGetSignalStrengthAct();
	void GetSignalStrength(TDes8& aExpSignalStrengthPckg, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetSignalStrengthAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TBool aUseMacros);
public:
	CTelephony::TSignalStrengthV1 iSignalStrength;
private:	// data	
	TDes8* iExpSignalStrengthPckg;
	TBool iUseMacros;
	
	TPckgBuf<CTelephony::TSignalStrengthV1> iSignalStrengthBuff;
	CTelephony::TSignalStrengthV1Pckg iSignalStrengthPckg;
	};

class CTestGetLockInfo : public CEtelIsvTestStep
	{
public:
	CTestGetLockInfo() ;
	~CTestGetLockInfo(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetLockInfoAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestGetLockInfoAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestGetLockInfoAct();
	void GetLockInfo(CTelephony::TIccLock aLock, TDes8& aExpLockInfoPckg, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestGetLockInfoAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TIccLockInfoV1 iLockInfo;
	TDes8* iExpLockInfoPckg;
	CTelephony::TIccLock iLock;
	
	TPckgBuf<CTelephony::TIccLockInfoV1> iLockInfoBuff;
	CTelephony::TIccLockInfoV1Pckg iLockInfoPckg;
	};

class CTestSendDTMFTones : public CEtelIsvTestStep
	{
public:
	CTestSendDTMFTones() ;
	~CTestSendDTMFTones(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSendDTMFTonesAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestSendDTMFTonesAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL();
	~CTestSendDTMFTonesAct();
	void SendDTMFTone(const TDesC& aTones, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestSendDTMFTonesAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:
	CTelephony::TCallId iCallId;
	};
	
class CTestFlightModeWithPublishSubscribe : public CEtelIsvTestStep
	{
public:
	CTestFlightModeWithPublishSubscribe() ;
	~CTestFlightModeWithPublishSubscribe(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestPhoneFuncFlightModeOnWithPublishSubscribe : public CEtelIsvTestStep
	{
public:
	CTestPhoneFuncFlightModeOnWithPublishSubscribe() ;
	~CTestPhoneFuncFlightModeOnWithPublishSubscribe(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
#endif
