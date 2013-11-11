
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
//

#include <e32base.h>
#include <e32property.h>
#include <ecom/ecom.h>

#include <lbs/lbslocerrors.h>
#include "lbsdevloggermacros.h"
#include <centralrepository.h>

#include <lbs/lbsadmin.h>
#include <lbs/gpssetclockbase.h>
#include "lbsadminimpl.h"

#if defined(LBSADMIN_UNIT_TEST)
#include "../test/te_lbsadmin/inc/te_lbsadminutils.h"
#endif

//
// NOTE: Please do not use switch statements when using the 'TLbsAdminSetting' type.
// 		 This is known to cause problems with how arm compliers handle 64 bit values.
//

//********************************************************************************************************
// Config Bus and its Policy
const TUid KConfigBusCategory = {0x1020E192}; //As the UID of this dll 
const TUid KLBSAdminRepositoryUid ={0x1028224B};

/** A bitmask containing all the settings */
const TLbsAdminSetting KAllSettings = 	
	KLbsSettingHomeSelfLocate |
	KLbsSettingRoamingSelfLocate |
	KLbsSettingQualityProfileSelfLocate |
	KLbsSettingHomeExternalLocate |
	KLbsSettingRoamingExternalLocate |
	KLbsSettingQualityProfileExternalLocate |
	KLbsSettingHomeEmergencyLocate |
	KLbsSettingRoamingEmergencyLocate |
	KLbsSettingHomeNetworkInducedLocate |
	KLbsSettingRoamingNetworkInducedLocate |
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
	KLbsProtocolModuleLoading |
	KLbsSettingMaximumExternalLocateRequests |
	KLbsSettingPrivacyShutdownDelay |
	KLbsSettingPrivacyMaxNetworkSessions |
	KLbsSettingPrivacyConnectionTimeout |
	KLbsSettingClockAdjust |
	KLbsSettingAllowManualClockAdjust |
	KLbsSettingClockAdjustThreshold |
	KLbsSettingClockAdjustInterval |
	KLbsSettingSetClockModule;


// Used to initially create all the properties
const TLbsAdminSetting KKeysArray[]=
	{
	KLbsSettingHomeSelfLocate,
	KLbsSettingRoamingSelfLocate,
	KLbsSettingQualityProfileSelfLocate,
	KLbsSettingHomeExternalLocate,
	KLbsSettingRoamingExternalLocate,
	KLbsSettingQualityProfileExternalLocate,
	KLbsSettingHomeEmergencyLocate,
	KLbsSettingRoamingEmergencyLocate,
	KLbsSettingHomeTransmitLocate,
	KLbsSettingRoamingTransmitLocate,
	KLbsSettingQualityProfileTransmitLocate,
	KLbsSettingHomeGpsMode,
	KLbsSettingRoamingGpsMode,
	KLbsSettingHomeProtocolModule,
	KLbsSettingRoamingProtocolModule,
	KLbsSettingLbsSystemStartMode,
	KLbsSettingPrivacyTimeoutAction,
	KLbsSettingPrivacyHandler,		
	KLbsSettingBehaviourMode,
	KLbsSettingLogger,		
	KLbsSpecialFeatureMaximumRequestDelay,
	KLbsSpecialFeatureAllowTBFinalNetPos,
	KLbsSpecialFeatureWithholdDeliveryOfReferencePositions,
	KLbsProtocolModuleLoading,	
	KLbsSettingMaximumExternalLocateRequests,
	KLbsSettingPrivacyShutdownDelay,
	KLbsSettingPrivacyMaxNetworkSessions,	
	KLbsSettingPrivacyConnectionTimeout,				
	KLbsSpecialFeatureIgnoreAccuracy,
	KLbsSettingPrivacyAppTimeout,
	KLbsSpecialFeatureIntermediateFutileUpdate,
	KLbsSettingHomeNetworkInducedLocate,
	KLbsSettingRoamingNetworkInducedLocate,
	KLbsSettingSetClockModule,	
	KLbsSettingClockAdjustInterval,
	KLbsSettingClockAdjustThreshold,
	KLbsSettingClockAdjust,
	KLbsSettingAllowManualClockAdjust,
	};
const TInt KKeysArrayCount = sizeof(KKeysArray)/sizeof(TLbsAdminSetting);

// Uid of the Network Protocol ECom Plugin Interface
const TUid KNetworkProtocolInterfaceUid = {0x10281D4A};

//********************************************************************************************************
// Cleanup for a RPointerArray of subscribers
//********************************************************************************************************
void CleanupSubscribersArray(TAny* aSubscribersArrayPtr)
	{
	LBSLOG(ELogP1, "CleanupSubscribersArray()");
	RPointerArray<CAdminSettingSubscriber>* array = reinterpret_cast<RPointerArray<CAdminSettingSubscriber>*>(aSubscribersArrayPtr);
	TInt count = array->Count();
	for (TInt i = 0; i < count; i++)
		{
		delete (*array)[i];
		}
	
	array->Reset();
	}


//********************************************************************************************************
// CAdminSettingSubscriber
//********************************************************************************************************
/** Static constructor.
@released */
CAdminSettingSubscriber* CAdminSettingSubscriber::NewL(TUid aCategory, TLbsAdminSetting aSetting, MLbsAdminObserver& aObserver)
	{
	LBSLOG(ELogP1, "CAdminSettingSubscriber::NewL()");
	CAdminSettingSubscriber* self = CAdminSettingSubscriber::NewLC(aCategory, aSetting, aObserver);
	CleanupStack::Pop(self);
	return self;
	}


/** Static constructor.
@released */
CAdminSettingSubscriber* CAdminSettingSubscriber::NewLC(TUid aCategory, TLbsAdminSetting aSetting, MLbsAdminObserver& aObserver)
	{
	LBSLOG(ELogP1, "CAdminSettingSubscriber::NewLC()");
	CAdminSettingSubscriber* self = new(ELeave)CAdminSettingSubscriber(aCategory, aSetting, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


/** Second phase constructor 
@released */	
void CAdminSettingSubscriber::ConstructL()
	{
	LBSLOG(ELogP1, "CAdminSettingSubscriber::ConstructL()");
	TInt err = iProperty.Attach(iCategory, LbsAdminUtils::BitPosToNumber(iSetting));
	User::LeaveIfError(err);
	iProperty.Subscribe(iStatus);
	SetActive();
	}	


/** Default constructor
@released */
CAdminSettingSubscriber::CAdminSettingSubscriber(TUid aCategory, TLbsAdminSetting aSetting, MLbsAdminObserver& aObserver)
: CActive(CActive::EPriorityUserInput),
iCategory(aCategory),
iSetting(aSetting),
iObserver(aObserver)
	{
	LBSLOG(ELogP1, "CAdminSettingSubscriber::CAdminSettingSubscriber()");
	CActiveScheduler::Add(this);
	}


/** Default destructor
@released */
CAdminSettingSubscriber::~CAdminSettingSubscriber()
	{
	LBSLOG(ELogP1, "CAdminSettingSubscriber::~CAdminSettingSubscriber()");
	Cancel();
	iProperty.Close();
	}


/*virtual*/ void CAdminSettingSubscriber::RunL()
	{
	LBSLOG(ELogP1, "CAdminSettingSubscriber::RunL()");
	TInt err = iStatus.Int();
	iProperty.Subscribe(iStatus);
	LBSLOG(ELogP9, "<-A MLbsAdminObserver::OnSettingUpdateEvent() LbsAdmin\n");
	LBSLOG2(ELogP9, "  > TInt err  = %d\n", err);
	LBSLOG2(ELogP9, "  > TLbsAdminSetting iSetting  = 0x%016X\n", iSetting);
	iObserver.OnSettingUpdateEvent(err, iSetting);
	SetActive();	
	}

	
/*virtual*/ void CAdminSettingSubscriber::DoCancel()
	{
	LBSLOG(ELogP1, "CAdminSettingSubscriber::DoCancel()");
	iProperty.Cancel();
	}


/*virtual*/ TInt CAdminSettingSubscriber::RunError(TInt aError)
	{
	LBSLOG(ELogP1, "CAdminSettingSubscriber::RunError()");
	return aError;
	}


//********************************************************************************************************
// Class CLbsAdminImplImpl - Public Methods
//********************************************************************************************************
/** Static constructor.
@return A pointer to the new CLbsAdminImpl object.
@capability WriteDeviceData if the configuration settings need to be created/initialised, none otherwise
@released */
CLbsAdminImpl* CLbsAdminImpl::NewL()
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::NewL()");
	CLbsAdminImpl* self = new(ELeave)CLbsAdminImpl(NULL);
	CleanupStack::PushL(self);
	self->ConstructL(KLbsSettingNone);
	CleanupStack::Pop(self);
	return self;
	}

/** Static constructor.
@return A pointer to the new CLbsAdminImpl object.
@capability WriteDeviceData if the configuration settings need to be created/initialised, none otherwise
@released */
CLbsAdminImpl* CLbsAdminImpl::NewL(MLbsAdminObserver& aObserver, const TLbsAdminSettingGroup& aSettingsMask)
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::NewL(MLbsAdminObserver, TLbsAdminSettingGroup)");
	CLbsAdminImpl* self = new(ELeave)CLbsAdminImpl(&aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSettingsMask);
	CleanupStack::Pop(self);
	return self;
	}

	
/** Second phase constructor 
@capability WriteDeviceData if the configuration settings need to be created/initialised, none otherwise
@released */	
void CLbsAdminImpl::ConstructL(const TLbsAdminSettingGroup& aSettingsMask)
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::ConstructL()");
	
	iCenRep = CRepository::NewL(KLBSAdminRepositoryUid);
	// Create the Properties with all the settings
	DefinePropertiesL();
	
	if (aSettingsMask != KLbsSettingNone)
		{
		SetNotificationMaskL(aSettingsMask);
		}
	}		


/** Default constructor 
@released */
CLbsAdminImpl::CLbsAdminImpl(MLbsAdminObserver* aObserver):
iObserver(aObserver),
iCenRep(NULL)
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::CLbsAdminImpl()");
	}


/** Default destructor */
CLbsAdminImpl::~CLbsAdminImpl()
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::~CLbsAdminImpl()");
	DeleteSubscribers();
	iSubscribers.Close();
	delete iCenRep;
	REComSession::FinalClose();
	}

	
// 
void CLbsAdminImpl::SetNotificationMaskL(const TLbsAdminSettingGroup& aSettingsMask)
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::SetNotificationMaskL()");
	if (aSettingsMask & ~KAllSettings)
		{
		User::Leave(KErrArgument);
		}
	
	__ASSERT_DEBUG(CActiveScheduler::Current(), User::Panic(KLbsAdminFault, EAdminActiveSchedulerNotInstalled));

	// Subscribers may exist already. Delete those not in aSettingMask and
	// prepare a new mask with only those settings in aSettingMask for which
	// a subscriber doesn't exist yet.
	TLbsAdminSettingGroup susbscriberCreationMask = aSettingsMask;
	DeleteSubscribersNotInMask(susbscriberCreationMask);	
	
	// Create the settings subscribers
	for (TUint i = 0; i < KKeysArrayCount; i++)
		{
		if ((KKeysArray[i]	& susbscriberCreationMask) != 0)
			{
			CAdminSettingSubscriber* newSubscriber = CAdminSettingSubscriber::NewLC(KConfigBusCategory, KKeysArray[i], *iObserver);
			iSubscribers.AppendL(newSubscriber);
			CleanupStack::Pop(newSubscriber);
			}
		}
	}

/** Cancels an outstanding setting event request
@see TLbsAdminSetting
@released */
void CLbsAdminImpl::ClearNotificationMask()
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::ClearNotificationMask()");
	DeleteSubscribers();
	}

TInt CLbsAdminImpl::ResetToDefault()
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::ResetToDefault()");
	TInt ret = KErrNone;
	ret = iCenRep->Reset();
	ASSERT(ret == KErrNone);
	if(ret==KErrNone)
		{
		// Notify the subscribers changing the value of all the properties.
		for (TInt i = 0; i < KKeysArrayCount; i++)
			{
			// Get the default value from CenRep and set the property with this value
			TInt value;
			ret = iCenRep->Get(LbsAdminUtils::BitPosToNumber(KKeysArray[i]),value);
			// If not found then is either an oldcenrep that hasn't got the new value in it or its corrupt!
			if (ret != KErrNone) 
				{
				// Look for new additions.
				switch (KKeysArray[i])
					{
					// Default logging is on.
					case KLbsSettingLogger:
						{
						value = CLbsAdmin::ELoggerOn;
						break;	
						}

					// Default is to load protocol modules at LBS start-up by roaming status.
					case KLbsProtocolModuleLoading:
						{
						value = TLbsAdminProtocolModuleLoadingInfo::EProtocolModuleLoadingByStatus;
						break;
						}
						
					// Default is to allow a single external locate request at a time
					case KLbsSettingMaximumExternalLocateRequests:
						{
						value = KLbsDefaultMaximumExternalLocateRequests;
						break;
						}

					// Default is a delay of 2 seconds
					case KLbsSettingPrivacyShutdownDelay:
						{
						value = KLbsDefaultPrivacyShutdownDelay;
						break;
						}

					// Default is to allow up to 4 concurrent network privacy sessions
					case KLbsSettingPrivacyMaxNetworkSessions:
						{
						value = KLbsDefaultPrivacyMaxNetworkSessions;
						break;
						}

					// Default is a timeout of 30 seconds
					case KLbsSettingPrivacyConnectionTimeout:
						{
						value = KLbsDefaultPrivacyConnectionTimeout;
						break;
						}

					// Default is feature off.
					case KLbsSpecialFeatureMaximumRequestDelay:
						{
						value = 0;	// 0 denotes off.	
						break;	
						}
						// Default is feature off.
					case KLbsSpecialFeatureAllowTBFinalNetPos:
						{
						value = CLbsAdmin::ESpecialFeatureOff;						
						break;
						}
  					// Default is feature off.
  					case KLbsSettingBehaviourMode:
 						value = CLbsAdmin::ELbsBehaviourCustom1;
 						break;
 						
					// Default is feature off.
					case KLbsSpecialFeatureWithholdDeliveryOfReferencePositions:
						{
						value = CLbsAdmin::ESpecialFeatureOff;						
						break;
						}

					// Default is feature off - the value is original symbian behaviour - ignore accuracy off.
					case KLbsSpecialFeatureIgnoreAccuracy:
						{
						value = CLbsAdmin::ESpecialFeatureOff;
						break;
						}

					case KLbsSettingPrivacyAppTimeout:
						{
						// for binary data compatibility set to a default value, 20s
						value = 20*1000;
						break;
						}
					
					// default value is original symbian behaviour - don't check for intermediate futile updates
					case KLbsSpecialFeatureIntermediateFutileUpdate:
						{
						value = CLbsAdmin::ESpecialFeatureOff;
						break;
						}

					// Default is Off, reject network induced privacy requests.
					case KLbsSettingHomeNetworkInducedLocate:
					case KLbsSettingRoamingNetworkInducedLocate:
						{
						value = CLbsAdmin::EExternalLocateOff;
						break;
						}

					// Setting really not found.
					default:
						{
						return ret;
						}
					}
				}

			// Set property here.
			ret = RProperty::Set(KConfigBusCategory, LbsAdminUtils::BitPosToNumber(KKeysArray[i]), value);
			if (ret != KErrNone) break;
			}
		}
	
	LBSLOG2(ELogP3, "NEW ACTION: Lbs Admin settings reset. errr: %d", ret);	
	return ret;
	}
	
/** Updates a setting in a relevant property and in the corresponding
Central Repository setting.
This method should be used by one of overloaded DoSet()s after
validating the attributes.

@param aSetting The setting to be changed.
@param aValue   The new value of the setting.
@return KErrNone if property has been set,  one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released */
TInt CLbsAdminImpl::SetSettingValidated(TLbsAdminSetting aSetting, const TInt& aValue)
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::SetSettingValidated()");
	
	// Validate value for particular setting
	TInt err = LbsAdminUtils::ValidateValue(aSetting, aValue);
	if (err != KErrNone)
		{
		return err;
		}
	// For settings that are changing a plugin Uid value, verify that
	// the plugin exists first
	// "Switch/Case" does not work with TInt64 so we have to use "if" instead
	if ((aSetting == KLbsSettingHomeProtocolModule) ||
		(aSetting == KLbsSettingRoamingProtocolModule) ||
		(aSetting == KLbsSettingSetClockModule))
		{
        #if defined(LBSADMIN_UNIT_TEST)
		// For unit testing we need to be able to set a Uid value, 
		// so use this test function instead of the real one to do it.
		TInt err = te_LbsAdminUtils::ConfirmSettingPluginExists(aSetting, aValue);
        #else
		TInt err = LbsAdminUtils::ConfirmSettingPluginExists(aSetting, aValue);
        #endif // LBSADMIN_UNIT_TEST
		if (err != KErrNone)
			{
			return err;
			}
		}
	
	TInt ret = KErrNone;
	ret = RProperty::Set(KConfigBusCategory, LbsAdminUtils::BitPosToNumber(aSetting), aValue);
	if(ret == KErrNone)
		{
		// A failure to write to the Central Repository is ignored as iCenRep is only used for
		// persitance (admin setting values are read from properties in CLbsAdminImpl::GetValidated)
		ret = iCenRep->Set(LbsAdminUtils::BitPosToNumber(aSetting), aValue);
		}
	
	if(ret == KErrNone)
		{
		ret = SyncCenrep(aSetting);
		}
	
	LBSLOG5(ELogP3, "NEW ACTION: Lbs Admin setting: 0x%08X%08X set to %d. err: %d", I64HIGH(aSetting), I64LOW(aSetting), aValue, ret);
	return ret;
	}
	
/** Updates a setting in a relevant property and in the corresponding
Central Repository setting.
This method should be used by one of overloaded DoSet()s after
validating the attributes.

@param aSetting The setting to be changed.
@param aValue   The new TLbsAdminInfoBase value of the setting.
@return KErrNone if property has been set,  one of the system wide error codes otherwise.
@capability WriteDeviceData
@see TLbsAdminSetting
@released */
TInt CLbsAdminImpl::SetInfoBaseValidated(TLbsAdminSetting aSetting, const TLbsAdminInfoBase& aInfo)
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::SetInfoBaseValidated()");
	
	// Validate value for particular setting
	TInt err = LbsAdminUtils::ValidateValue(aSetting, reinterpret_cast<const TInt&>(aInfo));
	if (err != KErrNone)
		{
		return err;
		}
	
	err = KErrNotFound;
	if(aSetting==KLbsProtocolModuleLoading)
		{
		const TLbsAdminProtocolModuleLoadingInfo& info = reinterpret_cast<const TLbsAdminProtocolModuleLoadingInfo&>(aInfo);
		err = RProperty::Set(KConfigBusCategory, LbsAdminUtils::BitPosToNumber(aSetting), info.LoadingStrategy());
		if(err == KErrNone)
			{
			// A failure to write to the Central Repository is ignored as iCenRep is only used for
			// persitance (admin setting values are read from properties in CLbsAdminImpl::GetValidated)
			err = iCenRep->Set(LbsAdminUtils::BitPosToNumber(aSetting), static_cast<TInt>(info.LoadingStrategy()));
			err = SyncCenrep(aSetting);
			}
		}
	else if((aSetting == KLbsSettingHomeProtocolModule) || (aSetting == KLbsSettingRoamingProtocolModule))
		{
		const TLbsAdminProtocolModulesInfo& info = reinterpret_cast<const TLbsAdminProtocolModulesInfo&>(aInfo);
		
		// Get the module Ids
		TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray moduleArray;
		TInt modules = KLbsAdminMaxProtocolModuleIds;
		info.GetModuleIds(moduleArray, modules);
				
		// Verify that the plugins exist
		#if defined(LBSADMIN_UNIT_TEST)
		err = KErrNone;
		#else
		err = LbsAdminUtils::ConfirmPmPluginsExist(aSetting, moduleArray, modules);
		#endif // LBSADMIN_UNIT_TEST
		
		if (err != KErrNone)
			{
			return err;
			}
		
		// If the number of modules is zero then blank the protocol module array and the default value
		if(modules == 0)
			{
			for(TUint i=0; i<KLbsAdminMaxProtocolModuleIds; i++)
				{
				moduleArray[i].iUid = 0;
				}
			modules = KLbsAdminMaxProtocolModuleIds;
			}
		
		// Set the default protocol module
		err = RProperty::Set(KConfigBusCategory, LbsAdminUtils::BitPosToNumber(aSetting), moduleArray[0].iUid);
		if(err == KErrNone)
			{
			err = iCenRep->Set(LbsAdminUtils::BitPosToNumber(aSetting), static_cast<TInt>(moduleArray[0].iUid));
			}
		
		if(err == KErrNone)
			{
			// Set the remining protocol modules
			TUint32 keyBase = (aSetting == KLbsSettingHomeProtocolModule) ? KLbsProtocolModulesHomeBase : KLbsProtocolModulesRoamingBase;
			TLbsAdminSetting adminSetting = (aSetting == KLbsSettingHomeProtocolModule) ? KLbsProtocolModulesHome : KLbsProtocolModulesRoaming;
			
			TBuf8<(KLbsAdminMaxProtocolModuleIds-1) * sizeof(TInt32)> buf;
	
			for(TInt count = 1; (count != modules) && (err == KErrNone); count++, keyBase++)
				{
		 		err = iCenRep->Set(keyBase, static_cast<TInt>(moduleArray[count].iUid));
	
		 		TPckg<TInt32> package(moduleArray[count].iUid);
		 		buf.Append(package);
				}
			
			if(err == KErrNone)
				{
				err = RProperty::Set(KConfigBusCategory, adminSetting, buf);
				}
			}
		}

	LBSLOG5(ELogP3, "NEW ACTION: Lbs Admin setting: 0x%08X%08X set to %d. err: %d", I64HIGH(aSetting), I64LOW(aSetting), aInfo, err);
	return err;
	}

/**
As a result of race conditions when there are two or more simultaneous calls of the set functions
the values stored in the P&S and CenRep may be different. So we may need to sync the CenRep value with
the last P&S value. 
We run the cycle only once. It must be enough for handling two concurrent calls. We consider the probability 
of three or more concurrent calls as very low.
*/
TInt CLbsAdminImpl::SyncCenrep(TLbsAdminSetting aSetting)
	{
	TInt ret = KErrNone;
	
	while(ret==KErrNone)
		{
		TInt propVal;
		ret = RProperty::Get(KConfigBusCategory, LbsAdminUtils::BitPosToNumber(aSetting), propVal);
		if(ret!=KErrNone)
			{
			break;
			}
		
		TInt repVal;
		ret = iCenRep->Get(LbsAdminUtils::BitPosToNumber(aSetting), repVal);
		if(ret!=KErrNone)
			{
			break;
			}
		
		//We exit the loop, when both values are consistent.
		if(propVal==repVal)
			{
			break;
			}
		
		iCenRep->Set(LbsAdminUtils::BitPosToNumber(aSetting), propVal);
		break;
		}
	
		return ret;
	}
	
/**
Returns an LBS Administration setting obtained from the corresponding property.
This method should be used by one of overloaded DoGet()s after validating the attributes.

For the KLbsSpecialFeatureMaximumRequestDelay read-only seting KErrArgument is returned if the
value is greater than KLbsMaximumRequestDelayMax.

@param aSetting The setting to be read.
@param aValue   The value of the setting (returned).
@return KErrNone if successful, one of the system wide error codes if failed to read the property value.
@see TLbsAdminSetting
@see KLbsMaximumRequestDelayMax
@released 
 */
TInt CLbsAdminImpl::GetSettingValidated(TLbsAdminSetting aSetting, TInt& aValue)
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::GetSettingValidated()");
	
	TInt err = KErrNone;
	if(aSetting==KLbsProtocolModuleLoading)
		{
		TInt val;
		err = RProperty::Get(KConfigBusCategory, LbsAdminUtils::BitPosToNumber(aSetting), val);
		if(err==KErrNone)
			{
			LBSLOG4(ELogP3, "Lbs Admin setting: value: %d read from: 0x%08X%08X", val,  I64HIGH(aSetting), I64LOW(aSetting) );
			TLbsAdminProtocolModuleLoadingInfo& info = reinterpret_cast<TLbsAdminProtocolModuleLoadingInfo&>(aValue);
			info.SetLoadingStrategy(val);
			}
		}
	else if(aSetting == KLbsProtocolModulesHome)
		{
		err = GetProtocolModulesValidated(aSetting, aValue, KLbsSettingHomeProtocolModule);
		}
	else if(aSetting == KLbsProtocolModulesRoaming)
		{
		err = GetProtocolModulesValidated(aSetting, aValue, KLbsSettingRoamingProtocolModule);
		}
	else
		{
		err = RProperty::Get(KConfigBusCategory, LbsAdminUtils::BitPosToNumber(aSetting), aValue);
		if (err==KErrNone)
			{
			LBSLOG4(ELogP3, "Lbs Admin setting: value: %d read from: 0x%08X%08X", aValue,  I64HIGH(aSetting), I64LOW(aSetting) );
		
			// For KLbsSpecialFeatureMaximumRequestDelay setting validate value.
			if ((aSetting == KLbsSpecialFeatureMaximumRequestDelay) && (aValue > KLbsMaximumRequestDelayMax))
				{
				// Return err and switch off KLbsSpecialFeatureMaximumRequestDelay setting.
				aValue = 0;	// 0 indicates setting off.
				err = KErrArgument;
				}
			}
		}
	
	if(err!=KErrNone)
		{
		LBSLOG4(ELogP3, "Lbs Admin setting: reading from: 0x%08X%08X returned error: %d", I64HIGH(aSetting), I64LOW(aSetting), err);
		}
	return err;
	}

/** 
Populate the passed TLbsAdminProtocolModulesInfo class with the module ids held in the RProperty
byte array
 
@param aSetting	The setting to be read.
@param aValue	The value of the setting (returned).
@param aDefault	default module cenrep key
@return KErrNone if successful, one of the system wide error codes if failed to read the property value.
@released */
TInt CLbsAdminImpl::GetProtocolModulesValidated(TLbsAdminSetting aSetting, TInt& aValue, const TLbsAdminSetting aDefault)
	{
	TInt err = KErrNone;
	TInt moduleCount = 0;
	TInt val;
	TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray moduleUids;
	TLbsAdminProtocolModulesInfo& info = reinterpret_cast<TLbsAdminProtocolModulesInfo&>(aValue);

	err = RProperty::Get(KConfigBusCategory, LbsAdminUtils::BitPosToNumber(aDefault), val);
	if(err == KErrNone)
		{
		// Set the default module
		moduleUids[0] = TUid::Uid(val);
		moduleCount++; // Add one for the default module
		
		TBuf8<(KLbsAdminMaxProtocolModuleIds-1) * sizeof(TInt32)> buf;
		err = RProperty::Get(KConfigBusCategory, aSetting, buf);

		if(buf.Length())
			{
			moduleCount += (buf.Length() / sizeof(TInt32)); // count of modules in the buffer
			
			if(err == KErrNone)
				{
				TInt32* myPtr = reinterpret_cast<TInt32*> (&buf[0]);
	
				// Set the remaining protocol modules
				for(TUint index = 1; index != moduleCount; index++, myPtr++)
					{
					moduleUids[index] = TUid::Uid(*myPtr);
					}
				}
			}
		// Set TLbsProtocolModuleIdArray in TLbsAdminProtocolModulesInfo 
		info.SetModuleIds(moduleUids, moduleCount);
		}
	
	return err;
	}


//********************************************************************************************************
// Class CLbsAdminImplImpl - Private Methods
//********************************************************************************************************

/** Check the admin cenrep values are within range
@leave KErrGeneral if one of the values is not within range
*/
void CLbsAdminImpl::CheckCenRepL()
	{
#ifdef _DEBUG
	// For UDEB builds only, check the CenRep settings to ensure no invalid
	// entries have been setup in ROM
	const TUid KLBSAdminRepositoryUid ={0x1028224B};
	CRepository* rep = CRepository::NewLC(KLBSAdminRepositoryUid);
	TBool errorsFound = EFalse;
	
	TInt val = CLbsAdmin::ESelfLocateUnknown;
	TInt err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingHomeSelfLocate), val);
	if	 (KErrNone == err)
		{
		if ((val < CLbsAdmin::ESelfLocateOn) ||
			(val > CLbsAdmin::ESelfLocateOff))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingHomeSelfLocate\n");
			errorsFound = ETrue;
			}
		}
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingRoamingSelfLocate), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::ESelfLocateOn) ||
			(val > CLbsAdmin::ESelfLocateOff))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingRoamingSelfLocate\n");
			errorsFound = ETrue;
			}
		}

	val = CLbsAdmin::EExternalLocateUnknown;
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingHomeExternalLocate), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::EExternalLocateOn) ||
			(val > CLbsAdmin::EExternalLocateOffButNotify))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingHomeExternalLocate\n");
			errorsFound = ETrue;
			}
		}
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingRoamingExternalLocate), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::EExternalLocateOn) ||
			(val > CLbsAdmin::EExternalLocateOffButNotify))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingRoamingExternalLocate\n");
			errorsFound = ETrue;
			}
		}
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingHomeEmergencyLocate), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::EExternalLocateOn) ||
			(val > CLbsAdmin::EExternalLocateOffButNotify))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingHomeEmergencyLocate\n");
			errorsFound = ETrue;
			}
		}
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingRoamingEmergencyLocate), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::EExternalLocateOn) ||
			(val > CLbsAdmin::EExternalLocateOffButNotify))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingRoamingEmergencyLocate\n");
			errorsFound = ETrue;
			}
		}

	val = CLbsAdmin::ETransmitLocateUnknown;
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingHomeTransmitLocate), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::ETransmitLocateOn) ||
			(val > CLbsAdmin::ETransmitLocateOff))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingHomeTransmitLocate\n");
			errorsFound = ETrue;
			}
		}
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingRoamingTransmitLocate), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::ETransmitLocateOn) ||
			(val > CLbsAdmin::ETransmitLocateOff))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingRoamingTransmitLocate\n");
			errorsFound = ETrue;
			}
		}

	val = CLbsAdmin::EGpsModeUnknown;
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingHomeGpsMode), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::EGpsPreferTerminalBased) ||
			(val > CLbsAdmin::EGpsAlwaysTerminalAssisted))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingHomeGpsMode\n");
			errorsFound = ETrue;
			}
		}
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingRoamingGpsMode), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::EGpsPreferTerminalBased) ||
			(val > CLbsAdmin::EGpsAlwaysTerminalAssisted))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingRoamingGpsMode\n");
			errorsFound = ETrue;
			}
		}

	val = CLbsAdmin::ELbsSystemStartModeUnknown;
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingLbsSystemStartMode), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::ELbsSystemTransient) ||
			(val > CLbsAdmin::ELbsSystemPersistent))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingLbsSystemStartMode\n");
			errorsFound = ETrue;
			}
		}

	val = CLbsAdmin::EPrivacyTimeoutActionUnknown;
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingPrivacyTimeoutAction), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::EPrivacyTimeoutNetworkDefined) ||
			(val > CLbsAdmin::EPrivacyTimeoutIgnore))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingPrivacyTimeoutAction\n");
			errorsFound = ETrue;
			}
		}

	val = CLbsAdmin::EPrivacyHandlerUnknown;
	err = rep->Get(LbsAdminUtils::BitPosToNumber(KLbsSettingPrivacyHandler), val);
	if (KErrNone == err)
		{
		if ((val < CLbsAdmin::EPrivacyHandleByNotifier) ||
			(val > CLbsAdmin::EPrivacyHandleByAdvancedNotifier))
			{
			LBSLOG_ERR(ELogP3, "CLbsSystem::DefineLBSPropertiesL() - Invalid value for KLbsSettingPrivacyHandler\n");
			errorsFound = ETrue;
			}
		}

	if (errorsFound)
		{
		User::LeaveIfError(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(rep);
#endif // _DEBUG
	}

/** Initialises the configuration

@leave One of the system wide error codes
@capability WriteDeviceData if the configuration settings need to be created/initialised, none otherwise
@released */
void CLbsAdminImpl::DefinePropertiesL() 
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::DefinePropertiesL()");
	TInt err;
	TInt value;
	TUint32 key;

	CheckCenRepL();

	// Define array properties
	err = DefineArrayProperties(KLbsProtocolModulesHome, KLbsProtocolModulesHomeBase);
	err = DefineArrayProperties(KLbsProtocolModulesRoaming, KLbsProtocolModulesRoamingBase);
	
	// Define integer properties
	for (TInt i = 0; i < KKeysArrayCount; i++)
		{
 		key = LbsAdminUtils::BitPosToNumber(KKeysArray[i]);
 		
  		// Attempt to get the property. If the property already exists, then we do not have to define it.
 		// This will allow client processes that have the correct read capabilites (none) to be able to read
 		// the property without trying to create or set it.
 		err = RProperty::Get(KConfigBusCategory, key, value);

		if (err == KErrNotFound)
			{
			// A process using the Admin API to Define the Properties must have a
			// SID below KUidSecurityThesholdCategoryValue and be WriteDeviceData.
			// This is neccessary after 9.1 to be able to use the following overload
			// of RProperty::Define.
	 		err = RProperty::Define(KConfigBusCategory, key, RProperty::EInt, 
									ECapability_None, ECapabilityWriteDeviceData);
			
			//If the Property has just been defined,we set it to the value of the setting in the CenRep.
	 		//If err==KErrAlreadyExists, that means another thread is calling DefinePropertiesL at the same moment.
	 		//By setting the property in this case, we avoid the situation when GetSettingValidated is called for
	 		//an initialized property. The drawback of this solution is that the value set in SetSettingValidated
	 		//may be overriden by another thread. But we consider the probability of it as extremely low. 
			if (err==KErrNone || err==KErrAlreadyExists)
				{
				LBSLOG2(ELogP1, "CLbsAdminImpl::DefinePropertiesL() i = %d",i);
				
			  	err= iCenRep->Get(key,value);
				// If not found then is either an oldcenrep that hasn't got the new value in it or its corrupt!
			  	if (err != KErrNone) 
					{
					// Look for new additions.
					switch (KKeysArray[i])
						{
						// Default logging is on.
						case KLbsSettingLogger:
							{
							err = KErrNone;
							value = CLbsAdmin::ELoggerOn;
							break;	
							}
							
						// Default is to load protocol modules at LBS start-up by roaming status.
						case KLbsProtocolModuleLoading:
							{
							err = KErrNone;
							value = TLbsAdminProtocolModuleLoadingInfo::EProtocolModuleLoadingByStatus;
							break;
							}
	
						// Default is to allow a single external locate request at a time
						case KLbsSettingMaximumExternalLocateRequests:
							{
							err = KErrNone;
							value = KLbsDefaultMaximumExternalLocateRequests;
							break;
							}
							
						// Default is a delay of 2 seconds
						case KLbsSettingPrivacyShutdownDelay:
							{
							err = KErrNone;
							value = KLbsDefaultPrivacyShutdownDelay;
							break;
							}
	
						// Default is to allow up to 4 concurrent network privacy sessions
						case KLbsSettingPrivacyMaxNetworkSessions:
							{
							err = KErrNone;
							value = KLbsDefaultPrivacyMaxNetworkSessions;
							break;
							}
	
						// Default is a timeout of 30 seconds
						case KLbsSettingPrivacyConnectionTimeout:
							{
							err = KErrNone;
							value = KLbsDefaultPrivacyConnectionTimeout;
							break;
							}
	
						// Default is feature off.
						case KLbsSpecialFeatureMaximumRequestDelay:
							{
							err = KErrNone;
							value = 0;	// 0 denotes off.	
							break;	
							}
	
						// Default is feature off.
						case KLbsSpecialFeatureAllowTBFinalNetPos:
							{
							err = KErrNone;
							value = CLbsAdmin::ESpecialFeatureOff;						
							break;	
							}
						
						// default value is original symbian behaviour - ignore accuracy off
						case KLbsSpecialFeatureIgnoreAccuracy:
							{
							err = KErrNone;
							value = CLbsAdmin::ESpecialFeatureOff;
							break;
							}
	  					// Default is feature off.
	  					case KLbsSettingBehaviourMode:
	  						err = KErrNone;
	 						value = CLbsAdmin::ELbsBehaviourCustom1;
	 						break;
	 						
							// default value is feature off
						case KLbsSpecialFeatureWithholdDeliveryOfReferencePositions:
							{
							err = KErrNone;
							value = CLbsAdmin::ESpecialFeatureOff;
							break;
							}
	
						case KLbsSettingPrivacyAppTimeout:
							{
							LBSLOG(ELogP1, "CLbsAdminImpl::DefinePropertiesL() setting KLbsSettingPrivacyAppTimeout to 20s as not found in cenrep");
							// for binary data compatibility set to a default value, 20s
							err = KErrNone;
							value = 20 * 1000;
							break;
							}
		
						// default value is original symbian behaviour - don't check for intermediate futile updates
						case KLbsSpecialFeatureIntermediateFutileUpdate:
							{
							err = KErrNone;
							value = CLbsAdmin::ESpecialFeatureOff;
							break;

							}

						// Default is Off, reject network induced privacy requests.
						case KLbsSettingHomeNetworkInducedLocate:
						case KLbsSettingRoamingNetworkInducedLocate:
							{
							err = KErrNone;
							value = CLbsAdmin::EExternalLocateOff;
							break;
							}

						// Setting really not found.
						default:
							{
							break;
							}
						}
					}
				// Set property here.
				if (err == KErrNone)	
					{
				  	LBSLOG3(ELogP1, "CLbsAdminImpl::DefinePropertiesL() setting property %d to %d", i, value);
					err = RProperty::Set(KConfigBusCategory, key, value);
					if (err != KErrNone) break;
					}
					
				else
					{
					break;
					}
				}
			else
				{
				// It is a real error during the RProperty::Define()
				LBSLOG_ERR2(ELogP3, "Defining Lbs Admin properties failed. err: %d", err);
				break;
				}
			}
		else if (err != KErrNone)
			{
			// It is a real error during the RProperty::Get()
			LBSLOG_ERR2(ELogP3, "Getting Lbs Admin properties failed. err: %d", err);
			break;
			}
		}
	
	User::LeaveIfError(err);
	}

/**
Defines the properties for array types. These are read from a number of central repository
keys starting at 'aKeyBase'
 
@param aAdminSetting	setting to be defined
@param aKeyBase			base cenrep value for range
@return KErrNone if successful, one of the system wide error codes if failed to set the property value.
@released */
TInt CLbsAdminImpl::DefineArrayProperties(const TLbsAdminSetting aAdminSetting, TUint32 aKeyBase)
	{
	TInt err;
	TInt value;

	err = RProperty::Get(KConfigBusCategory, aAdminSetting, value);
	if(err == KErrNotFound)
		{
		err = RProperty::Define(KConfigBusCategory, aAdminSetting, RProperty::EByteArray, 
							ECapability_None, ECapabilityWriteDeviceData);
		
		TBuf8<(KLbsAdminMaxProtocolModuleIds-1) * sizeof(TInt32)> buf;

		for(TInt count = 0; count < (KLbsAdminMaxProtocolModuleIds-1); count++, aKeyBase++)
			{
	 		err = iCenRep->Get(aKeyBase,value);
	 		
	 		if(err == KErrNone)
	 			{
	 			TPckg<TInt32> package(value);
	 			buf.Append(package);
	 			}
	 		else
	 			{
	 			break;
	 			}
			}
		err = RProperty::Set(KConfigBusCategory, aAdminSetting, buf);	
		}
	return err;
	}

/** Deletes all the setting subscribers.

It automatically cancels outstanding setting event request (if any).

@capability WriteDeviceData
@released */
void CLbsAdminImpl::DeleteSubscribers()
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::DeleteSubscribers()");
	CleanupSubscribersArray(&iSubscribers);
	}

/** Deletes all the setting subscribers whose setting is not selected by aMask.
If there is a subscriber for a setting selected by aMask, then that setting is
removed from aMask. That way, when the method finishes, the resulting mask selects
only those settings for which a subscriber is missing.

@capability WriteDeviceData
@released */
void CLbsAdminImpl::DeleteSubscribersNotInMask(TLbsAdminSettingGroup& aMask)
	{
	LBSLOG(ELogP1, "CLbsAdminImpl::DeleteSubscribersNotInMask()");
	for (TInt i = iSubscribers.Count()-1; i>=0; i--)
		{
		TLbsAdminSetting subscSetting = iSubscribers[i]->GetSetting();
		if (!(subscSetting & aMask))
			{
			// Subscriber not select by the mask
			delete (iSubscribers)[i];
			iSubscribers.Remove(i);
			}
		else
			{
			// Subscriber already exists: remove setting from mask
			aMask &= ~subscSetting;
			}
 		}
	}

//********************************************************************************************************
// Static Class LbsAdminUtils
//********************************************************************************************************
/** Bit Position to Number 1...64
    0 if aSetting is 0 */
TUint32 LbsAdminUtils::BitPosToNumber(TLbsAdminSetting aSetting)
	{
	TLbsAdminSetting setting = aSetting;
	TInt number = 0;
	
	while(setting != 0)
		{
		number++;
		setting = setting >> 1;
		}

	return number;
	}

/* Utility function for checking that a particular ECom plugin implementation exists.

This function is used by the version of CLbsAdmin::DoSet() that sets
a plugin Uid in the LbsAdmin cenrep. It is used to check that the Uid is
valid before the setting is changed.

This function exists because REComSession::ListImplementationsL is a leaving
function, so putting it in a separate function makes it easier to TRAP() in
CLbsAdmin::DoSet().

@param aInterfaceUid The interface Uid of the ECom plugin to check for.
@param aImplementationUid The implementation Uid of the ECom plugin to check for.
@return ETrue if the plugin exists, EFalse otherwise
@released
@internalComponent
*/
TInt LbsAdminUtils::ConfirmSettingPluginExists(TLbsAdminSetting aSetting, TInt aValue)
	{
	// Check that the aValue refers to an ECom plugin that actually exists
	TUid interfaceUid;
	if ((aSetting == KLbsSettingHomeProtocolModule) ||
		(aSetting == KLbsSettingRoamingProtocolModule))
		{
		// Interface Uid for CNetworkProtocolBase - derived plugins
		interfaceUid = KNetworkProtocolInterfaceUid;
		}
	else if (aSetting == KLbsSettingSetClockModule)
		{
		// Interface Uid for XXX - derived plugins
		interfaceUid = KGPSSetClockInterfaceUid;
		}
	else
		{
		// Setting doesn't relate to a Uid
		return KErrArgument;
		}

	RImplInfoPtrArray infoArray;
	TRAPD(err, REComSession::ListImplementationsL(interfaceUid, infoArray));
	if (err != KErrNone)
		{
		infoArray.ResetAndDestroy();
		return err;
		}
	
	TBool foundMatch(EFalse);
	const TInt count = infoArray.Count();
	const TUid implUid(TUid::Uid(aValue));
	for (TInt i = 0; i < count; i++)
		{
		if (implUid == infoArray[i]->ImplementationUid())
			{
			foundMatch = ETrue;
			break;
			}
		}
	infoArray.ResetAndDestroy();

	return (foundMatch ? KErrNone : KErrNotFound);
	}

TInt LbsAdminUtils::ConfirmPmPluginsExist(
		TLbsAdminSetting aSetting,
		TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray aArray,
		TInt aCount)
	{
	// Check that the aValue refers to an ECom plugin that actually exists
	TUid interfaceUid;
	if ((aSetting == KLbsSettingHomeProtocolModule) ||
		(aSetting == KLbsSettingRoamingProtocolModule))
		{
		// Interface Uid for CNetworkProtocolBase - derived plugins
		interfaceUid = KNetworkProtocolInterfaceUid;
		}
	else
		{
		// Setting doesn't relate to a Uid
		return KErrArgument;
		}

	RImplInfoPtrArray infoArray;
	TRAPD(err, REComSession::ListImplementationsL(interfaceUid, infoArray));
	if (err != KErrNone)
		{
		infoArray.ResetAndDestroy();
		return err;
		}
	
	TBool foundMatch(EFalse);
	const TInt count = infoArray.Count();
	for(TInt arrayIndex = 0; arrayIndex < aCount; arrayIndex++)
		{
		TUid implUid = aArray[arrayIndex];
		foundMatch = EFalse;
		
		for (TInt i = 0; i < count; i++)
			{
			if (implUid == infoArray[i]->ImplementationUid())
				{
				foundMatch = ETrue;
				break;
				}
			}
		
		// Break out completely if the PM was not found
		if(foundMatch == EFalse)
			{
			break;
			}
		}

	infoArray.ResetAndDestroy();

	return (foundMatch ? KErrNone : KErrNotFound);
	}

/* Utility function for checking the value for particular setting valid or not.

This function is used by the version of CLbsAdmin::DoSet() that sets
value in the LbsAdmin cenrep. It is used to check that the value is
valid or not 

@param aSetting The setting to be read.
@param aValue   The value of the setting 
@return KErrNone if the value valid, otherwise KErrArgument or KErrNotSupported.
@released 
@internalComponent
*/
TInt LbsAdminUtils::ValidateValue(TLbsAdminSetting aSetting, const TInt& aValue)
	{
	TInt err = KErrNone;

	//Self Locate Service state
	if ((aSetting == KLbsSettingHomeSelfLocate) ||
		(aSetting == KLbsSettingRoamingSelfLocate))
		{
        if(aValue <= CLbsAdmin::ESelfLocateUnknown || aValue > CLbsAdmin::ESelfLocateOff)		
            {
            err = KErrArgument;	
            }
		}
		
	//External Locate Service flags.
	else if (	(aSetting == KLbsSettingHomeExternalLocate) ||
				(aSetting == KLbsSettingRoamingExternalLocate) ||
				(aSetting == KLbsSettingHomeNetworkInducedLocate) ||
				(aSetting == KLbsSettingRoamingNetworkInducedLocate))
		{
		if(aValue <= CLbsAdmin::EExternalLocateUnknown || aValue > CLbsAdmin::EExternalLocateOffButNotify)
			{
			err = KErrArgument;	
	        }
		}
		
	//Send to 3rd party state.
	else if (	(aSetting == KLbsSettingHomeTransmitLocate) ||
				(aSetting == KLbsSettingRoamingTransmitLocate))
		{
		if(aValue <= CLbsAdmin::ETransmitLocateUnknown  || aValue > CLbsAdmin::ETransmitLocateOff)
			{
			err = KErrArgument;	
	        }
		}
		
	//GPS mode flags.
	else if (	(aSetting == KLbsSettingHomeGpsMode) ||
				(aSetting == KLbsSettingRoamingGpsMode))
		{
		if(aValue < CLbsAdmin::EGpsModeUnknown || aValue > CLbsAdmin::EGpsAlwaysTerminalAssisted)
			{
			err = KErrArgument;	
	        }
		}
		
	//Lbs Behaviour Mode
	else if (aSetting == KLbsSettingBehaviourMode)
		{
		if(aValue <= CLbsAdmin::ELbsBehaviourModeUnknown || aValue > CLbsAdmin::ELbsBehaviourCustom1)
			{
			err = KErrArgument;	
	        }
		}
		
	//Lbs System Startup Mode
	else if (aSetting == KLbsSettingLbsSystemStartMode)
		{
		if(aValue <= CLbsAdmin::ELbsSystemStartModeUnknown || aValue > CLbsAdmin::ELbsSystemPersistent)
			{
			err = KErrArgument;	
	        }
		}
		
	//Action if user does not respond
	else if (aSetting == KLbsSettingPrivacyTimeoutAction)
		{
		if(aValue <= CLbsAdmin::EPrivacyTimeoutActionUnknown || aValue > CLbsAdmin::EPrivacyTimeoutReject)
			{
			if(aValue == CLbsAdmin::EPrivacyTimeoutIgnore)
				{
				err = KErrNotSupported;
				}
			else
				{
				err = KErrArgument;
				}
	        }
		}
		
	//Privacy Handler to be used
	else if (aSetting == KLbsSettingPrivacyHandler)
		{
		if(aValue <= CLbsAdmin::EPrivacyHandlerUnknown || aValue > CLbsAdmin::EPrivacyHandleByAdvancedNotifier)
			{
			err = KErrArgument;	
	        }
		}
		
	//Logger state.
	else if (aSetting == KLbsSettingLogger)
		{
		if(aValue <= CLbsAdmin::ELoggerUnknown || aValue > CLbsAdmin::ELoggerOff)
			{
			err = KErrArgument;	
	        }
		}
		
	//Special feature state.
	else if (	(aSetting == KLbsSpecialFeatureAllowTBFinalNetPos)||
				(aSetting == KLbsSpecialFeatureIgnoreAccuracy) ||
				(aSetting == KLbsSpecialFeatureWithholdDeliveryOfReferencePositions) ||
				(aSetting == KLbsSpecialFeatureIntermediateFutileUpdate))
		{
		if(aValue <= CLbsAdmin::ESpecialFeatureUnknown || aValue > CLbsAdmin::ESpecialFeatureOff)
			{
			err = KErrArgument;	
	        }
		}

	// Protocol module loading state.
	else if (aSetting == KLbsProtocolModuleLoading)
		{
		const TLbsAdminProtocolModuleLoadingInfo& info = reinterpret_cast<const TLbsAdminProtocolModuleLoadingInfo&>(aValue);
		if(	info.LoadingStrategy() <= TLbsAdminProtocolModuleLoadingInfo::EProtocolModuleLoadingUnknown ||
			info.LoadingStrategy() > TLbsAdminProtocolModuleLoadingInfo::EProtocolModuleLoadingAll)
			{
			err = KErrArgument;	
			}
		}

	// Local Clock adjustment state.
	else if (	(aSetting == KLbsSettingClockAdjust) ||
				(aSetting == KLbsSettingAllowManualClockAdjust))
		{
		if (aValue <= CLbsAdmin::EClockAdjustUnknown || aValue > CLbsAdmin::EClockAdjustOff)
			{
			err = KErrArgument;
			}
		}

	return err;	
	}

