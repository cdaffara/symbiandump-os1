
// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains methods for all the PtrHolder classes used by the EtelMM
// subsessions.
// 
//

// From core API
#include <et_clsvr.h>
#include <etelext.h>

// Multimode header files
#include <etelmm.h>
#include <mmlist.h>
#include <mmretrieve.h>

#include "mm_hold.h"


/************************************************************************/
//
//  RMobilePhoneStore
//
/************************************************************************/

RMobilePhoneStore::RMobilePhoneStore()
	:iStorePtrHolder(NULL)
/** Constructor. */
	{
	}

EXPORT_C void RMobilePhoneStore::BaseConstruct(CMobilePhoneStorePtrHolder* aPtrHolder)
/**
@capability None
*/
	{
	iStorePtrHolder = aPtrHolder;
	}

EXPORT_C void RMobilePhoneStore::Destruct()
/** Standard destructor. 
@capability None
*/
	{
	delete iStorePtrHolder;
	iStorePtrHolder = NULL;
	ResetSessionHandle();
	}

RMobilePhoneStore::TMobilePhoneStoreEntryV1::TMobilePhoneStoreEntryV1()
:	iIndex(KIndexNotUsed)
	{
	iExtensionId = KETelExtMultimodeV1;
	}

void RMobilePhoneStore::TMobilePhoneStoreEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the generic attributes of a store entry from a stream
 *
 * @param aStream The read stream containing the store entry
 */
	{
	TMultimodeType::InternalizeL(aStream);
	iIndex = aStream.ReadInt32L();
	}

void RMobilePhoneStore::TMobilePhoneStoreEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the generic attributes of a store entry into a stream
 *
 * @param aStream The write stream that will contain the store entry
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream.WriteInt32L(iIndex);
	}

EXPORT_C RMobilePhoneStore::TMobilePhoneStoreInfoV1::TMobilePhoneStoreInfoV1()
:	iType(EPhoneStoreTypeUnknown),
	iTotalEntries(-1),
	iUsedEntries(-1),
	iCaps(0)
	{
	iExtensionId=KETelMobilePhoneStoreV1;
	}

EXPORT_C void RMobilePhoneStore::GetInfo(TRequestStatus& aReqStatus, TDes8& aInfo) const
/** This member function gets information about the selected store.

This information includes the store's name, the type of thing it stores, the
total number of storage slots and the number of used slots. It does not give
information about which slots are filled and which are empty.

Defined in RMobilePhoneBookStore::TMobilePhoneStoreInfoV1::iExtensionId is
the type of class that is being used. See TMobilePhoneStoreInfoExtId for the
different possible types.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreGetInfo) to cancel
a previously placed asynchronous GetInfo() request.

@param aReqStatus On return, KErrNone if successful.
@param aInfo On completion, a from RMobilePhoneBookStore::TMobilePhoneBookInfoV1
derived packaged class. 
@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	Get(EMobilePhoneStoreGetInfo, aReqStatus, aInfo);
	}

EXPORT_C void RMobilePhoneStore::Read(TRequestStatus& aReqStatus, TDes8& aEntry) const
/** This overloaded member function reads the entry in the slot specified.

This member function is implemented using a TDes8 descriptor for aEntry so
that any kind of store entry can be read from any kind of store using this
common member function. For example, a client will have opened one of the
specialised store sub-sessions, e.g. RMobileONStore. The client will read
a store entry by packaging the specialised store's entry definition into its
associated packaged version. In the own number store example, the client will
have a RMobileONStore::TMobileONEntryV1 which it will put into a RMobileONStore::TMobileONEntryV1Pckg.
The packaged version will be passed to the Read() request.

This function has a specialised version in the RMobilePhoneBookStore

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreRead) to cancel
a previously placed asynchronous Read() request.

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the slot
specified by iIndex is empty.
@param aEntry The iIndex field specifies what entry to read. On completion,
the entry data will be filled in. 
@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	Get(EMobilePhoneStoreRead, aReqStatus, aEntry);
	}

EXPORT_C void RMobilePhoneStore::Write(TRequestStatus& aReqStatus, TDes8& aEntry)  const
/** This overloaded member function writes the entry into the store.

This member function is implemented using a TDes8 descriptor for aEntry so
that any kind of store entry can be read from any kind of store using this
common member function. For example, a client will have opened one of the
specialised store sub-sessions, eg. RMobileONStore. The client will read a
store entry by packaging the specialised store's entry definition into its
associated packaged version. In the own number store example, the client will
have a RMobileONStore::TMobileONEntryV1 which it will put into a RMobileONStore::TMobileONEntryV1Pckg.
The packaged version will be passed to the Write() request.

This function has a specialised version in the RMobilePhoneBookStore

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreWrite) to cancel
a previously placed asynchronous Write() request.

@param aReqStatus On return, KErrNone if successful.
@param aEntry The packaged entry to write in the store. If aEntry.index==-1
the TSY will store the entry in the first free location and then return this
location within aEntry.iIndex when it completes the request 
@capability WriteUserData
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	Get(EMobilePhoneStoreWrite, aReqStatus, aEntry);
	}

EXPORT_C void RMobilePhoneStore::Delete(TRequestStatus& aReqStatus, TInt aIndex)  const
/** This member function deletes the entry in the slot.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreDelete) to cancel
a previously placed asynchronous Delete() request.

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the slot
is empty
@param aIndex The slot entry to delete. 
@capability WriteUserData
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iStorePtrHolder->iDeleteIndex=aIndex;
	TPtrC8& ptr1=iStorePtrHolder->SetC(CMobilePhoneStorePtrHolder::ESlot1Delete,iStorePtrHolder->iDeleteIndex);

	Set(EMobilePhoneStoreDelete, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhoneStore::DeleteAll(TRequestStatus& aReqStatus)  const
/** This member function deletes all the entries in the store.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreDeleteAll) to
cancel a previously placed asynchronous DeleteAll() request.

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the store
is empty 
@capability WriteUserData
*/
	{
	Blank(EMobilePhoneStoreDeleteAll, aReqStatus);
	}

EXPORT_C void RMobilePhoneStore::NotifyStoreEvent(TRequestStatus& aReqStatus, TUint32& aEvent, TInt& aIndex) const
/** This member functions allows a client to be notified when one or more of the
events occurs to the store.

These events allow a client to know the overall status of a store (full, empty
etc.) and also when a slot within a store changes. A slot could change because
another client or the phone has created, updated or deleted an entry.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent)
to cancel a previously placed asynchronous NotifyStoreEvent() request.

@param aReqStatus On return, KErrNone if successful
@param aEvent On completion, the TMobileStoreEvent.
@param aIndex On completion, the index of the changed slot. If multiple entries
have been added or deleted then it will not be possible to return the indices
of these entries, so the TSY could trigger the KStoreDoRefresh event. 
@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iStorePtrHolder->Set(CMobilePhoneStorePtrHolder::ESlot1NotifyStoreEvent,aEvent);
	TPtr8& ptr2=iStorePtrHolder->Set(CMobilePhoneStorePtrHolder::ESlot2NotifyStoreEvent,aIndex);
	Get(EMobilePhoneStoreNotifyStoreEvent, aReqStatus, ptr1, ptr2);
	}

/************************************************************************/
//
//  RMobileSmsStore
//
/************************************************************************/

EXPORT_C RMobileSmsStore::RMobileSmsStore()
/** Constructor */
	{
	}

EXPORT_C TInt RMobileSmsStore::Open(RMobileSmsMessaging& aMessaging, const TDesC& aStoreName)
/** This member function opens the message store.

@param aMessaging The Sms Messaging sub-session to use.
@param aStoreName Name of the store to open, the names of the stores can be
retrieved with RMobileSmsMessaging::GetMessageStoreInfo(). Must be one of
KETelMeSmsStore, KETelIccSmsStore, KETelCombinedSmsStore
@return KErrNone if successful.
@see RMobileSmsMessaging::GetMessageStoreInfo() 
@capability None
*/
	{
	RSessionBase* session=&aMessaging.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aMessaging.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		return ret;
	TIpcArgs args(const_cast<TDesC*>(&aStoreName),TIpcArgs::ENothing,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobileSmsStore::Close()
/** This member function closes a message store sub-session. 
@capability None
*/
 	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RMobileSmsStore::ConstructL()
/** Constructor. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	CMobilePhoneStorePtrHolder* ptrHolder = CSmsStorePtrHolder::NewL(CSmsStorePtrHolder::EMaxNumberSmsStorePtrSlots,CSmsStorePtrHolder::EMaxNumberSmsStorePtrCSlots);
	RMobilePhoneStore::BaseConstruct(ptrHolder);
	}

RMobileSmsStore::TMobileSmsEntryV1::TMobileSmsEntryV1()
: iMsgStatus(EStoredMessageUnknownStatus)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

void RMobileSmsStore::TMobileSmsEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the SMS entry from a stream
 *
 * @param aStream The read stream containing the SMS fixed size entry
 */
	{
	TMobilePhoneStoreEntryV1::InternalizeL(aStream);
	iMsgStatus = static_cast<TMobileSmsStoreStatus>(aStream.ReadUint32L());
	}

void RMobileSmsStore::TMobileSmsEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the SMS entry into a stream
 *
 * @param aStream The write stream that will contain the SMS entry
 */
	{
	TMobilePhoneStoreEntryV1::ExternalizeL(aStream);
	aStream.WriteUint32L(iMsgStatus);
	}

EXPORT_C RMobileSmsStore::TMobileGsmSmsEntryV1::TMobileGsmSmsEntryV1()
	{
	iExtensionId=KETelMobileGsmSmsEntryV1;
	}

void RMobileSmsStore::TMobileGsmSmsEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the SMS entry from a stream
 *
 * @param aStream The read stream containing the SMS fixed size entry
 */
	{
	TMobileSmsEntryV1::InternalizeL(aStream);
	aStream >> iServiceCentre;
	aStream >> iMsgData;
	}

void RMobileSmsStore::TMobileGsmSmsEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the SMS entry into a stream
 *
 * @param aStream The write stream that will contain the SMS entry
 */
	{
	TMobileSmsEntryV1::ExternalizeL(aStream);
	aStream << iServiceCentre;
	aStream << iMsgData;
	}

EXPORT_C RMobileSmsStore::TMobileCdmaSmsEntryV1::TMobileCdmaSmsEntryV1()
: iTeleservice(0), iServiceCategory(0)
	{
	iExtensionId=KETelMobileCdmaSmsEntryV1;
	}

void RMobileSmsStore::TMobileCdmaSmsEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the SMS entry from a stream
 *
 * @param aStream The read stream containing the SMS fixed size entry
 */
	{
	TMobileSmsEntryV1::InternalizeL(aStream);
	iTeleservice = aStream.ReadInt32L();
	iServiceCategory = aStream.ReadInt32L();
	aStream >> iRemoteParty;
	aStream >> iMsgData;
	}

void RMobileSmsStore::TMobileCdmaSmsEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the SMS entry into a stream
 *
 * @param aStream The write stream that will contain the SMS entry
 */
	{
	TMobileSmsEntryV1::ExternalizeL(aStream);
	aStream.WriteInt32L(iTeleservice);
	aStream.WriteInt32L(iServiceCategory);
	aStream << iRemoteParty;
	aStream << iMsgData;
	}


/************************************************************************/
//
//  RMobileNamStore
//
/************************************************************************/


EXPORT_C RMobileNamStore::RMobileNamStore()
/** Constructor. */
	{
	}


EXPORT_C void RMobileNamStore::ConstructL()
/** Initializes and allocated the members of the RMobileNamStore object.

@leave KErrNoMemory Out of memory 
@capability None
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	CMobilePhoneStorePtrHolder* ptrHolder = CNamStorePtrHolder::NewL(CNamStorePtrHolder::EMaxNumberNamStorePtrSlots,CNamStorePtrHolder::EMaxNumberNamStorePtrCSlots);
	RMobilePhoneStore::BaseConstruct(ptrHolder);
	}

EXPORT_C TInt RMobileNamStore::Open(RMobilePhone& aPhone)
/** This member function opens a RMobileNamStore subsession from RMobilePhone.

@param aPhone The phone subsession to use.
@return KErrNone if successful, a system-wide error code if not. 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;
	TPtrC name(KETelNamStore);
	TIpcArgs args(&name,TIpcArgs::ENothing,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobileNamStore::Close()
/** This member function closes a RMobileNamStore subsession. 
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C RMobileNamStore::TMobileNamStoreInfoV1::TMobileNamStoreInfoV1()
:	iNamCount(0),
	iActiveNam(0)
	{
	iExtensionId=KETelMobileNamStoreV1;
	}

EXPORT_C void RMobileNamStore::SetActiveNam(TRequestStatus& aReqStatus, TInt aNamId) const
/** This member function sets a new value for the active NAM.

@param aReqStatus On return, KErrNone if successful, a system-wide error code
if not.
@param aNamId Specifies which NAM to activate. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	CNamStorePtrHolder* namStorePtrHolder = static_cast<CNamStorePtrHolder*>(iStorePtrHolder);

	namStorePtrHolder->iSetActiveNamNamId = aNamId;
	TPtrC8& ptr1=namStorePtrHolder->SetC(CNamStorePtrHolder::ESlot1SetActiveNam,namStorePtrHolder->iSetActiveNamNamId);

	Set(EMobileNamStoreSetActiveNam,aReqStatus,ptr1);
	}

EXPORT_C RMobileNamStore::TMobileNamEntryV1::TMobileNamEntryV1()
: 	iNamId(0),
	iParamIdentifier(0)
	{
	}

EXPORT_C RMobileNamStore::TMobileNamEntryV4::TMobileNamEntryV4()
: 	iNamId(0),
	iParamIdentifier(0)
	{
	iExtensionId = KETelExtMultimodeV4;
	}

void RMobileNamStore::TMobileNamEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the NAM entry from a stream
 *
 * @param aStream The read stream containing the NAM entry
 */
	{
	TMobilePhoneStoreEntryV1::InternalizeL(aStream);
	iNamId = aStream.ReadInt32L();
	iParamIdentifier = aStream.ReadUint32L();
	aStream >> iData;
	}

void RMobileNamStore::TMobileNamEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the NAM entry into a stream
 *
 * @param aStream The write stream that will contain the NAM entry
 */
	{
	TMobilePhoneStoreEntryV1::ExternalizeL(aStream);
	aStream.WriteInt32L(iNamId);
	aStream.WriteUint32L(iParamIdentifier);
	aStream << iData;
	}

void RMobileNamStore::TMobileNamEntryV4::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the NAM entry from a stream
 *
 * @param aStream The read stream containing the NAM entry
 */
	{
	TMobilePhoneStoreEntryV1::InternalizeL(aStream);
	iNamId = aStream.ReadInt32L();
	iParamIdentifier = aStream.ReadInt32L();
	aStream >> iData;
	}

void RMobileNamStore::TMobileNamEntryV4::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the NAM entry into a stream
 *
 * @param aStream The write stream that will contain the NAM entry
 */
	{
	TMobilePhoneStoreEntryV1::ExternalizeL(aStream);
	aStream.WriteInt32L(iNamId);
	aStream.WriteInt32L(iParamIdentifier);
	aStream << iData;
	}

EXPORT_C void RMobileNamStore::StoreAllL(TRequestStatus& aReqStatus, TInt aNamId, CMobilePhoneNamList* aNamList) const
/** This member function stores a new version of the NAM list.

@param aReqStatus On return, KErrNone if successful, a system-wide error code
if not.
@param aNamId Specifies which NAM to use.
@param aNamList Pointer to the list containing the NAM entries to store.
@leave KErrNoMemory Out of memory 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(aNamList!=NULL,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	CNamStorePtrHolder* namStorePtrHolder = static_cast<CNamStorePtrHolder*>(iStorePtrHolder);

	delete namStorePtrHolder->iNamBuf;
	namStorePtrHolder->iNamBuf=NULL;

	namStorePtrHolder->iNamBuf=aNamList->StoreLC();
	CleanupStack::Pop();

	(namStorePtrHolder->iNamPtr).Set((namStorePtrHolder->iNamBuf)->Ptr(0));

	namStorePtrHolder->iStoreAllNamId = aNamId;
	TPtrC8& ptr1=namStorePtrHolder->SetC(CNamStorePtrHolder::ESlot1NamListStoreAll,namStorePtrHolder->iStoreAllNamId);

	Set(EMobileNamStoreStoreAll, aReqStatus, ptr1, namStorePtrHolder->iNamPtr);
	}

EXPORT_C void RMobileNamStore::StoreAllL(TRequestStatus& aReqStatus, TInt aNamId, CMobilePhoneNamListV4* aNamList) const
/** This member function stores a new version of the NAM list.

@param aReqStatus On return, KErrNone if successful, a system-wide error code
if not.
@param aNamId Specifies which NAM to use.
@param aNamList Pointer to the list containing the NAM entries to store.
@leave KErrNoMemory Out of memory 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(aNamList!=NULL,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	CNamStorePtrHolder* namStorePtrHolder = static_cast<CNamStorePtrHolder*>(iStorePtrHolder);

	delete namStorePtrHolder->iNamBufV4;
	namStorePtrHolder->iNamBufV4=NULL;

	namStorePtrHolder->iNamBufV4=aNamList->StoreLC();
	CleanupStack::Pop();

	(namStorePtrHolder->iNamPtrV4).Set((namStorePtrHolder->iNamBufV4)->Ptr(0));

	namStorePtrHolder->iStoreAllNamId = aNamId;
	TPtrC8& ptr1=namStorePtrHolder->SetC(CNamStorePtrHolder::ESlot1NamListStoreAllV4,namStorePtrHolder->iStoreAllNamId);

	Set(EMobileNamStoreStoreAllV4, aReqStatus, ptr1, namStorePtrHolder->iNamPtrV4);
	}

/************************************************************************/
//
//  RMobileONStore
//
/************************************************************************/


EXPORT_C RMobileONStore::RMobileONStore()
/** Constructor. */
	{

	}

EXPORT_C void RMobileONStore::ConstructL()
/** Initializes and allocated the members of the RMobileONStore object.

@leave KErrNoMemory Out of memory. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	CMobilePhoneStorePtrHolder* ptrHolder = CONStorePtrHolder::NewL(CONStorePtrHolder::EMaxNumberONStorePtrSlots, CONStorePtrHolder::EMaxNumberONStorePtrCSlots);
	RMobilePhoneStore::BaseConstruct(ptrHolder);
	}

EXPORT_C TInt RMobileONStore::Open(RMobilePhone& aPhone)
/** This member function opens a RMobileONStore subsession from RMobilePhone.

@param aPhone The phone subsession to use.
@return KErrNone if successful, a system-wide error code if not. 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;
	TPtrC name(KETelOwnNumberStore);
	TIpcArgs args(&name,TIpcArgs::ENothing,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobileONStore::Close()
/** This member function closes a RMobileONStore subsession. 
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}


EXPORT_C RMobileONStore::TMobileONStoreInfoV1::TMobileONStoreInfoV1()
: 	iNumberLen(0),
	iTextLen(0)
	{
	iExtensionId=KETelMobileONStoreV1;
	}

EXPORT_C RMobileONStore::TMobileONEntryV1::TMobileONEntryV1()
: 	iMode(RMobilePhone::ENetworkModeUnknown),
	iService(RMobilePhone::EServiceUnspecified)
	{
	}

void RMobileONStore::TMobileONEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the Own Number entry from a stream
 *
 * @param aStream The read stream containing the Own Number entry
 */
	{
	TMobilePhoneStoreEntryV1::InternalizeL(aStream);
	iMode = static_cast<RMobilePhone::TMobilePhoneNetworkMode>(aStream.ReadUint32L());
	aStream >> iText;
	aStream >> iNumber;
	iService = static_cast<RMobilePhone::TMobileService>(aStream.ReadUint32L());
	}

void RMobileONStore::TMobileONEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the Own Number entry into a stream
 *
 * @param aStream The write stream that will contain the Own Number entry
 */
	{
	TMobilePhoneStoreEntryV1::ExternalizeL(aStream);
	aStream.WriteUint32L(iMode);
	aStream << iText;
	aStream << iNumber;
	aStream.WriteUint32L(iService);
	}

EXPORT_C void RMobileONStore::StoreAllL(TRequestStatus& aReqStatus, CMobilePhoneONList* aONList) const
/** This member function stores a new version of the Own Number list onto the SIM.

@param aReqStatus On return, KErrNone if successful, a system-wide error code
if not.
@param aONList Pointer to the list containing the Own Number entries to store.
@leave KErrNoMemory Out of memory 
@capability WriteUserData
*/
	{
	__ASSERT_ALWAYS(aONList!=NULL,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	CONStorePtrHolder* onStorePtrHolder = static_cast<CONStorePtrHolder*>(iStorePtrHolder);

	delete onStorePtrHolder->iONBuf;
	onStorePtrHolder->iONBuf=NULL;

	onStorePtrHolder->iONBuf=aONList->StoreLC();
	CleanupStack::Pop();

	(onStorePtrHolder->iONPtr).Set((onStorePtrHolder->iONBuf)->Ptr(0));

	Set(EMobileONStoreStoreAll, aReqStatus, onStorePtrHolder->iONPtr);
	}


/************************************************************************/
//
//  RMobileENStore
//
/************************************************************************/


EXPORT_C RMobileENStore::RMobileENStore()
/** Empty constructor. */
	{
	}

EXPORT_C void RMobileENStore::ConstructL()
/** Initializes and allocated the members of the RMobileENStore object.

@leave KErrNoMemory Out of memory. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	CMobilePhoneStorePtrHolder* ptrHolder = CMobilePhoneStorePtrHolder::NewL(CMobilePhoneStorePtrHolder::EMaxNumberPhoneStorePtrSlots, CMobilePhoneStorePtrHolder::EMaxNumberPhoneStorePtrCSlots);
	RMobilePhoneStore::BaseConstruct(ptrHolder);
	}

EXPORT_C TInt RMobileENStore::Open(RMobilePhone& aPhone)
/** This function member opens a RMobileENStore subsession from RMobilePhone.

@param aPhone The phone subsession to use.
@return KErrNone if successful, a system-wide error code if not. 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;
	TPtrC name(KETelEmergencyNumberStore);
	TIpcArgs args(&name,TIpcArgs::ENothing,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobileENStore::Close()
/** This function member closes a RMobileENStore subsession. 
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C RMobileENStore::TMobileENEntryV1::TMobileENEntryV1()
: 	iNetworkSpecific(EFalse),
	iMode(RMobilePhone::ENetworkModeUnknown)
	{
	}

void RMobileENStore::TMobileENEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the Emergency Number entry from a stream
 *
 * @param aStream The read stream containing the Emergency Number entry
 */
	{
	TMobilePhoneStoreEntryV1::InternalizeL(aStream);

	iNetworkSpecific = static_cast<TBool>(aStream.ReadInt32L());
	iMode = static_cast<RMobilePhone::TMobilePhoneNetworkMode>(aStream.ReadUint32L());
	aStream >> iCountryCode;
	aStream >> iIdentity;
	aStream >> iNumber;
	aStream >> iAlphaId;
	iCallType = aStream.ReadInt32L();
	}

void RMobileENStore::TMobileENEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the Emergency Number entry into a stream
 *
 * @param aStream The write stream that will contain the Emergency Number entry
 */
	{
	TMobilePhoneStoreEntryV1::ExternalizeL(aStream);

	aStream.WriteInt32L(iNetworkSpecific);
	aStream.WriteUint32L(iMode);
	aStream << iCountryCode;
	aStream << iIdentity;
	aStream << iNumber;
	aStream << iAlphaId;
	aStream.WriteInt32L(iCallType);
	}

/************************************************************************/
//
//  RMobilePhoneBookStore
//
/************************************************************************/


EXPORT_C RMobilePhoneBookStore::RMobilePhoneBookStore()
/** Constructor. */
	{
	}

EXPORT_C void RMobilePhoneBookStore::ConstructL()
/** Constructor. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	CMobilePhoneStorePtrHolder* ptrHolder = CPhoneBookStorePtrHolder::NewL(CPhoneBookStorePtrHolder::EMaxNumberPhoneBookStorePtrSlots, CPhoneBookStorePtrHolder::EMaxNumberPhoneBookStorePtrCSlots);
	RMobilePhoneStore::BaseConstruct(ptrHolder);
	}

EXPORT_C TInt RMobilePhoneBookStore::Open(RMobilePhone& aPhone, const TDesC& aStore)
/** This function opens a RMobilePhoneBookStore sub-session from a previously opened
RMobilePhone sub-session. The name of the phone book to open is passed in
the aStore parameter. The type of valid phone books can be obtained by RMobilePhone::GetPhoneStoreInfo().

@param aPhone The opened RMobilePhone sub-session
@param aStore The name of the phone book to open.
@return KErrNone if successful, an general error code if not.
@see RMobilePhone::GetPhoneStoreInfo() 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;
	TIpcArgs args(const_cast<TDesC*>(&aStore),TIpcArgs::ENothing,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C TInt RMobilePhoneBookStore::Open(RMobilePhone& aPhone, const TDesC& aStore,const TDesC& aMode)
/**
This method opens a RMobilePhoneBookStore subsession from RMobilePhone.

This API method overloads the normal RMobilePhoneBookStore::Open. The phonebook
mode is indicated by the aMode parameter, which allows the client to distinguish
between the different sets of phonebooks that can be present on the ICC or
ME side. E.g. a client can open both the GSM ADN phonebook and the USIM ADN
phonebook on a UICC. The client would then be able to switch on the fly between
the GSM/global phonebook and the (active) USIM Application phonebook.

A UICC can comprise of many USIM applications and a GSM application. Only
one of application can be active at any one point. The USIM applications contain
their own phonebooks (ADN, FDN, etc..). To open a USIM Application phonebook,
the USIM Application must be active. It is not possible to access a phonebook
of a USIM Application that is not currently active.

This API method opens a RMobilePhoneBookStore sub-session from a previously
opened RMobilePhone sub-session. The name of the phone book to open is passed
in the aStore parameter. The names of known and valid phone books for this
API methods are listed in the table below. The phone may not support all of
these phone books so clients should check which are available using the method
in the previous section 4.8.1.

Note for the TSY development: Internally ETel will pass to the TSY one TDesC
variable containing: the phonebookname as defined in 8.4.1 for GSM and CDMA
ICCs or the phonebookname followed by a delimiting character (i.e. the PHBOOK_DELIMITER
defined in ETelMM.h) and the ICC type (i.e. UICC) for UMTS.

@param aPhone The opened RMobilePhone sub-session.
@param aStore The name of the phone book to open.
@param aMode The phonebook mode.
@return KErrNone if successful, an general error code if not. 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		return ret;
	TIpcArgs args;

	// The TSY should assume that the phonebook type is GSM or CDMA, if the aMode is not
	// present. In the case of USIM (WCDMA), the "mode" will be appended to the parameter
	// that is used to pass the phonebook name (using a delimiter to seperate the two params).
	// The TSY should tokenize this parameter (using the delimiter) and retrieve the phonebook
	// type (only USim currently) and phonebook name.

	if(aMode.Compare(KEtelUSimPhoneBookType) == 0)
		{
			if((aStore.Length()+aMode.Length()+KSizeOfPhbookDelimiter)<=KMaxName)
			{
				TName aUmtsStore;
				aUmtsStore.FillZ();
				aUmtsStore.Copy(aStore);
				aUmtsStore.Append(PHBOOK_DELIMITER);
				aUmtsStore.Append(aMode);
				args.Set(0,&aUmtsStore);
			}
			else
				return KErrOverflow;
		}
	else
		{
		args.Set(0,const_cast<TDesC*>(&aStore));
		}
	SetSessionHandle(*session);
	args.Set(1,TIpcArgs::ENothing);
  	args.Set(2,subSessionHandle);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobilePhoneBookStore::Close()
/** This member function closes the phone book sub-session. 
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C RMobilePhoneBookStore::TMobilePhoneBookInfoV1::TMobilePhoneBookInfoV1()
:	iMaxNumLength(-1),
	iMaxTextLength(-1),
	iLocation(ELocationUnknown),
	iChangeCounter(0)
	{
	iExtensionId=KETelMobilePhonebookStoreV1;
	}



EXPORT_C RMobilePhoneBookStore::TMobilePhoneBookInfoV2::TMobilePhoneBookInfoV2()
:	TMobilePhoneBookInfoV1(),
	iPhBkMode(0)
/**
* This is the initialization of the TMobilePhoneBookInfoV2 structured type
*
*/
	{
	iExtensionId=KETelMobilePhonebookStoreV2;
	}

EXPORT_C RMobilePhoneBookStore::TMobilePhoneBookInfoV5::TMobilePhoneBookInfoV5()
:	TMobilePhoneBookInfoV2(),
	iMaxSecondNames(-1), iMaxTextLengthSecondName(-1), iMaxAdditionalNumbers(-1), 
	iMaxNumLengthAdditionalNumber(-1), iMaxTextLengthAdditionalNumber(-1), 
	iMaxGroupNames(-1), iMaxTextLengthGroupName(-1), iMaxEmailAddr(-1), 
	iMaxTextLengthEmailAddr(-1)
/**
* This is the initialization of the TMobilePhoneBookInfoV5 structured type
*
*/
	{
	iExtensionId=KETelMobilePhonebookStoreV5;
	}

EXPORT_C void RMobilePhoneBookStore::Read(TRequestStatus& aReqStatus, TInt aIndex, TInt aNumSlots, TDes8& aPBData) const
/**
 * This method reads one or more entries from a phonebook store.
 * The reading will start at the slot specified by aIndex and will stop either after
 * aNumSlots of slots have been read or no more whole phonebook entries can fit in the
 * size of the supplied aPBData parameter.
 *
 * If there are no used entries in the slots requested then KErrNotFound will be returned.
 *
 * Example: If aIndex=1 and aNumSlots=1 then one slot (slot number 1) will be read and
 * if used, this entry will be returned within aPBData.
 *
 * @param aReqStatus returns the result code after the asynchronous call completes
 * @param  aIndex Specifies the index to start reading from, must be >= 1.
 * @param  aNumSlots Specifies the number of slots to read, must be >= 1 and <= total number of slots.
 * @param aPBData A descriptor that will contain the phonebook data in TLV format.
 
@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	CPhoneBookStorePtrHolder* pbStorePtrHolder = static_cast<CPhoneBookStorePtrHolder*>(iStorePtrHolder);

	pbStorePtrHolder->iReadPhoneBookEntry.iIndex = aIndex;
	pbStorePtrHolder->iReadPhoneBookEntry.iNumSlots = aNumSlots;

	TPtrC8& ptr1=pbStorePtrHolder->SetC(CPhoneBookStorePtrHolder::ESlot1PhoneBookStoreRead, pbStorePtrHolder->iReadPhoneBookEntry);
	SetAndGet(EMobilePhoneBookStoreRead, aReqStatus, ptr1, aPBData);
	}

EXPORT_C void RMobilePhoneBookStore::Write(TRequestStatus& aReqStatus, const TDesC8& aPBData, TInt& aIndex) const
/** This member function writes one phonebook entry to the store. This member function
is a specialised version of the base class RMobilePhoneStore::Write() and
all clients of RMobilePhoneBookStore should use this specialised version rather
than the base class version. The phonebook data will be in aPBData which will
contain the encoded byte stream of a TLV format phonebook entry.

Note:

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneStoreWrite) to cancel
a previously placed asynchronous Write request.

@param aReqStatus On return, KErrNone if successful, a system wide error code
if not.
@param aPBData The encoded byte stream of TLV formatted phonebook data.
@param aIndex The slot the TSY will store the entry, or if aIndex=-1 the TSY
will store the entry in the first free location and then return this location
within aIndex when it completes the request. 
@capability WriteUserData
*/
	{
	__ASSERT_ALWAYS(iStorePtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	CPhoneBookStorePtrHolder* pbStorePtrHolder = static_cast<CPhoneBookStorePtrHolder*>(iStorePtrHolder);

	TPtr8& ptr1=pbStorePtrHolder->Set(CPhoneBookStorePtrHolder::ESlot1PhoneBookStoreWrite, aIndex);

	SetAndGet(EMobilePhoneBookStoreWrite, aReqStatus, aPBData, ptr1);
	}
