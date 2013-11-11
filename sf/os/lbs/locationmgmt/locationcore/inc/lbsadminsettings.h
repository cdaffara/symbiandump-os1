// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Location Based Services Administration API Settings
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBS_ADMIN_SETTINGS_H
#define LBS_ADMIN_SETTINGS_H


//-------------------------------------------------------------------------------
#include <lbsadmindatatypes.h>


//-------------------------------------------------------------------------------
// Settings IDs
//-------------------------------------------------------------------------------
/** No setting */
const TLbsAdminSetting KLbsSettingNone							= 0x0000000000000000LL;	// Setting 0x00

/** Self locate configuration for Home Network 
@see CLbsAdmin::TSelfLocateService */
const TLbsAdminSetting KLbsSettingHomeSelfLocate				= 0x0000000000000001LL;	// Setting 0x01

/** Self locate configuration when roaming
@see CLbsAdmin::TSelfLocateService */
const TLbsAdminSetting KLbsSettingRoamingSelfLocate				= 0x0000000000000002LL;	// Setting 0x02
		
/** Positioning quality profile for the Self Locate requests.
Used when the application does not specify its own criteria.
Profile definitions TBD.
@see TProfileId */
const TLbsAdminSetting KLbsSettingQualityProfileSelfLocate		= 0x0000000000000004LL;	// Setting 0x03
		
/** External locate configuration for Home Network.

Defines if external location requests (received from the network or from the Network Privacy API or the Privacy Request API) are processed.
This setting is not checked when LBS is configured to use a Privacy Q&N Notifier (if KLbsSettingPrivacyHandler == CLbsAdmin::EPrivacyHandleByAdvancedNotifier) - requests are always sent to the Notifier.

The setting may be retrieved in two ways. The default setting may be read by using 
the CLbsAdmin::TExternalLocateService type when calling CLbsAdmin::Get(). By using the 
TLbsAdminExtLocServicesInfo type it is possible to get both, the default setting, and the module specific settings.

Module specific settings supersede the default setting.

@see CLbsAdmin::TExternalLocateService 
@see TLbsAdminExtLocServicesInfo
@see KLbsSettingPrivacyHandler
*/
const TLbsAdminSetting KLbsSettingHomeExternalLocate 			= 0x0000000000000008LL;	// Setting 0x04

/** External locate configuration when roaming.
For additional information see @see KLbsSettingHomeExternalLocate.

@see CLbsAdmin::TExternalLocateService 
@see TLbsAdminExtLocServicesInfo
@see KLbsSettingPrivacyHandler
*/
const TLbsAdminSetting KLbsSettingRoamingExternalLocate 		= 0x0000000000000010LL;	// Setting 0x05
 
/** Positioning quality profile for the External Locate requests.
Used when the application does not specify its own criteria.
Profile definitions TBD.
@see TProfileId */
const TLbsAdminSetting KLbsSettingQualityProfileExternalLocate	= 0x0000000000000020LL;	// Setting 0x06

/** Emergency External locate configuration for Home Network.
@see CLbsAdmin::TExternalLocateService */
const TLbsAdminSetting KLbsSettingHomeEmergencyLocate 			= 0x0000000000000040LL;	// Setting 0x07

/** Emergency External locate configuration when roaming
@see CLbsAdmin::TExternalLocateService */
const TLbsAdminSetting KLbsSettingRoamingEmergencyLocate 		= 0x0000000000000080LL;	// Setting 0x08

/** Send to 3rd party for Home Network 
@see CLbsAdmin::TTransmitLocateService */
const TLbsAdminSetting KLbsSettingHomeTransmitLocate			= 0x0000000000000100LL;	// Setting 0x09

/** Send to 3rd party when roaming
@see CLbsAdmin::TTransmitLocateService */
const TLbsAdminSetting KLbsSettingRoamingTransmitLocate		= 0x0000000000000200LL;	// Setting 0x0A

/** Positioning quality profile for the Transmit to 3rd party requests.
Used when the application does not specify its own criteria.
Profile definitions TBD.
@see TProfileId */
const TLbsAdminSetting KLbsSettingQualityProfileTransmitLocate	= 0x0000000000000400LL;	// Setting 0x0B

/** GPS mode when in home network 
@see CLbsAdmin::TGpsMode */
const TLbsAdminSetting KLbsSettingHomeGpsMode					= 0x0000000000000800LL;	// Setting 0x0C

/** GPS mode when roaming 
@see CLbsAdmin::TGpsMode */
const TLbsAdminSetting KLbsSettingRoamingGpsMode				= 0x0000000000001000LL;	// Setting 0x0D

/** Network Protocol module(s) for Home Network.

The setting may be retrieved in two ways. The Id of the default module may be read by using 
the TProtocolModuleId type when calling CLbsAdmin::Get(). By using the TLbsAdminProtocolModulesInfo type 
it is possible to retrieve a list of all the Protocol Modules to be loaded.

@see TProtocolModuleId 
@see TLbsAdminProtocolModulesInfo */
const TLbsAdminSetting KLbsSettingHomeProtocolModule 			= 0x0000000000002000LL;	// Setting 0x0E

/** Network Protocol module(s) when roaming.
For details see @see KLbsSettingHomeProtocolModule.

@see TProtocolModuleId
@see TLbsAdminProtocolModulesInfo */
const TLbsAdminSetting KLbsSettingRoamingProtocolModule 		= 0x0000000000004000LL;	// Setting 0x0F

/** System mode configuration 
@see CLbsAdmin::TLbsSystemStartMode */
const TLbsAdminSetting KLbsSettingLbsSystemStartMode			= 0x0000000000008000LL;	// Setting 0x10

/** Privacy settings 
@see CLbsAdmin::TPrivacyTimeoutAction */
const TLbsAdminSetting KLbsSettingPrivacyTimeoutAction			= 0x0000000000010000LL;	// Setting 0x11

/** Privacy Handler selection
@see CLbsAdmin::TPrivacyHandler */
const TLbsAdminSetting KLbsSettingPrivacyHandler				= 0x0000000000020000LL;	// Setting 0x12

/** LBS behaviour mode selection
@see CLbsAdmin::TLbsBehaviourMode */
const TLbsAdminSetting KLbsSettingBehaviourMode					= 0x0000000000040000LL;	// Setting 0x13


/** Usage Logging
This setting is designed to only be read on LBS system startup. 
Changing it via this API while LBS is running could cause undefined behaviour. 
@see CLbsAdmin::TLogger */
const TLbsAdminSetting KLbsSettingLogger					= 0x0000000000080000LL;	// Setting 0x14

/** Enabling LBS special features.
Extra caution and full understanding of the effects is required when manipulating special features.
A user MUST NOT be allowed to change them (e.g. through a UI Control Panel type application).
A licensee must decide which features will be supported and fix them before a phone's release.

These settings are read-only.
	
@see KLbsSpecialFeatureMaximumRequestDelay
@see KLbsSpecialFeatureAllowTBFinalNetPos
@see KLbsSpecialFeatureIgnoreAccuracy
@see KLbsSpecialFeatureWithholdDeliveryOfReferencePositions
@see KLbsSettingLogger
@publishedPartner 
@released
*/

/** This feature allows LBS to temporarily buffer the following locations and errors:
	a single Reference Location from the network,
	a single Final Network Location from the network or
	a single Error Code from an AGPS Module.

Reason for introducing the feature:
When using RPositioner from the Location Acquisition API there are very short periods of time when the LBS
subsystem has no outstanding location request. This happens when an application has received an asynchronous
notification (CActive::RunL()) signalling completion of a previous request, but has not yet sent a new request
by calling RPositioner::NotifyPositionUpdate().
	
The time is defined in milliseconds and is of TLbsTimeIntervalMilliSeconds type. Acceptable values are between 0
and KLbsMaximumRequestDelayMax (values above this report KErrAgrument) where 0 turns the feature OFF. Note that
under normal circumstances using values greater than 100ms should not be required and suggests problems with a
user application or platform performance (e.g. a high priority process is using nearly 100% of CPU time).

The feature must NOT be used to accommodate for badly behaving applications 
that do not reissue RPositioner::NotifyPositionUpdate() immediately.

This setting is read-only, and is disabled if a value of 0 is used.

@see TLbsTimeIntervalMilliSeconds
@see KLbsSettingMaximumRequestDelay
@see RPositioner::NotifyPositionUpdate() 
@see MLbsNetworkProtocolObserver::ProcessLocationUpdate() */
const TLbsAdminSetting KLbsSpecialFeatureMaximumRequestDelay	= 0x0000000000100000LL;	// Setting 0x15

/** This feature enables the delivery of Terminal Based Final Network Locations to a local application.

This feature may cause very serious side effects (including not up to date locations being reported
to the client application) and must NOT be normally used.

This setting is read-only.

@see TSpecialFeature
@see RPositioner::NotifyPositionUpdate()
@see MLbsNetworkProtocolObserver::ProcessLocationUpdate() */
const TLbsAdminSetting KLbsSpecialFeatureAllowTBFinalNetPos    = 0x0000000000200000LL;	// Setting 0x16

/** This feature enables the delivery of reference positions to a local application that has opened
the RPositioner by explicitly specifying the module ID. 
 
This feature may cause very serious side effects (including not up to date locations being reported
to the client application) and must NOT be normally used.

This setting is read-only.

@see TSpecialFeature
@see RPositioner::NotifyPositionUpdate() */
const TLbsAdminSetting KLbsSpecialFeatureWithholdDeliveryOfReferencePositions    = 0x0000000000400000LL;	// Setting 0x17

/** Specifies the Protocol Module loading strategy. There are two loading strategies
supported.
Loading just one Protocol Module saves memory, but requires the LBS system to be restarted
in order to load a different module (e.g. after roaming status change).
Loading all (both roaming and home) Protocol Modules uses more memory, but does not require
restarting LBS when roaming status changes. The latter strategy should be used if emergency call
support is required (to avoid memory allocation failures). 

@see TLbsAdminProtocolModuleLoadingInfo*/
const TLbsAdminSetting KLbsProtocolModuleLoading    = 0x0000000000800000LL; // Setting 0x18

/** The maximum number of external locate requests that can be active at any time.

The value 0 is reserved for future use.
The value 1 is the default, this allows only one active external locate request at a time.
Values greater than 1 allow multiple requests to be handled simultaneously. Values up to KMaxTInt can be set, but memory limitations restrict the number of requests that can be supported.
A privacy request will leave with error code KErrNoMemory if there is insufficient free memory to support it.

If the maximum number of simultaneous requests is reached, then any further requests are rejected.
This setting is read only on LBS system startup. Setting it via the LBS Admin API has no effect while LBS running. Setting will take effect during next LBS system startup.
@see MLbsPrivacyRequestObserver::OnPrivacyResponse()
@see MPosVerificationObserver::HandleVerifyComplete()
*/
const TLbsAdminSetting KLbsSettingMaximumExternalLocateRequests    = 0x0000000001000000LL; // Setting 0x19

/** The time to wait after the last privacy client disconnects before shutting down LBS.
Only used with the Network Privacy API and the Privacy Request API. 
This setting is read only on LBS system startup. Setting it via the LBS Admin API has no effect while LBS running. Setting will take effect during next LBS system startup.

Time is specified in microseconds.

@see CPosNetworkPrivacy 
@see CLbsPrivacyRequest*/
const TLbsAdminSetting KLbsSettingPrivacyShutdownDelay    = 0x0000000002000000LL;	// Setting 0x1A

/** The maximum number of concurrent network privacy sessions allowed with the Network Privacy API.
This setting is read only on LBS system startup. Setting it via the LBS Admin API has no effect while LBS running. Setting will take effect during next LBS system startup.

The value 0 disallows any network privacy sessions

@see CPosNetworkPrivacy */
const TLbsAdminSetting KLbsSettingPrivacyMaxNetworkSessions    = 0x0000000004000000LL;	// Setting 0x1B

/** The connection timeout for sessions using the Network Privacy API.
This setting is read only on LBS system startup. Setting it via the LBS Admin API has no effect while LBS running. Setting will take effect during next LBS system startup.

Time is specified in microseconds.

@see CPosNetworkPrivacy */
const TLbsAdminSetting KLbsSettingPrivacyConnectionTimeout    = 0x0000000008000000LL;	// Setting 0x1C

/** This feature enables the withholding of the delivery of reference positions to a local application
that has opened the RPositioner by explicitly specifying a GPS module ID
 	
@see TSpecialFeature
@see RPositioner::NotifyPositionUpdate 
*/
const TLbsAdminSetting KLbsSpecialFeatureIgnoreAccuracy				= 0x0000000020000000LL;	// Setting 0x1E

// leaving a space for setting added on 9.3 to make integration easier

/** Privacy Timeout
 * Time in milliseconds that LBS will wait for a response from the privacy controller application / notifier
 * A value of zero means never timeout/wait forever.
 * @see KLbsSettingPrivacyAppTimeout
 * @see TLbsTimeIntervalMilliSeconds
 */
const TLbsAdminSetting KLbsSettingPrivacyAppTimeout					= 0x000000040000000LL;	// Setting 0x1F

/** 
This feature enables the LBS subsystem to check for 'futile' GPS updates between location requests when LBS is working in either Terminal Assisted Mode or Simultaneous Terminal Assisted Mode & Terminal Based Mode (also known as Hybrid Mode).

The LBS subsystem does not normally return a location update to the network if it occurs while there is no outstanding location request from the network. This is normally the correct behaviour.

In Hybrid Mode or Terminal Assisted Mode, a licensee's GPS module may decide that there is no way of getting a GPS fix and return a 'futile' update during the session. 
If the update happens while there is no outstanding network request then it is ignored and the futile update is not sent to the network.

Turning this setting on enables the LBS subsystem to look for futile updates between location requests and to return the updates to the network.

This feature only applies when the LBS subsystem is working in Hybrid Mode or Terminal Assisted Mode.
This feature may cause very serious side effects and must NOT be normally used.

This setting is read-only.
@see TSpecialFeature
*/
const TLbsAdminSetting KLbsSpecialFeatureIntermediateFutileUpdate	= 0x0000000080000000LL;	// Setting 0x20

/** Network induced locate configuration for Home Network.
@see CLbsAdmin::TExternalLocateService */
const TLbsAdminSetting KLbsSettingHomeNetworkInducedLocate			= 0x000000100000000LL;	// Setting 0x21

/** Network induced locate configuration when roaming
@see CLbsAdmin::TExternalLocateService */
const TLbsAdminSetting KLbsSettingRoamingNetworkInducedLocate		= 0x000000200000000LL;	// Setting 0x22

/** Licensee specific Set Clock Module Id.
A null value means that a default, internal implementation should be used.
@see TLbsSetClockModuleId */
const TLbsAdminSetting KLbsSettingSetClockModule 				= 0x0800000000000000LL;	// Setting 0x3C

/** Automatic clock adjustment interval in microseconds.
The value defines the time between consecutive attempts 
to automatically adjust the clock.
Relevant only if automatic clock adjustment is enabled.
@see CLbsAdmin::KLbsSettingClockAdjust
@see CLbsAdmin::KLbsSettingClockAdjustThreshold */
const TLbsAdminSetting KLbsSettingClockAdjustInterval			= 0x1000000000000000LL;	// Setting 0x3D

/** Automatic clock adjustment threshold in microseconds.
If the difference between the system UTC time and the
GPS based UTC time is less than the threshold then the
system clock is not being adjusted.
Relevant only if automatic clock adjustment is enabled.
@see CLbsAdmin::KLbsSettingClockAdjust 
@see TTimeIntervalMilliSeconds */
const TLbsAdminSetting KLbsSettingClockAdjustThreshold			= 0x2000000000000000LL;	// Setting 0x3E

/** Allow the system clock to be adjusted manually (synchronised with GPS on user demand).
@see CLbsAdmin::TClockAdjust */
const TLbsAdminSetting KLbsSettingAllowManualClockAdjust		= 0x4000000000000000LL; // Setting 0x3F

/** Synchronise the handset clock with the time obtained from GPS.
@see CLbsAdmin::TClockAdjust */
const TLbsAdminSetting KLbsSettingClockAdjust					= 0x8000000000000000LL;	// Setting 0x40


#endif //LBS_ADMIN_SETTINGS_H
