// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file for sms prot stress test
// 
//

/**
 @file
*/

#ifndef TE_SMSPRTSTRESSUTILS_H
#define TE_SMSPRTSTRESSUTILS_H

#include "TE_smsprtstressbase.h"

/*-------------------------------------------------------------------------------

	CTestSmsActiveBase

	Base class for test active objects

-------------------------------------------------------------------------------*/
class CTestSmsActiveBase : public CActive
	{
public:
	CTestSmsActiveBase(TInt aHowManyRequests);
	
protected:
	RSocket iSocket;
	TPckgBuf<TUint> iSbuf;
	TUint8 iCounter;
	TInt iHowManyRequests;
	};

/*
-------------------------------------------------------------------------------

	CTestSmsActiveStop

	"Stopper" class for test active objects. This class stops all testing
	activity if tests are ran through or if an error is occurred.

-------------------------------------------------------------------------------
*/
class CTestSmsActiveStop
	{
public:
	CTestSmsActiveStop(RPointerArray<CTestSmsActiveBase>& aTestSmsActives,TInt& aRet);
	void StopTests(TInt aStatus);
	
private:
	RPointerArray<CTestSmsActiveBase>& iTestSmsActives;
	TInt& iRet;
	};

/*
-------------------------------------------------------------------------------

	CTestSmsSendActive

	This class makes send SMS requests to the SMS Stack.

-------------------------------------------------------------------------------
*/

class CTestSmsSendActive : public CTestSmsActiveBase
	{
public:
	static CTestSmsSendActive* NewL(RSocketServ& aSocketServer,CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CSmsStackTestUtils* aUtils, CTestStep* aTestStep);
	CTestSmsSendActive(CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, CTestStep* aTestStep);
	void CreateSmsWithStatusReportReqL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RFs& aFs) ;
	~CTestSmsSendActive();
	void StartL();
	
protected:
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	void SendMessageL();
	CSmsMessage* CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RFs& aFs, CSmsPDU::TSmsPDUType aType = CSmsPDU::ESmsSubmit);
private:
	CSmsMessage* iSmsMessage;
	CTestSmsActiveStop& iTestSmsActiveStop;
	CSmsStackTestUtils* iSmsStackTestUtils;
	TSmsServiceCenterAddress iTelephoneNumber;
	TSmsServiceCenterAddress iServiceCenterNumber;
	CTestStep* iTestStep;
	};

/*
-------------------------------------------------------------------------------

	CTestSmsParametersActive

	This class makes retrieve and store parameters requests to the
	SMS Stack. It also get retrieved parameter list from stream and
	compare it to the stored list.

-------------------------------------------------------------------------------
*/
class CTestSmsParametersActive : public CTestSmsActiveBase
	{
public:
	static CTestSmsParametersActive* NewL(RSocketServ& aSocketServer,CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, CSmsStackTestUtils* aSmsStackTestUtils, CTestStep* aTestStep);
	CTestSmsParametersActive(CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, CSmsStackTestUtils* aSmsStackTestUtils, CTestStep* aTestStep);
	~CTestSmsParametersActive();
	void StartL();
	
protected:
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	void StoreParametersL();
	void RetrieveParameters();
	void GetParametersL();
	
private:
	enum TSmsDeleteAndEnumState
		{
		ESmsNextStoreParameters,
		ESmsNextRetrieveParameters,
		ESmsNextGetParameters
		};
	
private:
	TSmsDeleteAndEnumState iState;
	CMobilePhoneSmspList* iStoreSmspList;
	CTestSmsActiveStop& iTestSmsActiveStop;
	CSmsStackTestUtils* iSmsStackTestUtils;
	CTestStep* iTestStep ;
	};
/*
-------------------------------------------------------------------------------

	CTestSmsStorageActive

	This class makes enumerate, write and delete messages requests to the
	SMS Stack. It also get enumerated messages from stream and
	compare them to the stored messages.

-------------------------------------------------------------------------------
*/
class CTestSmsStorageActive : public CTestSmsActiveBase
	{
public:
	static CTestSmsStorageActive* NewL(RSocketServ& aSocketServer,CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CSmsStackTestUtils* aSmsStackUtils, CTestStep* aTestStep);
	CTestSmsStorageActive(CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CTestStep* aTestStep);
	~CTestSmsStorageActive();
	void StartL();
	
protected:
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	void WriteMessageL();
	void DeleteMessageL();
	void EnumerateMessages();
	void GetMessagesL();
	CSmsMessage* CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RFs& aFs, CSmsPDU::TSmsPDUType aType = CSmsPDU::ESmsSubmit);
	void TestSmsListsL(RPointerArray<CSmsMessage>& aLeft,RPointerArray<CSmsMessage>& aRight) ;
	
private:
	enum TSmsDeleteAndEnumState
		{
		ESmsNextWriteMessage,
		ESmsNextEnumerateMessages,
		ESmsNextGetMessages,
		ESmsNextDeleteMessage
		};
	
private:
	CTestSmsActiveStop& iTestSmsActiveStop;
	RPointerArray<CSmsMessage> iEnumeratedMessages;
	RPointerArray<CSmsMessage> iStoredMessages;
	TSmsDeleteAndEnumState iState;
	TInt iReadMessagesCount;
	TSmsServiceCenterAddress iTelephoneNumber;
	TSmsServiceCenterAddress iServiceCenterNumber;
	RFs& iFs;
	CTestStep* iTestStep ;
	};

/*
-------------------------------------------------------------------------------

	CTestSmsReceiveActive

	This class makes receive message requests to the
	SMS Stack.

-------------------------------------------------------------------------------
*/
class CTestSmsReceiveActive : public CTestSmsActiveBase
	{
public:
	static CTestSmsReceiveActive* NewL(RSocketServ& aSocketServer,CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CSmsStackTestUtils* aSmsStackTestUtils, CTestStep* aTestStep);
	CTestSmsReceiveActive(CTestSmsActiveStop& aTestSmsActiveStop,TInt aHowManyRequests, RFs& aFs, CTestStep* aTestStep);
	~CTestSmsReceiveActive();
	void StartL();
	
protected:
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	void ReceiveMessage();
	void GetMessagesL();
	void PrintMessageL(const CSmsMessage* aSms);
	TText8 IsCharDisplayable( const TText8 aChar );
	
private:
	enum TSmsReceiveState
		{
		ESmsNextReceiveMessage,
		ESmsNextGetMessage,
		};
	
private:
	TSmsReceiveState iState;
	CTestSmsActiveStop& iTestSmsActiveStop;
	TInt iNumberReceived;
	RFs& iFs ;
	CTestStep* iTestStep ;
	};

#endif  // TE_SMSPRTSTRESSUTILS_H
