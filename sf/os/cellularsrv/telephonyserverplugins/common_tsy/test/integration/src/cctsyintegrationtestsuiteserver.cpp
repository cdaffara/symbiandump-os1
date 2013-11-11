// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The definition of the CTSY integration test suite server.
// 
//

/**
 @file
 @internalTechnology
*/
#include "e32property.h"
#include "cctsyintegrationtestsuiteserver.h"

#include "cctsyintegrationtestindicator.h"
#include "cctsyintegrationtestsmsstore.h"
#include "cctsyintegrationtestsignalstrength.h"
#include "cctsyintegrationtestsupplementaryservicecaps.h"
#include "cctsyintegrationtestphonecapability.h"
#include "cctsyintegrationtestalternatelineservice.h"
#include "cctsyintegrationtestapncontrollist.h"
#include "cctsyintegrationtestonstore.h"
#include "cctsyintegrationtestsmspstore.h"
#include "cctsyintegrationtestsubscriberinfo.h"
#include "cctsyintegrationtestmultibearerinteroperability.h"
#include "cctsyintegrationtestdatacallcontrol.h"
#include "cctsyintegrationtestcallcost.h"
#include "cctsyintegrationtestcallcontrol.h"
#include "cctsyintegrationtestmessagewaiting.h"
#include "cctsyintegrationtestphonebookstore.h"
#include "cctsyintegrationtestenstore.h"
#include "cctsyintegrationtestconferencecallcontrol.h"
#include "cctsyintegrationtestims.h"
#include "cctsyintegrationtestfax.h"
#include "cctsyintegrationtestussdmessaging.h"
#include "cctsyintegrationtestpacketservice.h"
#include "cctsyintegrationtestphoneidentity.h"
#include "cctsyintegrationtestmultimediacallcontrol.h"
#include "cctsyintegrationtestcustomapi.h"
#include "cctsyintegrationtestcallbarring.h"
#include "cctsyintegrationtestpacketcontext.h"
#include "cctsyintegrationtestairtimeduration.h"
#include "cctsyintegrationtestnetworksecurity.h"
#include "cctsyintegrationtestphonesecurity.h"
#include "cctsyintegrationtestcallownership.h"
#include "cctsyintegrationtestdtmf.h"
#include "cctsyintegrationtestbroadcastmessaging.h"
#include "cctsyintegrationtestprivacy.h"
#include "cctsyintegrationtestphonestore.h"
#include "cctsyintegrationtestcallcompletion.h"
#include "cctsyintegrationtestmailboxnumbers.h"
#include "cctsyintegrationtestcallemergency.h"
#include "cctsyintegrationtestsmsmessaging.h"
#include "cctsyintegrationtestphonecontrol.h"
#include "cctsyintegrationtestfixeddialling.h"
#include "cctsyintegrationtestpersonalisation.h"
#include "cctsyintegrationtestusernetworkaccess.h"
#include "cctsyintegrationtestcallforwarding.h"
#include "cctsyintegrationtestphonepower.h"
#include "cctsyintegrationtestnetworkcontrol.h"
#include "cctsyintegrationtestcallwaiting.h"
#include "cctsyintegrationtestusimapplication.h"

#include "cetelsessionmgr.h"

#include "cinidata.h"
#include <f32file.h>

#include "rpsmaster.h"
#include "rdebuglogger.h"

/** Path on C drive to INI files */
_LIT(KIniPath,"\\testdata\\configs");
/** Name of the global configuration file */
_LIT(KIniFile,"\\ctsyintegration_setup.ini");
/** init file entries */
_LIT(KIniTsyName,"TsyName");
_LIT(KIniPhoneName,"PhoneName");
_LIT(KIniKeepSessOpen,"KeepSessOpen");
_LIT(KIniUseRps,"UseRemotePartySimulator");

_LIT(KServerName,"TE_CTSYIntegrationTestSuite");

CCTSYIntegrationTestSuiteServer* CCTSYIntegrationTestSuiteServer::NewL()
/**
 * @return 	Instance of the test server.
 */
	{
	CCTSYIntegrationTestSuiteServer* server = new (ELeave) CCTSYIntegrationTestSuiteServer();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	
	CleanupStack::Pop(server);
	return server;
	}

void CCTSYIntegrationTestSuiteServer::ConstructL(const TDesC& aName)
/**
 * @param aName The test server name.
 */
	{
	DEBUG_PRINTF1(_L("CCTSYIntegrationTestSuiteServer::ConstructL - set test server"));
	CTestServer::ConstructL(aName);
	
	//read config file
	DEBUG_PRINTF1(_L("CCTSYIntegrationTestSuiteServer::ConstructL - About to read the config"));	
	ReadConfigL();
	DEBUG_PRINTF1(_L("CCTSYIntegrationTestSuiteServer::ConstructL - create CEtelSessionMgr"));	
	iSessManager = CEtelSessionMgr::NewL(iTsyName,iPhoneName,iKeepSessOpen);
	
	//Set session manager in own numbers manager for use later
	DEBUG_PRINTF1(_L("CCTSYIntegrationTestSuiteServer::ConstructL - set CEtelSessionMgr"));	
	iOwnNumbersManager.SetEtelSessMgr(iSessManager);

	//launch RPS
	if(iUseRps)
		{
		CreateRemotePartySimulatorL();
		}
	}

CCTSYIntegrationTestSuiteServer::~CCTSYIntegrationTestSuiteServer()
/**
 * Destructor.
 */
	{
	delete iSessManager;
	delete iRPS;
	}
	
void CCTSYIntegrationTestSuiteServer::ReadConfigL()
	{
	TDriveUnit cDrive(EDriveC);
	TDriveName driveName = cDrive.Name();

	RBuf fullPath;
	CleanupClosePushL(fullPath);
	fullPath.CreateL(driveName.Length() + KIniPath().Length() + KIniFile().Length());
	fullPath.Append(driveName);
	fullPath.Append(KIniPath);
	fullPath.Append(KIniFile);
	
	CIniData* iniData = CIniData::NewL(fullPath);
	CleanupStack::PushL(iniData);
	TPtrC data;
	if (iniData->FindVar(KIniTsyName,data))
		{
		iTsyName = data;
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	DEBUG_PRINTF2(_L("Config file: tsyname=%S"), &iTsyName);
	if (iniData->FindVar(KIniPhoneName,data))
		{
		iPhoneName = data;
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	DEBUG_PRINTF2(_L("Config file: phonename=%S"), &iPhoneName);
	if (!iniData->FindVar(KIniKeepSessOpen,iKeepSessOpen))
		{
		User::Leave(KErrNotFound);
		}
	DEBUG_PRINTF2(_L("Config file: keepsessopen=%d"), iKeepSessOpen);
	if (!iniData->FindVar(KIniUseRps,iUseRps))
		{
		User::Leave(KErrNotFound);
		}		
	DEBUG_PRINTF2(_L("Config file: userps=%d"), iUseRps);
	CleanupStack::PopAndDestroy(iniData);
	CleanupStack::PopAndDestroy(&fullPath);
	}
	
void CCTSYIntegrationTestSuiteServer::CreateRemotePartySimulatorL()
	{
	DEBUG_PRINTF1(_L("CCTSYIntegrationTestSuiteServer::CreateRemotePartySimulatorL"));
	TBool rpsPresent = EFalse;
	iRPS = CRPSMaster::NewL();
	DEBUG_PRINTF2(_L("CCTSYIntegrationTestSuiteServer::CreateRemotePartySimulatorL created 0x%x"), iRPS);
	if(iRPS)
		{
		iRPS->CheckRpsPresentL(rpsPresent);
		if(!rpsPresent)
			{
			//its not working so kill it
			delete iRPS;
			iRPS = NULL;
			DEBUG_PRINTF1(_L("CCTSYIntegrationTestSuiteServer::CreateRemotePartySimulatorL failed CheckRpsPresentL - leaving"));
			User::Leave(KErrBadHandle);
			}
		}
	}


LOCAL_C TInt MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CCTSYIntegrationTestSuiteServer* server = NULL;

	TRAPD(err,server = CCTSYIntegrationTestSuiteServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	return err;
	}

GLDEF_C TInt E32Main()
/**
 * @return 	Standard Epoc error code on process exit.
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


CTestStep* CCTSYIntegrationTestSuiteServer::CreateTestStep(const TDesC& aStepName)
/**
 * Implementation of CTestServer pure virtual.
 *
 * @return 	A CTestStep derived instance.
 * 
 */
	{
	DEBUG_PRINTF2(_L("CCTSYIntegrationTestSuiteServer::CreateTestStep aStepName = %S"), &aStepName);

	CTestStep* testStep = DoCreateTestStep(aStepName);
	
	if (testStep != NULL)
		{
		//ideally I would pass iRps in constructor of test step but I don't want to change all 700 constuctors so I do it by function call instead.
		static_cast<CCTSYIntegrationTestSuiteStepBase*>(testStep)->SetRpsHandle(iRPS);
		static_cast<CCTSYIntegrationTestSuiteStepBase*>(testStep)->SetOwnNumbersManager(&iOwnNumbersManager);
		}

	return testStep;
	}

CTestStep* CCTSYIntegrationTestSuiteServer::DoCreateTestStep(const TDesC& aStepName)
/**
 * Delegate function to create test step. The usual TEF "if/else if" is to complex for the
 * ARM compiler that crashes with a stack overflow. This implementation avoid that by
 * direcly returning the test step instance
 *
 * @return 	A CTestStep derived instance.
 * 
*/
	{
	if (aStepName == CCTSYIntegrationTestIndicator0001::GetTestStepName())
		return new CCTSYIntegrationTestIndicator0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestIndicator0002::GetTestStepName())
		return new CCTSYIntegrationTestIndicator0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestIndicator0003::GetTestStepName())
		return new CCTSYIntegrationTestIndicator0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0001::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0002::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0003::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0004::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0005::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0006::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0007::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0008::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0009::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0010::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0011::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0012::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0013::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0014::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0015::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0016::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0017::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0018::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0019::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0020::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0021::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0022::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSStore0023::GetTestStepName())
		return new CCTSYIntegrationTestSMSStore0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSignalStrength0001::GetTestStepName())
		return new CCTSYIntegrationTestSignalStrength0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSignalStrength0002::GetTestStepName())
		return new CCTSYIntegrationTestSignalStrength0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSupplementaryServiceCaps0001::GetTestStepName())
		return new CCTSYIntegrationTestSupplementaryServiceCaps0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneCapability0001::GetTestStepName())
		return new CCTSYIntegrationTestPhoneCapability0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneCapability0002::GetTestStepName())
		return new CCTSYIntegrationTestPhoneCapability0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAlternateLineService0001::GetTestStepName())
		return new CCTSYIntegrationTestAlternateLineService0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAlternateLineService0002::GetTestStepName())
		return new CCTSYIntegrationTestAlternateLineService0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAlternateLineService0003::GetTestStepName())
		return new CCTSYIntegrationTestAlternateLineService0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0001::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0002::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0003::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0004::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0005::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0006::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0007::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0008::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0009::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0010::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0011::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0012::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0013::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0014::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0015::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0016::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAPNControlList0017::GetTestStepName())
		return new CCTSYIntegrationTestAPNControlList0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0001::GetTestStepName())
		return new CCTSYIntegrationTestONStore0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0002::GetTestStepName())
		return new CCTSYIntegrationTestONStore0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0003::GetTestStepName())
		return new CCTSYIntegrationTestONStore0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0004::GetTestStepName())
		return new CCTSYIntegrationTestONStore0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0005::GetTestStepName())
		return new CCTSYIntegrationTestONStore0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0006::GetTestStepName())
		return new CCTSYIntegrationTestONStore0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0007::GetTestStepName())
		return new CCTSYIntegrationTestONStore0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0008::GetTestStepName())
		return new CCTSYIntegrationTestONStore0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0009::GetTestStepName())
		return new CCTSYIntegrationTestONStore0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0010::GetTestStepName())
		return new CCTSYIntegrationTestONStore0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0011::GetTestStepName())
		return new CCTSYIntegrationTestONStore0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0012::GetTestStepName())
		return new CCTSYIntegrationTestONStore0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0013::GetTestStepName())
		return new CCTSYIntegrationTestONStore0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0014::GetTestStepName())
		return new CCTSYIntegrationTestONStore0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0015::GetTestStepName())
		return new CCTSYIntegrationTestONStore0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0016::GetTestStepName())
		return new CCTSYIntegrationTestONStore0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0017::GetTestStepName())
		return new CCTSYIntegrationTestONStore0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0018::GetTestStepName())
		return new CCTSYIntegrationTestONStore0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0019::GetTestStepName())
		return new CCTSYIntegrationTestONStore0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0020::GetTestStepName())
		return new CCTSYIntegrationTestONStore0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0021::GetTestStepName())
		return new CCTSYIntegrationTestONStore0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0022::GetTestStepName())
		return new CCTSYIntegrationTestONStore0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0023::GetTestStepName())
		return new CCTSYIntegrationTestONStore0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0024::GetTestStepName())
		return new CCTSYIntegrationTestONStore0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0025::GetTestStepName())
		return new CCTSYIntegrationTestONStore0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0026::GetTestStepName())
		return new CCTSYIntegrationTestONStore0026(*iSessManager);

	if (aStepName == CCTSYIntegrationTestONStore0027::GetTestStepName())
		return new CCTSYIntegrationTestONStore0027(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSPStore0001::GetTestStepName())
		return new CCTSYIntegrationTestSMSPStore0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSPStore0002::GetTestStepName())
		return new CCTSYIntegrationTestSMSPStore0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSPStore0003::GetTestStepName())
		return new CCTSYIntegrationTestSMSPStore0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSPStore0004::GetTestStepName())
		return new CCTSYIntegrationTestSMSPStore0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSPStore0005::GetTestStepName())
		return new CCTSYIntegrationTestSMSPStore0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSPStore0006::GetTestStepName())
		return new CCTSYIntegrationTestSMSPStore0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSMSPStore0007::GetTestStepName())
		return new CCTSYIntegrationTestSMSPStore0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0001::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0002::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0003::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0004::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0005::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0006::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0007::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0008::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSubscriberInfo0009::GetTestStepName())
		return new CCTSYIntegrationTestSubscriberInfo0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultiBearerInteroperability0001::GetTestStepName())
		return new CCTSYIntegrationTestMultiBearerInteroperability0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultiBearerInteroperability0002::GetTestStepName())
		return new CCTSYIntegrationTestMultiBearerInteroperability0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultiBearerInteroperability0003::GetTestStepName())
		return new CCTSYIntegrationTestMultiBearerInteroperability0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultiBearerInteroperability0004::GetTestStepName())
		return new CCTSYIntegrationTestMultiBearerInteroperability0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultiBearerInteroperability0005::GetTestStepName())
		return new CCTSYIntegrationTestMultiBearerInteroperability0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0001::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0002::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0003::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0004::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0005::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0006::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0007::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0008::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0009::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0010::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDataCallControl0011::GetTestStepName())
		return new CCTSYIntegrationTestDataCallControl0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0001::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0002::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0003::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0004::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0005::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0006::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0007::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0008::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0009::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0010::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0011::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0012::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0013::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0014::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0015::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0016::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0017::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0018::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0019::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0020::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0021::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0022::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCost0023::GetTestStepName())
		return new CCTSYIntegrationTestCallCost0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0001::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0002::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0003::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0004::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0005::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0006::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0007::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0008::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0009::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0010::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0011::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0012::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0013::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0014::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0015::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0016::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0017::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0018::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0019::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0020::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0021::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0022::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0023::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0024::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0025::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0026::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0026(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0027::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0027(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0028::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0028(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0029::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0029(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0030::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0030(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0031::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0031(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0032::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0032(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0033::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0033(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0034::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0034(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0035::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0035(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0036::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0036(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0037::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0037(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0038::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0038(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0039::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0039(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0040::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0040(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0041::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0041(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0042::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0042(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0043::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0043(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0044::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0044(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0045::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0045(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0046::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0046(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0047::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0047(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0048::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0048(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0049::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0049(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0050::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0050(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0051::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0051(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0052::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0052(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0053::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0053(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0054::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0054(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0055::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0055(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0056::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0056(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0057::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0057(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallControl0058::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0058(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMessageWaiting0001::GetTestStepName())
		return new CCTSYIntegrationTestMessageWaiting0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMessageWaiting0002::GetTestStepName())
		return new CCTSYIntegrationTestMessageWaiting0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0001ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0001ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0001FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0001FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0001SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0001SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0001VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0001VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0001MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0001MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0001BDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0001BDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0002ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0002ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0002FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0002FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0002SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0002SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0002VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0002VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0002MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0002MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0003ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0003ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0003FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0003FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0003SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0003SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0003VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0003VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0003MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0003MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0004ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0004ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0004FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0004FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0004SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0004SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0004VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0004VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0004MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0004MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0005ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0005ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0005FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0005FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0005SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0005SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0005VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0005VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0005MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0005MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0006ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0006ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0006FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0006FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0006SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0006SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0006VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0006VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0006MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0006MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0007ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0007ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0007FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0007FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0007SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0007SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0007VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0007VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0007MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0007MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0008ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0008ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0008FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0008FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0008SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0008SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0008VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0008VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0008MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0008MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0009ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0009ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0009FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0009FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0009SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0009SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0009VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0009VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0009MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0009MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0010ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0010ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0010FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0010FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0010SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0010SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0010VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0010VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0010MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0010MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0011ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0011ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0011FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0011FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0011SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0011SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0011VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0011VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0011MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0011MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0012ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0012ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0012FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0012FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0012SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0012SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0012VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0012VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0012MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0012MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0013ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0013ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0013FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0013FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0013SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0013SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0013VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0013VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0013MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0013MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0014ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0014ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0014FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0014FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0014SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0014SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0014VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0014VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0014MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0014MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0015ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0015ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0015FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0015FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0015SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0015SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0015VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0015VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0015MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0015MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0016ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0016ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0016FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0016FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0016SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0016SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0016VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0016VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0016MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0016MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0017ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0017ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0017FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0017FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0017SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0017SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0017VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0017VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0017MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0017MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0018ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0018ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0018FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0018FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0018SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0018SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0018VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0018VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0018MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0018MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0019ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0019ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0019FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0019FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0019SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0019SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0019VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0019VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0019MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0019MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0020ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0020ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0020FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0020FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0020SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0020SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0020VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0020VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0020MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0020MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0021ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0021ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0021FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0021FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0021SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0021SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0021VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0021VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0021MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0021MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0022ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0022ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0022FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0022FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0022SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0022SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0022VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0022VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0022MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0022MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0023ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0023ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0023FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0023FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0023SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0023SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0023VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0023VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0023MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0023MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0024ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0024ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0024FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0024FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0024SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0024SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0024VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0024VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0024MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0024MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0025ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0025ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0025FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0025FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0025SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0025SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0025VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0025VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0025MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0025MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0026ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0026ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0026FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0026FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0026SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0026SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0026VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0026VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0026MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0026MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0027ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0027ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0027FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0027FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0027SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0027SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0027VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0027VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0027MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0027MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0028ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0028ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0028FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0028FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0028SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0028SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0028VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0028VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0028MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0028MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0029ADN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0029ADN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0029FDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0029FDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0029SDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0029SDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0029VMBX::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0029VMBX(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneBookStore0029MBDN::GetTestStepName())
		return new CCTSYIntegrationTestPhoneBookStore0029MBDN(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0001::GetTestStepName())
		return new CCTSYIntegrationTestENStore0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0002::GetTestStepName())
		return new CCTSYIntegrationTestENStore0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0003::GetTestStepName())
		return new CCTSYIntegrationTestENStore0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0004::GetTestStepName())
		return new CCTSYIntegrationTestENStore0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0005::GetTestStepName())
		return new CCTSYIntegrationTestENStore0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0006::GetTestStepName())
		return new CCTSYIntegrationTestENStore0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0007::GetTestStepName())
		return new CCTSYIntegrationTestENStore0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0008::GetTestStepName())
		return new CCTSYIntegrationTestENStore0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0009::GetTestStepName())
		return new CCTSYIntegrationTestENStore0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0010::GetTestStepName())
		return new CCTSYIntegrationTestENStore0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0011::GetTestStepName())
		return new CCTSYIntegrationTestENStore0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0012::GetTestStepName())
		return new CCTSYIntegrationTestENStore0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestENStore0013::GetTestStepName())
		return new CCTSYIntegrationTestENStore0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0001::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0002::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0003::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0004::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0005::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0006::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0007::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0008::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0009::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0010::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0011::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0012::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0013::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0014::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0015::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0016::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0017::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0018::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0019::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0020::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0021::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0022::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0023::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0024::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0025::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestConferenceCallControl0026::GetTestStepName())
		return new CCTSYIntegrationTestConferenceCallControl0026(*iSessManager);

	if (aStepName == CCTSYIntegrationTestIMS0001::GetTestStepName())
		return new CCTSYIntegrationTestIMS0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestFax0001::GetTestStepName())
		return new CCTSYIntegrationTestFax0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestFax0002::GetTestStepName())
		return new CCTSYIntegrationTestFax0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0001::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0002::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0003::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0004::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0005::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0006::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0007::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0008::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0009::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0010::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUssdMessaging0011::GetTestStepName())
		return new CCTSYIntegrationTestUssdMessaging0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0001::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0002::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0003::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0004::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0005::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0006::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0007::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0008::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0009::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0010::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0011::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0012::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0013::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0014::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0015::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0016::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0017::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0018::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0019::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0020::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0021::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0022::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0023::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0024::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0025::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0026::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0026(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0027::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0027(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0028::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0028(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0029::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0029(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketService0030::GetTestStepName())
		return new CCTSYIntegrationTestPacketService0030(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0001::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0002::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0003::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0004::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0005::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0006::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0007::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0008::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0009::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneIdentity0010::GetTestStepName())
		return new CCTSYIntegrationTestPhoneIdentity0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0001::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0002::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0003::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0004::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0005::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0006::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0007::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0008::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0009::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0010::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0011::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0012::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0013::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0014::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0015::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0016::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0017::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0018::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0019::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0020::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0021::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0022::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0023::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0024::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0025::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0026::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0026(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0027::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0027(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMultimediaCallControl0028::GetTestStepName())
		return new CCTSYIntegrationTestMultimediaCallControl0028(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0001::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0002::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0003::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0004::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0005::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0006::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0007::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0008::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0009::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0010::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0011::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0012::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0013::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0014::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0015::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0016::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0017::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0018::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0019::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0020::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0021::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0022::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0023::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0024::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0025::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0026::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0026(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0027::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0027(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0028::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0028(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0029::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0029(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0030::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0030(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0031::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0031(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0032::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0032(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0033::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0033(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0034::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0034(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0035::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0035(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0036::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0036(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0037::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0037(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0038::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0038(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0039::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0039(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0040::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0040(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0041::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0041(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0042::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0042(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0043::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0043(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0044::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0044(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0045::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0045(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0046::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0046(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0047::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0047(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0048::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0048(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0049::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0049(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0050::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0050(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0051::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0051(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0052::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0052(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0053::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0053(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0054::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0054(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0055::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0055(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0056::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0056(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0057::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0057(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0058::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0058(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0059::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0059(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0060::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0060(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0061::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0061(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0062::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0062(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0063::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0063(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0064::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0064(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0065::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0065(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0066::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0066(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0067::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0067(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0068::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0068(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0069::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0069(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0070::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0070(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0071::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0071(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0072::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0072(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0073::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0073(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0074::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0074(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCustomAPI0075::GetTestStepName())
		return new CCTSYIntegrationTestCustomAPI0075(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0001::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0002::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0003::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0004::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0005::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0006::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0007::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0008::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0009::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0010::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0011::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0012::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0013::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0014::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0015::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallBarring0016::GetTestStepName())
		return new CCTSYIntegrationTestCallBarring0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0001::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0002::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0003::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0004::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0005::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0006::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0007::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0008::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0009::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0010::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0011::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0012::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0013::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0014::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0015::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0016::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0017::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0018::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0019::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0020::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0021::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0022::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0023::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0024::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0025::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0026::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0026(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0027::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0027(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0028::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0028(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0029::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0029(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0030::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0030(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0031::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0031(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0032::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0032(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0033::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0033(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0034::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0034(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0035::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0035(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0036::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0036(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0037::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0037(*iSessManager);

	if (aStepName.Find(CCTSYIntegrationTestPacketContext0038::GetTestStepName()) == 0)
		return new CCTSYIntegrationTestPacketContext0038(*iSessManager,aStepName);

	if (aStepName.Find(CCTSYIntegrationTestPacketContext0039::GetTestStepName()) == 0)
		return new CCTSYIntegrationTestPacketContext0039(*iSessManager,aStepName);

	if (aStepName == CCTSYIntegrationTestPacketContext0040::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0040(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0041::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0041(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0042::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0042(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0043::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0043(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0044::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0044(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0045::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0045(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0046::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0046(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0047::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0047(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0048::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0048(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0049::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0049(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0050::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0050(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0051::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0051(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0052::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0052(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0053::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0053(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0054::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0054(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0055::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0055(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0056::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0056(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0057::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0057(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0058::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0058(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0059::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0059(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0060::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0060(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0061::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0061(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPacketContext0062::GetTestStepName())
		return new CCTSYIntegrationTestPacketContext0062(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAirTimeDuration0001::GetTestStepName())
		return new CCTSYIntegrationTestAirTimeDuration0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAirTimeDuration0002::GetTestStepName())
		return new CCTSYIntegrationTestAirTimeDuration0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAirTimeDuration0003::GetTestStepName())
		return new CCTSYIntegrationTestAirTimeDuration0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestAirTimeDuration0004::GetTestStepName())
		return new CCTSYIntegrationTestAirTimeDuration0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkSecurity0001::GetTestStepName())
		return new CCTSYIntegrationTestNetworkSecurity0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkSecurity0002::GetTestStepName())
		return new CCTSYIntegrationTestNetworkSecurity0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0001::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0002::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0003::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0004::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0005::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0006::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0007::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0008::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0009::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0010::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0011::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0012::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0013::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0014::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0015::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0016::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0017::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneSecurity0018::GetTestStepName())
		return new CCTSYIntegrationTestPhoneSecurity0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallOwnership0001::GetTestStepName())
		return new CCTSYIntegrationTestCallOwnership0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallOwnership0002::GetTestStepName())
		return new CCTSYIntegrationTestCallOwnership0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallOwnership0003::GetTestStepName())
		return new CCTSYIntegrationTestCallOwnership0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallOwnership0004::GetTestStepName())
		return new CCTSYIntegrationTestCallOwnership0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallOwnership0005::GetTestStepName())
		return new CCTSYIntegrationTestCallOwnership0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0001::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0002::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0003::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0004::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0005::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0006::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0007::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0008::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0009::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0010::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestDTMF0011::GetTestStepName())
		return new CCTSYIntegrationTestDTMF0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0001::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0002::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0003::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0004::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0005::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0006::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0007::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0008::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0009::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0010::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0011::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0012::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0013::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestBroadcastMessaging0014::GetTestStepName())
		return new CCTSYIntegrationTestBroadcastMessaging0014(*iSessManager);
		
	if (aStepName == CCTSYIntegrationTestPrivacy0001::GetTestStepName())
		return new CCTSYIntegrationTestPrivacy0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPrivacy0002::GetTestStepName())
		return new CCTSYIntegrationTestPrivacy0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPrivacy0003::GetTestStepName())
		return new CCTSYIntegrationTestPrivacy0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneStore0001::GetTestStepName())
		return new CCTSYIntegrationTestPhoneStore0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneStore0002::GetTestStepName())
		return new CCTSYIntegrationTestPhoneStore0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneStore0003::GetTestStepName())
		return new CCTSYIntegrationTestPhoneStore0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0001::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0002::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0003::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0004::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0005::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0006::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0007::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0008::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0009::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0010::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallCompletion0011::GetTestStepName())
		return new CCTSYIntegrationTestCallCompletion0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestMailboxNumbers0001::GetTestStepName())
		return new CCTSYIntegrationTestMailboxNumbers0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0001::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0002::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0003::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0004::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0005::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0006::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0007::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0008::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0009::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0010::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallEmergency0011::GetTestStepName())
		return new CCTSYIntegrationTestCallEmergency0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0001::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0002::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0003::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0004::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0005::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0006::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0007::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0008::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0009::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0010::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0011::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0012::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0013::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0014::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0015::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0016::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0017::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0018::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0019::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0020::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0021::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0022::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0023::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0024::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0025::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0026::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0026(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0027::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0027(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0028::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0028(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0029::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0029(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0030::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0030(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0031::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0031(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0032::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0032(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0033::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0033(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0034::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0034(*iSessManager);

	if (aStepName == CCTSYIntegrationTestSmsMessaging0035::GetTestStepName())
		return new CCTSYIntegrationTestSmsMessaging0035(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneControl0001::GetTestStepName())
		return new CCTSYIntegrationTestPhoneControl0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhoneControl0002::GetTestStepName())
		return new CCTSYIntegrationTestPhoneControl0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestFixedDialling0001::GetTestStepName())
		return new CCTSYIntegrationTestFixedDialling0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestFixedDialling0002::GetTestStepName())
		return new CCTSYIntegrationTestFixedDialling0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestFixedDialling0003::GetTestStepName())
		return new CCTSYIntegrationTestFixedDialling0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestFixedDialling0004::GetTestStepName())
		return new CCTSYIntegrationTestFixedDialling0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPersonalisation0001::GetTestStepName())
		return new CCTSYIntegrationTestPersonalisation0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestUserNetworkAccess0001::GetTestStepName())
		return new CCTSYIntegrationTestUserNetworkAccess0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0001::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0002::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0003::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0004::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0005::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0006::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0007::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0008::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0009::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0010::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0011::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0012::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0013::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0014::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0015::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallForwarding0016::GetTestStepName())
		return new CCTSYIntegrationTestCallForwarding0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhonePower0001::GetTestStepName())
		return new CCTSYIntegrationTestPhonePower0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestPhonePower0002::GetTestStepName())
		return new CCTSYIntegrationTestPhonePower0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0001::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0002::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0003::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0004::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0005::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0006::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0007::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0008::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0009::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0010::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0011::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0012::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0012(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0013::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0013(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0014::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0014(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0015::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0015(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0016::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0016(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0017::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0017(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0018::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0018(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0019::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0019(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0020::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0020(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0021::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0021(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0022::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0022(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0023::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0023(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0024::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0024(*iSessManager);

	if (aStepName == CCTSYIntegrationTestNetworkControl0025::GetTestStepName())
		return new CCTSYIntegrationTestNetworkControl0025(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0001::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0001(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0002::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0002(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0003::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0003(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0004::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0004(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0005::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0005(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0006::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0006(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0007::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0007(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0008::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0008(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0009::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0009(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0010::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0010(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0011::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0011(*iSessManager);

	if (aStepName == CCTSYIntegrationTestCallWaiting0012::GetTestStepName())
		return new CCTSYIntegrationTestCallWaiting0012(*iSessManager);
	
	if (aStepName == CCTSYIntegrationTestUSimApplication0001::GetTestStepName())
		return new CCTSYIntegrationTestUSimApplication0001(*iSessManager);	

	if (aStepName == CCTSYIntegrationTestCallControl0059::GetTestStepName())
		return new CCTSYIntegrationTestCallControl0059(*iSessManager);	

	if (aStepName == CCTSYIntegrationTestCallControl0060::GetTestStepName())
          return new CCTSYIntegrationTestCallControl0060(*iSessManager);  
		
	return NULL;
	}
	
