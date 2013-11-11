// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements Sms Storage Extensions
// 
//

#include <et_phone.h>
#include "mSMSREAD.H"
#include "mSMSWRIT.H"
#include "mSMSDEL.H"
#include "mSMSSTOR.H"
#include "ATINIT.H"
#include "mSLOGGER.H"
#include "smsutil.h"

// This class implements functionality to get info, entries and all stored entries from the SMS stores in the phone.
CMobileSmsStore* CMobileSmsStore::NewL(CATIO* aATIO, CATInit* aInit, CPhoneGlobals* aPhoneGlobals, TStorageType aStoreType)
	{
	CMobileSmsStore* subsession=new(ELeave) CMobileSmsStore(aATIO, aInit, aPhoneGlobals, aStoreType);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

TInt CMobileSmsStore::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	__ASSERT_ALWAYS((aIpc==EMobilePhoneStoreDelete) || (aIpc==EMobilePhoneStoreRead) || (aIpc==EMobilePhoneStoreWrite) ||
					(aIpc==EMobilePhoneStoreGetInfo), PanicClient(KErrUnknown));
	switch(aIpc)
		{
		case EMobilePhoneStoreWrite:
			return WriteCancel(aTsyReqHandle);
		case EMobilePhoneStoreRead:
			return ReadCancel(aTsyReqHandle);
		case EMobilePhoneStoreDelete:
			return DeleteCancel(aTsyReqHandle);
		case EMobilePhoneStoreGetInfo:
			return GetInfoCancel(aTsyReqHandle);
		default:
			return KErrNotSupported;
		}//switch
	}
	
void CMobileSmsStore::Init()
	{
	}

CMobileSmsStore::CMobileSmsStore(CATIO* aIo, CATInit* aInit, CPhoneGlobals* aPhoneGlobals, TStorageType aStoreType)
	:iIo(aIo)
	,iInit(aInit)
	,iPhoneGlobals(aPhoneGlobals)
	{
	iStoreName=aStoreType; // provides name for this object, ie: SM, ME, MT
	}

void CMobileSmsStore::ConstructL()
	{
	iATSmsStorageRead = CATSmsMessagingRead::NewL(iIo, this, iInit, iPhoneGlobals);
	iATSmsStorageWrite = CATSmsMessagingWrite::NewL(iIo, this, iInit, iPhoneGlobals);
	iATSmsStorageDelete = CATSmsStorageDelete::NewL(iIo, this, iInit, iPhoneGlobals);
	iATSmsStorageGetInfo = CATSmsMemoryStorage::NewL(iIo, this, iInit, iPhoneGlobals);
	}

CMobileSmsStore::~CMobileSmsStore()
	{
	delete iATSmsStorageRead;
	delete iATSmsStorageWrite;
	delete iATSmsStorageDelete;
	delete iATSmsStorageGetInfo;
	}

CTelObject* CMobileSmsStore::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CMobileSmsStore::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CMobileSmsStore::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// Sms Storage
//
	case EMobilePhoneStoreGetInfo:
		ret = KReqModeMultipleCompletionEnabled|KReqModeFlowControlObeyed;
		break;
	
	case EMobilePhoneStoreRead:
	case EMobilePhoneStoreWrite:
	case EMobilePhoneStoreDelete:
		ret=KReqModeFlowControlObeyed;
		break;
	
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	// Check if the data port is currently loaned. If it is and the requested IPC
	// is flow controlled then block Etel calling the IPC by leaving with KErrInUse
	if((ret&KReqModeFlowControlObeyed) && iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		LOGTEXT2(_L8("ReqModeL Leaving with KErrInUse as data port is loaned (aIpc=%d)"),aIpc);
		User::Leave(KErrInUse);
		}

	return ret;
	}

TInt CMobileSmsStore::NumberOfSlotsL(const TInt /*aIpc*/)
	{
	// Return the number of slots (buffered in server)
	// for any KReqRepostImmediately ipc calls in the above ReqModeL function
	User::Leave(KErrNotSupported);
	return 0; //just to make compiler happy
	}

TInt CMobileSmsStore::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

TInt CMobileSmsStore::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

TInt CMobileSmsStore::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc, const TDataPackage& aPackage)
	{
// Prior to dispatch check that we're not setting up or in the middle of a data or fax call
	if((iPhoneGlobals->iPhoneStatus.iPortAccess==EPortAccessDenied) || (iPhoneGlobals->iPhoneStatus.iMode == RPhone::EModeOnlineData))
		{
		LOGTEXT2(_L8("CMobileSmsStore::ExtFunc (aIpc=%d)"),aIpc);
		LOGTEXT(_L8("CMobileSmsStore::ExtFunc\tPort Access Denied/Mode Online flag detected"));
		switch(aIpc)
			{
// These may interfere with the Fax, so error the request now...
		case EMobilePhoneStoreGetInfo:
		case EMobilePhoneStoreRead:
		case EMobilePhoneStoreWrite:
		case EMobilePhoneStoreDelete:
			LOGTEXT(_L8("CMobileSmsStore::ExtFunc\tReturning KErrAccessDenied error"));
			return KErrAccessDenied;

		default:
			break;
			}
		}

	TAny* dataPtr=aPackage.Ptr1();
	
	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		return  GetInfo(aTsyReqHandle, aPackage.Des1n());
	case EMobilePhoneStoreRead:
		return  Read(aTsyReqHandle, aPackage.Des1n());
	case EMobilePhoneStoreWrite:
		return  Write(aTsyReqHandle,aPackage.Des1n());
	case EMobilePhoneStoreDelete: 
		return  Delete(aTsyReqHandle, reinterpret_cast<TInt*>(dataPtr));
	default:
		return KErrNotSupported;
		}
	}

TInt CMobileSmsStore::GetInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfoPckg)
/** Get SMS store Inforamtion
 *
 * This method starts retrieval of information about the current SMS store.
 * When the request is completed information about total number of slots, number 
 * of used slots, type of store and store capabilities is returned to the client.
 * @param aTsyReqHandle Handle to the request ID.
 * @param aInfoPckg A packet of type TMobilePhoneStoreInfoV1Pckg
 * @return TInt error code
 */
	{// special case, can't just call ExecuteCommand, because this'll call the WRONG Start() function, therefore only call it for re-start init etc.
	LOGTEXT(_L8("CMobileSmsStore\tSMS Store Get Info called."));

	if (iPhoneGlobals->iPhoneStatus.iInitStatus != EPhoneInitialised)
		ReqCompleted(aTsyReqHandle,KErrNotReady);

	iATSmsStorageGetInfo->CopyDataFromCATInit(iInit);
	iATSmsStorageGetInfo->StartGetInfo(aTsyReqHandle,aInfoPckg);

	return KErrNone;
	}

TInt CMobileSmsStore::Read(const TTsyReqHandle aTsyReqHandle, TDes8* aEntryPckg)
/** Read a Fixed Size SMS entry
 *
 * This method starts the retrieval of a fixed size SMS entry. The entry to read  
 * is specified by the index(entry.iIndex) in TMobileSmsEntryV1. During the 
 * retrieval of the specified entry the iFormat and iFlags fields are set as well.
 * @param aTsyReqHandle Handle to the request ID.
 * @param aEntryPckg A packet of type TMobilePhoneStoreInfoV1Pckg
 * @return TInt error code
 */
 	{
	LOGTEXT(_L8("CMobileSmsStore\tSMS Store Read called."));
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*  smsEntryPckg = static_cast<RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*>(aEntryPckg);
	RMobileSmsStore::TMobileGsmSmsEntryV1& entry = (*smsEntryPckg)();

	if (entry.ExtensionId()!=RMobileSmsStore::KETelMobileGsmSmsEntryV1)
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
	else
		iATSmsStorageRead->ExecuteCommand(aTsyReqHandle, &entry);

	return KErrNone;
	}


TInt CMobileSmsStore::Write(const TTsyReqHandle aTsyReqHandle, TDes8* aEntryPckg)
/** Write a SMS Entry to the store
 *
 * This method starts the writing of a SMS entry to the mobilephone. The entry written 
 * to the phone allways has a prepended SCA header({0x01,0x80}), regardless of what the 
 * client puts in the packet which is unpacket here. This is due to a lack of erorror
 * responses from some phones (Ericsson T28 and R320).
 * @param aTsyReqHandle Handle to the request ID.
 * @param aEntryPckg A packet of type TMobilePhoneStoreInfoV1Pckg
 * @return TInt error code
 */
	{
	LOGTEXT(_L8("CMobileSmsStore\tSMS Store Write called."));
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*  smsEntryPckg = static_cast<RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*>(aEntryPckg);
	RMobileSmsStore::TMobileGsmSmsEntryV1& entry = (*smsEntryPckg)();
		
	if (entry.ExtensionId()!=RMobileSmsStore::KETelMobileGsmSmsEntryV1)
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
	else
		iATSmsStorageWrite->ExecuteCommand(aTsyReqHandle, &entry);
	
	return KErrNone;
	}																	

TInt CMobileSmsStore::Delete(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
/** Delete a SMS Message
 *
 * This methos deletes the SMS entry defined by aIndex.
 * @param aTsyReqHandle Handle to the request ID.
 * @param aIndex The index of the SMS entry to delete.
 * @return TInt error code.
 */

	{
	LOGTEXT(_L8("CMobileSmsStore\tSMS Store Delete called."));
	iATSmsStorageDelete->ExecuteCommand(aTsyReqHandle, aIndex);
		
	return KErrNone;
	}

TInt CMobileSmsStore::WriteCancel(const TTsyReqHandle aTsyReqHandle)
/** Write Cancel
 *
 * This method cancels an outstanding request to write a SMS message to 
 * the current memory(ME or SM).
 * @param aTsyReqHandle Handle to the request ID.
 * @return TInt error code.
 */
	{
	iATSmsStorageWrite->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


TInt CMobileSmsStore::ReadCancel(const TTsyReqHandle aTsyReqHandle)
/** Read Cancel
 *
 * This method cancels a outstanding request to read a SMS message
 * from the current memory(ME or SM).
 * @param aTsyReqHandle Handle to the request ID.
 * @return TInt error code.
 */
	{
	LOGTEXT(_L8("CMobileSmsStore\tSMS Store Read Cancel called."));
	iATSmsStorageRead->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


TInt CMobileSmsStore::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
/** Delete Cancel
 *
 * This method cancels a outstanding request to write a SMS message 
 * to the current memory(ME or SM).
 * @param aTsyReqHandle Handle to the request ID.
 * @return TInt error code.
 */
	{
	LOGTEXT(_L8("CMobileSmsStore\tSMS Store Delete Cancel called."));
	iATSmsStorageDelete->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


TInt CMobileSmsStore::GetInfoCancel(const TTsyReqHandle aTsyReqHandle)
/** Delete Cancel
 *
 * This method cancels a outstanding request to get information about the 
 * current memory(ME or SM).
 * to the current memory.
 * @param aTsyReqHandle Handle to the request ID.
 * @return TInt error code.
 */
	{
	LOGTEXT(_L8("CMobileSmsStore\tSMS Store Get Info Cancel called."));
	iATSmsStorageGetInfo->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}
