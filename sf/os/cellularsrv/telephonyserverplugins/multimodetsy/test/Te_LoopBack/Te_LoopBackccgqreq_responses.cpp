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
//

#include <e32test.h>
#include <etelmm.h>					// for Etel MultiMode API
#include <etelpckt.h>				// for RPacketService and other GPRS classes

#include "Te_LoopBackccgqreq_responses.h"		// Local header for this source file

#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveCGQREQResponses* CTestDriveCGQREQResponses::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveCGQREQResponses* self=new(ELeave) CTestDriveCGQREQResponses(aScriptListEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTestDriveCGQREQResponses::CTestDriveCGQREQResponses(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveCGQREQResponses::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveCGQREQResponses::DriveETelApiL()
/**
 * This test is to increase the conditional coverage of our test code.
 * This test runs the intiailisation sequence multiple times, each time trying 
 * a different variant of the +CGQREQ: response.
 * At the end of each initialisation sequence our scirpt causes a timeout to force the TSY
 * into thinking the iPhone has not been initialised and thus will try to initiailise the iPhone
 * the next time Intiailise() is called.
 * @return KErrNone
 */
	{
	// Open iPhone
	_LIT(KMmPhoneName,"GsmPhone1");
	TESTL(iCGQREQResponsesPhone.Open(iServer,KMmPhoneName)==KErrNone);
	
	//
	// Run thru the init tests validating the QoS caps that the TSY has each time
	for(TInt testNumber=1;testNumber<=19;++testNumber)
		{
		INFO_PRINTF2(_L("Running thru init sequence number %d\n"),testNumber);

		TRequestStatus	reqStatus;
		INFO_PRINTF1(_L("Initialising the Phone...\n"));

		RMobilePhone::TMMTableSettings tableSettings;
		tableSettings.iLocId=KInternetAccessPoint;
		RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
		iCGQREQResponsesPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	

		User::WaitForRequest(reqStatus);

		TESTL(reqStatus == KErrNone);

		// Verify that QoS capabilties of TSY match our script values
		switch(testNumber)
			{
		case 1:
			TestQosCapsL(RPacketQoS::EUnspecifiedPrecedence,RPacketQoS::EUnspecifiedDelayClass,
									RPacketQoS::EUnspecifiedReliabilityClass,RPacketQoS::EUnspecifiedPeakThroughput,
									RPacketQoS::EUnspecifiedMeanThroughput);
			break;
		case 2:
			TestQosCapsL(RPacketQoS::EPriorityHighPrecedence,RPacketQoS::EDelayClass1,
									RPacketQoS::EReliabilityClass1,RPacketQoS::EPeakThroughput1000,
									RPacketQoS::EMeanThroughput100);
			break;
		case 3:
			TestQosCapsL(RPacketQoS::EPriorityMediumPrecedence,RPacketQoS::EDelayClass2,
									RPacketQoS::EReliabilityClass2,RPacketQoS::EPeakThroughput2000,
									RPacketQoS::EMeanThroughput200);
			break;
		case 4:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass3,
									RPacketQoS::EReliabilityClass3,RPacketQoS::EPeakThroughput4000,
									RPacketQoS::EMeanThroughput500);
			break;
		case 5:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass4,RPacketQoS::EPeakThroughput8000,
									RPacketQoS::EMeanThroughput1000);
			break;
		case 6:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput16000,
									RPacketQoS::EMeanThroughput2000);
			break;
		case 7:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput32000,
									RPacketQoS::EMeanThroughput5000);
			break;
		case 8:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput64000,
									RPacketQoS::EMeanThroughput10000);
			break;
		case 9:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput128000,
									RPacketQoS::EMeanThroughput20000);
			break;
		case 10:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput50000);
			break;
		case 11:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput100000);
			break;
		case 12:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput200000);
			break;
		case 13:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput500000);
			break;
		case 14:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput1000000);
			break;
		case 15:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput2000000);
			break;
		case 16:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput5000000);
			break;
		case 17:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput10000000);
			break;
		case 18:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput20000000);
			break;
		case 19:
			TestQosCapsL(RPacketQoS::EPriorityLowPrecedence,RPacketQoS::EDelayClass4,
									RPacketQoS::EReliabilityClass5,RPacketQoS::EPeakThroughput256000,
									RPacketQoS::EMeanThroughput50000000);
			break;
			}

		// Do something which will make the TSY think the iPhone is not initialised.
		{
		TRequestStatus status(KErrNone);
		RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
		TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1> networkInfoPckg(networkInfo);
		RMobilePhone::TMobilePhoneLocationAreaV1 area;
		iCGQREQResponsesPhone.GetCurrentNetwork(status,networkInfoPckg,area);
		User::WaitForRequest(status);
		TESTL(status==KErrTimedOut);
		}
	}

	// Close iPhone and we are done
	iCGQREQResponsesPhone.Close();		
	return KErrNone;
	}


void CTestDriveCGQREQResponses::TestQosCapsL(RPacketQoS::TQoSPrecedence aExpectedPrecedence,
																			      RPacketQoS::TQoSDelay aExpectedDelay,
									                          RPacketQoS::TQoSReliability aExpectedReliability,
									                          RPacketQoS::TQoSPeakThroughput aExpectedPeakThroughput,
									                          RPacketQoS::TQoSMeanThroughput aExpectedMeanThroughput)
/**
 * Utility funciton which checks if the TSY as the expected QoS capabilities.
 */
 	{
	RPacketService service;
	RPacketContext context;
  TBuf<128> contextName;
	RPacketQoS qos;
  TBuf<128> qosName;

	TInt ret=service.Open(iCGQREQResponsesPhone);

	TESTL(ret==KErrNone);
	ret=context.OpenNewContext(service,contextName);
	TESTL(ret==KErrNone);
	ret=qos.OpenNewQoS(context,qosName);
	TESTL(ret==KErrNone);

	TRequestStatus status(KErrNone);
	RPacketQoS::TQoSCapsGPRS qosCaps;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsPckg(qosCaps);

	qos.GetProfileCapabilities(status,qosCapsPckg);
	User::WaitForRequest(status);
	TESTL(status==KErrNone);
	TESTL(qosCaps.iPrecedence==static_cast<TUint>(aExpectedPrecedence));
	TESTL(qosCaps.iDelay==static_cast<TUint>(aExpectedDelay));
	TESTL(qosCaps.iReliability==static_cast<TUint>(aExpectedReliability));
	TESTL(qosCaps.iPeak==static_cast<TUint>(aExpectedPeakThroughput));
	TESTL(qosCaps.iMean==static_cast<TUint>(aExpectedMeanThroughput));
	
	qos.Close();
	context.Close();
	service.Close();
	}



//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestCGQREQResponses* CTestCGQREQResponses::NewL(const TScript* aScript)
	{
	CTestCGQREQResponses* cmgf=new(ELeave) CTestCGQREQResponses(aScript);
	CleanupStack::PushL(cmgf);
	cmgf->ConstructL();
	CleanupStack::Pop();
	return cmgf;
	}

CTestCGQREQResponses::CTestCGQREQResponses(const TScript* aScript) : iScript(aScript)
	{}

void CTestCGQREQResponses::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestCGQREQResponses::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestCGQREQResponses::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestCGQREQResponses::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
