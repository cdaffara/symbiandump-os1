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
// Location Based Services Administration API
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBS_ADMIN_H
#define LBS_ADMIN_H


//-------------------------------------------------------------------------------
#include <e32base.h>
#include <lbsadmindatatypes.h>
#include <lbsadminsettings.h>

//-------------------------------------------------------------------------------
class CLbsAdminImpl;
class MLbsAdminObserver;


//-------------------------------------------------------------------------------
/** Location Based Services Administration API.

Manages global configuration and sets/selects 
an active Network Protocol Module.
May be used:
	a) by a Control Panel UI application to configure the LBS component
	b) to notify internal LBS components about changes in the configuration
	c) by a UI Status Bar (Icons) to reflect the status of the LBS component.

A client may also use the API to receive changes to any of
the configuration settings through a call-back mechanism.
Please note that a setting may change several times before a client will receive 
the call-back (due to the pre-emptive and non pre-emptive multitasking mechanisms used).
The call-back will also be received if a setting was set to the same value. 
This means that the call-back is triggered more by a setting write (Set(), ResetToDefault() etc.) operation 
than by a real change to the value of the setting.

@publishedPartner
@released
@see MLbsAdminObserver
*/
class CLbsAdmin : public CBase
	{
public:
	/** Self Locate Service state.
	@see KLbsSettingHomeSelfLocate
	@see KLbsSettingRoamingSelfLocate */
	enum TSelfLocateService
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		ESelfLocateUnknown	= 0x00,
		
		/** Always On */
		ESelfLocateOn		= 0x01,
		
		/** Always Off */
		ESelfLocateOff		= 0x02
		};
		

	/** External Locate Service flags.
	Any further extensions should be applied to the 
	TLbsAdminExtLocService::TExternalLocateService enumeration only.
	
	@see KLbsSettingHomeExternalLocate
	@see KLbsSettingRoamingExternalLocate 
	@see TLbsAdminExtLocService::TExternalLocateService*/
	enum TExternalLocateService
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		EExternalLocateUnknown				= 0x00,
				
		/** On */
		EExternalLocateOn					= 0x01,
		
		/** On, but forces verification even if a network stipulates notification only.
		e.g In a "lcs-LocationNotification" message the "notificationType" argument is "notifyLocationAllowed" */
		EExternalLocateOnButAlwaysVerify	= 0x02,
		
		/** External locate is Off, Do not notify user about any external attempts to get location */
		EExternalLocateOff					= 0x03,
		
		/** External locate is Off, Notify user about any external attempts to get location */
		EExternalLocateOffButNotify			= 0x04
		};		


	/** Send to 3rd party state.
	@see KLbsSettingHomeTransmitLocate
	@see KLbsSettingRoamingTransmitLocate */
	enum TTransmitLocateService
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		ETransmitLocateUnknown	= 0x00,
				
		/** Always On */
		ETransmitLocateOn		= 0x01,
		
		/** Always Off */
		ETransmitLocateOff		= 0x02
		};


	/** GPS mode flags.
	@see KLbsSettingHomeGpsMode
	@see KLbsSettingRoamingGpsMode */
	enum TGpsMode
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		EGpsModeUnknown				= 0x00,
				
		/** Get assistance data if possible/required, autonomous otherwise.
			Calculate location locally (at the mobile). */
		EGpsPreferTerminalBased		= 0x01,
		
		/** Currently not supported. 
		Always get assistance data.
		Calculate location locally (at the mobile). */
		EGpsAlwaysTerminalBased		= 0x02,
		
		/** Never get assistance data */
		EGpsAutonomous				= 0x03,
		
		/** Get assistance data if possible/required, autonomous otherwise. 
			Send raw data from GPS to network. 
			Network calculates position. */		
		EGpsPreferTerminalAssisted	= 0x04,
		
		/** Always get assistance data.
			Send raw data from GPS to network. 
			Network calculates position. */		
		EGpsAlwaysTerminalAssisted	= 0x05
		};

   
 	/** Lbs Behaviour Mode
  	The behaviour of Lbs may be altered by changing the behavior mode
  	@see KLbsBehaviourMode */
  	enum TLbsBehaviourMode
  		{
  		/** Unknown. May be used to initialise a variable before getting the value of the setting */
  		ELbsBehaviourModeUnknown	= 0x00,
  
  		/** LBS original mode - behaves as per original Lbs specification*/
  		ELbsBehaviourModeOriginal	= 0x01,
  		
  		/** Alternative/custom mode of operation for LBS. */
  		ELbsBehaviourCustom1		= 0x02
  		};
  

	/** Lbs System Startup Mode
	In the Persistent Mode all the sub-components required to perform 
	an emergency call location are loaded and configured at boot time.
	@see KLbsSettingLbsSystemStartMode */
	enum TLbsSystemStartMode
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		ELbsSystemStartModeUnknown	= 0x00,

		/** The LBS component is in memory only if one or more local clients are connected. */
		ELbsSystemTransient			= 0x01,
		/** The LBS component is in memory at all times. 
			Used to support emergency calls and external location requests. */
		ELbsSystemPersistent		= 0x02
		};
		
	/** The action if a user does not respond to a privacy verification request.  
	This setting is ignored if LBS is configured to use a Privacy Q&N Notifier - if KLbsSettingPrivacyHandler is EPrivacyHandleByAdvancedNotifier. 
	If using a Privacy Q&N Notifier, the timeout action is specified by the Notifier - a negative error code returned from the Notifier means that LBS should reject the privacy request.
	@see KLbsSettingPrivacyTimeoutAction 
	@see TPrivacyHandler
	*/
	enum TPrivacyTimeoutAction
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		EPrivacyTimeoutActionUnknown	= 0x00,
		
		/** Action defined by network. 
		(The "notificationType" received in a "lcs-LocationNotification" message) */
		EPrivacyTimeoutNetworkDefined	= 0x01,
		
		/** Send Rejection */		
		EPrivacyTimeoutReject			= 0x02,
		
		/** Ignore request. Do not respond. (Not supported) */		
		EPrivacyTimeoutIgnore			= 0x03
		};
		
	/** Privacy Handler to be used.
	@see KLbsSettingPrivacyHandler */
	enum  TPrivacyHandler
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		EPrivacyHandlerUnknown				= 0x00,
		
		/** Use the privacy Notifier */
		EPrivacyHandleByNotifier			= 0x01,
		
		/** Use the privacy Controller */
		EPrivacyHandleByController			= 0x02,
		
		/** Use the advanced privacy Notifier */
		EPrivacyHandleByAdvancedNotifier	= 0x03
		};

	/** Logger state.
	When the logger is in the off state, any usage of the logging API will not result in an error
	@see KLbsSettingLogger */
	enum TLogger
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		ELoggerUnknown	= 0x00,
		
		/** The logger is on */
		ELoggerOn 		= 0x01,
		
		/** The logger is off */
		ELoggerOff		= 0x02
		};

	/** Special feature state.
	@see KLbsSpecialFeatureAllowTBFinalNetPos 
	@see KLbsSpecialFeatureIgnoreAccuracy
	@see KLbsSpecialFeatureWithholdDeliveryOfReferencePositions
	@see KLbsSpecialFeatureIntermediateFutileUpdate */
	enum TSpecialFeature
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		ESpecialFeatureUnknown	= 0x00,
		
		/** The special feature is on */
		ESpecialFeatureOn 		= 0x01,
		
		/** The special feature is off */
		ESpecialFeatureOff		= 0x02
		};

	/** Local Clock adjustment
	@see KLbsSettingClockAdjust
	@see KLbsSettingAllowManualClockAdjust */
	enum TClockAdjust
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		EClockAdjustUnknown	= 0x00,
				
		/** Adjust. */
		EClockAdjustOn		= 0x01,
		
		/** Don't adjust. */
		EClockAdjustOff		= 0x02
		};

public:		
	IMPORT_C static CLbsAdmin* NewL();
	IMPORT_C static CLbsAdmin* NewL(MLbsAdminObserver& aObserver, const TLbsAdminSettingGroup& aSettingsMask = KLbsSettingNone);
	virtual ~CLbsAdmin();
	
	IMPORT_C void SetNotificationMaskL(const TLbsAdminSettingGroup& aSettingsMask);
	IMPORT_C void ClearNotificationMask();
	
	IMPORT_C TInt ResetToDefault();
	
	template <typename T>
		inline TInt Set(const TLbsAdminSetting& aSetting, const T&  aValue);
		
	template <typename T>
		inline TInt Get(const TLbsAdminSetting& aSetting, T&  aValue) const;
		
	
private:
	CLbsAdmin();
	void ConstructL();
	void ConstructL(MLbsAdminObserver& aObserver, const TLbsAdminSettingGroup& aSettingsMask);
	
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TSelfLocateService& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TExternalLocateService& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TTransmitLocateService& aValue);	
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TGpsMode& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TLbsSystemStartMode& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TLbsBehaviourMode& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TPrivacyTimeoutAction& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TUid& aModuleId);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TLbsQualityProfileId& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TPrivacyHandler& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TLogger& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TClockAdjust& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TUint& aValue);
	IMPORT_C TInt DoSet(const TLbsAdminSetting& aSetting, const TLbsAdminInfoBase& aInfo);
	
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TSelfLocateService& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TExternalLocateService& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TTransmitLocateService& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TGpsMode& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TLbsSystemStartMode& aValue) const;
 	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TLbsBehaviourMode& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TPrivacyTimeoutAction& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TUid& aModule) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TLbsQualityProfileId& aModule) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TPrivacyHandler& aModule) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TLogger& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TSpecialFeature& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TClockAdjust& aModule) const;		
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TUint& aValue) const;
	IMPORT_C TInt DoGet(const TLbsAdminSetting& aSetting, TLbsAdminInfoBase& aInfo) const;

private:
	/** pointer to the implementation */
	CLbsAdminImpl* iImpl;
	};
	


//-------------------------------------------------------------------------------	
/**
MLbsAdminObserver Call-back used to inform a client of the Lbs Administration API about a change
@publishedPartner
@released
@see CLbsAdmin
@param aError aSetting
*/
class MLbsAdminObserver
	{ 
public:
	IMPORT_C virtual TVersion Version();
	
	/** Call-back used to inform a client of the Lbs Administration API about a change
	to one of the settings
	@param aError Standard Symbian OS error code
	@param aSetting TLbsAdminSetting to observer
	 */
	virtual void OnSettingUpdateEvent(TInt aError, const TLbsAdminSetting& aSetting) = 0;
	};


#include <lbs/lbsadmin.inl>
#endif //LBS_ADMIN_H
