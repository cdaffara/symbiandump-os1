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

#include "Te_LoopbackTestServer.h"

#include "Te_LoopBackcrxmess.h"
#include "Te_LoopBackCNoPduRxMess.h"
#include "Te_LoopBackcsca.h"
#include "Te_LoopBackctxmess.h"
#include "Te_LoopBackcdatacall.h"
#include "Te_LoopBackcoddinits.h"
#include "Te_LoopBackcmessstor.h"
#include "Te_Loopbackcfailinits.h"
#include "Te_LoopBackcincall.h"
#include "Te_LoopBackcsmscancel.h"
#include "Te_LoopBackcshutdown.h"
#include "Te_LoopBackCSmsdelete.h"
#include "Te_LoopBackcssfax.h"
#include "Te_LoopBackcdatacallcancel.h"
#include "Te_Loopbackcfaxpremclose.h"
#include "Te_LoopBackctwofaxrx.h"
#include "Te_LoopBackcvoicecall.h"
#include "Te_LoopBackCDataCallCallBack.h"
#include "Te_LoopBackCDataCallRemoteHangupDial.h"
#include "Te_LoopBackCDataCallRemoteTerm.h"
#include "Te_LoopBackcphoneBook.h"
#include "Te_LoopBackCNetwork.h"
#include "Te_LoopBackCPhoneLine.h"
#include "Te_LoopBackCCalls.h"
#include "Te_LoopBackCNoCnmi.h"
#include "Te_LoopBackcnocmgf.h"
#include "Te_Loopbackcoomvoicecall.h"
#include "Te_LoopBackCDataCallDiffParam.h"
#include "Te_LoopBackCCbstParse.h"
#include "Te_LoopBackCUnsolicited.h"
#include "Te_LoopBackccgqreq_responses.h"

_LIT(KServerName,"Te_Loopback");

CLoopbackTestServer* CLoopbackTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CLoopbackTestServer * testServer = new (ELeave) CLoopbackTestServer();
	CleanupStack::PushL(testServer);

	testServer->ConstructL(KServerName);
	CleanupStack::Pop(testServer);
	return testServer;
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
   	TInt result = StartC32();
	if (result != KErrNone  &&  result != KErrAlreadyExists)
		{
		User::Leave(result);
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CLoopbackTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CLoopbackTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete testServer;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }

CTestStep* CLoopbackTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	TInt varDelay;  // Variable Delay for EWait script, for scoping purposes

	TRAPD(error,
	    if(aStepName == _L("TestDriveRxMess"))
	    	{
	    	testStep = CTestDriveRxMess::NewL(ERxMessScript) ;
			testStep->SetTestStepName(_L("TestDriveRxMess")) ;
			}
	    else if(aStepName == _L("TestDriveRxMessCmt"))
	    	{
	    	testStep = CTestDriveRxMessCmt::NewL(ERxMessCmtAndCmtiScript) ;
			testStep->SetTestStepName(_L("TestDriveRxMessCmt")) ;
	        }
	    else if(aStepName == _L("TestDriveRxMessT28"))
	    	{
	    	testStep = CTestDriveRxMess::NewL(ERxMessEricssonT28Script) ;
			testStep->SetTestStepName(_L("TestDriveRxMessT28")) ;
	        }
	    else if(aStepName == _L("TestDriveNotMess"))
	    	{
	    	testStep = CTestDriveNotMess::NewL(ERxMessScript) ;
			testStep->SetTestStepName(_L("TestDriveNotMess")) ;
	        }
	    else if(aStepName == _L("TestDriveNotMessCmt"))
	    	{
	    	testStep = CTestDriveNotMessCmt::NewL(ERxMessCmtScript) ;
			testStep->SetTestStepName(_L("TestDriveNotMessCmt")) ;
	        }
	    else if(aStepName == _L("TestDriveNoPduRxMess"))
	    	{
	    	testStep = CTestDriveNoPduRxMess::NewL(ENoPduRxMessScript) ;
			testStep->SetTestStepName(_L("TestDriveNoPduRxMess")) ;
	        }
	    else if(aStepName == _L("TestDriveScaSimple"))
	    	{
	    	testStep = CTestDriveSca::NewL(EScaSimpleScript) ;
			testStep->SetTestStepName(_L("TestDriveScaSimple")) ;
	        }
	    else if(aStepName == _L("TestDriveSca8210Style"))
	    	{
	    	testStep = CTestDriveSca::NewL(ESca8210StyleScript) ;
			testStep->SetTestStepName(_L("TestDriveSca8210Style")) ;
	        }
	    else if(aStepName == _L("TestDriveTxMess"))
	    	{
	    	testStep = CTestDriveTxMess::NewL(ETxMessScript) ;
			testStep->SetTestStepName(_L("TestDriveTxMess")) ;
	        }
	    else if(aStepName == _L("TestDriveTxNewStdMess"))
	    	{
	    	testStep = CTestDriveTxNewStdMess::NewL(ETxMessNewStdScript) ;
			testStep->SetTestStepName(_L("TestDriveTxNewStdMess")) ;
	        }
	    else if(aStepName == _L("TestDriveT28TxNewStdMess"))
	    	{
	    	testStep = CTestDriveTxNewStdMess::NewL(ETxMessT28NewStdScript) ;
			testStep->SetTestStepName(_L("TestDriveT28TxNewStdMess")) ;
	        }
	    else if(aStepName == _L("TestDriveTxWithScaComboNoprefixNodefNew"))
	    	{
	    	testStep = CTestDriveTxWithScaCombo::NewL(ETxMessScriptNoprefixNodefscaNew, EFalse, EFalse, ETrue) ;
			testStep->SetTestStepName(_L("TestDriveTxWithScaComboNoprefixNodefNew")) ;
	        }
	    else if(aStepName == _L("TestDriveTxWithScaComboPrefixNodeNew"))
	    	{
	    	testStep = CTestDriveTxWithScaCombo::NewL(ETxMessScriptPrefixNodefscaNew, ETrue, EFalse, ETrue) ;
			testStep->SetTestStepName(_L("TestDriveTxWithScaComboPrefixNodeNew")) ;
	        }
	    else if(aStepName == _L("TestDriveTxWithScaComboNoprefixDefNew"))
	    	{
	    	testStep = CTestDriveTxWithScaCombo::NewL(ETxMessScriptNoprefixDefscaNew, EFalse, ETrue, ETrue) ;
			testStep->SetTestStepName(_L("TestDriveTxWithScaComboNoprefixDefNew")) ;
	        }
	    else if(aStepName == _L("TestDriveTxWithScaComboPrefixDefNew"))
	    	{
	    	testStep = CTestDriveTxWithScaCombo::NewL(ETxMessScriptPrefixDefscaNew, ETrue, ETrue, ETrue) ;
			testStep->SetTestStepName(_L("TestDriveTxWithScaComboPrefixDefNew")) ;
	        }
	    else if(aStepName == _L("TestDriveTxWithScaComboNoprefixNodefscaOld"))
	    	{
	    	testStep = CTestDriveTxWithScaCombo::NewL(ETxMessScriptNoprefixNodefscaOld, EFalse, EFalse, EFalse) ;
			testStep->SetTestStepName(_L("TestDriveTxWithScaComboNoprefixNodefscaOld")) ;
	        }
	    else if(aStepName == _L("TestDriveTxWithScaComboPrefixNodefscaOld"))
	    	{
	    	testStep = CTestDriveTxWithScaCombo::NewL(ETxMessScriptPrefixNodefscaOld, ETrue, EFalse, EFalse) ;
			testStep->SetTestStepName(_L("TestDriveTxWithScaComboPrefixNodefscaOld")) ;
	        }
	    else if(aStepName == _L("TestDriveTxWithScaComboNoprefixDefscaOld"))
	    	{
	    	testStep = CTestDriveTxWithScaCombo::NewL(ETxMessScriptNoprefixDefscaOld, EFalse, ETrue, EFalse) ;
			testStep->SetTestStepName(_L("TestDriveTxWithScaComboNoprefixDefscaOld")) ;
	        }
	    else if(aStepName == _L("TestDriveTxWithScaComboPrefixDefscaOld"))
	    	{
	    	testStep = CTestDriveTxWithScaCombo::NewL(ETxMessScriptPrefixDefscaOld, ETrue, ETrue, EFalse) ;
			testStep->SetTestStepName(_L("TestDriveTxWithScaComboPrefixDefscaOld")) ;
	        }
	    else if(aStepName == _L("TestDriveTxRx"))
	    	{
	    	testStep = CTestDriveTxRx::NewL(ETxMessRxTxScriptA) ;
			testStep->SetTestStepName(_L("TestDriveTxRx")) ;
	        }
	    else if(aStepName == _L("TestDriveDataCall"))
	    	{
	    	testStep = CTestDriveDataCall::NewL(EDataCallScript) ;
			testStep->SetTestStepName(_L("TestDriveDataCall")) ;
	        }
	    else if(aStepName == _L("TestDriveDataCallErrorA"))
	    	{
	    	testStep = CTestDriveDataCall::NewL(EDataCallErrorAScript) ;
			testStep->SetTestStepName(_L("TestDriveDataCallErrorA")) ;
	        }
	    else if(aStepName == _L("TestDriveOddInitAScript"))
	    	{
	    	testStep = CTestDriveOddInit::NewL(EOddInitAScript) ;
			testStep->SetTestStepName(_L("TestDriveOddInitAScript")) ;
	        }
	    else if(aStepName == _L("TestDriveOddInitBScript"))
	    	{
	    	testStep = CTestDriveOddInit::NewL(EOddInitBScript) ;
			testStep->SetTestStepName(_L("TestDriveOddInitBScript")) ;
	        }
	    else if(aStepName == _L("TestDriveOddInitCScript"))
	    	{
	    	testStep = CTestDriveOddInit::NewL(EOddInitCScript) ;
			testStep->SetTestStepName(_L("TestDriveOddInitCScript")) ;
	        }
	    else if(aStepName == _L("TestDriveOddInitDScript"))
	    	{
	    	testStep = CTestDriveOddInit::NewL(EOddInitDScript) ;
			testStep->SetTestStepName(_L("TestDriveOddInitDScript")) ;
	        }
	    else if(aStepName == _L("TestDriveOddInitEScript"))
	    	{
	    	testStep = CTestDriveOddInit::NewL(EOddInitEScript) ;
			testStep->SetTestStepName(_L("TestDriveOddInitEScript")) ;
	        }
	    else if(aStepName == _L("TestDriveOddInitFScript"))
	    	{
	    	testStep = CTestDriveOddInit::NewL(EOddInitFScript) ;
			testStep->SetTestStepName(_L("TestDriveOddInitFScript")) ;
	        }
	    else if(aStepName == _L("TestDriveOddInitGScript"))
	    	{
	    	testStep = CTestDriveOddInit::NewL(EOddInitGScript) ;
			testStep->SetTestStepName(_L("TestDriveOddInitGScript")) ;
	        }
	    else if(aStepName == _L("TestDriveMessStorOldSmsStd"))
	    	{
	    	testStep = CTestDriveMessStor::NewL(EMessStorOldSmsStdScript) ;
			testStep->SetTestStepName(_L("TestDriveMessStorOldSmsStd")) ;
	        }
	    else if(aStepName == _L("TestDriveMessStorNewSmsStd"))
	    	{
	    	testStep = CTestDriveMessStor::NewL(EMessStorNewSmsStdScript) ;
			testStep->SetTestStepName(_L("TestDriveMessStorNewSmsStd")) ;
	        }
	    else if(aStepName == _L("TestDriveFailInit"))
	    	{
	    	testStep = CTestDriveFailInit::NewL(EFailInitAScript) ;
			testStep->SetTestStepName(_L("TestDriveFailInit")) ;
	        }
	    else if(aStepName == _L("TestDriveInCallAScript"))
	    	{
	    	testStep = CTestDriveInCall::NewL(EInCallScriptA) ;
			testStep->SetTestStepName(_L("TestDriveInCallAScript")) ;
	        }
	    else if(aStepName == _L("TestDriveInCallBScript"))
	    	{
	    	testStep = CTestDriveInCall::NewL(EInCallScriptB) ;
			testStep->SetTestStepName(_L("TestDriveInCallBScript")) ;
	        }
	    else if(aStepName == _L("TestDriveSmsCancelCancelScript"))
	    	{
	    	testStep = CTestDriveSmsCancel::NewL(ESmsCancelScript) ;
			testStep->SetTestStepName(_L("TestDriveSmsCancelCancelScript")) ;
	        }
	    else if(aStepName == _L("TestDriveSmsCancelCmtiScript"))
	    	{
	    	testStep = CTestDriveSmsCancel::NewL(ESmsCancelCmtiScript) ;
			testStep->SetTestStepName(_L("TestDriveSmsCancelCmtiScript")) ;
	        }
	    else if(aStepName == _L("TestDriveShutdown"))
	    	{
	    	testStep = CTestDriveShutdown::NewL(EShutdownScript) ;
			testStep->SetTestStepName(_L("TestDriveShutdown")) ;
	        }
	    else if(aStepName == _L("TestDriveShutdownA"))
	    	{
	    	testStep = CTestDriveShutdownA::NewL(EShutdownScriptA) ;
			testStep->SetTestStepName(_L("TestDriveShutdownA")) ;
	        }
	    else if(aStepName == _L("TestDriveSmsDelete"))
	    	{
	    	testStep = CTestDriveSmsDelete::NewL(ESmsStorageDeleteScript) ;
			testStep->SetTestStepName(_L("TestDriveSmsDelete")) ;
	        }
	    else if(aStepName == _L("TestDriveSsFax"))
	    	{
	    	testStep = CTestDriveSsFax::NewL(ESsFaxScriptA) ;
			testStep->SetTestStepName(_L("TestDriveSsFax")) ;
	        }
	    else if(aStepName == _L("TestDriveSSData"))
	    	{
	    	testStep = CTestDriveSSData::NewL(ESsDataScriptA) ;
			testStep->SetTestStepName(_L("TestDriveSSData")) ;
	        }
	    else if(aStepName == _L("TestDriveDataCallCancel"))
	    	{
	    	testStep = CTestDriveDataCallCancel::NewL(EDataCallCancelScript) ;
			testStep->SetTestStepName(_L("TestDriveDataCallCancel")) ;
	        }
	    else if(aStepName == _L("TestDrivePremClose"))
	    	{
	    	testStep = CTestDrivePremClose::NewL(EFaxPremCloseScriptA) ;
			testStep->SetTestStepName(_L("TestDrivePremClose")) ;
	        }
	    else if(aStepName == _L("TestDriveTwoFaxRx"))
	    	{
	    	testStep = CTestDriveTwoFaxRx::NewL(ETwoFaxRxScriptA) ;
			testStep->SetTestStepName(_L("TestDriveTwoFaxRx")) ;
	        }
	    else if(aStepName == _L("TestDriveVoiceCall"))
	    	{
	    	testStep = CTestDriveVoiceCall::NewL(EVoiceCallScriptA) ;
			testStep->SetTestStepName(_L("TestDriveVoiceCall")) ;
	        }
	    else if(aStepName == _L("TestDriveDataCallRemoteTerm"))
	    	{
	    	testStep = CTestDriveDataCallRemoteTerm::NewL(EDataCallRemoteTermScript) ;
			testStep->SetTestStepName(_L("TestDriveDataCallRemoteTerm")) ;
	        }
	    else if(aStepName == _L("TestDriveDataCallCallBack"))
	    	{
			varDelay = KCallBackDefVarDelay;
	    	testStep = CTestDriveDataCallCallBack::NewL(EDataCallCallBackScript, varDelay) ;
			testStep->SetTestStepName(_L("TestDriveDataCallCallBack")) ;
	        }
	    else if(aStepName == _L("TestDriveRemoteHangupDial"))
	    	{
			varDelay = KHangupDialDefVarDelay;
	    	testStep = CTestDriveRemoteHangupDial::NewL(EDataCallRemoteHangDialScript, varDelay) ;
			testStep->SetTestStepName(_L("TestDriveRemoteHangupDial")) ;
	        }
	    else if(aStepName == _L("TestDrivePhoneBook"))
	    	{
	    	testStep = CTestDrivePhoneBook::NewL(EPhoneBookScript) ;
			testStep->SetTestStepName(_L("TestDrivePhoneBook")) ;
	        }
	    else if(aStepName == _L("TestDriveNetworkRegistration"))
	    	{
	    	testStep = CTestDriveNetworkRegistration::NewL(ENetworkRegistrationScript) ;
			testStep->SetTestStepName(_L("TestDriveNetworkRegistration")) ;
	        }
	    else if(aStepName == _L("TestDrivePhoneLine"))
	    	{
	    	testStep = CTestDrivePhoneLine::NewL(EPhoneLineScript) ;
			testStep->SetTestStepName(_L("TestDrivePhoneLine")) ;
	        }
	    else if(aStepName == _L("TestDriveAllCalls"))
	    	{
	    	testStep = CTestDriveAllCalls::NewL(EAllCallsScript) ;
			testStep->SetTestStepName(_L("TestDriveAllCalls")) ;
	        }
	    else if(aStepName == _L("TestDriveNoCnmi"))
	    	{
	    	testStep = CTestDriveNoCnmi::NewL(ENoCnmiScript) ;
			testStep->SetTestStepName(_L("TestDriveNoCnmi")) ;
	        }
	    else if(aStepName == _L("TestDriveNoCmgf"))
	    	{
	    	testStep = CTestDriveNoCmgf::NewL(ENoCmgfScript) ;
			testStep->SetTestStepName(_L("TestDriveNoCmgf")) ;
	        }
	    else if(aStepName == _L("TestDriveDataCallDiffParam"))
	    	{
	    	testStep = CTestDriveDataCallDiffParam::NewL(ECallDiffParamScript) ;
			testStep->SetTestStepName(_L("TestDriveDataCallDiffParam")) ;
	        }
	    else if(aStepName == _L("TestDriveCbstParse"))
	    	{
	    	testStep = CTestDriveCbstParse::NewL(ECbstParseScript) ;
			testStep->SetTestStepName(_L("TestDriveCbstParse")) ;
	        }
	    else if(aStepName == _L("TestDriveCGQREQResponses"))
	    	{
	    	testStep = CTestDriveCGQREQResponses::NewL(ECGQREQResponsesScript) ;
			testStep->SetTestStepName(_L("TestDriveCGQREQResponses")) ;
	        }
	    else if(aStepName == _L("TestDriveUnsolicited"))
	    	{
	    	testStep = CTestDriveUnsolicited::NewL(EUnsolicitedScript) ;
			testStep->SetTestStepName(_L("TestDriveUnsolicited")) ;
	        }
	    else if(aStepName == _L("TestDriveOOMVoiceCall"))
	    	{
	    	testStep = CTestDriveOOMVoiceCall::NewL(EOOMVoiceCall) ;
			testStep->SetTestStepName(_L("TestDriveOOMVoiceCall")) ;
	        }
		) // End of TRAP
	if (error==KErrNone)
		return testStep;
	else
		return NULL;
	}

GLDEF_C void StartScriptL(TScriptList* aScriptNum, const TInt aVarDelay,TRequestStatus** aRestartReqStat)
/**
 *	This method demultiplexes the script number enumeration (TScriptList), creates the
 *  appropriate instance of a script (CATScriptEng), then launches the script processing.  Any
 *  test in the TScriptList enumeration must be represented in this method.  An unsupported
 *  script number will result in this method leaving with KErrNotSupported.  Also,
 *  if any of the tests NewL functions fail, the same action will occur.  This method also
 *  starts the active scheduler which is created and installed in the Responder thread (which
 *  calls this method).
 *
 * @param		aScriptNum is a pointer to one of the values in the TScriptList enumeration.
 *				This pointer is initially passed during the thread creation.
 * @param		aVarDelay is an integer value to be used for the variable delay in an EWait
 *              script with an iParam value of -1.
 *				This pointer is initially passed during the thread creation.
 * @leave		This method can leave via the various test specific NewL functions.  Additionally,
 *				if an invalid test is specified or the return value after the active scheduler
 *				is started is not KErrNone.
 *
 */
 // Todo: Investigate test classes to see if they can be combined into a superclass for
 //       savings.  That is, many of test classes use the same NewL functions to create the
 //       CATScriptEng class to use when starting the script.
 	{
	CATScriptEng* script=NULL;
	TInt ReturnValue=KErrNone;
	switch(*aScriptNum)
		{
	case ERxMessScript:
		script=CTestRxMess::NewL(KRxMessScript);
		break;

	case ENoPduRxMessScript:
		script=CTestNoPduRxMess::NewL(KNoPduRxMessScript);
		break;

	case ERxMessCmtScript:
		script=CTestRxMess::NewL(KRxMessCmtScript);
		break;

	case ERxMessCmtAndCmtiScript:
		script=CTestRxMess::NewL(KRxMessCmtAndCmtiScript);
		break;

	case ERxMessEricssonT28Script:
		script=CTestRxMess::NewL(KRxMessEricssonT28Script);
		break;

	case EScaSimpleScript:
		script=CTestTxMess::NewL(KScaSimpleScript);
		break;

	case ESca8210StyleScript:
		script=CTestTxMess::NewL(KSca8210StyleScript);
		break;

	case ETxMessScript:
		script=CTestTxMess::NewL(KTxMessScript);
		break;

	case ETxMessNewStdScript:
		script=CTestTxMess::NewL(KTxMessNewStdScript);
		break;

	case ETxMessT28NewStdScript:
		script=CTestTxMess::NewL(KTxMessT28NewStdScript);
		break;

	case ETxMessScriptNoprefixNodefscaNew:
		script=CTestTxMess::NewL(KTxMessScriptNoprefixNodefscaNew);
		break;

	case ETxMessScriptPrefixNodefscaNew:
		script=CTestTxMess::NewL(KTxMessScriptPrefixNodefscaNew);
		break;

	case ETxMessScriptNoprefixDefscaNew:
		script=CTestTxMess::NewL(KTxMessScriptNoprefixDefscaNew);
		break;

	case ETxMessScriptPrefixDefscaNew:
		script=CTestTxMess::NewL(KTxMessScriptPrefixDefscaNew);
		break;

	case ETxMessScriptNoprefixNodefscaOld:
		script=CTestTxMess::NewL(KTxMessScriptNoprefixNodefscaOld);
		break;

	case ETxMessScriptPrefixNodefscaOld:
		script=CTestTxMess::NewL(KTxMessScriptPrefixNodefscaOld);
		break;

	case ETxMessScriptNoprefixDefscaOld:
		script=CTestTxMess::NewL(KTxMessScriptNoprefixDefscaOld);
		break;

	case ETxMessScriptPrefixDefscaOld:
		script=CTestTxMess::NewL(KTxMessScriptPrefixDefscaOld);
		break;

	case EDataCallScript:
		script=CTestDataCall::NewL(KDataCallScript);
		break;

	case EDataCallErrorAScript:
		script=CTestDataCall::NewL(KDataCallErrorAScript);
		break;

	case EOddInitAScript:
		script=CTestOddInit::NewL(KOddInitAScript);
		break;

	case EOddInitBScript:
		script=CTestOddInit::NewL(KOddInitBScript);
		break;

	case EOddInitCScript:
		script=CTestOddInit::NewL(KOddInitCScript);
		break;

	case EOddInitDScript:
		script=CTestOddInit::NewL(KOddInitDScript);
		break;

	case EOddInitEScript:
		script=CTestOddInit::NewL(KOddInitEScript);
		break;

	case EOddInitFScript:
		script=CTestOddInit::NewL(KOddInitFScript);
		break;

	case EOddInitGScript:
		script=CTestOddInit::NewL(KOddInitGScript);
		break;

	case EOddInitHScript:
		script=CTestOddInit::NewL(KOddInitHScript);
		break;
	
	case EOddInitIScript:
		script=CTestOddInit::NewL(KOddInitIScript);
		break;

	case EMessStorOldSmsStdScript:
		script=CTestMessStor::NewL(KMessStorOldSmsStdScript);
		break;

	case EMessStorNewSmsStdScript:
		script=CTestMessStor::NewL(KMessStorNewSmsStdScript);
		break;

	case ETxMessRxTxScriptA:
		script=CTestTxMess::NewL(KTxMessRxTxScriptA);
		break;

	case EFailInitAScript:
		script=CTestFailInit::NewL(KFailInitAScript);
		break;

	case EInCallScriptA:
		script=CTestInCall::NewL(KInCallScriptA);
		break;

	case EInCallScriptB:
		script=CTestInCall::NewL(KInCallScriptB);
		break;

	case ESmsCancelScript:
		script=CTestSmsCancel::NewL(KSmsCancelScript);
		break;

	case ESmsCancelCmtiScript:
		script=CTestSmsCancel::NewL(KSmsCancelCmtiScript);
		break;

	case EShutdownScript:
		script=CTestShutdown::NewL(KShutdownScript);
		break;

	case EShutdownScriptA:
		script=CTestShutdown::NewL(KShutdownScriptA);
		break;

	case ESmsStorageDeleteScript:
		script=CTestSmsDelMess::NewL(KDeleteSmsScript);
		break;

	case ESsFaxScriptA:
		script=CTestSsFax::NewL(KSsFaxScriptA);
		break;

	case ESsDataScriptA:
		script=CTestSSData::NewL(KSsDataScriptA);
		break;

	case EDataCallCancelScript:
		script=CTestDataCallCancel::NewL(KDataCallCancelScript);
		break;

	case EFaxPremCloseScriptA:
		script=CTestPremClose::NewL(KFaxPremCloseScriptA);
		break;

	case EFaxPremCloseScriptB:
		script=CTestPremClose::NewL(KFaxPremCloseScriptB);
		break;

	case ETwoFaxRxScriptA:
		script=CTestTwoFaxRx::NewL(KTwoFaxRxScriptA);
		break;

	case EVoiceCallScriptA:
		script=CTestVoiceCall::NewL(KVoiceCallScriptA);
		break;

	case EDataCallRemoteTermScript:
		script=CTestDataCallRemoteTerm::NewL(KDataCallRemoteTermScript);
		break;

	case EDataCallCallBackScript:
		if(aVarDelay<=1)
			script=CTestDataCallCallBack::NewL(KDataCallCallBackScriptOneSecond, aVarDelay);
		else
			script=CTestDataCallCallBack::NewL(KDataCallCallBackScriptMoreThanOneSecond, aVarDelay);
		break;

	case EDataCallRemoteHangDialScript:
		script=CTestRemoteHangupDial::NewL(KDataCallRemoteHangDialScript, aVarDelay);
		break;

	case EPhoneBookScript:
		script=CTestPhoneBook::NewL(KPhoneBookScript);
		break;

	case ENetworkRegistrationScript:
		script=CTestNetworkRegistration::NewL(KNetworkRegistrationScript);
		break;

	case EPhoneLineScript:
		script=CTestPhoneLine::NewL(KPhoneLineScript);
		break;

	case EAllCallsScript:
		script=CTestAllCalls::NewL(KAllCallsScript);
		break;

	case ENoCnmiScript:
		script=CTestNoCnmi::NewL(KNoCnmiScript);
		break;

	case ENoCmgfScript:
		script=CTestNoCmgf::NewL(KNoCMGFScript);
		break;

	case EOOMVoiceCall:
		script=CTestOOMVoiceCall::NewL(KOOMVoiceCallScriptA);
		break;
	
	case ECGQREQResponsesScript:
		script=CTestCGQREQResponses::NewL(KCGQREQResponsesScript);
		break;

	case ECallDiffParamScript:
		script=CTestDataCallDiffParam::NewL(KCDataCallDiffParamScript);
		break;

	case ECbstParseScript:
		script=CTestCbstParse::NewL(KCCbstParseScript);
		break;

	case EUnsolicitedScript:
		script=CTestUnsolicited::NewL(KCUnsolicitedScript);
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}
	CleanupStack::PushL(script);

	if(script==NULL)
		User::Leave(KErrNotSupported);

	script->StartRestart(aRestartReqStat);
	script->Start();

	CActiveScheduler::Start();
	ReturnValue = script->iReturnValue ; //get a result
	if(ReturnValue!=KErrNone)
		User::Leave(ReturnValue);
	CleanupStack::PopAndDestroy(script);
	}

GLDEF_C TInt ResponderThread(TAny* aThreadData)
//
// Main Function
//
/**
 * This method is the entry point for the thread created when a test is run.  This thread
 * is created by CTestBase::StartEmulator.  This thread communicates with it's "parent" thread
 * via the active scheduler.  After creating and installing an active scheduler entry for
 * the "Responder Thread", a rendezvous is mase for thread synchronization purposes. 
 * After thread synchronization, the StartScriptL method is called to actually process
 * the script.
 *
 * @param	aThreadData is an any pointer used to pass in a pointer to the structure containing
 *			the enum indicating the actual script to be run by StartScriptL and the variable
 *			delay value to be used as the timeout by the script processing when the iParam value
 *			is set to -1 in a TScript script whose iCommand is an EWait.
 *
 * @return	The return value is determined by the StartScriptL function and the test script
 *			function itself.  It represents the value of the global ReturnValue at the end
 *			of the StartScriptL function.
 */
	{

	TThreadData  *pThreadData; // typed pointer to input parameter data
	TThreadData   threadData;  // this Thread's storage for the script indicator and variable delay

	// because "aThreadData" points to an automatic variable in another thread, this data
	// ...must be copied into thread specific storage before the signal is given back from
	// ...this function that the Responder Thread was created successfully.
	pThreadData = (TThreadData *)aThreadData;
	threadData.iScriptList    = pThreadData->iScriptList;
	threadData.iVariableDelay = pThreadData->iVariableDelay;
	threadData.iRestartReqStat = pThreadData->iRestartReqStat;

	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		User::Panic(_L("GsmTsy Loopback Tester Fault"),KErrGeneral);

	CActiveScheduler* activeScheduler=new CActiveScheduler();
	if(activeScheduler==NULL)
		return KErrNoMemory;
	CActiveScheduler::Install(activeScheduler);
	
	// Complete all Rendezvous' with the current thread.
	RThread::Rendezvous(KErrNone);
	
	TRAPD(r,StartScriptL(&(threadData.iScriptList), threadData.iVariableDelay,threadData.iRestartReqStat));

	LOGDESTROY();

	delete activeScheduler;
	delete cleanup;
	return r;
	}
