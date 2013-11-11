// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the SMS Store manipulation code.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimSmsStoreTraces.h"
#endif

#include <mmlist.h>
#include <mmretrieve.h>
#include <testconfigfileparser.h>
#include "CSimSmsStore.h"
#include "csimsmsmess.h"
#include "CSimTsyMode.h"

const TInt KSmsStoreGranularity=2;
const TInt KCorruptPDU=9999;
_LIT(KNullTelephoneNumber,"0");


CListReadAllAttempt* CListReadAllAttempt::NewL(RMobilePhone::TClientId& aId, TTsyReqHandle aReqHandle)
/**
 * Standard two phase constructor.
 * @param aId The client ID (session and sub-session handles) that uniquely identifies a client performing a two phase read.
 * @param aReqHandle The TSY request handle of the client request associated with a two phase read.
 * @return CListReadAllAttempt* The newly created object.
 */
	{
	CListReadAllAttempt* read=new(ELeave) CListReadAllAttempt(aId, aReqHandle);
	CleanupStack::PushL(read);
	read->ConstructL();
	CleanupStack::Pop();
	return read;
	}

CListReadAllAttempt::CListReadAllAttempt(RMobilePhone::TClientId& aId, TTsyReqHandle aReqHandle)
		: iListBuf(NULL),iReqHandle(aReqHandle)
/**
 * Trivial first phase constructor.
 * @param aId The client ID (session and sub-session handles) that uniquely identifies a client performing a two phase read.
 * @param aReqHandle The TSY request handle of the client request associated with a two phase read.
 */
	{
	iClient.iSessionHandle=aId.iSessionHandle;
	iClient.iSubSessionHandle=aId.iSubSessionHandle;
	iReqHandle=aReqHandle;
	iListBuf = NULL;
	}

void CListReadAllAttempt::ConstructL()
/**
 * Trivial second phase constructor.
 */
	{
	}

CListReadAllAttempt::~CListReadAllAttempt()
/**
 * Trivial destructor.
 */
	{
	delete iListBuf;
	}

//
// CSimSmsStore
//
void CSimSmsStore::ClosePhone(TAny* aObj)
/**
 * A utility function for cleaning up the stack.
 */
	{
	((CObject*)aObj)->Close();
	}

CSimSmsStore* CSimSmsStore::NewL(CSimSmsMessaging* aSimSmsMess, const TDesC8& aName, TInt aMaxNumSlots, CSimPhone* aPhone)
/**
 * Standard two phase constructor.
 * @param aSimSmsMess	The SMS Messaging object from which this SMS Store was opened.
 * @param aName			The name of the created SMS Store.
 * @param aMaxNumSlots	The maximum number of slots in the SMS Store.
 * @return CSimSmsStore* The newly created object.
 */
	{
	CSimSmsStore* store=new(ELeave) CSimSmsStore(aSimSmsMess, aPhone);
	TCleanupItem newObjClose(ClosePhone,store);
	CleanupStack::PushL(newObjClose);
	store->ConstructL(aName,aMaxNumSlots);
	CleanupStack::Pop();
	return store;
	}

CSimSmsStore::CSimSmsStore(CSimSmsMessaging* aSmsMess, CSimPhone* aPhone)
		: iSmsMessaging(aSmsMess), iEvOutstandingReq(EFalse), iPhone(aPhone)
/**
 * Trivial first phase constructor.
 * @param aSimSmsMess	The SMS Messaging object from which this SMS Store was opened.
 */
	{}

void CSimSmsStore::ConstructL(const TDesC8& aName, TInt aMaxNumSlots)
/**
 * Second phase constructor that allocates memory for the SMS store, batch read buffer and
 * a delayed completion timer.  The constructor also reads the individual and batch read
 * delays from the configuration file.
 * 
 * @param aName			The name of the created SMS Store.
 * @param aMaxNumSlots	The maximum number of slots in the SMS Store.
 */
	{
	iGsmSmsStoreEntries=new(ELeave) RMobileSmsStore::TMobileGsmSmsEntryV1[aMaxNumSlots+1];//index 0 is not used
	iSmsMaxNumSlots=aMaxNumSlots;
	iSmsStoreName.Copy(aName);
	iSmsReadAll=new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(KSmsStoreGranularity);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_CONSTRUCTL_1, "Starting to parse SMS Store config parameters...");
	iSmsIndividualPause=CfgFileSection()->ItemValue(KSmsStoreIndividualReqPause,KDefaultSmsStoreIndividualReqPause);
	iSmsBatchPause=CfgFileSection()->ItemValue(KSmsStoreBatchReqPause,KDefaultSmsStoreBatchReqPause);
	
	const CTestConfigItem* item0=NULL;
	item0=CfgFileSection()->Item(KSmsPhoneStoreCaps,0);
	if (item0)
		{
		TPtrC8 value0;
		TInt ret0=CTestConfig::GetElement(item0->Value(),KStdDelimiter,0,value0);
		if(ret0!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element VALUE0 returned %d (element no. %d) from tag %s.",ret0,0,KSmsPhoneStoreCaps);
			iSmsStoreCaps=KDefaultSmsPhoneStoreCaps+KDefaultSmsOnlySmsCaps;
			}
		else
			{
			TUint32 intValue;
			TInt ret = AsciiToNum(value0, intValue);
			if(ret!=KErrNone)
				iSmsStoreCaps=KDefaultSmsPhoneStoreCaps+KDefaultSmsOnlySmsCaps;
			else
				iSmsStoreCaps = intValue+KDefaultSmsOnlySmsCaps;
			}
		}
	else
		iSmsStoreCaps=KDefaultSmsPhoneStoreCaps+KDefaultSmsOnlySmsCaps;


	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_CONSTRUCTL_3, "...Finished parsing SMS Store config parameters");
	iTimer=CSimTimer::NewL(iSmsMessaging->iPhone);
	iIncomingTimer=CSimTimer::NewL(iSmsMessaging->iPhone);
	}

void CSimSmsStore::PopulateStoreFromConfigFile()
/**
 * Populate the SMS Store from information in the configuration file.  This is performed
 * after the standard SMS Store construction in order to prevent reseting the configuation
 * file accessor class' pointers while possibly multiple SMS Stores are created.
 *
 * The store entries comply to the following format:
 * "SmsStoreEntry = <store name>, <slot number>, <entry status>, <pdu>"
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_POPULATESTOREFROMCONFIGFILE_1, "Starting reading SMS Store entries...");
	TInt count=CfgFileSection()->ItemCount(KSmsStoreEntry);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	for(TInt i=0;i<count;i++)
		{
		item=CfgFileSection()->Item(KSmsStoreEntry,i);
		if(!item)
			break;

		TPtrC8 storeName;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,storeName);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_POPULATESTOREFROMCONFIGFILE_2, "WARNING - CONFIGURATION FILE PARSING - Reading element STORENAME returned %d (element no. %d) from tag %s.",ret,0,KSmsStoreEntry);
			continue;
			}
		if(storeName.MatchF(iSmsStoreName)!=0)
			continue;						// Not this store

		TInt index,stat;
		TPtrC8 pdu;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,index);
		if((ret!=KErrNone)||(index>=iSmsMaxNumSlots))
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_POPULATESTOREFROMCONFIGFILE_3, "WARNING - CONFIGURATION FILE PARSING - Reading element INDEX returned %d (element no. %d) from tag %s.",ret,1,KSmsStoreEntry);
			continue;
			}
	
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,stat);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_POPULATESTOREFROMCONFIGFILE_4, "WARNING - CONFIGURATION FILE PARSING - Reading element STAT returned %d (element no. %d) from tag %s",ret,2,KSmsStoreEntry);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,pdu);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_POPULATESTOREFROMCONFIGFILE_5, "WARNING - CONFIGURATION FILE PARSING - Reading element PDU returned %d (element no. %d) from tag %s.",ret,3,KSmsStoreEntry);
			continue;
			}

		TPtrC8 sca;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,sca);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_POPULATESTOREFROMCONFIGFILE_6, "WARNING - CONFIGURATION FILE PARSING - Reading element SCA returned %d (element no. %d) from tag %s.",ret,4,KSmsStoreEntry);
			continue;
			}
		iGsmSmsStoreEntries[index].iIndex=index;
		iGsmSmsStoreEntries[index].iMsgStatus=(RMobileSmsStore::TMobileSmsStoreStatus)stat;
		ConvertAsciiSms(pdu,iGsmSmsStoreEntries[index].iMsgData);
		RecordSca(sca, index);
		}

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_POPULATESTOREFROMCONFIGFILE_7, "...Finished reading SMS Store entries");
	}



void CSimSmsStore::RecordSca(const TDesC8& aAsciiAddr, TInt aIndex)
/**
 * Used to set the sca number
 * 
 *
 * @param aAsciiSca Ascii representation of a telephone number such as the one found in the configuration file
 * @param aType specifies whether aAsciiAddres should be used to fill in the sca or destination field of the smsp entry
 * @param aIndex specifies at which index in the smsp list the number should be filled in
 */
	{
	if(aAsciiAddr.Length()>0)//only support for international numbers
		{
		iGsmSmsStoreEntries[aIndex].iServiceCentre.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
		iGsmSmsStoreEntries[aIndex].iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
		iGsmSmsStoreEntries[aIndex].iServiceCentre.iTelNumber.Copy(aAsciiAddr);
		}
	}

CSimSmsStore::~CSimSmsStore()
/**
 * Standard destructor.  Any objects created by the ::ConstructL() function
 * will be destroyed here.
 */
	{
	delete[] iGsmSmsStoreEntries;
	
	if (iSmsReadAll)
		{
		iSmsReadAll->ResetAndDestroy();
		delete iSmsReadAll;
		}

	delete iTimer;
	delete iIncomingTimer;
	}

#ifdef OST_TRACE_COMPILER_IN_USE
void CSimSmsStore::LogRequest(TBool aEntering, TInt aIpc, TInt aError)
#else
void CSimSmsStore::LogRequest(TBool aEntering, TInt aIpc, TInt /*aError*/)
#endif
/**
 * This method logs client requests to the t_reg.txt
 * @param aEntering Tells if the request is entering into simtsy tsy or completing
 * @param aIpc IPC number of request
 * @param aError Error code that the request has
 */
 	{
	TBuf8<64> ipcBuf;

	switch (aIpc)
		{
		case EMobilePhoneStoreGetInfo:
			ipcBuf = _L8("GetInfo");
			break;

		case EMobilePhoneStoreNotifyStoreEvent:
			ipcBuf = _L8("NotifyStoreEvent");
			break;

		case EMobilePhoneStoreRead:
			ipcBuf = _L8("Read");
			break;

		case EMobilePhoneStoreWrite:
			ipcBuf = _L8("Write");
			break;

		case EMobilePhoneStoreDelete:
			ipcBuf = _L8("Delete");
			break;

		case EMobilePhoneStoreReadAllPhase1:
			ipcBuf = _L8("ReadAllPhase1");
			break;


		case EMobilePhoneStoreReadAllPhase2:
			ipcBuf = _L8("ReadAllPhase2");
			break;

		case EMobilePhoneStoreDeleteAll:
			ipcBuf = _L8("DeleteAll");
			break;		
		
		default:
			ipcBuf = _L8("OTHER");
			break;
		}

	if (aEntering!=EFalse)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_LOGREQUEST_1, ">>%d,CSimSmsStore::%s",aIpc, ipcBuf );
		}
	else
		{	
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_LOGREQUEST_2, "<<%d,CSimSmsStore::%s with error %d",aIpc, ipcBuf, aError);	
		}
	}




TInt CSimSmsStore::ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
/**
 * Dispatch function for all SMS Store requests.
 * @param aReqHandle	The TSY request handle for this request.
 * @param aIpc			The IPC number of this request.
 * @param aPckg			The parameter package related to this request.
 * @return TInt			The return error condition.
 */
	{
// The following requests can be completed even if the completion of another request is pending.
	TInt ret=KErrNone;//error is only used with LogRequest when at the end of ExtFunc
	LogRequest(ETrue, aIpc, ret);
	switch(aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		ret = GetInfo(aReqHandle,aPckg.Des1n());
		LogRequest(EFalse, aIpc, ret);
		return ret;

	case EMobilePhoneStoreNotifyStoreEvent:
		ret =  NotifyStoreEvent(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		LogRequest(EFalse, aIpc, ret);
		return ret;

	default:
		break;
		}
	

// The TSY can only process one of the following requests at a time.  If a second is received
// while processing the first, then it will be errored with KErrInUse.  This restriction will
// be removed later, by inserting a request queuing mechanism.  Note that the standard TSY
// "flow control" mechanism works phone-wide and so is not suitable.

	if(iTimer->IsActive())
		{
		ReqCompleted(aReqHandle,KErrInUse);
		return KErrNone;
		}

	switch(aIpc)
		{
	case EMobilePhoneStoreRead:
		ret = Read(aReqHandle,aPckg.Des1n());
		break;

	case EMobilePhoneStoreWrite:
		ret = Write(aReqHandle,aPckg.Des1n());
		break;

	case EMobilePhoneStoreDelete:
		ret = Delete(aReqHandle,aPckg.Des1n());
		break;

	case EMobilePhoneStoreReadAllPhase1:
		ret = ReadAllPhase1(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		break;

	case EMobilePhoneStoreReadAllPhase2:
		ret = ReadAllPhase2(aReqHandle,aPckg.Des1n(),aPckg.Des2n());
		break;

	case EMobilePhoneStoreDeleteAll:
		ret = DeleteAll(aReqHandle);
		break;

	default:
		return KErrNotSupported;
		}
	LogRequest(EFalse, aIpc, ret);
	return ret;

	}

CTelObject* CSimSmsStore::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimSmsStore::OpenNewObjectL(TDes&)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimSmsStore::ReqModeL(const TInt aIpc)
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
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreDelete:
	case EMobilePhoneStoreReadAllPhase1:
	case EMobilePhoneStoreReadAllPhase2:
	case EMobilePhoneStoreDeleteAll:
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

TInt CSimSmsStore::RegisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the first client makes a notification
 * request.  If supported by the underlying protocol controlling the
 * signalling stack, this can be used to start requesting updates for the relevant
 * service.
 */
	{
	return KErrNone;
	}

TInt CSimSmsStore::DeregisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the last client that had previously
 * made a notification request closes its ETel Server handle.  If supported by
 * the underlying protocol controlling the	signalling stack, this can be used
 * to stop requesting updates for the relevant service.
 */
	{
	return KErrNone;
	}

TInt CSimSmsStore::NumberOfSlotsL(const TInt /*aIpc*/)
/**
 * Return the number of slots that the ETel Server should allocate for buffering requests
 * of the given IPC number.
 */
	{
	return KDefaultNumberOfSlots;
	}

TInt CSimSmsStore::CancelService(const TInt aIpc,const TTsyReqHandle /*aTsyReqHandle*/)
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
	case EMobilePhoneStoreRead:
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreDelete:
		if(iTimer->Running())
			{
			iTimer->Cancel();
			ReqCompleted(iPendingReqCompletion,KErrCancel);
			}
		break;
	case EMobilePhoneStoreReadAllPhase1:
		ReadAllCancel();
		break;
	case EMobilePhoneStoreDeleteAll:
		DeleteAllCancel();
		break;
	case EMobilePhoneStoreNotifyStoreEvent:
		NotifyStoreEventCancel();
		break;
	default:
		break;
		}
	return KErrNone;
	}

void CSimSmsStore::Init()
/**
 *	This function can be used to perform any necessary synchronous initialisation.
 */
	{
	}

TInt CSimSmsStore::GetInfo(TTsyReqHandle aReqHandle, TDes8* aPckg)
/**
 * Retrieve SMS Store information. This request is completed immediately, as it is assumed
 * that in a real TSY, all this data will be cached in the TSY.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package associated with this request.
 */
	{
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg* getInfoPckg=(RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg*)aPckg;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1& getInfo=(*getInfoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(getInfo);
	if(err != KErrNone)
		{
		ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	getInfo.iType=RMobilePhoneStore::EShortMessageStore;
	getInfo.iTotalEntries=MaxSlots();
	getInfo.iCaps=iSmsStoreCaps;
	getInfo.iName.Copy(iSmsStoreName);
	getInfo.iUsedEntries=UsedEntries();

	ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimSmsStore::Read(TTsyReqHandle aReqHandle, TDes8* aPckg)
/**
 * Read a single SMS store entry.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to go and get the information from a SIM card.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package associated with this request.
 */
	{
	if (!(iSmsStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iSmsStoreCaps & RMobilePhoneStore::KCapsReadAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	RMobileSmsStore::TMobileSmsEntryV1Pckg* smsPckg=(RMobileSmsStore::TMobileSmsEntryV1Pckg*)aPckg;	
	RMobileSmsStore::TMobileSmsEntryV1& sms=(*smsPckg)();
		
	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(sms);
	if(err != KErrNone)
		{
		ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	switch (sms.ExtensionId())
		{
		case RMobileSmsStore::KETelMobileGsmSmsEntryV1:
			{
			RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* gsmSmsPckg=(RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*)aPckg;
			RMobileSmsStore::TMobileGsmSmsEntryV1& gsmSms=(*gsmSmsPckg)();

			// Check that the data structure is supported by the simulated TSY version
			TInt err = iPhone->CheckSimTsyVersion(gsmSms);
			if(err != KErrNone)
				{
				ReqCompleted(aReqHandle, err);
				return KErrNone;
				}

			TInt& index=gsmSms.iIndex;
			if((index<1)||(index>iSmsMaxNumSlots))
				{
				ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}
				
			if (iGsmSmsStoreEntries[index].iMsgStatus == KCorruptPDU)
				{
				ReqCompleted(aReqHandle,KErrCorrupt);
				return KErrNone;			    
				}

			if(iGsmSmsStoreEntries[index].iMsgData.Length()==0)
				{
				ReqCompleted(aReqHandle, KErrNotFound);
				return KErrNone;
				}

			gsmSms.iMsgData.Copy(iGsmSmsStoreEntries[index].iMsgData);
			gsmSms.iMsgStatus=iGsmSmsStoreEntries[index].iMsgStatus;
			gsmSms.iServiceCentre=iGsmSmsStoreEntries[index].iServiceCentre;
			break;
			}
		case RMobileSmsStore::KETelMobileCdmaSmsEntryV1:
			 ReqCompleted(aReqHandle,KErrNotSupported);
			 return KErrNone;	

		default:
			ReqCompleted(aReqHandle,KErrArgument);
			return KErrNone;
		}
	
	DelayCompletion(iSmsIndividualPause,aReqHandle);
	return KErrNone;
	}

TInt CSimSmsStore::Write(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Write a single SMS store entry.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to write the information to a SIM card. A store
 * event may be triggered by this request.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package associated with this request.
 */
	{
	if (!(iSmsStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iSmsStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	TStoreEvent event=EStoreEventNoEvent;
	TInt index;

	RMobileSmsStore::TMobileSmsEntryV1Pckg* smsPckg=(RMobileSmsStore::TMobileSmsEntryV1Pckg*)aPckg;	
	RMobileSmsStore::TMobileSmsEntryV1& sms=(*smsPckg)();
		
	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(sms);
	if(err != KErrNone)
		{
		ReqCompleted(aReqHandle, err);
		return KErrNone;
		}

	switch (sms.ExtensionId())
		{
		case RMobileSmsStore::KETelMobileGsmSmsEntryV1:
			{
			RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* gsmSmsPckg=(RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*)aPckg;
			RMobileSmsStore::TMobileGsmSmsEntryV1& gsmSms=(*gsmSmsPckg)();

			// Check that the data structure is supported by the simulated TSY version
			TInt err = iPhone->CheckSimTsyVersion(gsmSms);
			if(err != KErrNone)
				{
				ReqCompleted(aReqHandle, err);
				return KErrNone;
				}

			index=gsmSms.iIndex;
			if(index==-1)//check the 1st free slot
				{
				for(TInt i=1;i<=iSmsMaxNumSlots;i++)
					{
					if(iGsmSmsStoreEntries[i].iMsgData.Length()==0)
						{
						index=i;
						//ret = KErrNone;
						break;
						}
					}
				}
			//else ret = KErrNone;//if index is specified entry will be overwitten, not possible to exceed memory
			if((index<1)||(index>iSmsMaxNumSlots))
				{
				ReqCompleted(aReqHandle,KErrArgument);
				return KErrNone;
				}

			//
			// This can never happen because either ret is KErrNone or
			// index is still -1 and we have returned.
			// 
				//if (ret==KErrNoMemory)
					//ReqCompleted(aReqHandle,ret);

			if(iGsmSmsStoreEntries[index].iMsgData.Length()==0)
				event=EStoreEventAdded;
			else
				event=EStoreEventChanged;

			gsmSms.iIndex = index;		// Update index data member for the client
			iGsmSmsStoreEntries[index].iIndex=index;
			iGsmSmsStoreEntries[index].iMsgData.Copy(gsmSms.iMsgData);
			iGsmSmsStoreEntries[index].iMsgStatus=gsmSms.iMsgStatus;
			iGsmSmsStoreEntries[index].iServiceCentre=gsmSms.iServiceCentre;
			break;
			}
		case RMobileSmsStore::KETelMobileCdmaSmsEntryV1:
			 ReqCompleted(aReqHandle,KErrNotSupported);
			 return KErrNone;		

		default:
			ReqCompleted(aReqHandle,KErrArgument);
			return KErrNone;
		}

	DelayCompletion(iSmsIndividualPause,aReqHandle,event,index);
	return KErrNone;
	}

TInt CSimSmsStore::StoreIncomingMessage(RMobileSmsStore::TMobileGsmSmsEntryV1* aSms)
/**
 * This method is used to store Incoming sms messages with the status EMtMessageStored
 * It is accessed by CSimSmsMessaging::AttemptSmsRxCompleteL.
 * Note: this will not call ReqComplete as no client request has been posted on this class
 * Note: the smsStack will delete the message from the store after ReceiveMessage has completed
 **/
	{
	
	TInt& index=aSms->iIndex;
	TInt ret=KErrNoMemory;
	TStoreEvent event=EStoreEventNoEvent;
	for(TInt i=1;i<=iSmsMaxNumSlots;i++)
		{
		if(iGsmSmsStoreEntries[i].iMsgData.Length()==0)
			{
			index=i;
			event=EStoreEventAdded;
			ret = KErrNone;
			break;
			}
			
		}

	aSms->iIndex=index;//set here to be used by CSimSmsMessaging
	
	if (ret==KErrNone)
		{
		iGsmSmsStoreEntries[index].iIndex=index;
		iGsmSmsStoreEntries[index].iMsgData.Copy(aSms->iMsgData);
		iGsmSmsStoreEntries[index].iMsgStatus=aSms->iMsgStatus;
		iGsmSmsStoreEntries[index].iServiceCentre=aSms->iServiceCentre;

		iPendingEvent=event;
		iPendingIndex=index;
		iIncomingTimer->Start(iSmsIndividualPause,this, 1);//delay the notification 
		}

	return ret;
	}


TInt CSimSmsStore::Delete(TTsyReqHandle aReqHandle,TDes8* aPckg)
/**
 * Delete a single SMS store entry.  The completion of this request is delayed in order to
 * simulate a real TSY that would have to write the information to a SIM card. A store
 * event may be triggered by this request.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg			The parameter package associated with this request.
 * @return				Standard return value.
 */
	{
	if (!(iSmsStoreCaps & RMobilePhoneStore::KCapsIndividualEntry && iSmsStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	TPckg<TInt>* intPckg=(TPckg<TInt>*)aPckg;
	TInt& index=(*intPckg)();

	if((index<1)||(index>iSmsMaxNumSlots))
		{
		ReqCompleted(aReqHandle,KErrArgument);
		return KErrNone;
		}

	iGsmSmsStoreEntries[index].iMsgData.Zero();
	iGsmSmsStoreEntries[index].iMsgStatus=RMobileSmsStore::EStoredMessageUnknownStatus;
	iGsmSmsStoreEntries[index].iServiceCentre.iTelNumber=KNullTelephoneNumber;
	
	DelayCompletion(iSmsIndividualPause,aReqHandle,EStoreEventDeleted,index);
	return KErrNone;
	}

TInt CSimSmsStore::ReadAllPhase1(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2)
/**
 * First phase of a batch SMS Store read.  The completion of this request is delayed in
 * order to simulate a real TSY that would have to get the information from a SIM card.
 * If the GetSmspListPhase1L should leave this method takes care of that and 
 * makes a premature ReqCompleted to the client.
 * 
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						It contains the client ID information required to uniquely identify this request.
 *						It also contains the batch read information (first slot, number of slots).
 * @param aPckg2		The parameter package associated with this request.
 *						The size of the information to be passed back to the client is returned in this package.
 * @return				Standard return value.
 */	{
 	if (!(iSmsStoreCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore) && iSmsStoreCaps & RMobilePhoneStore::KCapsReadAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}
		

	TPckg<CRetrieveMobilePhoneSmsList::TBatchRequestData>* clientIdPckg=(TPckg<CRetrieveMobilePhoneSmsList::TBatchRequestData>*) aPckg1;
	CRetrieveMobilePhoneSmsList::TBatchRequestData& clientId=(*clientIdPckg)();
	TPckg<TInt>* sizePckg=(TPckg<TInt>*)aPckg2;
	TInt& size=(*sizePckg)();

	TRAPD(leaveCode,ReadAllPhase1L(aReqHandle,clientId,size));

	return leaveCode;
	}




void CSimSmsStore::ReadAllPhase1L(TTsyReqHandle aReqHandle,CRetrieveMobilePhoneSmsList::TBatchRequestData& aClientId, 
											 TInt& aBufSize)
/**
 * Implementation of ReadAllPhase1. 
 *
 */

	{
	TInt cnt=0;
	CMobilePhoneListBase* listBase = NULL;

	if ((CSimTsyMode::GetMode() != CSimTsyMode::ECdmaV1) &&
		(aClientId.iEntryType == RMobileSmsStore::KETelMobileGsmSmsEntryV1))
		{
		listBase = CMobilePhoneGsmSmsList::NewL();
		}
	else
		{
		// Nothing to read.
		ReqCompleted(aReqHandle, KErrNone);
		return;
		}

	CleanupStack::PushL(listBase);

	if(!aClientId.iBatchRequest)
		{
		aClientId.iStartIndex=1;
		aClientId.iBatchSize=iSmsMaxNumSlots;
		}

	for(TInt i = aClientId.iStartIndex; cnt < aClientId.iBatchSize; i++)
		{
		if (i > iSmsMaxNumSlots)
			break;
		
		if (aClientId.iEntryType == RMobileSmsStore::KETelMobileGsmSmsEntryV1)
			{
			if (iGsmSmsStoreEntries[i].iMsgData.Length() != 0)
				{
				((CMobilePhoneGsmSmsList*)listBase)->AddEntryL(iGsmSmsStoreEntries[i]);
				cnt++;
				}
			}

		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aClientId.iClient, aReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = listBase->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iSmsReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufFlat’s size to client			
	aBufSize=(read->iListBuf)->Size();		
	
	CleanupStack::PopAndDestroy(); // pop&destroy list
	// Complete first phase of list retrieval

	DelayCompletion(iSmsBatchPause,aReqHandle);
//	return KErrNone;
	}

TInt CSimSmsStore::ReadAllPhase2(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aBuffer)
/**
 * Second phase of a batch SMS Store read.  The completion of this request is not delayed
 * as it is assumed that the TSY has already cached the required information (during the
 * first phase of the read).
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @param aPckg1		The first parameter package associated with this request.
 *						It contains the client ID information required to uniquely identify this request.
 * @param aBuffer		The parameter package associated with this request.
 *						It contains the read buffer in which the required information is returned.
 * @return				Standard return value.
 */
	{
	TPckg<RMobilePhone::TClientId>* clientIdPckg=(TPckg<RMobilePhone::TClientId>*) aPckg1;
	RMobilePhone::TClientId& clientId=(*clientIdPckg)();

	for (TInt i=0; i<iSmsReadAll->Count(); ++i)
		{
		CListReadAllAttempt* read = iSmsReadAll->At(i);
		if ((read->iClient.iSessionHandle==clientId.iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==clientId.iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iSmsReadAll->Delete(i);
			ReqCompleted(aReqHandle,KErrNone);
			return KErrNone;
			}
		}

// Don't delay completion of this request, as we assume the TSY will have the results in
// memory from the first request.
	ReqCompleted(aReqHandle,KErrNotFound);
	return KErrNone;
	}

TInt CSimSmsStore::DeleteAll(TTsyReqHandle aReqHandle)
/**
 * Delete all entries in the SIM Store.  The completion of this function is delayed in
 * order to simulate the SIM operations a real TSY would have to carry out.  This function
 * may trigger an SMS Store notification.
 *
 * @param aReqHandle	The TSY request handle associated with this request.
 * @return				Standard return value.
 */
	{
	if (!(iSmsStoreCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore) && iSmsStoreCaps & RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aReqHandle, KErrAccessDenied);
		return KErrNone;
		}

	for(TInt i=1;i<=iSmsMaxNumSlots;i++)
		{
	
			iGsmSmsStoreEntries[i].iIndex=i;
			iGsmSmsStoreEntries[i].iMsgStatus=RMobileSmsStore::EStoredMessageUnknownStatus;
			iGsmSmsStoreEntries[i].iMsgData.Zero();
			iGsmSmsStoreEntries[i].iServiceCentre.iTelNumber=KNullTelephoneNumber;
			
		}

	DelayCompletion(iSmsBatchPause,aReqHandle,EStoreEventDeleted,-1);
	return KErrNone;
	}

TInt CSimSmsStore::NotifyStoreEvent(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2)
/**
 * Register a client's interest in SMS Store events.
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
	if (!(iSmsStoreCaps & RMobilePhoneStore::KCapsNotifyEvent))
		{
		ReqCompleted(aReqHandle, KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(iEvOutstandingReq==EFalse,SimPanic(ENotificationAlreadyPending));
	
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

void CSimSmsStore::NotifyStoreEventCancel()
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

//coverity[bad_override]
TPtrC8 CSimSmsStore::Name()
/**
 * Accessor function fot the SMS Store name.
 *
 * @return TPtrC8	The name of this SMS Store.
 */
	{
	return iSmsStoreName;
	}

TInt CSimSmsStore::UsedEntries()
/**
 * Count the number of used entries in the SMS Store.
 * @return TInt	The number of used entries in the store.
 */
	{
	TInt cnt=0;
	for(TInt i=1;i<=iSmsMaxNumSlots;i++)
		{
		if(iGsmSmsStoreEntries[i].iMsgData.Length()!=0)
		cnt++;		
		}
	return cnt;
	}

TInt CSimSmsStore::MaxSlots()
/**
 * Retrieve the maximum number of slots in this SMS Store.
 * @return TInt	The maximum number of slots in this SMS Store.
 */
	{
	return iSmsMaxNumSlots;
	}

void CSimSmsStore::DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle)
/**
 * A shell function for functions that wish to delay completion but do not trigger store
 * events.
 */
	{
	DelayCompletion(aDelayDuration,aReqHandle,EStoreEventNoEvent,0);
	}

TUint CSimSmsStore::StoreCaps()
	{
	return iSmsStoreCaps;
	}

void CSimSmsStore::DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle,TStoreEvent aEvent,TInt aIndex)
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
	iTimer->Start(aDelayDuration,this);
	}


void CSimSmsStore::ReadAllCancel()
/*
 * Cancel an outstanding read/write smsp.
 */
	{
	iTimer->Cancel();
	ReqCompleted(iPendingReqCompletion,KErrCancel);
	}

void CSimSmsStore::DeleteAllCancel()
/*
 * Cancel an outstanding read/write smsp.
 */
	{
	iTimer->Cancel();
	ReqCompleted(iPendingReqCompletion,KErrNone);//The timer is cancelled but data has already been deleted
	}

void CSimSmsStore::TimerCallBack(TInt aId)
/**
 * Process a timer call back event.  The timer is used to kick requests which have had their
 * completions delayed.
 */
	{
	StoreEvent(iPendingEvent,iPendingIndex);
	if (aId!=1)//aId is 1 if reuest is StoreIncomingMessage(coming from CSimSmsMessaging)
		ReqCompleted(iPendingReqCompletion,KErrNone);
	}

void CSimSmsStore::StoreEvent(TStoreEvent aEvent,TInt aIndex)
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
			}

		if(UsedEntries()==0)
			event|=RMobilePhoneStore::KStoreEmpty;

		if(UsedEntries()==iSmsMaxNumSlots)
			event|=RMobilePhoneStore::KStoreFull;
		else
			event|=RMobilePhoneStore::KStoreHasSpace;

		*iEvEvent=event;
		*iEvIndex=aIndex;
		iEvOutstandingReq=EFalse;
		ReqCompleted(iEvReqHandle,KErrNone);
		}
	}

const CTestConfigSection* CSimSmsStore::CfgFileSection()
/**
* Returns a pointer to the config file section
*
* @return CTestConfigSection a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMSSTORE_CFGFILESECTION_1, ">>CSimSmsStore::CfgFileSection");
	return iSmsMessaging->CfgFileSection();
	}

