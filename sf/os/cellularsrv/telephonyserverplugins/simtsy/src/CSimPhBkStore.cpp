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
// Implements the Phonebook Store manipulation code.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPhBkStoreTraces.h"
#endif

#include "CSimPhBkStore.h"
#include "CSimPhone.h"
#include <testconfigfileparser.h>

const TUint16 KNpiTonInternational=145;		// < The Number Plan Identifier and Type of Number for an international telephone number.
const TUint16 KNpiTonNational=129;			// < The Number Plan Identifier and Type of Number for a national telephone number.
const TInt KPhonebookErrorGranularity=3;	// < Granularity of phonebook error list array.
//
// CSimPhBkStore
//
void CSimPhBkStore::ClosePhone(TAny* aObj)
/**
 * A utility function for cleaning up the stack.
 */
	{
	((CObject*)aObj)->Close();
	}

CSimPhBkStore* CSimPhBkStore::NewL(CSimPhone* aPhone, const TDesC8& aName, TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen)
/**
 * Standard two phase constructor.
 * @param aPhone			The phone object from which this Phonebook was opened.
 * @param aName				The name of the created Phonebook.
 * @param aMaxNumSlots		The maximum number of slots in the Phonebook.
 * @return CSimPhBkStore*	The newly created object.
 */
	{
	CSimPhBkStore* store=new(ELeave) CSimPhBkStore(aPhone);
	TCleanupItem newObjClose(ClosePhone,store);
	CleanupStack::PushL(newObjClose);
	store->ConstructL(aName,aMaxNumSlots,aMaxNumLen,aMaxTextLen);
	CleanupStack::Pop();
	return store;
	}

CSimPhBkStore::CSimPhBkStore(CSimPhone* aPhone)
		: CSimPhone(iDummyPhoneBaseRef), iPhone(aPhone),iIpcCnt(0),iEvOutstandingReq(EFalse)
/**
 * Trivial first phase constructor.
 * @param aPhone	The phone object from which this phonebook was opened.
 */
	{}

void CSimPhBkStore::ConstructL(const TDesC8& aName, TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen)
/**
 * Second phase constructor that allocates memory for the phonebook, batch read buffer and
 * a delayed completion timer.  The constructor also reads the individual and batch read
 * delays from the configuration file.
 * 
 * @param aName			The name of the created phonebook.
 * @param aMaxNumLen	The maximum length of a telephone number.
 * @param aMaxTextLen	The maximum length of an alpha tag.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_CONSTRUCTL_1, "Starting to parse Phonebook store additional config parameters...");
	__ASSERT_ALWAYS(aMaxNumLen<=KPhBkMaxTelNumSize,SimPanic(EPhonebookNameOrNumberTooLarge));
	__ASSERT_ALWAYS(aMaxTextLen<=KPhBkMaxAlphaTagSize,SimPanic(EPhonebookNameOrNumberTooLarge));

	iPhBkStoreEntries=new(ELeave) TPhBkStoreEntry[aMaxNumSlots+1]; //slot 0 is unused
	iPhBkMaxNumSlots=aMaxNumSlots;
	iPhBkMaxTelNumLen=aMaxNumLen;
	iPhBkMaxTextLen=aMaxTextLen;
	iPhBkStoreName.Copy(aName);

	iReqTimer=CSimTimer::NewL(iPhone);
	iOOBWriteTimer=CSimTimer::NewL(iPhone);
	iOOBDeleteTimer=CSimTimer::NewL(iPhone);
	iPhBkRwBuffer=new(ELeave) CPhoneBookBuffer();
	iPhBkError=new(ELeave) CArrayFixFlat<TPhBkError>(KPhonebookErrorGranularity);

	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	item=CfgFile()->Item(KTriggerEventIPC,0);
	if(item)
		{
		TInt ipc, cnt, event;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,ipc);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element IPC returned %d (element no. %d) from tag %s.",ret,0,KTriggerEventIPC);
			}
		else
			iTriggerEventIPC.iIPC=ipc;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,cnt);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element CNT returned %d (element no. %d) from tag %s.",ret,1,KTriggerEventIPC);
			}
		else
			iTriggerEventIPC.iIPCCnt=cnt;
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,event);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element EVENT returned %d (element no. %d) from tag %s.",ret,2,KTriggerEventIPC);
			}
		else
			iTriggerEventIPC.iEvent=RMobilePhone::TMobilePhoneSecurityEvent(event);

		
		}

	const CTestConfigItem* item0=NULL;
	item0=CfgFile()->Item(KPhBkPhoneStoreCaps,0);
	if (item0)
		{
		TPtrC8 value0;
		TInt ret0=CTestConfig::GetElement(item0->Value(),KStdDelimiter,0,value0);
		if(ret0!=KErrNone)
			{
			iPhBkStoreCaps=KDefaultPhBkPhoneStoreCaps;
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element VALUE0 returned %d (element no. %d) from tag %s.",ret0,0,KPhBkPhoneStoreCaps);
			}	
		else
			{
			TUint32 intValue;
			TInt ret = AsciiToNum(value0, intValue);
			if(ret!=KErrNone)
				iPhBkStoreCaps=KDefaultPhBkPhoneStoreCaps;
			else
				iPhBkStoreCaps = intValue;
			}
		}
	else
		iPhBkStoreCaps=KDefaultPhBkPhoneStoreCaps;


	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_CONSTRUCTL_6, "...Finished parsing Phonebook store additional config parameters...");
	}

void CSimPhBkStore::PopulateStoreFromConfigFileL()
/**
 * Populate the Phonebook Store from information in the configuration file.  This is performed
 * after the standard Phonebook Store construction in order to prevent reseting the configuation
 * file accessor class' pointers while possibly multiple Phonebook Stores are created.
 *
 * The store entries comply to the following format:
 * "PhBkStoreEntry = <store name>, <slot number>, <telephone number>, <alphatag>"
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATESTOREFROMCONFIGFILEL_1, "Starting to read Phonebook store entries...");
	iPhBkIndividualPause=CfgFile()->ItemValue(KPhBkStoreIndividualReqPause,KDefaultPhBkStoreIndividualReqPause);
	iPhBkBatchPause=CfgFile()->ItemValue(KPhBkStoreBatchReqPause,KDefaultPhBkStoreBatchReqPause);

	TInt count=CfgFile()->ItemCount(KPhBkStoreEntry);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KPhBkStoreEntry,i);
		if(!item)
			break;

		TPtrC8 phonebookName,alphaTag,telNum;
		TInt index;
		TUint8 npiTon;
		ret=GetPhBkEntry(item,0,phonebookName,index,telNum,alphaTag,npiTon);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATESTOREFROMCONFIGFILEL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOK ENTRY returned %d (element no. %d) from tag %s.",ret,index,KPhBkStoreEntry);
			continue;
			}
		if(phonebookName.MatchF(iPhBkStoreName)!=0)// Not this phonebook
			continue;

		iPhBkStoreEntries[index].iAlphaTag.Copy(alphaTag);
		iPhBkStoreEntries[index].iTelNum.Copy(telNum);
		iPhBkStoreEntries[index].iTonNpi=npiTon;
		}

	count=CfgFile()->ItemCount(KPhBkError);
	item=NULL;

	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KPhBkError,i);
		if(!item)
			break;

		TInt count,error;
		TPtrC8 phonebookName, phonebookStore;
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,count);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATESTOREFROMCONFIGFILEL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNT returned %d (element no. %d) from tag %s.",ret,0,KPhBkError);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,error);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATESTOREFROMCONFIGFILEL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element ERROR returned %d (element no. %d) from tag %s.",ret,1,KPhBkError);
			continue;
			}	
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,phonebookName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATESTOREFROMCONFIGFILEL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKNAME returned %d (element no. %d) from tag %s.",ret,2,KPhBkError);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,phonebookStore);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATESTOREFROMCONFIGFILEL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKSTORE returned %d (element no. %d) from tag %s.",ret,3,KPhBkError);
			}
		else  //not for the global phonebook
			continue;

		if(phonebookName.MatchF(iPhBkStoreName)!=0)// Not this phonebook
			continue;


		TPhBkError entry;
		entry.iCount=count;
		entry.iError=error;
		iPhBkError->AppendL(entry);
		}
	PopulateOOBWrite();
	PopulateOOBDelete();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATESTOREFROMCONFIGFILEL_7, "...Finished reading Phonebook store entries...");

	if(iPhBkOOBWriteDuration!=-1)
		iOOBWriteTimer->Start(iPhBkOOBWriteDuration,this,ETimerIdPhBkStorOOBWrite);
	if(iPhBkOOBDeleteDuration!=-1)
		iOOBDeleteTimer->Start(iPhBkOOBDeleteDuration,this,ETimerIdPhBkStorOOBDelete);
	}

void CSimPhBkStore::PopulateOOBWrite()
/**
 * Populate the member variables required to operate the OOB Store functionality from
 * the configuration file.
 * The OOBPhBkStore configuration file tag should have the following format:
 * "OOBPhBkStore= <duration>, <phonebook name>, <index>, <telephone number>, <alpha tag>"
 */
	{
	iPhBkOOBWriteDuration=KErrNotFound;
	const CTestConfigItem* item=CfgFile()->Item(KOOBPhBkWrite);
	if(!item)
		return;

	TInt count;
	TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,count);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATEOOBWRITE_1, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNT returned %d (element no. %d) from tag %s.",ret,0,KOOBPhBkWrite);
		return;
		}

	TPtrC8 phonebookName,alphaTag,telNum;
	TInt index;
	TUint8 npiTon;
	ret=GetPhBkEntry(item,1,phonebookName,index,telNum,alphaTag,npiTon);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATEOOBWRITE_2, "WARNING - CONFIGURATION FILE PARSING - Reading element NPITON returned %d (element no. %d) from tag %s.",ret,index,KOOBPhBkWrite);
		return;
		}
	if(phonebookName.MatchF(iPhBkStoreName)!=0)
		return;						// Not this phonebook

	iPhBkOOBWriteDuration=count;
	iPhBkOOBWriteIndex=index;
	iPhBkOOBWrite.iAlphaTag.Copy(alphaTag);
	iPhBkOOBWrite.iTelNum.Copy(telNum);
	iPhBkOOBWrite.iTonNpi=npiTon;
	}

void CSimPhBkStore::PopulateOOBDelete()
/**
 * Populate the member variables required to operate the OOB Delete functionality from
 * the configuration file.
 * The OOBPhBkDelete configuration file tag should have the following format:
 * "OOBPhBkDelete= <duration>, <phonebook name>, <index>
 */
	{
	iPhBkOOBDeleteDuration=KErrNotFound;
	const CTestConfigItem* item=CfgFile()->Item(KOOBPhBkDelete);
	if(!item)
		return;

	TInt count;
	TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,count);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATEOOBDELETE_1, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNT returned %d (element no. %d) from tag %s.",ret,0,KOOBPhBkDelete);
		return;
		}

	TPtrC8 phonebookName;
	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,phonebookName);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATEOOBDELETE_2, "WARNING - CONFIGURATION FILE PARSING - Reading element PHONEBOOKNAME returned %d (element no. %d) from tag %s.",ret,1,KOOBPhBkDelete);
		return;
		}
	if(phonebookName.MatchF(iPhBkStoreName)!=0)
		return;						// Not this phonebook

	TInt index;
	ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,index);
	if(ret!=KErrNone)
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_POPULATEOOBDELETE_3, "WARNING - CONFIGURATION FILE PARSING - Reading element INDEX returned %d (element no. %d) from tag %s.",ret,2,KOOBPhBkDelete);
		return;
		}

	iPhBkOOBDeleteDuration=count;
	iPhBkOOBDeleteIndex=index;
	}

TInt CSimPhBkStore::GetPhBkEntry(const CTestConfigItem* aItem, TInt aItemIndex,
								 TPtrC8& aPhonebookName, TInt& aIndex,
								 TPtrC8& aTelNum, TPtrC8& aAlphaTag, TUint8& aNpiTon)
/**
 * Retrieve a phonebook entry from the configuration file, starting at a given item index
 * value.
 * @param aItem				Pointer to the config file item from which the phonebook entry will be read.
 * @param aItemIndex		The index number within the item from which the phonebook entry will be read.
 * @param aPhonebookName	The returned phonebook name
 * @param aIndex			The returned index number
 * @param aTelNum			The returned telephone number
 * @param aAlphaTag			The returned alpha tag
 * @param aNpiTon			The returned Number Plan Identifier and Type of Number information
 * @return TInt				Standard error value.
 */
	{
	TInt ret;
	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aPhonebookName);
	if(ret!=KErrNone)
		return ret;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aIndex);
	if(ret!=KErrNone)
		return ret;

	if(aIndex>iPhBkMaxNumSlots) //the max number of slot is a valid slot
		return KErrArgument;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aTelNum);
	if(ret!=KErrNone)
		return ret;
	if(aTelNum.Length()>iPhBkMaxTelNumLen)
		return KErrArgument;

	ret=CTestConfig::GetElement(aItem->Value(),KStdDelimiter,aItemIndex++,aAlphaTag);
	if(ret!=KErrNone)
		return ret;
	if(aAlphaTag.Length()>iPhBkMaxTextLen)
		return KErrArgument;

	if((aTelNum.Length()>0) && (aTelNum[0]=='+'))
		{
		aTelNum.Set(aTelNum.Mid(1));
		aNpiTon=KNpiTonInternational;
		}
	else
		aNpiTon=KNpiTonNational;
	return ret;
	}

CSimPhBkStore::~CSimPhBkStore()
/**
 * Standard destructor.  Any objects created by the ::ConstructL() function
 * will be destroyed here.
 */
	{
	delete[] iPhBkStoreEntries;
	delete iPhBkError;
	delete iPhBkRwBuffer;
	delete iOOBWriteTimer;
	delete iOOBDeleteTimer;
	delete iReqTimer;
	}

TInt CSimPhBkStore::ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
/**
 * Dispatch function for all Phonebook Store requests.
 * @param aReqHandle	The TSY request handle for this request.
 * @param aIpc			The IPC number of this request.
 * @param aPckg			The parameter package related to this request.
 * @return TInt			The return error condition.
 */
	{
	iIpcCnt++;
	TInt error=KErrNone;
	if(FindIpcErrorMatch(error))
		{
		ReqCompleted(aReqHandle,error);
		return KErrNone;
		}

// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		error = GetInfo(aReqHandle,aPckg.Des1n());
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		return error;

	case EMobilePhoneStoreNotifyStoreEvent:
		error = NotifyStoreEvent(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
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
// The standard multimode store read and write are not supported.
	case EMobilePhoneStoreRead:
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
		ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;

	case EMobilePhoneBookStoreRead:
		 error = Read(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		 if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		 return error;
			
	case EMobilePhoneBookStoreWrite:
		error = Write(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		 return error;

	case EMobilePhoneStoreDelete:
		error =  Delete(aReqHandle,aPckg.Des1n());
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		 return error;

	case EMobilePhoneStoreDeleteAll:
		error =  DeleteAll(aReqHandle);
		if(iTriggerEventIPC.iIPC==aIpc)
			{
			iTriggerCnt++;
			if(iTriggerEventIPC.iIPCCnt==iTriggerCnt)
				iPhone->SecurityEvent(iTriggerEventIPC.iEvent);
			}
		 return error;

	default:
		break;
		}

	return KErrNotSupported;
	}

CTelObject* CSimPhBkStore::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimPhBkStore::OpenNewObjectL(TDes&)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimPhBkStore::ReqModeL(const TInt aIpc)
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
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreDeleteAll:
	case EMobilePhoneBookStoreRead:
	case EMobilePhoneBookStoreWrite:
		break;

	case EMobilePhoneStoreNotifyStoreEvent:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CSimPhBkStore::RegisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the first client makes a notification
 * request.  If supported by the underlying protocol controlling the
 * signalling stack, this can be used to start requesting updates for the relevant
 * service.
 */
	{
	return KErrNone;
	}

TInt CSimPhBkStore::DeregisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the last client that had previously
 * made a notification request closes its ETel Server handle.  If supported by
 * the underlying protocol controlling the	signalling stack, this can be used
 * to stop requesting updates for the relevant service.
 */
	{
	return KErrNone;
	}

TInt CSimPhBkStore::NumberOfSlotsL(const TInt /*aIpc*/)
/**
 * Return the number of slots that the ETel Server should allocate for buffering requests
 * of the given IPC number.
 */
	{
	return KDefaultNumberOfSlots;
	}

TInt CSimPhBkStore::CancelService(const TInt aIpc,const TTsyReqHandle /*aTsyReqHandle*/)
/**
 * Cancel an outstanding request.
 * @param aIpc			The IPC number of the request that is to be cancelled.
 * @param aTsyReqHandle	The TSY request handle of the request that is to be cancelled.
 * @param TInt			Standard return value.
 */
	{
	switch(aIpc)
		{
	case EMobilePhoneStoreGetInfo:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreDeleteAll:
	case EMobilePhoneBookStoreRead:
	case EMobilePhoneBookStoreWrite:
		break;

	case EMobilePhoneStoreNotifyStoreEvent:
		NotifyStoreEventCancel();
		break;

	default:
		break;
		}
	return KErrNone;
	}

void CSimPhBkStore::Init()
/**
 *	This function can be used to perform any necessary synchronous initialisation.
 */
	{
	}

TInt CSimPhBkStore::GetInfo(TTsyReqHandle aReqHandle, TDes8* aPckg)
/**
 * Retrieve Phonebook Store information. This request is completed immediately, as it is assumed
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

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg* getInfoPckg=(RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg*)aPckg;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1& getInfo=(*getInfoPckg)();

	PopulatePhBkStoreInfo(&getInfo);

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

void CSimPhBkStore::PopulatePhBkStoreInfo(RMobilePhoneStore::TMobilePhoneStoreInfoV1* aStoreInfo)
/**
 * Populate the passed parameter with Phonebook Store information.
 * @param aStoreInfo	Pointer to phonebook store information structure to be populated.
 */
	{
	__ASSERT_ALWAYS(aStoreInfo, SimPanic(EIllegalNullPtrParameter));
	
	aStoreInfo->iType=RMobilePhoneStore::EPhoneBookStore;
	aStoreInfo->iTotalEntries=MaxSlots();
	aStoreInfo->iCaps=iPhBkStoreCaps;
	aStoreInfo->iName.Copy(iPhBkStoreName);
	aStoreInfo->iUsedEntries=UsedEntries();

	if(aStoreInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV1)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1* getExtraInfo=(RMobilePhoneBookStore::TMobilePhoneBookInfoV1*)aStoreInfo;

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(*getExtraInfo);
		if(err != KErrNone)
			{
			getExtraInfo = NULL;
			return;
			}

		getExtraInfo->iMaxNumLength=iPhBkMaxTelNumLen;
		getExtraInfo->iMaxTextLength=iPhBkMaxTextLen;
		getExtraInfo->iLocation=RMobilePhoneBookStore::ELocationIccMemory;
		getExtraInfo->iChangeCounter=0;
		getExtraInfo->iIdentity.Copy(iPhone->GetImsi());		
		}
	else if(aStoreInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV2)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV2* getExtraInfo=(RMobilePhoneBookStore::TMobilePhoneBookInfoV2*)aStoreInfo;

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(*getExtraInfo);
		if(err != KErrNone)
			{
			getExtraInfo = NULL;
			return;
			}

		getExtraInfo->iMaxNumLength=iPhBkMaxTelNumLen;
		getExtraInfo->iMaxTextLength=iPhBkMaxTextLen;
		getExtraInfo->iLocation=RMobilePhoneBookStore::ELocationIccMemory;
		getExtraInfo->iChangeCounter=0;
		getExtraInfo->iIdentity.Copy(iPhone->GetImsi());

		getExtraInfo->iPhBkMode.Zero();		
		}
	else if(aStoreInfo->ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV5)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV5* getExtraInfo=(RMobilePhoneBookStore::TMobilePhoneBookInfoV5*)aStoreInfo;

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(*getExtraInfo);
		if(err != KErrNone)
			{
			getExtraInfo = NULL;
			return;
			}

		getExtraInfo->iMaxNumLength=iPhBkMaxTelNumLen;
		getExtraInfo->iMaxTextLength=iPhBkMaxTextLen;
		getExtraInfo->iLocation=RMobilePhoneBookStore::ELocationIccMemory;
		getExtraInfo->iChangeCounter=0;
		getExtraInfo->iIdentity.Copy(iPhone->GetImsi());

		getExtraInfo->iPhBkMode.Zero();

		getExtraInfo->iMaxSecondNames = 0;
		getExtraInfo->iMaxTextLengthSecondName = 0;
		getExtraInfo->iMaxAdditionalNumbers = 0;
		getExtraInfo->iMaxTextLengthAdditionalNumber = 0;
		getExtraInfo->iMaxNumLengthAdditionalNumber = 0;
		getExtraInfo->iMaxGroupNames = 0;
		getExtraInfo->iMaxTextLengthGroupName = 0;
		getExtraInfo->iMaxEmailAddr = 0;
		getExtraInfo->iMaxTextLengthEmailAddr = 0;
		}	
	}

TInt CSimPhBkStore::Read(TTsyReqHandle aReqHandle, TDes8* aPckg1,TDes8* aPckg2)
/**
 * Read Phonebook store entries.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to go and get the information from a SIM card.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						It contains the index from which the read should start and
 *						the number of entries which should be read.
 * @param aPckg2		The second parameter package associated with this request.
 *						It contains the buffer, which will be populated with the retrieved
 *						values.
 * @return TInt			Standard return value.
 */
	{
	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	TPckg<RMobilePhoneBookStore::TPBIndexAndNumEntries>* indexNumPckg=(TPckg<RMobilePhoneBookStore::TPBIndexAndNumEntries>*)aPckg1;
	RMobilePhoneBookStore::TPBIndexAndNumEntries& indexNum=(*indexNumPckg)();

	if(indexNum.iNumSlots==iPhBkMaxNumSlots)
		{
		if (!(iPhBkStoreCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore) && iPhBkStoreCaps & RMobilePhoneStore::KCapsReadAccess))
			{
			ReqCompleted(aReqHandle, KErrAccessDenied);
			return KErrNone;
			}
		}
	else if((indexNum.iIndex+indexNum.iNumSlots - 1)>iPhBkMaxNumSlots)
			{
			ReqCompleted(aReqHandle,KErrArgument);
			return KErrNone;
			}

	else if (!(iPhBkStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iPhBkStoreCaps & RMobilePhoneStore::KCapsReadAccess))
			{
			ReqCompleted(aReqHandle, KErrAccessDenied);
			return KErrNone;
			}
		
	iPhBkRwBuffer->Set(aPckg2);

	TInt cnt=0;
	TInt ret=0;
	TBool partial=EFalse;
	for(TInt i=indexNum.iIndex;i<=iPhBkMaxNumSlots;i++)
		{
		if((iPhBkStoreEntries[i].iTelNum.Length()!=0)||(iPhBkStoreEntries[i].iAlphaTag.Length()!=0))
			{
			ret=iPhBkRwBuffer->AddNewEntryTag();
			if(ret!=KErrNone)
				break;

			partial=ETrue;
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex,(TUint16)i);
			if(ret!=KErrNone)
				break;
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText,iPhBkStoreEntries[i].iAlphaTag);
			if(ret!=KErrNone)
				break;
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,iPhBkStoreEntries[i].iTonNpi);
			if(ret!=KErrNone)
				break;
			ret=iPhBkRwBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,iPhBkStoreEntries[i].iTelNum);
			if(ret!=KErrNone)
				break;

			partial=EFalse;
			if(++cnt>=indexNum.iNumSlots)
				break;
			}
		}
	if(partial)
		// EXPORT - but return value not relevant
		(void)iPhBkRwBuffer->RemovePartialEntry();

	indexNum.iNumSlots=cnt;

	if((cnt==0)&&(partial))
		ReqCompleted(aReqHandle,KErrArgument);		// An entry was found, but the buffer was too small to return it.
	else if((cnt==0)&&(!partial))
		ReqCompleted(aReqHandle,KErrNotFound);		// No entries found.
	else
		ReqCompleted(aReqHandle,KErrNone);

	return KErrNone;
	}

TInt CSimPhBkStore::Write(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2)
/**
 * Write a phonebook store entries.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to write the information to a SIM card. A store
 * event may be triggered by this request.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						This contains the TLV phonebook entry to be written.
 * @param aPckg2		The second parameter package associated with this request.
 *						This contains the slot index number to which the entry must be written.
 */
	{
	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}
	
	if (!(iPhBkStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iPhBkStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	TPckg<TInt>* indexPckg=(TPckg<TInt>*)aPckg2;
	TInt& index=(*indexPckg)();

// If the index is -1, then use the first available free slot.
	if(index==-1)
		{
		for(TInt i=1;i<=iPhBkMaxNumSlots;i++)
			{
			if((iPhBkStoreEntries[i].iTelNum.Length()==0)&&(iPhBkStoreEntries[i].iAlphaTag.Length()==0))
				{
				index=i;
				break;
				}
			}
		}

	if(index==-1)
		{
		ReqCompleted(aReqHandle,EXTENDEDERROR(KErrNoMemory, KErrPhonebookNoMemory));
		return KErrNone;
		}

	if((index<1)||(index>iPhBkMaxNumSlots))
		{
		ReqCompleted(aReqHandle,KErrArgument);
		return KErrNone;
		}

	TBool isSlotAlreadyUsed=EFalse;
	if(iPhBkStoreEntries[index].iTelNum.Length()!=0)
		isSlotAlreadyUsed=ETrue;

// Unpick the phonebook entry.
	iPhBkRwBuffer->Set(aPckg1);
	iPhBkRwBuffer->StartRead();

	TUint8 tagVal;
	TUint8 npiTon=0;
	TPtrC alphaTag;
	TPtrC telNum;
	TPtrC alphaTag2;
	TBool skipToNextEntry=EFalse;		
	CPhoneBookBuffer::TPhBkTagType tagType;
	TInt ret=KErrNone;

	while(ret==KErrNone)
		{	
		ret=iPhBkRwBuffer->GetTagAndType(tagVal,tagType);
		if(ret==KErrNotFound)
			{
			ret=KErrNone;
			break;
			}
		else if(ret!=KErrNone)
			break;

	
		switch(tagVal)
			{
		case RMobilePhoneBookStore::ETagPBNewEntry:
			skipToNextEntry=EFalse;
			break;

		case RMobilePhoneBookStore::ETagPBText:
			if(!skipToNextEntry)
				ret=iPhBkRwBuffer->GetValue(alphaTag);
			else
				{
				TPtrC throwAway;
				ret=iPhBkRwBuffer->GetValue(throwAway);
				}
			break;

		case RMobilePhoneBookStore::ETagPBNumber:
			if(!skipToNextEntry)
				ret=iPhBkRwBuffer->GetValue(telNum);
			else
				{
				TPtrC throwAway;
				ret=iPhBkRwBuffer->GetValue(throwAway);
				}
			break;

		case RMobilePhoneBookStore::ETagPBTonNpi:
			if(!skipToNextEntry)
				ret=iPhBkRwBuffer->GetValue(npiTon);
			else
				{
				TUint8 throwAway;
				ret=iPhBkRwBuffer->GetValue(throwAway);
				}
			break;
		
		// Alexandros - phbksync "shortcut" should be discussed and removed
		case RMobilePhoneBookStore::ETagPBAnrStart:
			skipToNextEntry=ETrue;
			break;

		case RMobilePhoneBookStore::ETagPBEmailAddress:
		case RMobilePhoneBookStore::ETagPBGroupName:
		case RMobilePhoneBookStore::ETagPBSecondName:
			ret=iPhBkRwBuffer->GetValue(alphaTag2);
			break;
		case RMobilePhoneBookStore::ETagPBHiddenInfo:
			TUint8 throwAway;
			ret=iPhBkRwBuffer->GetValue(throwAway);
			break;
		
		default:
			//DEF001769 - SIM TSY should ignore extra fields in short entries
			//ret=KErrNotSupported;
			break;
			}
		}

	if(ret!=KErrNone)
		{
		ReqCompleted(aReqHandle,ret);
		return KErrNone;
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_WRITE_1, "alphaTag Length = (%d)",alphaTag.Length());
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_WRITE_2, "Phonebook Max Text Length = (%d)",iPhBkMaxTextLen);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_WRITE_3, "TelNum Length = (%d)",telNum.Length());
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_WRITE_4, "TelNum Max Length = (%d)",iPhBkMaxTelNumLen);

	if(alphaTag.Length()>iPhBkMaxTextLen)
		{
		ReqCompleted(aReqHandle, EXTENDEDERROR(KErrOverflow, KErrPhonebookTextOverflow));
		return KErrNone;
		}
	else if(telNum.Length()>iPhBkMaxTelNumLen)
		{
		ReqCompleted(aReqHandle, EXTENDEDERROR(KErrOverflow, KErrPhonebookNumberOverflow));
		return KErrNone;
		}

	iPhBkStoreEntries[index].iAlphaTag.Copy(alphaTag);
	iPhBkStoreEntries[index].iTelNum.Copy(telNum);
	iPhBkStoreEntries[index].iTonNpi=npiTon;
	if(isSlotAlreadyUsed)
		DelayCompletion(iPhBkIndividualPause,aReqHandle,EStoreEventChanged,index);
	else
		DelayCompletion(iPhBkIndividualPause,aReqHandle,EStoreEventAdded,index);
	return KErrNone;
	}

TInt CSimPhBkStore::Delete(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Delete a single Phonebook store entry.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to write the information to a SIM card. A store
 * event may be triggered by this request.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package associated with this request.
 * @return				Standard return value.
 */
	{
	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}
	
	if (!(iPhBkStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iPhBkStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}


	TPckg<TInt>* intPckg=(TPckg<TInt>*)aPckg;
	TInt& index=(*intPckg)();

	if((index<1)||(index>iPhBkMaxNumSlots))
		{
		ReqCompleted(aReqHandle,KErrArgument);
		return KErrNone;
		}

	iPhBkStoreEntries[index].iTelNum.Zero();
	iPhBkStoreEntries[index].iAlphaTag.Zero();
	DelayCompletion(iPhBkIndividualPause,aReqHandle,EStoreEventDeleted,index);
	return KErrNone;
	}

TInt CSimPhBkStore::DeleteAll(TTsyReqHandle aReqHandle)
/**
 * Delete all entries in the Phonebook Store.  The completion of this function is delayed in
 * order to simulate the SIM operations a real TSY would have to carry out.  This function
 * may trigger an Phonebook Store notification.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @return				Standard return value.
 */
	{
	if(iPhone->IsICCLocked()!=EFalse)
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	if (!(iPhBkStoreCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore) && iPhBkStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	for(TInt i=1;i<=iPhBkMaxNumSlots;i++)
		{
		iPhBkStoreEntries[i].iTelNum.Zero();
		iPhBkStoreEntries[i].iAlphaTag.Zero();
		}
	DelayCompletion(iPhBkBatchPause,aReqHandle,EStoreEventDeleted,-1);
	return KErrNone;
	}

TInt CSimPhBkStore::NotifyStoreEvent(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2)
/**
 * Register a client's interest in Phonebook Store events.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						It contains the event flags that will be returned to the client.
 * @param aPckg2		The second parameter package associated with this request.
 *						It contains the index value associated with the event
 *						that will be returned to the client.
 * @return				Standard return value.
 */
	{
	TPckg<TUint32>* eventPckg=(TPckg<TUint32>*)aPckg1;
	TUint32& event=(*eventPckg)();
	TPckg<TInt>* indexPckg=(TPckg<TInt>*)aPckg2;
	TInt& index=(*indexPckg)();

	iEvOutstandingReq=ETrue;
	iEvReqHandle=aReqHandle;
	iEvEvent=&event;
	iEvIndex=&index;
	return KErrNone;
	}

void CSimPhBkStore::NotifyStoreEventCancel()
/**
 * Cancel an outstanding notify store request.
 */
	{
	if(iEvOutstandingReq)
		{
		iEvOutstandingReq=EFalse;
		ReqCompleted(iEvReqHandle,KErrCancel);
		}
	}

TPtrC8 CSimPhBkStore::Name()
/**
 * Accessor function fot the Phonebook Store name.
 *
 * @return TPtrC8	The name of this Phonebook Store.
 */
	{
	return iPhBkStoreName;
	}

TInt CSimPhBkStore::UsedEntries()
/**
 * Count the number of used entries in the Phonebook Store.
 * @return TInt	The number of used entries in the store.
 */
	{
	TInt cnt=0;
	for(TInt i=1;i<=iPhBkMaxNumSlots;i++)
		{
		if((iPhBkStoreEntries[i].iTelNum.Length()!=0)||(iPhBkStoreEntries[i].iAlphaTag.Length()!=0))
			cnt++;
		}
	return cnt;
	}

TInt CSimPhBkStore::MaxSlots()
/**
 * Retrieve the maximum number of slots in this Phonebook Store.
 * @return TInt	The maximum number of slots in this Phonebook Store.
 */
	{
	return iPhBkMaxNumSlots;
	}

void CSimPhBkStore::DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle)
/**
 * A shell function for functions that wish to delay completion but do not trigger store
 * events.
 */
	{
	DelayCompletion(aDelayDuration,aReqHandle,EStoreEventNoEvent,0);
	}

void CSimPhBkStore::DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle,TStoreEvent aEvent,TInt aIndex)
/**
 * Delay the completion of a TSY request.  It is assumed that the member variable
 * manipulation associated with the request has already taken place, and so all that is
 * left to do is call the ETel server's request completion function when the timer expires.
 * So, just record the parameters and kick off the timer.
 *
 * @param aDelayDuration	The time (in seconds) for which the request completion is to be delayed.
 * @param aReqHandle		The TSY request handle related to the delayed completion.
 * @param aEvent			The store event related to the delayed completion.
 * @param aIndex			The index related to the event passed in aEvent.
 */
	{
	iPendingReqCompletion=aReqHandle;
	iPendingEvent=aEvent;
	iPendingIndex=aIndex;
	iReqTimer->Start(aDelayDuration,this,ETimerIdPhBkStorReq);
	}

void CSimPhBkStore::TimerCallBack(TInt aId)
/**
 * Process a timer call back event.  There are three timers associated with this class
 * and this callback will be used for all of them.  The timers can be identified from the
 * aId parameter passed with the callback.
 *
 * The "Request" timer is used to kick requests which have had their completions delayed.
 * The "Out of Band Write" timer is used to schedule a non-client phonebook write.
 * The "Out of Band Delete" timer is used to schedule a non-client phonebook delete.
 *
 * @param aId	The Id of the timer to which this callback relates.
 */
	{
	switch(aId)
		{
	case ETimerIdPhBkStorReq:
		StoreEvent(iPendingEvent,iPendingIndex);
		ReqCompleted(iPendingReqCompletion,KErrNone);
		break;

	case ETimerIdPhBkStorOOBWrite:
		iPhBkStoreEntries[iPhBkOOBWriteIndex].iAlphaTag.Copy(iPhBkOOBWrite.iAlphaTag);
		iPhBkStoreEntries[iPhBkOOBWriteIndex].iTelNum.Copy(iPhBkOOBWrite.iTelNum);
		iPhBkStoreEntries[iPhBkOOBWriteIndex].iTonNpi=iPhBkOOBWrite.iTonNpi;
		StoreEvent(EStoreEventAdded,iPhBkOOBWriteIndex);
		break;

	case ETimerIdPhBkStorOOBDelete:
		iPhBkStoreEntries[iPhBkOOBDeleteIndex].iAlphaTag.Zero();
		iPhBkStoreEntries[iPhBkOOBDeleteIndex].iTelNum.Zero();
		iPhBkStoreEntries[iPhBkOOBDeleteIndex].iTonNpi=0;
		StoreEvent(EStoreEventDeleted,iPhBkOOBDeleteIndex);
		break;

	default:
		break;
		}
	}

void CSimPhBkStore::StoreEvent(TStoreEvent aEvent,TInt aIndex)
/**
 * Determine if a store event notification should be completed.
 * @param aEvent	The store event.
 * @param aIndex	The index related to the store event.
 */
	{
	if(iEvOutstandingReq)
		{
		TUint event=0;
		switch(aEvent)
			{
		case EStoreEventNoEvent:
			return;

		case EStoreEventAdded:
			event|=RMobilePhoneStore::KStoreEntryAdded;
			break;

		case EStoreEventDeleted:
			event|=RMobilePhoneStore::KStoreEntryDeleted;
			break;

		case EStoreEventChanged:
			event|=RMobilePhoneStore::KStoreEntryChanged;
			break;

		default:
			break;
			}

		TInt cnt=UsedEntries();
		if(cnt==0)
			event|=RMobilePhoneStore::KStoreEmpty;

		if(cnt==iPhBkMaxNumSlots)
			event|=RMobilePhoneStore::KStoreFull;
		else
			event|=RMobilePhoneStore::KStoreHasSpace;

		*iEvEvent=event;
		*iEvIndex=aIndex;
		iEvOutstandingReq=EFalse;
		ReqCompleted(iEvReqHandle,KErrNone);
		}
	}

TBool CSimPhBkStore::FindIpcErrorMatch(TInt& aError)
/**
 * Determine whether the IPC counter has signalled that the current request should
 * be errored, rather than executed.
 *
 * @param aError	If the function returns ETrue, this parameter will pass back the
 *					number of the error to be propagated.
 * @return TBool	Returns ETrue if a match with the IPC count is found, EFalse if not.
 */
	{
	TInt i;
	for(i=0;i<iPhBkError->Count();i++)
		{
		TInt pi=iPhBkError->At(i).iCount;
		if(pi==iIpcCnt)
			{
			aError=iPhBkError->At(i).iError;
			return ETrue;
			}
		}
	return EFalse;
	}

const CTestConfigSection* CSimPhBkStore::CfgFile()
/**
* Returns a pointer to the config file section
*
* @return CTestConfigSection a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHBKSTORE_CFGFILE_1, ">>CSimPhBkStore::CfgFile");
	return iPhone->CfgFile();
	}
