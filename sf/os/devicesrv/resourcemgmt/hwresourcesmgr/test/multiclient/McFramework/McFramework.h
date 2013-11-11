/*
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
*
*/



#ifndef TC_MCFRAMEWORK_H
#define  TC_MCFRAMEWORK_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>
#include <hwrmpowerstatesdkpskeys.h>

// CONSTANTS

const TUid KMyHWRMTestProperty={0x10005B92}; 
const TUint32 EHWRMNextTestToRun = 0x00001001;
const TUint32 EHWRMStepResult = 0x00001002;
const TInt EHWRMQuitTest = 0xFFFFFFFF;

// MACROS

// DATA TYPES

enum THWRMMultiClientCases
	{
	EMultiClientCase1 = 1,
	EMultiClientCase2 = 2,
	EMultiClientCase3 = 3,
	EMultiClientCase4 = 4,
	EMultiClientCase5 = 5,
	EMultiClientCase6 = 6,
	EMultiClientCase7 = 7,	
	EMultiClientCase8 = 8,
	EMultiClientCase9 = 9,
	EMultiClientCase10 = 10,
	EMultiClientCase11 = 11,
	EMultiClientCase12 = 12,
	EMultiClientCase13 = 13,
	EMultiClientCase14 = 14
	};

enum THWRMMultiClientKeys
	{
	EMultiClientResult = 1,
	EMultiClientCase = 2,
	EPSBatteryLevel = 3,
	EPSBatteryStatus = 4,
	EPSChargingStatus = 5
	};


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CTestCasePropertyWatch : public CActive
	{
	enum {EPriority=0};
public:
	static CTestCasePropertyWatch* NewL(TInt aKey);
	TInt iStatusReceived;
	TInt iKeyResult;
	void Start();
	~CTestCasePropertyWatch();
		
private:
	CTestCasePropertyWatch();
	void ConstructL(TInt aKey);
	
private: //from base class

	void RunL();
	void DoCancel();
	
private:
	RProperty iProperty;
	//RTimer iTimer;
	};
	

class CTimeOutTimer : public CActive
{
	public:
		static CTimeOutTimer* NewL();
		~CTimeOutTimer();
		void RequestTimeOut(TInt aSeconds);
		void RunL();
		void DoCancel();
		TBool iTimeOutOccured;
		
	protected:
		CTimeOutTimer();		
		void ConstructL();
	private:
		RTimer iTimer;
		
	
};

class CMcFramework : public CBase
{
	
public:    
	static CMcFramework* NewL();
	~CMcFramework();

	TInt WaitTestToStart();
	TInt StartTestCaseInSlave(TInt aTcase);
	TInt WaitNextTestStep();
	void StepExecuted(TInt aTstep);
	void StopMultiClientTestInSlave( );	

protected:
	void ConstructL();
	
private:
    TInt iTcNumber; 	
	TInt iStepResult;
	TInt timeOutCounter;
    //RProperty iTcaseProp;
    //RProperty iTstepProp;
    CTestCasePropertyWatch* iTestCasePropertyWatch;
    CTimeOutTimer* iTimeOutTimer;
};

#endif
