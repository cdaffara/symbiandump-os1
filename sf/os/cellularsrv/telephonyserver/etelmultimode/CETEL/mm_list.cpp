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
// ETel Multimode API
// 
//

#include <mmlist.h>
#include <etelext.h>

/********************************************************************/
//
// CMobilePhoneListBase
// Base class of thin-template idiom for ETel list classes
//
/********************************************************************/

EXPORT_C CMobilePhoneListBase::CMobilePhoneListBase(TInt aLength, TInt aGranularity) : 
	iList(aLength,aGranularity), 
	iMaxNumber(KMaxEntriesNotSet)
/** Standard constructor, sets the length of the entries in the list and the granularity 
of the list.

@param aLength Length of the CArrayFixFlat<TAny> entries to be stored in the 
list.
@param aGranularity Granularity of the list. */
	{
	}

EXPORT_C CMobilePhoneListBase::~CMobilePhoneListBase()
/** Empty destructor. */
	{
	}

EXPORT_C CBufBase* CMobilePhoneListBase::StoreLC()
/** Stores the streamed contents of the list into a CBufFlat. 
Placing the list's contents into a store is necessary prior to passing the 
list across the client-server interface.

@return Pointer to the allocated CBufFlat. 
@capability None
*/
	{
	CBufFlat* buf=CBufFlat::NewL(4);
	CleanupStack::PushL(buf);
	RBufWriteStream strm(*buf);
	strm << *this;
	strm.CommitL();
	return buf;
	}

EXPORT_C void CMobilePhoneListBase::StoreL(TDes8& aDes)
/** This member function will store the contents of the list class into a write 
stream. Placing the list's contents into a store is necessary prior to passing 
the list across the client-server interface.

This member function uses the aDes parameter as the write stream.

@param aDes Descriptor in which to store the list. 
@capability None
*/
	{
	RDesWriteStream strm(aDes);
	strm << *this;
	strm.CommitL();
	}

EXPORT_C void CMobilePhoneListBase::RestoreL(const TDesC8& aBuf)
/** This member function will restore the contents of a read stream into the list's 
internal structure. Restoring the list's contents is necessary after the list 
has crossed the client-server interface.

This member function uses the aDes parameter as the read stream.

@param aBuf The read stream used to restore the list from. 
@capability None
*/
	{
	RDesReadStream strm(aBuf);			// turn it into a stream
	strm >> *this;						// re-construct arrays 
	}

EXPORT_C const TAny* CMobilePhoneListBase::GetEntryL(TInt aIndex) const
/** This member function retrieves the entry at position aIndex from the list.

A valid index ranges from 0 to count -1, where count is the value returned 
from CMobilePhoneListBase::Enumerate(). 

@param aIndex Index of the entry to be retrieved.
@leave EListIndexOutOfRange This member function will leave if aIndex is outside 
the current number of entries.
@return Pointer to the entry at position aIndex. 
@capability None
*/
	{
	if (aIndex < 0 || aIndex >= iList.Count())
		User::Leave(EListIndexOutOfRange);
	return iList.At(aIndex);
	}

EXPORT_C void CMobilePhoneListBase::AddEntryL(const TAny* aPtr)
/** This member function adds a new entry into the list, placing it in the next 
empty position.
	
@param aEntry Pointer to the entry that will be added to the list.
@leave EListMaxNumberReached This member function will leave if the list fails 
to allocate memory for the new entry or if the maximum number of entries has 
been reached. 
 
@capability None
*/
	{
	if (iMaxNumber != KMaxEntriesNotSet && iList.Count() >= iMaxNumber)
		User::Leave(EListMaxNumberReached);
	iList.AppendL(aPtr);
	}

EXPORT_C TInt CMobilePhoneListBase::Enumerate() const
/** This member function gets the number of entries currently in the list. This 
number can therefore be between zero and the maximum number of entries if 
this attribute has been set.

@return The number of entries currently in the list. 
@capability None
*/
	{
	return iList.Count();
	}

EXPORT_C TInt CMobilePhoneListBase::MaxNumberEntries() const
/** This member function gets the maximum number of entries that can be stored 
in this list. This attribute is required because most phone-side storage will 
have an upper storage limit. 

@return The maximum number of entries that can be stored in this list. If 
the TSY has not set this attribute during a list retrieval then the value 
returned will be KMaxEntriesNotSet. 
@capability None
*/
	{
	return iMaxNumber;
	}

EXPORT_C void CMobilePhoneListBase::SetMaxNumberEntries(TInt aMax)
/** This member function sets the maximum number of entries that can be stored 
in this list. This member function is intended for use by the TSY only, after 
it has performed a list retrieval. Clients are able to add new entries to 
an editable list returned to them but only up to the maximum number.

@param aMax The (new) maximum number of entries for this list. 
@capability None
*/
	{
	__ASSERT_ALWAYS(aMax >=0, PanicClient(EEtelPanicIndexOutOfRange));
	__ASSERT_ALWAYS(aMax >= iList.Count(), PanicClient(EEtelPanicIndexOutOfRange));
	iMaxNumber=aMax;
	}

EXPORT_C void CMobilePhoneListBase::InternalizeL(RReadStream& aStream)
/** This member function internalizes the list contents from a stream.

@param aStream The read stream containing the list. 
@capability None
*/
	{
	iMaxNumber = aStream.ReadInt32L();
	iList.Reset();
	TInt count=aStream.ReadInt32L();
	for (TInt ii=0;ii<count;++ii)
		InternalizeEntryL(iList.ExtendL(),aStream);
	}

EXPORT_C void CMobilePhoneListBase::ExternalizeL(RWriteStream& aStream) const
/** This member function externalizes the list contents into a stream.

@param aStream The write stream to which to write the list. 
@capability None
*/
	{
	aStream.WriteInt32L(iMaxNumber);
	TInt count=iList.Count();
	aStream.WriteInt32L(count);
	for (TInt ii=0;ii<count;++ii)
		ExternalizeEntryL(iList.At(ii),aStream);
	}


/********************************************************************/
//
// CMobilePhoneEditableListBase
// Base class of thin-template idiom for ETel list classes
//
/********************************************************************/

EXPORT_C CMobilePhoneEditableListBase::CMobilePhoneEditableListBase(TInt aLength, TInt aGranularity)
: CMobilePhoneListBase(aLength,aGranularity)
/** Standard constructor.

@param aLength The length of the elements of this fixed length array. This 
value must be positive otherwise the constructor raises an E32USER-CBase 17 
panic.
@param aGranularity The granularity of the array. This value must be positive 
otherwise the constructor raises an E32USER-CBase 18 panic. */
	{
	}

EXPORT_C CMobilePhoneEditableListBase::~CMobilePhoneEditableListBase()
/** Standard empty destructor. */
	{}

EXPORT_C void CMobilePhoneEditableListBase::DeleteEntryL(TInt aIndex)
/** Deletes the entry at index aIndex from the list.

@param aIndex Index of the entry to be deleted from the list 
@capability None
*/
	{
	if (aIndex < 0 || aIndex >= iList.Count())
		User::Leave(EListIndexOutOfRange);
	iList.Delete(aIndex);
	}

EXPORT_C void CMobilePhoneEditableListBase::InsertEntryL(TInt aIndex, const TAny* aPtr)
/**
Inserts a new list entry at the specified index

@param aIndex Index of the point at which to insert the new entry
@param aPtr Pointer to the new entry
@leave EListIndexOutOfRange If aIndex > number of entries in the list.
@leave EListMaxNumberReached If the maximum number of entries is reached.
 
@capability None
*/
	{
	if (aIndex < 0 || aIndex >= iList.Count())
		User::Leave(EListIndexOutOfRange);
	if (iMaxNumber != KMaxEntriesNotSet && iList.Count() >= iMaxNumber)
		User::Leave(EListMaxNumberReached);
	iList.InsertL(aIndex, aPtr);
	}

/********************************************************************/
//
// CMobilePhoneNetworkList
// A concrete instantiation of an ETel list - holding RMobilePhone::TMobilePhoneNetworkInfoV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneNetworkList* CMobilePhoneNetworkList::NewL()
/** This member function creates a new instance of a CMobilePhoneNetworkList. It 
will leave if the two-phase construction fails at any point. 

@return A pointer to the new instance of CMobilePhoneNetworkList. 
@capability None
*/
	{
	CMobilePhoneNetworkList* r=new(ELeave) CMobilePhoneNetworkList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneNetworkList::CMobilePhoneNetworkList()
	: CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneNetworkInfoV1>()
	{}

EXPORT_C CMobilePhoneNetworkList::~CMobilePhoneNetworkList()
/** Empty destructor. */
	{}

void CMobilePhoneNetworkList::ConstructL()
	{}


/********************************************************************/
//
// CMobilePhoneNetworkListV2
// A concrete instantiation of an ETel list - holding RMobilePhone::TMobilePhoneNetworkInfoV2 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneNetworkListV2* CMobilePhoneNetworkListV2::NewL()
/** This member function creates a new instance of a CMobilePhoneNetworkListV2. It 
will leave if the two-phase construction fails at any point. 

@return A pointer to the new instance of CMobilePhoneNetworkList. 
@capability None
*/
	{
	CMobilePhoneNetworkListV2* r=new(ELeave) CMobilePhoneNetworkListV2();
	return r;
	}
	
CMobilePhoneNetworkListV2::CMobilePhoneNetworkListV2()
	: CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneNetworkInfoV2>()
	{}

EXPORT_C CMobilePhoneNetworkListV2::~CMobilePhoneNetworkListV2()
/** Empty destructor. */
	{}

/********************************************************************/
//
// CMobilePhoneNetworkListV5
// A concrete instantiation of an ETel list - holding RMobilePhone::TMobilePhoneNetworkInfoV5 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneNetworkListV5* CMobilePhoneNetworkListV5::NewL()
/** This member function creates a new instance of a CMobilePhoneNetworkListV5. It 
will leave if the two-phase construction fails at any point. 

@return A pointer to the new instance of CMobilePhoneNetworkList. 
@capability None
*/
	{
	CMobilePhoneNetworkListV5* r=new(ELeave) CMobilePhoneNetworkListV5();
	return r;
	}
	
CMobilePhoneNetworkListV5::CMobilePhoneNetworkListV5()
	: CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneNetworkInfoV5>()
	{}

EXPORT_C CMobilePhoneNetworkListV5::~CMobilePhoneNetworkListV5()
/** Empty destructor. */
	{}

/********************************************************************/
//
// CMobilePhoneNetworkListV8
// A concrete instantiation of an ETel list - holding RMobilePhone::TMobilePhoneNetworkInfoV8 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneNetworkListV8* CMobilePhoneNetworkListV8::NewL()
/** This member function creates a new instance of a CMobilePhoneNetworkListV8. It 
will leave if the two-phase construction fails at any point. 

@return A pointer to the new instance of CMobilePhoneNetworkList. 
@capability None
*/
	{
	CMobilePhoneNetworkListV8* r=new(ELeave) CMobilePhoneNetworkListV8();
	return r;
	}
	
CMobilePhoneNetworkListV8::CMobilePhoneNetworkListV8()
	: CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneNetworkInfoV8>()
	{}

EXPORT_C CMobilePhoneNetworkListV8::~CMobilePhoneNetworkListV8()
/** Empty destructor. */
	{}

/********************************************************************/
//
// CMobilePhoneCFList
// A concrete instantion of an ETel list - holding RMobilePhone::TMobilePhoneCFInfoEntryV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneCFList* CMobilePhoneCFList::NewL()
/** This function member creates a new instance of a CMobilePhoneCFList.

@return A pointer to the new instance of CMobilePhoneCFList. 
@capability None
*/
	{
	CMobilePhoneCFList* r=new(ELeave) CMobilePhoneCFList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneCFList::CMobilePhoneCFList()
	: CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneCFInfoEntryV1>()
	{}

EXPORT_C CMobilePhoneCFList::~CMobilePhoneCFList()
/** This function member destroys the CMobilePhoneCFList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{}

void CMobilePhoneCFList::ConstructL()
	{}

/********************************************************************/
//
// CMobilePhoneCBList
// A concrete instantion of an ETel list - holding RMobilePhone::TMobilePhoneCBInfoEntryV1 objects
//
/********************************************************************/


EXPORT_C CMobilePhoneCBList* CMobilePhoneCBList::NewL()
/** This member function creates a new instance of a CMobilePhoneCBList.

@return A pointer to the new instance of CMobilePhoneCBList 
@capability None
*/
	{
	CMobilePhoneCBList* r=new(ELeave) CMobilePhoneCBList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

EXPORT_C CMobilePhoneCBList::~CMobilePhoneCBList()
/** This member function destroys the CMobilePhoneCBList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{
	}

CMobilePhoneCBList::CMobilePhoneCBList()
	{
	}

void CMobilePhoneCBList::ConstructL()
	{}



/********************************************************************/
//
// CMobilePhoneCWList
// A concrete instantion of an ETel list - holding RMobilePhone::TMobilePhoneCWInfoEntryV1 objects
//
/********************************************************************/


EXPORT_C CMobilePhoneCWList* CMobilePhoneCWList::NewL()
/** This function member creates a new instance of a CMobilePhoneCWList.

@return A pointer to the new instance of CMobilePhoneCWList. 
@capability None
*/
	{
	CMobilePhoneCWList* r=new(ELeave) CMobilePhoneCWList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

EXPORT_C CMobilePhoneCWList::~CMobilePhoneCWList()
/** This function member destroys the CMobilePhoneCWList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{
	}

CMobilePhoneCWList::CMobilePhoneCWList()
	{
	}

void CMobilePhoneCWList::ConstructL()
	{}


/********************************************************************/
//
// CMobilePhoneCCBSList
// A concrete instantion of an ETel list - holding RMobilePhone::TMobilePhoneCCBSEntryV1 objects
//
/********************************************************************/


EXPORT_C CMobilePhoneCcbsList* CMobilePhoneCcbsList::NewL()
/** This member function creates a new instance of a CMobilePhoneCcbsList.

@return A pointer to the new instance of CMobilePhoneCcbsList. 
@capability None
*/
	{
	CMobilePhoneCcbsList* r=new(ELeave) CMobilePhoneCcbsList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

EXPORT_C CMobilePhoneCcbsList::~CMobilePhoneCcbsList()
/** This member function destroys the CMobilePhoneCcbsList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{
	}

CMobilePhoneCcbsList::CMobilePhoneCcbsList()
	{
	}

void CMobilePhoneCcbsList::ConstructL()
	{}


/********************************************************************/
//
// CMobilePhoneGsmSmsList
// A concrete instantion of an ETel list - holding RMobileSmsStore::TMobileGsmSmsEntryV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneGsmSmsList* CMobilePhoneGsmSmsList::NewL()
/** This member function creates a new instance of a CMobilePhoneGsmSmsList. It 
will leave if the two-phase construction fails at any point.

@return A pointer to newly created CMobilePhoneGsmSmsList. 
@capability None
*/
	{
	CMobilePhoneGsmSmsList* r=new(ELeave) CMobilePhoneGsmSmsList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneGsmSmsList::CMobilePhoneGsmSmsList()
	: CMobilePhoneReadOnlyList<RMobileSmsStore::TMobileGsmSmsEntryV1>()
	{}

EXPORT_C CMobilePhoneGsmSmsList::~CMobilePhoneGsmSmsList()
/** This member function destroys the CMobilePhoneGsmSmsList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{}

void CMobilePhoneGsmSmsList::ConstructL()
	{}

/********************************************************************/
//
// CMobilePhoneCdmaSmsList
// A concrete instantion of an ETel list - holding RMobileSmsStore::TMobileCdmaSmsEntryV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneCdmaSmsList* CMobilePhoneCdmaSmsList::NewL()
/** This member function creates a new instance of a CMobilePhoneCdmaSmsList.

@return A pointer to the new instance of CMobilePhoneCdmaSmsList. 
@capability None
*/
	{
	CMobilePhoneCdmaSmsList* r=new(ELeave) CMobilePhoneCdmaSmsList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneCdmaSmsList::CMobilePhoneCdmaSmsList()
	: CMobilePhoneReadOnlyList<RMobileSmsStore::TMobileCdmaSmsEntryV1>()
	{}

EXPORT_C CMobilePhoneCdmaSmsList::~CMobilePhoneCdmaSmsList()
/** This member function destroys the CMobilePhoneCdmaSmsList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{}

void CMobilePhoneCdmaSmsList::ConstructL()
	{}

/********************************************************************/
//
// CMobilePhoneSmspList
// A concrete instantion of an ETel list - holding RMobileSmsMessaging::TMobileSmspEntryV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneSmspList* CMobilePhoneSmspList::NewL()
/** This member function creates a new instance of a CMobilePhoneSmspList. It will 
leave if the two-phase construction fails at any point.

@return A pointer to the new instance of CMobilePhoneSmspList. 
@capability None
*/
	{
	CMobilePhoneSmspList* r=new(ELeave) CMobilePhoneSmspList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneSmspList::CMobilePhoneSmspList()
	: CMobilePhoneEditableList<RMobileSmsMessaging::TMobileSmspEntryV1>()
	{}

EXPORT_C CMobilePhoneSmspList::~CMobilePhoneSmspList()
/** Empty destructor. */
	{}

void CMobilePhoneSmspList::ConstructL()
	{}


/********************************************************************/
//
// CMobilePhoneBroadcastIdList
// A concrete instantion of an ETel list - holding RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneBroadcastIdList* CMobilePhoneBroadcastIdList::NewL()
/** This member function creates a new instance of a CMobilePhoneBroadcastIdList.

@return A pointer to the new instance of CMobilePhoneBroadcastIdList. 
@capability None
*/
	{
	CMobilePhoneBroadcastIdList* r=new(ELeave) CMobilePhoneBroadcastIdList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneBroadcastIdList::CMobilePhoneBroadcastIdList()
	: CMobilePhoneEditableList<RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1>()
	{}

EXPORT_C CMobilePhoneBroadcastIdList::~CMobilePhoneBroadcastIdList()
/** This member function destroys the CMobilePhoneBroadcastIdList object and the 
inherited virtual destructor frees-up any resources used by its base classes. */
	{}

void CMobilePhoneBroadcastIdList::ConstructL()
	{}

EXPORT_C void CMobilePhoneBroadcastIdList::AddRangeEntryL(const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& aStart, 
											  const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& aEnd)
/** This member function allows the client to add a sequential range of Cell Broadcast 
Identifiers into the BROADCASTID list.

For example, if the client wants to receive all broadcast messages within 
the range of identifier 100 to (but not including) 200, instead of having 
to perform 100 individual CMobilePhoneEditableList::AddEntryL() member functions 
on their list, they can use AddRangeEntryL(), specifying aStart.iId=100 and 
aEnd.id=200.

@param aStart The first value to add.
@param aEnd The first that will not be added. 
@capability None
*/
	{
	if (aEnd.iId < aStart.iId)
		User::Leave(EBadRange);

	TInt numToAdd;
	numToAdd = (aEnd.iId - aStart.iId);

	if (iMaxNumber != KMaxEntriesNotSet && numToAdd + iList.Count() >= iMaxNumber)
		User::Leave(EListMaxNumberReached);

	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 entry;

	TInt count=0;
	while ( count<numToAdd )
		{
		entry.iId=(TUint16)((aStart.iId)+count);
		AddEntryL(entry);
		++count;
		}
	}

/********************************************************************/
//
// CMobilePhoneNamList
// A concrete instantion of an ETel list - holding RMobileNamStore::TMobileNamEntryV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneNamList* CMobilePhoneNamList::NewL()
/** This member function creates a new instance of a CMobilePhoneNamList. It will 
leave if the two-phase construction fails at any point.

@return A pointer to the new instance of CMobilePhoneNamList. 
@capability None
*/
	{
	CMobilePhoneNamList* r=new(ELeave) CMobilePhoneNamList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneNamList::CMobilePhoneNamList()
	: CMobilePhoneEditableList<RMobileNamStore::TMobileNamEntryV1>()
	{}

EXPORT_C CMobilePhoneNamList::~CMobilePhoneNamList()
/** Empty destructor. */
	{}

void CMobilePhoneNamList::ConstructL()
	{}

/********************************************************************/
//
// CMobilePhoneNamListV4
// A concrete instantiation of an ETel list - holding RMobileNamStore::TMobileNamEntryV4 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneNamListV4* CMobilePhoneNamListV4::NewL()
/** This member function creates a new instance of a CMobilePhoneNamListV4. It will 
leave if the two-phase construction fails at any point.

@return A pointer to the new instance of CMobilePhoneNamListV4. 
@capability None
*/
	{
	CMobilePhoneNamListV4* r=new(ELeave) CMobilePhoneNamListV4();
	return r;
	}
	
CMobilePhoneNamListV4::CMobilePhoneNamListV4()
	: CMobilePhoneEditableList<RMobileNamStore::TMobileNamEntryV4>()
	{}

EXPORT_C CMobilePhoneNamListV4::~CMobilePhoneNamListV4()
/** Empty destructor. */
	{}

/********************************************************************/
//
// CMobilePhoneONList
// A concrete instantion of an ETel list - holding RMobileONStore::TMobileONEntryV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneONList* CMobilePhoneONList::NewL()
/** This member function creates a new instance of a CMobilePhoneONList. It will 
leave if the two-phase construction fails at any point.

@return pointer to the new instance of CMobilePhoneONList. 
@capability None
*/
	{
	CMobilePhoneONList* r=new(ELeave) CMobilePhoneONList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneONList::CMobilePhoneONList()
	: CMobilePhoneEditableList<RMobileONStore::TMobileONEntryV1>()
	{}

EXPORT_C CMobilePhoneONList::~CMobilePhoneONList()
/** Empty destructor. */
	{}

void CMobilePhoneONList::ConstructL()
	{}


/********************************************************************/
//
// CMobilePhoneENList
// A concrete instantion of an ETel list - holding RMobileENStore::TMobileENEntryV1 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneENList* CMobilePhoneENList::NewL()
/** This member function creates a list that will contain RMobileENStore::TMobileENEntryV1 
objects. It will leave if the two-phase construction fails at any point. 

@return Pointer to the newly created CMobilePhoneENList. 
@capability None
*/
	{
	CMobilePhoneENList* r=new(ELeave) CMobilePhoneENList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneENList::CMobilePhoneENList()
	: CMobilePhoneReadOnlyList<RMobileENStore::TMobileENEntryV1>()
	{}

EXPORT_C CMobilePhoneENList::~CMobilePhoneENList()
/** This member function destroys the CMobilePhoneENList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{}

void CMobilePhoneENList::ConstructL()
	{}

/********************************************************************/
//
// CMobilePhoneStoredNetworkList
// A concrete instantion of an ETel list - holding RMobilePhone::TMobilePreferredNetworkEntryV3 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneStoredNetworkList* CMobilePhoneStoredNetworkList::NewL()
/**
This method creates a list that will contain RMobilePhone::TMobilePreferredNetworkEntryV3 objects
 
@return  A pointer to the newly created CMobilePhoneStoredNetworkList object.
@capability None
*/
	{
	CMobilePhoneStoredNetworkList* r=new(ELeave) CMobilePhoneStoredNetworkList();
	return r;
	}
	
CMobilePhoneStoredNetworkList::CMobilePhoneStoredNetworkList()
	: CMobilePhoneEditableList<RMobilePhone::TMobilePreferredNetworkEntryV3>()
/**
Constructor.
*/
	{}

EXPORT_C CMobilePhoneStoredNetworkList::~CMobilePhoneStoredNetworkList()
/**
Destructor.
*/
	{}

EXPORT_C TInt CMobilePhoneStoredNetworkList::InsertEntryL(TInt aIndex, const RMobilePhone::TMobilePreferredNetworkEntryV3& aEntry)
/**
Inserts an entry in the stored network list. 

@param aIndex Index at which to insert the new entry.
@param aEntry Entry to be inserted.
@return KErrNone if successful;  KErrArgument if operation is attempted on non-user defined (i.e. TMobilePreferredNetworkEntryV3.iUserDefined==EFalse) entry.
@capability WriteDeviceData
*/
	{	
	if(IsEntryUserDefinedL(aIndex))
		{
		CMobilePhoneEditableList<RMobilePhone::TMobilePreferredNetworkEntryV3>::InsertEntryL(aIndex, aEntry);
		return KErrNone;
		}
	else		
		return KErrArgument;	
	}
EXPORT_C TInt CMobilePhoneStoredNetworkList::ChangeEntryL(TInt aIndex, const RMobilePhone::TMobilePreferredNetworkEntryV3& aEntry) 
/**
Changes an entry in the stored network list. 

@param aIndex Index of entry to change.
@param aEntry Changed stored network entry.
@return KErrNone if successful;  KErrArgument if operation is attempted on non-user defined (i.e. TMobilePreferredNetworkEntryV3.iUserDefined==EFalse) entry.
@capability WriteDeviceData
*/
	{
	if(IsEntryUserDefinedL(aIndex))	
		{
		CMobilePhoneEditableList<RMobilePhone::TMobilePreferredNetworkEntryV3>::ChangeEntryL(aIndex, aEntry);
		return KErrNone;
		}
	else	
		return KErrArgument;	
	}
	
EXPORT_C TInt CMobilePhoneStoredNetworkList::DeleteEntryL(TInt aIndex)
/**
Deletes an entry in the stored network list. 

@param aIndex Index of entry to be deleted.
@return KErrNone if successful;  KErrArgument if operation is attempted on non-user defined (i.e. TMobilePreferredNetworkEntryV3.iUserDefined==EFalse) entry.
@capability WriteDeviceData
*/
	{
	if(IsEntryUserDefinedL(aIndex))	
		{
		CMobilePhoneEditableList<RMobilePhone::TMobilePreferredNetworkEntryV3>::DeleteEntryL(aIndex);
		return KErrNone;
		}
	else
		return KErrArgument;	
	}
	
TBool CMobilePhoneStoredNetworkList::IsEntryUserDefinedL(TInt aIndex)
	{
	if (aIndex < iList.Count() && !GetEntryL(aIndex).iUserDefined)
		return EFalse;
	return ETrue;	
	}
/********************************************************************/
//
// CMobilePhoneGbaNafIdList
// A concrete instantion of an ETel list - holding RMobilePhone::TGbaNafEntryV8 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneGbaNafIdList* CMobilePhoneGbaNafIdList::NewL()
/** This member function creates a new instance of a CMobilePhoneGbaNafIdList. It 
will leave if the two-phase construction fails at any point.

@return A pointer to newly created CMobilePhoneGbaNafIdList. 
@capability None
*/
	{
	CMobilePhoneGbaNafIdList* r=new(ELeave) CMobilePhoneGbaNafIdList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneGbaNafIdList::CMobilePhoneGbaNafIdList()
	: CMobilePhoneReadOnlyList<RMobilePhone::TGbaNafEntryV8>()
/**
Constructor.
*/
	{}

EXPORT_C CMobilePhoneGbaNafIdList::~CMobilePhoneGbaNafIdList()
/** This member function destroys the CMobilePhoneGbaNafIdList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{}

void CMobilePhoneGbaNafIdList::ConstructL()
	{}
	
/********************************************************************/
//
// CMobilePhoneMbmsMskIdList
// A concrete instantion of an ETel list - holding RMobilePhone::TMskEntryV8 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneMbmsMskIdList* CMobilePhoneMbmsMskIdList::NewL()
/** This member function creates a new instance of a CMobilePhoneMbmsMskIdList. It 
will leave if the two-phase construction fails at any point.

@return A pointer to newly created CMobilePhoneMbmsMskIdList. 
@capability None
*/
	{
	CMobilePhoneMbmsMskIdList* r=new(ELeave) CMobilePhoneMbmsMskIdList();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
	
CMobilePhoneMbmsMskIdList::CMobilePhoneMbmsMskIdList()
	: CMobilePhoneReadOnlyList<RMobilePhone::TMskEntryV8>()
/**
Constructor.
*/
	{}

EXPORT_C CMobilePhoneMbmsMskIdList::~CMobilePhoneMbmsMskIdList()
/** This member function destroys the CMobilePhoneMbmsMskIdList object and the inherited 
virtual destructor frees-up any resources used by its base classes. */
	{}

void CMobilePhoneMbmsMskIdList::ConstructL()
	{}
	
/********************************************************************/
//
// CMobilePhoneStoredWlanSIDList
// A concrete instantion of an ETel list - holding RMobilePhone::TWlanSIDV8 objects
//
/********************************************************************/

EXPORT_C CMobilePhoneStoredWlanSIDList* CMobilePhoneStoredWlanSIDList::NewL()
/** This member function creates a new instance of a CMobilePhoneStoredWlanSIDList. It will 
leave if the two-phase construction fails at any point.

@return A pointer to the new instance of CMobilePhoneStoredWlanSIDList. 
@capability None
*/
	{
	CMobilePhoneStoredWlanSIDList* r=new(ELeave) CMobilePhoneStoredWlanSIDList();
	return r;
	}
	
CMobilePhoneStoredWlanSIDList::CMobilePhoneStoredWlanSIDList()
	: CMobilePhoneEditableList<RMobilePhone::TWlanSIDV8>()
	{}

EXPORT_C CMobilePhoneStoredWlanSIDList::~CMobilePhoneStoredWlanSIDList()
/** Empty destructor. */
	{}

EXPORT_C TInt CMobilePhoneStoredWlanSIDList::InsertEntryL(TInt aIndex, const RMobilePhone::TWlanSIDV8& aEntry)
/**
Inserts an entry in the stored Wlan Specific identifier list. 

@param aIndex Index at which to insert the new entry.
@param aEntry Entry to be inserted.
@return KErrNone if successful;  KErrArgument if operation is attempted on non-user defined (i.e. TWlanSIDV8.iUserDefined==EFalse) entry.
@capability WriteDeviceData
*/
	{	
	if(IsEntryUserDefinedL(aIndex))
		{
		CMobilePhoneEditableList<RMobilePhone::TWlanSIDV8>::InsertEntryL(aIndex, aEntry);
		return KErrNone;
		}
	else
		{
		return KErrArgument;	
		}		
	}
	
EXPORT_C TInt CMobilePhoneStoredWlanSIDList::ChangeEntryL(TInt aIndex, const RMobilePhone::TWlanSIDV8& aEntry) 
/**
Changes an entry in the stored Wlan Specific identifier list. 

@param aIndex Index of entry to change.
@param aEntry Changed stored network entry.
@return KErrNone if successful;  KErrArgument if operation is attempted on non-user defined (i.e. TWlanSIDV8.iUserDefined==EFalse) entry.
@capability WriteDeviceData
*/
	{
	if(IsEntryUserDefinedL(aIndex))	
		{
		CMobilePhoneEditableList<RMobilePhone::TWlanSIDV8>::ChangeEntryL(aIndex, aEntry);
		return KErrNone;
		}
	else
		{
		return KErrArgument;		
		}	
	}
	
EXPORT_C TInt CMobilePhoneStoredWlanSIDList::DeleteEntryL(TInt aIndex)
/**
Deletes an entry in the stored Wlan Specific identifier list. 

@param aIndex Index of entry to be deleted.
@return KErrNone if successful;  KErrArgument if operation is attempted on non-user defined (i.e. TWlanSIDV8.iUserDefined==EFalse) entry.
@capability WriteDeviceData
*/
	{
	if(IsEntryUserDefinedL(aIndex))	
		{
		CMobilePhoneEditableList<RMobilePhone::TWlanSIDV8>::DeleteEntryL(aIndex);
		return KErrNone;
		}
	else
		{
		return KErrArgument;	
		}
	}
	
TBool CMobilePhoneStoredWlanSIDList::IsEntryUserDefinedL(TInt aIndex)
	{
	if (aIndex < iList.Count() && !GetEntryL(aIndex).iUserDefined)
		return EFalse;
	return ETrue;	
	}
