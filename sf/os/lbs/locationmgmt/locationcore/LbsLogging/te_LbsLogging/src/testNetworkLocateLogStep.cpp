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
 @file TestNetworkLocateLogStep.cpp
*/
#include "testNetworkLocateLogStep.h"
#include "Te_LbsLoggingSuiteDefs.h"

#include <lbs/lbsnetworklocatelogevent.h>

CTestNetworkLocateLogStep::~CTestNetworkLocateLogStep()
/**
 * Destructor
 */
	{
	}

CTestNetworkLocateLogStep::CTestNetworkLocateLogStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestNetworkLocateLogStep);
	}

TVerdict CTestNetworkLocateLogStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	__UHEAP_MARK;
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_LbsLoggingSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTestNetworkLocateLogStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("CTestNetworkLocateLogStep::doTestStepL() in the file TestNetworkLocateLogStep.cpp"));  //Block start
		TInt TheLocalAppUid;
		TInt TheCost;
		TInt TheRequestOutcome;
		TInt TheModuleId;
		TInt ThePosUpdateType;
		TInt TheTechType;
		TInt TheModeReason;
		TInt TheVerAccuracy;
		TInt TheHorAccuracy;
		TInt TheLat;
		TInt TheLong;
		TInt TheMax;
		
		// read log value from ini file.
		TBool read = ETrue;
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteMax, TheMax);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteLocalAppUid, TheLocalAppUid);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteCost, TheCost);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteRequestOutcome, TheRequestOutcome);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteModuleId, TheModuleId);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuitePosUpdateType, ThePosUpdateType);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteTechType, TheTechType);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteModeReason, TheModeReason);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteVerAccuracy, TheVerAccuracy);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteHorAccuracy, TheHorAccuracy);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteLat, TheLat);
		read &= GetIntFromConfig(ConfigSection(), KTe_LbsLoggingSuiteLong, TheLong);
		if(read)
			{
			RLbsLogger logger;
			logger.Open(10);
			CLbsNetworkLocateLogEvent* event = CLbsNetworkLocateLogEvent::NewL(TUid::Uid(0), NULL);
			event->SetCostInformation(TheCost); // set cost information if there are any;
			event->SetRequestOutcome((TLbsRequestOutcome)TheRequestOutcome); // by passing the location request outcome;
			static_cast<CLbsNetworkLocateLogEvent*>(event)->SetLocalApp(TUid::Uid(TheLocalAppUid)); // by passing in NewL() the UID of the local application;

			// set position information
			TPositionInfoBase* posInfo = new TPositionInfo();
			posInfo->SetModuleId(TUid::Uid(TheModuleId));
			posInfo->SetUpdateType(EPositionUpdateGeneral);
			posInfo->SetPositionMode((TPositionModuleInfo::TTechnologyType)TheTechType);
			posInfo->SetPositionModeReason((TPositionModeReason)TheModeReason);
			TPosition position;
			position.SetCurrentTime();
			position.SetAccuracy((TReal32)TheVerAccuracy,(TReal32)TheHorAccuracy);
			position.SetCoordinate((TReal64)TheLat,(TReal64)TheLong);
			static_cast<TPositionInfo*>(posInfo)->SetPosition(position);
			
			// set position course information
			TPositionInfoBase* posCourseInfo = new TPositionCourseInfo();
			posCourseInfo->SetModuleId(TUid::Uid(TheModuleId));
			posCourseInfo->SetUpdateType(EPositionUpdateGeneral);
			posCourseInfo->SetPositionMode((TPositionModuleInfo::TTechnologyType)TheTechType);
			posCourseInfo->SetPositionModeReason((TPositionModeReason)TheModeReason);
			TPosition position2;
			position2.SetCurrentTime();
			position2.SetAccuracy((TReal32)TheVerAccuracy,(TReal32)TheHorAccuracy);
			position2.SetCoordinate((TReal64)TheLat,(TReal64)TheLong);
			static_cast<TPositionInfo*>(posCourseInfo)->SetPosition(position2);
			

			TInt ErrorCode = logger.AddEvent(*event);
			iTimer->Start(KWait2second, KWait2second, TCallBack(StopScheduler, this));
			CActiveScheduler::Start();
			iTimer->Cancel();
	
			if(ErrorCode == KErrNone)
				{
				INFO_PRINTF1(_L("\t NetworkLocate Event Logged in."));
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(_L("\t Error: NetworkLocate Logging Event."));
				SetTestStepResult(EFail);
				}
			event->SetPositionInfo(posInfo); // by passing optional TpositionInfoBase*;
			event->SetDirection(ETrue);
			ErrorCode = logger.AddEvent(*event);
			iTimer->Start(KWait2second, KWait2second, TCallBack(StopScheduler, this));
			CActiveScheduler::Start();
			iTimer->Cancel();
	
			if(ErrorCode == KErrNone)
				{
				INFO_PRINTF1(_L("\t NetworkLocate Event Logged in."));
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(_L("\t Error: NetworkLocate Logging Event."));
				SetTestStepResult(EFail);
				}
			
			// add group of events to test the queue
			for(TInt index = 0; index <= TheMax - 1 ; index++)
				{
				if ((index % 2) == 0)
					{
					event->SetPositionInfo(posInfo);
					ErrorCode = logger.AddEvent(*event);
					}
				else
					{
					event->SetPositionInfo(posCourseInfo);
					ErrorCode = logger.AddEvent(*event);
					}
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
			filter->SetEventType(TUid::Uid(KLogLbsNetworkLocateEventType));
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
		
			TPosition pos;
			static_cast<TPositionInfo*>(lbsLogEvent->PositionInfo())->GetPosition(pos);
			
			read = ETrue;
			read &= (static_cast<CLbsNetworkLocateLogEvent*>(lbsLogEvent)->LocalApp() == TUid::Uid(TheLocalAppUid));
			read &= (lbsLogEvent->CostInformation() == (TBool)TheCost);
			read &= (lbsLogEvent->EventType() == TUid::Uid(KLogLbsNetworkLocateEventType));
			//read &= (lbsLogEvent->RequestOutcome() == (TLbsRequestOutcome)TheRequestOutcome);
			// test for update event
			read &= (lbsLogEvent->RequestOutcome() == ELbsRequestOutcomeCancel);
			read &= (lbsLogEvent->PositionInfo()->ModuleId() == TUid::Uid(TheModuleId));
			read &= (lbsLogEvent->PositionInfo()->PositionMode() == (TPositionModuleInfo::TTechnologyType)TheTechType);
			read &= (lbsLogEvent->PositionInfo()->PositionModeReason() == (TPositionModeReason)TheModeReason);
			read &= (pos.HorizontalAccuracy() == (TReal32)TheHorAccuracy);
			read &= (pos.VerticalAccuracy() == (TReal32)TheVerAccuracy);
			read &= (pos.Latitude() == position.Latitude());
			read &= (pos.Longitude() == position.Longitude());
			if(!read)
				{
				INFO_PRINTF1(_L("\t Error: Read log info not match."));
				SetTestStepResult(EFail);
				return TestStepResult();
				}
				
			client->Cancel();
			view->Cancel();
			logger.Close();
			delete posInfo;
			delete posCourseInfo;
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



TVerdict CTestNetworkLocateLogStep::doTestStepPostambleL()
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
