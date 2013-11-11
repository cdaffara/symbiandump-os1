// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_LbsAssistanceDataSuiteServer.cpp
*/

#include "Te_LbsAssistanceDataSuiteServer.h"
#include "NavigationModelRootStep.h"
#include "NavModelSatInfoArrayStep.h"
#include "NavModelSatInfoEphemerisStep.h"
#include "NavModelSatInfoEphemParameterStep.h"
#include "NavModelSatInfoEphemSf1RsrvdArrayStep.h"
#include "NavModelSatInfoNodeParameterStep.h"
#include "NavModelSatInfoNodeStep.h"
#include "NavModelFinalCheckAndCleanupStep.h"
#include "GpsUtcModelRootStep.h"
#include "GpsUtcModelParamsStep.h"
#include "GpsUtcModelFinalCheckAndCleanupStep.h"
#include "ReferenceLocationRootStep.h"
#include "ReferenceLocationAltitudeStep.h"
#include "ReferenceLocationAltitudeParamsStep.h"
#include "ReferenceLocationFinalCheckAndCleanupStep.h"
#include "GpsIonosphericRootStep.h"
#include "GpsIonosphericParamsStep.h"
#include "GpsIonosphericModelFinalCheckAndCleanupStep.h"
#include "GpsAlmanacRootStep.h"
#include "GpsAlmanacParamsStep.h"
#include "GpsAlmanacSatInfoArrayStep.h"
#include "GpsAlmanacSatInfoNodeStep.h"
#include "GpsAlmanacSatInfoNodeParamsStep.h"
#include "AlmanacFinalCheckAndCleanupStep.h"
#include "BadSatListRootStep.h"
#include "BadSatLisArrayStep.h"
#include "GpsAcqAssistRootStep.h"
#include "GpsAcqAssistParamsStep.h"
#include "GpsAcqAssistSatInfoArrayStep.h"
#include "GpsAcqAssistSatInfoNodeStep.h"
#include "GpsAcqAssistSatInfoNodeParamsStep.h"
#include "GpsAcqAssistSatInfoNodeAzimuthParamsStep.h"
#include "GpsAcqAssistSatInfoNodeExtraDopplerParamsStep.h"
#include "GpsAcqAssistUtranGpsTimeStep.h"
#include "GpsAcqAssistUtranGpsTimeParamsStep.h"
#include "GpsAcqAssistUtranGpsTimeCpichStep.h"
#include "GpsAcqAssistRrlpGpsTimeParamsStep.h"
#include "GpsRefTimeRootStep.h"
#include "GpsRefTimeParamsStep.h"
#include "GpsRefTimeUtranGpsTimeStep.h"
#include "GpsRefTimeUtranGpsTimeParamsStep.h"
#include "GpsRefTimeUtranGpsTimeCpichStep.h"
#include "GpsRefTimeRrlpGpsTimeParamsStep.h"
#include "GpsRefTimeTowAssistArrayStep.h"
#include "GpsRefTimeTowAssistNodeStep.h"
#include "GpsRefTimeTowAssistNodeParamsStep.h"
#include "GpsRefTimeFinalCleanupStep.h"
#include "GpsAcqAssistFinalCleanupStep.h"
#include "AssistanceDataSetCreateStep.h"
#include "AssistanceDataSetMergeStep.h"
#include "AssistanceDataReaderSetStep.h"

_LIT(KServerName,"Te_LbsAssistanceDataSuite");
CTe_LbsAssistanceDataSuite* CTe_LbsAssistanceDataSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsAssistanceDataSuite * server = new (ELeave) CTe_LbsAssistanceDataSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
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
	CTe_LbsAssistanceDataSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsAssistanceDataSuite::NewL());
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

CTestStep* CTe_LbsAssistanceDataSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
              if(aStepName == KNavigationModelRootStep)
                            testStep = new CNavigationModelRootStep(*this);
              else if(aStepName == KNavModelSatInfoArrayStep)
                            testStep = new CNavModelSatInfoArrayStep(*this);
              else if(aStepName == KNavModelSatInfoEphemerisStep)
                            testStep = new CNavModelSatInfoEphemerisStep(*this);
              else if(aStepName == KNavModelSatInfoEphemParameterStep)
                           testStep = new CNavModelSatInfoEphemParameterStep(*this);
              else if(aStepName == KNavModelSatInfoNodeParameterStep)
                            testStep = new CNavModelSatInfoNodeParameterStep(*this);
              else if(aStepName == KNavModelSatInfoNodeStep)
                            testStep = new CNavModelSatInfoNodeStep(*this);
              else if(aStepName == KNavModelSatInfoEphemSf1RsrvdArrayStep)
                            testStep = new CNavModelSatInfoEphemSf1RsrvdArrayStep(*this);
              else if(aStepName == KNavModelFinalCheckAndCleanupStep)
                            testStep = new CNavModelFinalCheckAndCleanupStep(*this);
              else if(aStepName == KBadSatLisArrayStep)
                            testStep = new CBadSatLisArrayStep(*this);
              else if(aStepName == KBadSatListRootStep)
                            testStep = new CBadSatListRootStep(*this);
              else if(aStepName == KGpsAcqAssistFinalCleanupStep)
                            testStep = new CGpsAcqAssistFinalCleanupStep(*this);
             else if(aStepName == KGpsAcqAssistParamsStep)
                            testStep = new CGpsAcqAssistParamsStep(*this);
              else if(aStepName == KGpsAcqAssistRootStep)
                            testStep = new CGpsAcqAssistRootStep(*this);
              else if(aStepName == KGpsAcqAssistSatInfoArrayStep)
                            testStep = new CGpsAcqAssistSatInfoArrayStep(*this);
              else if(aStepName == KGpsAcqAssistSatInfoNodeAzimuthParamsStep)
                            testStep = new CGpsAcqAssistSatInfoNodeAzimuthParamsStep(*this);
              else if(aStepName == KGpsAcqAssistSatInfoNodeExtraDopplerParamsStep)
                            testStep = new CGpsAcqAssistSatInfoNodeExtraDopplerParamsStep(*this);
              else if(aStepName == KGpsAcqAssistSatInfoNodeParamsStep)
                            testStep = new CGpsAcqAssistSatInfoNodeParamsStep(*this);
              else if(aStepName == KGpsAcqAssistSatInfoNodeStep)
                            testStep = new CGpsAcqAssistSatInfoNodeStep(*this);            
              else if(aStepName == KGpsAcqAssistUtranGpsTimeCpichStep)
                            testStep = new CGpsAcqAssistUtranGpsTimeCpichStep(*this);
              else if(aStepName == KGpsAcqAssistUtranGpsTimeParamsStep)
                            testStep = new CGpsAcqAssistUtranGpsTimeParamsStep(*this);
              else if(aStepName == KGpsAcqAssistUtranGpsTimeStep)
                            testStep = new CGpsAcqAssistUtranGpsTimeStep(*this);
              else if(aStepName == KGpsAcqAssistRrlpGpsTimeParamsStep)
                            testStep = new CGpsAcqAssistRrlpGpsTimeParamsStep(*this);
              else if(aStepName == KAlmanacFinalCheckAndCleanupStep)
                            testStep = new CAlmanacFinalCheckAndCleanupStep(*this);
              else if(aStepName == KGpsAlmanacSatInfoNodeStep)
                            testStep = new CGpsAlmanacSatInfoNodeStep(*this);
              else if(aStepName == KGpsAlmanacParamsStep)
                            testStep = new CGpsAlmanacParamsStep(*this);
              else if(aStepName == KGpsAlmanacRootStep)
                            testStep = new CGpsAlmanacRootStep(*this);
              else if(aStepName == KGpsAlmanacSatInfoArrayStep)
                            testStep = new CGpsAlmanacSatInfoArrayStep(*this);
              else if(aStepName == KGpsAlmanacSatInfoNodeParamsStep)
                            testStep = new CGpsAlmanacSatInfoNodeParamsStep(*this);
              else if(aStepName == KGpsIonosphericParamsStep)
                           testStep = new CGpsIonosphericParamsStep(*this);
              else if(aStepName == KGpsIonosphericRootStep)
                            testStep = new CGpsIonosphericRootStep(*this);
              else if(aStepName == KGpsIonosphericModelFinalCheckAndCleanupStep)
                            testStep = new CGpsIonosphericModelFinalCheckAndCleanupStep(*this);                
              else if(aStepName == KGpsRefTimeParamsStep)
                            testStep = new CGpsRefTimeParamsStep(*this);
              else if(aStepName == KGpsRefTimeRootStep)
                            testStep = new CGpsRefTimeRootStep(*this);
              else if(aStepName == KGpsRefTimeFinalCleanupStep)
                            testStep = new CGpsRefTimeFinalCleanupStep(*this);              
              else if(aStepName == KGpsRefTimeTowAssistArrayStep)
                            testStep = new CGpsRefTimeTowAssistArrayStep(*this);
              else if(aStepName == KGpsRefTimeTowAssistNodeParamsStep)
                            testStep = new CGpsRefTimeTowAssistNodeParamsStep(*this);
              else if(aStepName == KGpsRefTimeTowAssistNodeStep)
                            testStep = new CGpsRefTimeTowAssistNodeStep(*this);
              else if(aStepName == KGpsRefTimeUtranGpsTimeCpichStep)
                            testStep = new CGpsRefTimeUtranGpsTimeCpichStep(*this);
              else if(aStepName == KGpsRefTimeUtranGpsTimeParamsStep)
                            testStep = new CGpsRefTimeUtranGpsTimeParamsStep(*this);
              else if(aStepName == KGpsRefTimeUtranGpsTimeStep)
                            testStep = new CGpsRefTimeUtranGpsTimeStep(*this);
              else if(aStepName == KGpsRefTimeRrlpGpsTimeParamsStep)
                            testStep = new CGpsRefTimeRrlpGpsTimeParamsStep(*this);
              else if(aStepName == KGpsUtcModelParamsStep)
                            testStep = new CGpsUtcModelParamsStep(*this);
              else if(aStepName == KGpsUtcModelRootStep)
                            testStep = new CGpsUtcModelRootStep(*this);
              else if(aStepName == KGpsUtcModelFinalCheckAndCleanupStep)
                            testStep = new CGpsUtcModelFinalCheckAndCleanupStep(*this);
              else if(aStepName == KReferenceLocationAltitudeParamsStep)
                          testStep = new CReferenceLocationAltitudeParamsStep(*this);
              else if(aStepName == KReferenceLocationAltitudeStep)
                          testStep = new CReferenceLocationAltitudeStep(*this);
              else if(aStepName == KReferenceLocationRootStep)
                          testStep = new CReferenceLocationRootStep(*this);
              else if(aStepName == KReferenceLocationFinalCheckAndCleanupStep)
                          testStep = new CReferenceLocationFinalCheckAndCleanupStep(*this);
              else if(aStepName == KAssistanceDataSetCreateStep)
                          testStep = new CAssistanceDataSetCreateStep(*this);
              else if(aStepName == KAssistanceDataSetMergeStep)
                          testStep = new CAssistanceDataSetMergeStep(*this);
              else if(aStepName == KAssistanceDataReaderSetStep)
                          testStep = new CAssistanceDataReaderSetStep(*this);
            
	return testStep;
	}
