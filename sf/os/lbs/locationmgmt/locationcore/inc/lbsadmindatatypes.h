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
// Location Based Services Administration API Advanced Settings
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBS_ADMIN_DATA_TYPES_H
#define LBS_ADMIN_DATA_TYPES_H

#include <e32base.h>
#include <lbsadminclasstypes.h>

//-------------------------------------------------------------------------------
/** Network Location Protocol Module Id
@publishedPartner
@released
 */
typedef TUid TLbsProtocolModuleId;

/** Network Location Protocol Module Id const
@publishedPartner
@released
 */
const TLbsProtocolModuleId KLbsProtocolNullModuleId = {KNullUidValue};

/** Custom Set Clock Module Id
@publishedPartner
@released
 */
typedef TUid TLbsSetClockModuleId;

/** Custom Set Clock Module Id const
@publishedPartner
@released
 */
const TLbsSetClockModuleId KLbsSetClockNullModuleId = {KNullUidValue};

/** Quality profile ID for defining detailed positioning requirements
@publishedPartner
@released
 */
typedef TUint32 TLbsQualityProfileId;

/** const for Quality profile ID for defining detailed positioning requirements
@publishedPartner
@released
 */
const TLbsQualityProfileId KLbsNullQualityProfileId = 0;

/** const for Quality profile ID meaning no profile is in use
@publishedPartner
@released
 */
const TLbsQualityProfileId KLbsNoProfileInUseQualityProfileId = 0xFFFFFFFF;


/** Time in mili seconds
@publishedPartner
@released
*/
typedef TUint TLbsTimeIntervalMiliSeconds;

/** Time in milli seconds
@publishedPartner
@released
*/
typedef TUint TLbsTimeIntervalMilliSeconds;

/** const for the maximum acceptable delay for special feature KLbsSpecialFeatureMaximumRequestDelay
@publishedPartner
@released
@see KLbsSpecialFeatureMaximumRequestDelay
*/
const TUint KLbsMaximumRequestDelayMax	= 2000;

/** Default value for the maximum number of external locate requests setting
@publishedPartner
@released
*/
const TUint KLbsDefaultMaximumExternalLocateRequests = 1;

/** Default value for the time to wait before shutting down LBS when using the network privacy API setting
@publishedPartner
@released
*/
const TUint KLbsDefaultPrivacyShutdownDelay = 2000000;

/** Default value for the maximum number of concurrent network privacy sessions setting
@publishedPartner
@released
*/
const TUint KLbsDefaultPrivacyMaxNetworkSessions = 4;

/** Default value for the connection timeout for sessions using the network privacy API setting
@publishedPartner
@released
*/
const TUint KLbsDefaultPrivacyConnectionTimeout = 30000000;


//-------------------------------------------------------------------------------
/** define type TUint64 TLbsAdminSetting
@publishedPartner
@released
 */
typedef TUint64 TLbsAdminSetting;

/** define type TUint64 TLbsAdminSettingGroup
@publishedPartner
@released
 */
typedef TUint64 TLbsAdminSettingGroup;


//-------------------------------------------------------------------------------
/**
Currently LBS supports up to 2 Protocol Modules, however the data classes
are able to support up to 10 (future expansion).
@publishedPartner
@released
*/
const TInt KLbsAdminMaxProtocolModuleIds=10;

//-------------------------------------------------------------------------------
/**
Base class for advanced LBS administration settings.
An advanced setting is a setting containing more than one simple value.
It may be an array, couple of related values or more complex data structures.

@publishedPartner
@released
*/
class TLbsAdminInfoBase
	{
public:
	IMPORT_C TUint32 ClassType() const;
	IMPORT_C TUint ClassSize() const;

protected:
	IMPORT_C TLbsAdminInfoBase();

protected:
	/** The type of the derived class */
	TUint32 iClassType;
	/** The size of the derived class */
	TUint   iClassSize;
	};


//------------------------------------------------------------------------------------------------------------------	
/** The TLbsAdminProtocolModulesInfo class provides a data structure required to manage a
list of Protocol Modules to be used. It is a functional extension of the TLbsProtocolModuleId
type.

@see TLbsProtocolModuleId
@see KSettingHomeProtocolModule
@see KSettingRoamingProtocolModule

@publishedPartner
@released
*/
class TLbsAdminProtocolModulesInfo : public TLbsAdminInfoBase
	{
public:
	typedef TLbsProtocolModuleId TLbsProtocolModuleIdArray[KLbsAdminMaxProtocolModuleIds];
	
public:
	IMPORT_C TLbsAdminProtocolModulesInfo();
	
	IMPORT_C TLbsProtocolModuleId DefaultModuleId() const;
	
	IMPORT_C void GetModuleIds(TLbsProtocolModuleIdArray& aModuleIdArray, TInt& aCount) const;
	IMPORT_C TInt SetModuleIds(const TLbsProtocolModuleIdArray& aModuleIdArray, TInt aCount);

private:		
	TLbsProtocolModuleIdArray	iModuleIdArray;
	TInt						iModuleIdArrayCount;
	TUint32						iReserved[8];
	};


//------------------------------------------------------------------------------------------------------------------	
/** The TLbsAdminExtLocService is always used in conjunction with the TLbsAdminExtLocServicesInfo class
and defines external location request preferences for a single Protocol Module.

@see TLbsAdminExtLocServicesInfo

@publishedPartner
@released
*/
class TLbsAdminExtLocService
	{
public:
	/** Defined type for maintaining Binary Compatibility of the _TExternalLocateService enumeration. */
	typedef TUint32 TExternalLocateService;
	
	/** External Locate Service flags.
	It is an equivalent of the CLbsAdmin::TExternalLocateService enumeration.
	
	Note that the enumeration may be extended in the future by adding more values. 
	To maintain compatibility any unrecognized values must be handled as 
	EExternalLocateUnknown.*/
	enum _TExternalLocateService
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

public:
	IMPORT_C TLbsAdminExtLocService();
	
	IMPORT_C TExternalLocateService ModuleService() const;
	IMPORT_C void SetModuleService(const TExternalLocateService aModuleService);
	
	IMPORT_C TLbsProtocolModuleId ModuleId() const;
	IMPORT_C void SetModuleId(const TLbsProtocolModuleId aModuleId);

private:
	TExternalLocateService	iModuleService;
	TLbsProtocolModuleId	iModuleId;
	TUint32 				iReserved;
	};


//------------------------------------------------------------------------------------------------------------------	
/** The TLbsAdminExtLocServicesInfo provides a functional extension of the 
CLbaAdmin::TExternalLocateService type (default setting).

It allows enabling or disabling external location requests on per module basis. Module specific
settings supersede the default setting.

@see TLbsAdminExtLocService
@see CLbaAdmin::TExternalLocateService
@see KSettingHomeExternalLocate
@see KSettingRoamingExternalLocate

@publishedPartner
@released
*/
class TLbsAdminExtLocServicesInfo : public TLbsAdminInfoBase
	{
public:
	typedef TLbsAdminExtLocService TLbsAdminExtLocServiceArray[KLbsAdminMaxProtocolModuleIds];	
	
public:
	IMPORT_C TLbsAdminExtLocServicesInfo();

	IMPORT_C TLbsAdminExtLocService::TExternalLocateService DefaultService() const;
	IMPORT_C void SetDefaultService(const TLbsAdminExtLocService::TExternalLocateService& aModuleService);

	IMPORT_C TInt GetModuleService(const TLbsProtocolModuleId& aModuleId, TLbsAdminExtLocService::TExternalLocateService& aModuleService) const;
	
	IMPORT_C void GetModuleServices(TLbsAdminExtLocServiceArray& aModuleServices, TInt& aCount) const;
	IMPORT_C TInt SetModuleServices(const TLbsAdminExtLocServiceArray& aModuleServices, TInt aCount);

private:
	TLbsAdminExtLocServiceArray iServiceArray;
	TInt						iServiceArrayCount;
	TUint32						iReserved[8];
	};


//------------------------------------------------------------------------------------------------------------------	
/** The TLbsAdminProtocolModuleLoadingInfo class allows to define Protocol Module Loading strategy.
Although at this point it contains just one simple value, it has been encapsulated in the T class
to allow for future extensions.

@see KLbsProtocolModuleLoading

@publishedPartner
@released
*/
class TLbsAdminProtocolModuleLoadingInfo : public TLbsAdminInfoBase
	{
public:
	typedef TUint32 TProtocolModuleLoading;
	
	/** the enum may be extended in the future */
	enum _TProtocolModuleLoading
		{
		/** Unknown. May be used to initialise a variable before getting the value of the setting */
		EProtocolModuleLoadingUnknown	= 0x00,
				
		/** Load all roaming OR home Protocol Modules at LBS start-up. The Protocol Modules to be loaded depend
		on the roaming status*/
		EProtocolModuleLoadingByStatus		= 0x01,

		/** Load all roaming and non-roaming Protocol Modules at LBS start-up. */
		EProtocolModuleLoadingAll		= 0x02,
		
		/** Dynamically re-load modules when roaming status changes.
		Note that Protocol Modules intended to be used for emergency calls support should
		be present in both, roaming and non-roaming list of modules.
		@see KLbsSettingHomeProtocolModule
		@see KLbsSettingRoamingProtocolModule
		
		The setting is currently not supported*/
		EProtocolModuleLoadingDynamic	= 0x03
		};

public:
	IMPORT_C TLbsAdminProtocolModuleLoadingInfo();

	IMPORT_C TProtocolModuleLoading LoadingStrategy() const;
	IMPORT_C void SetLoadingStrategy(const TProtocolModuleLoading aLoadingStrategy);
	
private:
	TProtocolModuleLoading	iLoadingStrategy;
	TUint32					iReserved[8];
	};


#endif //LBS_ADMIN_DATA_TYPES_H
