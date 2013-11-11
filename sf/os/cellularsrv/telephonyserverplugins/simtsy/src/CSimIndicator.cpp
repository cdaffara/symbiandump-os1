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
// indicators information.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimIndicatorTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimIndicator.h"
#include "CSimPhone.h"


const TInt KIndicatorsGranularity=5;		// < Granularity for indicators list array

CSimIndicator* CSimIndicator::NewL(CSimPhone* aPhone)
/**
 * Standard two-phase constructor.
 * @param aPhone				The parent phone object.
 * @return CSimIndicator		The new Ondicator class class.
 */
	{
	CSimIndicator* self=new(ELeave) CSimIndicator(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimIndicator::CSimIndicator(CSimPhone* aPhone)
		: iPhone(aPhone),iNotificationNumber(0)
/**
 * Trivial first phase construction.
 * @param aPhone				The parent phone object.
 */
	{
	}


void CSimIndicator::ConstructL()
/**
 * Second phase construction.  Create instances of the necessary heap-based
 * objects and read in the indicators information from the configuration file.
 * Finally, if any indicators tags have been read, the initial values
 * will be loaded and the timer started.
 *
 * Entries in the configuration file will take the following format:
 * "Indicator= <duration>, <Flags>"
 * A number of these entries may be included to create an indicator profile
 * for the duration of the test.
 */
	{
	iIndicatorsInfo=new(ELeave) CArrayFixFlat<TPhoneIndicatorsInfo>(KIndicatorsGranularity);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CONSTRUCTL_1, "Starting to parse Indicators config parameters...");
	TInt count=CfgFile()->ItemCount(KIndicators);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KIndicators,i);
		if(!item)
			break;

		TInt number;
		TInt error;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,number);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element NUMBER returned %d (element no. %d) from tag %s.",ret,0,KIndicators);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,error);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element ERROR returned %d (element no. %d) from tag %s.",ret,1,KIndicators);
			continue;
			}
		TPhoneIndicatorsInfo indicInfo;
		indicInfo.iNotificationNumber=number;
		indicInfo.iError=error;
		iIndicatorsInfo->AppendL(indicInfo);
		}
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CONSTRUCTL_4, "Finished parsing Indicators config parameters...%d items found", count);

	iIndicatorCheckPeriod = 1;
	count = CfgFile()->ItemCount(KIndicatorCheckPeriod);
	if (count > 1)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CONSTRUCTL_5, "Warning: Error parsing IndicatorCheckPeriod in config file.");
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CONSTRUCTL_6, "         More than one value found, using default value.");
		}
	else
		{
		item=CfgFile()->Item(KIndicatorCheckPeriod);

		if (item != NULL)
			{
			TInt period;
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,period);
			if(ret!=KErrNone)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CONSTRUCTL_7, "Warning: Error parsing IndicatorCheckPeriod in config file.");
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CONSTRUCTL_8, "         No value for IndicatorCheckPeriod found.");
				}
			else
				{
				iIndicatorCheckPeriod = period;
				}
			}
		}
	
	iCurrentIndicator=IndicatorState();
	iTimer = CSimTimer::NewL(iPhone);
	}


CSimIndicator::~CSimIndicator()
/**
 * Standard destructor.  Destroy the heap-based object owned by this object.
 */
	{
	if (iIndicatorsInfo)
		{
		iIndicatorsInfo->Delete(0,iIndicatorsInfo->Count());
		delete iIndicatorsInfo;//todo check ptr
		}
	delete iTimer;
	}


TInt CSimIndicator::GetIndicatorCaps(TTsyReqHandle aReqHandle,TDes8* aPckg1, TDes8* aPckg2)
/**
 * Retrieve Indicators capability information.  This function completes the
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
	TPckg<TUint32>* indicatorActionCapsPckg=(TPckg<TUint32>*)aPckg1;
	TUint32& indicatorActionCaps=(*indicatorActionCapsPckg)();

	TPckg<TUint32>* indicatorIndCapsPckg=(TPckg<TUint32>*)aPckg2;
	TUint32& indicatorIndCaps=(*indicatorIndCapsPckg)();

	indicatorActionCaps= RMobilePhone::KCapsGetIndicator | RMobilePhone::KCapsNotifyIndicatorChange;
	indicatorIndCaps= RMobilePhone::KIndChargerConnected |	RMobilePhone::KIndNetworkAvailable | RMobilePhone::KIndCallInProgress;
	
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}


TInt CSimIndicator::GetIndicator(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Return the current indicators information.  This function completes synchronously.
 * If the configuration file contains any indicators profile information, the
 * request completes successfully, otherwise it completes with KErrNotSupported.
 *
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		This is populated with the Indicators flags.
 * @return TInt			Standard error value.
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_GETINDICATOR_1, "CSimIndicator::GetIndicator request made: returning %d",iCurrentIndicator);
	TPckg<TUint32>* indicPckg=(TPckg<TUint32>*)aPckg1;
	TUint32& indic=(*indicPckg)();
	
	indic=iCurrentIndicator;
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimIndicator::NotifyIndicatorChange(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Register a client's interest in being notified when the Indicators change.
 * This function records the request's parameters and awaits a change in 
 * indicators before completing.
 *
 * @param aPckg1		The first parameter package.  This is populated with the Indicators flags.
 * @return TInt			Standard error value.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_NOTIFYINDICATORCHANGE_1, "CSimIndicator::NotifyIndicatorChange notification posted");
	TPckg<TUint32>* indicPckg=(TPckg<TUint32>*)aPckg1;
	TUint32& indic=(*indicPckg)();

	__ASSERT_ALWAYS(!iIndicatorsChangeNotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iIndicatorsChangeNotificationPending=ETrue;
	iIndicatorsChangeNotificationReqHandle=aReqHandle;
	iIndicatorsChangeNofificationValue=&indic;
	if (!(iTimer->Running()))
		iTimer->Start(iIndicatorCheckPeriod, this);
	return KErrNone;	
	}

void CSimIndicator::NotifyIndicatorChangeCancel()
/**
 * Cancel a previous request to be notified of a change in indicators.
 */
	{
	if(iIndicatorsChangeNotificationPending)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_NOTIFYINDICATORCHANGECANCEL_1, "CSimIndicator::NotifyIndicatorChange notification cancelled");
		iIndicatorsChangeNotificationPending=EFalse;
		iPhone->ReqCompleted(iIndicatorsChangeNotificationReqHandle,KErrCancel);
		}
	}


void CSimIndicator::CheckNotification()
/**
 * The timer callback function.  This function will be called when the timer
 * completes.  It indicates a change in indicators.  So, the new
 * indicators settings must be loaded into the member variables representing
 * the current settings and any pending indicators notification requests must
 * be completed.  Finally, the next timer is started.
 *
 * @param aId	This parameter is unused.  It is only required for CSimXxx classes
 *				that have more than one timer instance and need to identify which
 *				timer has expired.
 */
	{
	TUint32 newIndicator=IndicatorState();
	if(newIndicator!=iCurrentIndicator)
		{
		iCurrentIndicator=newIndicator;
		if(iIndicatorsChangeNotificationPending)
			{
// First see if this notification should be completed with an error...
			++iNotificationNumber;
			TInt ret=KErrNone;
			for(TInt i=0;i<iIndicatorsInfo->Count();i++)
				{
				if (iIndicatorsInfo->At(i).iNotificationNumber==iNotificationNumber)
					ret=iIndicatorsInfo->At(i).iError;
				}

// Trigger notification with appropriate data and result codes
			iIndicatorsChangeNotificationPending=EFalse;
			*iIndicatorsChangeNofificationValue=iCurrentIndicator;
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMINDICATOR_CHECKNOTIFICATION_1, "CSimIndicator::NotifyIndicatorChange triggered: returned %d",iCurrentIndicator);
			iPhone->ReqCompleted(iIndicatorsChangeNotificationReqHandle,ret);
			}
		}
	}

TUint32 CSimIndicator::IndicatorState()
/**
 * Construct and return the current value for the indicator.
 *
 * @return TUint32 Current indicator value.
 */
	{
	TUint indicator=0;

	RMobilePhone::TMobilePhoneBatteryStatus state = iPhone->BatteryStatus();

	if (state == RMobilePhone::EBatteryConnectedButExternallyPowered)
		indicator |= RMobilePhone::KIndChargerConnected;

	RPhone::TMode mode = iPhone->Mode();

	if((mode == RPhone::EModeEstablishingLink) || (mode == RPhone::EModeOnlineData) ||
	   (mode == RPhone::EModeOnlineCommand))
		indicator |= RMobilePhone::KIndCallInProgress;

	RMobilePhone::TMobilePhoneRegistrationStatus nkStatus = iPhone->RegistrationStatus();
	if ((nkStatus == RMobilePhone::ERegisteredOnHomeNetwork) ||
		(nkStatus == RMobilePhone::ERegisteredRoaming) ||
		(nkStatus == RMobilePhone::ERegisteredBusy))
		indicator |= RMobilePhone::KIndNetworkAvailable;

	return indicator;
	}

const CTestConfigSection* CSimIndicator::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file section.
 */
	{
	return iPhone->CfgFile();
	}

void CSimIndicator::TimerCallBack(TInt /*aId*/)
	{
	CheckNotification();
	}
