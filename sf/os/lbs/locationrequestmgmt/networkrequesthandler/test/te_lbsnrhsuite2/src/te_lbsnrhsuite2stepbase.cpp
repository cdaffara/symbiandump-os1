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
//

/**
 @file te_lbsnrhsuite2stepbase.cpp
*/
#include "lbscommoninternaldatatypes.h"
#include "lbsnrhngmsgs.h"
#include "lbsnetregstatusint.h"
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsadmin.h>
#include "te_lbsnrhsuite2stepbase.h"
#include "te_lbsnrhsuite2defs.h"


// Device driver constants

TVerdict CTe_LbsNrhSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	//INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPreambleL() of the class CTe_LbsNrhPrivacySuiteStepBase!"));
	
	User::SetJustInTime(ETrue);
	
	// Create Active Scheduler
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	iAdmin = CLbsAdmin::NewL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsNrhSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	//INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPostambleL() of the class CTe_LbsNrhSuiteStepBase!"));
	
	delete iAdmin;
	
	// remove active scheduler
	delete iScheduler;
	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_LbsNrhSuiteStepBase::~CTe_LbsNrhSuiteStepBase()
	{
	}

CTe_LbsNrhSuiteStepBase::CTe_LbsNrhSuiteStepBase()
	{
	}



void CTe_LbsNrhSuiteStepBase::GetDeviceGpsModeCapabilitiesFromIniL(TPositionModuleInfoExtended::TDeviceGpsModeCapabilities& aIntegrationModuleCaps)	
	{
	TInt val;
	if (GetIntFromConfig(ConfigSection(), KModuleCapsName, val))
		{
		aIntegrationModuleCaps = val;
		}
	else
		{
		aIntegrationModuleCaps = TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB;
		}
	}

/* Parse the .ini file for this test step to get LbaAdmin settings
 */

void CTe_LbsNrhSuiteStepBase::ParseAdminSettingsFromIniL()
	{	
	// Used throughout method
	TPtrC ptr;
	TInt n;
	
	// Locate settings
	#define LOCATECOUNT 9
	const TPtrC LocateStrings[LOCATECOUNT] =
        {
        KHomeExternalLocateKeyName(),
        KHomeNetworkInducedLocateKeyName(),
        KHomeEmergencyLocateKeyName(),
        KHomeSelfLocateKeyName(),
        KHomeTransmitLocateKeyName(),
        KRoamingTransmitLocateKeyName(),
        KRoamingExternalLocateKeyName(),
        KRoamingEmergencyLocateKeyName(),
        KRoamingSelfLocateKeyName(),
        };
	const TLbsAdminSetting LocateValues[LOCATECOUNT] =
		{
		KLbsSettingHomeExternalLocate,
		KLbsSettingHomeNetworkInducedLocate, 
		KLbsSettingHomeEmergencyLocate,
		KLbsSettingHomeSelfLocate,
		KLbsSettingHomeTransmitLocate,
		KLbsSettingRoamingTransmitLocate,
		KLbsSettingRoamingExternalLocate,
		KLbsSettingRoamingEmergencyLocate,
		KLbsSettingRoamingSelfLocate,
		};

	// Do the work
	for (TInt x = 0; x < LOCATECOUNT; x++)
		{
		TInt value;
		if (GetIntFromConfig(ConfigSection(), LocateStrings[x], value))
			{
			// We must cast value to the correct Enum type in order for the necessary overloading to take place within admin.
			switch(LocateValues[x])
			    {
			     case KLbsSettingHomeExternalLocate:
			     case KLbsSettingRoamingExternalLocate:
			     case KLbsSettingHomeNetworkInducedLocate:
			     case KLbsSettingRoamingNetworkInducedLocate:
			     case KLbsSettingHomeEmergencyLocate:
			            {
			            User::LeaveIfError(iAdmin->Set( LocateValues[x], (CLbsAdmin::TExternalLocateService)value));
			            break;
			            }
			     case KLbsSettingHomeSelfLocate:
			     case KLbsSettingRoamingSelfLocate:
                        {
                        User::LeaveIfError(iAdmin->Set( LocateValues[x], (CLbsAdmin::TSelfLocateService)value));
                        break;
                        }
                 case KLbsSettingHomeTransmitLocate:
                 case KLbsSettingRoamingTransmitLocate:
                        {
                        User::LeaveIfError(iAdmin->Set( LocateValues[x], (CLbsAdmin::TTransmitLocateService)value));
                        break;
                        }
			    }//switch
			} // if
		} // for
	
	// Quality Profiles
	#define QUALITYPROFILECOUNT 3
	const TPtrC QualityProfileStrings[QUALITYPROFILECOUNT] =
		{
		KQualityProfileSelfLocateKeyName(),
		KQualityProfileExternalLocateKeyName(),
		KQualityProfileTransmitLocateKeyName(),
		};
	const TInt QualityProfileValues[QUALITYPROFILECOUNT] =
		{
		KLbsSettingQualityProfileSelfLocate,
		KLbsSettingQualityProfileExternalLocate,
		KLbsSettingQualityProfileTransmitLocate,
		};
	// Do the work
	for (TInt x = 0; x < QUALITYPROFILECOUNT; x++)
		{
		TInt profileId = 0;
		if (GetIntFromConfig(ConfigSection(), QualityProfileStrings[x], profileId))
			{
			User::LeaveIfError(iAdmin->Set(QualityProfileValues[x], TLbsQualityProfileId(profileId)));
			}
		} // for
	
	// Look to see if there are any special admin settings
	if (GetIntFromConfig(ConfigSection(), KPrivacyTimeoutActionName, n))
		{
		User::LeaveIfError(iAdmin->Set(KLbsSettingPrivacyTimeoutAction, (CLbsAdmin::TPrivacyTimeoutAction)n));
		}
		
	if (GetStringFromConfig(ConfigSection(), KPrivacyMechanismName, ptr))
		{
		CLbsAdmin::TPrivacyHandler privacyHandler(CLbsAdmin::EPrivacyHandleByController);
		User::LeaveIfError(ConvertStringToPrivacyMechanism(ptr, privacyHandler));
		User::LeaveIfError(iAdmin->Set(KLbsSettingPrivacyHandler, privacyHandler));
		}

	// Maximum number of concurrent external locate (MT-LR/privacy) requests
	TInt numRequests(1);
	if (GetIntFromConfig(ConfigSection(), KMaximumExternalLocateRequests, numRequests))
		{
		User::LeaveIfError(iAdmin->Set(KLbsSettingMaximumExternalLocateRequests, (TUint)numRequests));
		}	
	

	CLbsAdmin::TGpsMode mode;
	TInt intMode;
	
	if (GetIntFromConfig(ConfigSection(), KAGPSModeName, intMode))
			{
			mode = (CLbsAdmin::TGpsMode)intMode; 
			TInt err = iAdmin->Set(KLbsSettingHomeGpsMode, mode);	
			}
	
	if (GetIntFromConfig(ConfigSection(), KAGPSRoamingModeName, intMode))
			{
			mode = (CLbsAdmin::TGpsMode)intMode; 
			TInt err = iAdmin->Set(KLbsSettingRoamingGpsMode, mode);	
			}	
	
	if (GetIntFromConfig(ConfigSection(), KRegistrationStatusName, intMode))
			{
			RLbsNetworkRegistrationStatus netRegStatus;
			netRegStatus.OpenL();
			CleanupClosePushL(netRegStatus);
			RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus regStatus;
			regStatus = static_cast<RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus>(intMode);
			netRegStatus.SetNetworkRegistrationStatus(regStatus);
			CleanupStack::PopAndDestroy(&netRegStatus);
			}	
	
	TLbsTimeIntervalMilliSeconds appTimeout(2000);
	iAdmin->Set(KLbsSettingPrivacyAppTimeout, appTimeout);

	}

/* Converts a string (i.e. from an .ini file) into the enum value used
   for the privacy notification value in LbsAdmin.
*/
TInt CTe_LbsNrhSuiteStepBase::ConvertStringToPrivacyMechanism(
		const TDesC& aString, 
		CLbsAdmin::TPrivacyHandler& aPrivacyMechanism)
	{
	_LIT(KEPrivacyHandleByNotifier, "Notifier");
	_LIT(KEPrivacyHandleByController, "Controller");
	_LIT(KEPrivacyHandleByAdvancedNotifier, "AdvancedNotifier");

	TInt err(KErrNone);
	
	if (aString == KEPrivacyHandleByNotifier)
		{
		aPrivacyMechanism = CLbsAdmin::EPrivacyHandleByNotifier;
		}
	else
		{
		if (aString == KEPrivacyHandleByController)
			{
			aPrivacyMechanism = CLbsAdmin::EPrivacyHandleByController;
			}
		else
			{
			if (aString == KEPrivacyHandleByAdvancedNotifier)
				{
				aPrivacyMechanism = CLbsAdmin::EPrivacyHandleByAdvancedNotifier;
				}
			else
				{
				err = KErrNotFound;
				}
			}
		}
	
	return err;
	}
	
/* Converts a string (i.e. from an .ini file) into the enum value used
   for external locate settings in LbsAdmin.
*/
TInt CTe_LbsNrhSuiteStepBase::ConvertStringToExternalLocateService(
		const TDesC& aString, 
		CLbsAdmin::TExternalLocateService& aExternalLocate)
	{
	_LIT(KEExternalLocateOn, "EExternalLocateOn");
	_LIT(KEExternalLocateOnButAlwaysVerify, "EExternalLocateOnButAlwaysVerify");
	_LIT(KEExternalLocateOff, "EExternalLocateOff");
	_LIT(KEExternalLocateOffButNotify, "EExternalLocateOffButNotify");
	TInt err(KErrNone);
	
	if (aString == KEExternalLocateOn)
		{
		aExternalLocate = CLbsAdmin::EExternalLocateOn;
		}
	else if (aString == KEExternalLocateOnButAlwaysVerify)
		{
		aExternalLocate = CLbsAdmin::EExternalLocateOnButAlwaysVerify;
		}
	else if (aString == KEExternalLocateOff)
		{
		aExternalLocate = CLbsAdmin::EExternalLocateOff;
		}
	else if (aString == KEExternalLocateOffButNotify)
		{
		aExternalLocate = CLbsAdmin::EExternalLocateOffButNotify;
		}
	else
		{
		aExternalLocate = CLbsAdmin::EExternalLocateUnknown;
		err = KErrNotFound;
		}
	
	return err;
	}


TLbsNetMtLrRequestMsg* CTe_LbsNrhSuiteStepBase::CreatePrivacyMsgLC(
        const TLbsNetSessionIdInt& aSessionId,
        TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt aAdvice,
        TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt aAction,
        TBool aEmergency)
    {
    TLbsNetPosRequestPrivacyInt reqPriv;
    
    reqPriv.SetRequestAdvice(aAdvice);
    reqPriv.SetRequestAction(aAction);
            
    _LIT(KRequesterId, "Brandt");
    _LIT(KClientName, "Heidegger");
    _LIT8(KClientExternalId, "0800 123456");
    TLbsExternalRequestInfo2 reqInfo;
    reqInfo.SetRequesterId(KRequesterId);
    reqInfo.SetClientName(KClientName);
    reqInfo.SetClientExternalId(KClientExternalId);
    reqInfo.SetNetworkType(TLbsExternalRequestInfo::ENetworkGSM);
    reqInfo.SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
    reqInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatMDN);
    reqInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatMDN);
    
    TLbsNetMtLrRequestMsg* msg = new (ELeave) TLbsNetMtLrRequestMsg(
                                    aSessionId,
                                    aEmergency,
                                    reqPriv,
                                    reqInfo);
    CleanupStack::PushL(msg);
    return msg;
    }

TLbsNetMtLrRequestMsg* CTe_LbsNrhSuiteStepBase::CreatePrivacyVerificationMsgLC(
		const TLbsNetSessionIdInt& aSessionId, TBool aEmergency)
	{
	TLbsNetPosRequestPrivacyInt reqPriv;
	reqPriv.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify);
	reqPriv.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionAllow);		
	
	_LIT(KRequesterId, "your momma");
	_LIT(KClientName, "Charlene");
	_LIT8(KClientExternalId, "0800 123456");
	TLbsExternalRequestInfo2 reqInfo;
	reqInfo.SetRequesterId(KRequesterId);
	reqInfo.SetClientName(KClientName);
	reqInfo.SetClientExternalId(KClientExternalId);
	reqInfo.SetNetworkType(TLbsExternalRequestInfo::ENetworkGSM);
	reqInfo.SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
	reqInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatMDN);
	reqInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatMDN);
	
	TLbsNetMtLrRequestMsg* msg = new (ELeave) TLbsNetMtLrRequestMsg(
									aSessionId,
									aEmergency,
									reqPriv,
									reqInfo);
	CleanupStack::PushL(msg);
	return msg;
	}

TLbsNetMtLrRequestMsg* CTe_LbsNrhSuiteStepBase::CreatePrivacyVerificationMsgAdviceNotifyLC(
		const TLbsNetSessionIdInt& aSessionId, TBool aEmergency)
	{
	TLbsNetPosRequestPrivacyInt reqPriv;
	reqPriv.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
	reqPriv.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionAllow);		
	
	_LIT(KRequesterId, "my momma");
	_LIT(KClientName, "Mary");
	_LIT8(KClientExternalId, "0800 123457");
	TLbsExternalRequestInfo2 reqInfo;
	reqInfo.SetRequesterId(KRequesterId);
	reqInfo.SetClientName(KClientName);
	reqInfo.SetClientExternalId(KClientExternalId);
	reqInfo.SetNetworkType(TLbsExternalRequestInfo::ENetworkGSM);
	reqInfo.SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
	reqInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatMDN);
	reqInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatMDN);
	
	TLbsNetMtLrRequestMsg* msg = new (ELeave) TLbsNetMtLrRequestMsg(
									aSessionId,
									aEmergency,
									reqPriv,
									reqInfo);
	CleanupStack::PushL(msg);
	return msg;
	}

TLbsNetMtLrRequestMsg* CTe_LbsNrhSuiteStepBase::CreatePrivacyNotificationMsgLC(
		const TLbsNetSessionIdInt& aSessionId)
	{
	TLbsNetPosRequestPrivacyInt reqPriv;
	reqPriv.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
	reqPriv.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionAllow);		
	
	_LIT(KRequesterId, "your momma");
	_LIT(KClientName, "Charlene");
	_LIT8(KClientExternalId, "0800 123456");
	TLbsExternalRequestInfo2 reqInfo;
	reqInfo.SetRequesterId(KRequesterId);
	reqInfo.SetClientName(KClientName);
	reqInfo.SetClientExternalId(KClientExternalId);
	reqInfo.SetNetworkType(TLbsExternalRequestInfo::ENetworkGSM);
	reqInfo.SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
	reqInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatMDN);
	reqInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatMDN);
	
	TLbsNetMtLrRequestMsg* msg = new (ELeave) TLbsNetMtLrRequestMsg(
									aSessionId,
									EFalse,
									reqPriv,
									reqInfo);
	CleanupStack::PushL(msg);
	return msg;
	}

TLbsNetLocationRequestMsg* CTe_LbsNrhSuiteStepBase::CreateLocationRequestMsgLC(
		const TLbsNetSessionIdInt& aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService)
	{
	TLbsNetPosMethodInt method;
	method.SetPosMethod(KLbsPositioningMeansGpsInt, 
						(TPositionModuleInfo::ETechnologyTerminal 
						 | TPositionModuleInfo::ETechnologyAssisted));
	TLbsNetPosRequestMethodInt methodArray;
	methodArray.SetPosMethods(&method, 1);

	return CreateLocationRequestMsgLC(aSessionId,
									  aService,
									  methodArray);
	}

TLbsNetLocationRequestMsg* CTe_LbsNrhSuiteStepBase::CreateEmergencyLocationRequestMsgLC(
		const TLbsNetSessionIdInt& aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService)
	{
	TLbsNetPosMethodInt method;
	method.SetPosMethod(KLbsPositioningMeansGps, 
						(TPositionModuleInfo::ETechnologyTerminal 
						 | TPositionModuleInfo::ETechnologyAssisted));
	TLbsNetPosRequestMethodInt methodArray;
	methodArray.SetPosMethods(&method, 1);

	return CreateEmergencyLocationRequestMsgLC(aSessionId,
									  aService,
									  methodArray);
	}

TLbsNetLocationRequestMsg* CTe_LbsNrhSuiteStepBase::CreateEmergencyLocationRequestMsgLC(
		const TLbsNetSessionIdInt& aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
		const TLbsNetPosRequestMethodInt& aPosMethodArray)
	{
	TLbsNetPosRequestQualityInt quality;
	TTimeIntervalMicroSeconds maxFixTime(5000000);
	quality.SetMaxFixTime(maxFixTime);
	quality.SetMinHorizontalAccuracy(TReal32(40.0));
	quality.SetMinVerticalAccuracy(TReal32(50.0));

	return CreateEmergencyLocationRequestMsgLC(aSessionId,
									  aService,
									  aPosMethodArray,
									  quality);
	}


TLbsNetLocationRequestMsg* CTe_LbsNrhSuiteStepBase::CreateLocationRequestMsgLC(
		const TLbsNetSessionIdInt& aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
		const TLbsNetPosRequestMethodInt& aPosMethodArray)
	{
	TLbsNetPosRequestQualityInt quality;
	TTimeIntervalMicroSeconds maxFixTime(5000000);
	quality.SetMaxFixTime(maxFixTime);
	quality.SetMinHorizontalAccuracy(TReal32(40.0));
	quality.SetMinVerticalAccuracy(TReal32(50.0));

	return CreateLocationRequestMsgLC(aSessionId,
									  aService,
									  aPosMethodArray,
									  quality);
	}

TLbsNetLocationRequestMsg* CTe_LbsNrhSuiteStepBase::CreateLocationRequestMsgLC(
		const TLbsNetSessionIdInt& aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
		const TLbsNetPosRequestQualityInt& aPosQuality)
	{
	TLbsNetPosMethodInt method;
	method.SetPosMethod(KLbsPositioningMeansGpsInt, 
						(TPositionModuleInfo::ETechnologyTerminal 
						 | TPositionModuleInfo::ETechnologyAssisted));
	TLbsNetPosRequestMethodInt methodArray;
	methodArray.SetPosMethods(&method, 1);
	
	return CreateLocationRequestMsgLC(aSessionId,
									  aService,
									  methodArray,
									  aPosQuality);
	}


TLbsNetLocationRequestMsg* CTe_LbsNrhSuiteStepBase::CreateLocationRequestMsgLC(
		const TLbsNetSessionIdInt& aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
		const TLbsNetPosRequestMethodInt& aPosMethodArray,
		const TLbsNetPosRequestQualityInt& aPosQuality)
	{	
	TLbsNetLocationRequestMsg* msg = new (ELeave) TLbsNetLocationRequestMsg(
									aSessionId,
									EFalse,
									aService,
									aPosQuality,
									aPosMethodArray);
	CleanupStack::PushL(msg);
	return msg;
	}

TLbsNetLocationRequestMsg* CTe_LbsNrhSuiteStepBase::CreateEmergencyLocationRequestMsgLC(
		const TLbsNetSessionIdInt& aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt  aService,
		const TLbsNetPosRequestMethodInt& aPosMethodArray,
		const TLbsNetPosRequestQualityInt& aPosQuality)
	{	
	TLbsNetLocationRequestMsg* msg = new (ELeave) TLbsNetLocationRequestMsg(
									aSessionId,
									ETrue,
									aService,
									aPosQuality,
									aPosMethodArray);
	CleanupStack::PushL(msg);
	return msg;
	}

// This creates a reference position. When the values are defened in the .ini file then 
// those values are used otherwise, default values are used.
TLbsNetLocationUpdateMsg* CTe_LbsNrhSuiteStepBase::CreateNetworkReferencePositionMsgLC(
		const TLbsNetSessionIdInt& aSessionId)
	{
	TTime now;
	now.UniversalTime();
	TPosition pos;
	

    // use values from .ini file
    pos.SetAccuracy(iRefToNrhHacc, TReal32(150.0));
    pos.SetCurrentTime();
	    pos.SetCoordinate(iRefToNrhLat, iRefToNrhLng, TReal32(50.0));

	
	TPositionInfo posInfo;
	posInfo.SetPosition(pos);
	posInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
	posInfo.SetUpdateType(EPositionUpdateGeneral);
	posInfo.SetPositionModeReason(EPositionModeReasonNone);
	
	TLbsNetLocationUpdateMsg* msg = new (ELeave) TLbsNetLocationUpdateMsg(
									aSessionId,
									KErrNone,
									posInfo,
									now);
	CleanupStack::PushL(msg);
	return msg;	
	}

TLbsNetLocationUpdateMsg* CTe_LbsNrhSuiteStepBase::CreateNetworkFinalPositionMsgLC(
		const TLbsNetSessionIdInt& aSessionId)
	{
	TTime now;
	now.UniversalTime();
	TPosition pos;

	if (KErrNotFound != iNetworkPosToNrhLat)
		{
		pos.SetAccuracy(iNetworkPosToNrhHacc, TReal32(50.0));
		pos.SetCoordinate(iNetworkPosToNrhLat, iNetworkPosToNrhLng, TReal32(50.0));
		}
	else
		{
		pos.SetAccuracy(TReal32(30.0), TReal32(50.0));
		pos.SetCoordinate(TReal64(52.2455), TReal64(0.1665), TReal32(50.0));
		}
	
	pos.SetCurrentTime();

	TInt error = KErrNone;
	 	if (KErrNotFound != iNetworkPosToNrhError)
		{
		error = iNetworkPosToNrhError;
		}

	TPositionInfo posInfo;
	posInfo.SetPosition(pos);
	posInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
	posInfo.SetUpdateType(EPositionUpdateGeneral);
	posInfo.SetPositionModeReason(EPositionModeReasonNone);
	
	TLbsNetLocationUpdateMsg* msg = new (ELeave) TLbsNetLocationUpdateMsg(
									aSessionId,
									error,
									posInfo,
									now);
	CleanupStack::PushL(msg);
	return msg;	
	}
			
TLbsNetSessionCompleteMsg* CTe_LbsNrhSuiteStepBase::CreateSessionCompeleteMsgLC(
		const TLbsNetSessionIdInt& aSessionId,
		TInt aReason)
	{
	TLbsNetSessionCompleteMsg* msg = new (ELeave) TLbsNetSessionCompleteMsg(
									aSessionId,
									aReason);
	CleanupStack::PushL(msg);	
	return msg;
	}
