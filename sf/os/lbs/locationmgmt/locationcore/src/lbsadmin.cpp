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

#include <e32base.h>

#include "lbsdevloggermacros.h"
#include <lbs/lbsadmin.h>
#include "lbsadminimpl.h"
#include "LbsInternalInterface.h"
#include "lbsqualityprofile.h"


//********************************************************************************************************
// Version of the admin API
const TInt8 KLbsAdminMajorVersion = 1;
const TInt8 KLbsAdminMinorVersion = 0;
const TInt16 KLbsAdminBuild       = 0;



//********************************************************************************************************
// Class CLbsAdmin - Public Methods
//********************************************************************************************************
/** Static constructor.
@return A pointer to the new CLbsAdmin object.
@capability WriteDeviceData if the configuration settings need to be created/initialised, none otherwise
@released*/
EXPORT_C CLbsAdmin* CLbsAdmin::NewL()
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::NewL() LbsAdmin\n");

	CLbsAdmin* self = new(ELeave)CLbsAdmin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Static constructor.
@param   aObserver       A object that will be receiving the call-backs
@param   aSettingsMask   A bit mask containing all the Settings a user should be notified about.
						 Default value is KLbsSettingNone.

It is possible to change the bitmask using the RequestSettingUpdateEventL() method.

@return A pointer to the new CLbsAdmin object.
@capability WriteDeviceData if the configuration settings need to be created/initialised, none otherwise
@released*/
EXPORT_C CLbsAdmin* CLbsAdmin::NewL(MLbsAdminObserver& aObserver, const TLbsAdminSettingGroup& aSettingsMask)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::NewL(2) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > MLbsAdminObserver aObserver  = 0x%X", &aObserver);
	LBSLOG2(ELogP9, "  > TLbsAdminSettingGroup aSettingsMask  = 0x%016X\n", aSettingsMask);

	CLbsAdmin* self = new(ELeave)CLbsAdmin();
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aSettingsMask);
	CleanupStack::Pop(self);
	return self;
	}

	
/** Second phase constructor 
@capability WriteDeviceData if the configuration settings need to be created/initialised, none otherwise
@released*/	
void CLbsAdmin::ConstructL()
	{
	LBSLOG(ELogP1, "CLbsAdmin::ConstructL()");
	iImpl = CLbsAdminImpl::NewL();
	}		


/** Second phase constructor 
@param   aObserver       A object that will be receiving the call-backs
@param   aSettingsMask   A bit mask containing all the Settings a user should be nitified about.

It is possible to change the bitmask using the RequestSettingUpdateEventL() method. 
@capability WriteDeviceData if the configuration settings need to be created/initialised, none otherwise
@released*/	
void CLbsAdmin::ConstructL(MLbsAdminObserver& aObserver, const TLbsAdminSettingGroup& aSettingsMask)
	{
	LBSLOG(ELogP1, "CLbsAdmin::ConstructL(aObserver, aSettingsMask)");
	iImpl = CLbsAdminImpl::NewL(aObserver, aSettingsMask);
	}		


/** Default constructor 
@released*/
CLbsAdmin::CLbsAdmin():
iImpl(NULL)
	{
	LBSLOG(ELogP1, "CLbsAdmin::CLbsAdmin()");
	}


/** Default destructor */
CLbsAdmin::~CLbsAdmin()
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::~CLbsAdmin() LbsAdmin\n");
	delete iImpl;
	}

	
/** Requests a notification (callback) if the value of one or more of the given settings changes.
It replaces a previous request, either passed through NewL(...) or RequestSettingUpdateEventL(...).

Events will continue to be delivered to the supplied observer until the CLbsAdmin object exists
or CancelSettingEvent() is called.

@param   aSettingsMask   A bit mask containing all the Settings a user should be nitified about.
@leave   KErrorArgument, KErrNoMemory or one of the other system wide error codes

@panic EAdminObserverNotDefined If an observer has not been specified in the static constructor - NewL(...)
@see TLbsAdminSetting
@see MLbsAdminObserver
@released*/
EXPORT_C void CLbsAdmin::SetNotificationMaskL(const TLbsAdminSettingGroup& aSettingsMask)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::SetNotificationMaskL() LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSettingGroup aSettingsMask  = 0x%016X\n", aSettingsMask);

	iImpl->SetNotificationMaskL(aSettingsMask);
	}

	
/** Cancels an outstanding setting event request
@see TLbsAdminSetting
@released*/
EXPORT_C void CLbsAdmin::ClearNotificationMask()
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::ClearNotificationMask() LbsAdmin\n");

	iImpl->ClearNotificationMask();
	}


/** Resets all the settings to their default states.
A licensee may define own default values.
@return KErrNone if successful, one of the system wide error codes otherwise.
@released*/
EXPORT_C TInt CLbsAdmin::ResetToDefault()
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::ResetToDefault() LbsAdmin\n");
	
	TInt err = iImpl->ResetToDefault();
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


//********************************************************************************************************
// Class CLbsAdmin - Private Methods
//********************************************************************************************************
/** Sets the KLbsSettingHomeSelfLocate or KLbsSettingRoamingSelfLocate settings.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful, KErrArgument if the value is out of range, KErrNotSupported if the value is not supported and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TSelfLocateService& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TSelfLocateService) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TSelfLocateService aValue  = 0x%02X\n", aValue);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeSelfLocate) || (aSetting == KLbsSettingRoamingSelfLocate))
		{
		err = iImpl->SetSettingValidated(aSetting, aValue);
		}
	
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


/** Sets the KLbsSettingHomeExternalLocate, KLbsSettingRoamingExternalLocate,
    KLbsSettingHomeEmergencyLocate KLbsSettingRoamingEmergencyLocate settings.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful,KErrArgument if the value is out of range and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TExternalLocateService& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TExternalLocateService) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TExternalLocateService aValue  = 0x%02X\n", aValue);

	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeExternalLocate) || (aSetting == KLbsSettingRoamingExternalLocate) ||
	    (aSetting == KLbsSettingHomeEmergencyLocate) || (aSetting == KLbsSettingRoamingEmergencyLocate) ||
		(aSetting == KLbsSettingHomeNetworkInducedLocate) || (aSetting == KLbsSettingRoamingNetworkInducedLocate))
		{
		err = iImpl->SetSettingValidated(aSetting, aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


/** Sets the KLbsSettingHomeTransmitLocate or KLbsSettingRoamingTransmitLocate setting.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful,KErrArgument if the value is out of range and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TTransmitLocateService& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TransmitLocateService) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TTransmitLocateService aValue  = 0x%02X\n", aValue);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeTransmitLocate) || (aSetting == KLbsSettingRoamingTransmitLocate))
		{
		err = iImpl->SetSettingValidated(aSetting, aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}			


/** Sets the KLbsSettingHomeGpsMode or KLbsSettingRoamingGpsMode settings.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful. KErrNotSupported if aValue set to EGpsModeUnknown, KLbsAlwaysTerminalAssisted or EGpsAlwaysTerminalAssisted. 
KErrArgument if the value is out of range and One of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/

EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TGpsMode& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TGpsMode) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TGpsMode aValue  = 0x%02X\n", aValue);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeGpsMode) || (aSetting == KLbsSettingRoamingGpsMode))
		{
		if (aValue == EGpsModeUnknown  || aValue == EGpsAlwaysTerminalBased)
			{
			err = KErrNotSupported;	
			}
		else
			{
			err = iImpl->SetSettingValidated(aSetting, aValue);
			}	
		}
	
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	


/** Sets the KLbsSettingQualityProfileSelfLocate or KLbsSettingQualityProfileExternalLocate 
    or KLbsSettingQualityProfileTransmitLocate settings.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TLbsQualityProfileId& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TLbsQualityProfileId) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TLbsQualityProfileId aValue  = %u\n", aValue);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingQualityProfileSelfLocate) || (aSetting == KLbsSettingQualityProfileExternalLocate) ||
		(aSetting == KLbsSettingQualityProfileTransmitLocate))
		{
		//to validate quality profile Id
		TQualityProfile temp;
		err = LbsQualityProfile::GetQualityProfileById(aValue, temp);
		if(KErrNone == err)
			{
			err = iImpl->SetSettingValidated(aSetting, aValue);
			}
		else
			{
			err = KErrArgument;
			}
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	
	
/** Sets the KLbsSettingHomeProtocolModule and KLbsSettingRoamingProtocolModule 
and KLbsSettingSetClockModule settings.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TUid& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TUid) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TUid aValue  = 0x%08X\n", aValue.iUid);

	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeProtocolModule) || (aSetting == KLbsSettingRoamingProtocolModule) ||
		(aSetting == KLbsSettingSetClockModule))
		{
		err = iImpl->SetSettingValidated(aSetting, aValue.iUid);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	
	
/** Sets the KLbsSettingLbsSystemStartMode setting.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful,KErrArgument if the value is out of range and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TLbsSystemStartMode& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TLbsSystemStartMode) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TLbsSystemStartMode aValue  = 0x%02X\n", aValue);
	
	TInt err = KErrArgument;

	if (aSetting == KLbsSettingLbsSystemStartMode)
		{
		err = iImpl->SetSettingValidated(aSetting, aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	
/** Sets the KSettingLbsBehaviourMode setting.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful,KErrArgument if the value is out of range and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TLbsBehaviourMode& aValue)
	{
	LBSLOG(ELogP1, "CLbsAdmin::DoSet(TLbsAdminSetting, TLbsBehaviourMode)");
	if (aSetting == KLbsSettingBehaviourMode)
		{
		return iImpl->SetSettingValidated(aSetting, aValue);
		}
	else
		{
		return KErrArgument;
		}
	}
	
	
/** Sets the KLbsSettingPrivacyTimeoutAction setting.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful, KErrArgument if the value is out of range and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TPrivacyTimeoutAction& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TPrivacyTimeoutAction) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TPrivacyTimeoutAction aValue  = 0x%02X\n", aValue);
	
	TInt err = KErrArgument;

	if (aSetting == KLbsSettingPrivacyTimeoutAction)
		{
		err = iImpl->SetSettingValidated(aSetting, aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	
	
/** Sets the KLbsSettingPrivacyHandler setting.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful,KErrArgument if the value is out of range and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TPrivacyHandler& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TPrivacyHandler) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TPrivacyHandler aValue  = 0x%02X\n", aValue);
	
	TInt err = KErrArgument;

	if (aSetting == KLbsSettingPrivacyHandler)
		{
		err = iImpl->SetSettingValidated(aSetting, aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


/** Sets the KLbsSettingLogger setting.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful,KErrArgument if the value is out of range and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TLogger& aValue)
	{
	LBSLOG(ELogP1, "CLbsAdmin::DoSet(TLbsAdminSetting, TLogger)");
	if (aSetting == KLbsSettingLogger)
		{
		return iImpl->SetSettingValidated(aSetting, aValue);
		}
	else
		{
		return KErrArgument;
		}
	}
	
	
/** Sets the KLbsSettingClockAdjust or KLbsSettingAllowManualClockAdjust setting.
@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if successful,KErrArgument if the value is out of range and one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TClockAdjust& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TClockAdjust) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TClockAdjust aValue  = 0x%02X\n", aValue);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingClockAdjust) || (aSetting == KLbsSettingAllowManualClockAdjust))
		{
		err = iImpl->SetSettingValidated(aSetting, aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}	
	
	
/** Sets the settings:
    KLbsSettingClockAdjustThreshold
    KLbsSettingClockAdjustInterval
    KLbsSettingMaximumExternalLocateRequests
    KLbsSettingPrivacyShutdownDelay
    KLbsSettingPrivacyMaxNetworkSessions
    KLbsSettingPrivacyConnectionTimeout
    KLbsSettingPrivacyAppTimeout
@param aSetting The setting to be changed.
@param aValue   The new value of the setting. The value should be smaller than KMaxTInt.
@return KErrNone if successful, one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting */
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TUint& aValue)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TUint) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TUint aValue  = %u\n", aValue);
	
	TInt err = KErrArgument;

	if (((aSetting == KLbsSettingClockAdjustThreshold) || 
		(aSetting == KLbsSettingClockAdjustInterval) ||
		((aSetting == KLbsSettingMaximumExternalLocateRequests) && (aValue>0))|| 
		(aSetting == KLbsSettingPrivacyShutdownDelay) ||
		(aSetting == KLbsSettingPrivacyMaxNetworkSessions) ||
		(aSetting == KLbsSettingPrivacyConnectionTimeout) ||
		(aSetting == KLbsSettingPrivacyAppTimeout)) 
		&& aValue <= KMaxTInt)
		{
		err = iImpl->SetSettingValidated(aSetting, aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;

	}	

/** Sets the extended KSettingHomeExternalLocate, KSettingHomeExternalLocate, KSettingHomeProtocolModule,
KSettingRoamingProtocolModule and KLbsProtocolModuleLoading settings.
The extended settings derive from the TLbsAdminInfoBase class.

@param aSetting [In] The setting to be changed.
@param aValue   [In] The new value of the setting.
@return KErrNone if successful, KErrNotSupported if a setting value is not supported,
        KErrArgument if the setting values are incorrect or the type of the TLbsAdminInfoBase derived
        class does not match the setting. One of the system wide error codes otherwise.

@capability WriteDeviceData
@see TLbsAdminSetting
@see TLbsAdminInfoBase
@released*/
EXPORT_C TInt CLbsAdmin::DoSet(const TLbsAdminSetting& aSetting, const TLbsAdminInfoBase& aInfo)
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TLbsAdminInfoBase) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting   = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TLbsAdminInfoBase.ClassType = 0x%02X\n", aInfo.ClassType());
	
	TInt err = KErrArgument;

	if (aSetting == KLbsProtocolModuleLoading)
		{
#ifdef ENABLE_LBS_DEV_LOGGER		
		const TLbsAdminProtocolModuleLoadingInfo& info = static_cast<const TLbsAdminProtocolModuleLoadingInfo&>(aInfo);
		LBSLOG2(ELogP9, "  > TProtocolModuleLoading info.iLoadingStrategy  = %u\n", info.LoadingStrategy());
		(void) info; // avoid a compiler warning
#endif		
		err = iImpl->SetInfoBaseValidated(aSetting, aInfo);
		}
	else if((aSetting == KLbsSettingHomeProtocolModule) || (aSetting == KLbsSettingRoamingProtocolModule))
		{
		err = iImpl->SetInfoBaseValidated(aSetting, aInfo);
		}
	
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}	

//--------------------------------------------------------------------------------------------------------
/** Gets the KLbsSettingHomeSelfLocate or KLbsSettingRoamingSelfLocate settings.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TSelfLocateService& aValue) const
	{ 
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TSelfLocateService) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeSelfLocate) || (aSetting == KLbsSettingRoamingSelfLocate))
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TSelfLocateService aValue  = 0x%02X\n", aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


/** Gets the KLbsSettingHomeExternalLocate, KLbsSettingRoamingExternalLocate,
    KLbsSettingHomeEmergencyLocate or KLbsSettingRoamingEmergencyLocate settings.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TExternalLocateService& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TExternalLocateService) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeExternalLocate) || (aSetting == KLbsSettingRoamingExternalLocate) ||
	    (aSetting == KLbsSettingHomeEmergencyLocate) || (aSetting == KLbsSettingRoamingEmergencyLocate) ||
		(aSetting == KLbsSettingHomeNetworkInducedLocate) || (aSetting == KLbsSettingRoamingNetworkInducedLocate))
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TExternalLocateService aValue  = 0x%02X\n", aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


/** Gets the KLbsSettingHomeTransmitLocate or KLbsSettingRoamingTransmitLocate settings.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TTransmitLocateService& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TTransmitLocateService) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeTransmitLocate) || (aSetting == KLbsSettingRoamingTransmitLocate))
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TTransmitLocateService aValue  = 0x%02X\n", aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}		


/** Gets the KLbsSettingHomeGpsMode or KLbsSettingRoamingGpsMode settings.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TGpsMode& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TGpsMode) LbsAdmin");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingHomeGpsMode) || (aSetting == KLbsSettingRoamingGpsMode))
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TGpsMode aValue  = 0x%02X\n", aValue);
		}
	
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


/** Gets the KLbsSettingHomeProtocolModule or KLbsSettingRoamingProtocolModule settings
or KLbsSettingSetClockModule settings.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TUid& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TUid) LbsAdmin");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;
	
	if ((aSetting == KLbsSettingHomeProtocolModule) || (aSetting == KLbsSettingRoamingProtocolModule) ||
		(aSetting == KLbsSettingSetClockModule))
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TUid aValue  = 0x%08X\n", aValue.iUid);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	
	
/** Gets the KLbsSettingQualityProfileSelfLocate or KLbsSettingQualityProfileExternalLocate 
    or KLbsSettingQualityProfileTransmitLocate settings.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TLbsQualityProfileId& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TLbsQualityProfileId) LbsAdmin");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;
	
	if ((aSetting == KLbsSettingQualityProfileSelfLocate) || (aSetting == KLbsSettingQualityProfileExternalLocate) ||
		(aSetting == KLbsSettingQualityProfileTransmitLocate))
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TLbsQualityProfileId aValue  = %u\n", aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	
	
/** Gets the KLbsSettingLbsSystemStartMode setting.
@param aSetting The setting to be retrieved. 
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TLbsSystemStartMode& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Set(TLbsSystemStartMode) LbsAdmin");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;

	if (aSetting == KLbsSettingLbsSystemStartMode)
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TLbsSystemStartMode aValue  = 0x%02X\n", aValue);
		}
	
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	
   	
  /** Gets the KLbsBehaviourMode setting.
  @param aSetting The setting to be retrieved. 
  @param aValue   The value of the setting.
  @return KErrNone if successful, one of the system wide error codes otherwise.
  @see TLbsAdminSetting
  @released*/
  EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TLbsBehaviourMode& aValue) const
  	{
  	LBSLOG(ELogP1, "CLbsAdmin::DoGet(TLbsAdminSetting, TLbsBehaviourMode)");
  	if (aSetting == KLbsSettingBehaviourMode)
  		{
  		return iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
  		}
  	else
  		{
  		return KErrArgument;
  		}
  	}
   	

	
/** Gets the KLbsSettingPrivacyTimeoutAction setting.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TPrivacyTimeoutAction& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TPrivacyTimeoutAction) LbsAdmin");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;

	if (aSetting == KLbsSettingPrivacyTimeoutAction)
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TPrivacyTimeoutAction aValue  = 0x%02X\n", aValue);
		}
	
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}		


/** Gets the KLbsSettingPrivacyHandler setting.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TPrivacyHandler& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TPrivacyHandler) LbsAdmin");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;

	if (aSetting == KLbsSettingPrivacyHandler)
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TPrivacyHandler aValue  = 0x%02X\n", aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}
	
/** Gets the KLbsSettingLogger setting.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TLogger& aValue) const
	{
	LBSLOG(ELogP1, "CLbsAdmin::DoGet(TLbsAdminSetting, TLogger)");
	if (aSetting == KLbsSettingLogger)
		{
		return iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		}
	else
		{
		return KErrArgument;
		}
	}


/** Gets the settings: 
 	KLbsSpecialFeatureAllowTBFinalNetPos
 	KLbsSpecialFeatureIgnoreAccuracy
  	KLbsSpecialFeatureWithholdDeliveryOfReferencePositions 
  	KLbsSpecialFeatureIntermediateFutileUpdate
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TSpecialFeature& aValue) const
	{
	LBSLOG(ELogP1, "CLbsAdmin::DoGet(TLbsAdminSetting, TSpecialFeature)");
	if ((aSetting == KLbsSpecialFeatureAllowTBFinalNetPos) 
		|| (aSetting == KLbsSpecialFeatureIgnoreAccuracy) 
		|| (aSetting == KLbsSpecialFeatureWithholdDeliveryOfReferencePositions)
		|| (aSetting == KLbsSpecialFeatureIntermediateFutileUpdate))
		{
		return iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		}
	else
		{
		return KErrArgument;
		}
	}
	
	


	
/** Gets the KLbsSettingClockAdjust or KLbsSettingAllowManualClockAdjust setting.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released*/
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TClockAdjust& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TClockAdjust) LbsAdmin");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;
	
	if ((aSetting == KLbsSettingClockAdjust) || (aSetting == KLbsSettingAllowManualClockAdjust))
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TClockAdjust aValue  = 0x%02X\n", aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}			

/** Gets the settings:
    KLbsSettingClockAdjustThreshold
    KLbsSettingClockAdjustInterval
    KLbsSpecialFeatureMaximumRequestDelay
    KLbsSettingMaximumExternalLocateRequests
    KLbsSettingPrivacyShutdownDelay
    KLbsSettingPrivacyMaxNetworkSessions
    KLbsSettingPrivacyConnectionTimeout
    KLbsSettingPrivacyAppTimeout
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting */
EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TUint& aValue) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TUint) LbsAdmin");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting  = 0x%016X\n", aSetting);
	
	TInt err = KErrArgument;

	if ((aSetting == KLbsSettingClockAdjustThreshold) || 
		(aSetting == KLbsSettingClockAdjustInterval) ||
		(aSetting == KLbsSpecialFeatureMaximumRequestDelay) ||
		(aSetting == KLbsSettingMaximumExternalLocateRequests) ||
		(aSetting == KLbsSettingPrivacyShutdownDelay) ||
		(aSetting == KLbsSettingPrivacyMaxNetworkSessions) ||
		(aSetting == KLbsSettingPrivacyConnectionTimeout) ||
		(aSetting == KLbsSettingPrivacyAppTimeout))
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aValue));
		LBSLOG2(ELogP9, "  < TUint aValue  = %u\n", aValue);
		}

	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}	

/** Gets the extended KSettingHomeExternalLocate, KSettingRoamingExternalLocate, KSettingHomeProtocolModule,
KSettingRoamingProtocolModule and KLbsProtocolModuleLoading settings.
The extended settings derive from the TLbsAdminInfoBase class.

@param aSetting [In]  The setting to be retrieved.
@param aValue   [Out] The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.

@see TLbsAdminSetting
@see TLbsAdminInfoBase
@released*/


EXPORT_C TInt CLbsAdmin::DoGet(const TLbsAdminSetting& aSetting, TLbsAdminInfoBase& aInfo) const
	{
	LBSLOG(ELogP9, "->S CLbsAdmin::Get(TLbsAdminInfoBase) LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TLbsAdminSetting aSetting   = 0x%016X\n", aSetting);
	LBSLOG2(ELogP9, "  > TLbsAdminInfoBase.ClassType = 0x%02X\n", aInfo.ClassType());
	
	TInt err = KErrArgument;

	if (aSetting == KLbsProtocolModuleLoading)
		{
		err = iImpl->GetSettingValidated(aSetting, reinterpret_cast<TInt&>(aInfo));
#ifdef ENABLE_LBS_DEV_LOGGER		
		TLbsAdminProtocolModuleLoadingInfo& info = static_cast<TLbsAdminProtocolModuleLoadingInfo&>(aInfo);
		LBSLOG2(ELogP9, "  < TProtocolModuleLoading info.iLoadingStrategy  = %u\n", info.LoadingStrategy());
		(void) info; // avoid a compiler warning
#endif		
		}
	else if (aSetting == KLbsSettingHomeProtocolModule)
		{
		err = iImpl->GetSettingValidated(KLbsProtocolModulesHome, reinterpret_cast<TInt&>(aInfo));
		}
	else if (aSetting == KLbsSettingRoamingProtocolModule)
		{
		err = iImpl->GetSettingValidated(KLbsProtocolModulesRoaming, reinterpret_cast<TInt&>(aInfo));
		}
		
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}	

	
//********************************************************************************************************
// Class MLbsAdminObserver - Public Methods
//********************************************************************************************************
/** Returns Observer version number
@return Version number.
@released*/
/*virtual*/ EXPORT_C TVersion MLbsAdminObserver::Version() 
	{
	LBSLOG(ELogP9, "<-S MLbsAdminObserver::Version() LbsAdmin\n");
	LBSLOG4(ELogP9, "  Return TVersion  = %d, %d, %d\n", KLbsAdminMajorVersion, KLbsAdminMinorVersion, KLbsAdminBuild);
	
	return TVersion(KLbsAdminMajorVersion, KLbsAdminMinorVersion, KLbsAdminBuild);
	}
