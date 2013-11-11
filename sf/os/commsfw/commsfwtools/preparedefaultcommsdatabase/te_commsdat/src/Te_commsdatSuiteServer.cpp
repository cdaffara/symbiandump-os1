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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
*/

#include "Te_commsdatSuiteServer.h"
#include "commsdatBasicFunctionalityTests.h"
#include "commsdatDatabaseOperationsTests.h"
#include "commsdatGenericRecordTests.h"
#include "commsdatValidationTests.h"
#include "commsdatUtilityFunctionTests.h"
#include "commsdatAttributeTests.h"
#include "commsdatNotificationTests.h"
#include "commsdatConcurrencyTests.h"
#include "commsdatSnapTests.h" 
#include "CommsDatCorruptionTests.h"
#include "commsdatWifiTests.h" 
#include "commsdatExtensionTests.h"
#include "commsdatMixedSnapValidationTests.h"
#include "commsdatAPPriorityTests.h"


CTe_commsdatSuite* CTe_commsdatSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_commsdatSuite * server = new (ELeave) CTe_commsdatSuite();
#ifndef __TOOLS2__
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	TParsePtrC serverName(RProcess().FileName());
	server->StartL(serverName.Name()); 
	CleanupStack::Pop(server);
#endif
	return server;
	}

#ifndef __TOOLS2__
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
	CTe_commsdatSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_commsdatSuite::NewL());
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
	err = err; // To avoid compiler warning (declared but never used)
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
#endif

CTestStep* CTe_commsdatSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
		CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
	  if(aStepName == KCommsdat1Step)
					testStep = new CCommsdat1Step();
	  else if(aStepName == KCommsdat2Step)
					testStep = new CCommsdat2Step();
	  else if(aStepName == KCommsdat3Step)
					testStep = new CCommsdat3Step();
	  else if(aStepName == KCommsdat4Step)
					testStep = new CCommsdat4Step();
	  else if(aStepName == KCommsdat5Step)
					testStep = new CCommsdat5Step();
	  else if(aStepName == KCommsdat6Step)
					testStep = new CCommsdat6Step();
	  else if(aStepName == KCommsdat7Step)
					testStep = new CCommsdat7Step();
	  else if(aStepName == KCommsdat8Step)
					testStep = new CCommsdat8Step();
	  else if(aStepName == KCommsdat9Step)
					testStep = new CCommsdat9Step();
	  else if(aStepName == KCommsdat10Step)
					testStep = new CCommsdat10Step();
	  else if(aStepName == KCommsdat11Step)
					testStep = new CCommsdat11Step();
	  else if(aStepName == KCommsdat12Step)
					testStep = new CCommsdat12Step();
	  else if(aStepName == KCommsdat13Step)
					testStep = new CCommsdat13Step();
	  else if(aStepName == KCommsdat14Step)
					testStep = new CCommsdat14Step();
	  else if(aStepName == KCommsdat15Step)
					testStep = new CCommsdat15Step();
	  else if(aStepName == KCommsdat16Step)
					testStep = new CCommsdat16Step();
	  else if(aStepName == KCommsdat17Step)
					testStep = new CCommsdat17Step();
	  else if(aStepName == KCommsdat18Step)
					testStep = new CCommsdat18Step();
	  else if(aStepName == KCommsdat19Step)
					testStep = new CCommsdat19Step();
	  else if(aStepName == KCommsdat20Step)
					testStep = new CCommsdat20Step();
	  else if(aStepName == KCommsdat21Step)
					testStep = new CCommsdat21Step();
	  else if(aStepName == KCommsdat22Step)
					testStep = new CCommsdat22Step();
	  else if(aStepName == KCommsdat23Step)
					testStep = new CCommsdat23Step();
	  else if(aStepName == KCommsdat24Step)
					testStep = new CCommsdat24Step();
	  else if(aStepName == KCommsdat25Step)
					testStep = new CCommsdat25Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat26Step)
					testStep = new CCommsdat26Step();
#endif
	  else if(aStepName == KCommsdat27Step)
					testStep = new CCommsdat27Step();
	  else if(aStepName == KCommsdat28Step)
					testStep = new CCommsdat28Step();
	  
	  else if(aStepName == KCommsdat29Step)
					testStep = new CCommsdat29Step();
 	  else if(aStepName == KCommsdat30Step)
 					testStep = new CCommsdat30Step();
 	  else if(aStepName == KCommsdat31Step)
 	  				testStep = new CCommsdat31Step();
 	  else if(aStepName == KCommsdat32Step)
 	  				testStep = new CCommsdat32Step();
 	 else if(aStepName == KCommsdat33Step)
 	   				testStep = new CCommsdat33Step();
     else if(aStepName == KCommsdat34Step)
 	  	  	  		testStep = new CCommsdat34Step();
     else if(aStepName == KCommsdat35Step)
 	  	  	  		testStep = new CCommsdat35Step();
	  
	  else if(aStepName == KCommsdat40Step)
					testStep = new CCommsdat40Step();
	  else if(aStepName == KCommsdat41Step)
					testStep = new CCommsdat41Step();
	  else if(aStepName == KCommsdat42Step)
					testStep = new CCommsdat42Step();
	  else if(aStepName == KCommsdat43Step)
					testStep = new CCommsdat43Step();
	  else if(aStepName == KCommsdat44Step)
					testStep = new CCommsdat44Step();
	  else if(aStepName == KCommsdat45Step)
					testStep = new CCommsdat45Step();
	  else if(aStepName == KCommsdat46Step)
					testStep = new CCommsdat46Step();
	  else if(aStepName == KCommsdat47Step)
					testStep = new CCommsdat47Step();
	  else if(aStepName == KCommsdat48Step)
					testStep = new CCommsdat48Step();
	  else if(aStepName == KCommsdat49Step)
					testStep = new CCommsdat49Step();
	  else if(aStepName == KCommsdat50Step)
					testStep = new CCommsdat50Step();
	  else if(aStepName == KCommsdat51Step)
					testStep = new CCommsdat51Step();
	  else if(aStepName == KCommsdat52Step)
					testStep = new CCommsdat52Step();
	  else if(aStepName == KCommsdat53Step)
					testStep = new CCommsdat53Step();	
	  else if(aStepName == KCommsdat54Step)
					testStep = new CCommsdat54Step();	
	  else if(aStepName == KCommsdat55Step)
					testStep = new CCommsdat55Step();	
	  else if(aStepName == KCommsdat56Step)
					testStep = new CCommsdat56Step();	
	  else if(aStepName == KCommsdat57Step)
					testStep = new CCommsdat57Step();	
	  else if(aStepName == KCommsdat58Step)
					testStep = new CCommsdat58Step();	
	  else if(aStepName == KCommsdat59Step)
					testStep = new CCommsdat59Step();	
	  else if(aStepName == KCommsdat60Step)
					testStep = new CCommsdat60Step();	
	  else if(aStepName == KCommsdat61Step)
					testStep = new CCommsdat61Step();	
	  else if(aStepName == KCommsdat62Step)
					testStep = new CCommsdat62Step();	
	  else if(aStepName == KCommsdat63Step)
					testStep = new CCommsdat63Step();	
	  else if(aStepName == KCommsdat64Step)
					testStep = new CCommsdat64Step();	
	  else if(aStepName == KCommsdat65Step)
					testStep = new CCommsdat65Step();	
	  else if(aStepName == KCommsdat66Step)
					testStep = new CCommsdat66Step();
  	  else if(aStepName == KCommsdat67Step)
				testStep = new CCommsdat67Step();
  	  else if(aStepName == KCommsdat68Step)
				testStep = new CCommsdat68Step();
  	  else if(aStepName == KCommsdat69Step)
				testStep = new CCommsdat69Step();
   	  else if(aStepName == KCommsdat70Step)
				testStep = new CCommsdat70Step();
  	  else if(aStepName == KCommsdat71Step)
				testStep = new CCommsdat71Step();
   	  else if(aStepName == KCommsdat72Step)
				testStep = new CCommsdat72Step();
  	  else if(aStepName == KCommsdat73Step)
				testStep = new CCommsdat73Step();
  	  else if(aStepName == KCommsdat80Step)
					testStep = new CCommsdat80Step();
	  else if(aStepName == KCommsdat81Step)
					testStep = new CCommsdat81Step();
	  else if(aStepName == KCommsdat82Step)
					testStep = new CCommsdat82Step();
	  else if(aStepName == KCommsdat83Step)
					testStep = new CCommsdat83Step();
	  else if(aStepName == KCommsdat84Step)
					testStep = new CCommsdat84Step();
	  else if(aStepName == KCommsdat85Step)
					testStep = new CCommsdat85Step();
	  else if(aStepName == KCommsdat86Step)
					testStep = new CCommsdat86Step();
	  else if(aStepName == KCommsdat87Step)
					testStep = new CCommsdat87Step();
	  else if(aStepName == KCommsdat88Step)
					testStep = new CCommsdat88Step();
	  else if(aStepName == KCommsdat89Step)
					testStep = new CCommsdat89Step();
      else if(aStepName == KCommsdat90Step)
			        testStep = new CCommsdat90Step();
	  else if(aStepName == KCommsdat91Step)
		  			testStep = new CCommsdat91Step();
	  else if(aStepName == KCommsdat92Step)
		  			testStep = new CCommsdat92Step();
	  else if(aStepName == KCommsdat100Step)
					testStep = new CCommsdat100Step();
	  else if(aStepName == KCommsdat101Step)
					testStep = new CCommsdat101Step();
	  else if(aStepName == KCommsdat102Step)
					testStep = new CCommsdat102Step();
	  else if(aStepName == KCommsdat103Step)
					testStep = new CCommsdat103Step();
	  else if(aStepName == KCommsdat104Step)
					testStep = new CCommsdat104Step();
	  else if(aStepName == KCommsdat105Step)
					testStep = new CCommsdat105Step();
	  else if(aStepName == KCommsdat106Step)
					testStep = new CCommsdat106Step();
	  else if(aStepName == KCommsdat107Step)
					testStep = new CCommsdat107Step();
#ifndef __TOOLS2__	  
	  else if(aStepName == KCommsdat115Step)
					testStep = new CCommsdat115Step();
	  else if(aStepName == KCommsdat116Step)
					testStep = new CCommsdat116Step();
	  else if(aStepName == KCommsdat117Step)
					testStep = new CCommsdat117Step();
	  else if(aStepName == KCommsdat118Step)
					testStep = new CCommsdat118Step();
#endif	  
	  else if(aStepName == KCommsdatInitAttributes)
					testStep = new CCommsdatInitAttributes();
	  else if(aStepName == KCommsdat130Step)
					testStep = new CCommsdat130Step();
	  else if(aStepName == KCommsdat131Step)
					testStep = new CCommsdat131Step();
	  else if(aStepName == KCommsdat132Step)
					testStep = new CCommsdat132Step();
	  else if(aStepName == KCommsdat133Step)
					testStep = new CCommsdat133Step();
	  else if(aStepName == KCommsdat134Step)
					testStep = new CCommsdat134Step();
	  else if(aStepName == KCommsdat135Step)
					testStep = new CCommsdat135Step();
	  else if(aStepName == KCommsdat136Step)
					testStep = new CCommsdat136Step();
	  else if(aStepName == KCommsdat137Step)
					testStep = new CCommsdat137Step();
	  else if(aStepName == KCommsdat138Step)
					testStep = new CCommsdat138Step();
	  else if(aStepName == KCommsdat139Step)
					testStep = new CCommsdat139Step();
	  else if(aStepName == KCommsdat140Step)
					testStep = new CCommsdat140Step();
	  else if(aStepName == KCommsdat141Step)
					testStep = new CCommsdat141Step();
	  else if(aStepName == KCommsdat142Step)
					testStep = new CCommsdat142Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat150Step)
					testStep = new CCommsdat150Step();
	  else if(aStepName == KCommsdat151Step)
					testStep = new CCommsdat151Step();
	  else if(aStepName == KCommsdat152Step)
					testStep = new CCommsdat152Step();
	  else if(aStepName == KCommsdat153Step)
					testStep = new CCommsdat153Step();	 
	  else if(aStepName == KCommsdat154Step)
					testStep = new CCommsdat154Step();	 
	  else if(aStepName == KCommsdat155Step)
					testStep = new CCommsdat155Step();	 
	  else if(aStepName == KCommsdat156Step)
					testStep = new CCommsdat156Step();	 
	  else if(aStepName == KCommsdat157Step)
					testStep = new CCommsdat157Step();	 
	  else if(aStepName == KCommsdat158Step)
					testStep = new CCommsdat158Step();	 
	  else if(aStepName == KCommsdat159Step)
					testStep = new CCommsdat159Step();
	  else if(aStepName == KCommsdat160Step)
					testStep = new CCommsdat160Step();
	  else if(aStepName == KCommsdat161Step)
					testStep = new CCommsdat161Step();
	  else if(aStepName == KCommsdat162Step)
					testStep = new CCommsdat162Step();
	  else if(aStepName == KCommsdat163Step)
					testStep = new CCommsdat163Step();
	  else if(aStepName == KCommsdat164Step)
					testStep = new CCommsdat164Step();
	  else if(aStepName == KCommsdat165Step)
					testStep = new CCommsdat165Step();
	  else if(aStepName == KCommsdat170Step)
					testStep = new CCommsdat170Step();	 
	  else if(aStepName == KCommsdat171Step)
					testStep = new CCommsdat171Step();	 
	  else if(aStepName == KCommsdat172Step)
					testStep = new CCommsdat172Step();	 	  
#endif
	  else if(aStepName == KCommsdat190Step)
					testStep = new CCommsdat190Step();
	  else if(aStepName == KCommsdat191Step)
					testStep = new CCommsdat191Step();
	  else if(aStepName == KCommsdat192Step)
					testStep = new CCommsdat192Step();
	  else if(aStepName == KCommsdat193Step)
					testStep = new CCommsdat193Step();
	  else if(aStepName == KCommsdat194Step)
					testStep = new CCommsdat194Step();
	  else if(aStepName == KCommsdat195Step)
					testStep = new CCommsdat195Step();
	  else if(aStepName == KCommsdat196Step)
					testStep = new CCommsdat196Step();
	  else if(aStepName == KCommsdat197Step)
					testStep = new CCommsdat197Step();
	  else if(aStepName == KCommsdat198Step)
					testStep = new CCommsdat198Step();
	  else if(aStepName == KCommsdat199Step)
					testStep = new CCommsdat199Step();
	  else if(aStepName == KCommsdat200Step)
					testStep = new CCommsdat200Step();
	  else if(aStepName == KCommsdat201Step)
					testStep = new CCommsdat201Step();
	  else if(aStepName == KCommsdat202Step)
					testStep = new CCommsdat202Step();
	  else if(aStepName == KCommsdat203Step)
					testStep = new CCommsdat203Step();
	  else if(aStepName == KCommsdat204Step)
					testStep = new CCommsdat204Step();
	  else if(aStepName == KCommsdat205Step)
					testStep = new CCommsdat205Step();
	  else if(aStepName == KCommsdat206Step)
					testStep = new CCommsdat206Step();
	  else if(aStepName == KCommsdat207Step)
					testStep = new CCommsdat207Step();
      else if(aStepName == KCommsdat208Step)
					testStep = new CCommsdat208Step();
  	  else if(aStepName == KCommsdat209Step)
					testStep = new CCommsdat209Step();
#ifndef __TOOLS2__
  	  else if(aStepName == KCommsdat210Step)
					testStep = new CCommsdat210Step();
  	  else if(aStepName == KCommsdat211Step)
					testStep = new CCommsdat211Step();
#endif
   	  else if(aStepName == KCommsdat212Step)
					testStep = new CCommsdat212Step();
   	  else if(aStepName == KCommsdat213Step)
					testStep = new CCommsdat213Step();
#ifndef __TOOLS2__
   	  else if(aStepName == KCommsdat214Step)
					testStep = new CCommsdat214Step();
   	  else if(aStepName == KCommsdat215Step)
					testStep = new CCommsdat215Step();
#endif

	  // Corruption tests
	  else if(aStepName == KCommsdat300Step)
					testStep = new CCommsdat300Step();
	  else if(aStepName == KCommsdat301Step)
					testStep = new CCommsdat301Step();
	  else if(aStepName == KCommsdat302Step)
					testStep = new CCommsdat302Step();
	  else if(aStepName == KCommsdat303Step)
					testStep = new CCommsdat303Step();
	  else if(aStepName == KCommsdat304Step)
					testStep = new CCommsdat304Step();
	  else if(aStepName == KCommsdat305Step)
					testStep = new CCommsdat305Step();
	  else if(aStepName == KCommsdat306Step)
					testStep = new CCommsdat306Step();
	  
	  //MAPPING - SNAP FIELD TESTS STEPS, AND RECORD TESTS
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat307Step)
	  					testStep = new CCommsdat307Step();
	  else if(aStepName == KCommsdat308Step)
	  					testStep = new CCommsdat308Step();
	  else if(aStepName == KCommsdat309Step)
	  					testStep = new CCommsdat309Step();
	  else if(aStepName == KCommsdat310Step)
	  	  				testStep = new CCommsdat310Step();
	  else if(aStepName == KCommsdat311Step)
	  	  				testStep = new CCommsdat311Step();
	  else if(aStepName == KCommsdat312Step)
	  	  				testStep = new CCommsdat312Step();
	  else if(aStepName == KCommsdat313Step)
	  	  	  			testStep = new CCommsdat313Step();
	  else if(aStepName == KCommsdat314Step)
	  	  	  			testStep = new CCommsdat314Step();
	  else if(aStepName == KCommsdat315Step)
	  	  	  			testStep = new CCommsdat315Step();
#endif
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	  else if(aStepName == KCommsdat316Step)
	  	  	  			testStep = new CCommsdat316Step();
	  else if(aStepName == KCommsdat317Step)
	  	  	  			testStep = new CCommsdat317Step();
	  else if(aStepName == KCommsdat318Step)
	  	  	  			testStep = new CCommsdat318Step();
	  else if(aStepName == KCommsdat319Step)
	  	  	  			testStep = new CCommsdat319Step();
	  else if(aStepName == KCommsdat320Step)
	  	  	  			testStep = new CCommsdat320Step();
	  else if(aStepName == KCommsdat321Step)
	  	  	  			testStep = new CCommsdat321Step();
	  else if(aStepName == KCommsdat322Step)
	  	  	  			testStep = new CCommsdat322Step();
	  else if(aStepName == KCommsdat323Step)
	  	  	  			testStep = new CCommsdat323Step();
	  else if(aStepName == KCommsdat324Step)
	  	  	  			testStep = new CCommsdat324Step();
	  else if(aStepName == KCommsdat325Step)
	  	  	  			testStep = new CCommsdat325Step();
	  else if(aStepName == KCommsdat326Step)
	  	  	  			testStep = new CCommsdat326Step();
	  else if(aStepName == KCommsdat327Step)
	  	  	  			testStep = new CCommsdat327Step();
	  else if(aStepName == KCommsdat328Step)
	  	  	  			testStep = new CCommsdat328Step();
	  else if(aStepName == KCommsdat329Step)
	  	  	  			testStep = new CCommsdat329Step();
	  else if(aStepName == KCommsdat330Step)
	  	  	  			testStep = new CCommsdat330Step();
	  else if(aStepName == KCommsdat331Step)
	  	  	  			testStep = new CCommsdat331Step();
	  else if(aStepName == KCommsdat3311Step)
	  	  	  			testStep = new CCommsdat3311Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat3312Step)
	  	  	  			testStep = new CCommsdat3312Step();
	  else if(aStepName == KCommsdat3313Step)
	  	  	  			testStep = new CCommsdat3313Step();
	  else if(aStepName == KCommsdat3314Step)
	  	  	  			testStep = new CCommsdat3314Step();
	  else if(aStepName == KCommsdat3315Step)
	  	  	  			testStep = new CCommsdat3315Step();
	  else if(aStepName == KCommsdat3316Step)
	  	  	  			testStep = new CCommsdat3316Step();
	  else if(aStepName == KCommsdat3317Step)
	  	  	  			testStep = new CCommsdat3317Step();
	  else if(aStepName == KCommsdat3318Step)
	  	  	  			testStep = new CCommsdat3318Step();
	  else if(aStepName == KCommsdat3319Step)
	  	  	  			testStep = new CCommsdat3319Step();
#endif
	  //VALIDATION - CONNPREF AND IAP FIELD TESTS STEPS, AND RECORD TESTS
	  
	  else if(aStepName == KCommsdat332Step)
	  	  	  			testStep = new CCommsdat332Step();
	  else if(aStepName == KCommsdat333Step)
	  	  	  			testStep = new CCommsdat333Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat334Step)
	  	  	  			testStep = new CCommsdat334Step();
#endif
	  else if(aStepName == KCommsdat335Step)
	  	  	  			testStep = new CCommsdat335Step();
	  else if(aStepName == KCommsdat336Step)
	  	  	  			testStep = new CCommsdat336Step();
	  else if(aStepName == KCommsdat337Step)
	  	  	  			testStep = new CCommsdat337Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat338Step)
	  	  	  			testStep = new CCommsdat338Step();
	  else if(aStepName == KCommsdat339Step)
	  	  	  			testStep = new CCommsdat339Step();
#endif
	  else if(aStepName == KCommsdat340Step)
	  	  	  			testStep = new CCommsdat340Step();
	  else if(aStepName == KCommsdat341Step)
	  	  	  			testStep = new CCommsdat341Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat342Step)
	  	  	  			testStep = new CCommsdat342Step();
	  else if(aStepName == KCommsdat343Step)
	  	  	  			testStep = new CCommsdat343Step();
	  else if(aStepName == KCommsdat344Step)
	  	  	  			testStep = new CCommsdat344Step();
#endif
	  else if(aStepName == KCommsdat345Step)
	  	  	  			testStep = new CCommsdat345Step();
	  else if(aStepName == KCommsdat346Step)
	  	  	  			testStep = new CCommsdat346Step();
	  else if(aStepName == KCommsdat347Step)
	  	  	  			testStep = new CCommsdat347Step();
	  else if(aStepName == KCommsdat348Step)
	  	  	  			testStep = new CCommsdat348Step();
	  else if(aStepName == KCommsdat349Step)
	  	  	  			testStep = new CCommsdat349Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat350Step)
	  	  	  			testStep = new CCommsdat350Step();
	  else if(aStepName == KCommsdat351Step)
	  	  	  			testStep = new CCommsdat351Step();
#endif
	  else if(aStepName == KCommsdat352Step)
	  	  	  			testStep = new CCommsdat352Step();
	  else if(aStepName == KCommsdat353Step)
	  	  	  			testStep = new CCommsdat353Step();
	  else if(aStepName == KCommsdat354Step)
	  	  	  			testStep = new CCommsdat354Step();
	  else if(aStepName == KCommsdat355Step)
	  	  	  			testStep = new CCommsdat355Step();
	  else if(aStepName == KCommsdat356Step)
	  	  	  			testStep = new CCommsdat356Step();
	  else if(aStepName == KCommsdat357Step)
	  	  	  			testStep = new CCommsdat357Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat358Step)
	  	  	  			testStep = new CCommsdat358Step();
	  else if(aStepName == KCommsdat359Step)
	  	  	  			testStep = new CCommsdat359Step();
	  else if(aStepName == KCommsdat360Step)
	  	  	  			testStep = new CCommsdat360Step();
#endif
	  else if(aStepName == KCommsdat361Step)
	  	  	  			testStep = new CCommsdat361Step();
	  else if(aStepName == KCommsdat362Step)
	  	  	  			testStep = new CCommsdat362Step();
	  else if(aStepName == KCommsdat363Step)
	  	  	  			testStep = new CCommsdat363Step();
	  else if(aStepName == KCommsdat364Step)
	  	  	  			testStep = new CCommsdat364Step();
	  else if(aStepName == KCommsdat365Step)
	  	  	  			testStep = new CCommsdat365Step();
	  else if(aStepName == KCommsdat366Step)
	  	  	  			testStep = new CCommsdat366Step();
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat367Step)
	  	  	  			testStep = new CCommsdat367Step();
#endif
	  else if(aStepName == KCommsdat368Step)
	  	  	  			testStep = new CCommsdat368Step();
	  else if(aStepName == KCommsdat369Step)
	  	  	  			testStep = new CCommsdat369Step();
	  else if(aStepName == KCommsdat370Step)
	  	  	  			testStep = new CCommsdat370Step();
	  else if(aStepName == KCommsdat371Step)
	  	  	  			testStep = new CCommsdat371Step();
	  else if(aStepName == KCommsdat372Step)
	  	  	  			testStep = new CCommsdat372Step();
#ifndef __TOOLS2__
#endif
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat380Step)
	  	  	  			testStep = new CCommsdat380Step();
	  else if(aStepName == KCommsdat381Step)
	  	  	  			testStep = new CCommsdat381Step();
	  else if(aStepName == KCommsdat382Step)
	  	  	  			testStep = new CCommsdat382Step();
	  else if(aStepName == KCommsdat383Step)
	  	  	  			testStep = new CCommsdat383Step();
	  else if(aStepName == KCommsdat384Step)
	  	  	  			testStep = new CCommsdat384Step();
	  else if(aStepName == KCommsdat385Step)
		                testStep = new CCommsdat385Step();

#endif	  
	  //mixed cases for testing when there is an IAPPrioritySelPol record and a
	  //ConnPref record in the config file
#ifndef __TOOLS2__
	  else if(aStepName == KCommsdat400Step)
	  	  	  			testStep = new CCommsdat400Step();
	  else if(aStepName == KCommsdat401Step)
	  	  	  			testStep = new CCommsdat401Step();
	  else if(aStepName == KCommsdat402Step)
	  	  	  			testStep = new CCommsdat402Step();
	  else if(aStepName == KCommsdat403Step)
	  	  	  			testStep = new CCommsdat403Step();
	  else if(aStepName == KCommsdat404Step)
	  	  	  			testStep = new CCommsdat404Step();
	  else if(aStepName == KCommsdat405Step)
	  	  	  			testStep = new CCommsdat405Step();
	  else if(aStepName == KCommsdat406Step)
	  	  	  			testStep = new CCommsdat406Step();
	  else if(aStepName == KCommsdat407Step)
	  	  	  			testStep = new CCommsdat407Step();
	  else if(aStepName == KCommsdat408Step)
	  	  	  			testStep = new CCommsdat408Step();
	  else if(aStepName == KCommsdat409Step)
	  	  	  			testStep = new CCommsdat409Step();
#endif	  

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	  // APPriority tests
	  else if(aStepName == KCommsdat500Step)
	  	  	  			testStep = new CCommsdat500Step();
#ifndef __TOOLS2__	  
	  else if(aStepName == KCommsdat501Step)
	  	  	  			testStep = new CCommsdat501Step();
#endif // __TOOLS2__

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	  return testStep;
	}
