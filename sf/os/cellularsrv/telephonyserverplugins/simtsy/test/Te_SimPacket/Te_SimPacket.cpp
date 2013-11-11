// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed the test of the .
// 
//

/**
 @file
*/

#include "Te_SimPacket.h"

//******************************************************************************
// Context Configuration
//******************************************************************************
#define SIMTSY_PDP_TYPE1				RPacketContext::EPdpTypePPP
#define SIMTSY_ACCESS_POINT1			_L8("ACME INTERNET PROVIDERS LTD")
#define SIMTSY_PDP_ADDRESS1			_L8("i-MODE GATEWAY")
#define SIMTSY_COMPRESSION1			RPacketContext::KPdpDataCompression
#define SIMTSY_ANONYMOUS_ACCESS1		RPacketContext::ENotRequired
#define SIMTSY_EGPRS_REQUIRED			TRUE		

#define SIMTSY_PDP_TYPE2				RPacketContext::EPdpTypeIPv6
#define SIMTSY_ACCESS_POINT2			_L8("SYMBIAN EMPLOYEE INTRANET")
#define SIMTSY_PDP_ADDRESS2			_L8("A WAP PORTAL")
#define SIMTSY_COMPRESSION2			RPacketContext::KPdpHeaderCompression
#define SIMTSY_ANONYMOUS_ACCESS2		RPacketContext::ERequired

#define SIMTSY_CSY					_L("LOOPBACK.CSY")
#define SIMTSY_PORT					_L("LOOPBACK::0")

#define SIM_PACKET_SERVICE_CAPS (RPacketService::KCapsSuspendSupported | \
				RPacketService::KCapsMSClassSupported |\
				RPacketService::KCapsCDMA2000HighSpeedDataSupported |\
				RPacketService::KCapsNetworkAvailabilitySupported | \
				RPacketService::KCapsProtocolPPPSupported)

#define SIM_PACKET_SERVICE_OPTION	RPacketContext::KHighSpeedCDMA2000Data
#define SIM_PACKET_PROTOCOL			RPacketContext::EPdpTypePPP
#define SIM_DEFAULT_PARAM			RPacketContext::KHighSpeedCDMA2000Data

// CHAP authentication
#define SIM_PACKET_CONTEXT_PROTOCOL1 RPacketContext::EProtocolCHAP
#define SIM_PACKET_CONTEXT_USERNAME1 _L8("USER1")
#define SIM_PACKET_CONTEXT_PASSWORD1 _L8("PASSWORD1")
#define SIM_PACKET_CONTEXT_CHALLENGE1 _L8("CHALLENGE1")
#define SIM_PACKET_CONTEXT_RESPONSE1 _L8("RESPONSE1")
#define SIM_PACKET_CONTEXT_PRIMARYSERVER1 _L8("PRIMARYSERVER1")
#define SIM_PACKET_CONTEXT_SECONDARYSERVER1 _L8("SECONDARYSERVER1")
#define SIM_PACKET_CONTEXT_ID1 1

// PAP authentication 
#define SIM_PACKET_CONTEXT_PROTOCOL2 RPacketContext::EProtocolPAP
#define SIM_PACKET_CONTEXT_USERNAME2 _L("USER2")
#define SIM_PACKET_CONTEXT_PASSWORD2 _L("PASSWORD2")
#define SIM_PACKET_CONTEXT_CHALLENGE2 _L("NULL")
#define SIM_PACKET_CONTEXT_RESPONSE2 _L("NULL")
#define SIM_PACKET_CONTEXT_PRIMARYSERVER2 _L("PRIMARYSERVER2")
#define SIM_PACKET_CONTEXT_SECONDARYSERVER2 _L("SECONDARYSERVER2")
#define SIM_PACKET_CONTEXT_ID2 2

//MBMS information
#define SIM_PACKET_MBMS_TESTNUMBER	0
#define SIM_PACKET_MBMS_SESSIONID	2
#define SIM_PACKET_MBMS_SERVICEID	1155
#define SIM_PACKET_MBMS_MNCID		1122
#define SIM_PACKET_MBMS_MCCID      	1234

//******************************************************************************

static TInt NTRasThreadFunction(TAny* /*aThreadData*/)
/**
* Does 'durty' work to  start NT RAS session
* @note It sets
*	iRate = EBps38400;
*	iDataBits = EData8;
*	iStopBits = EStop1 ;
*	iParity = EParityNone;
* be sure that your NT comport has the same settings.
*/
	{
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName,"LOOPBACK::1");
	
	_LIT8(KServerText,"SERVER");

	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;

	RCommServ commServ;
	TInt r=commServ.Connect();
	r=commServ.LoadCommModule(KCsyName);
	if( KErrNone == r)
		{
		RComm comm;
		r=comm.Open(commServ,KPortName,ECommShared);
		if( KErrNone == r)
			{
			TBuf8<6> readBuf;  
			TRequestStatus stat0;
			comm.Read(stat0,readBuf);
			User::WaitForRequest(stat0) ;

			comm.Write(stat0,KServerText);
			User::WaitForRequest(stat0) ;
			}
		comm.Close() ;
		}
	commServ.Close() ;
	delete cleanup;
	return r;
	}

TInt StartNTRasSimulation()
/**
* Starts virual, because of lookback serial driver,
* NT RAS session
*/
	{
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;
	TInt res = KErrNone ;
	_LIT(KNtRasName,"NTRasEmulation");

	RThread t;
	res=t.Create(KNtRasName,NTRasThreadFunction,
					KStackSize,KHeapSize,KMaxHeapSize,NULL);
	t.Resume();
	t.Close();

	return res;
	}

//------------------------------------------------------------------
//			** TEST SimPacket STEPS ***

//------------------------------------------------------------------

CGprsDefaultContextParamsTest::CGprsDefaultContextParamsTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsDefaultContextParamsTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsDefaultContextParamsTest::doTestStepL( void )
/**
 * This tests the default context params
 */
    {
	SetSimtsyTestNumberL(0);
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

	// Some default params to set, does not matter what it is.
	//  The default params used are defined at the beginning of this file

	TESTL(iPacketService.Open(iPhone) == KErrNone);

    RPacketContext::TContextConfigGPRS     setDefaultParams;
	TPckg<RPacketContext::TContextConfigGPRS>    setcontextConfigPckg(setDefaultParams);

	setDefaultParams.iPdpType = SIMTSY_PDP_TYPE1;
    setDefaultParams.iAccessPointName = SIMTSY_ACCESS_POINT1;
    setDefaultParams.iPdpAddress = SIMTSY_PDP_ADDRESS1;
    setDefaultParams.iPdpCompression = SIMTSY_COMPRESSION1;
	setDefaultParams.iAnonymousAccessReqd = SIMTSY_ANONYMOUS_ACCESS1;
	setDefaultParams.iUseEdge = SIMTSY_EGPRS_REQUIRED;
	setDefaultParams.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	setDefaultParams.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	setDefaultParams.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	setDefaultParams.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	setDefaultParams.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	setDefaultParams.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	setDefaultParams.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	setDefaultParams.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;
    
    TESTL(iPacketService.SetDefaultContextParams(setcontextConfigPckg) == KErrNone);

	RPacketContext::TContextConfigGPRS     getDefaultParams;
	TPckg<RPacketContext::TContextConfigGPRS>    getcontextConfigPckg(getDefaultParams);

    TESTL(iPacketService.GetDefaultContextParams(getcontextConfigPckg) == KErrNone);
    TestConfigParamsL(setDefaultParams, getDefaultParams);

    iPacketService.Close();
	iPhone.Close();
	User::After(100000L);
	return TestStepResult();
	}
//------------------------------------------------------------------

CQoSTest::CQoSTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("QoSTest"));
	}
//------------------------------------------------------------------

TVerdict CQoSTest::doTestStepL( void )
/**
 * This tests the static GPRS caps.
 */
    {
	SetSimtsyTestNumberL(0);
    TBuf<30>                            contextName;
	TBuf<30>                            profileName;
	TRequestStatus						stat1;
	TRequestStatus						stat2;
	RPacketQoS							qoS;
	RPacketQoS::TQoSCapsGPRS			staticQoSCaps;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

    TESTL(iPacketService.Open(iPhone ) == KErrNone);
	TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
	qoS.OpenNewQoS(iPacketContext[0], profileName);

	TPckg<RPacketQoS::TQoSCapsGPRS> staticQoSCapsPkg(staticQoSCaps);
	qoS.GetProfileCapabilities(stat1, staticQoSCapsPkg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
	TESTL(staticQoSCaps.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);

	TESTL(staticQoSCaps.iPrecedence		== RPacketQoS::EPriorityMediumPrecedence);
	TESTL(staticQoSCaps.iDelay			== RPacketQoS::EDelayClass2 );
	TESTL(staticQoSCaps.iReliability	== RPacketQoS::EReliabilityClass2);
	TESTL(staticQoSCaps.iPeak			== RPacketQoS::EPeakThroughput64000);
	TESTL(staticQoSCaps.iMean			== RPacketQoS::EMeanThroughput100);

	RPacketQoS::TQoSGPRSNegotiated		negotiatedQoS;
	RPacketQoS::TQoSGPRSRequested		SetQoS;
	RPacketQoS::TQoSGPRSNegotiated      comparenegotiatedQoS;
	RPacketQoS::TQoSGPRSRequested		notifyQoS;
	// Setting to Best effort.
	comparenegotiatedQoS.iPrecedence		= RPacketQoS::EPriorityMediumPrecedence;
	comparenegotiatedQoS.iDelay				= RPacketQoS::EDelayClass2;
	comparenegotiatedQoS.iReliability		= RPacketQoS::EReliabilityClass2;
	comparenegotiatedQoS.iPeakThroughput	= RPacketQoS::EPeakThroughput64000;
	comparenegotiatedQoS.iMeanThroughput	= RPacketQoS::EMeanThroughput100;
	TPckg<RPacketQoS::TQoSGPRSNegotiated>		negotiatedQoSPkg(negotiatedQoS);
	qoS.GetProfileParameters(stat1, negotiatedQoSPkg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
	TestConfigParamsL(negotiatedQoS, comparenegotiatedQoS);

	// Set to anything just to test that it can be done..
	SetQoS.iReqPrecedence		= RPacketQoS::EPriorityMediumPrecedence;
	SetQoS.iMinPrecedence		= RPacketQoS::EPriorityLowPrecedence;
	SetQoS.iReqDelay			= RPacketQoS::EDelayClass2;
	SetQoS.iMinDelay			= RPacketQoS::EDelayClass1;
	SetQoS.iReqReliability		= RPacketQoS::EReliabilityClass2;
	SetQoS.iMinReliability		= RPacketQoS::EReliabilityClass1;
	SetQoS.iReqPeakThroughput	= RPacketQoS::EPeakThroughput64000;
	SetQoS.iMinPeakThroughput	= RPacketQoS::EPeakThroughput16000;
	SetQoS.iReqMeanThroughput	= RPacketQoS::EMeanThroughput2000;
	SetQoS.iMinMeanThroughput	= RPacketQoS::EMeanThroughput200;


	TPckg<RPacketQoS::TQoSGPRSRequested>		SetQoSPkg(SetQoS);
	TPckg<RPacketQoS::TQoSGPRSRequested>		notifyQoSPkg(notifyQoS);
	qoS.NotifyProfileChanged(stat2, notifyQoSPkg);
	qoS.SetProfileParameters(stat1, SetQoSPkg);
	User::WaitForRequest(stat2);
	User::WaitForRequest(stat1);
	TESTL(stat2.Int() == KErrNone);
	TESTL(stat1.Int() == KErrNone);

	qoS.Close();
	iPacketContext[0].Close();
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsUnSupportedTest::CGprsUnSupportedTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsUnSupportedTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsUnSupportedTest::doTestStepL( void )
/**
 * Tests all unsupported GPRS API calls to the TSY.
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus stat1;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

    TESTL(iPacketService.Open(iPhone ) == KErrNone);
    iPacketService.RejectActivationRequest(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int()== KErrNotSupported);

    iPacketService.RejectActivationRequest(stat1);
    iPacketService.CancelAsyncRequest(EPacketRejectActivationRequest);
    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNotSupported);

    RPacketService::TMSClass msClass;
    iPacketService.NotifyMSClassChange(stat1, msClass);
    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNotSupported);

    RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
    TESTL(iPacketService.SetAttachMode(attachMode) == KErrNotSupported);
    TESTL(iPacketService.GetAttachMode(attachMode) == KErrNone);
    RPacketService::TRegistrationStatus registrationStatus = RPacketService::ERegisteredRoaming;
    iPacketService.GetNtwkRegStatus(stat1, registrationStatus);
  User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
/*	//This test should not be here as this method is supported 
    iPacketService.NotifyChangeOfNtwkRegStatus(stat1, registrationStatus);
    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNotSupported);
*/
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}

/** Each test step initialises its own name
*/
CTestDynamicCaps::CTestDynamicCaps()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestDynamicCaps"));
	}

/**
 *  Tests the dynamic capabilities APIs: RPacketService::GetDynamicCaps and RPacketService::NotifyDynamicCapsChange
 *  @return Test step verdict.
 */
TVerdict CTestDynamicCaps::doTestStepL()
    {
    SetSimtsyTestNumberL(0);

    // Initialise and open the packet service...
    TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
    TESTL(iPhone.Initialise()==KErrNone);
    TESTL(iPacketService.Open(iPhone ) == KErrNone);

    //
    // test GetDynamicCaps
    //
    RPacketService::TDynamicCapsFlags dynCapsFlags;
    TEST(iPacketService.GetDynamicCaps(dynCapsFlags) == KErrNone);
    TEST(dynCapsFlags == RPacketService::KCapsEGPRS);
    
    //
    // test NotifyDynamicCapsChange
    //
    TRequestStatus requestStatus;
    dynCapsFlags = 0;
    iPacketService.NotifyDynamicCapsChange(requestStatus, dynCapsFlags);
    User::WaitForRequest(requestStatus);
    TEST(requestStatus == KErrNone); 
    TEST(dynCapsFlags == RPacketService::KCapsHSDPA);

    //
    // test cancellation of NotifyDynamicCapsChange
    //
    iPacketService.NotifyDynamicCapsChange(requestStatus, dynCapsFlags);
    iPacketService.CancelAsyncRequest(EPacketNotifyDynamicCapsChange);
    User::WaitForRequest(requestStatus);
    TEST(requestStatus == KErrCancel);
    
    iPacketService.Close();
    iPhone.Close();
    return TestStepResult();
    }

//------------------------------------------------------------------

CGprsContextAttachTest::CGprsContextAttachTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsContextAttachTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsContextAttachTest::doTestStepL( void )
/**
 * This tests the context attach and detach
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus                      stat1;
    TRequestStatus						stat2;
	TBuf<50>                            contextName;
    RPacketContext::TContextStatus        contextStatus;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
    TESTL(iPacketService.Open(iPhone ) == KErrNone);
	StartNTRasSimulation() ;

    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);

    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    iPacketContext[0].CancelAsyncRequest(EPacketContextNotifyStatusChange);
    User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrCancel);

    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
	iPacketContext[0].Activate(stat2);

	User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNone);
    TESTL(contextStatus == RPacketContext::EStatusActivating);

    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);	
	User::WaitForRequest(stat2);
	TESTL(stat2.Int()== KErrNone);

    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNone);
    TESTL(contextStatus == RPacketContext::EStatusActive);

    iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
	
	// need a few seconds between activating and deactivating - allow NTRAS server to respond
	User::After(100000L);
	iPacketContext[0].Deactivate(stat2);
	User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNone);
    TESTL(contextStatus == RPacketContext::EStatusDeactivating);
	
	iPacketContext[0].NotifyStatusChange(stat1, contextStatus);
	User::WaitForRequest(stat2);
	TESTL(stat2.Int() == KErrNone);

    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNone);
    TESTL(contextStatus == RPacketContext::EStatusInactive);

    iPacketContext[0].Close();
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsContextDeleteTest::CGprsContextDeleteTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsContextDeleteTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsContextDeleteTest::doTestStepL( void )
/**
 *  This tests the context delete.
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus						stat1;
	TBuf<30>                            contextName;
    RPacketContext::TContextStatus		contextStatus;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

    TESTL(iPacketService.Open(iPhone ) == KErrNone);
    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);

	iPacketContext[0].Delete(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);

	iPacketContext[0].Activate(stat1);
	User::WaitForRequest(stat1);

	TESTL(stat1.Int() == KErrNotReady);
    TESTL(iPacketContext[0].GetStatus(contextStatus) == KErrNone);
	TESTL(contextStatus == RPacketContext::EStatusDeleted);

    iPacketContext[0].Close();
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsContextLoanComport::CGprsContextLoanComport()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsContextLoanComport"));
	}
//------------------------------------------------------------------

TVerdict CGprsContextLoanComport::doTestStepL( void )
/**
 * This tests the comport loan.
 */
    {
	SetSimtsyTestNumberL(0);
	TRequestStatus						stat1;
    TBuf<30>                            contextName;
	RCall::TCommPort dataPort;
	StartNTRasSimulation() ;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

    TESTL(iPacketService.Open(iPhone ) == KErrNone);
    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);

    iPacketContext[0].RecoverCommPort(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrEtelPortNotLoanedToClient);

	//!!!!
    iPacketContext[0].LoanCommPort(stat1, dataPort);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrEtelCallNotActive);

    iPacketContext[0].Activate(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);

    iPacketContext[0].LoanCommPort(stat1, dataPort);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);

    iPacketContext[0].Activate(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrEtelCallAlreadyActive);

    iPacketContext[0].RecoverCommPort(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);

    iPacketContext[0].Deactivate(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);

    iPacketContext[0].Close();
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsContextUnSupportedTest::CGprsContextUnSupportedTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsContextUnSupportedTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsContextUnSupportedTest::doTestStepL( void )
/**
 *  This tests all GPRS context API calls.
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus            stat1;
    TBuf<30>                  contextName;
    RPacketContext::TDataVolume volume;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

	TESTL(iPacketService.Open(iPhone ) == KErrNone);
    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);

    TESTL(iPacketContext[0].GetDataVolumeTransferred(volume) == KErrNotSupported);
    iPacketContext[0].NotifyDataTransferred(stat1, volume);
    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNotSupported);

    iPacketContext[0].Close();
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsCancelTest::CGprsCancelTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsCancelTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsCancelTest::doTestStepL( void )
/**
 * This test will call all GPRS API calls
 * that can be canceled with an outstanding request
 * to the phone.
 */
	{
	SetSimtsyTestNumberL(0);
	TRequestStatus	stat;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);

	iPacketService.Attach(stat);
	iPacketService.CancelAsyncRequest(EPacketAttach);
	User::WaitForRequest(stat);
	TESTL((stat.Int() == KErrCancel) || (stat.Int() == KErrNone));

	iPacketService.Detach(stat);
	iPacketService.CancelAsyncRequest(EPacketDetach);
	User::WaitForRequest(stat);
	TESTL((stat.Int() == KErrCancel)||(stat.Int() == KErrNone));

	RPacketService::TMSClass currentClass;
	RPacketService::TMSClass maxClass;

	iPacketService.GetMSClass(stat, currentClass, maxClass);
	iPacketService.CancelAsyncRequest(EPacketGetMSClass);
	User::WaitForRequest(stat);
	TESTL((stat.Int() == KErrCancel)||(stat.Int() == KErrNone));

	currentClass = RPacketService::EMSClassSuspensionRequired;
	iPacketService.SetMSClass(stat, currentClass);
	iPacketService.CancelAsyncRequest(EPacketSetMSClass);
	User::WaitForRequest(stat);
	TESTL((stat.Int() == KErrCancel)||(stat.Int() == KErrNone));

	iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsAttachTest::CGprsAttachTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsAttachTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsAttachTest::doTestStepL( void )
/**
 *  This tests the attach and detach.
 */
    {
	SetSimtsyTestNumberL(0);
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
    TRequestStatus stat1;
    RPacketService::TStatus status;
    TESTL(iPacketService.Open(iPhone ) == KErrNone);
    iPacketService.Attach(stat1);
    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNone);
    iPacketService.GetStatus(status);
    TESTL(status == RPacketService::EStatusAttached);
    iPacketService.Detach(stat1);
    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNone);
    iPacketService.GetStatus(status);
    TESTL(status == RPacketService::EStatusUnattached);
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsNotificationTest::CGprsNotificationTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsNotificationTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsNotificationTest::doTestStepL( void )
/**
 * Tests Notify Context added, Tests Notify status
 */
    {
	SetSimtsyTestNumberL(1);
    TRequestStatus stat1,stat2,stat3;
    TBuf<20> context= _L("");
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

    TESTL(iPacketService.Open(iPhone ) == KErrNone);
    TBuf<40> contextId;

    iPacketService.NotifyContextAdded(stat1, contextId);
    iPacketService.CancelAsyncRequest(EPacketNotifyContextAdded);
	User::WaitForRequest(stat1);
    TESTL(stat1.Int()== KErrCancel);
    iPacketService.NotifyContextAdded(stat1, contextId);

    TESTL(iPacketContext[0].OpenNewContext(iPacketService, context) == KErrNone);
    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNone);

    RPacketService::TStatus gprsStatus;
    iPacketService.NotifyStatusChange(stat2, gprsStatus);
    iPacketService.CancelAsyncRequest(EPacketNotifyStatusChange);
    User::WaitForRequest(stat2);
    TESTL(stat2.Int() == KErrCancel);

    iPacketService.NotifyStatusChange(stat2, gprsStatus);
    iPacketService.Attach(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
    User::WaitForRequest(stat2);
    TESTL(stat2.Int() == KErrNone);
    TESTL(gprsStatus == RPacketService::EStatusAttached);

    iPacketService.NotifyStatusChange(stat3, gprsStatus);
    iPacketService.Detach(stat1);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
    User::WaitForRequest(stat3);
    TESTL(stat3.Int() == KErrNone);
    TESTL(gprsStatus == RPacketService::EStatusUnattached);

    iPacketContext[0].Close();
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsMsClassTest::CGprsMsClassTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsMsClassTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsMsClassTest::doTestStepL( void )
/**
 * This tests the MS Class.
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus stat1;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
    TESTL(iPacketService.Open(iPhone ) == KErrNone);

    RPacketService::TMSClass     currentClass;
    RPacketService::TMSClass     maxClass;
    iPacketService.GetMSClass(stat1, currentClass, maxClass);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);

	iPacketService.SetMSClass(stat1, currentClass);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
	TESTL(currentClass	== RPacketService::EMSClassSuspensionRequired);
	TESTL(maxClass		== RPacketService::EMSClassSuspensionRequired);

    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsContextConfigTest::CGprsContextConfigTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsContextConfigTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsContextConfigTest::doTestStepL( void )
/**
 * Test context config.
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus stat1;
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);
    TBuf<30> contextName;
    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
    TESTL(iPacketContext[1].OpenNewContext(iPacketService, contextName) == KErrNone);

    RPacketContext::TContextConfigGPRS     Getparams;
    RPacketContext::TContextConfigGPRS     Setparams;
	Setparams.iPdpType = SIMTSY_PDP_TYPE2;
    Setparams.iAccessPointName = SIMTSY_ACCESS_POINT2;
    Setparams.iPdpAddress = SIMTSY_PDP_ADDRESS2;
    Setparams.iPdpCompression = SIMTSY_COMPRESSION2;
	Setparams.iAnonymousAccessReqd = SIMTSY_ANONYMOUS_ACCESS2;
	Setparams.iUseEdge = SIMTSY_EGPRS_REQUIRED;

    TPckg<RPacketContext::TContextConfigGPRS>    GetcontextConfigPckg(Getparams);
    iPacketContext[0].GetConfig(stat1, GetcontextConfigPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
    Setparams = Getparams;

    TPckg<RPacketContext::TContextConfigGPRS>    SetcontextConfigPckg(Setparams);
    iPacketContext[0].SetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
    iPacketContext[1].SetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
    iPacketContext[0].GetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
	TRequestStatus stat;
	iPacketContext[0].SetConfig(stat, SetcontextConfigPckg);
	iPacketContext[0].CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(stat);
	TESTL(stat.Int() == KErrCancel || stat.Int() == KErrNone);
	//!!!!
	iPacketContext[0].GetConfig(stat, GetcontextConfigPckg);
	iPacketContext[0].CancelAsyncRequest(EPacketContextGetConfig);
	User::WaitForRequest(stat);
	TESTL(stat.Int() == KErrCancel || stat.Int() == KErrNone);


	Setparams.iPdpType = RPacketContext::TProtocolType(20); // Set out of range
	TPckg<RPacketContext::TContextConfigGPRS>    errorPackage(Setparams);
	iPacketContext[0].SetConfig(stat1, errorPackage);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrCorrupt);


    iPacketContext[0].Close();
    iPacketContext[1].Close();
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CGprsContextConfigNotificationsTest::CGprsContextConfigNotificationsTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsContextConfigNotificationsTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsContextConfigNotificationsTest::doTestStepL( void )
/**
 * Test context notifications.
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus                      stat1, stat2;
    TBuf<30>                            contextName;
    RPacketContext::TContextConfigGPRS   params, notifyParams;
    TPckg<RPacketContext::TContextConfigGPRS>  contextConfigPckg(params);
    TPckg<RPacketContext::TContextConfigGPRS>  notifyContextConfigPckg(notifyParams);
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

    TESTL(iPacketService.Open(iPhone ) == KErrNone);
    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
    TESTL(iPacketContext[1].OpenNewContext(iPacketService, contextName) == KErrNone);
    iPacketContext[0].NotifyConfigChanged(stat1, notifyContextConfigPckg);
    iPacketContext[0].CancelAsyncRequest(EPacketContextNotifyConfigChanged);
    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrCancel);

    iPacketContext[0].GetConfig(stat1, contextConfigPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
    iPacketContext[0].NotifyConfigChanged(stat1, notifyContextConfigPckg);

	iPacketContext[0].SetConfig(stat2, contextConfigPckg);
	User::WaitForRequest(stat2);
	TESTL(stat2.Int() == KErrNone);

    User::WaitForRequest(stat1);
    TESTL(stat1.Int() == KErrNone);
    TestConfigParamsL(params, notifyParams);

    iPacketContext[0].Close();
    iPacketContext[1].Close();
    iPacketService.Close();
	iPhone.Close();
	User::After(1000000);	// to allow the next text to be able to see the contexts as closed
	return TestStepResult();
	}
//------------------------------------------------------------------

COpenGprsMaxContextsTest::COpenGprsMaxContextsTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("OpenGprsMaxContextsTest"));
	}
//------------------------------------------------------------------

TVerdict COpenGprsMaxContextsTest::doTestStepL( void )
/**
 *  Tests enumerate contexts, open new context, open existing context.
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus	stat1;
	TBuf<20> contextName = _L("");
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);

    TESTL(iPacketService.Open(iPhone ) == KErrNone);
    TInt countContexts;
    TInt maxAllowedContexts;
	//!!!
    iPacketService.EnumerateContexts(stat1, countContexts, maxAllowedContexts);
	User::WaitForRequest(stat1);
    TESTL(countContexts == 0);	

	TESTL(maxAllowedContexts == 11);

    TInt i = 0;
    RPacketService::TContextInfo contextinfo;

    for(i = 0; i < maxAllowedContexts;i++)
        {
        RPacketContext contextPacket;
        TBuf<30> tmpContextName;
        tmpContextName = _L("PRIMARY");
        tmpContextName.AppendNum(i+1);
        TESTL(iPacketContext[i].OpenNewContext(iPacketService, contextName) == KErrNone);
        iPacketService.GetContextInfo(stat1, i, contextinfo);
		User::WaitForRequest(stat1);
        TESTL(contextinfo.iName == tmpContextName);
        TESTL(contextinfo.iStatus == RPacketContext::EStatusInactive);
        TESTL(contextPacket.OpenExistingContext(iPacketService, contextName) == KErrNone);
        TESTL(contextName == tmpContextName);
        contextPacket.Close();
		iPhone.Close();
        }

	iPacketService.GetContextInfo(stat1, i+1, contextinfo);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrArgument);
    for(i = 0; i < maxAllowedContexts;i++)
        {
        iPacketContext[i].Close();
        }
    iPacketService.Close();
    iPhone.Close();
	return TestStepResult();
	}

//------------------------------------------------------------------

CGprsContextConfigArbitraryAddressTest::CGprsContextConfigArbitraryAddressTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GprsContextConfigArbitraryAddressTest"));
	}
//------------------------------------------------------------------

TVerdict CGprsContextConfigArbitraryAddressTest::doTestStepL()
/**
 * Test context config.
 */
    {
	SetSimtsyTestNumberL(0);
    TRequestStatus stat1;
    RPacketContext::TContextConfigGPRS     getParams;
    RPacketContext::TContextConfigGPRS     setParams;
    
    // Loading configuration from File
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);
    TBuf<30> contextName;
    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);

    // Getting the Actual Configuration
    TPckg<RPacketContext::TContextConfigGPRS>    getContextConfigPckg(getParams);
    iPacketContext[0].GetConfig(stat1, getContextConfigPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
    setParams = getParams;
    
    // Hacking the PdpAddress with an Arbitrary String (doesn't need to be a proper IP)
    setParams.iPdpAddress = _L8("ARBITRARY ADDRESS");

    // Setting the New Configuration checking that it is accepted (no "KErrCorrupt")
    TPckg<RPacketContext::TContextConfigGPRS>    setContextConfigPckg(setParams);
    iPacketContext[0].SetConfig(stat1, setContextConfigPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() != KErrCorrupt && stat1.Int() == KErrNone);
	
	// Getting the Configuration
    iPacketContext[0].GetConfig(stat1, setContextConfigPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1.Int() == KErrNone);
	
	// Checking the Configuration was correctly changed by Sim.tsy
	TESTL(setParams.iPdpAddress.Compare(getParams.iPdpAddress) == 0);
	
    iPacketContext[0].Close();
    iPacketService.Close();
	iPhone.Close();
	return TestStepResult();
	}

//------------------------------------------------------------------

CMbmsOpenNewContextTest::CMbmsOpenNewContextTest()
/** Each test step initialises it's own name
 */
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("EtelPcktMbmsCap1"));
	}
//------------------------------------------------------------------

TVerdict CMbmsOpenNewContextTest::doTestStepL( void )
/**
 *  Tests creation of new MBMS context activation using
 * OpenNewContext of RPacketMbmsContext
 */
	{
	SetSimtsyTestNumberL(SIM_PACKET_MBMS_TESTNUMBER);
	TRequestStatus	reqStatus;
	TBuf<20> contextName = _L("");
	
	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);
	
	TInt ret= iPacketMbmsContext.OpenNewContext(iPacketService, contextName);
	TEST(ret == KErrNone); //OpenNewContext will not use any capabilities.

	iPacketMbmsContext.Close();
	iPacketService.Close();
	iPhone.Close();

	return TestStepResult();
	}


//-------------------------------------------------------------------------

CMbmsUpdateSessionTest::CMbmsUpdateSessionTest()
/** Each test step initialises it's own name
 */
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("EtelPcktMbmsCap2"));
	}
//------------------------------------------------------------------

TVerdict CMbmsUpdateSessionTest::doTestStepL( void )
/**
 * Tests creation of new MBMS context activation and
 * tests the update session functionality of the MBMS context using the
 * API MbmsUpdateSessionTest of RPacketMbmsContext 
 */
	{
	SetSimtsyTestNumberL(SIM_PACKET_MBMS_TESTNUMBER);
	TRequestStatus	reqStatus;
	TBuf<20> contextName = _L("");

	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);

	TInt ret= iPacketMbmsContext.OpenNewContext(iPacketService, contextName);
	TESTL(ret == KErrNone);

	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> contextConfigPckg(contextConfig);

	//Get configuration for the MBMS
	iPacketMbmsContext.GetConfig(reqStatus, contextConfigPckg);
	User::WaitForRequest(reqStatus);

	RPacketContext::TContextStatus contextStatus;
	TRequestStatus notifyStatusRequest;
	iPacketMbmsContext.NotifyStatusChange(notifyStatusRequest, contextStatus);	

	// Activate the MBMS context
	iPacketMbmsContext.Activate(reqStatus);
	User::WaitForRequest(reqStatus);

	User::WaitForRequest(notifyStatusRequest);//completes NotifyStatusChange with state “ctivating”

	/** Session is added to the activated services table */
	TMbmsAction aAction=EAddEntries;
	TUint sessionid=SIM_PACKET_MBMS_SESSIONID;
	TRequestStatus updateSessionRequest;
	iPacketMbmsContext.UpdateMbmsSessionList(updateSessionRequest,reinterpret_cast<TMbmsAction &>(aAction), sessionid);

	/** the state of the session added changes from inactive to activated */
	User::WaitForRequest(updateSessionRequest);//completes UpdateSession
	TESTL(updateSessionRequest.Int() == KErrPermissionDenied);
	
	iPacketMbmsContext.Close();
	iPacketService.Close();
	iPhone.Close();

	return TestStepResult();
	}

//-------------------------------------------------------------------------
CGetMbmsNetworkServiceStatus::CGetMbmsNetworkServiceStatus()
/** Each test step initialises it's own name
 */
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("EtelPcktMbmsCap3"));
	}
//------------------------------------------------------------------

TVerdict CGetMbmsNetworkServiceStatus::doTestStepL( void )
/**
 * Tests the functionality of GetMbmsNetworkServiceStatus of RPacketService.
 * 
 */
	{
	SetSimtsyTestNumberL(SIM_PACKET_MBMS_TESTNUMBER);
	TRequestStatus	reqStatus;
	TBuf<20> contextName = _L("");

	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);

	TMbmsNetworkServiceStatus tMbmsNetworkServiceStatus;
	TBool attemptAttached = ETrue;

	iPacketService.GetMbmsNetworkServiceStatus(reqStatus, attemptAttached, tMbmsNetworkServiceStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrPermissionDenied);

	iPacketService.Close();
	iPhone.Close();

	return TestStepResult();
	}

//-------------------------------------------------------------------------

CNotifyMbmsNetworkServiceStatusChange::CNotifyMbmsNetworkServiceStatusChange()
/** Each test step initialises it's own name
 */
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("EtelPcktMbmsCap4"));
	}
//------------------------------------------------------------------

TVerdict CNotifyMbmsNetworkServiceStatusChange::doTestStepL( void )
/**
 * Tests the functionality of NotifyMbmsNetworkServiceStatusChange of RPacketService.
 * 
 */
	{
	SetSimtsyTestNumberL(SIM_PACKET_MBMS_TESTNUMBER);
	TRequestStatus	reqStatus;
	TBuf<20> contextName = _L("");

	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);

	TMbmsNetworkServiceStatus tMbmsNetworkServiceStatus;

	//To get notified when the network service status changes 
	iPacketService.NotifyMbmsNetworkServiceStatusChange(reqStatus, tMbmsNetworkServiceStatus);
	User::WaitForRequest(reqStatus);

	TESTL(reqStatus.Int() == KErrPermissionDenied);

	iPacketService.Close();
	iPhone.Close();

	return TestStepResult();
	}

//-------------------------------------------------------------------------

CNotifyMbmsServiceAvailabilityChange::CNotifyMbmsServiceAvailabilityChange()
/** Each test step initialises it's own name
 */
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("EtelPcktMbmsCap5"));
	}
//------------------------------------------------------------------

TVerdict CNotifyMbmsServiceAvailabilityChange::doTestStepL( void )
/**
 * Tests Notification of the Service Availability List using the 
 * API NotifyMbmsServiceAvailabilityListChange of RPacketService.
 */
	{
	SetSimtsyTestNumberL(SIM_PACKET_MBMS_TESTNUMBER);
	TRequestStatus	reqStatus;
	TBuf<20> contextName = _L("");

	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);

	CPcktMbmsMonitoredServiceList* mbmslist =  CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(mbmslist);

	//Update the Mbms service availability List
	TMbmsAction aAction=EAddEntries;
	iPacketService.UpdateMbmsMonitorServiceListL(reqStatus, reinterpret_cast<TMbmsAction &>(aAction), mbmslist);
	User::WaitForRequest(reqStatus);

	//Notify the change in the mbms service parameter list
	iPacketService.NotifyMbmsServiceAvailabilityChange (reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);

	CleanupStack::PopAndDestroy();	//mbmslist
	iPacketService.Close();
	iPhone.Close();

	return TestStepResult();
	}


//-------------------------------------------------------------------------

CUpdateMbmsMonitorServiceList::CUpdateMbmsMonitorServiceList()
/** Each test step initialises it's own name
 */
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("EtelPcktMbmsCap6"));
	}
//------------------------------------------------------------------

TVerdict CUpdateMbmsMonitorServiceList::doTestStepL( void )
/**
 *  Tests updation of the Service Monitoring List using the 
 * 	API UpdateMbmsMonitorServiceListL of RPacketService.
 */
	{
	SetSimtsyTestNumberL(SIM_PACKET_MBMS_TESTNUMBER);
	TRequestStatus	reqStatus;
	TBuf<20> contextName = _L("");

	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);

	CPcktMbmsMonitoredServiceList* mbmsList =  CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(mbmsList);

	/* ADD SERVICE */

	RPacketService::TMbmsServiceAvailabilityV1 mbmsServAvail; 
	TMbmsAction action=EAddEntries;
	TInt ab1=0,s1=1,a1=0;
	TInt ab2=1,s2=2,a2=0;

	mbmsServAvail.iTmgi.SetServiceId(SIM_PACKET_MBMS_SERVICEID);
	mbmsServAvail.iTmgi.SetMCC(SIM_PACKET_MBMS_MCCID);
	mbmsServAvail.iTmgi.SetMNC(SIM_PACKET_MBMS_MNCID);
	mbmsServAvail.iMbmsServiceMode = reinterpret_cast<TMbmsServiceMode&>(s1);
	mbmsServAvail.iMbmsAvailabilityStatus = reinterpret_cast<TMbmsAvailabilityStatus&>(a1);
	mbmsServAvail.iMbmsAccessBearer = reinterpret_cast<TMbmsScope&>(ab1);
	mbmsList->AddEntryL(mbmsServAvail);

	mbmsServAvail.iTmgi.SetServiceId(SIM_PACKET_MBMS_SERVICEID);
	mbmsServAvail.iTmgi.SetMCC(SIM_PACKET_MBMS_MCCID);
	mbmsServAvail.iTmgi.SetMNC(SIM_PACKET_MBMS_MNCID);
	mbmsServAvail.iMbmsServiceMode = reinterpret_cast<TMbmsServiceMode&>(s2);
	mbmsServAvail.iMbmsAvailabilityStatus = reinterpret_cast<TMbmsAvailabilityStatus&>(a2);
	mbmsServAvail.iMbmsAccessBearer = reinterpret_cast<TMbmsScope&>(ab2);
	mbmsList->AddEntryL(mbmsServAvail);	

	iPacketService.UpdateMbmsMonitorServiceListL(reqStatus,action,mbmsList);
	User::WaitForRequest(reqStatus);
	CleanupStack::PopAndDestroy();
	TESTL(reqStatus.Int() == KErrPermissionDenied);

	/* REMOVE SPECIFIC SERVICE */
	mbmsList= CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(mbmsList);
	action=ERemoveEntries;
	mbmsServAvail.iTmgi.SetServiceId(2345); 
	mbmsServAvail.iTmgi.SetMCC(2345);
	mbmsServAvail.iTmgi.SetMNC(2345);
	mbmsServAvail.iMbmsServiceMode = reinterpret_cast<TMbmsServiceMode&>(s2);
	mbmsServAvail.iMbmsAvailabilityStatus = reinterpret_cast<TMbmsAvailabilityStatus&>(a2);
	mbmsServAvail.iMbmsAccessBearer = reinterpret_cast<TMbmsScope&>(ab2);
	mbmsList->AddEntryL(mbmsServAvail);

	iPacketService.UpdateMbmsMonitorServiceListL(reqStatus,action,mbmsList);
	User::WaitForRequest(reqStatus);
	CleanupStack::PopAndDestroy();
	TESTL(reqStatus.Int() == KErrPermissionDenied);

	/* REMOVE_ALL */
	action=ERemoveAllEntries;
	iPacketService.UpdateMbmsMonitorServiceListL(reqStatus,action);
	User::WaitForRequest(reqStatus);
	
	TESTL(reqStatus.Int() == KErrPermissionDenied);

	iPacketService.Close();
	iPhone.Close();

	return TestStepResult();
	}

//-------------------------------------------------------------------------

CEnumerateMbmsActiveServiceList::CEnumerateMbmsActiveServiceList()
/** Each test step initialises it's own name
 */
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("EtelPcktMbmsCap7"));
	}
//------------------------------------------------------------------

TVerdict CEnumerateMbmsActiveServiceList::doTestStepL( void )
/**
 * Tests the functionality of EnumerateMbmsActiveServiceList of RPacketService.
 * 
 */
	{
	SetSimtsyTestNumberL(SIM_PACKET_MBMS_TESTNUMBER);
	TRequestStatus	reqStatus;
	TBuf<20> contextName = _L("");

	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);

	TInt currActiveServices, maxActiveServices;
	iPacketService.EnumerateMbmsActiveServiceList(reqStatus,currActiveServices, maxActiveServices);
	User::WaitForRequest(reqStatus);

	TESTL(reqStatus.Int() == KErrPermissionDenied);

	iPacketService.Close();
	iPhone.Close();

	return TestStepResult();
	}


//-------------------------------------------------------------------------

CEnumerateMbmsMonitorServiceList::CEnumerateMbmsMonitorServiceList()
/** Each test step initialises it's own name
 */
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("EtelPcktMbmsCap8"));
	}
//------------------------------------------------------------------

TVerdict CEnumerateMbmsMonitorServiceList::doTestStepL( void )
/**
 * Tests the functionality of EnumerateMbmsActiveServiceList of RPacketService.
 * 
 */
	{
	SetSimtsyTestNumberL(SIM_PACKET_MBMS_TESTNUMBER);
	TRequestStatus	reqStatus;
	TBuf<20> contextName = _L("");

	TESTL(iPhone.Open(iTelServer, KPhoneName)==KErrNone);
	TESTL(iPhone.Initialise()==KErrNone);
	TESTL(iPacketService.Open(iPhone ) == KErrNone);

	TInt currActiveServices, maxActiveServices;
	iPacketService.EnumerateMbmsActiveServiceList(reqStatus,currActiveServices, maxActiveServices);
	User::WaitForRequest(reqStatus);

	TESTL(reqStatus.Int() == KErrPermissionDenied);

	iPacketService.Close();
	iPhone.Close();

	return TestStepResult();
	}


/**
 *  Tests closing one of two handles that both have notifications on them.
 *  This can cause ETel to close multiple requests during the same cancel
 *  request, which in turn can panic ETel if the iterator gets corrupted.
 * 
 *  @return Test step verdict.
 */
TVerdict CTestMultipleCompletionWithClosingContext::doTestStepL()
    {
    //
    // Initialise and open the packet service...
    //
	SetSimtsyTestNumberL(2);
	
	TESTL(iPhone.Open(iTelServer, KPhoneName) == KErrNone);
	TESTL(iPhone.Initialise() == KErrNone);
    TESTL(iPacketService.Open(iPhone) == KErrNone);
	StartNTRasSimulation();
	
	//
	// Open the first context...
	//
    TRequestStatus  activateContext1Status;
	TBuf<50>        contextName;
	
    TESTL(iPacketContext[0].OpenNewContext(iPacketService, contextName) == KErrNone);
	iPacketContext[0].Activate(activateContext1Status);
	User::WaitForRequest(activateContext1Status);
    
	//
	// Open the second context...
	//
    TRequestStatus  activateContext2Status;

    TESTL(iPacketContext[1].OpenNewContext(iPacketService, contextName) == KErrNone);
	iPacketContext[1].Activate(activateContext2Status);
	User::WaitForRequest(activateContext2Status);

	//
	// Open another handle to the second context by name...
	//
    TESTL(iPacketContext[2].OpenExistingContext(iPacketService, _L("PRIMARY2")) == KErrNone);

    //
    // Post notifications on both handles for the second context...
    //
    TRequestStatus  notifyContext1Status, notifyContext2Status;
	RPacketContext::TContextStatus  contextStatus1, contextStatus2;

	iPacketContext[1].NotifyStatusChange(notifyContext1Status, contextStatus1);	
    iPacketContext[2].NotifyStatusChange(notifyContext2Status, contextStatus2);

    //
    // Delete second context using the first handle...
    //
    TRequestStatus   deleteContext2Status;

    iPacketContext[1].Delete(deleteContext2Status);
    User::WaitForRequest(deleteContext2Status);

    //
    // The notifications will have triggered and a request will have
    // been reposted to the TSY.
    //
    User::WaitForRequest(notifyContext1Status);
    User::WaitForRequest(notifyContext2Status);

    //
    // Deactivate the fist context...
    //
    TRequestStatus   deactivateContext1Status;
    
    iPacketContext[0].Deactivate(deactivateContext1Status);
    User::WaitForRequest(deactivateContext1Status);
    
    //
    // Close the contexts. Note this is closing iPacketContext[1] before
    // iPacketContext[2]! This forces the notification from the first handle
    // to move to the second handle, but there it errors as the context is
    // closed. This would cause ETel server to panic with KERN-EXEC:3.
    //
	iPacketContext[1].Close();
    iPacketContext[2].Close();
    iPacketContext[0].Close();
    iPacketService.Close();
	iPhone.Close();

	//
	// Now check that ETel is still up...
	//
	TEST(iPhone.Open(iTelServer, KPhoneName) == KErrNone);
	iPhone.Close();

	return TestStepResult();
	} //  CTestMultipleCompletionWithClosingContext::doTestStepL
