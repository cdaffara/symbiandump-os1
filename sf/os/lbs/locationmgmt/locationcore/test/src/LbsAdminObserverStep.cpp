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
// Example CTestStep derived implementation
// 
//

/**
 @file LbsAdminObserverStep.cpp
*/
#include "lbsadminobserverstep.h"
#include "te_lbsadminsuitedefs.h"
#include <lbs/lbsadmin.h>

const TTimeIntervalMicroSeconds32 KWait1second(1000000); // 1 second

CLbsAdminObserverStep::~CLbsAdminObserverStep()
	{
	if(iLbsAdmin)
		{
		iLbsAdmin->ResetToDefault();
		delete iLbsAdmin;
		}
	if(iTimer)
		{
		delete iTimer;
		}
	if(iActSchd)
		{
		delete iActSchd;
		}
	}

CLbsAdminObserverStep::CLbsAdminObserverStep():
	iNotified(EFalse),
	iNotificationsCount(0)
	{
	SetTestStepName(KLbsAdminObserverStep);
	}

TVerdict CLbsAdminObserverStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsAdminSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	
	// Install the active scheduler
    iActSchd = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActSchd);
	
	// Create an admin API to observe changes to all settings
	// (the selected setting will change as the test progresses)
	//
	iLbsAdmin = CLbsAdmin::NewL(*this, 	KLbsSettingHomeSelfLocate |
										KLbsSettingRoamingSelfLocate |
										KLbsSettingQualityProfileSelfLocate |
										KLbsSettingHomeExternalLocate |
										KLbsSettingRoamingExternalLocate |
										KLbsSettingQualityProfileExternalLocate |
										KLbsSettingHomeEmergencyLocate |
										KLbsSettingRoamingEmergencyLocate |
										KLbsSettingHomeTransmitLocate |
										KLbsSettingRoamingTransmitLocate |
										KLbsSettingQualityProfileTransmitLocate |
										KLbsSettingHomeGpsMode |
										KLbsSettingRoamingGpsMode |
										KLbsSettingHomeProtocolModule |
										KLbsSettingRoamingProtocolModule |
										KLbsSettingLbsSystemStartMode |
										KLbsSettingBehaviourMode |
										KLbsSettingPrivacyAppTimeout |
										KLbsSettingPrivacyTimeoutAction |
										KLbsSettingPrivacyHandler |
										KLbsSettingLogger |
										KLbsSettingSetClockModule |
										KLbsSettingClockAdjustInterval |
										KLbsSettingClockAdjustThreshold |
										KLbsSettingAllowManualClockAdjust |
										KLbsSettingClockAdjust |
										KLbsSettingHomeNetworkInducedLocate |
										KLbsSettingRoamingNetworkInducedLocate);										

	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
								  	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsAdminObserverStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	  TBool notificationsReceived;
	  TUint notifExpectNb;
	  if (TestStepResult()==EPass)
		{
		TLbsAdminSettingGroup notificationsRequestedMask = KLbsSettingNone;
		for(TInt i = 0; i <= KNumberOfProperties; i++)
			{
			//Expand the notifications mask with every iteration
			
	        notifExpectNb = AddToNotificationMaskL(notificationsRequestedMask,i);
	        INFO_PRINTF2(_L("Expecting %d notifications"),i);
	        // Switch to modifier thread
    	    AllowSettingsModification();
        	WaitForNotifications();
        	notificationsReceived = CheckResults(notificationsRequestedMask,notifExpectNb);
        	if(!notificationsReceived)
        		{
        		ERR_PRINTF2(_L("Failed to receive notification for setting number %d"),i);
    			SetTestStepResult(EFail);
        		}
        	else
        		{
        		INFO_PRINTF2(_L("Received %D notifications\n"),iNotificationsCount);        		
        		}	
			}
		}
	  iLbsAdmin->ClearNotificationMask();
	  iModifierSem.Signal(); //Let modifier finish
	  INFO_PRINTF1(_L("Observer finishes"));
	  return TestStepResult();
	}

void CLbsAdminObserverStep::WaitForNotifications()
	{
	// Let notifications arrive for one second at at time.
    // Keep doing this until one second goes with no notifications.
    //
    iNotificationsCount = 0;
    iNotificationsReceivedMask = 0;
    do	{
		iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
		iNotified = EFalse; // Will change to true if a notification is received in the next second.
		CActiveScheduler::Start(); // Wait for notification
		}
	while (iNotified);
	}

TInt CLbsAdminObserverStep::StopWaitingForNotification(TAny* aPtr)
	{
	CLbsAdminObserverStep* observerStep = static_cast<CLbsAdminObserverStep*>(aPtr);
	if (observerStep)
		{
		observerStep->DoStopWaitingForNotification();
		}
	CActiveScheduler::Stop();
	return KErrNone;
	}

void CLbsAdminObserverStep::DoStopWaitingForNotification()
	{
	INFO_PRINTF1(_L("Stopped waiting for notifications"));
	iTimer->Cancel();
	}

void CLbsAdminObserverStep::OnSettingUpdateEvent(TInt aError, const TLbsAdminSetting& aSetting)
	{
	if (aError == KErrNone)
		{
		iNotified = true;
		iNotificationsCount++;
		iNotificationsReceivedMask |= aSetting;
		}
	else
		{
		ERR_PRINTF1(_L("Error reported from observer"));
 		User::Panic(KLbsAdminObserverStep, aError);
		}
	}

// Let modifier step run and wait for it to finish
void CLbsAdminObserverStep::AllowSettingsModification()
	{
        iModifierSem.Signal();
        INFO_PRINTF1(_L("SIGNAL modifier....."));
        iObserverSem.Wait();
        INFO_PRINTF1(_L("...OBSERVING!"));
	}
	
// Set Notification mask
//
TUint CLbsAdminObserverStep::AddToNotificationMaskL(TLbsAdminSettingGroup& notificationsRequestedMask, const TUint i)
	{
	TUint notificationsNb = i;
    switch(i)
    	{
    	case 0:
    	    notificationsRequestedMask = KLbsSettingNone;
    	    break;
    	case 1:
    		notificationsRequestedMask |= KLbsSettingHomeSelfLocate;
    		break;
    	case 2:
    		notificationsRequestedMask |= KLbsSettingRoamingSelfLocate;
    		break;
    	case 3:
    		notificationsRequestedMask |= KLbsSettingQualityProfileSelfLocate;
    		break;
    	case 4:
    	    notificationsRequestedMask |= KLbsSettingHomeExternalLocate;
    		break;
    	case 5:
    	    notificationsRequestedMask |= KLbsSettingRoamingExternalLocate;
    		break;
    	case 6:
    	    notificationsRequestedMask |= KLbsSettingQualityProfileExternalLocate;
    		break;
    	case 7:
    		// Slightly different here. Remove two of the previous settings to test
    		// that observers are removed.
    	    notificationsRequestedMask |= KLbsSettingHomeEmergencyLocate;
    	    notificationsRequestedMask &= ~KLbsSettingQualityProfileExternalLocate;
    	    notificationsRequestedMask &= ~KLbsSettingRoamingSelfLocate;
    	    notificationsNb = 5;
    		break;
    	case 8:
    	    notificationsRequestedMask |= KLbsSettingRoamingEmergencyLocate;
    	    notificationsRequestedMask |= KLbsSettingQualityProfileExternalLocate;
    	    notificationsRequestedMask |= KLbsSettingRoamingSelfLocate;
    		break;
    	case 9:
    	    notificationsRequestedMask |= KLbsSettingHomeTransmitLocate;
    		break;
    	case 10:
    	    notificationsRequestedMask |= KLbsSettingRoamingTransmitLocate;
    		break;
    	case 11:
    	    notificationsRequestedMask |= KLbsSettingQualityProfileTransmitLocate;
    		break;
    	case 12:
    	    notificationsRequestedMask |= KLbsSettingHomeGpsMode;
    		break;
    	case 13:
    	    notificationsRequestedMask |= KLbsSettingRoamingGpsMode;
    		break;
    	case 14:
    	    notificationsRequestedMask |= KLbsSettingHomeProtocolModule;
    		break;
    	case 15:
    	    notificationsRequestedMask |= KLbsSettingRoamingProtocolModule;
    		break;
    	case 16:
    	    notificationsRequestedMask |= KLbsSettingLbsSystemStartMode;
    		break;
      	case 17:
    	    notificationsRequestedMask |= KLbsSettingPrivacyTimeoutAction;
    		break;
    	case 18:
    	    notificationsRequestedMask |= KLbsSettingPrivacyHandler;
    		break;
    	case 19:
    		notificationsRequestedMask |= KLbsSettingLogger;
    		break;
    	case 20:
    	    notificationsRequestedMask |= KLbsSettingClockAdjust;
    		break;
    	case 21:
    	    notificationsRequestedMask |= KLbsSettingAllowManualClockAdjust;
    		break;
    	case 22:
    	    notificationsRequestedMask |= KLbsSettingSetClockModule;
    		break;
    	case 23:
    	    notificationsRequestedMask |= KLbsSettingClockAdjustInterval;
    		break;
    	case 24:
    	    notificationsRequestedMask |= KLbsSettingClockAdjustThreshold;
    		break;
	  	case 25:
    	    notificationsRequestedMask |= KLbsSettingBehaviourMode;
    		break;
	  	case 26:
	  		notificationsRequestedMask |= KLbsSettingPrivacyAppTimeout;
	  		break;
    	case 27:
    		notificationsRequestedMask |= KLbsSettingHomeNetworkInducedLocate;
    		break;
    	case 28:
    		notificationsRequestedMask |= KLbsSettingRoamingNetworkInducedLocate;
    		break;
    	}
    	
    iLbsAdmin->SetNotificationMaskL(notificationsRequestedMask);
    return notificationsNb;
	}

// Check that both the number of notifications received and the settings notified
// are as expected.
//
TBool CLbsAdminObserverStep::CheckResults(const TLbsAdminSettingGroup& notificationsRequestedMask, const TUint i)
{
	TBool ret = false;
	if(notificationsRequestedMask == iNotificationsReceivedMask)
		{
		if(iNotificationsCount == i)
			{
			ret = ETrue;	 
			}
		else
			{
			ERR_PRINTF3(_L("Notifications expected: %d; Notifications received: %d"),i,iNotificationsCount)
			}
		}
	else
		{
		ERR_PRINTF1(_L("Expected notifications different from received notifications"));		
		}
	return ret;
}



TVerdict CLbsAdminObserverStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsAdminSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

