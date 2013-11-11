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
// Example CTestStep derived implementation
// 
//

/**
 @file TestAssistanceDataLogStep.cpp
*/
#include "testAssistanceDataLogStep.h"
#include "Te_LbsLoggingSuiteDefs.h"

#include <lbs/lbsassistancedatalogevent.h>

CTestAssistanceDataLogStep::~CTestAssistanceDataLogStep()
/**
 * Destructor
 */
	{
	}

CTestAssistanceDataLogStep::CTestAssistanceDataLogStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestAssistanceDataLogStep);
	}

TVerdict CTestAssistanceDataLogStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	__UHEAP_MARK;
	CTe_LbsLoggingSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTestAssistanceDataLogStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("CTestAssistanceDataLogStep::doTestStepL() in the file TestAssistanceDataLogStep.cpp"));  //Block start
		TInt TheLocalAppUid;
		TInt TheRequestMask;
		TInt TheReceiveMask;
		TInt TheRequestOutcome;
		TInt TheMax;
		
		// read log value from ini file.
		TBool read = ETrue;
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteMax, TheMax);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteLocalAppUid, TheLocalAppUid);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteRequestMask, TheRequestMask);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteReceiveMask, TheReceiveMask);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteRequestOutcome, TheRequestOutcome);
		
		if(read)
			{
			RLbsLogger logger;
			logger.Open(10);
			CLbsAssistanceDataLogEvent* event = CLbsAssistanceDataLogEvent::NewL(0, 0);
			static_cast<CLbsAssistanceDataLogEvent*>(event)->SetRequestAssistanceDataType((TUint32)TheRequestMask); // requested assistance data type
			static_cast<CLbsAssistanceDataLogEvent*>(event)->SetReceiveAssistanceDataType((TUint32)TheReceiveMask); // received assistance data type
			static_cast<CLbsAssistanceDataLogEvent*>(event)->SetRequestOutcome(ELbsRequestOutcomeInconclusive); // by passing the location request outcome;
			TInt ErrorCode = logger.AddEvent(*event);
			iTimer->Start(KWait2second, KWait2second, TCallBack(StopScheduler, this));
			CActiveScheduler::Start();
			iTimer->Cancel();
			if(ErrorCode == KErrNone)
				{
				INFO_PRINTF1(_L("\t AssistanceData Event Logged in."));
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(_L("\t Error: AssistanceData Logging Event."));
				SetTestStepResult(EFail);
				}

			// add group of events to test the queue
			for(TInt index = 0; index <= TheMax - 1 ; index++)
				{
				ErrorCode = logger.AddEvent(*event);
				if(ErrorCode != KErrNone)
					{
					INFO_PRINTF1(_L("\t Error: Add Events failed."));
					if(ErrorCode != KErrOverflow)
						{
						INFO_PRINTF1(_L("\t Error: Error reported wrong."));
						SetTestStepResult(EFail);
						return TestStepResult();
						}			
					else
						{
						break;
						}
					}
				}
			iTimer->Start(KWait15second, KWait15second, TCallBack(StopScheduler, this));
			CActiveScheduler::Start();
			iTimer->Cancel();
			INFO_PRINTF1(_L("\t Info: Add Event group successfully."));	
				
			// test the update
			event->SetRequestOutcome(ELbsRequestOutcomeCancel);	
			ErrorCode = logger.UpdateEvent(*event);
			iTimer->Start(KWait2second, KWait2second, TCallBack(StopScheduler, this));
			CActiveScheduler::Start();
			iTimer->Cancel();
			
			// update group of events to test the queue
			for(TInt index = 0; index <= TheMax - 1 ; index++)
				{
				ErrorCode = logger.UpdateEvent(*event);
				if(ErrorCode != KErrNone)
					{
					INFO_PRINTF1(_L("\t Error: Update Events failed."));
					if(ErrorCode != KErrOverflow)
						{
						INFO_PRINTF1(_L("\t Error: Error reported wrong."));
						SetTestStepResult(EFail);
						return TestStepResult();
						}			
					else
						{
						break;
						}
					}
				}
			iTimer->Start(KWait10second, KWait10second, TCallBack(StopScheduler, this));
			CActiveScheduler::Start();
			iTimer->Cancel();
				
			INFO_PRINTF1(_L("\t Info: Update Event group successfully."));	

			// read logs using CLogViewEvent
			RFs theFs;
			ErrorCode = theFs.Connect();
			if(ErrorCode)
				{
				INFO_PRINTF1(_L("\t Error: Open FileServer failed."));
				SetTestStepResult(EFail);
				return TestStepResult();
				}
		    CLogClient* client = CLogClient::NewL(theFs, CActive::EPriorityStandard);
			CleanupStack::PushL(client);
			
			CLogViewEvent* view = CLogViewEvent::NewL(*client, CActive::EPriorityStandard);
			CleanupStack::PushL(view);

			CLogFilter* filter = CLogFilter::NewL();
			CleanupStack::PushL(filter);
			filter->SetEventType(TUid::Uid(KLogLbsAssistanceDataEventType));
			CLbsLogTestActive* active = new(ELeave)CLbsLogTestActive;
			CleanupStack::PushL(active);

			// add the 2nd event using the same event object
			// but via RFs connectiong and another CLogClient
			// session instead.
			// to test multiple RFs clients
			CLogEvent * tmpLogEvent = event->ExternalizeL();
			client->AddEvent(*tmpLogEvent, active->iStatus);
			active->StartL();
			CActiveScheduler::Start();
			if (active->iStatus != KErrNone)
				{
				INFO_PRINTF1(_L("\t Error: Add 2nd Event failed."));
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			INFO_PRINTF1(_L("\t Info: Add 2nd Event from 2nd CLogClient session."));
			
			client->DeleteEvent(tmpLogEvent->Id(), active->iStatus);
			delete tmpLogEvent;
			active->StartL();
			CActiveScheduler::Start();
			if (active->iStatus != KErrNone)
				{
				INFO_PRINTF1(_L("\t Error: Delete 2nd Event failed."));
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			INFO_PRINTF1(_L("\t Info: Delete 2nd Event from 2nd CLogClient session."));
			delete event;			

			// obtain the last added event from LBS logger 
			view->SetFilterL(*filter, active->iStatus);
			active->StartL();
			CActiveScheduler::Start();
			if (active->iStatus != KErrNone)
				{
				INFO_PRINTF1(_L("\t Error: Create Log view failed."));
				SetTestStepResult(EFail);
				return TestStepResult();
				}
				
			CLogEvent* logEvent = CLogEvent::NewL();
			CleanupStack::PushL(logEvent);
			// read the latest one which should be just logged in
			if(!view->LastL(active->iStatus))
				{
				INFO_PRINTF1(_L("\t Error: Read last log failed."));
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			logEvent->CopyL(view->Event());
			// internalise event to certain type of LBS log event
			CLbsLogEvent * lbsLogEvent = CLbsLogEvent::NewL(*logEvent);
			CleanupStack::PushL(lbsLogEvent);
			INFO_PRINTF1(_L("\t Info: Read Logged Event from DB successful."));
	
			read = ETrue;
			read &= (lbsLogEvent->EventType() == TUid::Uid(KLogLbsAssistanceDataEventType));
			read &= !lbsLogEvent->IsCostInformationAvailable();
			//read &= (lbsLogEvent->RequestOutcome() == (TLbsRequestOutcome)TheRequestOutcome);
			// test for update event
			read &= (lbsLogEvent->RequestOutcome() == ELbsRequestOutcomeCancel);
			read &= (static_cast<CLbsAssistanceDataLogEvent*>(lbsLogEvent)->RequestAssistanceDataType() == (TUint32)TheRequestMask);
			read &= (static_cast<CLbsAssistanceDataLogEvent*>(lbsLogEvent)->ReceiveAssistanceDataType() == (TUint32)TheReceiveMask);

			if(!read)
				{
				INFO_PRINTF1(_L("\t Error: Read log info not match."));
				SetTestStepResult(EFail);
				return TestStepResult();
				}
	
			client->Cancel();
			view->Cancel();
			logger.Close();

			SetTestStepResult(EPass);
			//-------------------------------------------------------------
			CleanupStack::PopAndDestroy();// lbslogEvent
			CleanupStack::PopAndDestroy();// logEvent
			CleanupStack::PopAndDestroy();// active
			CleanupStack::PopAndDestroy();// filter
			CleanupStack::PopAndDestroy();// view
			CleanupStack::PopAndDestroy();// client
			
			}
		else
			{
			INFO_PRINTF1(_L("\t Error: Read INI file failed."));
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		}
	return TestStepResult();
	}



TVerdict CTestAssistanceDataLogStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	CTe_LbsLoggingSuiteStepBase::doTestStepPostambleL();
	SetTestStepResult(EPass);		// or EFail
	__UHEAP_MARKEND;
	return TestStepResult();
	}
