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
// te_hwrm.exe
// 
//

#include <centralrepository.h>
#include <hwrmvibrasdkcrkeys.h>
#include <hwrmlightdomaincrkeys.h>
#include <hwrmpowerdomaincrkeys.h>
#include <hwrmfmtxdomaincrkeys.h>
#include <hwrmfmtxaudiopolicydomainpskeys.h>
#include "HWRMFmTxInternalCRKeys.h"
#include "hwrmtestbase.h"
#include "hwrmtestmacros.h"
#include "HWRMPrivatePSKeys.h"

_LIT(KVibraModeON, "EVibraModeON");
_LIT(KVibraModeOFF, "EVibraModeOFF");
_LIT(KVibraModeUnknown, "EVibraModeUnknown");

_LIT(KVibraStatusUnknown, "EVibraStatusUnknown");
_LIT(KVibraStatusNotAllowed, "EVibraStatusNotAllowed");
_LIT(KVibraStatusStopped, "EVibraStatusStopped");
_LIT(KVibraStatusOn, "EVibraStatusOn");

_LIT(KLightStatusUnknown, "ELightStatusUnknown");
_LIT(KLightOn, "ELightOn");
_LIT(KLightOff, "ELightOff");
_LIT(KLightBlink, "ELightBlink");

_LIT(KFmTxStateOff, "EFmTxStateOff");
_LIT(KFmTxStateActive, "EFmTxStateActive");
_LIT(KFmTxStateInactive, "EFmTxStateInactive");
_LIT(KFmTxStatePowerSaveInactivity, "EFmTxStatePowerSaveInactivity");
_LIT(KFmTxStatePowerSaveAccessory, "EFmTxStatePowerSaveAccessory");
_LIT(KFmTxStateScanning, "EFmTxStateScanning");
_LIT(KFmTxStateUnknown, "EFmTxStateUnkown");


void CHWRMTestBase::OpenSessionsL(TInt aNoLightSessionsToOpen, TInt aNoVibraSessionsToOpen, TInt aNoExtLightSessionsToOpen, TInt aNoFmTxSessionsToOpen, TInt aNoPowerSessionsToOpen)
	{
	INFO_PRINTF1(_L("--START CHWRMTestBase::OpenSessionsL"));
	aNoLightSessionsToOpen = (aNoLightSessionsToOpen>KMaxNoOpenSessionsForResource) ? KMaxNoOpenSessionsForResource: aNoLightSessionsToOpen;
	aNoVibraSessionsToOpen = (aNoVibraSessionsToOpen>KMaxNoOpenSessionsForResource) ? KMaxNoOpenSessionsForResource: aNoVibraSessionsToOpen;
	aNoExtLightSessionsToOpen = (aNoExtLightSessionsToOpen>KMaxNoOpenSessionsForResource) ? KMaxNoOpenSessionsForResource: aNoExtLightSessionsToOpen;
	aNoPowerSessionsToOpen = (aNoPowerSessionsToOpen>KMaxNoOpenSessionsForResource) ? KMaxNoOpenSessionsForResource: aNoPowerSessionsToOpen;
	aNoFmTxSessionsToOpen = (aNoFmTxSessionsToOpen>KMaxNoOpenSessionsForResource) ? KMaxNoOpenSessionsForResource: aNoFmTxSessionsToOpen;
	
	for(TInt i = 0; i < aNoVibraSessionsToOpen; i++)
		{
		if(i==0)
			{
			//used first session as one listening for callbacks
			iVibraSessions[i] = CHWRMVibra::NewL(this);
			}
		else
			{
			iVibraSessions[i] = CHWRMVibra::NewL();	
			}		
		}

	for(TInt i = 0; i < aNoLightSessionsToOpen; i++)
		{
		if(i==0)
			{
			//used first session as one listening for callbacks
			INFO_PRINTF1(_L("CHWRMTestBase::OpenSessionsL - BEFORE CREATING FIRST LIGHT SESSION"));
			iLightSessions[i] = CHWRMLight::NewL(this);
			INFO_PRINTF1(_L("CHWRMTestBase::OpenSessionsL - AFTER CREATING FIRST LIGHT SESSION"));
			}
		else
			{
			iLightSessions[i] = CHWRMLight::NewL();	
			}		
		}

	for(TInt i = 0; i < aNoExtLightSessionsToOpen; i++)
		{
		if(i==0)
			{
			//used first session as one listening for callbacks
			iExtLightSessions[i] = CHWRMExtendedLight::NewL(this);
			}
		else
			{
			iExtLightSessions[i] = CHWRMExtendedLight::NewL();	
			}		
		}
	
	for(TInt i = 0; i < aNoPowerSessionsToOpen; i++)
		{
		if(i==0)
			{
			//used first session as one listening for callbacks
			iPowerSessions[i] = CHWRMPower::NewL();	
			}
		else
			{
			iPowerSessions[i] = CHWRMPower::NewL();	
			}
		}
	
	for(TInt i = 0; i <  aNoFmTxSessionsToOpen; i++)
		{
		if(i==0)
			{
			//used first session as one listening for callbacks
			iFmTxSessions[i] = CHWRMFmTx::NewL(this);
			}
		else
			{
			iFmTxSessions[i] =  CHWRMFmTx::NewL();	
			}		
		}

	INFO_PRINTF1(_L("CHWRMTestBase::OpenSessionsL - BEFORE OPENING MOCKSY SESSIONS"));
	// Open sessions to MockSY	
	if (aNoLightSessionsToOpen > 0 && !iIsMockLightSessionOpen)
		{
		User::LeaveIfError(iMockLight.Connect());
		iIsMockLightSessionOpen = ETrue;
		}
	if (aNoVibraSessionsToOpen > 0 && !iIsMockVibraSessionOpen)
		{
		User::LeaveIfError(iMockVibra.Connect());
		iIsMockVibraSessionOpen = ETrue;
		}
	if (aNoExtLightSessionsToOpen > 0 && !iIsMockLightSessionOpen)
		{
		User::LeaveIfError(iMockLight.Connect());
		iIsMockLightSessionOpen = ETrue;
		}
	if (aNoPowerSessionsToOpen > 0 && !iIsMockPowerSessionOpen)
		{
		User::LeaveIfError(iMockPower.Connect());
		iIsMockPowerSessionOpen = ETrue;
		}
	if (aNoFmTxSessionsToOpen > 0 && !iIsMockFmTxSessionOpen)
		{
		User::LeaveIfError(iMockFmTx.Connect());
		iIsMockFmTxSessionOpen = ETrue;
		}
	
	INFO_PRINTF1(_L("--END CHWRMTestBase::OpenSessionsL"));	
	}

void CHWRMTestBase::OpenLightSessionEnhancedL(TInt aNoEnhancedLightSessionsToOpen)
	{
	INFO_PRINTF1(_L("--START CHWRMTestBase::OpenEnhancedLightSessionL"));
	aNoEnhancedLightSessionsToOpen = (aNoEnhancedLightSessionsToOpen>KMaxNoOpenSessionsForResource) ? KMaxNoOpenSessionsForResource: aNoEnhancedLightSessionsToOpen;
	
	for(TInt i = 0; i < aNoEnhancedLightSessionsToOpen; i++)
		{
		if(i==0)
			{
			//used first session as one listening for callbacks
			iEnhancedLightSessions[i] = CHWRMEnhancedLight::NewL(this);
			}
		else
			{
			iEnhancedLightSessions[i] = CHWRMEnhancedLight::NewL();	
			}		
		}
	
	INFO_PRINTF1(_L("CHWRMTestBase::OpenEnhancedLightSessionL - BEFORE OPENING MOCKSY SESSIONS"));
	// Open sessions to MockSY	
	if (aNoEnhancedLightSessionsToOpen > 0 && !iIsMockLightSessionOpen)
		{
		User::LeaveIfError(iMockLight.Connect());
		iIsMockLightSessionOpen = ETrue;
		}
	INFO_PRINTF1(_L("--END CHWRMTestBase::OpenSessionsL"));	
	}

void CHWRMTestBase::OpenPowerStateSessionL()
	{
	if (!iIsMockPowerStateSessionOpen)
		{
		User::LeaveIfError(iMockPowerState.Connect());
		iIsMockPowerStateSessionOpen = ETrue;
		}
		
	// P&S property observers
	CleanupPropertyObservers();
	
	iBatteryLevelObserver = CPsPropertyObserver::NewL(KPSUidHWRMPowerState, KHWRMBatteryLevel,*this);
	iBatteryStatusObserver = CPsPropertyObserver::NewL(KPSUidHWRMPowerState, KHWRMBatteryStatus,*this);
	iExtendedBatteryStatusObserver = CPsPropertyObserver::NewL(KPSUidHWRMPowerState, KHWRMExtendedBatteryStatus,*this);
	iChargingStatusObserver = CPsPropertyObserver::NewL(KPSUidHWRMPowerState, KHWRMChargingStatus,*this);
	iExtendedChargingStatusObserver = CPsPropertyObserver::NewL(KPSUidHWRMPowerState, KHWRMExtendedChargingStatus,*this);
	}

void CHWRMTestBase::CleanupPropertyObservers()
	{
	delete iBatteryLevelObserver;
	iBatteryLevelObserver = NULL;
	delete iBatteryStatusObserver;
	iBatteryStatusObserver = NULL;
	delete iExtendedBatteryStatusObserver;
	iExtendedBatteryStatusObserver = NULL;
	delete iChargingStatusObserver;
	iChargingStatusObserver = NULL;
	delete iExtendedChargingStatusObserver;
	iExtendedChargingStatusObserver = NULL;
	}

void CHWRMTestBase::GetCRVibraAttributeL(TUint32 aVibraKey, TInt &aValue)
	{
	CRepository* rep = CRepository::NewLC(KCRUidVibraCtrl);
    User::LeaveIfError(rep->Get(aVibraKey, aValue));
    CleanupStack::PopAndDestroy(rep);
	}


void CHWRMTestBase::GetVibraTimeAttributesL()
	{
	GetCRVibraAttributeL(KVibraMinOffTime, iVibraMinOffTime);
    if ( iVibraMinOffTime < 0 )
        {
        iVibraMinOffTime = 0;  // do not allow negative value
        }
    
    GetCRVibraAttributeL(KVibraCtrlMaxTime, iVibraMaxOnTime);
    if ( iVibraMaxOnTime < 0 )
        {
        iVibraMaxOnTime = 0;  // do not allow negative value
        }
        
    GetCRVibraAttributeL(KVibraCtrlFeedbackIntensity, iPulseMaxIntensity);
    if ( iPulseMaxIntensity < 0 )
        {
        iPulseMaxIntensity = 0;  // do not allow negative value
        }
        
    GetCRVibraAttributeL(KVibraCtrlFeedbackDuration, iPulseMaxDuration);
    if ( iPulseMaxDuration < 0 )
        {
        iPulseMaxDuration = 0;  // do not allow negative value
        }
    }
    
	
void CHWRMTestBase::SetCRVibraAttributeL(TUint32 aVibraKey, TInt aValue)
	{
	CRepository* rep = CRepository::NewLC(KCRUidVibraCtrl);
    User::LeaveIfError(rep->Set(aVibraKey, aValue));
	CleanupStack::PopAndDestroy(rep);
	}

TInt CHWRMTestBase::SetCRVibraAttribute(TUint32 aVibraKey, TInt aValue)
	{
	CRepository* rep = CRepository::NewL(KCRUidVibraCtrl);
    TInt err = rep->Set(aVibraKey, aValue);
	return err;
	}

void CHWRMTestBase::SetCRLightAttributes(TUint32 aLightKey, TInt aValue)
	{
    
	CRepository* rep = CRepository::NewLC(KCRUidLightSettings);
    User::LeaveIfError(rep->Set(aLightKey, aValue));
	CleanupStack::PopAndDestroy(rep);
	}

void CHWRMTestBase::GetCRLightAttributes(TUint32 aLightKey, TInt &aValue)
	{
	CRepository* rep = CRepository::NewLC(KCRUidLightSettings);
    User::LeaveIfError(rep->Get(aLightKey, aValue));
    CleanupStack::PopAndDestroy(rep);
	}

void CHWRMTestBase::SetLightControlAttributes(TInt aIntensity, TInt aSensitivity, TInt aInUse)
	{
	iLightDefaultIntensity = aIntensity;
	iLightDefaultSensitivity = aSensitivity;
	iLightSensorEnabled = aInUse;
	
	SetCRLightAttributes(KLightIntensity, iLightDefaultIntensity);
	SetCRLightAttributes(KLightSensorSensitivity, iLightDefaultSensitivity);
	SetCRLightAttributes(KLightSensorInUse, iLightSensorEnabled);
	}

void CHWRMTestBase::GetLightControlAttributes()
	{
	GetCRVibraAttributeL(KLightIntensity, iLightDefaultIntensity);
	GetCRVibraAttributeL(KLightSensorSensitivity, iLightDefaultSensitivity);
	GetCRVibraAttributeL(KLightSensorInUse, iLightSensorEnabled);
	}

void CHWRMTestBase::GetCRPowerAttributeL(TUint32 aPowerKey, TInt &aValue)
	{
	CRepository* rep = CRepository::NewLC(KCRUidPowerSettings);
    User::LeaveIfError(rep->Get(aPowerKey, aValue));
    CleanupStack::PopAndDestroy(rep);
	}

void CHWRMTestBase::GetPowerAttributesL()
	{
	TInt value;
	GetCRVibraAttributeL(KPowerBaseTimeInterval, value);
	iPowerBaseIntervalMicroSeconds = 1000 * value; //from milli to microseconds
	GetCRVibraAttributeL(KPowerMaxReportingPeriod, value); //from milli to microseconds
	iPowerMaxReportingPeriodMicroSeconds = 1000 * value;
	}

void CHWRMTestBase::SetCRPowerAttributeL(TUint32 aPowerKey, TInt aValue)
	{
	CRepository* rep = CRepository::NewLC(KCRUidPowerSettings);
    User::LeaveIfError(rep->Set(aPowerKey, aValue));
	CleanupStack::PopAndDestroy(rep);
	}

void CHWRMTestBase::SetCRFmTxAttributeL(TUint32 aFmTxKey, TInt aValue)
	{
	CRepository* rep = CRepository::NewLC(KCRUidFmTxCenRes);
    User::LeaveIfError(rep->Set(aFmTxKey, aValue));
    CleanupStack::PopAndDestroy(rep);
	}

void CHWRMTestBase::GetCRFmTxAttributeL(TUint32 aFmTxKey, TInt &aValue)
	{
	CRepository* rep = CRepository::NewLC(KCRUidFmTxCenRes);
    User::LeaveIfError(rep->Get(aFmTxKey, aValue));
    CleanupStack::PopAndDestroy(rep);
	}

void CHWRMTestBase::GetFmTxFrequencyStepSizeL()
	{
	GetCRFmTxAttributeL(KFmTxCenResKeyStepSize, iFmTxFrequencyStepSize);
	}

void CHWRMTestBase::SetFmTxFrequencyRangeEuropeL()
	{
	SetCRFmTxAttributeL(KFmTxCenResKeyMinFrequency, 88100);
	SetCRFmTxAttributeL(KFmTxCenResKeyMaxFrequency, 107900);
	SetCRFmTxAttributeL(KFmTxCenResKeyStepSize, 50);
	}

void CHWRMTestBase::SetFmTxFrequencyRangeJapanL()
	{
	SetCRFmTxAttributeL(KFmTxCenResKeyMinFrequency, 88100);
	SetCRFmTxAttributeL(KFmTxCenResKeyMaxFrequency, 90000);
	SetCRFmTxAttributeL(KFmTxCenResKeyStepSize, 50);
	}

void CHWRMTestBase::SetFmTxInactivityTimeOutL(TUint aInactivityTimeOut)
	{
	SetCRFmTxAttributeL(KFmTxCenResKeyPowerSaveTimerLength, aInactivityTimeOut);
	}

void CHWRMTestBase::ExpectBatteryLevelNotificationL(EPSHWRMBatteryLevel aBatteryLevel)
	{
	iExpectedBatteryLevelNotifications.AppendL(aBatteryLevel);
	}

void CHWRMTestBase::ExpectBatteryStatusNotificationL(EPSHWRMBatteryStatus aStatus)
	{
	iExpectedBatteryStatusNotifications.AppendL(aStatus);
	}

void CHWRMTestBase::ExpectExtendedBatteryStatusNotificationL(EPSHWRMBatteryStatus aStatus)
    {
    iExpectedExtendedBatteryStatusNotifications.AppendL(aStatus);
    }

void CHWRMTestBase::ExpectedChargingStatusNotificationsL(EPSHWRMChargingStatus aStatus)
	{
	iExpectedChargingStatusNotifications.AppendL(aStatus);
	}

void CHWRMTestBase::ExpectedExtendedChargingStatusNotificationsL(EPSHWRMChargingStatus aStatus)
    {
    iExpectedExtendedChargingStatusNotifications.AppendL(aStatus);
    }
	
void CHWRMTestBase::ExpectVibraStatusNotificationL(CHWRMVibra::TVibraStatus aStatus)
	{
	iExpectedVibraStatusNotifications.AppendL(aStatus);
	}
	
void CHWRMTestBase::ExpectLightStatusNotificationL(TInt aTarget, CHWRMLight::TLightStatus aStatus)
	{
	if (IgnoringStatusNotifications())
		{
		return;
		}
		
	TLightNotification notification;
	notification.iTarget = aTarget;
	notification.iStatus = aStatus; 	
	iExpectedLightNotifications.AppendL(notification);
	}

void CHWRMTestBase::ExpectExtLightStatusNotificationL(TInt aTarget, CHWRMExtendedLight::TLightStatus aStatus)
	{
	if (IgnoringStatusNotifications())
		{
		return;
		}
		
	TExtLightNotification notification;
	notification.iTarget = aTarget;
	notification.iStatus = aStatus; 	
	iExpectedExtLightNotifications.AppendL(notification);
	}

void CHWRMTestBase::ExpectFmTxStatusNotificationL(TFmTxState aStatus1,TFmTxState aStatus2)
	{
	TFmTxNotification notification;
	
	notification.iState1=aStatus1;
	notification.iState2=aStatus2;
	
	iExpectedFmTxStatusNotifications.AppendL(notification);
	}

void CHWRMTestBase::ExpectFmTxStatusNotificationL(TFmTxState aStatus)
	{
	ExpectFmTxStatusNotificationL(aStatus, aStatus);
	}

void CHWRMTestBase::ExpectFmTxFrequencyNotificationL(TInt aFrequency)
	{
	iExpectedFmTxFrequencyNotifications.AppendL(aFrequency);
	}

void CHWRMTestBase::ExpectVibraModeNotificationL()
	{
	TInt value(-1);
	GetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled,value);
	

	CHWRMVibra::TVibraModeState expectedVibraMode = 
			(value == 0) ? CHWRMVibra::EVibraModeOFF : CHWRMVibra::EVibraModeON;
		
	
	ExpectVibraModeNotificationL(expectedVibraMode);
	}
		
void CHWRMTestBase::ExpectVibraModeNotificationL(CHWRMVibra::TVibraModeState aState)
	{
	iExpectedVibraModeNotifications.AppendL(aState);
	}

void CHWRMTestBase::ExpectVibraFeedbackModeNotificationL(CHWRMVibra::TVibraFeedbackModeState aState)
	{
	iExpectedVibraFeedbackModeNotifications.AppendL(aState);
	}

//callbacks of state change
 void CHWRMTestBase::VibraModeChanged(CHWRMVibra::TVibraModeState aStatus)
 	{
 	switch (aStatus)
        {
	    case CHWRMVibra::EVibraModeUnknown:
	        INFO_PRINTF1(_L("Vibra mode changed: EVibraModeUnknown"));
	        break;
	    case CHWRMVibra::EVibraModeON:
	        INFO_PRINTF1(_L("Vibra mode changed: EVibraModeON"));
	        break;
	    case CHWRMVibra::EVibraModeOFF:
	        INFO_PRINTF1(_L("Vibra mode changed: EVibraModeOFF"));
	        break;
	    default:
	        INFO_PRINTF1(_L("Vibra mode changed: UNDEFINED ! "));
	        break;
        }
  	
  	if (iExpectedVibraModeNotifications.Count() > 0)
  		{
  		if (aStatus != iExpectedVibraModeNotifications[0])
  			{
	  		INFO_PRINTF3(_L("### ERROR: VibraModeChanged, expected:%d, actual:%d"),iExpectedVibraModeNotifications[0],aStatus);
	  		SetTestFail();
  			}
        iExpectedVibraModeNotifications.Remove(0);
 		}
 	else
 		{
 	  	INFO_PRINTF2(_L("### ERROR: VibraModeChanged, expected:NONE, actual:%d"),aStatus);
	  	SetTestFail();
 		}
    CheckForEndOfTransition();
 	}

void CHWRMTestBase::VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus)
	{
	switch (aStatus)
        {
    case CHWRMVibra::EVibraStatusUnknown:
        INFO_PRINTF1(_L("Vibra state changed: EVibraStatusUnknown"));
        break;
    case CHWRMVibra::EVibraStatusNotAllowed:
        INFO_PRINTF1(_L("Vibra state changed: EVibraStatusNotAllowed"));
        break;
    case CHWRMVibra::EVibraStatusStopped:
        INFO_PRINTF1(_L("Vibra state changed: EVibraStatusStopped"));        
        break;
    case CHWRMVibra::EVibraStatusOn:
        INFO_PRINTF1(_L("Vibra state changed: EVibraStatusOn"));
        break;
    default:
        INFO_PRINTF1(_L("Vibra state changed: UNDEFINED !"));
        break;
        }
        
   	if (iExpectedVibraStatusNotifications.Count() > 0)
  		{
  		if (aStatus != iExpectedVibraStatusNotifications[0])
  			{
	  		INFO_PRINTF3(_L("### ERROR: VibraStatusChanged, expected:%d, actual:%d"),iExpectedVibraStatusNotifications[0],aStatus);
	  		SetTestFail();
  			}        
        iExpectedVibraStatusNotifications.Remove(0);        
 		}
 	else
 		{
 	  	INFO_PRINTF2(_L("### ERROR: VibraStatusChanged, expected:NONE, actual:%d"),aStatus);
	  	SetTestFail();
 		}
    CheckForEndOfTransition();
	}

void CHWRMTestBase::VibraFeedbackModeChanged(CHWRMVibra::TVibraFeedbackModeState aMode)
    {	
	switch(aMode)
	    {
		case CHWRMVibra::EVibraFeedbackModeUnknown:
			INFO_PRINTF1(_L("Vibra Feedback Mode changed to EVibraFeedbackModeUnknown"));			
		    break;		
		case CHWRMVibra::EVibraFeedbackModeON:
			INFO_PRINTF1(_L("Vibra Feedback Mode changed to EVibraFeedbackModeON"));					    
		    break;		
		case CHWRMVibra::EVibraFeedbackModeOFF:
			INFO_PRINTF1(_L("Vibra Feedback Mode changed to EVibraFeedbackModeOFF"));	
		    break;							
		default:
			INFO_PRINTF2(_L("Vibra Feedback Mode changed to %d"), aMode);
    	}
  	if (iExpectedVibraFeedbackModeNotifications.Count() > 0)
  		{
  		if (aMode != iExpectedVibraFeedbackModeNotifications[0])
  			{
	  		INFO_PRINTF3(_L("### ERROR: VibraFeedbackModeChanged, expected:%d, actual:%d"),iExpectedVibraModeNotifications[0],aMode);
	  		SetTestFail();
  			}
        iExpectedVibraFeedbackModeNotifications.Remove(0);
 		}
 	else
 		{
 	  	INFO_PRINTF2(_L("### ERROR: VibraFeedbackModeChanged, expected:NONE, actual:%d"), aMode);
	  	SetTestFail();
 		}
    CheckForEndOfTransition();    	    	
    }
    
void CHWRMTestBase::LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus)
	{
    switch (aStatus)
        {
	    case CHWRMLight::ELightOn:
	        INFO_PRINTF2(_L("Light state changed: ELightOn, target:%d"),aTarget);
	        break;
	    case CHWRMLight::ELightOff:
	        INFO_PRINTF2(_L("Light state changed: ELightOff, target:%d"),aTarget);
	        break;
	    case CHWRMLight::ELightBlink:
	        INFO_PRINTF2(_L("Light state changed: ELightBlink, target:%d"),aTarget);
	        break;
	    case CHWRMLight::ELightStatusUnknown:
	        INFO_PRINTF2(_L("Light state changed: ELightStatusUnknown, target:%d"),aTarget);
	        break;
	    default:
	        INFO_PRINTF1(_L("Light state changed: UNDEFINED !"));
	        break;
        }

	if (iExpectedLightNotifications.Count() > 0)
		{
		if 	( 
  				(aStatus != iExpectedLightNotifications[0].iStatus || aTarget != iExpectedLightNotifications[0].iTarget)
  			&&	(IgnoringStatusNotifications() == EFalse) && (iTearingDown == EFalse)
  		    )
  			{
	  		INFO_PRINTF5(_L("### ERROR: LightStatusChanged, expected: target=%d, status=%d, actual: target=%d, status=%d"),
	  			iExpectedLightNotifications[0].iTarget,iExpectedLightNotifications[0].iStatus, aTarget, aStatus);
	  		SetTestFail();
  			}
  		else
  			{
  			INFO_PRINTF3(_L("INFO: LightStatusChanged, target=%d, status=%d"),aTarget,aStatus);
  			}
  		iExpectedLightNotifications.Remove(0);
		}
		
  		
	// ignore notifications during teardown	or if we have chosen to do so
 	else if ( (iTearingDown == EFalse) && (IgnoringStatusNotifications() == EFalse) )
 		{
 	  	INFO_PRINTF3(_L("### ERROR: LightStatusChanged, expected:NONE, actual: target=%d, status=%d"),aTarget,aStatus);
	  	SetTestFail();
 		}
 	else
 		{
 		INFO_PRINTF3(_L("INFO: LightStatusChanged, target=%d, status=%d"),aTarget,aStatus);
 		}
 		
    CheckForEndOfTransition();
	}

void CHWRMTestBase::LightStatusChanged(TInt aTarget, CHWRMExtendedLight::TLightStatus aStatus)
	{
    switch (aStatus)
        {
	    case CHWRMExtendedLight::ELightOn:
	        INFO_PRINTF2(_L("Light state changed: ELightOn, target:%d"),aTarget);
	        break;
	    case CHWRMExtendedLight::ELightOff:
	        INFO_PRINTF2(_L("Light state changed: ELightOff, target:%d"),aTarget);
	        break;
	    case CHWRMExtendedLight::ELightBlink:
	        INFO_PRINTF2(_L("Light state changed: ELightBlink, target:%d"),aTarget);
	        break;
	    case CHWRMExtendedLight::ELightStatusUnknown:
	        INFO_PRINTF2(_L("Light state changed: ELightStatusUnknown, target:%d"),aTarget);
	        break;
	    default:
	        INFO_PRINTF1(_L("Light state changed: UNDEFINED !"));
	        break;
        }
        
	if (iExpectedExtLightNotifications.Count() > 0)
		{
		if 	( 
  				(aStatus != iExpectedExtLightNotifications[0].iStatus || aTarget != iExpectedExtLightNotifications[0].iTarget)
  			&&	(IgnoringStatusNotifications() == EFalse) && (iTearingDown == EFalse)
  		    )
  			{
	  		INFO_PRINTF5(_L("### ERROR: LightStatusChanged, expected: target=%d, status=%d, actual: target=%d, status=%d"),
	  			iExpectedLightNotifications[0].iTarget,iExpectedLightNotifications[0].iStatus, aTarget, aStatus);
	  		SetTestFail();
  			}
  		else
  			{
  			INFO_PRINTF3(_L("INFO: LightStatusChanged, target=%d, status=%d"),aTarget,aStatus);
  			}
		iExpectedExtLightNotifications.Remove(0);
		}
	// ignore notifications during teardown	or if we have chosen to do so
 	else if ( (iTearingDown == EFalse) && (IgnoringStatusNotifications() == EFalse) )
 		{
 	  	INFO_PRINTF3(_L("### ERROR: LightStatusChanged, expected:NONE, actual: target=%d, status=%d"),aTarget,aStatus);
	  	SetTestFail();
 		}
 	else
 		{
 		INFO_PRINTF3(_L("INFO: LightStatusChanged, target=%d, status=%d"),aTarget,aStatus);
 		}
 		
    CheckForEndOfTransition();
	}
	
void CHWRMTestBase::FmTxStatusChanged(TFmTxState aStatus)
	{
	switch (aStatus)
        {
    case EFmTxStateUnknown:
        INFO_PRINTF1(_L("FmTx state changed: EFmTxStateUnknown"));
        break;
    case EFmTxStateOff:
        INFO_PRINTF1(_L("FmTx state changed: EFmTxStateOff"));
        break;
    case EFmTxStateActive:
        INFO_PRINTF1(_L("FmTx state changed: EFmTxStateActive"));
        break;
    case EFmTxStateInactive:
        INFO_PRINTF1(_L("FmTx state changed: EFmTxStateInactive"));
        break;
    case EFmTxStatePowerSaveInactivity:
    	INFO_PRINTF1(_L("FmTx state changed: EFmTxStatePowerSaveInactivity"));
    	break;
    case EFmTxStatePowerSaveAccessory:
		INFO_PRINTF1(_L("FmTx state changed: EFmTxStatePowerSaveAccessory"));
		break;
    case EFmTxStateScanning:
		INFO_PRINTF1(_L("FmTx state changed: EFmTxStateScanning"));
		break;
    default:
        INFO_PRINTF1(_L("FmTx state changed: UNDEFINED !"));
        break;
        }
	
   	if (iExpectedFmTxStatusNotifications.Count() > 0)
  		{
  		if (aStatus != iExpectedFmTxStatusNotifications[0].iState1 && aStatus != iExpectedFmTxStatusNotifications[0].iState2)
  			{
	  		INFO_PRINTF4(_L("### ERROR: FmTxStatusChanged, expected:%d OR %d, actual:%d"),iExpectedFmTxStatusNotifications[0].iState1,
	  				iExpectedFmTxStatusNotifications[0].iState2,aStatus);
	  		SetTestFail();
  			}
        iExpectedFmTxStatusNotifications.Remove(0);
 		}
 	else
 		{
 	  	INFO_PRINTF2(_L("### ERROR: FmTxStatusChanged, expected:NONE, actual:%d"),aStatus);
	  	SetTestFail();
 		}
    CheckForEndOfTransition();
	}

void CHWRMTestBase::FmTxFrequencyChanged(TInt aFrequency)
 	{
 	INFO_PRINTF2(_L("FmTx Frequency Changed. New Frequency = %d "),aFrequency);

    if (iExpectedFmTxFrequencyNotifications.Count() > 0)
    	{
  		if (aFrequency != iExpectedFmTxFrequencyNotifications[0])
  			{
	  		INFO_PRINTF3(_L("### ERROR: FmTxFrequencyChanged, expected:%d OR %d, actual:%d"),
	  				iExpectedFmTxFrequencyNotifications[0],aFrequency);
	  		SetTestFail();
  			}
  		iExpectedFmTxFrequencyNotifications.Remove(0);
 		}
    	else
    		{
    		INFO_PRINTF2(_L("### ERROR: FmTxFrequencyChanged, expected:NONE, actual:%d"),aFrequency);
    		SetTestFail();
    		}

    CheckForEndOfTransition();
 	}

	
void CHWRMTestBase::PsPropertyChanged(TInt aKey, TInt aValue)
 	{
 	INFO_PRINTF3(_L("PsPropertyChanged: [key=%d, value=%d]"), aKey, aValue);
  	
  	switch (aKey)
  		{
  	case KHWRMBatteryLevel:
  		BatteryLevelChanged(aValue);
  		break;
	case KHWRMBatteryStatus:
		BatteryStatusChanged(aValue);
		break;
	case KHWRMChargingStatus:
        ChargingStatusChanged(aValue);
        break;
	case KHWRMExtendedBatteryStatus:
	    ExtendedBatteryStatusChanged(aValue);
	    break;	
	case KHWRMExtendedChargingStatus:
        ExtendedChargingStatusChanged(aValue);
        break;		
	default:
		break;
  		}
  	}	

void CHWRMTestBase::BatteryLevelChanged(TInt aBatteryLevel)
	{
  	if (iExpectedBatteryLevelNotifications.Count() > 0)
  		{
  		if (aBatteryLevel != iExpectedBatteryLevelNotifications[0])
  			{
	  		INFO_PRINTF3(_L("### ERROR: BatteryLevelChanged, expected:%d, actual:%d"),iExpectedBatteryLevelNotifications[0],aBatteryLevel);
	  		SetTestFail();
  			}
        iExpectedBatteryLevelNotifications.Remove(0);
 		}
 	else
 		{
 	  	INFO_PRINTF2(_L("### ERROR: BatteryLevelChanged, expected:NONE, actual:%d"),aBatteryLevel);
	  	SetTestFail();
 		}
    CheckForEndOfTransition();
 	}
 
 void CHWRMTestBase::BatteryStatusChanged(TInt aBatteryStatus)
	{
  	if (iExpectedBatteryStatusNotifications.Count() > 0)
  		{
  		if (aBatteryStatus != iExpectedBatteryStatusNotifications[0])
  			{
	  		INFO_PRINTF3(_L("### ERROR: BatteryStatusChanged, expected:%d, actual:%d"),iExpectedBatteryStatusNotifications[0],aBatteryStatus);
	  		SetTestFail();
  			}
        iExpectedBatteryStatusNotifications.Remove(0);
 		}
 	else
 		{
 	  	INFO_PRINTF2(_L("### ERROR: BatteryStatusChanged, expected:NONE, actual:%d"),aBatteryStatus);
	  	SetTestFail();
 		}
    CheckForEndOfTransition();
 	}
 
 void CHWRMTestBase::ExtendedBatteryStatusChanged(TInt aBatteryStatus)
    {
    if (iExpectedExtendedBatteryStatusNotifications.Count() > 0)
        {
        if (aBatteryStatus != iExpectedExtendedBatteryStatusNotifications[0])
            {
            INFO_PRINTF3(_L("### ERROR: ExtendedBatteryStatusChanged, expected:%d, actual:%d"),iExpectedExtendedBatteryStatusNotifications[0],aBatteryStatus);
            SetTestFail();
            }
        iExpectedExtendedBatteryStatusNotifications.Remove(0);
        }
    else
        {
        INFO_PRINTF2(_L("### ERROR: ExtendedBatteryStatusChanged, expected:NONE, actual:%d"),aBatteryStatus);
        SetTestFail();
        }
    CheckForEndOfTransition();
    }

  void CHWRMTestBase::ChargingStatusChanged(TInt aChargingStatus)
	{
  	if (iExpectedChargingStatusNotifications.Count() > 0)
  		{
  		if (aChargingStatus != iExpectedChargingStatusNotifications[0])
  			{
	  		INFO_PRINTF3(_L("### ERROR: ChargingStatusChanged, expected:%d, actual:%d"),iExpectedChargingStatusNotifications[0],aChargingStatus);
	  		SetTestFail();
  			}
        iExpectedChargingStatusNotifications.Remove(0);
 		}
 	else
 		{
 	  	INFO_PRINTF2(_L("### ERROR: ChargingStatusChanged, expected:NONE, actual:%d"),aChargingStatus);
	  	SetTestFail();
 		}
    CheckForEndOfTransition();
 	}

  void CHWRMTestBase::ExtendedChargingStatusChanged(TInt aChargingStatus)
    {
    if (iExpectedExtendedChargingStatusNotifications.Count() > 0)
        {
        if (aChargingStatus != iExpectedExtendedChargingStatusNotifications[0])
            {
            INFO_PRINTF3(_L("### ERROR: ExtendedChargingStatusChanged, expected:%d, actual:%d"),iExpectedExtendedChargingStatusNotifications[0],aChargingStatus);
            SetTestFail();
            }
        iExpectedExtendedChargingStatusNotifications.Remove(0);
        }
    else
        {
        INFO_PRINTF2(_L("### ERROR: ExtendedChargingStatusChanged, expected:NONE, actual:%d"),aChargingStatus);
        SetTestFail();
        }
    CheckForEndOfTransition();
    }
  
void CHWRMTestBase::CheckPluginInsensitivity()
	{
	TInt value;
	iLightIntensityProperty.Get(value);
	if (value != iLightDefaultIntensity)
		{
		INFO_PRINTF3(_L("### FAILURE: Light plugin has invalid default intensity (%d != %d)"), iLightDefaultIntensity, value);
		User::Leave(KErrGeneral);
		}
	
	iLightSensitivityProperty.Get(value);
    //if (iSensorSupported)
    //	{
    	if (value != iLightDefaultSensitivity)
    		{
    		INFO_PRINTF3(_L("### FAILURE: Light plugin has invalid default sensitivity (%d != %d)"), iLightDefaultSensitivity, value);
    		User::Leave(KErrGeneral);
    		}
    //	}
    //else
    //	{
    	// Skipped as there is no sensor enabled in features
    //	INFO_PRINTF1(_L("### Skipped sensor sensitivity check, as sensor is not supported"));
    //	}
	}
	
void CHWRMTestBase::ExecuteTestL()
	{
	iEndStateTimer->Start(0);	//start the test with an immmediate callback
	CActiveScheduler::Start();
	
	if (iTestResult)
		{
		TRAP_IGNORE(this->TearDownL()); // teardown is not called by TEFUnit if the test leave !!
		User::Leave(iTestResult);
		}
	}
	
void CHWRMTestBase::CheckForEndOfTransition()
	{
	TBool isDone = ETrue;
	
	// Check SY events
	TBool SYHasWaitingEvent;
	TBool SYHasPendingEvent;
	TBool SYHasError;
	if (iIsMockLightSessionOpen)
		{
		iMockLight.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasPendingEvent)
			isDone = EFalse;
		}
	if (iIsMockVibraSessionOpen)
		{
		iMockVibra.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasPendingEvent)
			isDone = EFalse;
		}
	
	if (iIsMockPowerStateSessionOpen)
		{
		iMockPowerState.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasPendingEvent)
			isDone = EFalse;
		}
	
	if (iIsMockPowerSessionOpen)
		{
		iMockPower.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasPendingEvent)
			isDone = EFalse;
		}

	if (iIsMockFmTxSessionOpen)
		{
		iMockFmTx.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasPendingEvent)
			isDone = EFalse;
		}
	
	// Check if we are waiting for some notification
	if(		iExpectedVibraStatusNotifications.Count() 			!=0 
		|| 	iExpectedVibraModeNotifications.Count() 			!=0 
		|| 	iExpectedLightNotifications.Count() 				!=0
		|| 	iExpectedBatteryLevelNotifications.Count()			!=0
		|| 	iExpectedBatteryStatusNotifications.Count()			!=0
		||  iExpectedExtendedBatteryStatusNotifications.Count() !=0
		|| 	iExpectedChargingStatusNotifications.Count()		!=0
        ||  iExpectedExtendedChargingStatusNotifications.Count()!=0
		||  iExpectedBatteryPowerMeasurementsError.Count()		!=0
		||  iExpectedFmTxStatusNotifications.Count() 			!=0
		||  iExpectedFmTxFrequencyNotifications.Count()			!=0
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
		||  iExpectedBatteryChargingTimeMeasurementsError.Count() !=0
		||  iExpectedBatteryChargingCurrentMeasurementsError.Count() !=0
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	  )
		{
		isDone = EFalse;
		}
		
	// Stop the timer if everybody is done
	if (isDone)
		{
		iEndStateTimer->DoCancel();
		}
	}
	

void CHWRMTestBase::SetupL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("HWRM - Setup."));
	
	//zero arrays
	memset(&iVibraSessions, 0, KMaxNoOpenSessionsForResource*iVibraSessions.Length());
	memset(&iLightSessions, 0, KMaxNoOpenSessionsForResource*iLightSessions.Length());
	memset(&iPowerSessions, 0, KMaxNoOpenSessionsForResource*iPowerSessions.Length());
	memset(&iFmTxSessions, 0, KMaxNoOpenSessionsForResource*iFmTxSessions.Length());
	memset(&iExtLightSessions, 0, KMaxNoOpenSessionsForResource*iExtLightSessions.Length());
	memset(&iEnhancedLightSessions, 0, KMaxNoOpenSessionsForResource*iEnhancedLightSessions.Length());
	

	ResetDefaultsL();
	
	//set default values
	iLightDefaultIntensity = 50;
	iLightDefaultSensitivity = 55;
	iLightSensorEnabled = 0;
	
    iLightIntensityProperty.Attach(KPSUidHWResourceNotification, KHWRMTestLightIntensityValue);
    iLightSensitivityProperty.Attach(KPSUidHWResourceNotification, KHWRMTestLightSensitivityValue);   
    	
    iPowerAttributesMeasurementProcessingTime = 0;
    
#ifdef INSECURE_AUDIO_POLICY_KEYS

    // Test module mimics the behaviour of Audio Policy    
    // WARNING - HWRM must be built with INSECURE_AUDIO_POLICY_KEYS defined, else KErrPermissionDenied
    iAudioRoutedProperty.Attach(KPSUidHWRMFmTx, KHWRMFmTxAudioRoutedFlag);

#endif // INSECURE_AUDIO_POLICY_KEYS

	iScheduler = new CActiveScheduler;

    if ( iScheduler )
        {
        CActiveScheduler::Install( iScheduler );
        } 
	iEndStateTimer = CEndStateTimer::NewL(*this);
	}
	
const TDesC& CHWRMTestBase::MapVibraModeState(CHWRMVibra::TVibraModeState aState)
	{
	switch(aState)
		{
	case CHWRMVibra::EVibraModeON:
		return KVibraModeON;
	case CHWRMVibra::EVibraModeOFF:
		return KVibraModeOFF;
	default:
		return KVibraModeUnknown;
		}
	}
	
const TDesC& CHWRMTestBase::MapVibraModeStatus(CHWRMVibra::TVibraStatus aStatus)
	{
	 
	switch(aStatus)
		{
	case CHWRMVibra::EVibraStatusNotAllowed:
		return KVibraStatusNotAllowed;
	case CHWRMVibra::EVibraStatusStopped:
		return KVibraStatusStopped;
	case CHWRMVibra::EVibraStatusOn:
		return KVibraStatusOn;
	default:
		return KVibraStatusUnknown;
		}
	}
	
	

const TDesC& CHWRMTestBase::MapLightStatus(CHWRMLight::TLightStatus aStatus)
	{
	 
	switch(aStatus)
		{
	case CHWRMLight::ELightOn:
		return KLightOn;
	case CHWRMLight::ELightOff:
		return KLightOff;
	case CHWRMLight::ELightBlink:
		return KLightBlink;
	default:
		return KLightStatusUnknown;
		}
	}
		
const TDesC& CHWRMTestBase::MapFmTxStatus(TFmTxState aStatus)
	{ 
	switch(aStatus)
		{
	case EFmTxStateOff:
		return KFmTxStateOff;
	case EFmTxStateActive:
		return KFmTxStateActive;
	case EFmTxStateInactive:
		return KFmTxStateInactive;
	case EFmTxStatePowerSaveInactivity:
		return KFmTxStatePowerSaveInactivity;
	case EFmTxStatePowerSaveAccessory:
		return KFmTxStatePowerSaveAccessory;
	case EFmTxStateScanning:
		return KFmTxStateScanning;
	default:
		return KFmTxStateUnknown;
		}
	}

void CHWRMTestBase::TearDownL()
	{
	iTearingDown = ETrue;
	if (iIsMockLightSessionOpen)
		{
		iMockLight.Close();
		iIsMockLightSessionOpen = EFalse;
		}

	if (iIsMockVibraSessionOpen)
		{
		iMockVibra.Close();
		iIsMockVibraSessionOpen = EFalse;
		}

	if (iIsMockPowerStateSessionOpen)
		{
		iMockPowerState.Close();
		iIsMockPowerStateSessionOpen = EFalse;
		}
		
	if (iIsMockPowerSessionOpen)
		{
		iMockPower.Close();
		iIsMockPowerSessionOpen = EFalse;
		}

	if (iIsMockFmTxSessionOpen)
		{
		iMockFmTx.Close();
		iIsMockFmTxSessionOpen = EFalse;
		}

	//log light and vibra states on tear down.

	if(iVibraSessions[0])
		{
		CHWRMVibra& vibra = *iVibraSessions[0];
		CHWRMVibra::TVibraModeState state = vibra.VibraSettings();
		CHWRMVibra::TVibraStatus status = vibra.VibraStatus();
		INFO_PRINTF3(_L("Teardown - TVibraModeState=%S, TVibraStatus=%S"), &MapVibraModeState(state), &MapVibraModeStatus(status));		
		}
		
	if(iLightSessions[0])
		{
		CHWRMLight& light = *iLightSessions[0];
		TUint sup_targets = light.SupportedTargets();
		TUint targetsOnMask(0);
		INFO_PRINTF2(_L("Teardown - Light supported targets bitmask=%d"), sup_targets);		
		TUint target = 0x00000001;
		CHWRMLight::TLightStatus st;
		do
			{
			if(target & sup_targets)
				{
				st = light.LightStatus(target);
				INFO_PRINTF3(_L("Light target %d, state=%S"), target, &MapLightStatus(st));		
				if (st == CHWRMLight::ELightOn)
					{
					targetsOnMask |= target;
					}
				}
			if(target == CHWRMLight::ESystemTarget)
				{
				break;
				}
				
			target  = target << 1;			
			}
		while(ETrue);
		
		if (targetsOnMask != 0)
			{
			INFO_PRINTF2(_L("Teardown - Going to switch off targets bitmask=%d"), targetsOnMask);
			light.LightOffL(targetsOnMask);
			}
		}
	
	if(iFmTxSessions[0])
		{
		CHWRMFmTx& fmTx = *iFmTxSessions[0];
		TInt frequency = fmTx.Frequency();
		TFmTxState status = fmTx.Status();
		INFO_PRINTF3(_L("Teardown - TFmTxState=%S, FmTxFrequency=%d"), &MapFmTxStatus(status), frequency);		
		}
			
	for(TInt i = 0; i<KMaxNoOpenSessionsForResource;i++)
		{
		delete iVibraSessions[i];
		delete iLightSessions[i];
		delete iExtLightSessions[i];
		delete iPowerSessions[i];
		delete iFmTxSessions[i];
		delete iEnhancedLightSessions[i];
		}
		
	delete iScheduler;
	delete iEndStateTimer;
	for (TInt i=0; i< iStepStates.Count(); i++)
		{
		delete iStepStates[i];
		}
	iStepStates.Close();
	iExpectedVibraStatusNotifications.Close();
	iExpectedVibraModeNotifications.Close();
	iExpectedLightNotifications.Close();
	iExpectedExtLightNotifications.Close();
	iExpectedFmTxStatusNotifications.Close();
	iExpectedFmTxFrequencyNotifications.Close();
	iExpectedBatteryLevelNotifications.Close();
	iExpectedBatteryStatusNotifications.Close();
	iExpectedExtendedBatteryStatusNotifications.Close();
	iExpectedChargingStatusNotifications.Close();
	iExpectedExtendedChargingStatusNotifications.Close();
	iExpectedVibraFeedbackModeNotifications.Close();
	iExpectedBatteryPowerMeasurementsError.Close();
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	iExpectedBatteryChargingTimeMeasurementsError.Close();
	iExpectedBatteryChargingCurrentMeasurementsError.Close();
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	iLightIntensityProperty.Close();
	iLightSensitivityProperty.Close();
	
#ifdef INSECURE_AUDIO_POLICY_KEYS
	iAudioRoutedProperty.Close();
#endif // INSECURE_AUDIO_POLICY_KEYS
	    		
	CleanupPropertyObservers();
	iTearingDown = EFalse;
	__UHEAP_MARKEND;
	}

void CHWRMTestBase::ResetDefaultsL()
	{
	// Vibra send sync notification when the CHWRMVibra object is create
	// so we have to queue those expected notification before opening the session
	TInt value;
	GetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled, value);
	INFO_PRINTF2(_L("KVibraCtrlProfileVibraEnabledvalue = %d"), value);
	
	if (value == 0)
		{
		SetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled, 1);
		GetCRVibraAttributeL(KVibraCtrlProfileVibraEnabled, value);
		INFO_PRINTF2(_L("KVibraCtrlProfileVibraEnabledvalue updated = %d"), value);
		}
	}

	
void CHWRMTestBase::EndStateTimerExpired(TInt /*aError*/)
	{
	
	//the timer has triggered for the end of a state. There are two things we have to do
	//1) check to see if all expected notifications were received
	//2) move to the next state (or end if no more states to process)
	
	INFO_PRINTF2(_L("EndStateTimerExpired. Current State = %d"), iCurrentState);	
	
	CheckMockSYResultL();
	CheckAllExpectedNotificationsReceived();
	
	//move to next state or finish if no more states to go to
	TBool bFinished = EFalse;
	TInt err = KErrNone;
	//is there another state to move to?
	
	if(iCurrentState < iStepStates.Count())
		{
		// execute the test state	
		TRAP(err, iStepStates[iCurrentState]->ExecuteTestState());
		
		// Expected leave are not errors
		if (err == iExpectedLeaveCode)
			{
			err = KErrNone;
			iExpectedLeaveCode = KErrNone;
			}
		else
			{
			INFO_PRINTF3(_L("### ERROR Test step left with error %d, expected error %d"), err, iExpectedLeaveCode);
			SetTestFail();
			}
		if(err)
			{	
			SetTestFail();
			}
		else
			{				
			//start timer to end test
			iEndStateTimer->Start(iStepStates[iCurrentState]->iMaxDurationInSecs);
			}
		}
	else
		{
		bFinished=ETrue; //no more transitions to go through, so finish step
		}
		
	if(bFinished || iTestResult)
		{
		//test has finished so stop scheduler
		CActiveScheduler::Stop();		
		}
	//move to next state
	iCurrentState++;
	
	}

/**
This method is called by the ASSERT_LEAVE macro. previous statement is expected to leave, so if it is executed
it's an error.
*/
void CHWRMTestBase::EndExpectLeaveL(TInt aErr, const TText8* aFile,TInt aLine)
	{
	TPtrC8 fileNamePtrC8(aFile);
	TBuf<100> fname;
	fname.Copy(fileNamePtrC8);
	INFO_PRINTF4(_L("### ERROR statement was expected to leave with error %d (File=%S, line=%d)"), aErr, &fname, aLine);
	User::Leave(KErrTEFUnitFail);	
	}

	
void CHWRMTestBase::CheckAllExpectedNotificationsReceived()
	{
	/*
	if (IgnoringStatusNotifications() == ETrue)
		{
		return;
		}
	*/
	//if these arrays are non zero it implies that notifications were expected but not received which means a failure of the test
	if (iExpectedVibraStatusNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Vibra Status Notification not received"));
		}
	if (iExpectedVibraModeNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Vibra Mode Notification not received"));
		}
	if (iExpectedVibraFeedbackModeNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Vibra Feedback Mode Notification not received"));
		}
	if (iExpectedLightNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Light Status Notification not received"));
		}
	if (iExpectedBatteryLevelNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Battery Level Notification not received"));
		}
	if (iExpectedBatteryStatusNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Battery Status Notification not received"));
		}
	if (iExpectedExtendedBatteryStatusNotifications.Count() != 0)
        {
        SetTestFail();
        INFO_PRINTF1(_L("### ERROR Expected Extended Battery Status Notification not received"));
        }
	if (iExpectedChargingStatusNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Charging Status Notification not received"));
		}
	if (iExpectedExtendedChargingStatusNotifications.Count() != 0)
        {
        SetTestFail();
        INFO_PRINTF1(_L("### ERROR Expected Extended Charging Status Notification not received"));
        }
	if (iExpectedBatteryPowerMeasurementsError.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Battery Power Measurements Error not received"));
		}
	if (iExpectedFmTxFrequencyNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected FmTx Frequency Notification not received"));
		}
	if (iExpectedFmTxStatusNotifications.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected FmTx Status Notification not received"));
		}
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	if (iExpectedBatteryChargingTimeMeasurementsError.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Remaining charging time error notification not received"));
		}	
	if (iExpectedBatteryChargingCurrentMeasurementsError.Count() != 0)
		{
		SetTestFail();
		INFO_PRINTF1(_L("### ERROR Expected Charging current error notification not received"));
		}
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	}

void CHWRMTestBase::CheckMockSYResultL()
	{
	TBool SYHasWaitingEvent;
	TBool SYHasPendingEvent;
	TBool SYHasError;
	TBuf<255> log;
	// Light
	if (iIsMockLightSessionOpen)
		{
		iMockLight.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasWaitingEvent)
			{
			INFO_PRINTF1(_L("### ERROR MockLight SY didn't receive all expected commands"));
			}
		if (SYHasError || SYHasWaitingEvent)
			{
			SetTestFail();
			do	{
				iMockLight.GetNextLogLine(log);
				INFO_PRINTF1(log);
				} while(log.Length() > 0);
			}
			
		}
	// Vibra	
	if (iIsMockVibraSessionOpen)
		{
		iMockVibra.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasWaitingEvent)
			{
			INFO_PRINTF1(_L("### ERROR MockVibra SY didn't receive all expected commands"));
			}
		if (SYHasError || SYHasWaitingEvent)
			{
			SetTestFail();
			do	{
				iMockVibra.GetNextLogLine(log);
				INFO_PRINTF1(log);
				} while(log.Length() > 0);
			}
		}
	// Power State
	if (iIsMockPowerStateSessionOpen)
		{
		iMockPowerState.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasWaitingEvent)
			{
			INFO_PRINTF1(_L("### ERROR MockPowerState SY didn't receive all expected commands"));
			}
		if (SYHasError || SYHasWaitingEvent)
			{
			SetTestFail();
			do	{
				iMockPowerState.GetNextLogLine(log);
				INFO_PRINTF1(log);
				} while(log.Length() > 0);
			}
		}		
	// Power	
	if (iIsMockPowerSessionOpen)
		{
		iMockPower.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasWaitingEvent)
			{
			INFO_PRINTF1(_L("### ERROR MockPower SY didn't receive all expected commands"));
			}
		if (SYHasError || SYHasWaitingEvent)
			{
			SetTestFail();
			do	{
				iMockPower.GetNextLogLine(log);
				INFO_PRINTF1(log);
				} while(log.Length() > 0);
			}
		}

	// FmTx
	if (iIsMockFmTxSessionOpen)
		{
		iMockFmTx.GetStatus(SYHasWaitingEvent,SYHasPendingEvent,SYHasError);
		if (SYHasWaitingEvent)
			{
			INFO_PRINTF1(_L("### ERROR MockFmTx SY didn't receive all expected commands"));
			}
		if (SYHasError || SYHasWaitingEvent)
			{
			SetTestFail();
			do	{
				iMockFmTx.GetNextLogLine(log);
				INFO_PRINTF1(log);
				} while(log.Length() > 0);
			}
		}
	}

void CHWRMTestBase::GetTargetsOn(TInt& aTargetsOnMask) const
	{
	aTargetsOnMask = 0;
	if(iLightSessions[0])
		{
		CHWRMLight& light = *iLightSessions[0];
		TUint sup_targets = light.SupportedTargets();
		TUint target = 0x00000001;
		CHWRMLight::TLightStatus st;
		do
			{
			if(target & sup_targets)
				{
				st = light.LightStatus(target);
				if (st == CHWRMLight::ELightOn)
					{
					aTargetsOnMask |= target;
					}
				}
			if(target == CHWRMLight::ESystemTarget)
				{
				break;
				}
				
			target  = target << 1;			
			}
		while(ETrue);
		}
	}

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
void CHWRMTestBase::OpenPowerStateSessionforChrgStatusL()
	{
	if (!iIsMockPowerStateSessionOpen)
		{
		User::LeaveIfError(iMockPowerState.Connect());
		iIsMockPowerStateSessionOpen = ETrue;
		}
	}

void CHWRMTestBase::ChargingStatusChange(TInt aErrCode, CHWRMPower::TBatteryChargingStatus aChrgStatus)
	{
	if (aErrCode == KErrNone)
		{
		switch(aChrgStatus)
			{
			case EChargingStatusError:
				{
				INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusError"));
				break;
				}
			case EChargingStatusIllegalChargerError:
                {
                INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusIllegalChargerError"));
                break;
                }
            case EChargingStatusChargerUnderVoltageError:
                {
                INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusChargerUnderVoltageError"));
                break;
                }
            case EChargingStatusChargerOverVoltageError:
                {
                INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusChargerOverVoltageError"));
                break;
                }
            case EChargingStatusTempTooHotError:
                {
                INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusTempTooHotError"));
                break;
                }
            case EChargingStatusTempTooColdError:
                {
                INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusTempTooColdError"));
                break;
                }
			case EChargingStatusNotConnected:
				{
				INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusNotConnected"));
				break;
				}
			case EChargingStatusCharging:
				{
				INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusCharging"));
				break;
				}
			case EChargingStatusNotCharging:
				{
				INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusNotCharging"));
				break;
				}
			case EChargingStatusAlmostComplete:
				{
				INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusAlmostComplete"));
				break;
				}
			case EChargingStatusChargingComplete:
				{
				INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusChargingComplete"));
				break;
				}
			case EChargingStatusChargingContinued:
				{
				INFO_PRINTF1(_L("INFO: Battery charging status changed: EChargingStatusChargingContinued"));
				break;
				}
			default:
				{
				INFO_PRINTF1(_L("INFO: Battery charging status changed: UNDEFINED!"));
				}
			}
		
		if (iExpectedExtendedChargingStatusNotifications.Count() > 0)
			{
			if (aChrgStatus != iExpectedExtendedChargingStatusNotifications[0])
				{
				INFO_PRINTF3(_L("### ERROR: ChargingStatusChange, expected:%d, actual:%d"),iExpectedExtendedChargingStatusNotifications[0],aChrgStatus);
				SetTestFail();
				}
			iExpectedExtendedChargingStatusNotifications.Remove(0);
			}
		else
			{
			INFO_PRINTF2(_L("### ERROR: ChargingStatusChange, expected:NONE, actual:%d"),aChrgStatus);
			SetTestFail();
			}
		}
	}

void CHWRMTestBase::ExpectBatteryChargingTimeMeasurementsErrorL(TInt aErrorCode)
	{
	iExpectedBatteryChargingTimeMeasurementsError.AppendL(aErrorCode);
	}

void CHWRMTestBase::BatteryFullChargingTimeChange(TInt aErrorCode, TUint aTime)
	{
	if (aErrorCode == KErrNone)
		{
		INFO_PRINTF2(_L("INFO: Remaining charging time received, Time=%d minutes"),aTime);		
		}
	else
		{
		if (iExpectedBatteryChargingTimeMeasurementsError.Count() > 0)
	  		{
	  		if (aErrorCode != iExpectedBatteryChargingTimeMeasurementsError[0])
	  			{
		  		INFO_PRINTF3(_L("### ERROR: BatteryFullChargingTimeChange, expected error:%d, actual:%d"),iExpectedBatteryChargingTimeMeasurementsError[0],aErrorCode);
		  		SetTestFail();
	  			}
	  		iExpectedBatteryChargingTimeMeasurementsError.Remove(0);
	 		}
	 	else
	 		{
	 	  	INFO_PRINTF2(_L("### ERROR: BatteryFullChargingTimeChange, expected error:NONE, actual:%d"),aErrorCode);
		  	SetTestFail();
	 		}
		
		INFO_PRINTF2(_L("INFO: Remaining charging time Error, error=%d"),aErrorCode);
		}
	}

void CHWRMTestBase::SetPowerChargingCurrentMeasurementProcessingTime(TUint aCallbackProcessingTime)
	{
	iPowerChargingCurrentMeasurementProcessingTime = aCallbackProcessingTime;
	}

void CHWRMTestBase::ExpectBatteryChargingCurrentMeasurementsErrorL(TInt aErrorCode)
	{
	iExpectedBatteryChargingCurrentMeasurementsError.AppendL(aErrorCode);
	}

void CHWRMTestBase::AverageChargingCurrentChange(TInt aErrorCode, TInt aMeasurement)
	{
	if (aErrorCode == KErrNone)
		{
		INFO_PRINTF2(_L("INFO: Charging current Measurement Received, measurement=%d"),aMeasurement);
		
		// Simulate time spent on processing the response
		if (iPowerChargingCurrentMeasurementProcessingTime > 0 )
			{
			User::After(iPowerChargingCurrentMeasurementProcessingTime * 1000);
			}
		}
	else
		{
		if (iExpectedBatteryChargingCurrentMeasurementsError.Count() > 0)
	  		{
	  		if (aErrorCode != iExpectedBatteryChargingCurrentMeasurementsError[0])
	  			{
		  		INFO_PRINTF3(_L("### ERROR: ChargingCurrentChange, expected error:%d, actual:%d"),iExpectedBatteryChargingCurrentMeasurementsError[0],aErrorCode);
		  		SetTestFail();
	  			}
	  		iExpectedBatteryChargingCurrentMeasurementsError.Remove(0);
	 		}
	 	else
	 		{
	 	  	INFO_PRINTF2(_L("### ERROR: ChargingCurrentChange, expected error:NONE, actual:%d"),aErrorCode);
		  	SetTestFail();
	 		}
		
		INFO_PRINTF2(_L("INFO: Charging current Measurement Error, error=%d"),aErrorCode);
		}
	}
#endif //SYMBIAN_HWRM_EXTPOWERINFO

void CHWRMTestBase::ExpectBatteryPowerMeasurementsErrorL(TInt aErrorCode)
	{
	iExpectedBatteryPowerMeasurementsError.AppendL(aErrorCode);
	}

void CHWRMTestBase::SetPowerAttributesMeasurementProcessingTime(TUint aCallbackProcessingTime)
	{
	iPowerAttributesMeasurementProcessingTime = aCallbackProcessingTime;
	}
	
void CHWRMTestBase::PowerMeasurement(TInt aErrorCode, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement)
	{
	if (aErrorCode == KErrNone)
		{
		INFO_PRINTF3(_L("INFO: Power Attribute Measurement Received, measurement=(Voltage=%d, Current=%d)"),aMeasurement.iAverageVoltage, aMeasurement.iAverageCurrent);
		
		// Simulate time spent on processing the response
		if (iPowerAttributesMeasurementProcessingTime > 0 )
			{
			User::After(iPowerAttributesMeasurementProcessingTime * 1000);
			}
		if ((iExpectedBatteryPowerMeasurementsError.Count() > 0) && (iExpectedBatteryPowerMeasurementsError[0] == KErrNone))
	  		{
	  		iExpectedBatteryPowerMeasurementsError.Remove(0);
			}		
		}
	else
		{
		if (iExpectedBatteryPowerMeasurementsError.Count() > 0)
	  		{
	  		if (aErrorCode != iExpectedBatteryPowerMeasurementsError[0])
	  			{
		  		INFO_PRINTF3(_L("### ERROR: PowerMeasurement, expected error:%d, actual:%d"),iExpectedBatteryPowerMeasurementsError[0],aErrorCode);
		  		SetTestFail();
	  			}
	  		iExpectedBatteryPowerMeasurementsError.Remove(0);
	 		}
	 	else
	 		{
	 	  	INFO_PRINTF2(_L("### ERROR: PowerMeasurement, expected error:NONE, actual:%d"),aErrorCode);
		  	SetTestFail();
	 		}
		
		INFO_PRINTF2(_L("INFO: Power Attributes Measurement Error, error=%d"),aErrorCode);
		}
	}

