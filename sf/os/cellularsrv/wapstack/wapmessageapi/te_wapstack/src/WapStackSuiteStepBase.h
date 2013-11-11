/**
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file WapStackSuiteStepBase.h
*/

#if (!defined __WAPSTACK_SUITE_STEP_BASE__)
#define __WAPSTACK_SUITE_STEP_BASE__
#include <test/testexecutestepbase.h>
#include <es_wsms.h>

#define WAP_MAX_HOST_NAME 128

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define PDD_NAME2 _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

typedef enum
	{
	EIdle,
	EConnect,
	ESend,
	EAwaitSize,
	EReceive,
	EMutipleReceive,
	EAwaitPush,
	EUnitInvoke,
	EUnitResult,
	EFinish,
	EAbortTest
	} TWapStep;
const TUint KMaxWapWdpPduBufferLength=1080;
const TUint KMaxWapWspHeaderBufferLength=256;
const TUint KMaxWapWspDataBufferLength=1080;
class MControlNotify
{
public:
	virtual TInt CallStateMachine() = 0;
};

class CActiveControl : public CActive
{
public:
	static CActiveControl* NewL(MControlNotify* aControl);
	CActiveControl(MControlNotify* aControl);
	~CActiveControl();
	void ConstructL();
	virtual void RunL();
	virtual void DoCancel();
	virtual void ReStart();
	inline TRequestStatus* Status() {return &iStatus;}
public:
	MControlNotify* iControl;
};

class CWapStackSuiteStepBase : public CTestStep, public MControlNotify
	{
public:
	CWapStackSuiteStepBase();
	virtual ~CWapStackSuiteStepBase();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TInt CallStateMachine();
	void StartScheduler();
	void StopScheduler();
	void WaitForInitializeL();
	inline void SetScheduler(CActiveScheduler* aScheduler) {iScheduler = aScheduler;}
	inline CActiveScheduler* Scheduler() {return iScheduler;}
	inline void SetState(TWapStep aState) { iState = aState; }
	inline TWapStep State() { return iState; }
	inline void SetLastError(TInt aError) { iLastError = aError; }
	inline TInt GetLastError() { return iLastError; }
	inline void SetExpectedError(TInt aError) {iExError = aError; }
	inline TInt GetExpectedError() { return iExError; }
	inline TBool UseSimTsy() { return iUseSIMTSY; }
	inline TBool DetailedLogging() { return iDetailedLogging; }
	void LoadConfig();
	void DumpBuf(const TDesC8& aBuf);
	void ShowConnectionInfo();
protected:
	CActiveControl* iControl;
	TWapStep iState;
	TRequestStatus* iStatus;

	//Predefined value in .ini file
	TInt iExError;
	TInt iExError2;
	TInt iRepeat;
	TUint32 iTimeout;
	TBool iSecure;
	TBool iReconnect;
	TBool iUseSIMTSY;
	TBool iAwaitLengthCancel;
	TBool iMiddleCancel;
	TBool iMultiReadCancel;
	TBool iAwaitLengthTwice;
	TBool iDetailedLogging;
	TBool iDelayPortClosure;
	TInt iSimTsyState;
	TUint16 iWdpRecvBufLength;
	TUint16 iHeaderRecvBufLength;
	TUint16 iBodyRecvBufLength;
	Wap::TBearer iBearer;
	Wap::TPort iLocalPort;
	Wap::TPort iRemotePort;
	TBuf8<WAP_MAX_HOST_NAME> iRemoteHost;
	TBuf8<WAP_MAX_HOST_NAME> iLocalHost;
	TInetAddr iRemoteAddr;
	TInetAddr iLocalAddr;
	TInt iRmtPort;
	TInt iLclPort;	
	//For WDP sending and receiving
	HBufC8* iSendBuffPtr;
	HBufC8* iRecvBuffPtr;
	TUint16 iLenBuffer;
	TPckg<TUint16> iLength;
	TPtr8 iRecvBuf;
	TBool iTruncated;
	Wap::TPort iRecvRemotePort;
	TBuf8<WAP_MAX_HOST_NAME> iRecvRemoteHost;
	TBuf8<KMaxWapWdpPduBufferLength> iMaxWdpRecvBuffer;


	//For both WSP Sending;
	TUint8 iTransactionId;
	TUint iMethod;
	HBufC16* iURI;
	HBufC8* iSendHeaders;
	HBufC8* iSendBody;

	//For WSP Receiving and Push
	TPckgBuf<TUint8> iTransactionIdPckg;
	TWSPStatus iWspStatus;
	HBufC8* iRecvHeaders;
	HBufC8* iRecvBody;
	TPtr8 iRecvHeadersBuf;
	TPtr8 iRecvBodyBuf;
	TBuf8<KMaxWapWspHeaderBufferLength> iMaxWspHeaderRecvBuffer;
	TBuf8<KMaxWapWspDataBufferLength> iMaxWspDataRecvBuffer;
private:
	CActiveScheduler* iScheduler;
	TInt iLastError;
	};

#endif
