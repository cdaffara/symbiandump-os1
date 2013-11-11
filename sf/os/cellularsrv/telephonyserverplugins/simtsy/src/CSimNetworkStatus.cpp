// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the functionality required to provide clients with
// network status, registration and NITZ information.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimNetworkStatusTraces.h"
#endif

#include <e32property.h>
#include <testconfigfileparser.h>
#include <sacls.h>
#include "CSimNetworkStatus.h"
#include "CSimPhone.h"
#include "CSimPubSubChange.h"
#include "CSimTsyMode.h"
#include "etelmmerr.h"
#include "CSimPubSub.h"
#include "simtsyglobalproperties.h"

const TInt KSettingListGranularity=5;	// < The granularity used for parameter list arrays.

// Timer values
const TInt KNetStatTimeoutDefault=1;

CSimNetworkStatus* CSimNetworkStatus::NewL(CSimPhone* aPhone)
/**
 * Standard two-phase constructor.
 * @param aPhone				The parent phone object.
 * @return CSimNetworkStatus	The new network status class.
 */
	{
	CSimNetworkStatus* self=new(ELeave) CSimNetworkStatus(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimNetworkStatus::CSimNetworkStatus(CSimPhone* aPhone)
		: iPhone(aPhone)
/**
 * Trivial first phase construction.
 * @param aPhone				The parent phone object.
 */
	{
	iCurrNetChangeV1NotificationPending = EFalse;
	iCurrNetChangeV2NotificationPending = EFalse;
	iCurrNetChangeEtelIsvNotificationPending = EFalse;
	iCurrNetChangeNoLocV1NotificationPending = EFalse;
	iCurrNetChangeNoLocV2NotificationPending = EFalse;
	iCurrNetChangeV5NotificationPending = EFalse;
	iCurrNetChangeV8NotificationPending = EFalse;
	iCurrNetChangeNoLocV5NotificationPending = EFalse;
	iCurrNetChangeNoLocV8NotificationPending = EFalse;
	iCurrentNetworkChangeLocationAreaNotificationPending = EFalse;

	iRoamStatGetTimeout = KNetStatTimeoutDefault;

	iRoamStatTimer = NULL;
	iRoamStatGetClients = NULL;
	iRoamStatNotifyClients = NULL;
	}

void CSimNetworkStatus::ConstructL()
/**
 * Second phase construction.  Create instances of the necessary heap-based
 * objects and read in the network, Network Registration and NITZ information
 * from the configuration file.
 * If any network registration status tags have been read, the initial values
 * will be loaded and the network registration timer started.
 * Similarly, if any NITZ tags have been read, the initial NITZ values will be loaded
 * and the NITZ timer started.
 *
 * Entries in the configuration file will take the following formats:
 *
 * "NetworkInfo= <CountryCode>, <NetworkId>, <DisplayTag>, <NetworkShortName>, <NetworkLongName> , <NetworkAccess> "
 * A number of these tags may be included to create a list of Network Information
 * that can be used via the RegStatus tag during the test.
 * "LocationArea= <AreaKnown>, <LocationAreaCode>, <CellId>"
 * that can be used via the RegStatus tag during the test.
 *
 * "RegStatus= <Duration>, <RegistrationStatus>, <NetworkIndex>"
 * The RegistrationStatus is taken from the RMoiblePhone::TMobilePhoneRegistrationStatus
 * enumeration.  The NetworkIndex is a reference to the NetworkInfo list.
 * A number of these tags may be included to create a Network Registration Profile.
 *
 * "NitzInfo= <Duration>, <Year>, <Month>, <Day>, <Hour>, <Minute>, <Second>, <Microsecond>,
 *            <Timezone>, <DayTimeSavings>"
 * A number of these tags may be included to create a NITZ Profile.
 * 
 * "CellInfo= <Duration>, <mode>, <CellId>, <timingAdvance>, <cellInfoStatus>"
 * A number of these tags may be included to create a Cell Information Profile.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_1, "Starting to parse Network Status config parameters...");
	iRegStatTimer=CSimTimer::NewL(iPhone);
	iNitzTimer=CSimTimer::NewL(iPhone);
	iRadioModeTimer=CSimTimer::NewL(iPhone);
	iCellInfoTimer=CSimTimer::NewL(iPhone);
	
	iRegStatusInfo=new(ELeave) CArrayFixFlat<TRegStatus>(KSettingListGranularity);
	iNetworkInfo=new(ELeave) CArrayFixFlat<RMobilePhone::TMobilePhoneNetworkInfoV2>(KSettingListGranularity);
	iNetworkInfoV8=new(ELeave) CArrayFixFlat<RMobilePhone::TMobilePhoneNetworkInfoV8>(KSettingListGranularity);
	iNitzInfo=new(ELeave) CArrayFixFlat<TNitzInfoAndDuration>(KSettingListGranularity);
	iRadioModeInfo=new(ELeave) CArrayFixFlat<TRadioMode>(KSettingListGranularity);
	iCellInfo = new (ELeave)CArrayFixFlat<TCellInfoAndDuration>(KSettingListGranularity);
	iLocationArea  = new (ELeave)CArrayFixFlat<RMobilePhone::TMobilePhoneLocationAreaV1>(KSettingListGranularity);
	
	TInt count;
	count=CfgFile()->ItemCount(KNetworkInfo);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KNetworkInfo,i);
		if(!item)
			break;

		TPtrC8 countryCode,networkIdentify,displayTag,longName,shortName;
		TInt status;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,countryCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNTRYCODE returned %d (element no. %d) from tag %s.",ret,0,KNetworkInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,networkIdentify);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element NETWORKIDENTIFY returned %d (element no. %d) from tag %S.",ret,1,KNetworkInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,displayTag);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element DISPLAYTAG returned %d (element no. %d) from tag %s.",ret,2,KNetworkInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,shortName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element SHORTNAME returned %d (element no. %d) from tag %s.",ret,3,KNetworkInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,longName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element LONGNAME returned %d (element no. %d) from tag %s.",ret,4,KNetworkInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,status);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element STATUS returned %d (element no. %d) from tag %s.",ret,5,KNetworkInfo);
			continue;
			}
		RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfo;
		RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;

		TInt mode;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,mode);
		if(ret!=KErrNone)
			{
			// It's ok, this element is optional. Assume that the
			// config file is old, and default mode to GSM
			mode = RMobilePhone::ENetworkModeGsm;
			}
		networkInfo.iMode=RMobilePhone::TMobilePhoneNetworkMode(mode);

		networkInfo.iStatus=RMobilePhone::TMobilePhoneNetworkStatus(status);
		networkInfo.iBandInfo=RMobilePhone::EBandUnknown;
		networkInfo.iCountryCode.Copy(countryCode);
		networkInfo.iAnalogSID.SetLength(0);
		networkInfo.iNetworkId.Copy(networkIdentify);
		networkInfo.iDisplayTag.Copy(displayTag);		
				
		TInt maxLength = networkInfo.iShortName.MaxLength();
		if (shortName.Length() > maxLength)
			{
			networkInfo.iShortName.Copy(shortName.Left(maxLength));
			}
		else 
			{
			networkInfo.iShortName.Copy(shortName);	
			}
		
		maxLength = networkInfo.iLongName.MaxLength();
		if (longName.Length() > maxLength)
			{
			networkInfo.iLongName.Copy(longName.Left(maxLength));
			}
		else
			{
			networkInfo.iLongName.Copy(longName);	
			}
		
		networkInfo.iAccess = RMobilePhone::ENetworkAccessGsm;
		networkInfoV8.iHsdpaAvailableIndicator=EFalse;
		networkInfoV8.iEgprsAvailableIndicator=EFalse;
		networkInfoV8.iHsupaAvailableIndicator=ETrue;
		iNetworkInfo->AppendL(networkInfo);
		iNetworkInfoV8->AppendL(networkInfoV8);
		}

		     
// Read in the Registration status profile.
	count=CfgFile()->ItemCount(KRegStatus);
	item=NULL;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KRegStatus,i);
		if(!item)
			break;

		TInt duration,regStat,networkIndex;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KRegStatus);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,regStat);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element REGSTAT returned %d (element no. %d) from tag %s.",ret,1,KRegStatus);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,networkIndex);
		if(ret!=KErrNone)
			networkIndex=KErrNotFound;		// Network Index is optional.

		TRegStatus regStatus;
		regStatus.iDuration=duration;
		regStatus.iRegStatus=(RMobilePhone::TMobilePhoneRegistrationStatus)regStat;
		regStatus.iNetworkInfoIndex=networkIndex;
		iRegStatusInfo->AppendL(regStatus);
		}
		
		count=CfgFile()->ItemCount(KRadioMode);
		item=NULL;
		for(i=0;i<count;i++)
			{
			item=CfgFile()->Item(KRadioMode,i);
			if(!item)
				break;

			TInt duration,radioStatus;
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KRadioMode);
				continue;
				}
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,radioStatus);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element RADIOSTATUS returned %d (element no. %d) from tag %s.",ret,1,KRadioMode);
				continue;
				}
			

			TRadioMode radioMode;
			radioMode.iDuration=duration;
			radioMode.iRadioOn=(TBool)radioStatus;
			iRadioModeInfo->AppendL(radioMode);
			}

// Read the NITZ information
	count=CfgFile()->ItemCount(KNitzInfo);
	item=NULL;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KNitzInfo,i);
		if(!item)
			break;

		TInt duration,year,month,day,hour,min,sec,microsec,tz,dst;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,year);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element YEAR returned %d (element no. %d) from tag %s.",ret,1,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,month);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element MONTH returned %d (element no. %d) from tag %s.",ret,2,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,day);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_15, "WARNING - CONFIGURATION FILE PARSING - Reading element DAY returned %d (element no. %d) from tag %s.",ret,3,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,hour);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_16, "WARNING - CONFIGURATION FILE PARSING - Reading element HOUR returned %d (element no. %d) from tag %s.",ret,4,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,min);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_17, "WARNING - CONFIGURATION FILE PARSING - Reading element MIN returned %d (element no. %d) from tag %s.",ret,5,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,sec);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_18, "WARNING - CONFIGURATION FILE PARSING - Reading element SEC returned %d (element no. %d) from tag %s.",ret,6,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,microsec);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_19, "WARNING - CONFIGURATION FILE PARSING - Reading element MICROSEC returned %d (element no. %d) from tag %s.",ret,7,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,tz);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_20, "WARNING - CONFIGURATION FILE PARSING - Reading element TZ returned %d (element no. %d) from tag %s.",ret,8,KNitzInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,dst);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_21, "WARNING - CONFIGURATION FILE PARSING - Reading element DST returned %d (element no. %d) from tag %s.",ret,9,KNitzInfo);
			continue;
			}

		RMobilePhone::TMobilePhoneNITZ nitzInfo(year,(TMonth)month,day,hour,min,sec,microsec);
		nitzInfo.iTimeZone=tz;
		nitzInfo.iDST=dst;
		nitzInfo.iNitzFieldsUsed=	(RMobilePhone::KCapsTimeAvailable
								    |RMobilePhone::KCapsTimezoneAvailable
									|RMobilePhone::KCapsDSTAvailable);

		TNitzInfoAndDuration nitzInfoAndDuration;
		nitzInfoAndDuration.iNitz=nitzInfo;
		nitzInfoAndDuration.iDuration=duration;
		iNitzInfo->AppendL(nitzInfoAndDuration);
		}

	// Read the Cell Info information
		count=CfgFile()->ItemCount(KCellInfo);
		item=NULL;
		for(i=0;i<count;i++)
			{
			item=CfgFile()->Item(KCellInfo,i);
			if(!item)
				break;

			TInt duration,mode,cellId,timingAdvance,cellInfoStatus;
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_22, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KCellInfo);
				continue;
				}
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,mode);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_23, "WARNING - CONFIGURATION FILE PARSING - Reading element MODE returned %d (element no. %d) from tag %s.",ret,1,KCellInfo);
				continue;
				}
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,cellId);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_24, "WARNING - CONFIGURATION FILE PARSING - Reading element CELLID returned %d (element no. %d) from tag %s.",ret,2,KCellInfo);
				continue;
				}
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,timingAdvance);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_25, "WARNING - CONFIGURATION FILE PARSING - Reading element TIMINGADVANCE returned %d (element no. %d) from tag %s.",ret,3,KCellInfo);
				continue;
				}
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,cellInfoStatus);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_26, "WARNING - CONFIGURATION FILE PARSING - Reading element CELLINFOSTATUS returned %d (element no. %d) from tag %s.",ret,3,KCellInfo);
				continue;
				}
			
			TCellInfoAndDuration cellInfoAndDuration;
			cellInfoAndDuration.iInfo.iMode = static_cast<RMobilePhone::TCellMode>(mode);
			cellInfoAndDuration.iInfo.iCellId = static_cast<TUint>(cellId);
			cellInfoAndDuration.iInfo.iTimingAdvance = static_cast<RMobilePhone::TGsmTimingAdvance>(timingAdvance);
			cellInfoAndDuration.iInfo.iStatus = static_cast<RMobilePhone::TCellInfoStatus>(cellInfoStatus);

			cellInfoAndDuration.iDuration=duration;
			iCellInfo->AppendL(cellInfoAndDuration);
			}

		// Read the Location Area information
			count=CfgFile()->ItemCount(KLocationArea);
			item=NULL;
			for(i=0;i<count;i++)
				{
				item=CfgFile()->Item(KLocationArea,i);
				if(!item)
					break;

				TBool areaKnown;
				TInt locationAreaCode, cellId;
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,areaKnown);
				if(ret!=KErrNone)
					{
					ret = EFalse;
					}
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,locationAreaCode);
				if(ret!=KErrNone)
					{
					locationAreaCode = 0;
					}
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,cellId);
				if(ret!=KErrNone)
					{
					cellId = 0;
					}

				RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
				locationArea.iAreaKnown = areaKnown;
				locationArea.iLocationAreaCode = locationAreaCode;
				locationArea.iCellId= cellId;
				iLocationArea->AppendL(locationArea);
				}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_27, "...Finished parsing Network Status config parameters...");

// If present load the first registration status settings and start the registration
// status timer.
	if(iRegStatusInfo->Count()!=0)
		{
		iRegStatIndex=0;
		iCurrentRegStatus=iRegStatusInfo->At(0).iRegStatus;
		iCurrentNetworkIndex=iRegStatusInfo->At(0).iNetworkInfoIndex;
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_28, "Starting ETimerIdNtwkStatRegStat for duration:%d", iRegStatusInfo->At(0).iDuration);
		iRegStatTimer->Start(iRegStatusInfo->At(0).iDuration,this,ETimerIdNtwkStatRegStat);
		}

// If present load the first NITZ settings and start the NITZ timer.
	if(iNitzInfo->Count()!=0)
		{
		iNitzIndex=0;
		iCurrentNitzInfo=iNitzInfo->At(0).iNitz;
		iNitzTimer->Start(iNitzInfo->At(0).iDuration,this,ETimerIdNtwkStatNitz);
		}
		
// If present load the first Cell Info settings and start the cell info timer.
	if(iCellInfo->Count()!=0)
		{
		iCellInfoIndex=0;
		iCurrentCellInfo=iCellInfo->At(0).iInfo;
		iCellInfoTimer->Start(iCellInfo->At(0).iDuration,this,ETimerIdCellInfo);
		}
		
	if(iRadioModeInfo->Count()!=0)
		{
		iRadioModeIndex=0;
		iRadioMode.iRadioOn=iRadioModeInfo->At(0).iRadioOn;
		if(iRadioMode.iRadioOn==EFalse)
			{
			RProperty::Set(KUidSystemCategory, KUidPhonePwrValue, ESAPhoneOff);
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_29, "Radio Mode = OFF (from config)");
			}
		else
			{
			RProperty::Set(KUidSystemCategory, KUidPhonePwrValue, ESAPhoneOn);			
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_30, "Radio Mode = ON (from config)");
			}
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_31, "Radio Mode duration = %d (from config)", iRadioModeInfo->At(0).iDuration);

 		iRadioModeTimer->Start(iRadioModeInfo->At(0).iDuration,this,ETimerIdNtwkStatRadioMode);
		}
	else
		{
		//default to on
		RProperty::Set(KUidSystemCategory, KUidPhonePwrValue, ESAPhoneOn);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_32, "Radio Mode = ON (no config)");
		}

//If present read in Service Provider settings
	TPtrC8 spname, plmnfield;
	TInt displayreq;
	item=CfgFile()->Item(KServiceProviderName);
	if (item)
		{
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,displayreq);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_33, "WARNING - CONFIGURATION FILE PARSING - Reading element DISPLAYREQ returned %d (element no. %d) from tag %s.",ret,0,KServiceProviderName);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,spname);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_34, "WARNING - CONFIGURATION FILE PARSING - Reading element SPNAME returned %d (element no. %d) from tag %s.",ret,1,KServiceProviderName);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,plmnfield);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_CONSTRUCTL_35, "WARNING - CONFIGURATION FILE PARSING - Reading element PLMNFIELD returned %d (element no. %d) from tag %s.",ret,2,KServiceProviderName);
			}
		iServiceProvider.iDisplayReq = static_cast<RMobilePhone::TDisplayRequirements>(displayreq);
		
		TInt maxLength = iServiceProvider.iSPName.MaxLength();
		if (spname.Length() > maxLength)
			{
			iServiceProvider.iSPName.Copy(spname.Left(maxLength));
			}
		else
			{
			iServiceProvider.iSPName.Copy(spname);	
			}
			
		maxLength = iServiceProvider.iPLMNField.MaxLength();
		if (plmnfield.Length() > maxLength)
			{
			iServiceProvider.iPLMNField.Copy(plmnfield.Left(maxLength));
			}
		else
			{
			iServiceProvider.iPLMNField.Copy(plmnfield);		
			}
		}
	else
		{
		iServiceProvider.iDisplayReq = KDisplayReqDefault;
		iServiceProvider.iSPName.Copy(KSPNameDefault);
		iServiceProvider.iPLMNField.Copy(KPLMNFieldDefault);		
		}

	iRegStatChange=CSimPubSubChange::NewL(this, CSimPubSub::TPubSubProperty(KUidSimtsyCategoryLocal, KSimtsyRegStatChange, KSimtsyRegStatChangeType));
	//also listen on new P&S uids
	iPSRegStatChange=CSimPubSubChange::NewL(this, CSimPubSub::TPubSubProperty(KUidPSSimTsyCategory, KPSSimTsyRegStatChange, KPSSimTsyRegStatChangeKeyType));
		
	}

void CSimNetworkStatus::FindRoamStatSettings()
/**
 * Get Roaming Status Configuration
 */
	{
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	//If present read in Service Provider settings
	TInt getTimer;
	item=CfgFile()->Item(KRoamStatTimers);
	if (item)
		{
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,getTimer);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_FINDROAMSTATSETTINGS_1, "WARNING: CSimNetworkStatus::FindRoamStatSettings found ERROR with Config");
			}
		iRoamStatGetTimeout = getTimer;
		}
	}

void CSimNetworkStatus::SetNetworkMode()
/**
 * Initialize iPhone->NetworkMode().
 */
	{
	if(iCurrentNetworkIndex != KErrNotFound)
		{
	    if(iNetworkInfo->Count()>iCurrentNetworkIndex)
			{
			iPhone->UpdateNetworkMode(iNetworkInfo->At(iCurrentNetworkIndex).iMode);
			}
		}
	}
	
CSimNetworkStatus::~CSimNetworkStatus()
/**
 * Standard destructor.  Destroy the heap-based object owned by this object.
 */
	{
	delete iCellInfoTimer;
	delete iNitzTimer;
	delete iRegStatTimer;
	delete iRegStatChange;
	delete iPSRegStatChange;
	delete iRadioModeTimer;
	if (iCellInfo)
		{
		iCellInfo->Delete(0,iCellInfo->Count());
		delete iCellInfo;
		}
	if (iNitzInfo)
		{
		iNitzInfo->Delete(0,iNitzInfo->Count());
		delete iNitzInfo;//todo check ptr
		}
	if (iRegStatusInfo)
		{
		iRegStatusInfo->Delete(0,iRegStatusInfo->Count());
		delete iRegStatusInfo;//todo check ptr
		}
	if (iNetworkInfo)
		{
		iNetworkInfo->Delete(0,iNetworkInfo->Count());
		delete iNetworkInfo;//todo check ptr
		}
	if (iNetworkInfoV8)
		{
		iNetworkInfoV8->Delete(0,iNetworkInfoV8->Count());
		delete iNetworkInfoV8;//todo check ptr
		}
	if (iRadioModeInfo)
		{
		iRadioModeInfo->Delete(0,iRadioModeInfo->Count());
		delete iRadioModeInfo;//todo check ptr
		}
	if(iLocationArea)
		{
		iLocationArea->Delete(0,iLocationArea->Count());
		delete iLocationArea;
		}
		
	delete iRoamStatTimer;
	if(iRoamStatGetClients)
		{
		TInt i;
		TRoamStatClient client;
		for(i=0;i<iRoamStatGetClients->Count();i++)
			{
			client = iRoamStatGetClients->At(i);
			iPhone->ReqCompleted(client.iReqHandle, KErrCancel);
			}	
		iRoamStatGetClients->Reset();
		delete iRoamStatGetClients;
		}
	if(iRoamStatNotifyClients)
		{
		TInt i;
		TRoamStatClient client;
		for(i=0;i<iRoamStatNotifyClients->Count();i++)
			{
			client = iRoamStatNotifyClients->At(i);
			iPhone->ReqCompleted(client.iReqHandle, KErrCancel);
			}	
		iRoamStatNotifyClients->Reset();
		delete iRoamStatNotifyClients;
		}
	}

TInt CSimNetworkStatus::GetNetworkCaps(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Retrieve the Network Capability information.  Network capabilities for this test
 * are deduced from the tags, and therefore loaded information, for this test.
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						capability data to be returned.
 * @return TInt			Standard error value.
 */
{
	TPckg<TUint32>* networkCapsPckg=(TPckg<TUint32>*)aPckg1;
	TUint32& networkCaps=(*networkCapsPckg)();

	if(iRegStatusInfo->Count()==0)
		networkCaps=RMobilePhone::KCapsGetCurrentMode;
	else
		networkCaps=RMobilePhone::KCapsGetRegistrationStatus |
					RMobilePhone::KCapsNotifyRegistrationStatus | 
					RMobilePhone::KCapsGetCurrentMode |
					RMobilePhone::KCapsGetCurrentNetwork | 
					RMobilePhone::KCapsNotifyCurrentNetwork |
					RMobilePhone::KCapsGetHomeNetwork;

	if(iNetworkInfo->Count()>=1)
		networkCaps|=RMobilePhone::KCapsGetHomeNetwork;

	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimNetworkStatus::GetHomeNetwork(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Retrieve information about the Home Network.  The first NetworkInfo tag information
 * is taken to be the "home" network.  If this information is not present, the
 * request is completed with KErrNotSupported.
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						Home Network information to be returned.
 * @return TInt			Standard error value.+
 */
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* homeNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aPckg1;
	RMobilePhone::TMultimodeType& packagetype = (*homeNetworkPckg)();
	RMobilePhone::TMobilePhoneNetworkInfoV1& homeNetwork=(*homeNetworkPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(homeNetwork);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	if(iNetworkInfo->Count()==0) 
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	PopulateNetworkInfoV1(0,homeNetwork);
	
	if(packagetype.ExtensionId() == KETelExtMultimodeV1 || packagetype.ExtensionId() == KETelExtMultimodeV2 || \
	   packagetype.ExtensionId() == KEtelExtMultimodeV5 || packagetype.ExtensionId() == KEtelExtMultimodeV8 )
		{	
		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* homeNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV1& homeNetwork=(*homeNetworkPckg)();		

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(homeNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
	
		PopulateNetworkInfoV1(0,homeNetwork);
		
		//if V2 package then set its paramters as well
		if(packagetype.ExtensionId() == KETelExtMultimodeV2 || packagetype.ExtensionId() == KEtelExtMultimodeV5 || \
		   packagetype.ExtensionId() == KEtelExtMultimodeV8)
			{
			RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* homeNetworkPckgV2=(RMobilePhone::TMobilePhoneNetworkInfoV2Pckg*)aPckg1;
			RMobilePhone::TMobilePhoneNetworkInfoV2& homeNetworkV2=(*homeNetworkPckgV2)();

			// Check that the data structure is supported by the simulated TSY version
			TInt err = iPhone->CheckSimTsyVersion(homeNetworkV2);
			if(err != KErrNone)
				{
				iPhone->ReqCompleted(aReqHandle, err);
				return KErrNone;
				}

			homeNetworkV2.iAccess = iNetworkInfo->At(iCurrentNetworkIndex).iAccess;
		
			if(packagetype.ExtensionId() == KEtelExtMultimodeV5 || packagetype.ExtensionId() == KEtelExtMultimodeV8)
				{
				RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* homeNetworkPckgV5=(RMobilePhone::TMobilePhoneNetworkInfoV5Pckg*)aPckg1;
				RMobilePhone::TMobilePhoneNetworkInfoV5& homeNetworkV5=(*homeNetworkPckgV5)();
	
				// Check that the data structure is supported by the simulated TSY version
				TInt err = iPhone->CheckSimTsyVersion(homeNetworkV5);
				if(err != KErrNone)
					{
					iPhone->ReqCompleted(aReqHandle, err);
					return KErrNone;
					}

				homeNetworkV5.iHsdpaAvailableIndicator = iNetworkInfoV8->At(0).iHsdpaAvailableIndicator;
				homeNetworkV5.iEgprsAvailableIndicator = iNetworkInfoV8->At(0).iEgprsAvailableIndicator;
			
		 		if(packagetype.ExtensionId() == KEtelExtMultimodeV8)
					{
					RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* homeNetworkPckgV8=(RMobilePhone::TMobilePhoneNetworkInfoV8Pckg*)aPckg1;
					RMobilePhone::TMobilePhoneNetworkInfoV8& homeNetworkV8=(*homeNetworkPckgV8)();

					// Check that the data structure is supported by the simulated TSY version
					TInt err = iPhone->CheckSimTsyVersion(homeNetworkV8);
					if(err != KErrNone)
						{
						iPhone->ReqCompleted(aReqHandle, err);
						return KErrNone;
						}

					homeNetworkV8.iHsupaAvailableIndicator = iNetworkInfoV8->At(0).iHsupaAvailableIndicator;
					}
				}
			}
		}
	else
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;		
		}
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CSimNetworkStatus::GetCurrentNetwork(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2)
/**
 * Retrieve information about the Current Network.  The current network is given by the
 * iCurrentNetworkIndex member variable.  It will be updated from RegStat tags in the
 * configuration file.  If there is no network information currently available (perhaps
 * because the SIM TSY is simulating a network search) KErrNotFound will be
 * returned.  If no registration status profile information is contained in the
 * configuration file KErrNotSupported will be returned.
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						Current Network information to be returned.
 * @return TInt			Standard error value.
 */
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aPckg1;
	RMobilePhone::TMultimodeType& packagetype = (*currentNetworkPckg)();
	
	if(aPckg2)
		{
		TPckg<RMobilePhone::TMobilePhoneLocationAreaV1>* areaPckg=(TPckg<RMobilePhone::TMobilePhoneLocationAreaV1>*)aPckg2;
		RMobilePhone::TMobilePhoneLocationAreaV1& area=(*areaPckg)();	

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(area);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
		if((iCurrentNetworkIndex >=0) && (iCurrentNetworkIndex <iLocationArea->Count()))
			{
			area.iAreaKnown = iLocationArea->At(iCurrentNetworkIndex).iAreaKnown;
			area.iLocationAreaCode = iLocationArea->At(iCurrentNetworkIndex).iLocationAreaCode;
			area.iCellId = iLocationArea->At(iCurrentNetworkIndex).iCellId;
			}
		else
			{
			area.iAreaKnown=EFalse;		// Area information not currently supported.
			}
		}
	
	
	if(iRegStatusInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	if(iCurrentNetworkIndex==KErrNotFound)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotFound);
		return KErrNone;
		}
	
	if(packagetype.ExtensionId() == KETelExtMultimodeV1 || packagetype.ExtensionId() == KETelExtMultimodeV2 || \
	   	packagetype.ExtensionId() == KEtelExtMultimodeV5 || packagetype.ExtensionId() == KEtelExtMultimodeV8 )
		{	
		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV1& currentNetwork=(*currentNetworkPckg)();		

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
	
		PopulateNetworkInfoV1(iCurrentNetworkIndex,currentNetwork);
	//if V2 package then set its paramters as well
		if(currentNetwork.ExtensionId() == KETelExtMultimodeV2 || packagetype.ExtensionId() == KEtelExtMultimodeV5 || \
		  	packagetype.ExtensionId() == KEtelExtMultimodeV8)
			{
			RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* currentNetworkPckgV2=(RMobilePhone::TMobilePhoneNetworkInfoV2Pckg*)aPckg1;
			RMobilePhone::TMobilePhoneNetworkInfoV2& currentNetworkV2=(*currentNetworkPckgV2)();

			// Check that the data structure is supported by the simulated TSY version
			TInt err = iPhone->CheckSimTsyVersion(currentNetworkV2);
			if(err != KErrNone)
				{
				iPhone->ReqCompleted(aReqHandle, err);
				return KErrNone;
				}

			currentNetworkV2.iAccess = iNetworkInfo->At(iCurrentNetworkIndex).iAccess;
		
			if(packagetype.ExtensionId() == KEtelExtMultimodeV5 || packagetype.ExtensionId() == KEtelExtMultimodeV8)
				{
				RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* currentNetworkPckgV5=(RMobilePhone::TMobilePhoneNetworkInfoV5Pckg*)aPckg1;
				RMobilePhone::TMobilePhoneNetworkInfoV5& currentNetworkV5=(*currentNetworkPckgV5)();

				// Check that the data structure is supported by the simulated TSY version
				TInt err = iPhone->CheckSimTsyVersion(currentNetworkV5);
				if(err != KErrNone)
					{
					iPhone->ReqCompleted(aReqHandle, err);
					return KErrNone;
					}

				currentNetworkV5.iHsdpaAvailableIndicator = iNetworkInfoV8->At(0).iHsdpaAvailableIndicator;
				currentNetworkV5.iEgprsAvailableIndicator = iNetworkInfoV8->At(0).iEgprsAvailableIndicator;
			
				if(packagetype.ExtensionId() == KEtelExtMultimodeV8)
					{
					RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* currentNetworkPckgV8=(RMobilePhone::TMobilePhoneNetworkInfoV8Pckg*)aPckg1;
					RMobilePhone::TMobilePhoneNetworkInfoV8& currentNetworkV8=(*currentNetworkPckgV8)();

					// Check that the data structure is supported by the simulated TSY version
					TInt err = iPhone->CheckSimTsyVersion(currentNetworkV8);
					if(err != KErrNone)
						{
						iPhone->ReqCompleted(aReqHandle, err);
						return KErrNone;
						}

					currentNetworkV8.iHsupaAvailableIndicator = iNetworkInfoV8->At(0).iHsupaAvailableIndicator;
					}
				}
			}
		}
		
	else if(packagetype.ExtensionId() == KETelExt3rdPartyV1)
		{
		RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg* currentNetworkPckg=(RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg*)aPckg1;
		RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1& currentNetwork=(*currentNetworkPckg)();			

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}

		PopulateEtel3rdPartyNetworkInfoV1(iCurrentNetworkIndex, currentNetwork);
		}
	else
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;		
		}
	
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimNetworkStatus::GetCurrentNetworkNoLocation(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Retrieve information about the Current Network excluding loation information.  The current network is given by the
 * iCurrentNetworkIndex member variable.  It will be updated from RegStat tags in the
 * configuration file.  If there is no network information currently available (perhaps
 * because the SIM TSY is simulating a network search) KErrNotFound will be
 * returned.  If no registration status profile information is contained in the
 * configuration file KErrNotSupported will be returned.
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						Current Network information to be returned. This is either a packaged TMobilePhoneNetworkInfoV1 or TMobilePhoneNetworkInfoV2
 * @return TInt			Standard error value.
 */
	{	
	return GetCurrentNetwork(aReqHandle, aPckg1, NULL);
	}

TInt CSimNetworkStatus::GetNetworkRegistrationStatus(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Retrieve information about the Current Network Registration Status.  This is given by
 * the iCurrentRegStatus member variable.  It will be updated from RegStat tags in the
 * configuration file.  If no registration status profile information is contained in the
 * configuration file KErrNotSupported will be returned.
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						Current Network Registration Status to be returned.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneRegistrationStatus>* regStatusPckg=(TPckg<RMobilePhone::TMobilePhoneRegistrationStatus>*)aPckg;
	RMobilePhone::TMobilePhoneRegistrationStatus& regStatus=(*regStatusPckg)();

	if(iRegStatusInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	regStatus=(RMobilePhone::TMobilePhoneRegistrationStatus)iCurrentRegStatus;
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimNetworkStatus::NotifyNetworkRegistrationStatusChange(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Register a client's interest in being notified when the Network Registration Status
 * changes.
 * This function records the request's parameters and awaits a change in network registration
 * status before completing.
 *
 * @param aPckg			The first parameter package.  This is populated with the network registration
 *						status.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneRegistrationStatus>* regStatusPckg=(TPckg<RMobilePhone::TMobilePhoneRegistrationStatus>*)aPckg;
	RMobilePhone::TMobilePhoneRegistrationStatus& regStatus=(*regStatusPckg)();

	if(iRegStatusInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iRegStatChangeNotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iRegStatChangeNotificationPending=ETrue;
	iRegStatChangeNotificationReqHandle=aReqHandle;
	iRegStatChangeNotificationRegStatus=&regStatus;
	return KErrNone;
	}

void CSimNetworkStatus::NotifyNetworkRegistrationStatusChangeCancel()
/**
 * Cancel a previous request to be notified of a change in network registration status.
 */
	{
	if(iRegStatChangeNotificationPending)
		{
		iRegStatChangeNotificationPending=EFalse;
		iPhone->ReqCompleted(iRegStatChangeNotificationReqHandle,KErrCancel);
		}
	}

TInt CSimNetworkStatus::NotifyCurrentNetworkChange(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2)
/**
 * Register a client's interest in being notified when the current network changes.
 * This function records the request's parameters and awaits a change in current network
 * before completing.
 *
 * @param aPckg1		The first parameter package.  This is populated with the current network 
 *						information when the request completes.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneLocationAreaV1>* areaPckg=(TPckg<RMobilePhone::TMobilePhoneLocationAreaV1>*)aPckg2;
	RMobilePhone::TMobilePhoneLocationAreaV1& area=(*areaPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(area);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	TPckg<RMobilePhone::TMultimodeType>* paramPckg=(TPckg<RMobilePhone::TMultimodeType>*)aPckg1;
	RMobilePhone::TMultimodeType& paramclass=(*paramPckg)();
	
	if(iRegStatusInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	if(paramclass.ExtensionId() == KETelExtMultimodeV1)
		{
		__ASSERT_ALWAYS(!iCurrNetChangeV1NotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
		
		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV1& currentNetwork=(*currentNetworkPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
				
		iCurrNetChangeV1NotificationPending=ETrue;
		iCurrNetChangeV1NotificationReqHandle=aReqHandle;
		iCurrNetChangeV1NotificationNetInfo=&currentNetwork;
		}
	else if(paramclass.ExtensionId() == KETelExtMultimodeV2)
		{
		__ASSERT_ALWAYS(!iCurrNetChangeV2NotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
		
		RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV2Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV2& currentNetwork=(*currentNetworkPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
				
		iCurrNetChangeV2NotificationPending=ETrue;
		iCurrNetChangeV2NotificationReqHandle=aReqHandle;
		iCurrNetChangeV2NotificationNetInfo=&currentNetwork;
		}
	else if(paramclass.ExtensionId() == KEtelExtMultimodeV5)
		{
		__ASSERT_ALWAYS(!iCurrNetChangeV5NotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
		
		RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV5Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV5& currentNetwork=(*currentNetworkPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
				
		iCurrNetChangeV5NotificationPending=ETrue;
		iCurrNetChangeV5NotificationReqHandle=aReqHandle;
		iCurrNetChangeV5NotificationNetInfo=&currentNetwork;
		}
	else if(paramclass.ExtensionId() == KEtelExtMultimodeV8)
		{
		__ASSERT_ALWAYS(!iCurrNetChangeV8NotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
		
		RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV8Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV8& currentNetwork=(*currentNetworkPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
				
		iCurrNetChangeV8NotificationPending=ETrue;
		iCurrNetChangeV8NotificationReqHandle=aReqHandle;
		iCurrNetChangeV8NotificationNetInfo=&currentNetwork;
		}
	else if(paramclass.ExtensionId() == KETelExt3rdPartyV1)
		{
		RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg* currentNetworkPckg=(RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg*)aPckg1;
		RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1& currentNetwork=(*currentNetworkPckg)();			

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
		
		iCurrNetChangeEtelIsvNotificationPending=ETrue;
		iCurrNetChangeEtelIsvNotificationReqHandle=aReqHandle;
		iCurrNetChangeEtelIsvNotificationNetInfo=&currentNetwork;
		}
	else 
		{	
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iCurrentNetworkChangeLocationAreaNotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iCurrentNetworkChangeLocationArea = &area;
	iCurrentNetworkChangeLocationAreaNotificationPending = ETrue;
	
	return KErrNone;
	}

void CSimNetworkStatus::NotifyCurrentNetworkChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancel a previous request to be notified of a change in current network.
 * @param aTsyReqHandle request handle
 */
	{
	if(iCurrNetChangeV1NotificationPending && iCurrNetChangeV1NotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeV1NotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeV1NotificationReqHandle,KErrCancel);
		}
	else if(iCurrNetChangeV2NotificationPending && iCurrNetChangeV2NotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeV2NotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeV2NotificationReqHandle,KErrCancel);
		}
	else if(iCurrNetChangeV5NotificationPending && iCurrNetChangeV5NotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeV5NotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeV5NotificationReqHandle,KErrCancel);
		}
	else if(iCurrNetChangeV8NotificationPending && iCurrNetChangeV8NotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeV8NotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeV8NotificationReqHandle,KErrCancel);
		}
	else if(iCurrNetChangeEtelIsvNotificationPending && iCurrNetChangeEtelIsvNotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeEtelIsvNotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeEtelIsvNotificationReqHandle,KErrCancel);
		}
	iCurrentNetworkChangeLocationAreaNotificationPending = EFalse;
	}

TInt CSimNetworkStatus::NotifyCurrentNetworkNoLocationChange(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Register an EtelISV client's interest in being notified when the current network changes.
 * This function records the request's parameters and awaits a change in current network
 * before completing.
 *
 * @param aPckg1		The first parameter package.  This is populated with the current network 
 *						information when the request completes.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMultimodeType>* paramPckg=(TPckg<RMobilePhone::TMultimodeType>*)aPckg1;
	RMobilePhone::TMultimodeType& paramclass=(*paramPckg)();
	
	if(iRegStatusInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	if(paramclass.ExtensionId() == KETelExtMultimodeV1)
		{
		__ASSERT_ALWAYS(!iCurrNetChangeNoLocV1NotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
		
		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV1& currentNetwork=(*currentNetworkPckg)();


		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
				
		iCurrNetChangeNoLocV1NotificationPending=ETrue;
		iCurrNetChangeNoLocV1NotificationReqHandle=aReqHandle;
		iCurrNetChangeNoLocV1NotificationNetInfo=&currentNetwork;
		}
	else if(paramclass.ExtensionId() == KETelExtMultimodeV2)
		{
		__ASSERT_ALWAYS(!iCurrNetChangeNoLocV2NotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
		
		RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV2Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV2& currentNetwork=(*currentNetworkPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}

		iCurrNetChangeNoLocV2NotificationPending=ETrue;
		iCurrNetChangeNoLocV2NotificationReqHandle=aReqHandle;
		iCurrNetChangeNoLocV2NotificationNetInfo=&currentNetwork;
		}
	else if(paramclass.ExtensionId() == KEtelExtMultimodeV5)
		{
		__ASSERT_ALWAYS(!iCurrNetChangeNoLocV5NotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
		
		RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV5Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV5& currentNetwork=(*currentNetworkPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}

		iCurrNetChangeNoLocV5NotificationPending=ETrue;
		iCurrNetChangeNoLocV5NotificationReqHandle=aReqHandle;
		iCurrNetChangeNoLocV5NotificationNetInfo=&currentNetwork;
		}
	else if(paramclass.ExtensionId() == KEtelExtMultimodeV8)
		{
		__ASSERT_ALWAYS(!iCurrNetChangeNoLocV8NotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
		
		RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* currentNetworkPckg=(RMobilePhone::TMobilePhoneNetworkInfoV8Pckg*)aPckg1;
		RMobilePhone::TMobilePhoneNetworkInfoV8& currentNetwork=(*currentNetworkPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(currentNetwork);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}

		iCurrNetChangeNoLocV8NotificationPending=ETrue;
		iCurrNetChangeNoLocV8NotificationReqHandle=aReqHandle;
		iCurrNetChangeNoLocV8NotificationNetInfo=&currentNetwork;
		}
	else 
		{	
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}
	return KErrNone;
	}

void CSimNetworkStatus::NotifyCurrentNetworkNoLocationChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * Cancel a previous request from an EtelISV client to be notified of a change in current network.
 * @param aTsyReqHandle request handle
 */
	{
	if(iCurrNetChangeNoLocV1NotificationPending && iCurrNetChangeNoLocV1NotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeNoLocV1NotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeNoLocV1NotificationReqHandle,KErrCancel);
		}
	else if(iCurrNetChangeNoLocV2NotificationPending && iCurrNetChangeNoLocV2NotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeNoLocV2NotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeNoLocV2NotificationReqHandle,KErrCancel);
		}
	else if(iCurrNetChangeNoLocV5NotificationPending && iCurrNetChangeNoLocV5NotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeNoLocV5NotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeNoLocV5NotificationReqHandle,KErrCancel);
		}
	else if(iCurrNetChangeNoLocV8NotificationPending && iCurrNetChangeNoLocV8NotificationReqHandle == aTsyReqHandle)
		{
		iCurrNetChangeNoLocV8NotificationPending=EFalse;
		iPhone->ReqCompleted(iCurrNetChangeNoLocV8NotificationReqHandle,KErrCancel);
		}
	}

TInt CSimNetworkStatus::GetNITZInfo(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Retrieve the current "Network Information and Time Zone" (NITZ) information.
 * It will be updated from NitzInfo tags in the configuration file.  If no NITZ
 * profile information is contained in the configuration file KErrNotSupported
 * will be returned.
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg			The first parameter package.  This will be populated with the
 *						NITZ information to be returned.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneNITZ>* nitzPckg=(TPckg<RMobilePhone::TMobilePhoneNITZ>*)aPckg;
	RMobilePhone::TMobilePhoneNITZ& nitz=(*nitzPckg)();

	if(iNitzInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	nitz=iCurrentNitzInfo;
	PopulateNitzNetworkNames(nitz);
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimNetworkStatus::NotifyNITZInfoChange(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Register a client's interest in being notified when NITZ information is received.
 * This function records the request's parameters and awaits a NITZ information event, as
 * scheduled by the "NitzInfo" tags in the configuration file.
 *
 * @param aPckg			The first parameter package.  This is populated with the NITZ
 *						information when the request completes.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneNITZ>* nitzPckg=(TPckg<RMobilePhone::TMobilePhoneNITZ>*)aPckg;
	RMobilePhone::TMobilePhoneNITZ& nitz=(*nitzPckg)();

	if(iNitzInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	iNitzChangeNotificationPending=ETrue;
	iNitzChangeNotificationReqHandle=aReqHandle;
	iNitzChangeNotificationInfo=&nitz;
	return KErrNone;
	}

void CSimNetworkStatus::NotifyNITZInfoChangeCancel()
/**
 * Cancel a previous request to be notified of new NITZ informations.
 */
	{
	if(iNitzChangeNotificationPending)
		{
		iNitzChangeNotificationPending=EFalse;
		iPhone->ReqCompleted(iNitzChangeNotificationReqHandle,KErrCancel);
		}
	}

void CSimNetworkStatus::TimerCallBack(TInt aId)
/**
 * The timer callback function.  This function will be called when either the
 * Registration Status timer, cell info timer or the NITZ timer complete.  The aId parameter
 * is used to indicate which timer has completed and the appropriate callback function
 * is called.
 * @param aId	This parameter indicates which timer has completed.  The timer ID is
 *				registered when the timer is started.
 */
	{
	switch(aId)
		{
	case ETimerIdNtwkStatRegStat:
		TimerCallBackRegStat();
		break;
	case ETimerIdNtwkStatNitz:
		TimerCallBackNitz();
		break;
	case ETimerIdNtwkStatRadioMode:
		TInt err;
		err = KErrNone;
		TRAP(err,TimerCallBackRadioModeL());
		break;

	case ETimerIdNtwkStatRoamStat:
		 SimPanic(EIllegalCallBackId);		
		break;
	case ETimerIdCellInfo:
		TimerCallBackCellInfo();
		break;
	default:
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACK_1, "CSimNetworkStatus::TimerCallBack(%d), OOR [%d]",aId, ETimerIdNtwkStatRegStat);
		SimPanic(EIllegalCallBackId);
		break;
		}
	}

void CSimNetworkStatus::TimerCallBackRegStat()
/**
 * The Registration timer callback function.  This function will be called when the
 * Network Registration timer completes.  It indicates a simulated change in registration
 * status. So, the new network settings must be loaded into the relevant member variables
 * and any pending registration notification requests must be completed.
 * Finally, the next Network Registration timer is started.
 */
	{
	iRegStatIndex++;
	if(iRegStatusInfo->Count()<=iRegStatIndex)
		return;

	TInt oldNetworkIndex=iCurrentNetworkIndex;
	iCurrentRegStatus=iRegStatusInfo->At(iRegStatIndex).iRegStatus;
	iCurrentNetworkIndex=iRegStatusInfo->At(iRegStatIndex).iNetworkInfoIndex;

	// Ask phone to check if indicator notification needs to be triggered
	iPhone->CheckIndicatorNotification();

	if(iRegStatChangeNotificationPending)
		{
		iRegStatChangeNotificationPending=EFalse;
		*iRegStatChangeNotificationRegStatus=(RMobilePhone::TMobilePhoneRegistrationStatus)iCurrentRegStatus;
		iPhone->ReqCompleted(iRegStatChangeNotificationReqHandle,KErrNone);
		}
	if(oldNetworkIndex!=iCurrentNetworkIndex && iCurrentNetworkIndex>=0)
		{
		if(iCurrentNetworkChangeLocationAreaNotificationPending)
			{
			if((iCurrentNetworkIndex >= 0) &&(iCurrentNetworkIndex < iLocationArea->Count()))
				{
				iCurrentNetworkChangeLocationArea->iAreaKnown = iLocationArea->At(iCurrentNetworkIndex).iAreaKnown;
				iCurrentNetworkChangeLocationArea->iLocationAreaCode = iLocationArea->At(iCurrentNetworkIndex).iLocationAreaCode;
				iCurrentNetworkChangeLocationArea->iCellId = iLocationArea->At(iCurrentNetworkIndex).iCellId;
				}
			else
				{
				iCurrentNetworkChangeLocationArea->iAreaKnown = EFalse;
				}
			iCurrentNetworkChangeLocationAreaNotificationPending=EFalse;
			}
		if(iCurrNetChangeV1NotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_1, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TMobilePhoneNetworkInfoV1");
			iCurrNetChangeV1NotificationPending = EFalse;
			RMobilePhone::TMobilePhoneNetworkInfoV1* changeNetInfoV1 = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV1*>(iCurrNetChangeV1NotificationNetInfo);

			PopulateNetworkInfoV1(iCurrentNetworkIndex,*changeNetInfoV1);
			iPhone->ReqCompleted(iCurrNetChangeV1NotificationReqHandle,KErrNone);
			}
			
		if(iCurrNetChangeV2NotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_2, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TMobilePhoneNetworkInfoV2");
			iCurrNetChangeV2NotificationPending = EFalse;
			RMobilePhone::TMobilePhoneNetworkInfoV2* changeNetInfoV2 = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV2*>(iCurrNetChangeV2NotificationNetInfo);
			PopulateNetworkInfoV1(iCurrentNetworkIndex,*changeNetInfoV2);
			//also set v2 paramaters
			changeNetInfoV2->iAccess = iNetworkInfo->At(iCurrentNetworkIndex).iAccess;
			iPhone->ReqCompleted(iCurrNetChangeV2NotificationReqHandle,KErrNone);
			}
		
		if(iCurrNetChangeV5NotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_3, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TMobilePhoneNetworkInfoV5");
			iCurrNetChangeV5NotificationPending = EFalse;
			RMobilePhone::TMobilePhoneNetworkInfoV5* changeNetInfoV5 = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV5*>(iCurrNetChangeV5NotificationNetInfo);
			PopulateNetworkInfoV1(iCurrentNetworkIndex,*changeNetInfoV5);
			//also set v5 paramaters
			changeNetInfoV5->iAccess = iNetworkInfoV8->At(0).iAccess;
			changeNetInfoV5->iHsdpaAvailableIndicator= iNetworkInfoV8->At(0).iHsdpaAvailableIndicator;
			changeNetInfoV5->iEgprsAvailableIndicator = iNetworkInfoV8->At(0).iEgprsAvailableIndicator;
			iPhone->ReqCompleted(iCurrNetChangeV5NotificationReqHandle,KErrNone);
			}
			
		if(iCurrNetChangeV8NotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_4, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TMobilePhoneNetworkInfoV8");
			iCurrNetChangeV8NotificationPending = EFalse;
			RMobilePhone::TMobilePhoneNetworkInfoV8* changeNetInfoV8 = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV8*>(iCurrNetChangeV8NotificationNetInfo);
			PopulateNetworkInfoV1(iCurrentNetworkIndex,*changeNetInfoV8);
			//also set v5 paramaters
			changeNetInfoV8->iAccess = iNetworkInfo->At(iCurrentNetworkIndex).iAccess;
			changeNetInfoV8->iHsdpaAvailableIndicator = iNetworkInfoV8->At(0).iHsdpaAvailableIndicator;
			changeNetInfoV8->iEgprsAvailableIndicator = iNetworkInfoV8->At(0).iEgprsAvailableIndicator;
			changeNetInfoV8->iHsupaAvailableIndicator = iNetworkInfoV8->At(0).iHsupaAvailableIndicator;
			iPhone->ReqCompleted(iCurrNetChangeV8NotificationReqHandle,KErrNone);
			}
					
		if(iCurrNetChangeEtelIsvNotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_5, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TEtel3rdPartyMobileNetworkInfoV1");
			iCurrNetChangeEtelIsvNotificationPending = EFalse;
			RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1* changeNetInfoEtelIsv = reinterpret_cast<RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1*>(iCurrNetChangeEtelIsvNotificationNetInfo);

			PopulateEtel3rdPartyNetworkInfoV1(iCurrentNetworkIndex, *changeNetInfoEtelIsv);
			iPhone->ReqCompleted(iCurrNetChangeEtelIsvNotificationReqHandle,KErrNone);
			}
		if(iCurrNetChangeNoLocV1NotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_6, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TMobilePhoneNetworkInfoV1 (No Location)");
			iCurrNetChangeNoLocV1NotificationPending = EFalse;
			RMobilePhone::TMobilePhoneNetworkInfoV1* changeNetInfoV1 = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV1*>(iCurrNetChangeNoLocV1NotificationNetInfo);

			PopulateNetworkInfoV1(iCurrentNetworkIndex,*changeNetInfoV1);
			iPhone->ReqCompleted(iCurrNetChangeNoLocV1NotificationReqHandle,KErrNone);
			}
			
		if(iCurrNetChangeNoLocV2NotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_7, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TMobilePhoneNetworkInfoV2 (No Location)");
			iCurrNetChangeNoLocV2NotificationPending = EFalse;
			RMobilePhone::TMobilePhoneNetworkInfoV2* changeNetInfoV2 = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV2*>(iCurrNetChangeNoLocV2NotificationNetInfo);

			PopulateNetworkInfoV1(iCurrentNetworkIndex,*changeNetInfoV2);
			//also set v2 paramaters
			changeNetInfoV2->iAccess = iNetworkInfo->At(iCurrentNetworkIndex).iAccess;
			iPhone->ReqCompleted(iCurrNetChangeNoLocV2NotificationReqHandle,KErrNone);
			}
			
		if(iCurrNetChangeNoLocV5NotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_8, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TMobilePhoneNetworkInfoV5 (No Location)");
			iCurrNetChangeNoLocV5NotificationPending = EFalse;
			RMobilePhone::TMobilePhoneNetworkInfoV5* changeNetInfoV5 = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV5*>(iCurrNetChangeNoLocV5NotificationNetInfo);

			PopulateNetworkInfoV1(iCurrentNetworkIndex,*changeNetInfoV5);
			//also set v5 paramaters
			changeNetInfoV5->iAccess = iNetworkInfoV8->At(0).iAccess;
			changeNetInfoV5->iHsdpaAvailableIndicator= iNetworkInfoV8->At(0).iHsdpaAvailableIndicator;
			changeNetInfoV5->iEgprsAvailableIndicator= iNetworkInfoV8->At(0).iEgprsAvailableIndicator;
			iPhone->ReqCompleted(iCurrNetChangeNoLocV5NotificationReqHandle,KErrNone);
			}
			
		if(iCurrNetChangeNoLocV8NotificationPending)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKREGSTAT_9, "CSimNetworkStatus::TimerCallBackRegStat() - completing NotifyCurrentNetworkChange TMobilePhoneNetworkInfoV8 (No Location)");
			iCurrNetChangeNoLocV8NotificationPending = EFalse;
			RMobilePhone::TMobilePhoneNetworkInfoV8* changeNetInfoV8 = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV8*>(iCurrNetChangeNoLocV8NotificationNetInfo);

			PopulateNetworkInfoV1(iCurrentNetworkIndex,*changeNetInfoV8);
			//also set v8 paramaters
			changeNetInfoV8->iAccess = iNetworkInfo->At(iCurrentNetworkIndex).iAccess;
			changeNetInfoV8->iHsdpaAvailableIndicator= iNetworkInfoV8->At(0).iHsdpaAvailableIndicator;
			changeNetInfoV8->iEgprsAvailableIndicator= iNetworkInfoV8->At(0).iEgprsAvailableIndicator;
			changeNetInfoV8->iHsupaAvailableIndicator= iNetworkInfoV8->At(0).iHsupaAvailableIndicator;
			iPhone->ReqCompleted(iCurrNetChangeNoLocV8NotificationReqHandle,KErrNone);
			}			
		}		

	iRegStatTimer->Start(iRegStatusInfo->At(iRegStatIndex).iDuration,this,ETimerIdNtwkStatRegStat);
	}

void CSimNetworkStatus::TimerCallBackNitz()
/**
 * The NITZ timer callback function.  This function will be called when the NITZ timer
 * completes.  It indicates a simulated new NITZ information event. So, the new
 * NITZ settings must be loaded into the relevant NITZ member variables and any
 * pending NITZ notification requests must be completed.
 * Finally, the next NITZ timer is started.
 */
	{
	iNitzIndex++;
	if(iNitzInfo->Count()<=iNitzIndex)
		return;
	
	iCurrentNitzInfo=iNitzInfo->At(iNitzIndex).iNitz;

	if(iNitzChangeNotificationPending)
		{
		iNitzChangeNotificationPending=EFalse;
		*iNitzChangeNotificationInfo=iCurrentNitzInfo;
		PopulateNitzNetworkNames(*iNitzChangeNotificationInfo);
		iPhone->ReqCompleted(iNitzChangeNotificationReqHandle,KErrNone);
		}

	iNitzTimer->Start(iNitzInfo->At(iNitzIndex).iDuration,this,ETimerIdNtwkStatNitz);
	}
	
void CSimNetworkStatus::TimerCallBackRadioModeL()
/**
 * The RadioMode timer callback function.  This function will be called when the RadioMode timer
 * completes.  
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKRADIOMODEL_1, "CSimNetworkStatus::TimerCallBackRadioModeL");
	iRadioModeIndex++;
	if(iRadioModeInfo->Count()<=iRadioModeIndex)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKRADIOMODEL_2, "CSimNetworkStatus::TimerCallBackRadioModeL returning, no more config");
		return;
		}

	iRadioMode=iRadioModeInfo->At(iRadioModeIndex);
	if(iRadioMode.iRadioOn==EFalse)
		{
		RProperty::Set(KUidSystemCategory, KUidPhonePwrValue, ESAPhoneOff);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKRADIOMODEL_3, "Radio Mode = OFF (config duration passed)");
		}
	else
		{
		RProperty::Set(KUidSystemCategory, KUidPhonePwrValue, ESAPhoneOn);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKRADIOMODEL_4, "Radio Mode = ON (config duration passed)");
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNETWORKSTATUS_TIMERCALLBACKRADIOMODEL_5, "Next radio Mode duration = %d (from config)", iRadioModeInfo->At(iRadioModeIndex).iDuration);
	iRadioModeTimer->Start(iRadioModeInfo->At(iRadioModeIndex).iDuration,this,ETimerIdNtwkStatRadioMode);
	}

void CSimNetworkStatus::PopulateNetworkInfoV1(TInt aIndex,RMobilePhone::TMobilePhoneNetworkInfoV1& aNetInfo)
/**
 * A helper function to copy network information structures.
 */
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1& curNetnfo=reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV1&>(iNetworkInfo->At(aIndex));	
	aNetInfo.iMode = curNetnfo.iMode;
	aNetInfo.iStatus = curNetnfo.iStatus;
	aNetInfo.iBandInfo = curNetnfo.iBandInfo;
	aNetInfo.iCountryCode = curNetnfo.iCountryCode;
	//aNetInfo.iCdmaSID = curNetnfo.iCdmaSID;
	aNetInfo.iAnalogSID = curNetnfo.iAnalogSID;
	aNetInfo.iNetworkId = curNetnfo.iNetworkId;
	aNetInfo.iDisplayTag = curNetnfo.iDisplayTag;
	aNetInfo.iShortName = curNetnfo.iShortName;
	aNetInfo.iLongName = curNetnfo.iLongName;
	}
	
void CSimNetworkStatus::PopulateEtel3rdPartyNetworkInfoV1(TInt aIndex,RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1& aNetInfo)
/**
 * A helper function to copy network information structures.
 */
	{
	RMobilePhone::TMobilePhoneNetworkInfoV2& curNetnfo=iNetworkInfo->At(aIndex);	
	aNetInfo.iMode = curNetnfo.iMode;
	aNetInfo.iStatus = curNetnfo.iStatus;
	aNetInfo.iBandInfo = curNetnfo.iBandInfo;
	aNetInfo.iCountryCode = curNetnfo.iCountryCode;
	//aNetInfo.iCdmaSID = curNetnfo.iCdmaSID;
	aNetInfo.iNetworkId = curNetnfo.iNetworkId;
	aNetInfo.iDisplayTag = curNetnfo.iDisplayTag;
	aNetInfo.iShortName = curNetnfo.iShortName;
	aNetInfo.iLongName = curNetnfo.iLongName;
	aNetInfo.iAccess = curNetnfo.iAccess;
	}

RMobilePhone::TMobilePhoneNetworkStatus CSimNetworkStatus::NetWorkStatus()
/**
* A helper function that returns the network status;
*/
	{
	if(!iNetworkInfo || (iCurrentNetworkIndex==KErrNotFound))
		{
		return RMobilePhone::ENetworkStatusUnknown;
		}
	return iNetworkInfo->At(iCurrentNetworkIndex).iStatus;
	}

void CSimNetworkStatus::PopulateNitzNetworkNames(RMobilePhone::TMobilePhoneNITZ& aNitz)
/**
 * A helper function to populate the long and short network name fields in the NITZ
 * data structures.  These are taken from the current network settings.  If no network
 * name information is available this is indicated in the "NITZ fields used" structure.
 * @param aNitz		The NITZ data structure which must be populated with network name
 *					information.
 */
	{
	if((iNetworkInfo->Count()>0)&&(iRegStatusInfo->Count()>0))
		{
		if(iCurrentNetworkIndex>=0)
			{
			aNitz.iShortNetworkId.Copy(iNetworkInfo->At(iCurrentNetworkIndex).iShortName);
			aNitz.iLongNetworkId.Copy(iNetworkInfo->At(iCurrentNetworkIndex).iLongName);
			aNitz.iNitzFieldsUsed|=	(RMobilePhone::KCapsShortNameAvailable
									|RMobilePhone::KCapsLongNameAvailable);
			}
		}
	}

TInt CSimNetworkStatus::GetCurrentServiceProvider(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Completes the request to get the current service provider name
 */
	{
	RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg* currentSPPckg=(RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg*)aPckg;
	RMobilePhone::TMobilePhoneServiceProviderNameV2& currenSP=(*currentSPPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(currenSP);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}
	
	currenSP = iServiceProvider;
	
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CSimNetworkStatus::GetCurrentNetworkName(TTsyReqHandle aReqHandle,TDes8* aNetworkName, TDes8* aLocInfo)
/**
 * Completes request to get current network name.
 */
	{
	if(iRegStatusInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}
	
	if(iCurrentNetworkIndex==KErrNotFound)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotFound);
		return KErrNone;
		}
	
	RMobilePhone::TMobilePhoneNetworkNameV3Pckg* networknamePckg=REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkNameV3Pckg*, aNetworkName);
	RMobilePhone::TMobilePhoneNetworkNameV3& networkname=(*networknamePckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(networkname);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}
	
	RMobilePhone::TMobilePhoneOPlmnV3Pckg* locInfoPckg=REINTERPRET_CAST(RMobilePhone::TMobilePhoneOPlmnV3Pckg*, aLocInfo);
	RMobilePhone::TMobilePhoneOPlmnV3& locInfo=(*locInfoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	err = iPhone->CheckSimTsyVersion(locInfo);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}
	
	//use the current network info to populate the current networkname details.
	RMobilePhone::TMobilePhoneNetworkInfoV1& currentNetwork = reinterpret_cast<RMobilePhone::TMobilePhoneNetworkInfoV1&>(iNetworkInfo->At(iCurrentNetworkIndex));

	// Check that the data structure is supported by the simulated TSY version
	err = iPhone->CheckSimTsyVersion(currentNetwork);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	
	networkname.iLongName.Copy(currentNetwork.iLongName);
	networkname.iShortName.Copy(currentNetwork.iShortName);
	networkname.iOtherNames.Zero();
	
	locInfo.iCountryCode = currentNetwork.iCountryCode;
	locInfo.iFirstLocationAreaCode = KFirstLACDefault;
	locInfo.iLastLocationAreaCode = KLastLACDefault;
	locInfo.iNetworkId = currentNetwork.iNetworkId;
	locInfo.iPNNid = 0;	//set to 0 so that it does not require a lookup in the TMobilePhoneOPlmnV3.iOtherNames field for display name - which is emtpy.
	
	iPhone->ReqCompleted(aReqHandle,KErrNone);
		
	return KErrNone;	
	}
const CTestConfigSection* CSimNetworkStatus::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file data section.
 */
	{
	return iPhone->CfgFile();
	}

void CSimNetworkStatus::PubSubCallback(TInt aRegStat)
/**
 * Called by CSimNetStatChange when a network registration status change request has been received.
 *
 * @param aNewVal The new network registration value (number of bars)
 */
	{
	if (iCurrentRegStatus == aRegStat)
		{
		return;
		}

	iCurrentRegStatus = aRegStat;
	
	// Ask phone to check if indicator notification needs to be triggered
	iPhone->CheckIndicatorNotification();

	if(iRegStatChangeNotificationPending)
		{
		iRegStatChangeNotificationPending=EFalse;
		*iRegStatChangeNotificationRegStatus=(RMobilePhone::TMobilePhoneRegistrationStatus)iCurrentRegStatus;
		iPhone->ReqCompleted(iRegStatChangeNotificationReqHandle,KErrNone);
		}
	}

TInt CSimNetworkStatus::GetCellInfo(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Retrieve the current Cell information.
 * It will be updated from CellInfo tags in the configuration file.  If no cell
 * information is contained in the configuration file KErrNotSupported
 * will be returned.
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg			The first parameter package.  This will be populated with the
 *						cell information to be returned.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneCellInfoV9>* cellInfoPckg=(TPckg<RMobilePhone::TMobilePhoneCellInfoV9>*)aPckg;
	RMobilePhone::TMobilePhoneCellInfoV9& cellInfo=(*cellInfoPckg)();

	if(iCellInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	cellInfo=iCurrentCellInfo;
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimNetworkStatus::NotifyCellInfoChange(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Register a client's interest in being notified when cell information is received.
 * This function records the request's parameters and awaits a cell information event, as
 * scheduled by the "CellInfo" tags in the configuration file.
 *
 * @param aPckg			The first parameter package.  This is populated with the cell
 *						information when the request completes.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMobilePhoneCellInfoV9>* cellInfoPckg=(TPckg<RMobilePhone::TMobilePhoneCellInfoV9>*)aPckg;
	RMobilePhone::TMobilePhoneCellInfoV9& cellInfo=(*cellInfoPckg)();

	if(iCellInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	iCellInfoChangeNotificationPending=ETrue;
	iCellInfoChangeNotificationReqHandle=aReqHandle;
	iCellInfoChangeNotificationInfo=&cellInfo;
	return KErrNone;
	}

void CSimNetworkStatus::NotifyCellInfoChangeCancel(TTsyReqHandle aTsyReqHandle)
/**
 * Cancel a previous request to be notified of new cell informations.
 * @param aTsyReqHandle request handle
 */
	{
	if((iCellInfoChangeNotificationPending) && (aTsyReqHandle == iCellInfoChangeNotificationReqHandle))
		{
		iCellInfoChangeNotificationPending=EFalse;
		iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	}

void CSimNetworkStatus::TimerCallBackCellInfo()
/**
 * The CellInfo timer callback function.  This function will be called when the cell info timer
 * completes.  It indicates a simulated new cell info information event. So, the new
 * cell info settings must be loaded into the relevant cell info member variables and any
 * pending cell info notification requests must be completed.
 * Finally, the next cell info timer is started.
 */
	{
	iCellInfoIndex++;
	if(iCellInfo->Count()<=iCellInfoIndex)
		return;
	
	iCurrentCellInfo=iCellInfo->At(iCellInfoIndex).iInfo;

	if(iCellInfoChangeNotificationPending)
		{
		iCellInfoChangeNotificationPending=EFalse;
		*iCellInfoChangeNotificationInfo=iCurrentCellInfo;
		iPhone->ReqCompleted(iCellInfoChangeNotificationReqHandle,KErrNone);
		}

	iCellInfoTimer->Start(iCellInfo->At(iCellInfoIndex).iDuration,this,ETimerIdCellInfo);
	}
