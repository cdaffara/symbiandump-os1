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
// Battery charging information.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimBatteryChargerTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimBatteryCharger.h"
#include "CSimPhone.h"

const TInt KChargerGranularity=5;		// < Granularity for Battery Charger list array


CSimBatteryCharger* CSimBatteryCharger::NewL(CSimPhone* aPhone)
/**
 * Standard two-phase constructor.
 * @param aPhone				The parent phone object.
 * @return CSimBatteryCharger		The new Ondicator class class.
 */
	{
	CSimBatteryCharger* self=new(ELeave) CSimBatteryCharger(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimBatteryCharger::CSimBatteryCharger(CSimPhone* aPhone)
		: iPhone(aPhone)
/**
 * Trivial first phase construction.
 * @param aPhone				The parent phone object.
 */
	{
	}


void CSimBatteryCharger::ConstructL()
/**
 * Second phase construction.  Create instances of the necessary heap-based
 * objects and read in the Battery Charger information from the configuration file.
 * Finally, if any Battery Charger tags have been read, the initial values
 * will be loaded and the timer started.
 *
 * Entries in the configuration file will take the following format:
 * "BatteryCharger= <duration>, <Status> <battery level> <error code>"
 * A number of these entries may be included to create an battery charger profile
 * for the duration of the test.
 */
	{
	iTimer=CSimTimer::NewL(iPhone);
	iBatteryChargerInfo=new(ELeave) CArrayFixFlat<TBatteryChargerInfo>(KChargerGranularity);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_CONSTRUCTL_1, "Starting to parse Battery Charger config parameters...");
	TInt count=CfgFile()->ItemCount(KBatteryCharger);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KBatteryCharger,i);
		if(!item)
			break;

		TInt duration, status, level, error;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KBatteryCharger);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,status);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element STATUS returned %d (element no. %d) from tag %s.",ret,1,KBatteryCharger);
			continue;
			}
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,level);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element LEVEL returned %d (element no. %d) from tag %s.",ret,2,KBatteryCharger);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,error);
		if(ret!=KErrNone)
			error = KErrNone;

		TBatteryChargerInfo chargerInfo;
		chargerInfo.iDuration=duration;
		chargerInfo.iStatus = RMobilePhone::TMobilePhoneBatteryStatus(status);
		chargerInfo.iChargeLevel = level;
		chargerInfo.iError = error;
		iBatteryChargerInfo->AppendL(chargerInfo);
		}
	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_CONSTRUCTL_5, "Finished parsing Battery Charger config parameters...%d items found",count);

	if(iBatteryChargerInfo->Count()!=0)
		{
		iBatteryChargerIndex = 0;
		iCurrentBatteryCharger = iBatteryChargerInfo->At(0).iChargeLevel;
		iCurrentStatus = iBatteryChargerInfo->At(0).iStatus;
		iCurrentErr = iBatteryChargerInfo->At(0).iError;
		iTimer->Start(iBatteryChargerInfo->At(0).iDuration,this);
		}
	}




CSimBatteryCharger::~CSimBatteryCharger()
/**
 * Standard destructor.  Destroy the heap-based object owned by this object.
 */
	{
	if (iTimer)
		delete iTimer;
	
	if (iBatteryChargerInfo)
		{
		iBatteryChargerInfo->Delete(0,iBatteryChargerInfo->Count());
		delete iBatteryChargerInfo;//todo check ptr
		}
	}


TInt CSimBatteryCharger::GetBatteryCaps(TTsyReqHandle aReqHandle,TDes8* aCaps)
/**
 * Retrieve Battery Charger capability information.  This function completes the
 * client's request synchronously.  If the configuration file contains any 
 * indicator profile information, then it indicates support for indicator
 * requests, otherwise it does not.
 *
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the action caps(methods supported)
 *						.
 * @param aPckg2		the second parameter package. This will be populated with the Indicator caps(wich indicator(s) are supported)
 * @return TInt			Standard error value.
 */
	{
	TPckg<TUint32>* batteryChargerCapsPckg=(TPckg<TUint32>*)aCaps;
	TUint32& batteryChargerCaps=(*batteryChargerCapsPckg)();

	if(iBatteryChargerInfo->Count()==0)
		{
		batteryChargerCaps=0;	
		}
	else if((iBatteryChargerInfo->Count()-1) <= iBatteryChargerIndex)
		batteryChargerCaps= RMobilePhone::KCapsGetBatteryInfo;
	else
		{
		batteryChargerCaps= RMobilePhone::KCapsGetBatteryInfo | RMobilePhone::KCapsNotifyBatteryInfoChange;
		}
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}


TInt CSimBatteryCharger::GetBatteryInfo(TTsyReqHandle aReqHandle, TDes8* aInfo)
/**
 * Return the current Battery Charger information.  This function completes synchronously.
 * If the configuration file contains any Battery Charger profile information, the
 * request completes successfully, otherwise it completes with KErrNotSupported.
 *
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		This is populated with the Battery Charger flags.
 * @return TInt			Standard error value.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_GETBATTERYINFO_1, ">>CSimBatteryCharger::GetBatteryInfo");
	TPckg<RMobilePhone::TMobilePhoneBatteryInfoV1>* batteryInfoPckg = (TPckg<RMobilePhone::TMobilePhoneBatteryInfoV1>*)aInfo;
	RMobilePhone::TMobilePhoneBatteryInfoV1& batteryInfo = (*batteryInfoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(batteryInfo);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	if(iBatteryChargerInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	batteryInfo.iChargeLevel=iCurrentBatteryCharger;
	batteryInfo.iStatus = iCurrentStatus;
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_GETBATTERYINFO_2, "<<CSimBatteryCharger::GetBatteryInfo with level=%d and status=%d",iCurrentBatteryCharger,iCurrentStatus);
	iPhone->ReqCompleted(aReqHandle,iCurrentErr);
	return KErrNone;
	}

TInt CSimBatteryCharger::NotifyBatteryInfoChange(TTsyReqHandle aReqHandle, TDes8* aInfo)
/**
 * Register a client's interest in being notified when the Battery Charger change.
 * This function records the request's parameters and awaits a change in 
 * Battery Charger before completing.
 *
 * @param aPckg1		The first parameter package.  This is populated with the Battery Charger flags.
 * @return TInt			Standard error value.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_NOTIFYBATTERYINFOCHANGE_1, ">>CSimBatteryCharger::NotifyBatteryInfoChange");
	TPckg<RMobilePhone::TMobilePhoneBatteryInfoV1>* batteryInfoPckg=(TPckg<RMobilePhone::TMobilePhoneBatteryInfoV1>*)aInfo;
	RMobilePhone::TMobilePhoneBatteryInfoV1& batteryInfo=(*batteryInfoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(batteryInfo);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	if(iBatteryChargerInfo->Count()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iBatteryChargerNotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iBatteryChargerNotificationPending=ETrue;
	iBatteryChargerNotificationReqHandle=aReqHandle;
	iBatteryChargerNofificationValue=&batteryInfo;
	return KErrNone;	
	}

void CSimBatteryCharger::NotifyBatteryInfoCancel()
/**
 * Cancel a previous request to be notified of a change in Battery Charger.
 */
	{
	if(iBatteryChargerNotificationPending)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_NOTIFYBATTERYINFOCANCEL_1, "CSimBatteryCharger::NotifyBatteryInfoChange has been cancelled");
		iBatteryChargerNotificationPending=EFalse;
		iPhone->ReqCompleted(iBatteryChargerNotificationReqHandle,KErrCancel);
		}
	else
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_NOTIFYBATTERYINFOCANCEL_2, "CSimBatteryCharger::NotifyBatteryInfoChange was not outstanding and hasn't been cancelled");
	}



void CSimBatteryCharger::TimerCallBack(TInt /*aId*/)
/**
 * The timer callback function.  This function will be called when the timer
 * completes.  It indicates a change in Battery Charger.  So, the new
 * Battery Charger settings must be loaded into the member variables representing
 * the current settings and any pending Battery Charger notification requests must
 * be completed.  Finally, the next timer is started.
 *
 * @param aId	This parameter is unused.  It is only required for CSimXxx classes
 *				that have more than one timer instance and need to identify which
 *				timer has expired.
 */
	{
	iBatteryChargerIndex++;
	if(iBatteryChargerInfo->Count()<=iBatteryChargerIndex)
		return;

	iCurrentBatteryCharger=iBatteryChargerInfo->At(iBatteryChargerIndex).iChargeLevel;
	iCurrentStatus = iBatteryChargerInfo->At(iBatteryChargerIndex).iStatus;
	iCurrentErr = iBatteryChargerInfo->At(iBatteryChargerIndex).iError;

	// Ask phone to check if indicator notification needs to be triggered
	iPhone->CheckIndicatorNotification();

	if(iBatteryChargerNotificationPending)
		{
		iBatteryChargerNotificationPending=EFalse;
		(*iBatteryChargerNofificationValue).iChargeLevel=iCurrentBatteryCharger;
		(*iBatteryChargerNofificationValue).iStatus=iCurrentStatus;
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMBATTERYCHARGER_TIMERCALLBACK_1, "<<CSimBatteryCharger::NotifyBatteryInfoChange with level=%d and status=%d",iCurrentBatteryCharger,iCurrentStatus);
		iPhone->ReqCompleted(iBatteryChargerNotificationReqHandle,iCurrentErr);
		}
	iTimer->Start(iBatteryChargerInfo->At(iBatteryChargerIndex).iDuration,this);
	}

const CTestConfigSection* CSimBatteryCharger::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file section.
 */
	{
	return iPhone->CfgFile();
	}

