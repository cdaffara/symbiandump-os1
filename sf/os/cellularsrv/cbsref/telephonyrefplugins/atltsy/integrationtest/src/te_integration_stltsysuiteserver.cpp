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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file Te_integration_stltsySuiteServer.cpp
 @internalTechnology
*/

#include "te_integration_stltsysuiteserver.h"
#include "testltsysmscontrolreceivesmscase1step.h"
#include "testltsysmscontrolsendsmscase1step.h"
#include "testltsycallcontrolbase.h"
#include "testltsycallcontroldialhangupcase.h"
#include "testltsycallcontrolnotifyincomingcallcase1.h"
#include "testltsycallcontrolnotifyincomingcallcase2.h"
#include "testltsycallcontrolnotifyincomingcallcase3.h"
#include "testltsycallcontrolswapcalls.h"
#include "testltsycallcontrolcustomapiterminateallcalls.h"
#include "testltsycallcontrolphoneterminateallcalls.h"
#include "testltsycallcontroldialemergencycall.h"
#include "testltsycallcontroldialbadcall.h"
#include "testltsycallcontroldialselfcall.h"
#include "testltsycallcontroldialemergencycallstopallcalls.h"

#include "testltsyopenphone.h"
#include "testltsyclosephone.h"
#include "testltsyclosephone.h"
#include "testltsyopenphone.h"
#include "testltsygetphonebatteryinfo.h"
#include "testltsygetphonesignalstrength.h"
#include "testltsygetphonebatterycap.h"
#include "testltsygetphonesignalcap.h"
#include "testltsyphonegetfdnstatus.h"
#include "testltsyphonegetnetworkregistrationstatus.h"
#include "testltsyphonegetphoneid.h"
#include "testltsyphonegetdetectednetworks.h"
#include "testltsyphonegetdetectednetworkscancel.h"
#include "testltsyphonesetnetworkselectionsetting.h"
#include "testltsyphoneselectnetwork.h"
#include "testltsyphonegethomenetwork.h"
#include "testltsyphonegetcurrentnetworkinfo.h"
#include "testltsyphonegetcellinfo.h"
#include "testltsyphoneregistercellinfochangenotification.h"


#include "testltsypktopenpacket.h"
#include "testltsypktclosepacket.h"
#include "testltsypktopencontext.h"
#include "testltsypktinitialisecontext.h"
#include "testltsypktclosecontext.h"
#include "testltsypktattachnetwork.h"
#include "testltsypktdetachnetwork.h"
#include "testltsypktactivatecontext.h"
#include "testltsypktdeactivatecontext.h"
#include "testltsypktdeletecontext.h"
#include "testltsypktgetservicestatus.h"
#include "testltsypktgetnwkregistrationstatus.h"
#include "testltsypktgetcontextconfig.h"
#include "testltsypktgetdefaultparams.h"
#include "testltsypktsetdefaultparams.h"
#include "testltsypktgetcontextinfo.h"
#include "testltsypktsetcontextconfig.h"
#include "testltsypktopencontextqos.h"
#include "testltsypktclosecontextqos.h"
#include "testltsypktgetcontextqosprofile.h"
#include "testltsypktsetcontextqosprofile.h"
#include "testltsypktgetstaticcapabilities.h"
#include "testltsypktgetdynamiccapabilities.h"

#include "testltsyopensms.h"
#include "testltsysmssend.h"
#include "testltsyopensmsstore.h"
#include "testltsyclosesms.h"
#include "testltsyclosesmsstore.h"
#include "testltsygetmessagestoreinfo.h"
#include "testltsysmsstoregetinfo.h"
#include "testltsysmsack.h"
#include "testltsysmsnack.h"
#include "testltsysmsreceive.h"
#include "testltsysmssendinvalidparam.h"
#include "testltsysmsread.h"
#include "testltsysmsreadinvalidindex.h"
#include "testltsysmsdelete.h"
#include "testltsysmsdeleteinvalidindex.h"
#include "testltsysmsdeleteall.h"
#include "testltsysmsstore.h"
#include "testltsysmswrite.h"
#include "testltsysmswriteinvalidparam.h"

#include "testltsyphbkopen.h"
#include "testltsyphbkclose.h"
#include "testltsyphbkgetstoreinfo.h"
#include "testltsyphbkdelete.h"
#include "testltsyphbkdeleteinvalidindex.h"
#include "testltsyphbkwrite.h"
#include "testltsyphbkwriteinvalidindex.h"
#include "testltsyphbkread.h"
#include "testltsyphbkreadinvalidindex.h"

_LIT(KServerName,"Te_integration_stltsySuite");
CTe_integration_stltsySuite* CTe_integration_stltsySuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_integration_stltsySuite * server = new (ELeave) CTe_integration_stltsySuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
CTe_integration_stltsySuite::CTe_integration_stltsySuite()
:	CTestServer()
,	iSharedData(NULL)
/**
Constructor
*/
	{
	}

void CTe_integration_stltsySuite::ConstructL(const TDesC& aName)
/**
  Constructs a CTestTelephonySharedData ojbect to 
  hold the shared data (CTelephony object)
*/
	{
	CTestServer::ConstructL(aName);
	iSharedData = CTestLtsySharedData::NewL();
	iTestModel = CTestLtsyModel::NewL();
	}

CTe_integration_stltsySuite::~CTe_integration_stltsySuite()
/**
  Destructor deleting the CTestTelephonySharedData ojbect. 
*/
	{
	delete iSharedData;
	delete iTestModel;
	}

// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_integration_stltsySuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_integration_stltsySuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
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


CTestStep* CTe_integration_stltsySuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	  if(aStepName == KCTestLtsySmsControlReceiveSmsCase1Step)
	      testStep = new CCTestLtsySmsControlReceiveSmsCase1Step();
	  else if(aStepName == KCTestLtsySmsControlSendSmsCase1Step)
	      testStep = new CCTestLtsySmsControlSendSmsCase1Step();
	  else if(aStepName==KTestLtsyOpenPhone)
		  testStep = new CTestLtsyOpenPhone(*iTestModel);
	  else if(aStepName==KTestLtsyClosePhone)
	      testStep = new CTestLtsyClosePhone(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneGetFdnStatus)
	      testStep = new CTestLtsyPhoneGetFdnStatus(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneGetNetworkRegistrationStatus)
	      testStep = new CTestLtsyPhoneGetNetworkRegistrationStatus(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneGetPhoneID)
	      testStep = new CTestLtsyPhoneGetPhoneID(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneGetDetectedNetworks)
	      testStep = new CTestLtsyPhoneGetDetectedNetworks(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneGetDetectedNetworksCancelCancel)
	      testStep = new CTestLtsyPhoneGetDetectedNetworksCancelCancel(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneSetNetworkSelectionSetting)
	      testStep = new CTestLtsyPhoneSetNetworkSelectionSetting(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneSelectNetwork)
	      testStep = new CTestLtsyPhoneSelectNetwork(*iTestModel);
	  else if(aStepName==KTestLtsyGetPhoneBatteryInfo)
	      testStep = new CTestLtsyGetPhoneBatteryInfo(*iTestModel);
	  else if(aStepName==KTestLtsyGetPhoneSingalStrength)
	      testStep = new CTestLtsyGetPhoneSignalStrength(*iTestModel);
	  else if(aStepName==KTestLtsyGetPhoneBatteryCap)
	      testStep = new CTestLtsyGetPhoneBatteryCap(*iTestModel);
	  else if(aStepName==KTestLtsyGetPhoneSingalCap)
	      testStep = new CTestLtsyGetPhoneSignalCap(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneGetHomeNetwork)
	      testStep = new CTestLtsyPhoneGetHomeNetwork(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneGetCurrentNetworkInfo)
	      testStep = new CTestLtsyPhoneGetCurrentNetworkInfo(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneRegisterCellInfoChangeNotification)
	      testStep = new CTestLtsyPhoneRegisterCellInfoChangeNotification(*iTestModel);
	  else if(aStepName==KTestLtsyPhoneGetCellInfo)
	      testStep = new CTestLtsyPhoneGetCellInfo(*iTestModel);
	  else if(aStepName==KTestLtsyPktActivateContext)
	      testStep = new CTestLtsyPktActivateContext(*iTestModel);
	  else if(aStepName==KTestLtsyPktAttachNetwork)
	      testStep = new CTestLtsyPktAttachNetwork(*iTestModel);
	  else if(aStepName==KTestLtsyPktCloseContext)
	      testStep = new CTestLtsyPktCloseContext(*iTestModel);
	  else if(aStepName==KTestLtsyPktCloseContextQos)
	      testStep = new CTestLtsyPktCloseContextQos(*iTestModel);
	  else if(aStepName==KTestLtsyPktClosePacket)
	      testStep = new CTestLtsyPktClosePacket(*iTestModel);
	  else if(aStepName==KTestLtsyPktDeactivateContext)
	      testStep = new CTestLtsyPktDeactivateContext(*iTestModel);
	  else if(aStepName==KTestLtsyPktDeleteContext)
	      testStep = new CTestLtsyPktDeleteContext(*iTestModel);
	  else if(aStepName==KTestLtsyPktDetachNetwork)
	      testStep = new CTestLtsyPktDetachNetwork(*iTestModel);
	  else if(aStepName==KTestLtsyPktGetContextConfig)
	      testStep = new CTestLtsyPktGetContextConfig(*iTestModel);
	  else if(aStepName==KTestLtsyPktGetContextInfo)
	      testStep = new CTestLtsyPktGetContextInfo(*iTestModel);
	  else if(aStepName==KTestLtsyPktGetContextQoSProfile)
	      testStep = new CTestLtsyPktGetContextQoSProfile(*iTestModel);
	  else if(aStepName==KTestLtsyPktGetDefaultParams)
	      testStep = new CTestLtsyPktGetDefaultParams(*iTestModel);
	  else if(aStepName==KTestLtsyPktNetworkRegistrationStatus)
	      testStep = new CTestLtsyPktNetworkRegistrationStatus(*iTestModel);
	  else if(aStepName==KTestLtsyPktGetServiceStatust)
	      testStep = new CTestLtsyPktGetServiceStatus(*iTestModel);
	  else if(aStepName==KTestLtsyPktInitialiseContext)
	      testStep = new CTestLtsyPktInitialiseContext(*iTestModel);
	  else if(aStepName==KTestLtsyPktGetStaticCapabilities)
	      testStep = new CTestLtsyPktGetStaticCapabilities(*iTestModel);
	  else if(aStepName==KTestLtsyPktGetDynamicCapabilities)
	      testStep = new CTestLtsyPktGetDynamicCapabilities(*iTestModel);
	  else if(aStepName==KTestLtsyPktOpenContext)
	      testStep = new CTestLtsyPktOpenContext(*iTestModel);
	  else if(aStepName==KTestLtsyPktOpenContextQos)
	      testStep = new CTestLtsyPktOpenContextQos(*iTestModel);
	  else if(aStepName==KTestLtsyPktOpenPacket)
	      testStep = new CTestLtsyPktOpenPacket(*iTestModel);
	  else if(aStepName==KTestLtsyPktSetContextConfig)
	      testStep = new CTestLtsyPktSetContextConfig(*iTestModel);
	  else if(aStepName==KTestLtsyPktSetContextQoSProfile)
	      testStep = new CTestLtsyPktSetContextQoSProfile(*iTestModel);
	  else if(aStepName==KTestLtsyPktSetDefaultParams)
	      testStep = new CTestLtsyPktSetDefaultParams(*iTestModel);
     else if (aStepName == KTestLtsyCallControlNotifyIncomingCallCase1)
    	 testStep = new CTestLtsyCallControlNotifyIncomingCallCase1(*iTestModel);
     else if (aStepName == KTestLtsyCallControlDialHangupCase)
    	 testStep = new CTestLtsyCallControlDialHangupCase(*iTestModel);
     else if (aStepName == KTestLtsyCallControlNotifyIncomingCallCase2)
    	 testStep = new CTestLtsyCallControlNotifyIncomingCallCase2(*iTestModel);
     else if (aStepName == KTestLtsyCallControlNotifyIncomingCallCase3)
    	 testStep = new CTestLtsyCallControlNotifyIncomingCallCase3(*iTestModel);
     else if (aStepName == KTestLtsyCallControlSwapCalls)
    	 testStep = new CTestLtsyCallControlSwapCalls(*iTestModel);
     else if (aStepName == KTestLtsyCallControlCustomAPITerminateAllCalls)
    	 testStep = new CTestLtsyCallControlCustomAPITerminateAllCalls(*iTestModel);
     else if (aStepName == KTestLtsyCallControlPhoneTerminateAllCalls)
    	 testStep = new CTestLtsyCallControlPhoneTerminateAllCalls(*iTestModel);
     else if (aStepName == KTestLtsyCallControlDialEmergencyCall)
    	 testStep = new CTestLtsyCallControlDialEmergencyCall(*iTestModel);
     else if (aStepName == KTestLtsyCallControlBase)
    	 testStep = new CTestLtsyCallControlBase(*iTestModel);
     else if(aStepName == KCTestLtsyOpenSms) 
       	 testStep = new CTestLtsyOpenSms(*iTestModel);
        else if(aStepName == KCTestLtsyOpenSmsStore) 
       	 testStep = new CTestLtsyOpenSmsStore(*iTestModel);
        else if(aStepName == KCTestLtsySmsSend)
       	 testStep = new CTestLtsySmsSend(*iTestModel);	
        else if(aStepName == KCTestLtsyCloseSms) 
            testStep = new CTestLtsyCloseSms(*iTestModel);
        else if(aStepName == KCTestLtsyCloseSmsStore) 
       	 testStep = new CTestLtsyCloseSmsStore(*iTestModel);
        else if(aStepName == KCTestLtsyGetMessageStoreInfo) 
            testStep = new CTestLtsyGetMessageStoreInfo(*iTestModel);
        else if(aStepName == KCTestLtsySmsStoreGetInfo) 
       	 testStep = new CTestLtsySmsStoreGetInfo(*iTestModel); 
        else if(aStepName == KCTestLtsySmsReceive) 
            testStep = new CTestLtsySmsReceive(*iTestModel);
        else if(aStepName == KCTestLtsySmsAck) 
            testStep = new CTestLtsySmsAck(*iTestModel);
        else if(aStepName == KCTestLtsySmsNack) 
          	 testStep = new CTestLtsySmsNack(*iTestModel);  
        else if(aStepName == KCTestLtsySmsSendInvalidParam) 
            testStep = new CTestLtsySmsSendInvalidParam(*iTestModel);  
        else if(aStepName == KCTestLtsySmsRead) 
            testStep = new CTestLtsySmsRead(*iTestModel);
        else if(aStepName == KCTestLtsySmsReadInvalidIndex) 
         	 testStep = new CTestLtsySmsReadInvalidIndex(*iTestModel); 
        else if(aStepName == KCTestLtsySmsDelete) 
                testStep = new CTestLtsySmsDelete(*iTestModel);
        else if(aStepName == KCTestLtsySmsDeleteInvalidIndex) 
          	 testStep = new CTestLtsySmsDeleteInvalidIndex(*iTestModel);  
        else if(aStepName == KCTestLtsySmsDeleteAll) 
            testStep = new CTestLtsySmsDeleteAll(*iTestModel);  
        else if(aStepName == KCTestLtsySmsStore) 
            testStep = new CTestLtsySmsStore(*iTestModel);
        else if(aStepName == KCTestLtsySmsWrite) 
            testStep = new CTestLtsySmsWrite(*iTestModel);  
        else if(aStepName == KCTestLtsySmsWriteInvalidParam) 
            testStep = new CTestLtsySmsWriteInvalidParam(*iTestModel); 
        else if(aStepName == KCTestLtsyPhbkOpen) 
            testStep = new CTestLtsyPhbkOpen(*iTestModel);
        else if(aStepName == KCTestLtsyPhbkClose) 
            testStep = new CTestLtsyPhbkClose(*iTestModel); 
        else if(aStepName == KCTestLtsyPhbkGetStoreInfo) 
            testStep = new CTestLtsyPhbkGetStoreInfo(*iTestModel);  
        else if(aStepName == KCTestLtsyPhbkDelete) 
            testStep = new CTestLtsyPhbkDelete(*iTestModel); 
        else if(aStepName == KCTestLtsyPhbkDeleteInvalidIndex) 
            testStep = new CTestLtsyPhbkDeleteInvalidIndex(*iTestModel); 
        else if(aStepName == KCTestLtsyPhbkWrite) 
            testStep = new CTestLtsyPhbkWrite(*iTestModel); 
        else if(aStepName == KCTestLtsyPhbkWriteInvalidIndex) 
            testStep = new CTestLtsyPhbkWriteInvalidIndex(*iTestModel); 
        else if(aStepName == KCTestLtsyPhbkRead) 
            testStep = new CTestLtsyPhbkRead(*iTestModel); 
        else if(aStepName == KCTestLtsyPhbkReadInvalidIndex) 
            testStep = new CTestLtsyPhbkReadInvalidIndex(*iTestModel);   	 
        else if (aStepName == KTestLtsyCallControlDialBadCall)
    	   testStep = new CTestLtsyCallControlDialBadCall(*iTestModel);
        else if (aStepName == KTestLtsyCallControlDialSelfCall)
    	   testStep = new CTestLtsyCallControlDialSelfCall(*iTestModel);
        else if (aStepName == KTestLtsyCallControlDialEmergencyCallStopAllCall)
    	    testStep = new CTestLtsyCallControlDialEmergencyCallStopAllCall(*iTestModel);
	  
	return testStep;
	}
