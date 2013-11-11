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
// Implements the Own Number store manipulation code.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimONStoreTraces.h"
#endif

#include "CSimONStore.h"
#include "CSimPhone.h"
#include <testconfigfileparser.h>

//
// CSimONStore
//
void CSimONStore::ClosePhone(TAny* aObj)
/**
 * A utility function for cleaning up the stack.
 */
	{
	((CObject*)aObj)->Close();
	}

CSimONStore* CSimONStore::NewL(CSimPhone* aPhone, TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen)
/**
 * Standard two phase constructor.
 * @param aPhone			The phone object from which this Own Number store was opened.
 * @param aName				The name of the created Own Number store.
 * @param aMaxNumSlots		The maximum number of slots in the Own Number store.
 * @return CSimONStore*	The newly created object.
 */
	{
	CSimONStore* store=new(ELeave) CSimONStore(aPhone);
	TCleanupItem newObjClose(ClosePhone,store);
	CleanupStack::PushL(newObjClose);
	store->ConstructL(aMaxNumSlots,aMaxNumLen,aMaxTextLen);
	CleanupStack::Pop();
	return store;
	}

CSimONStore::CSimONStore(CSimPhone* aPhone)
		: CSimPhone(iDummyPhoneBaseRef), iPhone(aPhone)
/**
 * Trivial first phase constructor.
 * @param aPhone	The phone object from which this Own Number store was opened.
 */
	{}

void CSimONStore::ConstructL(TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen)
/**
 * Second phase constructor that allocates memory for the Own Number store, batch read buffer and
 * a delayed completion timer.  The constructor also reads the individual and batch read
 * delays from the configuration file.
 * 
 * @param aMaxNumLen	The maximum length of a telephone number.
 * @param aMaxTextLen	The maximum length of an alpha tag.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMONSTORE_CONSTRUCTL_1, "Starting to parse Own Number store additional config parameters...");
	__ASSERT_ALWAYS(aMaxNumLen<=KONMaxTelNumSize,SimPanic(EOwnNumberNameOrNumberTooLarge));
	__ASSERT_ALWAYS(aMaxTextLen<=KONMaxTextSize,SimPanic(EOwnNumberNameOrNumberTooLarge));

	iONStoreEntries=new(ELeave) TONStoreEntry[aMaxNumSlots+1]; //slot 0 is unused
	iONMaxNumSlots=aMaxNumSlots;
	iONMaxTelNumLen=aMaxNumLen;
	iONMaxTextLen=aMaxTextLen;

	iReqTimer=CSimTimer::NewL(iPhone);
	iONRwBuffer=new(ELeave) CPhoneBookBuffer();

	const CTestConfigItem* item=NULL;
	item=CfgFile()->Item(KONPhoneStoreCaps,0);
	if (item)
		{
		TPtrC8 value;
		TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,value);
		if(ret!=KErrNone)
			{
			iONStoreCaps=KDefaultONPhoneStoreCaps;
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMONSTORE_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element VALUE returned %d (element no. %d) from tag %s.",ret,0,KONPhoneStoreCaps);
			}	
		else
			{
			TUint32 intValue;
			TInt ret = AsciiToNum(value, intValue);
			if(ret!=KErrNone)
				iONStoreCaps=KDefaultONPhoneStoreCaps;
			else
				iONStoreCaps = intValue;
			}
		}
	else
		iONStoreCaps=KDefaultONPhoneStoreCaps;


	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMONSTORE_CONSTRUCTL_3, "...Finished parsing Own Number store additional config parameters...");
	}

void CSimONStore::PopulateStoreFromConfigFileL()
/**
 * Populate the Own Number store from information in the configuration file.  This is performed
 * after the standard Own Number store construction in order to prevent reseting the configuation
 * file accessor class' pointers while possibly multiple Own Number stores are created.
 *
 * The store entries comply to the following format:
 * "PhBkStoreEntry = <store name>, <slot number>, <telephone number>, <alphatag>"
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMONSTORE_POPULATESTOREFROMCONFIGFILEL_1, "Starting to read Own Number store entries...");
	iONIndividualPause=CfgFile()->ItemValue(KONStoreIndividualReqPause,KDefaultONStoreIndividualReqPause);

	TInt count=CfgFile()->ItemCount(KONStoreEntry);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KONStoreEntry,i);
		if(!item)
			break;

		TPtrC8 telNum, name;
		TInt index;
		RMobilePhone::TMobileTON typeOfNumber;
		RMobilePhone::TMobileService service; 
		RMobilePhone::TMobilePhoneNetworkMode mode;
		RMobilePhone::TMobileNPI numberPlan;
		ret=GetONEntry(item,0,index,telNum,name,typeOfNumber,service,mode,numberPlan);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMONSTORE_POPULATESTOREFROMCONFIGFILEL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element OWN NUMBER ENTRY returned %d (element no. %d) from tag %s.",ret,index,KONStoreEntry);
			continue;
			}

		iONStoreEntries[index].iMode=mode;
		iONStoreEntries[index].iService=service;
		iONStoreEntries[index].iTelNum.Copy(telNum);
		iONStoreEntries[index].iText.Copy(name);
		iONStoreEntries[index].iTypeOfNumber=typeOfNumber;	
		iONStoreEntries[index].iNumberPlan=numberPlan;
		}

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMONSTORE_POPULATESTOREFROMCONFIGFILEL_3, "...Finished reading Own Number store entries...");

	}

TInt CSimONStore::GetONEntry(const CTestConfigItem* aItem, TInt aItemIndex, TInt& aIndex,
								 TPtrC8& aTelNum, TPtrC8& aText,
								 RMobilePhone::TMobileTON& aTypeOfNumber,
								 RMobilePhone::TMobileService& aService, 
								 RMobilePhone::TMobilePhoneNetworkMode& aMode,
								 RMobilePhone::TMobileNPI& aNumberPlan)
/**
 * Retrieve a Own Number entry from the configuration file, starting at a given item index
 * value.
 * @param aItem				Pointer to the config file item from which the Own Number entry will be read.
 * @param aItemIndex		The index number within the item from which the Own Number entry will be read.
 * @param aIndex			The returned index number
 * @param aTelNum			The returned telephone number
 * @param aTypeOfNumber		The returned Type of Number information
 * @param aService			The returned service 
 * @param aMode				The returned Network Mode
 * @return TInt				Standard error value.
 */
	{
	TInt ret;
	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aIndex);
	if(ret!=KErrNone)
		return ret;

	if(aIndex>iONMaxNumSlots) //the max number of slot is a valid slot
		return KErrArgument;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aTelNum);
	if(ret!=KErrNone)
		return ret;
	if(aTelNum.Length()>iONMaxTelNumLen)
		return KErrArgument;

	TInt typeOfNumber;
	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,typeOfNumber);
	if(ret!=KErrNone)
		return ret;
	
	aTypeOfNumber = (RMobilePhone::TMobileTON) typeOfNumber;
	if ( (TInt) RMobilePhone::EAbbreviatedNumber < typeOfNumber )
		return KErrArgument;

	TInt service;
	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,service);
	if(ret!=KErrNone)
		return ret;
	
	aService = (RMobilePhone::TMobileService) service;
	
	aMode = RMobilePhone::ENetworkModeGsm;
	
	TInt numberPlan;
	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,numberPlan);
	if(ret!=KErrNone)
		return ret;
	
	aNumberPlan = (RMobilePhone::TMobileNPI) numberPlan;
	switch (aNumberPlan)
		{
		case RMobilePhone::EUnknownNumberingPlan:
		case RMobilePhone::EIsdnNumberPlan:
		case RMobilePhone::EDataNumberPlan:
		case RMobilePhone::ETelexNumberPlan:
		case RMobilePhone::EServiceCentreSpecificPlan1:
		case RMobilePhone::EServiceCentreSpecificPlan2:
		case RMobilePhone::ENationalNumberPlan:
		case RMobilePhone::EPrivateNumberPlan:
		case RMobilePhone::EERMESNumberPlan:
		break;
	default:
		return KErrArgument;
		}

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aText);
	if(ret!=KErrNone)
		return ret;
	if(aText.Length()>iONMaxTextLen)
		return KErrArgument;

	return ret;
	}

CSimONStore::~CSimONStore()
/**
 * Standard destructor.  Any objects created by the ::ConstructL() function
 * will be destroyed here.
 */
	{
	delete[] iONStoreEntries;
	delete iONRwBuffer;
	delete iReqTimer;
	}

TInt CSimONStore::ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
/**
 * Dispatch function for all Own Number store requests.
 * @param aReqHandle	The TSY request handle for this request.
 * @param aIpc			The IPC number of this request.
 * @param aPckg			The parameter package related to this request.
 * @return TInt			The return error condition.
 */
	{
	TInt error;
	
// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		error = GetInfo(aReqHandle,aPckg.Des1n());
		return error;

	default:
		break;
		}


// The TSY can only process one of the following requests at a time.  If a second is received
// while processing the first, then it will be errored with KErrInUse.  This restriction will
// be removed later, by inserting a request queuing mechanism.  Note that the standard TSY
// "flow control" mechanism works phone-wide and so is not suitable.

	if(iReqTimer->IsActive())
		{
		ReqCompleted(aReqHandle,KErrInUse);
		return KErrNone;
		}

	switch(aIpc)
		{
	case EMobilePhoneStoreRead:
		 error = Read(aReqHandle,aPckg.Des1n());
		 return error;

	default:
		break;
		}

	return KErrNotSupported;
	}

CTelObject* CSimONStore::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimONStore::OpenNewObjectL(TDes&)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimONStore::ReqModeL(const TInt aIpc)
/**
 * This function returns the Request Mode for the request with the passed IPC value.
 * @param aIpc		The IPC number of the request.
 * @return TReqMode	The request mode.
 */
	{
	CTelObject::TReqMode ret=0;	

	switch(aIpc)
		{
	case EMobilePhoneStoreGetInfo:
	case EMobilePhoneStoreRead:
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CSimONStore::RegisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the first client makes a notification
 * request.  If supported by the underlying protocol controlling the
 * signalling stack, this can be used to start requesting updates for the relevant
 * service.
 */
	{
	return KErrNone;
	}

TInt CSimONStore::DeregisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the last client that had previously
 * made a notification request closes its ETel Server handle.  If supported by
 * the underlying protocol controlling the	signalling stack, this can be used
 * to stop requesting updates for the relevant service.
 */
	{
	return KErrNone;
	}

TInt CSimONStore::NumberOfSlotsL(const TInt /*aIpc*/)
/**
 * Return the number of slots that the ETel Server should allocate for buffering requests
 * of the given IPC number.
 */
	{
	return KDefaultNumberOfSlots;
	}

TInt CSimONStore::CancelService(const TInt /*aIpc*/,const TTsyReqHandle /*aTsyReqHandle*/)
/**
 * Cancel an outstanding request.
 * @param aIpc			The IPC number of the request that is to be cancelled.
 * @param aTsyReqHandle	The TSY request handle of the request that is to be cancelled.
 * @param TInt			Standard return value.
 */
	{
	return KErrNone;
	}

void CSimONStore::Init()
/**
 *	This function can be used to perform any necessary synchronous initialisation.
 */
	{
	}

TInt CSimONStore::GetInfo(TTsyReqHandle aReqHandle, TDes8* aPckg)
/**
 * Retrieve Own Number store information. This request is completed immediately, as it is assumed
 * that in a real TSY, all this data will be cached in the TSY.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package associated with this request.
 */
	{

	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	RMobileONStore::TMobileONStoreInfoV1Pckg* getInfoPckg=(RMobileONStore::TMobileONStoreInfoV1Pckg*)aPckg;
	RMobileONStore::TMobileONStoreInfoV1& getInfo=(*getInfoPckg)();

	PopulateONStoreInfo(&getInfo);

	if(&getInfo!=NULL) // This will be the case if the version checking within PopulatePhBkStoreInfo has failed.
		{
		ReqCompleted(aReqHandle,KErrNone);
		}
	else
		{
		ReqCompleted(aReqHandle,KErrNotSupported);
		}

	return KErrNone;
	}

void CSimONStore::PopulateONStoreInfo(RMobileONStore::TMobileONStoreInfoV1* aStoreInfo)
/**
 * Populate the passed parameter with Own Number store information.
 * @param aStoreInfo	Pointer to Own Number store information structure to be populated.
 */
	{
	__ASSERT_ALWAYS(aStoreInfo, SimPanic(EIllegalNullPtrParameter));
	
	aStoreInfo->iType=RMobilePhoneStore::EOwnNumberStore;
	aStoreInfo->iTotalEntries=iONMaxNumSlots;
	aStoreInfo->iCaps=iONStoreCaps;
	aStoreInfo->iName = KETelOwnNumberStore;
	aStoreInfo->iUsedEntries=UsedEntries();

	aStoreInfo->iNumberLen = iONMaxTelNumLen;
	aStoreInfo->iTextLen = iONMaxTextLen;

	}

TInt CSimONStore::Read(TTsyReqHandle aReqHandle, TDes8* aPckg1)
/**
 * Read Own Number store entries.  
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						It contains the index from which to read and value.
 * @return TInt			Standard return value.
 */
	{
	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	RMobileONStore::TMobileONEntryV1Pckg* ownNumberPckg = (RMobileONStore::TMobileONEntryV1Pckg*)aPckg1;
	RMobileONStore::TMobileONEntryV1& ownNumber = (*ownNumberPckg)();
	
	if(ownNumber.iIndex>iONMaxNumSlots)
			{
			ReqCompleted(aReqHandle,KErrArgument);
			return KErrNone;
			}

	else if (!(iONStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iONStoreCaps & RMobilePhoneStore::KCapsReadAccess))
			{
			ReqCompleted(aReqHandle, KErrAccessDenied);
			return KErrNone;
			}
		
	iONRwBuffer->Set(aPckg1);

	if(iONStoreEntries[ownNumber.iIndex].iTelNum.Length()!=0)
		{
		ownNumber.iMode = iONStoreEntries[ownNumber.iIndex].iMode;
		ownNumber.iService = iONStoreEntries[ownNumber.iIndex].iService;
		ownNumber.iNumber.iTelNumber.Copy(iONStoreEntries[ownNumber.iIndex].iTelNum);
		ownNumber.iNumber.iTypeOfNumber = iONStoreEntries[ownNumber.iIndex].iTypeOfNumber;
		ownNumber.iNumber.iNumberPlan = iONStoreEntries[ownNumber.iIndex].iNumberPlan;
		ownNumber.iText.Copy(iONStoreEntries[ownNumber.iIndex].iText);
		
		DelayCompletion(1, aReqHandle);
		}
	else
		{
		ReqCompleted(aReqHandle,KErrNotFound);		// No entries found.
		}

	return KErrNone;
	}

TInt CSimONStore::UsedEntries()
/**
 * Count the number of used entries in the Own Number store.
 * @return TInt	The number of used entries in the store.
 */
	{
	TInt cnt=0;
	for(TInt i=1;i<=iONMaxNumSlots;i++)
		{
		if(iONStoreEntries[i].iTelNum.Length()!=0)
			cnt++;
		}
	return cnt;
	}

void CSimONStore::DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle)
/**
 * A shell function for functions that wish to delay completion but do not trigger store
 * events.
 */
	{
	iPendingReqCompletion=aReqHandle;
	iReqTimer->Start(aDelayDuration,this,ETimerIdONStorReq);
	}

void CSimONStore::TimerCallBack(TInt aId)
/**
 * Process a timer call back event.  
 *
 * The "Request" timer is used to kick requests which have had their completions delayed.
 *
 * @param aId	The Id of the timer to which this callback relates.
 */
	{
	switch(aId)
		{
	case ETimerIdONStorReq:
		ReqCompleted(iPendingReqCompletion,KErrNone);
		break;

	default:
		break;
		}
	}

const CTestConfigSection* CSimONStore::CfgFile()
/**
* Returns a pointer to the config file section
*
* @return CTestConfigSection a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMONSTORE_CFGFILE_1, ">>CSimONStore::CfgFile");
	return iPhone->CfgFile();
	}
