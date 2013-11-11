// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TE_SMS_TESTSTEP_BASE__
#define __TE_SMS_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>
#include <e32base.h>
#include <e32test.h>
#include <c32comm.h>
#include "f32file.h"
#include <etel.h>
#include <etelmm.h>
#include <exterror.h>   // for kerrgsmsmsfailureinme
#include <mmretrieve.h>

typedef TBuf<RMobilePhone::KMaxMobileTelNumberSize> TTelNumberBuf;

_LIT(KtsyName,"MM");
_LIT(KPhoneName,"GsmPhone1");
_LIT(KTSSMSConfigFile,"te_sms.ini"); 
_LIT(KTestSCANumber,"TestServiceCentreAddressNumber");
_LIT(KestSimNumber,"TestPhoneSimNumber");

class CSmsTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;		
	RMobileSmsMessaging iSms;
	TTelNumberBuf iSCANumber;
	TTelNumberBuf iTelNumber;
	RMobileSmsMessaging::TMobileSmsCapsV1 iSmsCaps;
	TRequestStatus iStatus;

protected:
	TInt AppendAddressToAscii(TDes8& aAscii,const RMobilePhone::TMobileAddress& aAddress);
	void AppendOctet(TInt aOctet,TDes8& aAscii);
	TBool IsAddressChar(TChar aChar);
	TInt ConvertTypeOfNumber(RMobilePhone::TMobileTON aEnum);
	TInt ConvertNumberingPlan(RMobilePhone::TMobileNPI aEnum);  
	TInt ConvertAddressChar(TChar aChar);
	void GetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry);
	void SetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry);
	void PrintMessageStoreCaps(TUint32 aCaps);
	TInt SendSms(TBool aQuiet=EFalse);
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds);

private:
	CActiveScheduler* iTestScheduler;
	};

class CConfigParams : public CBase
	{
public:
	static CConfigParams* NewL(const TDesC &aCfgFileName);
	const TPtrC FindAlphaVar(const TDesC &aVarName,const TDesC &aDefault);
	TInt	FindNumVar(const TDesC &aVarName, const TInt aDefault);
protected:
	void ConstructL(const TDesC &aCfgFileName);
	CConfigParams(){};
private:
	TBool iFileExist;
	TBuf8<500> iConfigParamBuf8;
	TBuf<1000> iConfigParamBuf;
	};

class CTestGetSmspList : public CActive
	{
public:
	static CTestGetSmspList* NewLC(RMobileSmsMessaging& aSmsMessaging);
	~CTestGetSmspList();
	void Start();
	CMobilePhoneSmspList* RetrieveList();
protected:
	CTestGetSmspList(RMobileSmsMessaging& aSmsMessaging);
	void ConstructL();
	void RunL();
	void DoCancel();
protected:
	CRetrieveMobilePhoneSmspList* iRetrieve;
	RMobileSmsMessaging& iSmsMessaging;
	};

#endif // __TE_SMS_TESTSTEP_BASE__
