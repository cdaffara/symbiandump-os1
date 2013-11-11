// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestPARAM_SVR_SUITENAMESuite 
// 
//

#if (!defined CAP_PARAM_SVR_SUITENAME_SERVER_H_)
#define CAP_PARAM_SVR_SUITENAME_SERVER_H_



#include <e32base.h>
#include <e32svr.h>
#include "f32file.h"
#include "e32test.h"
#include <test/TestExecuteStepBase.h>
#include <test/TestExecuteServerBase.h>
#include <e32property.h>
#include <etel.h>
#include <rsshared.h>
#include <c32root.h>

//The two ways to get thru
#define API_RetValue_ServerPanic			0
#define API_RetValue_NoCapError				0

//The only way to get rejected
#define API_RetValue_PermissionDenied		1
#define DEBUG_ONLY		1
#define DYNAMIC_IPC		2


const TUint KTestSecureServerMajorVersionNumber = 0;
const TUint KTestSecureServerMinorVersionNumber	= 1; 
const TUint KTestSecureServerBuildVersionNumber = 1;

enum TCapTestPanicCodes
{
ECapTestOutOfSequence	
};


class  CTestPARAM_SVR_SUITENAMEServer : public CTestServer
{
public:
	static CTestPARAM_SVR_SUITENAMEServer* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

};



class CCapabilityTestStep : public CTestStep, public RSessionBase
{
public:
 	virtual TVerdict doTestStepPreambleL( void );
	virtual TVerdict doTestStepPostambleL( void );
//	void ServerClose(RTelServer& aServer);
	CActiveScheduler*	testScheduler;

	TInt StartServer() ;
// from CStep.h
	#define SR_MESSAGE_TYPE_CHNGED		2
	#define SR_MESSAGE_ID_CHNGED		10
	#define SR_ServerName_CHNGED		_L("FLogger server")

	#define SR_CAPTEST_INVERSE			0	

	//The Server Name (eg: CommServer, EtelServer,FLogger server, etc)
	TBuf<100>	SR_ServerName;	
	TBuf<100>	iServer_Panic;

	//Following flags influence inverse tests
	TBool		iExpect_Rejection;
	
	TUint32  	iStepCap;

	
	//Is it Async or sync?
	TInt		SR_MESSAGE_TYPE;

	//It holds the IPC number
	TInt		SR_MESSAGE_ID;

	//Holds the cap mask for the message
	TInt		SR_MESSAGE_MASK;

	//We name the child thread appended by the IPC_Number it tests
	TBuf<100>	ChildThread_SR;		

	//The flag informs the main thread whether the connection to the server was established
	volatile TBool iSessionCreated;

	//To find out if an Async message was completed and if then with what result?
	TRequestStatus	RequestStatus_SR;

	//For an Sync message, nResult_SR get you the return value
	TInt	iResult_SR;	

	//Hold the retusn value from "CreateSession" API
	TInt	iResult_Server;

	TInt	iResult_C32;
		
	enum 	TDbgFns  {MarkHeapStart, MarkHeapEnd, CheckHeap, FailNext, ResetFailNext};
   	   	   
 	TInt	iOptions;
   	

	//The Child thread object
	RThread tChildThread;	
	
	//This is the Function called from "doTestStepL" by the test Suite,and it creates an 
	//child thread which internally calls the corresponding Exec_SendReceive_SERVERNAME fn.
	TVerdict MainThread();
	TVerdict GetVerdict(TInt aAPIretValue);
 	TVerdict GetVerdict(TExitType aExit,TInt aInitRetValue, TInt aApiRetValue);
 	TInt TestDebugHeap(TInt aDbgIPCNo);


	//This is for the scheduler test framework
	virtual enum TVerdict doTestStepL();

	// Stuff that derived classes need to implement
	virtual TVersion Version()=0;
	virtual TInt Exec_SendReceive()=0;
	 
	
};


#endif /* TS_PARAM_SVR_SUITENAME_SERVER_H_ */
