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
// This file contains the base test class CTestBase which is derived from CTestStep.
// It also contains the script list enumeration (TScriptList).  This file should be
// included by all test specific include files.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TE_LOOPBACK_TESTSTEP_BASE__
#define __TE_LOOPBACK_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <e32test.h>
#include <etelmm.h>   

#define PHONE_NAME	_L("GsmPhone1")
#define MODULE_NAME	_L("MM")

//Test Checking Macros
#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (code != expected ) INFO_PRINTF1(msg);\
	}

#define CHECKPOINT_EXPR(boolexpr,msg) {\
	TEST((boolexpr));\
	if (EFalse == (boolexpr)) INFO_PRINTF1(msg);\
	}


// TScriptList is a list of all the test scripts available to the regression test harness.
// This list must be modified to add another test script.  These are mapped to script names
// by a switch statement in the StartScriptL function (StartThread.cpp)
enum TScriptList
	{
	ERxMessScript,						//< Simple SMS Receive and SMS CMTI Notification Tests  (see Crxmess.h) 
	ENoPduRxMessScript,					//< Modem claims No SMS PDU Capability Test  (see CNoPduRxMess.h)
	ERxMessCmtScript,					//< CMT SMS Rx and SMS CMT Notification and Rx Tests  (see Crxmess.h)
	ERxMessCmtAndCmtiScript,			//< CMT and CMTI SMS Rx Tests  (see Crxmess.h)
	ERxMessEricssonT28Script,			//< CMTI SMS Rx Test emulating an Ericsson T28  (see Crxmess.h)
	EScaSimpleScript,					//< Simple SCA retrieval and setting test  (see CSca.h)
	ESca8210StyleScript,				//< 8210-style SCA retrieval and setting test  (see CSca.h)
	ETxMessScript,						//< Simple SMS Tx Test (see Ctxmess.h)
	ETxMessNewStdScript,				//< New Standard SMS Tx Test (see Ctxmess.h)
	ETxMessT28NewStdScript,				//< New Standard SMS Tx Test emulating an Ericsson T28  (see Ctxmess.h)
	ETxMessScriptNoprefixNodefscaNew,	//< Tx Test, No PDU Prefix, No Default SCA Set, New Standard  (see Ctxmess.h)
	ETxMessScriptPrefixNodefscaNew,		//< Tx Test, PDU Prefix, No Default SCA Set, New Standard  (see Ctxmess.h)
	ETxMessScriptNoprefixDefscaNew,		//< Tx Test, No PDU Prefix, Default SCA Set, New Standard  (see Ctxmess.h)
	ETxMessScriptPrefixDefscaNew,		//< Tx Test, PDU Prefix, Default SCA Set, New Standard  (see Ctxmess.h)
	ETxMessScriptNoprefixNodefscaOld,	//< Tx Test, No PDU Prefix, No Default SCA Set, Old Standard  (see Ctxmess.h)
	ETxMessScriptPrefixNodefscaOld,		//< Tx Test, PDU Prefix, No Default SCA Set, Old Standard  (see Ctxmess.h)
	ETxMessScriptNoprefixDefscaOld,		//< Tx Test, No PDU Prefix, Default SCA Set, Old Standard  (see Ctxmess.h)
	ETxMessScriptPrefixDefscaOld,		//< Tx Test, PDU Prefix, Default SCA Set, Old Standard  (see file Ctxmess.h)
	ETxMessRxTxScriptA,					//< Test a +CMS ERROR 321 from a Read - observed on Ericsson 888 (see Ctxmess.h) 
	EDataCallScript,					//< Dial a Simple Data Call (see CDataCall.h)
	EDataCallErrorAScript,				//< Dial a Simple Data Call with Modem ERRORing some command (see CDataCall.h)
	EOddInitAScript,					//< Test of an odd initialization sequence (see COddInits.h) 
	EOddInitBScript,					//< Test of an odd initialization sequence (see COddInits.h)
	EOddInitCScript,					//< Test of an odd initialization sequence (see COddInits.h)
	EOddInitDScript,					//< Test of an odd initialization sequence (see COddInits.h)
	EOddInitEScript,					//< Test of an odd initialization sequence (see COddInits.h)
	EOddInitFScript,					//< Test of an odd initialization sequence (see COddInits.h)
	EOddInitGScript,					//< Test of an odd initialization sequence (see COddInits.h)
	EOddInitHScript,					//< Test of an odd initialization sequence (see COddInits.h)
	EOddInitIScript,					//< Test of an odd initialization sequence (see COddInits.h)
	EMessStorOldSmsStdScript,			//< Test Message Storage Functions with Old Standard PDUs (see CMessStor.h)
	EMessStorNewSmsStdScript,			//< Test Message Storage Functions with New Standard PDUs (see CMessStor.h)
	EFailInitAScript,					//< Test Initialization Failure Scenarios (see CFailInits.h)
	EInCallScriptA,						//< Test Incoming Call Scenarios (see CInCall.h)
	EInCallScriptB,						//< Test Incoming Call Scenarios with a Nokia (see CInCall.h)
	ESmsCancelScript,					//< Test SMS Cancel CMT Scenarios  (see CSmsCancel.h)
	ESmsCancelCmtiScript,				//< Test SMS Cancel CMTI Scenarios (see CSmsCancel.h)
	EShutdownScript,					//< Test Shutdown Scenarios (see CShutdown.h)
	EShutdownScriptA,					//< Test Shutdown Scenario A (see CShutdown.h)
	ESmsStorageDeleteScript,			//< Simple SMS Storage Delete Test (see CSmsDelete.h) 
	ESsFaxScriptA,						//< Test Simultaneous Fax Rx and Signal Strength Retrieval (see CSsFax.h)
	ESsDataScriptA,						//< Test Simultaneous Data Call and Signal Strength Retrieval (see CSsFax.h)
	EDataCallCancelScript,				//< Test Data Call Cancel Scenarios (see CDataCallCancel.h)
	EFaxPremCloseScriptA,				//< Test Fax Premature Close Scenarios (see CFaxPremClose.h)
	EFaxPremCloseScriptB,				//< Test Fax Premature Close Scenarios Part B (see CFaxPremClose.h)
	ETwoFaxRxScriptA,					//< Test Two Fax Reception Scenarios (see CTwoFaxRx.h)
	EVoiceCallScriptA,					//< Test Voice Call Scenarios (see CVoiceCall.h)
	EDataCallRemoteTermScript,			//< Test Data Call Set-up, Data Transfer and Remote Termination (see CDataCallRemoteTerm.H)
	EDataCallCallBackScript,			//< Test Data Call Dial-up Networking Call-back (see CDataCallCallBack.H)
	EDataCallRemoteHangDialScript,		//< Test Data Call Answer and Remote Hang-up Closely Followed by a Dial (see CDataCallRemoteHangupDial.H)
	EPhoneBookScript,
	ENetworkRegistrationScript,
	EPhoneLineScript, 
	EAllCallsScript,
	ENoCnmiScript,
	ENoCmgfScript,
	EOOMVoiceCall,
        ECallDiffParamScript,
	ECbstParseScript,
	ECGQREQResponsesScript,
	EUnsolicitedScript
	};

// TThreadData is a stucture containing the TScriptList value and the Call Delay value 
// to be used to pass information to the Responder Thread to identify the scripts and the 
// EWait's varialble delay value (i.e., when required) to be used for the test in the regression test harness.
typedef struct threadData_s
	{
	TScriptList			iScriptList;
	TInt				iVariableDelay;
	TRequestStatus**	iRestartReqStat;
	} TThreadData;

/**
 * This is the base test class, derived from CTestStep, that is the super-class of all test 
 * specific test classes.  This class "glues" the user interface (from RunTest), the ETel API
 * (from DriveEtelApi), and the Modem Emulation (from StartEmulator) together.
 */
class CTestBase : public CTestStep
	{
public:
	CTestBase();
	//> left for backward compatibility
	void ConstructL(){}; 
	~CTestBase();

	// Called by the User Interface executable to invoke the selected test.
	virtual TInt RunTestL()=0;
 	virtual TVerdict doTestStepPreambleL( void );
	virtual TVerdict doTestStepPostambleL( void );
	virtual TVerdict doTestStepL( void ) ; 
	void RestartScript();
	void TerminateScript();
protected:
	TInt StartEmulatorL();

	// Called by StartEmulator after spawning modem emulator to drive the ETel API in specific test.
	virtual TInt DriveETelApiL()=0;
	TScriptList  iCurrentScript;	//< Identifies to emulator which test is being run 
	TInt         iVariableDelay;	//< Variable delay value to be used as the timeout by the 
									//< ...script processing when the iParam value is set -1 in 
	RTelServer iServer;
	RTelServer iServer2;
	RMobilePhone iPhone;		
	RMobilePhone iPhone2;
									//< ...a TScript script whose iCommand is an EWait.
	TRequestStatus* iRestartReqStat;	//< The Request Status of the script restart signal
private:
	CActiveScheduler* iTestScheduler;
	RThread iScriptThread;
	TProcessPriority  iOrgProcessPriority;
	TThreadPriority  iOrgThreadPriority;
	};

#endif
