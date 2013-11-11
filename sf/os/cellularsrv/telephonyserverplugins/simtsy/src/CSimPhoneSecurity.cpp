// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the Phone Security manipulation code.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPhoneSecurityTraces.h"
#endif

#include "CSimPhone.h"
#include "utils.h"
#include "CSimPhoneSecurity.h"
#include <testconfigfileparser.h>
#include "CSimTsyMode.h"
#include <e32math.h>

//
// CSimPhoneSecurity
//
const TInt KICCProfileGranularity=5;		// < Granularity for Icc profile list array

const TInt KDefaultPinEntryAttempts=3;		// < default number of PIN entry attempts if not defined in the config file
const TInt KDefaultPukEntryAttempts=10;		// < default number of PUK entry attempts if not defined in the config file

CSimPhoneSecurity* CSimPhoneSecurity::NewL(CSimPhone* aPhone)
/**
 * Standard two phase constructor.
 * @param aPhone The phone object from which the PhoneSecurity was opened.
 */
	{
	CSimPhoneSecurity* obj=new(ELeave) CSimPhoneSecurity(aPhone);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop();
	return obj;
	}

CSimPhoneSecurity::CSimPhoneSecurity(CSimPhone* aPhone)
		: iPhone(aPhone) 
/**
 * Trivial first phase constructor.
 * @param aPhone	The phone object from which this phonebook was opened.
 */
	{}

void CSimPhoneSecurity::Init()
	{}

void CSimPhoneSecurity::ConstructL()
/** Change
 * Second phase constructor that allocates memory for the phonebook, batch read buffer and
 * a delayed completion timer.  The constructor also reads the individual and batch read
 * delays from the configuration file.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_1, "Starting to parse PhoneSecurity additional config parameters...");
	
	iNotifySettingsTimer = CIccTimer::NewL();
	iNotifySecurityEventsTimer = CIccTimer::NewL();
	
	iNotifySettingsTimer->iTimer = CSimTimer::NewL(iPhone);
	iNotifySecurityEventsTimer->iTimer = CSimTimer::NewL(iPhone);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	TInt i;
	iICCStatusProfile=new(ELeave) CArrayFixFlat<TICCStatusEntry>(KICCProfileGranularity);
	iICCSettingProfile=new(ELeave) CArrayFixFlat<TICCSettingEntry>(KICCProfileGranularity);

	item=CfgFile()->Item(KSecurityCodes);
	if(item)
		{
		TPtrC8 temp;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,0,KSecurityCodes);
			}
		else
			iSecCodes.iPin1.Copy(temp);
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,1,KSecurityCodes);
			}
		else
			iSecCodes.iPin2.Copy(temp);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,2,KSecurityCodes);
			}
		else
			iSecCodes.iPuk1.Copy(temp);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,3,KSecurityCodes);
			}
		else
			iSecCodes.iPuk2.Copy(temp);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,4,KSecurityCodes);
			}
		else
			iSecCodes.iPhonePassword.Copy(temp);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,5,KSecurityCodes);
			}
		else
			iSecCodes.iSPC.Copy(temp);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,6,KSecurityCodes);
			}
		else
			iSecCodes.iPhBkHiddenKey.Copy(temp);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,7,KSecurityCodes);
			}
		else
			iSecCodes.iUSimAppPin.Copy(temp);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,8,KSecurityCodes);
			}
		else
			iSecCodes.iSecondUSimAppPin.Copy(temp);

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,9,KSecurityCodes);
			}
		else
			iSecCodes.iUniversalPin.Copy(temp);
		}
	
	
	item=CfgFile()->Item(KSecurityCodeAttempts);
	if(item)
		{
		TInt temp;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element SECURITYCODEATTEMPTS returned %d (element no. %d) from tag %s." ,ret,0,KSecurityCodeAttempts);
			}
		else
			{
			if (temp <= 0)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_13, "Using default value...");
				temp = KDefaultPinEntryAttempts;
				}
			iSecurityCodeRemainigAttempts.iPin1=temp;
			iSecurityCodeDefaultRemainigAttempts.iPin1=temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element SECURITYCODEATTEMPTS returned %d (element no. %d) from tag %s." ,ret,1,KSecurityCodeAttempts);
			}
		else
			{
			if (temp <= 0)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_15, "Reading element SECURITYCODEATTEMPTS returned %d (element no. %d) from tag %s expected > 0" ,temp,1,KSecurityCodeAttempts);
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_16, "Using default value...");
				temp = KDefaultPinEntryAttempts;
				}
			iSecurityCodeRemainigAttempts.iPin2=temp;
			iSecurityCodeDefaultRemainigAttempts.iPin2=temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_17, "WARNING - CONFIGURATION FILE PARSING - Reading element SECURITYCODEATTEMPTS returned %d (element no. %d) from tag %s." ,ret,2,KSecurityCodeAttempts);
			}
		else
			{
			if (temp <= 0)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_18, "Reading element SECURITYCODEATTEMPTS returned %d (element no. %d) from tag %s expected > 0" ,temp,2,KSecurityCodeAttempts);
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_19, "Using default value...");
				temp = KDefaultPukEntryAttempts;
				}
			iSecurityCodeRemainigAttempts.iPuk1=temp;
			iSecurityCodeDefaultRemainigAttempts.iPuk1=temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_20, "WARNING - CONFIGURATION FILE PARSING - Reading element SECURITYCODEATTEMPTS returned %d (element no. %d) from tag %s."  ,ret,3,KSecurityCodeAttempts);
			}
		else
			{
			if (temp <= 0)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_21, "Reading element SECURITYCODEATTEMPTS returned %d (element no. %d) from tag %s expected > 0" ,temp,3,KSecurityCodeAttempts);
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_22, "Using default value...");
				temp = KDefaultPukEntryAttempts;
				}
			iSecurityCodeRemainigAttempts.iPuk2=temp;
			iSecurityCodeDefaultRemainigAttempts.iPuk2=temp;
			}
		}
	else
		{
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_23, "NOTE tag %s not defined, using default values",KSecurityCodeAttempts);
		iSecurityCodeDefaultRemainigAttempts.iPin1= KDefaultPinEntryAttempts;
		iSecurityCodeRemainigAttempts.iPin1= iSecurityCodeDefaultRemainigAttempts.iPin1;
		iSecurityCodeDefaultRemainigAttempts.iPin2= KDefaultPinEntryAttempts;
		iSecurityCodeRemainigAttempts.iPin2= iSecurityCodeDefaultRemainigAttempts.iPin2;
		iSecurityCodeDefaultRemainigAttempts.iPuk1= KDefaultPukEntryAttempts;
		iSecurityCodeRemainigAttempts.iPuk1= iSecurityCodeDefaultRemainigAttempts.iPuk1;
		iSecurityCodeDefaultRemainigAttempts.iPuk2= KDefaultPukEntryAttempts;
		iSecurityCodeRemainigAttempts.iPuk2 = iSecurityCodeDefaultRemainigAttempts.iPuk2;
		}
	
	item=CfgFile()->Item(KICCLockedAtStart);
	if(item)
		{
		TInt temp;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_24, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,0,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iPin1=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iPin1=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_25, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,1,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iPin2=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iPin2=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_26, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,2,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iPuk1=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iPuk1=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_27, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,3,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iPuk2=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iPuk2=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_28, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,4,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iPhonePassword=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iPhonePassword=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_29, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,5,KICCLockedAtStart);
			}
		else
			iSecStatus.iSPC=(RMobilePhone::TMobilePhoneLockStatus) temp;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_30, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,6,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iPhBkHiddenKey=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iPhBkHiddenKey=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_31, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,7,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iUSimAppPin=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iUSimAppPin=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_32, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,8,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iSecondUSimAppPin=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iSecondUSimAppPin=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,temp);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_33, "WARNING - CONFIGURATION FILE PARSING - Reading element TEMP returned %d (element no. %d) from tag %s.",ret,9,KICCLockedAtStart);
			}
		else
			{
			iSecStatus.iUniversalPin=(RMobilePhone::TMobilePhoneLockStatus) temp;
			iSecSetting.iUniversalPin=(RMobilePhone::TMobilePhoneLockSetting) temp;
			}
		}
		
		TInt count=CfgFile()->ItemCount(KICCStatus);
		for(i=0;i<count;i++)
			{
			item=CfgFile()->Item(KICCStatus,i);
			if(!item)
				break;

			TInt duration;
			TInt ICCStatus;
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_34, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KICCStatus);
				continue;
				}
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,ICCStatus);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_35, "WARNING - CONFIGURATION FILE PARSING - Reading element ICCSTATUS returned %d (element no. %d) from tag %s.",ret,1,KICCStatus);
				continue;
				}
			
			TICCStatusEntry icc;
			icc.iDuration=duration;
			icc.iICCStatus=(RMobilePhone::TMobilePhoneSecurityEvent) ICCStatus;
			iICCStatusProfile->AppendL(icc);
			}
			
			
		count=CfgFile()->ItemCount(KICCSetting);
		for(i=0;i<count;i++)
			{
			item=CfgFile()->Item(KICCSetting,i);
			if(!item)
				break;

	
			TInt duration(0), lock(0), lockSetting(0), lockStatus(0);

			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_36, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KICCSetting);
				continue;
				}
				
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,lock);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_37, "WARNING - CONFIGURATION FILE PARSING - Reading element LOCK returned %d (element no. %d) from tag %s.",ret,1,KICCSetting);
				continue;
				}
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,lockSetting);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_38, "WARNING - CONFIGURATION FILE PARSING - Reading element LOCKSETTING returned %d (element no. %d) from tag %s.",ret,2,KICCSetting);
				continue;
				}
			
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,lockStatus);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CONSTRUCTL_39, "WARNING - CONFIGURATION FILE PARSING - Reading element LOCKSTATUS returned %d (element no. %d) from tag %s.",ret,3,KICCSetting);
				continue;
				}
			
			TICCSettingEntry icc;
			icc.iLock = (RMobilePhone::TMobilePhoneLock)lock;
			icc.iDuration=duration;
			icc.iLockInfo.iSetting=(RMobilePhone::TMobilePhoneLockSetting) lockSetting;
			icc.iLockInfo.iStatus=(RMobilePhone::TMobilePhoneLockStatus) lockStatus;
			iICCSettingProfile->AppendL(icc);
			}

	}


CSimPhoneSecurity::~CSimPhoneSecurity()
/**
 * Standard destructor.  Any objects created by the ::ConstructL() function
 * will be destroyed here.
 */
	{

	if(iICCStatusProfile)
		{
		iICCStatusProfile->Delete(0,iICCStatusProfile->Count());
		delete iICCStatusProfile;
		}
	if(iICCSettingProfile)
		{
		iICCSettingProfile->Delete(0, iICCSettingProfile->Count());
		delete iICCSettingProfile;
		}
		
	delete iNotifySettingsTimer;
	delete iNotifySecurityEventsTimer;
	
	if (iUnlockNamTimer)
		{
		iUnlockNamTimer->Cancel();
		delete iUnlockNamTimer;
		iUnlockNamTimer = NULL;
		}
	if (iLockNamTimer)
		{
		iLockNamTimer->Cancel();
		delete iLockNamTimer;
		iLockNamTimer = NULL;
		}
	}

TInt CSimPhoneSecurity::ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
/**
 * Dispatch function for all Phonebook Store requests.
 * @param aReqHandle	The TSY request handle for this request.
 * @param aIpc			The IPC number of this request.
 * @param aPckg			The parameter package related to this request.
 * @return TInt			The return error condition.
 */
	{

	TAny* dataPtr = aPckg.Ptr1();
	TAny* dataPtr2 = aPckg.Ptr2();

// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{
	case EMobilePhoneGetSecurityCaps:
		return GetSecurityCaps(aReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneNotifySecurityCapsChange:
		return NotifySecurityCapsChange(aReqHandle, 
			REINTERPRET_CAST(TUint32*, dataPtr2));

	case EMobilePhoneGetLockInfo:
		return GetLockInfo(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLock*, dataPtr), 
			aPckg.Des2n());

	case EMobilePhoneNotifyLockInfoChange:
		return NotifyLockInfoChange(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLock*, dataPtr), 
			aPckg.Des2n());

	case EMobilePhoneSetLockSetting:
		return SetLockSetting(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLock*, dataPtr), 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLockSetting*, dataPtr2));

	case EMobilePhoneGetSecurityCodeInfo:
		return GetSecurityCodeInfo(aReqHandle, 
			reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCode*> (dataPtr), 
			aPckg.Des2n());

	case EMobilePhoneNotifySecurityCodeInfoChange:
		return NotifySecurityCodeInfoChange(aReqHandle, 
			reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCode*> (dataPtr), 
			aPckg.Des2n());
		
	case EMobilePhoneChangeSecurityCode:
		return ChangeSecurityCode(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneSecurityCode*, dataPtr),
			aPckg.Des2n());

	case EMobilePhoneNotifySecurityEvent:
		return NotifySecurityEvent(aReqHandle, aPckg.Des1n());

	case EMobilePhoneVerifySecurityCode:
		return VerifySecurityCode(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneSecurityCode*, dataPtr), 
			REINTERPRET_CAST(RMobilePhone::TCodeAndUnblockCode*, dataPtr2));
 
	case EMobilePhoneAbortSecurityCode:
		return AbortSecurityCode(aReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneSecurityCode*, dataPtr));
	default:
		break;
		}


// The TSY can only process one of the following requests at a time.  If a second is received
// while processing the first, then it will be errored with KErrInUse.  This restriction will
// be removed later, by inserting a request queuing mechanism.  Note that the standard TSY
// "flow control" mechanism works phone-wide and so is not suitable.

//	switch(aIpc)
//		{
//	default:
//		break;
//		}

	return KErrNotSupported;
	}

CTelObject* CSimPhoneSecurity::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimPhoneSecurity::OpenNewObjectL(TDes&)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimPhoneSecurity::ReqModeL(const TInt aIpc)
/**
 * This function returns the Request Mode for the request with the passed IPC value.
 * @param aIpc		The IPC number of the request.
 * @return TReqMode	The request mode.
 */
	{
	CTelObject::TReqMode ret=0;	
	// This function seems be unused. ReqModeL for security related IPC is in CSimPhone. 
	switch(aIpc)
		{
	case EMobilePhoneGetSecurityCaps:
	case EMobilePhoneGetLockInfo:
	case EMobilePhoneSetLockSetting:
	case EMobilePhoneChangeSecurityCode:
	case EMobilePhoneVerifySecurityCode:
	case EMobilePhoneAbortSecurityCode:
		break;

	case EMobilePhoneNotifyLockInfoChange:
	case EMobilePhoneNotifySecurityCapsChange:
	case EMobilePhoneNotifySecurityEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CSimPhoneSecurity::RegisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the first client makes a notification
 * request.  If supported by the underlying protocol controlling the
 * signalling stack, this can be used to start requesting updates for the relevant
 * service.
 */
	{
	return KErrNone;
	}

TInt CSimPhoneSecurity::DeregisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the last client that had previously
 * made a notification request closes its ETel Server handle.  If supported by
 * the underlying protocol controlling the	signalling stack, this can be used
 * to stop requesting updates for the relevant service.
 */
	{
	return KErrNone;
	}

TInt CSimPhoneSecurity::NumberOfSlotsL(const TInt aIpc)
/**
 * Return the number of slots that the ETel Server should allocate for buffering requests
 * of the given IPC number.
 */
	{
	switch(aIpc)
		{
	case EMobilePhoneNotifyLockInfoChange:
	case EMobilePhoneNotifySecurityCapsChange:
	case EMobilePhoneNotifySecurityEvent:
		return KDefaultNumberOfSlots;

	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_NUMBEROFSLOTSL_1, "CSimPhoneSecurity: Number of Slots error, unknown IPC");
		User::Leave(KErrNotSupported);
		break;
		}
	return KDefaultNumberOfSlots;
	}

TInt CSimPhoneSecurity::CancelService(const TInt aIpc, const TTsyReqHandle aReqHandle)
/**
 * Cancel an outstanding request.
 * @param aIpc			The IPC number of the request that is to be cancelled.
 * @param aTsyReqHandle	The TSY request handle of the request that is to be cancelled.
 * @param TInt			Standard return value.
 */
	{
	switch(aIpc)
		{
	case EMobilePhoneNotifySecurityCapsChange:
		return NotifySecurityCapsChangeCancel(aReqHandle);

	case EMobilePhoneGetLockInfo:
		return GetLockInfoCancel(aReqHandle);

	case EMobilePhoneNotifyLockInfoChange:
		return NotifyLockInfoChangeCancel(aReqHandle);

	case EMobilePhoneSetLockSetting:
		return SetLockSettingCancel(aReqHandle);

	case EMobilePhoneGetSecurityCodeInfo:
		return GetSecurityCodeInfoCancel(aReqHandle);
		
	case EMobilePhoneNotifySecurityCodeInfoChange:
		return NotifySecurityCodeInfoChangeCancel(aReqHandle);
	
	case EMobilePhoneChangeSecurityCode:
		return ChangeSecurityCodeCancel(aReqHandle);

	case EMobilePhoneNotifySecurityEvent:
		return NotifySecurityEventCancel(aReqHandle);

	case EMobilePhoneVerifySecurityCode:
		return VerifySecurityCodeCancel(aReqHandle);
	default:
		break;
		}
	return KErrNone;
	}


const CTestConfigSection* CSimPhoneSecurity::CfgFile()
/**
* Returns a pointer to the config file section
*
* @return CTestConfigSection a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_CFGFILE_1, ">>CSimPhoneSecurity::CfgFile");
	return iPhone->CfgFile();
	}


TInt CSimPhoneSecurity::GetSecurityCaps(const TTsyReqHandle aReqHandle, TUint32* aCaps)
	{
		//Support all the security features that can be configured using config.txt
		*aCaps= ( RMobilePhone::KCapsLockPhone |
				  RMobilePhone::KCapsLockICC |
				  RMobilePhone::KCapsAccessPin1 |
				  RMobilePhone::KCapsAccessPin2 |
				  RMobilePhone::KCapsAccessPhonePassword |
				  RMobilePhone::KCapsAccessSPC |
				  RMobilePhone::KCapsAccessHiddenKey |
				  RMobilePhone::KCapsAccessUSIMAppPin |
				  RMobilePhone::KCapsAccessUSIMAppSecondPin |
				  RMobilePhone::KCapsAccessUniversalPin );  
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}

TInt CSimPhoneSecurity::NotifySecurityCapsChange(const TTsyReqHandle aReqHandle, TUint32* /*aCaps*/)
	{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
	}

TInt CSimPhoneSecurity::NotifySecurityCapsChangeCancel(const TTsyReqHandle aReqHandle)
	{
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}

TInt CSimPhoneSecurity::GetLockInfo(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneLock* aLock, TDes8* aLockInfo)
	{
		RMobilePhone::TMobilePhoneLockInfoV1Pckg* lockPckg = (RMobilePhone::TMobilePhoneLockInfoV1Pckg*) aLockInfo;
		RMobilePhone::TMobilePhoneLockInfoV1& lock = (*lockPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(lock);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}

		switch(*aLock)
		{
		case RMobilePhone::ELockICC:
			lock.iStatus=iSecStatus.iPin1;
			lock.iSetting=iSecSetting.iPin1;
			break;
		case RMobilePhone::ELockPin2:
			lock.iStatus=iSecStatus.iPin2;
			lock.iSetting=iSecSetting.iPin2;
			break;
		case RMobilePhone::ELockHiddenKey:
			lock.iStatus=iSecStatus.iPhBkHiddenKey;
			lock.iSetting=iSecSetting.iPhBkHiddenKey;
			break;
		case RMobilePhone::ELockUSimApp:
			lock.iStatus=iSecStatus.iUSimAppPin;
			lock.iSetting=iSecSetting.iUSimAppPin;
			break;
		case RMobilePhone::ELockSecondUSimApp:
			lock.iStatus=iSecStatus.iSecondUSimAppPin;
			lock.iSetting=iSecSetting.iSecondUSimAppPin;
			break;
		case RMobilePhone::ELockUniversalPin:
			lock.iStatus=iSecStatus.iUniversalPin;
			lock.iSetting=iSecSetting.iUniversalPin;
			break;

		case RMobilePhone::ELockNam:
			iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
			return KErrNone;	
				
//		Not supported!
//		case RMobilePhone::ELockPhoneDevice:
//		case RMobilePhone::ELockPhoneToIcc:
//		case RMobilePhone::ELockPhoneToFirstIcc:
//		case RMobilePhone::ELockOTA:	
		default:
			iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
			return KErrNone;	
		}
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}

TInt CSimPhoneSecurity::GetLockInfoCancel(const TTsyReqHandle aReqHandle)
	{
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}

TInt CSimPhoneSecurity::NotifyLockInfoChange(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneLock* aLock, TDes8* aLockInfo)
	{
	RMobilePhone::TMobilePhoneLockInfoV1Pckg* lockPckg = (RMobilePhone::TMobilePhoneLockInfoV1Pckg*) aLockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1& lock = (*lockPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(lock);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifyLock.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	if (iNotifySettingsTimer->iIndex <  iICCSettingProfile->Count())
		{
		TICCSettingEntry entry = iICCSettingProfile->At(iNotifySettingsTimer->iIndex);
		if(iNotifySettingsTimer->iTimerStarted)
			{
			delete iNotifySettingsTimer->iTimer;
			iNotifySettingsTimer->iTimer = NULL;
			TRAPD(err,iNotifySettingsTimer->iTimer=CSimTimer::NewL(iPhone));
			if (err!=KErrNone)
				return (err);
			}
		iNotifySettingsTimer->iTimer->Start(entry.iDuration, this, ETimerIdSecurityNotifyLockInfo);
		iNotifySettingsTimer->iTimerStarted = ETrue;
		}
	
	iNotifyLock.iNotifyPending=ETrue;
	iNotifyLock.iNotifyHandle=aReqHandle;
	iNotifyLock.iNotifyData=&lock;
	iNotifyLock.iNotifyDataXtra=aLock;
	return KErrNone;
	}

TInt CSimPhoneSecurity::NotifyLockInfoChangeCancel(const TTsyReqHandle aReqHandle)
	{
	if(iNotifyLock.iNotifyPending)
		{
		iNotifyLock.iNotifyPending=EFalse;
		iPhone->ReqCompleted(aReqHandle,KErrCancel);
		return KErrNone;
		}	
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSecurity::SetLockSetting(const TTsyReqHandle aTsyReqHandle,
									   RMobilePhone::TMobilePhoneLock* aLock,
									   RMobilePhone::TMobilePhoneLockSetting* aSetting)
	{
	// create dummy RMobilePhone::TMobilePhoneLockInfoV1 for
	// UpdateLockInfo and CompleteNotifyLockInfoChange
	RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(lockInfo);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	lockInfo.iSetting = *aSetting;

	// set the iStatus of our local lock info to whatever the current
	// lock info iStatus is set to. The reason is because the intent of
	// SetLockSetting is to set the iSetting value of the lock while
	// leaving the iStatus value untouched
	switch(*aLock)
		{
		case RMobilePhone::ELockICC:
			lockInfo.iStatus=iSecStatus.iPin1;
			break;
		case RMobilePhone::ELockPin2:
			lockInfo.iStatus=iSecStatus.iPin2;
			break;
		case RMobilePhone::ELockHiddenKey:
			lockInfo.iStatus=iSecStatus.iPhBkHiddenKey;
			break;
		case RMobilePhone::ELockUSimApp:
			lockInfo.iStatus=iSecStatus.iUSimAppPin;
			break;
		case RMobilePhone::ELockSecondUSimApp:
			lockInfo.iStatus=iSecStatus.iSecondUSimAppPin;
			break;
		case RMobilePhone::ELockUniversalPin:
			lockInfo.iStatus=iSecStatus.iUniversalPin;
			break;

		case RMobilePhone::ELockNam:
			iPhone->ReqCompleted(aTsyReqHandle,KErrNotSupported);
			return KErrNone;	
				
//		Not supported!
//		case RMobilePhone::ELockPhoneDevice:
//		case RMobilePhone::ELockPhoneToIcc:
//		case RMobilePhone::ELockPhoneToFirstIcc:
//		case RMobilePhone::ELockOTA:
		default:
			iPhone->ReqCompleted(aTsyReqHandle,KErrNotSupported);
			return KErrNone;
		}

	err = UpdateLockInfo(*aLock, lockInfo);
	if(err)
		{
		iPhone->ReqCompleted(aTsyReqHandle,err);
		return KErrNone;	
		}

	CompleteNotifyLockInfoChange(*aLock, lockInfo);		

	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSecurity::SetLockSettingCancel(const TTsyReqHandle aReqHandle)
	{
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}

TInt CSimPhoneSecurity::GetSecurityCodeInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode,TDes8* aSecurityCodeInfo)
/**
Handle GetSecurityCodeInfo request

@param aTsyReqHandle		TSY handle associated with this request
@param aSecurityCode		Security code to get info on
@param aSecurityCodeInfo	The TMobilePhoneSecurityCodeInfoV5 that will be populated with the
							security code information
@return KErrNone
*/
	{
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg* securityCodeInfoPckg = 
		reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg*> (aSecurityCodeInfo);
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5& securityCodeInfo = (*securityCodeInfoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(securityCodeInfo);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	switch(*aSecurityCode)
		{
		case RMobilePhone::ESecurityCodePin1:
			securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPin1;
			break;
		case RMobilePhone::ESecurityCodePin2:
			securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPin2;
			break;
		case RMobilePhone::ESecurityCodePuk1:
			securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPuk1;
			break;
		case RMobilePhone::ESecurityCodePuk2:
			securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPuk2;
			break;
		default:
			iPhone->ReqCompleted(aTsyReqHandle,KErrNotFound);
			return KErrNone;
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CSimPhoneSecurity::GetSecurityCodeInfoCancel(const TTsyReqHandle aTsyReqHandle)
/**
Retrieve a security code information

@param aTsyReqHandle		TSY handle associated with this request
@return KErrNone
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSecurity::NotifySecurityCodeInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode,TDes8* aSecurityCodeInfo)
/**
Record a client's interest in being notified when information of asecurity code changes.

@param aTsyReqHandle		TSY handle associated with this request
@param aSecurityCode		pointer that will be populated with the security code that changed
@param aSecurityCodeInfo	The TMobilePhoneSecurityCodeInfoV5 that will be populated with the
							security code information
@return KErrNone
*/
	{
	__ASSERT_ALWAYS(!iNotifySecurityCodeInfo.iNotifyPending,SimPanic(ENotificationAlreadyPending));

	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg* securityCodeInfoPckg = 
		reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg*> (aSecurityCodeInfo);
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5& securityCodeInfo = (*securityCodeInfoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(securityCodeInfo);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	iNotifySecurityCodeInfo.iNotifyPending=ETrue;
	iNotifySecurityCodeInfo.iNotifyHandle=aTsyReqHandle;
	iNotifySecurityCodeInfo.iNotifyData=&securityCodeInfo;
	iNotifySecurityCodeInfo.iNotifyDataXtra=aSecurityCode;
	return KErrNone;
	}
	
TInt CSimPhoneSecurity::NotifySecurityCodeInfoChangeCancel(const TTsyReqHandle aReqHandle)
/**
Cancel a client's interest in being notified when the RMobileCall capabilities change.

@param aTsyReqHandle		TSY handle associated with this request
@return KErrNone
*/
	{
	if(iNotifySecurityCodeInfo.iNotifyPending)
		{
		iNotifySecurityCodeInfo.iNotifyPending=EFalse;
		iPhone->ReqCompleted(aReqHandle,KErrCancel);
		return KErrNone;
		}	
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CSimPhoneSecurity::ChangeSecurityCode(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aType,const TDes8* aChange)
	{
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV1>* passPckg= (TPckg<RMobilePhone::TMobilePhonePasswordChangeV1>*)aChange;
	RMobilePhone::TMobilePhonePasswordChangeV1& pass = (*passPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(pass);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	switch(*aType)
		{
		case RMobilePhone::ESecurityCodePin1:
			if(iSecCodes.iPin1 == pass.iOldPassword)
				{
				iSecCodes.iPin1.Copy(pass.iNewPassword);
				Pin1Success();
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				Pin1Failure();
				return KErrNone;
				}
			break;
		case RMobilePhone::ESecurityCodePin2:
			if(iSecCodes.iPin2==pass.iOldPassword)
				{
				iSecCodes.iPin2.Copy(pass.iNewPassword);
				Pin2Success();
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				Pin2Failure();
				return KErrNone;
				}
			break;
		case RMobilePhone::ESecurityCodePuk1:
			if(iSecCodes.iPuk1==pass.iOldPassword)
				{
				iSecCodes.iPuk1.Copy(pass.iNewPassword);
				Puk1Success();
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				Puk1Failure();
				return KErrNone;
				}
			break;
		case RMobilePhone::ESecurityCodePuk2:
			if(iSecCodes.iPuk2==pass.iOldPassword)
				{
				iSecCodes.iPuk2.Copy(pass.iNewPassword);
				Puk2Success();
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				Puk2Failure();
				return KErrNone;
				}
			break;
		case RMobilePhone::ESecurityHiddenKey:
			if(iSecCodes.iPhBkHiddenKey==pass.iOldPassword)
				iSecCodes.iPhBkHiddenKey.Copy(pass.iNewPassword);
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}
			break;
		case RMobilePhone::ESecurityUSIMAppPin:
			if(iSecCodes.iUSimAppPin==pass.iOldPassword)
				iSecCodes.iUSimAppPin.Copy(pass.iNewPassword);
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}
			break;
		case RMobilePhone::ESecuritySecondUSIMAppPin:
			if(iSecCodes.iSecondUSimAppPin==pass.iOldPassword)
				iSecCodes.iSecondUSimAppPin.Copy(pass.iNewPassword);
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}
			break;
		case RMobilePhone::ESecurityUniversalPin:
			if(iSecCodes.iUniversalPin==pass.iOldPassword)
				iSecCodes.iUniversalPin.Copy(pass.iNewPassword);
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}
			break;
	//  Not supported
	//	case RMobilePhone::ESecurityCodePhonePassword:
	//	case RMobilePhone::ESecurityCodeSPC:
	
		default:
			iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
			return KErrNone;
		}
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSecurity::ChangeSecurityCodeCancel(const TTsyReqHandle aReqHandle)
	{
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}

/*TInt CSimPhoneSecurity::UnlockNam(const TTsyReqHandle aReqHandle, const RMobilePhone::TMobilePassword* aSpc)
	{
	if(iSecStatus.iSPC==RMobilePhone::EStatusUnlocked)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotReady);
		return KErrNone;
		}	
	else if(iSecCodes.iSPC==*aSpc)
		{
		iUnlockNamHandle = aReqHandle;
		iUnlockNamTimer->Start(KNamTimeout,this,ETimerIdSecurityUnlockNam);
		return KErrNone;
		}
	else
		{
		iPhone->ReqCompleted(aReqHandle,KErrAccessDenied);
		return KErrNone;
		}
	}*/
	
void CSimPhoneSecurity::TimerCallBackUnlockNam()
	{
	SecurityEvent(RMobilePhone::ESPCVerified);
	iPhone->ReqCompleted(iUnlockNamHandle,KErrNone);
	}

TInt CSimPhoneSecurity::UnlockNamCancel(const TTsyReqHandle aReqHandle)
	{
	iUnlockNamTimer->Cancel();
	iPhone->ReqCompleted(aReqHandle,KErrCancel);
	return KErrNone;
	}

void CSimPhoneSecurity::TimerCallBackLockNam()
	{
	LockSpc();
	iPhone->ReqCompleted(iLockNamHandle,KErrNone);
	}

TInt CSimPhoneSecurity::LockNamCancel(const TTsyReqHandle aReqHandle)
	{
	iLockNamTimer->Cancel();
	iPhone->ReqCompleted(aReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CSimPhoneSecurity::VerifySecurityCode(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aType, const RMobilePhone::TCodeAndUnblockCode* aCode)
	{
	switch(*aType)
		{
		case RMobilePhone::ESecurityCodePin1:
			if(iSecCodes.iPin1==(*aCode).iCode)
				{
				SecurityEvent(RMobilePhone::EPin1Verified);
				Pin1Success();
				break;
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				Pin1Failure();
				return KErrNone;
				}
		case RMobilePhone::ESecurityCodePin2:
			if(iSecCodes.iPin2==(*aCode).iCode)
				{
				SecurityEvent(RMobilePhone::EPin2Verified);
				Pin2Success();
				break;
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				Pin2Failure();
				return KErrNone;
				}
		case RMobilePhone::ESecurityCodePuk1:
			if(iSecCodes.iPuk1==(*aCode).iUnblockCode)
				{
				SecurityEvent(RMobilePhone::EPuk1Verified);
				Puk1Success();
				break;
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				Puk1Failure();
				return KErrNone;
				}
		case RMobilePhone::ESecurityCodePuk2:
			if(iSecCodes.iPuk2==(*aCode).iUnblockCode)
				{
				SecurityEvent(RMobilePhone::EPuk2Verified);
				Puk2Success();
				break;
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				Puk2Failure();
				return KErrNone;
				}
		case RMobilePhone::ESecurityHiddenKey:
			if(iSecCodes.iPhBkHiddenKey==(*aCode).iCode)
				{
				SecurityEvent(RMobilePhone::EHiddenKeyVerified);
				break;
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}
		case RMobilePhone::ESecurityUSIMAppPin:
			if(iSecCodes.iUSimAppPin==(*aCode).iCode)
				{
				SecurityEvent(RMobilePhone::EUSIMAppPinVerified);
				break;
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrAccessDenied);
				return KErrNone;
				}
		case RMobilePhone::ESecuritySecondUSIMAppPin:
			if(iSecCodes.iSecondUSimAppPin==(*aCode).iCode)
				{
				SecurityEvent(RMobilePhone::ESecondUSIMAppPinVerified);
				break;
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}
		case RMobilePhone::ESecurityUniversalPin:
			if(iSecCodes.iUniversalPin==(*aCode).iCode)
				{
				SecurityEvent(RMobilePhone::EUniversalPinVerified);
				break;
				}
			else
				{
				iPhone->ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}
	//  Not supported
	//	case RMobilePhone::ESecurityCodePhonePassword:
	//	case RMobilePhone::ESecurityCodeSPC:
	
		default:
			iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
			return KErrNone;
		}
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}

TInt CSimPhoneSecurity::VerifySecurityCodeCancel(const TTsyReqHandle aReqHandle)
	{
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}

TInt CSimPhoneSecurity::AbortSecurityCode(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneSecurityCode* /*aType*/)
	{
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
	}


TInt CSimPhoneSecurity::NotifySecurityEvent(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg1)
/**
 * Registers a Client Interest in security event 
 *
 */
	{
	__ASSERT_ALWAYS(!iNotifyEvent.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	TPckg<TUint32>* eventPckg=(TPckg<TUint32>*)aPckg1;
	TUint32& event=(*eventPckg)();

	TInt count = iICCStatusProfile->Count();
	
	if (iNotifySecurityEventsTimer->iIndex < count)
		{
		TICCStatusEntry entry = iICCStatusProfile->At(iNotifySecurityEventsTimer->iIndex);
		if(iNotifySecurityEventsTimer->iTimerStarted)
			{
			delete iNotifySecurityEventsTimer->iTimer;
			iNotifySecurityEventsTimer->iTimer=NULL;
			TRAPD(err,iNotifySecurityEventsTimer->iTimer=CSimTimer::NewL(iPhone));
			if (err!=KErrNone)
				{
				return (err);
				}
			}
		iNotifySecurityEventsTimer->iTimer->Start(entry.iDuration, this, ETimerIdSecurityNotifyEvent);
		iNotifySecurityEventsTimer->iTimerStarted = ETrue;
		}

	iNotifyEvent.iNotifyPending=ETrue;
	iNotifyEvent.iNotifyHandle=aTsyReqHandle;
	iNotifyEvent.iNotifyData=&event;
	
	return KErrNone;
	}

TInt CSimPhoneSecurity::NotifySecurityEventCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * De-registeres the client interest in security events
 *
 */
	{
	if(iNotifyEvent.iNotifyPending)
		{
		iNotifyEvent.iNotifyPending=EFalse;
		iPhone->ReqCompleted(aTsyReqHandle, KErrCancel);
		return KErrNone;
		}
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


void CSimPhoneSecurity::SecurityEvent(RMobilePhone::TMobilePhoneSecurityEvent aEvent)
/** 
 * Hanldes events related to the ICC state (locked/unlocked)
 *
 */
	{
	switch(aEvent)
		{
		case RMobilePhone::EPin1Required:
			LockICC();		
			break;
		case  RMobilePhone::EPin1Verified:
			UnLockICC();
			break;
		case RMobilePhone::EPin2Required:
			LockPIN2();
			break;
		case RMobilePhone::EPin2Verified:
			UnLockPIN2();
			break;
		case RMobilePhone::EHiddenKeyRequired:
			HideEntries();
			break;
		case RMobilePhone::EHiddenKeyVerified:
			ShowHidden();
			break;
		case RMobilePhone::ESPCVerified:
			 return;
				
		case RMobilePhone::ESPCRequired:
			return;
		case RMobilePhone::ENoICCFound:
		case RMobilePhone::EICCTerminated:
		case RMobilePhone::EPhonePasswordRequired:
		case RMobilePhone::EPhonePasswordVerified:
		case RMobilePhone::ESecondUSIMAppPinRequired:
		case RMobilePhone::ESecondUSIMAppPinVerified:
		case RMobilePhone::EUniversalPinRequired:
		case RMobilePhone::EUniversalPinVerified:
			return;
		
		case RMobilePhone::EUSIMAppPinRequired:
		case RMobilePhone::EUSIMAppPinVerified:			
			break;

		case RMobilePhone::ESPCChanged:
			//  FALLTHRU
		default:
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESECURITY_SECURITYEVENT_1, "CSimPhoneSecurity::SecurityEvent %d OOR [%d,%d]",aEvent, RMobilePhone::ENoICCFound, RMobilePhone::ESPCChanged);
			return;
		}
	
	if(iNotifyEvent.iNotifyPending)
		{
		iNotifyEvent.iNotifyPending=EFalse;
		*(RMobilePhone::TMobilePhoneSecurityEvent*)iNotifyEvent.iNotifyData=aEvent;
		iPhone->ReqCompleted(iNotifyEvent.iNotifyHandle, KErrNone);
		}
	}

void CSimPhoneSecurity::TimerCallBack(TInt aId)
/**
 * The timer callback function.  This function will be called when the timer
 * completes.  It indicates a change in ICC state.  So, the new
 * ICC state settings must be loaded into the member variables representing
 * the current settings   Finally, the next timer is started.
 *
 * @param aId	This parameter is unused.  It is only required for CSimXxx classes
 *				that have more than one timer instance and need to identify which
 *				timer has expired.
 */
	{
	if(aId == ETimerIdSecurityNotifyEvent)
		{
		if((iNotifySecurityEventsTimer->iIndex < iICCStatusProfile->Count()) && iNotifyEvent.iNotifyPending)
			{
			TICCStatusEntry entry = iICCStatusProfile->At(iNotifySecurityEventsTimer->iIndex);
			SecurityEvent(entry.iICCStatus);
			iNotifySecurityEventsTimer->iIndex++;
			}
		}
	else if(aId == ETimerIdSecurityNotifyLockInfo)
		{
		if((iNotifySettingsTimer->iIndex < iICCSettingProfile->Count()) &&	iNotifyLock.iNotifyPending)
			{
			TICCSettingEntry entry = iICCSettingProfile->At(iNotifySettingsTimer->iIndex);
			UpdateLockInfo(entry.iLock, entry.iLockInfo);
			iNotifySettingsTimer->iIndex++;
			iNotifySettingsTimer->iTimerStarted = EFalse;
			CompleteNotifyLockInfoChange(entry.iLock, entry.iLockInfo);			
			}
		}

	}

TBool CSimPhoneSecurity::IsICCLocked()
	{
	if((iSecStatus.iPin1==RMobilePhone::EStatusLocked) &&
	   (iSecSetting.iPin1==RMobilePhone::ELockSetEnabled)) 
			return ETrue;
	
	 return EFalse;
	}
	
TBool CSimPhoneSecurity::IsPIN2Locked()
	{
	if((iSecStatus.iPin2==RMobilePhone::EStatusLocked) &&
	   (iSecSetting.iPin2==RMobilePhone::ELockSetEnabled)) 
			return ETrue;
	
	 return EFalse;
	}	

void CSimPhoneSecurity::LockICC()
	{
	iSecStatus.iPin1=RMobilePhone::EStatusLocked;
	return;
	}

void CSimPhoneSecurity::UnLockICC()
	{
	iSecStatus.iPin1=RMobilePhone::EStatusUnlocked;
	return;
	}
	
void CSimPhoneSecurity::LockPIN2()
	{
	iSecStatus.iPin2=RMobilePhone::EStatusLocked;
	return;
	}

void CSimPhoneSecurity::UnLockPIN2()
	{
	iSecStatus.iPin2=RMobilePhone::EStatusUnlocked;
	return;
	}

TBool CSimPhoneSecurity::IsHiddenEnabled()
	{
	if((iSecStatus.iPhBkHiddenKey==RMobilePhone::EStatusLocked) &&
	   (iSecSetting.iPhBkHiddenKey==RMobilePhone::ELockSetEnabled)) 
		return ETrue;
	
	 return EFalse;
	}

void CSimPhoneSecurity::HideEntries()
	{
	iSecStatus.iPhBkHiddenKey=RMobilePhone::EStatusLocked;
	return;
	}

void CSimPhoneSecurity::ShowHidden()
	{
	iSecStatus.iPhBkHiddenKey=RMobilePhone::EStatusUnlocked;
	return;
	}
	
void CSimPhoneSecurity::LockSpc()
	{
	if (iNotifyLock.iNotifyPending)
		{
		// there is a notification waiting
		RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;

		lockInfo.iStatus = RMobilePhone::EStatusLocked;
		lockInfo.iSetting = RMobilePhone::ELockSetEnabled;
		CompleteNotifyLockInfoChange(RMobilePhone::ELockNam, lockInfo);
		}
	iSecStatus.iSPC=RMobilePhone::EStatusLocked;
	return;
	}

void CSimPhoneSecurity::UnLockSpc()
	{
	if (iNotifyLock.iNotifyPending)
		{
		RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;

		lockInfo.iStatus = RMobilePhone::EStatusUnlocked;
		lockInfo.iSetting = RMobilePhone::ELockSetEnabled;
		// there is a notification waiting
		CompleteNotifyLockInfoChange(RMobilePhone::ELockNam, lockInfo);
		}
	iSecStatus.iSPC=RMobilePhone::EStatusUnlocked;
	return;
	}

CSimPhoneSecurity::CIccTimer::CIccTimer()
	: iTimerStarted(EFalse)
	, iIndex(0)
	, iTimer(NULL)
	{
	}

CSimPhoneSecurity::CIccTimer::~CIccTimer()
	{
	if(iTimer)
		iTimer->Cancel();
	delete iTimer; 
	}
	
CSimPhoneSecurity::CIccTimer* CSimPhoneSecurity::CIccTimer::NewL()
	{
	CSimPhoneSecurity::CIccTimer* self = new(ELeave)CSimPhoneSecurity::CIccTimer;
	return self;
	}
	
TInt CSimPhoneSecurity::UpdateLockInfo(RMobilePhone::TMobilePhoneLock aLock, const RMobilePhone::TMobilePhoneLockInfoV1& aLockInfo)
	{
	TInt err = KErrNone;
	switch(aLock)
		{
		case RMobilePhone::ELockICC:
			iSecSetting.iPin1 = aLockInfo.iSetting;	
			iSecStatus.iPin1 = aLockInfo.iStatus;	
			break;
		case RMobilePhone::ELockPin2:
			iSecSetting.iPin2 = aLockInfo.iSetting;
			iSecStatus.iPin2 = aLockInfo.iStatus;	
			break;
		case RMobilePhone::ELockHiddenKey:
			iSecSetting.iPhBkHiddenKey = aLockInfo.iSetting;
			iSecStatus.iPhBkHiddenKey = aLockInfo.iStatus;	
			break;
		case RMobilePhone::ELockUSimApp:
			iSecSetting.iUSimAppPin = aLockInfo.iSetting;
			iSecStatus.iUSimAppPin = aLockInfo.iStatus;	
			break;
		case RMobilePhone::ELockSecondUSimApp:
			iSecSetting.iSecondUSimAppPin = aLockInfo.iSetting;
			iSecStatus.iSecondUSimAppPin = aLockInfo.iStatus;	
			break;
		case RMobilePhone::ELockUniversalPin:
			iSecSetting.iUniversalPin = aLockInfo.iSetting;
			iSecStatus.iUniversalPin = aLockInfo.iStatus;	
			break;

		case RMobilePhone::ELockNam:
			err = KErrNotSupported;	
		
			break;

//		Not supported!
//		case RMobilePhone::ELockPhoneDevice:
//		case RMobilePhone::ELockPhoneToIcc:
//		case RMobilePhone::ELockPhoneToFirstIcc:
//		case RMobilePhone::ELockOTA:
		
		default:
			err = KErrNotSupported;	
		}
	return err;
	}

void CSimPhoneSecurity::Pin1Failure()
/**
Handle wrong PIN1 entry. Decrement the entry counter, lock the corresponding lock after the maximum 
of entry. Complete the Security Code Info Change Notification if the counter has changed
*/
	{
	if (iSecurityCodeRemainigAttempts.iPin1 > 0)
		{
		iSecurityCodeRemainigAttempts.iPin1--;
		if (iSecurityCodeRemainigAttempts.iPin1 == 0)
			{
			iSecStatus.iPin1 = RMobilePhone::EStatusBlocked;
			RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;

			lockInfo.iSetting = iSecSetting.iPin1;
			lockInfo.iStatus = iSecStatus.iPin1;
			CompleteNotifyLockInfoChange(RMobilePhone::ELockICC,lockInfo);
			}
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
		// Simulated version checking is omitted in this case - function is called after a ReqCompleted

		securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPin1;
		CompleteNotifySecurityCodeInfoChange(RMobilePhone::ESecurityCodePin1,securityCodeInfo);
		}
	}

void CSimPhoneSecurity::Pin1Success()
/**
Handle correct PIN1 entry. if require, Reset the entry counter and complete the Security Code Info 
Change Notification.
*/
	{
	if (iSecurityCodeRemainigAttempts.iPin1 <  iSecurityCodeDefaultRemainigAttempts.iPin1)
		{
		iSecurityCodeRemainigAttempts.iPin1 = iSecurityCodeDefaultRemainigAttempts.iPin1;
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
		// Simulated version checking is omitted in this case - function is called after a ReqCompleted

		securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPin1;
		CompleteNotifySecurityCodeInfoChange(RMobilePhone::ESecurityCodePin1,securityCodeInfo);
		}
	}

void CSimPhoneSecurity::Pin2Failure()
/**
Handle wrong PIN2 entry. Decrement the entry counter, lock the corresponding lock after the maximum 
of entry. Complete the Security Code Info Change Notification if the counter has changed
*/
	{
	if (iSecurityCodeRemainigAttempts.iPin2 > 0)
		{
		iSecurityCodeRemainigAttempts.iPin2--;
		if (iSecurityCodeRemainigAttempts.iPin2 == 0)
			{
			iSecStatus.iPin2 = RMobilePhone::EStatusBlocked;
			RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;

			lockInfo.iSetting = iSecSetting.iPin2;
			lockInfo.iStatus = iSecStatus.iPin2;
			CompleteNotifyLockInfoChange(RMobilePhone::ELockPin2,lockInfo);
			}
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
		// Simulated version checking is omitted in this case - function is called after a ReqCompleted

		securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPin2;
		CompleteNotifySecurityCodeInfoChange(RMobilePhone::ESecurityCodePin2,securityCodeInfo);
		}
	}

void CSimPhoneSecurity::Pin2Success()
/**
Handle correct PIN2 entry. if require, Reset the entry counter and complete the Security Code Info 
Change Notification.
*/
	{
	if (iSecurityCodeRemainigAttempts.iPin2 <  iSecurityCodeDefaultRemainigAttempts.iPin2)
		{
		iSecurityCodeRemainigAttempts.iPin2 = iSecurityCodeDefaultRemainigAttempts.iPin2;
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
		// Simulated version checking is omitted in this case - function is called after a ReqCompleted

		securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPin2;
		CompleteNotifySecurityCodeInfoChange(RMobilePhone::ESecurityCodePin2,securityCodeInfo);
		}
	}

void CSimPhoneSecurity::Puk1Failure()
/**
Handle wrong PUK1 entry. Decrement the entry counter. Complete the Security Code Info Change Notification 
if the counter has changed
*/
	{
	if (iSecurityCodeRemainigAttempts.iPuk1 > 0)
		{
		iSecurityCodeRemainigAttempts.iPuk1--;
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
		// Simulated version checking is omitted in this case - function is called after a ReqCompleted

		securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPuk1;
		CompleteNotifySecurityCodeInfoChange(RMobilePhone::ESecurityCodePuk1,securityCodeInfo);
		}
	}

void CSimPhoneSecurity::Puk1Success()
/**
Handle correct PUK1 entry. if require, Reset the entry counter and complete the Security Code Info 
Change Notification.
*/
	{
	if (iSecurityCodeRemainigAttempts.iPuk1 <  iSecurityCodeDefaultRemainigAttempts.iPuk1)
		{
		iSecurityCodeRemainigAttempts.iPuk1 = iSecurityCodeDefaultRemainigAttempts.iPuk1;
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
		// Simulated version checking is omitted in this case - function is called after a ReqCompleted

		securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPuk1;
		CompleteNotifySecurityCodeInfoChange(RMobilePhone::ESecurityCodePuk1,securityCodeInfo);
		}
	}

void CSimPhoneSecurity::Puk2Failure()
/**
Handle wrong PUK2 entry. Decrement the entry counter. Complete the Security Code Info Change Notification 
if the counter has changed
*/
	{
	if (iSecurityCodeRemainigAttempts.iPuk2 > 0)
		{
		iSecurityCodeRemainigAttempts.iPuk2--;
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
		// Simulated version checking is omitted in this case - function is called after a ReqCompleted

		securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPuk2;
		CompleteNotifySecurityCodeInfoChange(RMobilePhone::ESecurityCodePuk2,securityCodeInfo);
		}
	}

void CSimPhoneSecurity::Puk2Success()
/**
Handle correct PUK2 entry. if require, Reset the entry counter and complete the Security Code Info 
Change Notification.
*/
	{
	if (iSecurityCodeRemainigAttempts.iPuk2 <  iSecurityCodeDefaultRemainigAttempts.iPuk2)
		{
		iSecurityCodeRemainigAttempts.iPuk2 = iSecurityCodeDefaultRemainigAttempts.iPuk2;
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
		// Simulated version checking is omitted in this case - function is called after a ReqCompleted

		securityCodeInfo.iRemainingEntryAttempts = iSecurityCodeRemainigAttempts.iPuk2;
		CompleteNotifySecurityCodeInfoChange(RMobilePhone::ESecurityCodePuk2,securityCodeInfo);
		}
	}
	
void CSimPhoneSecurity::CompleteNotifyLockInfoChange(RMobilePhone::TMobilePhoneLock aLock, const RMobilePhone::TMobilePhoneLockInfoV1& aLockInfo)
	{
	if (iNotifyLock.iNotifyPending)
		{
		*(RMobilePhone::TMobilePhoneLockInfoV1*)iNotifyLock.iNotifyData=aLockInfo;
		*(RMobilePhone::TMobilePhoneLock*)iNotifyLock.iNotifyDataXtra=aLock;
		iNotifyLock.iNotifyPending=EFalse;
		iPhone->ReqCompleted(iNotifyLock.iNotifyHandle,KErrNone);
		}
	}

void CSimPhoneSecurity::CompleteNotifySecurityCodeInfoChange(RMobilePhone::TMobilePhoneSecurityCode aSecurityCode, const RMobilePhone::TMobilePhoneSecurityCodeInfoV5& aSecurityCodeInfo)
/**
Complete a Security code info notification if pending.

@param aSecurityCode 		security code that must be notified
@param aSecurityCodeInfo	security code information for the notification
*/
	{
	if (iNotifySecurityCodeInfo.iNotifyPending)
		{
		*reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCodeInfoV5*>(iNotifySecurityCodeInfo.iNotifyData)=aSecurityCodeInfo;
		*reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCode*>(iNotifySecurityCodeInfo.iNotifyDataXtra)=aSecurityCode;
		iNotifySecurityCodeInfo.iNotifyPending=EFalse;
		iPhone->ReqCompleted(iNotifySecurityCodeInfo.iNotifyHandle,KErrNone);
		}
	}

