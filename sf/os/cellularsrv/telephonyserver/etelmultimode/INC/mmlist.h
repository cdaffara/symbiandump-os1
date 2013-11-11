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
// Multimode ETel API v1.0
// Header file for phone list classes
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef _MMLIST_H_
#define _MMLIST_H_

#include <etelmm.h>
#include <e32base.h>



class CMobilePhoneListBase : public CBase
/**
A base class within a thin-template implementation.

Implements the methods for "read-only" access to a list class.
Entries are stored in a CArrayFixFlat<TAny>.

@publishedPartner
@released
*/

	{
public:
	IMPORT_C ~CMobilePhoneListBase();
	
	IMPORT_C TInt Enumerate() const;
	IMPORT_C TInt MaxNumberEntries() const;
	IMPORT_C void SetMaxNumberEntries(TInt aMax);

	IMPORT_C void StoreL(TDes8& aDes);
	IMPORT_C void RestoreL(const TDesC8& aBuf);
	IMPORT_C CBufBase* StoreLC();

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	enum 
		{
		KMaxEntriesNotSet = - 1
		};

	enum
		{
		EBadRange,
		EListIndexOutOfRange,
		EListMaxNumberReached
		};

protected:
	IMPORT_C CMobilePhoneListBase(TInt aLength, TInt aGranularity);
	IMPORT_C const TAny* GetEntryL(TInt aIndex) const;
	IMPORT_C void AddEntryL(const TAny* aEntry);
	/** Pure virtual function that should internalise entry *aPtr from the aStream 
	read stream. This member function is used by InternalizeL to internalise the 
	separate entries in the stream.
	
	@param aPtr On return, pointer to the object that will contain the entry.
	@param aStream The read stream containing the entry to be internalized. */
	virtual void InternalizeEntryL(TAny* aPtr,RReadStream& aStream) const=0;
	/** Pure virtual function that should externalise entry *aPtr into the aStream 
	write stream. This member function is used by ExternalizeL to externalise 
	the separate entries in the list.
	
	@param aPtr Pointer to the entry to be written to the stream
	@param aStream On return, the write stream. */
	virtual void ExternalizeEntryL(const TAny* aPtr,RWriteStream& aStream) const=0;

protected:
	CArrayFixFlat<TAny> iList;
	TInt iMaxNumber;
	};



class CMobilePhoneEditableListBase : public CMobilePhoneListBase
/**
A base class within a thin-template implementation

Implements the methods for "write" access to a list class

@publishedPartner
@released
*/
{
public:
	IMPORT_C ~CMobilePhoneEditableListBase();

	IMPORT_C void DeleteEntryL(TInt aIndex);
protected:
	IMPORT_C void InsertEntryL(TInt aIndex, const TAny* aEntry);
	IMPORT_C CMobilePhoneEditableListBase(TInt aLength, TInt aGranularity);
};



template<class T>
class CMobilePhoneReadOnlyList : public CMobilePhoneListBase
/**
A template class within a thin-template implementation.

Provides the type-safe wrappers for "read-only" access to a list class.

@publishedPartner
@released
*/
{
protected:
	inline CMobilePhoneReadOnlyList();
public:
	inline void AddEntryL(const T& aEntry);
	inline const T& GetEntryL(TInt aIndex) const; 

private:
	void InternalizeEntryL(TAny* aPtr,RReadStream& aStream) const
	{aStream >> *(new(aPtr) T);} // use in place new to construct T
	void ExternalizeEntryL(const TAny* aPtr,RWriteStream& aStream) const
		{aStream << *static_cast<const T*>(aPtr);}
};



template<class T>
class CMobilePhoneEditableList : public CMobilePhoneEditableListBase
/**
A template class within a thin-template implementation

Provides the type-safe wrappers for "read-write" access to the
CMobilePhoneEditableListBase list class

@publishedPartner
@released
*/
{
protected:
	inline CMobilePhoneEditableList();
public:
	inline void AddEntryL(const T& aEntry);
	inline const T& GetEntryL(TInt aIndex) const; 
	inline void InsertEntryL(TInt aIndex, const T& aEntry);
	inline void ChangeEntryL(TInt aIndex, const T& aEntry);

private:
	void InternalizeEntryL(TAny* aPtr,RReadStream& aStream) const
	{aStream >> *(new(aPtr) T);} // use in place new to construct T
	void ExternalizeEntryL(const TAny* aPtr,RWriteStream& aStream) const
		{aStream << *static_cast<const T*>(aPtr);}
};


/********************************************************************/
//
// CMobilePhoneReadOnlyList<T>
// Implementation of the inline wrapper methods
//
/********************************************************************/
/**
@publishedPartner
@released
*/
template <class T>
inline CMobilePhoneReadOnlyList<T>::CMobilePhoneReadOnlyList()
	: CMobilePhoneListBase(sizeof(T), 1)
	{}

template <class T>
inline const T& CMobilePhoneReadOnlyList<T>::GetEntryL(TInt aIndex) const
	/** This function member retrieves the entry at position aIndex from the list. 
	A valid index ranges from 0 to count -1, where count is the value returned 
	from CMobilePhoneListBase::Enumerate().
	
	@param aIndex Index of the entry to be retrieved.
	@leave EListIndexOutOfRange This function member will leave if aIndex is outside 
	the current number of entries.
	@return Pointer to the entry at position aIndex. 
	@publishedPartner
	@released
	*/
	{return(*((const T*)CMobilePhoneListBase::GetEntryL(aIndex)));}

template <class T>
inline void CMobilePhoneReadOnlyList<T>::AddEntryL(const T& aEntry)
	/** This function member adds a new entry into the list, placing it in the next 
	empty position.
	
	@param aEntry Pointer to the entry that will be added to the list.
	@leave EListMaxNumberReached This function member will leave if the list fails 
	to allocate memory for the new entry or if the maximum number of entries has 
	been reached. 
	@publishedPartner
	@released
	*/
	{CMobilePhoneListBase::AddEntryL(&aEntry);}
	
/********************************************************************/
//
// CMobilePhoneEditableList<T>
// Implementation of the inline wrapper methods
//
/********************************************************************/

template <class T>
inline CMobilePhoneEditableList<T>::CMobilePhoneEditableList()
	: CMobilePhoneEditableListBase(sizeof(T), 1)
	/** Constructor.
	
	@see CMobilePhoneEditableListBase::CMobilePhoneEditableListBase() 
	@publishedPartner
	@released
	*/
	{}

template <class T>
inline const T& CMobilePhoneEditableList<T>::GetEntryL(TInt aIndex) const
	/** Gets entry from the list at the given position.
	
	@param aIndex Index into the list of the entry to get.
	@return The entry from the list at position aIndex.
	@see CMobilePhoneListBase::GetEntryL() 
	@publishedPartner
	@released
	*/
	{return(*((const T*)CMobilePhoneListBase::GetEntryL(aIndex)));}

template <class T>
inline void CMobilePhoneEditableList<T>::AddEntryL(const T& aEntry)
	/** Adds entry at the given position.
	
	@param aEntry Entry to be added to list.
	@see CMobilePhoneListBase::AddEntryL() 
	@publishedPartner
	@released
	*/
	{CMobilePhoneListBase::AddEntryL(&aEntry);}

template <class T>
inline void CMobilePhoneEditableList<T>::InsertEntryL(TInt aIndex, const T& aEntry)
	/** This function member inserts a new list entry at the specified index.
	
	@param aIndex Index of the point at which to insert the new entry.
	@param aEntry Pointer to the new entry.
	@see CMobilePhoneEditableListBase::InsertEntryL() 
	@publishedPartner
	@released
	*/
	{CMobilePhoneEditableListBase::InsertEntryL(aIndex, &aEntry);}

template <class T>
inline void CMobilePhoneEditableList<T>::ChangeEntryL(TInt aIndex, const T& aEntry)
	/** This function member changes the entry at position aIndex in the list.
	
	@param aIndex The entry position to change. A valid index ranges from 0 to 
	count-1, where count is the value returned from CMobilePhoneListBase::Enumerate().
	@param aEntry Entry to replace the entry at position aEntry in the list.
	@see CMobilePhoneEditableList::ChangeEntryL() 
	@publishedPartner
	@released
	*/
	{*((T*)CMobilePhoneListBase::GetEntryL(aIndex))=aEntry;}




class CMobilePhoneNetworkList : public CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneNetworkInfoV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of detected networks retrieved from the phone.

@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneNetworkList* NewL();
	IMPORT_C ~CMobilePhoneNetworkList ();
protected:
	CMobilePhoneNetworkList();
private:
	void ConstructL();
};



class CMobilePhoneNetworkListV2 : public CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneNetworkInfoV2>
/**
An instantiation of the list thin-template.

Used to hold the V2 "read-only" list of detected networks retrieved from the phone.

This class is used in conjunction with the V2 method for detected networks retrieval

@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneNetworkListV2* NewL();
	IMPORT_C ~CMobilePhoneNetworkListV2();
protected:
	CMobilePhoneNetworkListV2();
};

class CMobilePhoneNetworkListV5 : public CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneNetworkInfoV5>
/**
An instantiation of the list thin-template.

Used to hold the V5 "read-only" list of detected networks retrieved from the phone.

This class is used in conjunction with the V5 method for detected networks retrieval

@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneNetworkListV5* NewL();
	IMPORT_C ~CMobilePhoneNetworkListV5();
protected:
	CMobilePhoneNetworkListV5();
};

class CMobilePhoneNetworkListV8 : public CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneNetworkInfoV8>
/**
An instantiation of the list thin-template.

Used to hold the V8 "read-only" list of detected networks retrieved from the phone.

This class is used in conjunction with the V8 method for detected networks retrieval

@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneNetworkListV8* NewL();
	IMPORT_C ~CMobilePhoneNetworkListV8();
protected:
	CMobilePhoneNetworkListV8();
};

class CMobilePhoneCFList : public CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneCFInfoEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of Call Forwarding status entries retrieved from the phone.

@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneCFList* NewL();
	IMPORT_C ~CMobilePhoneCFList ();
protected:
	CMobilePhoneCFList();
private:
	void ConstructL();
};



class CMobilePhoneCBList : public CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneCBInfoEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of Call Barring status entries retrieved from the phone.

@publishedPartner
@released


@see RMobilePhone::TMobilePhoneCBInfoEntryV1 
*/
{
public:
	IMPORT_C static CMobilePhoneCBList* NewL();
	IMPORT_C ~CMobilePhoneCBList();
protected:
	CMobilePhoneCBList();
private:
	void ConstructL();
};



class CMobilePhoneCWList : public CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneCWInfoEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of Call Waiting status entries retrieved from the phone.

@see RMobilePhone::TMobilePhoneCWInfoEntryV1
@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneCWList* NewL();
	IMPORT_C ~CMobilePhoneCWList();
protected:
	CMobilePhoneCWList();
private:
	void ConstructL();
};



class CMobilePhoneCcbsList : public CMobilePhoneReadOnlyList<RMobilePhone::TMobilePhoneCCBSEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of CCBS request entries retrieved from the phone.

@publishedPartner
@released
@see RMobilePhone::TMobilePhoneCCBSEntryV1 
*/
{
public:
	IMPORT_C static CMobilePhoneCcbsList* NewL();
	IMPORT_C ~CMobilePhoneCcbsList();
protected:
	CMobilePhoneCcbsList();
private:
	void ConstructL();
};




class CMobilePhoneGsmSmsList : public CMobilePhoneReadOnlyList<RMobileSmsStore::TMobileGsmSmsEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of SMS messages retrieved from the phone.

@see RMobileSmsStore::TMobileGsmSmsEntryV1
@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneGsmSmsList* NewL();
	IMPORT_C ~CMobilePhoneGsmSmsList ();
protected:
	CMobilePhoneGsmSmsList();
private:
	void ConstructL();
};



class CMobilePhoneCdmaSmsList : public CMobilePhoneReadOnlyList<RMobileSmsStore::TMobileCdmaSmsEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of SMS messages retrieved from the phone.

@see RMobileSmsStore::TMobileCdmaSmsEntryV1
@publishedPartner
@released
@deprecated 9.5
*/
{
public:
	IMPORT_C static CMobilePhoneCdmaSmsList* NewL();
	IMPORT_C ~CMobilePhoneCdmaSmsList ();
protected:
	CMobilePhoneCdmaSmsList();
private:
	void ConstructL();
};



class CMobilePhoneSmspList : public CMobilePhoneEditableList<RMobileSmsMessaging::TMobileSmspEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-write" list of SMS parameter entries retrieved from the phone.

@see RMobileSmsMessaging::TMobileSmspEntryV1
@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneSmspList* NewL();
	IMPORT_C ~CMobilePhoneSmspList ();
protected:
	CMobilePhoneSmspList();
private:
	void ConstructL();
};

class CMobilePhoneNamList : public CMobilePhoneEditableList<RMobileNamStore::TMobileNamEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-write" list of NAM entries retrieved from the phone

@publishedPartner
@released
@see RMobileNamStore::TMobileNamEntryV1
@deprecated 9.5
*/
{
public:
	IMPORT_C static CMobilePhoneNamList* NewL();
	IMPORT_C ~CMobilePhoneNamList();
protected:
	CMobilePhoneNamList();
private:
	void ConstructL();
};

class CMobilePhoneNamListV4 : public CMobilePhoneEditableList<RMobileNamStore::TMobileNamEntryV4>
/**
An instantiation of the list thin-template.

Used to hold the "read-write" list of NAM entries retrieved from the phone

@publishedPartner
@released
@see RMobileNamStore::TMobileNamEntryV4
@deprecated 9.5
*/
{
public:
	IMPORT_C static CMobilePhoneNamListV4* NewL();
	IMPORT_C ~CMobilePhoneNamListV4();
protected:
	CMobilePhoneNamListV4();
};

class CMobilePhoneBroadcastIdList : public CMobilePhoneEditableList<RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-write" list of Broadcast ID entries retrieved from the phone.

@see RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1
@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneBroadcastIdList* NewL();
	IMPORT_C ~CMobilePhoneBroadcastIdList ();
	IMPORT_C void AddRangeEntryL(const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& aStart, const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& aEnd);

protected:
	CMobilePhoneBroadcastIdList();
private:
	void ConstructL();
};




class CMobilePhoneONList : public CMobilePhoneEditableList<RMobileONStore::TMobileONEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-write" list of Own Numbers retrieved from the phone

@see RMobileONStore::TMobileONEntryV1
@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneONList* NewL();
	IMPORT_C ~CMobilePhoneONList();
protected:
	CMobilePhoneONList();
private:
	void ConstructL();
};



class CMobilePhoneENList : public CMobilePhoneReadOnlyList<RMobileENStore::TMobileENEntryV1>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of Emergency Numbers retrieved from the phone.

@see RMobileENStore::TMobileENEntryV1
@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneENList* NewL();
	IMPORT_C ~CMobilePhoneENList();
protected:
	CMobilePhoneENList();
private:
	void ConstructL();
};


class CMobilePhoneStoredNetworkList : public CMobilePhoneEditableList<RMobilePhone::TMobilePreferredNetworkEntryV3>
/**
Is an instantiation of the list thin-template inheriting from CMobilePhoneEditableList. 
Used to hold an editable list of stored network entries. List manipulation functions are restricted to only be applied to entries 
that are of a used defined type. i.e. TMobilePreferredNetworkEntryV3.iUserDefined == ETrue.

@see RMobilePhone::TMobilePreferredNetworkEntryV3
@publishedPartner
@released
*/
{
public:
	IMPORT_C static CMobilePhoneStoredNetworkList* NewL();
	IMPORT_C ~CMobilePhoneStoredNetworkList ();
	 
	IMPORT_C TInt InsertEntryL(TInt aIndex, const RMobilePhone::TMobilePreferredNetworkEntryV3& aEntry);
	IMPORT_C TInt ChangeEntryL(TInt aIndex, const RMobilePhone::TMobilePreferredNetworkEntryV3& aEntry); 
	IMPORT_C TInt DeleteEntryL(TInt aIndex);
protected:
	CMobilePhoneStoredNetworkList();
private:
	TBool IsEntryUserDefinedL(TInt aIndex);
};

class CMobilePhoneGbaNafIdList : public CMobilePhoneReadOnlyList<RMobilePhone::TGbaNafEntryV8>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of Gba Naf(EF_GBANL) List retrieved from the phone.

@see RMobilePhone::TGbaNafEntryV8

@publishedPartner
@released
*/
	{
public:
	IMPORT_C static CMobilePhoneGbaNafIdList* NewL();
	IMPORT_C ~CMobilePhoneGbaNafIdList ();
protected:
	CMobilePhoneGbaNafIdList();
private:
	void ConstructL();
	};

class CMobilePhoneMbmsMskIdList : public CMobilePhoneReadOnlyList<RMobilePhone::TMskEntryV8>
/**
An instantiation of the list thin-template.

Used to hold the "read-only" list of MBMS MSK ID(EF_MSK) List retrieved from the phone.

@see RMobilePhone::TMskEntryV8

@publishedPartner
@released
*/
	{
public:
	IMPORT_C static CMobilePhoneMbmsMskIdList* NewL();
	IMPORT_C ~CMobilePhoneMbmsMskIdList ();
protected:
	CMobilePhoneMbmsMskIdList();
private:
	void ConstructL();
	};	

class CMobilePhoneStoredWlanSIDList	: public CMobilePhoneEditableList<RMobilePhone::TWlanSIDV8>
/**
An instantiation of the list thin-template.

Used to hold the "read-write" list of Wlan Specific identifier list(User/Operator controlled)
retrieved from the phone.

@see RMobilePhone::TWlanSIDV8

@publishedPartner
@released
*/
	{
public:
	IMPORT_C static CMobilePhoneStoredWlanSIDList* NewL();
	IMPORT_C ~CMobilePhoneStoredWlanSIDList ();
public:
	IMPORT_C TInt InsertEntryL(TInt aIndex, const RMobilePhone::TWlanSIDV8& aEntry);
	IMPORT_C TInt ChangeEntryL(TInt aIndex, const RMobilePhone::TWlanSIDV8& aEntry); 
	IMPORT_C TInt DeleteEntryL(TInt aIndex);
protected:
	CMobilePhoneStoredWlanSIDList();
private:
	TBool IsEntryUserDefinedL(TInt aIndex);
	};	

#endif
