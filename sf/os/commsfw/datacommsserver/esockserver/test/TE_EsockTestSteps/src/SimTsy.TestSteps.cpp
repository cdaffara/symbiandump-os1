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
//

/**
 @file SimTsy.TestSteps.cpp
*/

#include "Connections.TestSteps.h"
#include "SocketServer.TestSteps.h"


// Start SimTsy
//-------------

CStartSimTSYStep::CStartSimTSYStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KStartSimTSYStep);
	}

//Set and get methods for SIM TSYs section number
void CStartSimTSYStep::SetTestSectionNumber(TInt aTestSectionNumber)
	{
	iTestSectionNumber = aTestSectionNumber;
	}
 	
TInt CStartSimTSYStep::TestSectionNumber() const
	{
	return iTestSectionNumber;
	}
 
TVerdict CStartSimTSYStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CStartSimTSYStep::ConfigureFromIni()
    {
	//Read the test number from the ini file
 	TInt testNumber=0;
	if (GetIntFromConfig(iSection, KSimTsySectionNumber, testNumber) != 1)
		{
		SetTestSectionNumber(0);
		}
	else
		{
		SetTestSectionNumber(testNumber);
		}

	return KErrNone;
    }

TVerdict CStartSimTSYStep::doSingleTestStep()
	{
	TInt error = 0;
     
	// Apply the test number to the instance of simtsy
	if (TestSectionNumber() >= 0)
 		{
     	error = iEsockTest->iSimTsy.SetTestNumber(TestSectionNumber());
     	}

    if (error != KErrNone)
        {
        INFO_PRINTF3(_L("Could not set simtsy test number. number:%d, error:%d"), TestSectionNumber(), error);
        return EFail;
        }
        
	// Start the sim tsy with its given settings
	error = iEsockTest->iSimTsy.Start();
     
 	if (error!=KErrNone)
    	{
        INFO_PRINTF2(_L("Could not start simtsy. error:%d"), error);
        return EFail;
		}
          
	return EPass;
	}


// Stop SimTsy
//------------

CStopSimTSYStep::CStopSimTSYStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KStopSimTSYStep);
	}

TInt CStopSimTSYStep::ConfigureFromIni()
    {
	// No configuration involved
	return KErrNone;
    }

TVerdict CStopSimTSYStep::doSingleTestStep()
	{
    iEsockTest->iSimTsy.Stop();
	return EPass;
	}


// SimTsy Trigger Event
//---------------------

CSimTSYTriggerEventStep::CSimTSYTriggerEventStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSimTSYTriggerEventStep);
	}

TVerdict CSimTSYTriggerEventStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CSimTSYTriggerEventStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();
    
    if (GetStringFromConfig(iSection, KTe_SimtsyTriggerEventName, iParams.iSimTsyEventName)!=1
    || iParams.iSimTsyEventName.Length()==0)
        return KErrNotFound;

            
    if (!GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName))
    {
    INFO_PRINTF2(_L("%S: Connection name missing."),&iParams.iSimTsyEventName);
    return KErrNotFound;
    }
    
    if (!GetStringFromConfig(iSection,KTe_SimTsyEventName,iParams.iEventName))
    {
    INFO_PRINTF2(_L("%S: Event name missing."),&iParams.iSimTsyEventName);
    return KErrNotFound;
    }
    
    if (!GetIntFromConfig(iSection,KTe_SimTsyEventIndex,iParams.iEventIndex))
    {
    INFO_PRINTF2(_L("%S: Event index missing."),&iParams.iSimTsyEventName);
    return KErrNotFound;
    }

	if(iParams.iEventName.Compare(_L("ENetworkQoSChange")) == 0)
		{
		iParams.iEventKey =  KPSSimTsyNetworkQoSChange;
		}
	else if(iParams.iEventName.Compare(_L("ENetworkChangeRegStatus")) == 0)
		{
		iParams.iEventKey =  KPSSimtsyPacketServiceNtwkRegStatusChange;
		}
	else if(iParams.iEventName.Compare(_L("EContextStatusChange")) == 0)
		{
		iParams.iEventKey =  KPSSimtsyPacketContextStatusChange;
		}
	else if((iParams.iEventName.Compare(_L("EMbmsServAvailChange")) == 0)  	||
			(iParams.iEventName.Compare(_L("EMbmsBearerAvailChange")) == 0) ||
			(iParams.iEventName.Compare(_L("EMbmsServiceRemove")) == 0)  	||
			(iParams.iEventName.Compare(_L("EMbmsActiveListChange")) == 0)  ||
			(iParams.iEventName.Compare(_L("EMbmsMonitorListChange")) == 0))
		{
		iParams.iEventKey =  KPSSimTsyMbmsService;
		}
	else
		{
		ERR_PRINTF2(_L("Unknown event [%S] requested"), &iParams.iEventName);
		return KErrNotFound;
		}	
	
    return KErrNone;
    }

TVerdict CSimTSYTriggerEventStep::doSingleTestStep()
	{
			
	// Will fire after we register for notifications	
	TInt error = iEsockTest->EtelRequestL(iParams.iEventKey, iParams.iEventIndex); 
	if (error!=KErrNone)
        {
        INFO_PRINTF1(_L("Could not trigger simtsy event."));
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;

	}


