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
// Te_LoopbackCDataCallRemoteHangupDial.CPP
// This file initiates the "Data Calls with diff params(speed)
// The client (application) side must start the emulator and drive the ETel API.
// The emulator side runs the designated script.
// 
//

/**
 @file
 @note  There are mulitple classes implemented in this file.
 @note  These classes are CTestDriveDataCallDiffParam and CTestDataCallDiffParam.
*/


#include <e32test.h>
#include "Te_LoopBackSLOGGER.H"
#include "Te_LoopBackCDataCallDiffParam.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

_LIT(KMmPhoneName,"GsmPhone1");
_LIT(KDataLineName,"Data");
_LIT(KDialString,"+1234");

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveDataCallDiffParam* CTestDriveDataCallDiffParam::NewL(const TScriptList aScriptListEntry,
															 TInt aVarDelay)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestDriveDataCallDiffParam.  The ConstructL for
 * for CTestDriveDataCallDiffParam is inherited from and implemented in CTestBase. The
 * ConstructL uses the CTestDriveDataCallDiffParam object to load ond open the two iPhone
 * objects, one from each server.
 *
 * @param aScriptListEntry: enum indicating which script to use for this test.
 * @param aVarDelay: integer indicating variable delay value, in seconds, to be used with an EWait script
 * @leave  Leaves if a failure occurs during connect or open of the iPhone by ConstructL.
 * @return pointer to the instance of CTestDriveDataCallDiffParam.
 */
	{
	CTestDriveDataCallDiffParam* aA=new(ELeave) CTestDriveDataCallDiffParam(aScriptListEntry, aVarDelay);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}


CTestDriveDataCallDiffParam::CTestDriveDataCallDiffParam(const TScriptList aScriptListEntry,
													   TInt aVarDelay
													  ) : iScriptListEntry(aScriptListEntry),
													      iVarDelay(aVarDelay)
/**
 * This method is the constructor for CTestDriveDataCallDiffParam.
 *
 * @param aScriptListEntry: enum indicating which script to use for this test.
 * @param aVarDelay: integer indicating variable delay value, in seconds, to be used with an EWait script
 * @note Initializes private data "aScriptListEntry" to received parameter.
 * @note Initializes private data "aVarDelay" to received parameter.
 */
	{}

TInt CTestDriveDataCallDiffParam::RunTestL()
/**
 * This method is invoked to start a "Data Call answer and remote hang-up closely followed by
 * a dial" test.  This method sets the CTestBase current script to run and the CTestBase variable
 * delay value to use for an EWait
 * script and then starts the modem emulator side of the test.
 *
 * @return KErrNone when no error exists.
 * @return KErrAlreadyExists is returned if modem emulator exists and retry limit expires.
 * @return Variable depending on return value from test's DriveETelApiL method and thread monitoring.
 */
	{
	iCurrentScript=iScriptListEntry;
	iVariableDelay=iVarDelay;
	return StartEmulatorL();
	}

TInt CTestDriveDataCallDiffParam::DriveETelApiL()
/**
 * This method contains the real meat of the Client-side "Data Call answer and remote
 * hang-up closely followed by a dial" test code.  This method sets up to answer a
 * call and receive a data transfer.  The call is then terminated from the remote end,
 * and then must quickly respond by dialing the remote end back.  Data is then
 * transfered and the call is terminated again from the remote end.
 *
 * @return KErrNone.
 */
	{
	RMobilePhone iPhone3;
	INFO_PRINTF1(_L("Opening Mobile Phone\n"));
	TESTL(iPhone3.Open(iServer,KMmPhoneName)==KErrNone);
	INFO_PRINTF1(_L("Initialising Mobile Phone\n"));
	
	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone3.InitialiseMM(reqStatus, tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	DialDiffDataServiceL(RMobileCall::EServiceUnspecified) ; //44
	DialDiffDataServiceL(RMobileCall::EServiceDataCircuitAsync) ; //45
	DialDiffDataServiceL(RMobileCall::EServiceDataCircuitAsyncRdi) ; //46
	DialDiffDataServiceL(RMobileCall::EServicePADAsyncUDI) ; //48
	DialDiffDataServiceL(RMobileCall::EServicePADAsyncRDI) ; //49
	DialDiffDataServiceL(RMobileCall::EServicePacketAccessSyncUDI) ; //50
	DialDiffDataServiceL(RMobileCall::EServicePacketAccessSyncRDI) ;  //51
	DialDiffDataServiceL(RMobileCall::EServiceDataCircuitSyncRdi) ; //47

	DialDiffSpeedL(RMobileCall::ESpeedUnspecified) ; //1
	DialDiffSpeedL(RMobileCall::ESpeedAutobauding) ;	 //2

	DialDiffSpeedL(RMobileCall::ESpeed2400, RMobileCall::EProtocolV22bis) ; //3
	DialDiffSpeedL(RMobileCall::ESpeed2400, RMobileCall::EProtocolV110) ; //4
	DialDiffSpeedL(RMobileCall::ESpeed2400, RMobileCall::EProtocolV120) ; //5
	DialDiffSpeedL(RMobileCall::ESpeed2400, RMobileCall::EProtocolX31FlagStuffing) ; //6

	DialDiffSpeedL(RMobileCall::ESpeed4800, RMobileCall::EProtocolV32) ;	//7
	DialDiffSpeedL(RMobileCall::ESpeed4800, RMobileCall::EProtocolV110) ;//8
	DialDiffSpeedL(RMobileCall::ESpeed4800, RMobileCall::EProtocolV120) ;//9
	DialDiffSpeedL(RMobileCall::ESpeed4800, RMobileCall::EProtocolX31FlagStuffing) ; //10

	DialDiffSpeedL(RMobileCall::ESpeed9600, RMobileCall::EProtocolV32) ;	//11
	DialDiffSpeedL(RMobileCall::ESpeed9600, RMobileCall::EProtocolV34) ;	//12
	DialDiffSpeedL(RMobileCall::ESpeed9600, RMobileCall::EProtocolV110) ;//13
	DialDiffSpeedL(RMobileCall::ESpeed9600, RMobileCall::EProtocolV120) ;//14
	DialDiffSpeedL(RMobileCall::ESpeed9600, RMobileCall::EProtocolX31FlagStuffing) ; //15

	DialDiffSpeedL(RMobileCall::ESpeed14400, RMobileCall::EProtocolV34) ; //16
	DialDiffSpeedL(RMobileCall::ESpeed14400, RMobileCall::EProtocolV110) ; //17
	DialDiffSpeedL(RMobileCall::ESpeed14400, RMobileCall::EProtocolV120) ; //18
	DialDiffSpeedL(RMobileCall::ESpeed14400, RMobileCall::EProtocolX31FlagStuffing) ; //19

	DialDiffSpeedL(RMobileCall::ESpeed19200, RMobileCall::EProtocolV34) ; //20
	DialDiffSpeedL(RMobileCall::ESpeed19200, RMobileCall::EProtocolV110) ;//21
	DialDiffSpeedL(RMobileCall::ESpeed19200, RMobileCall::EProtocolV120) ;//22
	DialDiffSpeedL(RMobileCall::ESpeed19200, RMobileCall::EProtocolX31FlagStuffing) ; //23

	DialDiffSpeedL(RMobileCall::ESpeed28800, RMobileCall::EProtocolV34) ; //24
	DialDiffSpeedL(RMobileCall::ESpeed28800, RMobileCall::EProtocolV110) ; //25
	DialDiffSpeedL(RMobileCall::ESpeed28800, RMobileCall::EProtocolV120) ; //26
	DialDiffSpeedL(RMobileCall::ESpeed28800, RMobileCall::EProtocolX31FlagStuffing) ; //27

	DialDiffSpeedL(RMobileCall::ESpeed38400, RMobileCall::EProtocolV110) ;//28
	DialDiffSpeedL(RMobileCall::ESpeed38400, RMobileCall::EProtocolV120) ;//29
	DialDiffSpeedL(RMobileCall::ESpeed38400, RMobileCall::EProtocolX31FlagStuffing) ; //30

	DialDiffSpeedL(RMobileCall::ESpeed48000, RMobileCall::EProtocolV110) ;//31
	DialDiffSpeedL(RMobileCall::ESpeed48000, RMobileCall::EProtocolV120) ;//32
	DialDiffSpeedL(RMobileCall::ESpeed48000, RMobileCall::EProtocolX31FlagStuffing) ; //33

	DialDiffSpeedL(RMobileCall::ESpeed56000, RMobileCall::EProtocolV110) ;//34
	DialDiffSpeedL(RMobileCall::ESpeed56000, RMobileCall::EProtocolV120) ;//35
	DialDiffSpeedL(RMobileCall::ESpeed56000, RMobileCall::EProtocolX31FlagStuffing) ; //36
	DialDiffSpeedL(RMobileCall::ESpeed56000, RMobileCall::EProtocolBitTransparent) ;//37

	DialDiffSpeedL(RMobileCall::ESpeed64000, RMobileCall::EProtocolBitTransparent) ;//38

	DialDiffQoSL(RMobileCall::EQoSUnspecified) ; //39
	DialDiffQoSL(RMobileCall::EQoSTransparent) ;	 //40
	DialDiffQoSL(RMobileCall::EQoSNonTransparent) ; //41
	DialDiffQoSL(RMobileCall::EQosTransparentPreferred) ; //42
	DialDiffQoSL(RMobileCall::EQosNonTransparentPreferred) ;	//43

	iPhone3.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestDataCallDiffParam* CTestDataCallDiffParam::NewL(const TScript* aScript)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestDataCallDiffParam.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @leave Leaves if out of memory when attempting to create.
 * @return pointer to the instance of CTestDataCallDiffParam.
 */
	{
	CTestDataCallDiffParam* aA=new(ELeave) CTestDataCallDiffParam(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDataCallDiffParam* CTestDataCallDiffParam::NewL(const TScript* aScript, const TInt aVarDelay)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestDataCallDiffParam.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @param aVarDelay: time delay
 * @leave Leaves if out of memory when attempting to create.
 * @return pointer to the instance of CTestDataCallDiffParam.
 */
	{
	CTestDataCallDiffParam* aA=new(ELeave) CTestDataCallDiffParam(aScript, aVarDelay);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDataCallDiffParam::CTestDataCallDiffParam(const TScript* aScript) : iScript(aScript)
/**
 * This method is the constructor for CTestDataCallDiffParam.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @note Initializes private data "aScript" to received parameter.
 */
	{}

CTestDataCallDiffParam::CTestDataCallDiffParam(const TScript* aScript, const TInt aVarDelay) : CATScriptEng(aVarDelay), iScript(aScript)
/**
 * This method is the constructor for CTestDataCallDiffParam.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @param aVarDelay: user specified time delay
 * @note Initializes private data "aScript" to received parameter.
 */
	{}

void CTestDataCallDiffParam::ConstructL()
/**
 * This method is used to implement the 2 Phase Constructor for CTestDataCallDiffParam.
 * This method uses the CATBase ConstructL to configure the port to be used.
 *
 * @leave Leaves if CATBase leaves.
 */
	{
	CATScriptEng::ConstructL();
	}

TInt CTestDataCallDiffParam::Start()
/**
 * This method is defined as a pure virtual function that must be implemented.
 * This method is currently not used by the Etel regression test harness.
 * Instead of using this method to start the scripts, the CTestTxMess::Start()
 * method is used to start the scripts.  The CTestTxMess::Start() is called by
 * the responder thread of the scripting engine to start the execution of the script.
 *
 * @return KErrNone.
 */
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestDataCallDiffParam::SpecificAlgorithmL(TInt /* aParam */)
/**
 * This method is defined as a pure virtual function that must be implemented.
 * This method is currently not used by the Etel regression test harness.
 * Instead of using this method to perform an algorithm specific to this test,
 * the CTestTxMess::SpecificAlgorithm() method is used.  The CTestTxMess::SpecificAlgorithm()
 * is called by the scripting engine to perform the test specific algorithm.
 */
	{
	}

void CTestDataCallDiffParam::Complete(TInt aError)
/**
 * This method is defined as a pure virtual function that must be implemented.
 * This method is currently not used by the Etel regression test harness.
 * Instead of using this method to end the scripts, the CTestTxMess::Complete()
 * method is used to end the scripts.  The CTestTxMess::Complete() is called by
 * the scripting engine to end the execution of the script.
 */
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}

void CTestDriveDataCallDiffParam::Print_RMobileCall_TMobileCallDataSpeed(RMobileCall::TMobileCallDataSpeed& aArg)
/** Display value of enum RMobileCall::TMobileCallDataSpeed to screen.
 * @param value to print
*/
	{
	switch(aArg)
		{
		case RMobileCall::ESpeedUnspecified:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeedUnspecified\n")) ;
		break ;
		case RMobileCall::ESpeedAutobauding:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeedAutobauding\n")) ;
		break ;
		case RMobileCall::ESpeed2400:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed2400\n")) ;
		break ;
		case RMobileCall::ESpeed4800:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed4800\n")) ;
		break ;
		case RMobileCall::ESpeed9600:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed9600\n")) ;
		break ;
		case RMobileCall::ESpeed14400:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed14400\n")) ;
		break ;
		case RMobileCall::ESpeed19200:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed19200\n")) ;
		break ;
		case RMobileCall::ESpeed28800:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed28800\n")) ;
		break ;
		case RMobileCall::ESpeed32000:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed32000\n")) ;
		break ;
		case RMobileCall::ESpeed33600:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed33600\n")) ;
		break ;
		case RMobileCall::ESpeed38400:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed38400\n")) ;
		break ;
		case RMobileCall::ESpeed43200:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed43200\n")) ;
		break ;
		case RMobileCall::ESpeed48000:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed48000\n")) ;
		break ;
		case RMobileCall::ESpeed56000:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed56000\n")) ;
		break ;
		case RMobileCall::ESpeed57600:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed57600\n")) ;
		break ;
		case RMobileCall::ESpeed64000:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::ESpeed64000\n")) ;
		break ;
		}
	}

void CTestDriveDataCallDiffParam::Print_RMobileCall_TMobileCallDataProtocol(RMobileCall::TMobileCallDataProtocol& aArg)
/** Display value of enum RMobileCall::TMobileCallDataProtocol to screen.
 * @param value to print
*/
	{
	switch(aArg)
		{
		case RMobileCall::EProtocolUnspecified:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolUnspecified\n")) ;
		break ;
		case RMobileCall::EProtocolV22bis:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolV22bis\n")) ;
		break ;
		case RMobileCall::EProtocolV32:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolV32\n")) ;
		break ;
		case RMobileCall::EProtocolV34:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolV34\n")) ;
		break ;
		case RMobileCall::EProtocolV110:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolV110\n")) ;
		break ;
		case RMobileCall::EProtocolV120:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolV120\n")) ;
		break ;
		case RMobileCall::EProtocolX31FlagStuffing:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolX31FlagStuffing\n")) ;
		break ;
		case RMobileCall::EProtocolPIAFS:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolPIAFS\n")) ;
		break ;
		case RMobileCall::EProtocolBitTransparent:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolBitTransparent\n")) ;
		break ;
		case RMobileCall::EProtocolPstnMultimediaVoiceFallback:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolPstnMultimediaVoiceFallback\n")) ;
		break ;
		case RMobileCall::EProtocolPstnMultimedia:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolPstnMultimedia\n")) ;
		break ;
		case RMobileCall::EProtocolIsdnMultimedia:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EProtocolIsdnMultimedia\n")) ;
		break ;
		}
	}

void  CTestDriveDataCallDiffParam::DialDiffSpeedL(RMobileCall::TMobileCallDataSpeed aSpeed,
												 RMobileCall::TMobileCallDataProtocol aArg)
{
   	TRequestStatus reqStatus;

	RLine line;
	RCall call;
	INFO_PRINTF1(_L("Opening Data Line\n"));
	TESTL(line.Open(iPhone,KDataLineName)==KErrNone);
	INFO_PRINTF1(_L("Opening New Data Call\n"));
	TESTL(call.OpenNewCall(line)==KErrNone);

//
// Initialization complete
// Now perform the outgoing call...
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);
	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iProtocol = aArg ;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;
	callDataParams.iSpeed = aSpeed;
	Print_RMobileCall_TMobileCallDataSpeed(aSpeed) ;
	Print_RMobileCall_TMobileCallDataProtocol(aArg) ;

	INFO_PRINTF2(_L("Dialling %S....\n"), &KDialString);
	call.Dial(reqStatus, callDataParamsPckg, KDialString);
	User::WaitForRequest(reqStatus);
	INFO_PRINTF2(_L("Dial returned %d\n"), reqStatus.Int());
	if(KErrNone != reqStatus.Int())
		TESTL(reqStatus.Int()) ;
	call.HangUp();

	// close iPhone, line and call
	line.Close();
	call.Close();
}


void CTestDriveDataCallDiffParam::Print_RMobileCall_TMobileCallDataQoS(RMobileCall::TMobileCallDataQoS& aArg)
/** Display value of enum RMobileCall::TMobileCallDataQoS to screen.
 * @param value to print
*/
	{
	switch(aArg)
		{
		case RMobileCall::EQoSUnspecified:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EQoSUnspecified\n")) ;
		break ;
		case RMobileCall::EQoSTransparent:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EQoSTransparent\n")) ;
		break ;
		case RMobileCall::EQoSNonTransparent:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EQoSNonTransparent\n")) ;
		break ;
		case RMobileCall::EQosTransparentPreferred:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EQosTransparentPreferred\n")) ;
		break ;
		case RMobileCall::EQosNonTransparentPreferred:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EQosNonTransparentPreferred\n")) ;
		break ;
		}
	}

void  CTestDriveDataCallDiffParam::DialDiffQoSL(RMobileCall::TMobileCallDataQoS aQoS)
{
   	TRequestStatus reqStatus;

	RLine line;
	RCall call;
	INFO_PRINTF1(_L("Opening Data Line\n"));
	TESTL(line.Open(iPhone,KDataLineName)==KErrNone);
	INFO_PRINTF1(_L("Opening New Data Call\n"));
	TESTL(call.OpenNewCall(line)==KErrNone);
//
// Initialization complete
// Now perform the outgoing call...
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iQoS = aQoS;


	Print_RMobileCall_TMobileCallDataQoS(aQoS) ;
	INFO_PRINTF2(_L("Dialling %S....\n"), &KDialString);
	call.Dial(reqStatus, callDataParamsPckg, KDialString);
	User::WaitForRequest(reqStatus);
	INFO_PRINTF2(_L("Dial returned %d\n"), reqStatus.Int());
	if(KErrNone != reqStatus.Int())
		TESTL(reqStatus.Int()) ;
	call.HangUp();

	// close iPhone, line and call
	line.Close();
	call.Close();
}


void  CTestDriveDataCallDiffParam::DialDiffProtocolL(RMobileCall::TMobileCallDataProtocol aArg)
{
   	TRequestStatus reqStatus;

	RLine line;
	RCall call;
	INFO_PRINTF1(_L("Opening Data Line\n"));
	TESTL(line.Open(iPhone,KDataLineName)==KErrNone);
	INFO_PRINTF1(_L("Opening New Data Call\n"));
	TESTL(call.OpenNewCall(line)==KErrNone);
//
// Initialization complete
// Now perform the outgoing call...
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;
	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iProtocol = aArg;

	Print_RMobileCall_TMobileCallDataProtocol(aArg) ;

	INFO_PRINTF2(_L("Dialling %S....\n"), &KDialString);
	call.Dial(reqStatus, callDataParamsPckg, KDialString);
	User::WaitForRequest(reqStatus);
	INFO_PRINTF2(_L("Dial returned %d\n"), reqStatus.Int());
	TESTL(reqStatus.Int()==KErrNone);
	call.HangUp();

	// close iPhone, line and call
	line.Close();
	call.Close();
}

void CTestDriveDataCallDiffParam::Print_RMobileCall_TMobileCallDataService(RMobileCall::TMobileCallDataService aArg)
/** Display value of enum RMobileCall::TMobileCallDataService to screen.
 * @param value to print
*/
	{
	switch(aArg)
		{
		case RMobileCall::EServiceUnspecified:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServiceUnspecified\n")) ;
		break ;
		case RMobileCall::EServiceDataCircuitAsync:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServiceDataCircuitAsync\n")) ;
		break ;
		case RMobileCall::EServiceDataCircuitAsyncRdi:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServiceDataCircuitAsyncRdi\n")) ;
		break ;
		case RMobileCall::EServiceDataCircuitSync:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServiceDataCircuitSync\n")) ;
		break ;
		case RMobileCall::EServiceDataCircuitSyncRdi:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServiceDataCircuitSyncRdi\n")) ;
		break ;
		case RMobileCall::EServicePADAsyncUDI:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServicePADAsyncUDI\n")) ;
		break ;
		case RMobileCall::EServicePADAsyncRDI:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServicePADAsyncRDI\n")) ;
		break ;
		case RMobileCall::EServicePacketAccessSyncUDI:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServicePacketAccessSyncUDI\n")) ;
		break ;
		case RMobileCall::EServicePacketAccessSyncRDI:
			INFO_PRINTF1(_L("Value of enum is RMobileCall::EServicePacketAccessSyncRDI\n")) ;
		break ;
		}
	}

void  CTestDriveDataCallDiffParam::DialDiffDataServiceL(RMobileCall::TMobileCallDataService aArg)
{
   	TRequestStatus reqStatus;

	RLine line;
	RCall call;
	INFO_PRINTF1(_L("Opening Data Line\n"));
	TESTL(line.Open(iPhone,KDataLineName)==KErrNone);
	INFO_PRINTF1(_L("Opening New Data Call\n"));
	TESTL(call.OpenNewCall(line)==KErrNone);
//
// Initialization complete
// Now perform the outgoing call...
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;

	callDataParams.iService = aArg;

	Print_RMobileCall_TMobileCallDataService(aArg) ;

	INFO_PRINTF2(_L("Dialling %S....\n"), &KDialString);
	call.Dial(reqStatus, callDataParamsPckg, KDialString);
	User::WaitForRequest(reqStatus);
	INFO_PRINTF2(_L("Dial returned %d\n"), reqStatus.Int());
	if(KErrNone != reqStatus.Int())
		TESTL(reqStatus.Int()) ;
	call.HangUp();

	// close iPhone, line and call
	line.Close();
	call.Close();
}

