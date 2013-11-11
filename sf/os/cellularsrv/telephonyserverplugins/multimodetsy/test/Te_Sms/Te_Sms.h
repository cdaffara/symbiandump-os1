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
// GSM Sms tests Header file
// 
//

/**
 @file
 @internalComponent 
*/


#ifndef __TE_SMS_H__
#define __TE_SMS_H__

#include "Te_SmsTestStepBase.h"

#define MODULENAME	_L("GSMBSC")
#define PHONENAME	_L("GsmPhone1")
#define DATALINENAME	_L("Data")
#define FAXLINENAME	_L("Fax")
#define VOICELINENAME _L("Voice")
#define CALLNAME	_L("Harry")
#define CALLNAME2	_L("John")
#define SMS_OPEN_FAXCALL_FROM_PHONE_NAME	_L("Fax::Harry")
#define SMS_OPEN_FAXCALL_FROM_SERVER_NAME	_L("GsmPhone1::Fax::Harry")
#define SMS_OPEN_DATACALL_FROM_SERVER_NAME	_L("GsmPhone1::Data::Harry")
#define SMS_OPEN_DATALINE_FROM_SERVER_NAME	_L("GsmPhone1::Data")
#define PHONENUMBER1 _L8("386")			//01632960000 , 9,0467439100
#define PHONENUMBER2 _L8("")
#define RESPONDER_SEMAPHORE_NAME _L("ResponderStartupSem")
#define RESPONDER_NAME	  _L("Responder")
#define ONE_SECOND	1000000

#define LEAVE_IF_ERROR(b)\
{\
const TInt a=b;\
if (a!=KErrNone)\
	{\
    TText8* fileChars = (TText8*)__FILE__;\
    TPtrC8 fileName8(fileChars);\
	TBuf<128> fileName;\
	fileName.Copy(fileName8);\
    INFO_PRINTF4(_L("%S ret=%d line=%d"),&fileName,a,__LINE__);\
	}\
User::LeaveIfError(a);\
}

#define LEAVE(b)\
    {\
	const TInt a=b;\
	TText8* fileChars = (TText8*)__FILE__;\
    TPtrC8 fileName8(fileChars);\
	TBuf<128> fileName;\
	fileName.Copy(fileName8);\
    INFO_PRINTF4(_L("%S ret=%d line=%d"),&fileName,a,__LINE__);\
	User::Leave(a);\
	}


class RTelServer;

/**
 * \class CTestSms Te_sms.h "test/Te_sms/Te_sms.h"
 * \brief Test class that enables GSM Sms tests.
 * 
 * CTestSms inherits from the CSmsTestStep class.
 */

class CTestSmsMessagingOpen : public CSmsTestStep
	{
public:
	CTestSmsMessagingOpen() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetCaps : public CSmsTestStep
	{
public:
	CTestGetCaps() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestSmspList : public CSmsTestStep
	{
public:
	CTestSmspList() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestMessageStoreInfo : public CSmsTestStep
	{
public:
	CTestMessageStoreInfo() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestMessageStoreOps : public CSmsTestStep
	{
public:
	CTestMessageStoreOps() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestSendSms : public CSmsTestStep
	{
public:
	CTestSendSms() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestReceiveUnstoredSms : public CSmsTestStep
	{
public:
	CTestReceiveUnstoredSms() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestReceiveStoredSms : public CSmsTestStep
	{
public:
	CTestReceiveStoredSms() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestReceiveStoredSmsAndCancel : public CSmsTestStep
	{
public:
	CTestReceiveStoredSmsAndCancel() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestSendSmsAndCancel : public CSmsTestStep
	{
public:
	CTestSendSmsAndCancel() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestAckSmsStored : public CSmsTestStep
	{
public:
	CTestAckSmsStored() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestAckSmsStoredCancel : public CSmsTestStep
	{
public:
	CTestAckSmsStoredCancel() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNackSmsStored : public CSmsTestStep
	{
public:
	CTestNackSmsStored() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNackSmsStoredCancel : public CSmsTestStep
	{
public:
	CTestNackSmsStoredCancel() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestAckSmsStoredWithResponse : public CSmsTestStep
	{
public:
	CTestAckSmsStoredWithResponse() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestAckSmsStoredWithResponseCancel : public CSmsTestStep
	{
public:
	CTestAckSmsStoredWithResponseCancel() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNackSmsStoredWithResponse : public CSmsTestStep
	{
public:
	CTestNackSmsStoredWithResponse() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNackSmsStoredWithResponseCancel : public CSmsTestStep
	{
public:
	CTestNackSmsStoredWithResponseCancel() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestResumeSmsReception : public CSmsTestStep
	{
public:
	CTestResumeSmsReception() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNotifySmsEvent : public CSmsTestStep
	{
public:
	CTestNotifySmsEvent() ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNotifySmsEventCancel : public CSmsTestStep
	{
public:
	CTestNotifySmsEventCancel() ;
	virtual enum TVerdict doTestStepL();
	};

/*class CTestSms : public CSmsTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	CTestSms();
	~CTestSms();

private:
	RMobileCall iVoiceCall;
	RMobileCall iDataCall;
	TBool ibReceivedStoredSupported;
	TBool ibReceivedUnstoredSupported;
	TRequestStatus iStatus;
	CPeriodic* iPeriodic;
	
	TUint msgRef;
	TRequestStatus aBooReadStatus;
	TRequestStatus aBooWriteStatus;
	TRequestStatus aBooDeleteStatus;

	TInt aIndex1;
	TInt aIndex2;
	TBool isTrue;
	RMobileSmsStore::TMobileGsmSmsEntryV1 aMsg;
	RMobileSmsStore::TMobileGsmSmsEntryV1 aMsg2;

	TPtrC8 pduPtr;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendAttr;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendAttr1;
	TBuf8<256> smsData;
	};
*/

#endif // __TE_SMS_H__
