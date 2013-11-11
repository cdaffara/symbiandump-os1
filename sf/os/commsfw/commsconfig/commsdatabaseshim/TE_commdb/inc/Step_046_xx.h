/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for CommDb test 046.XX group of scenarios 

#if (!defined __STEP_046_XX_H__)
#define __STEP_046_XX_H__


////////////////////
//	Test step 046.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest046_01) : public CTestStepCommDb
{
public:
	CCommDbTest046_01();
	~CCommDbTest046_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 046.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest046_02) : public CTestStepCommDb
{
public:
	CCommDbTest046_02();
	~CCommDbTest046_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:
	void insertTestRecordsL( TUint aQuantity );
	void deleteLastRecordsL( TUint aQuantity );

};

////////////////////
//	Test step 046.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest046_03) : public CTestStepCommDb
{
public:
	struct TThreadInfo
	{
	TRequestStatus* iThread1Status;
	TRequestStatus* iMainThreadStatus;
	RThread iMainThread;
	};

	static void DoThread1TestL(TAny* aPtr);
	CCommDbTest046_03();
	~CCommDbTest046_03();

	static TInt Thread1Function(TAny* aPtr);

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 046.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest046_04) : public CTestStepCommDb
{
public:
	struct TThreadInfo
	{
	TRequestStatus* iStatus1;
	TRequestStatus* iStatus2;
	RThread iCaller;
	};

	static TInt Thread1Function(TAny* aPtr);
	static void DoThread1TestL(TAny* aPtr);

	CCommDbTest046_04();
	~CCommDbTest046_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 046.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest046_05) : public CTestStepCommDb
{
public:
	struct TThreadInfo
	{
	TRequestStatus* iStatus1;
	RThread iCaller;
	};

	static TInt Thread1Function(TAny* aPtr);
	static void DoThread1TestL(TAny* aPtr);

	CCommDbTest046_05();
	~CCommDbTest046_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	static TBool InsertTestRecordsL(TBool aCallerIsMainThread, TUint aQuantity, CCommsDatabase& aDb );
	static void DeleteLastRecordsL( TBool aCallerIsMainThread, TUint aQuantity, CCommsDatabase& aDb );
	static void RandomDelay(TInt64 &aSeed);
	static void PrintDebug(TPtrC aString);
};
#endif //__STEP_046_XX_H__
