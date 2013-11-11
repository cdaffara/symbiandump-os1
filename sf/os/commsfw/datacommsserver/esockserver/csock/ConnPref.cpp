// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalTechnology 
*/

#include "ConnPref.h"
#include <es_connpref.h>

#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>

using namespace Meta;


EXPORT_C TConnPref::TConnPref()
: TBuf8<KMaxConnPrefSize>()
/**
Default Constructor 
Setting length to 0x40 and Extension ID to 0x00

*/
	{
	SetLength(KMaxConnPrefSize);
	SetExtensionId(TConnPref::EConnPrefUnknown);
	}

EXPORT_C TConnPref::TConnPref(TInt aExtensionId)
: TBuf8<KMaxConnPrefSize>()
/**
Constructor 
Setting length to 0x40 and Extension ID to aExtensionId

@param aExtensionId
*/
	{
	SetLength(KMaxConnPrefSize);
	SetExtensionId(aExtensionId);
	}

EXPORT_C void TConnPref::SetExtensionId(TInt aExtensionId)
/**
Set the extension (derrived class) id

@param aExtensionId
*/
	{
	BasePtr()->iExtensionId = aExtensionId;
	}

EXPORT_C TInt TConnPref::ExtensionId() const
/**
Get the extension (derrived class) id 

@return Extension ID.
*/
	{
	return BasePtr()->iExtensionId;
	}

EXPORT_C void TConnPref::SetUserLen(TInt aLen)
/**
Set descriptor length to actual length of subclass

@param aLen,length of subclass.
*/
	{

	SetLength(aLen+Align4(sizeof(SConnPref)));
	}

EXPORT_C TInt TConnPref::GetUserLen()
/**
Get the length of the subclass 

@return length of the subclass 
*/
	{

	return Length()-Align4(sizeof(SConnPref));
	}

struct SCommApPref
/**
@internalTechnology
@released since v9.1
*/
	{
	TUint32 iAp;
	};

EXPORT_C TConnSnapPref::TConnSnapPref() :
	TConnPref(EConnPrefSnap)
	{
	SetSnap( 0 );
	}
		
EXPORT_C TConnSnapPref::TConnSnapPref(TUint32 aSnap) :
	TConnPref(EConnPrefSnap)
	{
	SetSnap( aSnap );
	}

EXPORT_C void TConnSnapPref::SetSnap(TUint32 aSnap)
/**
Sets SNAP
@publishedAll
@param aSnap - new SNAP value
@released since v9.1
*/
	{
	SCommApPref* prefPtr = reinterpret_cast<SCommApPref*>(UserPtr());
	prefPtr->iAp = aSnap;
	}

EXPORT_C TUint32 TConnSnapPref::Snap() const
/**
@publishedPartner
@return stored SNAP value
@released since v9.1
*/
	{
	return reinterpret_cast<SCommApPref*>(UserPtr())->iAp;
	}


EXPORT_C TConnIdList::TConnIdList() :
	TConnPref(EConnPrefIdList)
	{
	CountRef() = 0;
	}

EXPORT_C TInt TConnIdList::MaxCount()
/**
@publishedPartner
@return max number of APs to store
@released since v9.1
*/
	{
	return (KMaxConnPrefSize - sizeof(TInt))/sizeof(TInt);
	}

EXPORT_C TInt& TConnIdList::operator[](TInt aIndex)
/**
operator to access the list based on index
@publishedPartner
@param aIndex - index
@released since v9.1
*/
	{
	if (! (aIndex >= 0 && aIndex < Count()))
		{
	  	User::Panic (KNullDesC, KErrOverflow);
     	}
	
	return *(IdList() + aIndex);
	}

EXPORT_C void TConnIdList::Append(TInt aId)
/**
Appends new Id
@publishedPartner
@param aAp - Id to add
@released since v9.1
*/
	{
	TUint16& count = CountRef();
	if (count >= MaxCount())
		{
		User::Panic (KNullDesC, KErrOverflow);
		}
	
	(*this)[count++] = aId;
	}

EXPORT_C TInt TConnIdList::Get(TInt aIndex) const
/**
Allows read only access to Id based on index
@publishedPartner
@released since v9.1
*/
	{
	//cast the const away to reuse the op[] tha's not changing anything
	return (*(const_cast<TConnIdList*>(this)))[aIndex];
	}

EXPORT_C TInt TConnIdList::Count() const
/**
@publishedPartner
@return number of Ids in the list
@released since v9.1
*/
	{
	//cast the const away to reuse the CountRef() fn that's not changing anything
	return (const_cast<TConnIdList*>(this))->CountRef();
	}


EXPORT_C TConnPrefList::TConnPrefList()
/**
Constructor for TConnPrefList
@publishedAll
*/
	{
	SetExtensionId(TConnPref::EConnPrefEComList);
	}

EXPORT_C TConnPrefList::~TConnPrefList()
/**
Destrcutor - closes iPrefs deleting the pointers but not the objects they point
at. These should be deleted explicitly.
@publishedAll
*/
	{
	iPrefs.Close();
	}

EXPORT_C TConnPrefList* TConnPrefList::NewL()
	{
	TConnPrefList* self = new (ELeave) TConnPrefList();
	return self;	
	}

EXPORT_C TConnPrefList* TConnPrefList::LoadL(TDesC8& aDes)
/**
This function loads an instance of TConnPrefList from the given descriptor and
returns a pointer to that oject in memory is possible, otherwise leaves
@publishedAll
@param aDes - a descriptor containing a serialized form of TConnPrefList
@return TConnPrefList* to the instance of the object in memory
*/
	{
	TConnPrefList* obj = TConnPrefList::NewL();
	CleanupStack::PushL(obj);
	User::LeaveIfError(obj->Load(aDes));
	CleanupStack::Pop(obj);
	return obj;
	}

EXPORT_C TInt TConnPrefList::Length()
/**
Returns the size of the desciptor needed to store the object using the store
function
@publishedAll
@return Integer representing the length of descriptor need to store the object
*/
	{
	return KMaxConnPrefSize + iPrefs.Length();
	}

EXPORT_C TInt TConnPrefList::Load(TDesC8& aDes)
/**
This function loads an instance of TConnPrefList from the given descriptor into 
the object its called on. It returns an integer error on failiure
@publishedAll
@param - a descriptor containing a serialized instance of TConnPrefList
@return - KErrNone if successful otherwise KErrArgument
*/
	{	
	TPtrC8 aBuffer(aDes);
	
	Mem::Copy(&iBuf, aBuffer.Ptr() ,sizeof(iBuf));
	aBuffer.Set(aBuffer.Ptr() + sizeof( iBuf), aBuffer.Length() - sizeof(iBuf));
	
	if(ExtensionId() == TConnPref::EConnPrefUnknown || ExtensionId() == TConnPref::EConnPrefCommDb || 
			ExtensionId() == TConnPref::EConnPrefCommDbMulti || ExtensionId() == TConnPref::EConnPrefSnap ||
			ExtensionId() == TConnPref::EConnPrefIdList || ExtensionId() == TConnPref::EConnPrefSip ||
			ExtensionId() == TConnPref::EConnPrefProviderInfo)
		{
			return KErrNone;
		}
	else if(ExtensionId() != TConnPref::EConnPrefEComList)
		{
			return KErrArgument;
		}
		
	TInt ret = iPrefs.GetTypeId().Check(aBuffer);
	if(ret != KErrNone)
		{
		return ret;
		}
	return ret = iPrefs.Load(aBuffer);
	}

EXPORT_C TInt TConnPrefList::Store(TDes8& aDes)
/**
This puts a serilized form of the object that the function is called on into the
descriptor supplied
@publishedAll
@param - a descriptor that is sufficiently long enough to contain the serialized
version of the object
@return - KErrNone if successful
 		- KErrOverflow if the descriptor is too short
 		- otherwise one of the standard error codes
*/
	{
	TUint length = Length();
	
	aDes.Append((TUint8*)&iBuf,sizeof(iBuf));
	
	if(aDes.MaxLength() < length)
		{
		return KErrOverflow;
		}
	
	TInt ret = iPrefs.Store(aDes);
	if(ret != KErrNone)
		{
		return ret;
		}
		 
	return KErrNone;
	}

EXPORT_C void TConnPrefList::AppendL(SMetaDataECom* aFamily)
/**
This adds the pointer supplied to the list. Also see TConnAPPref
Only one instance of TConnAPPref can be appended to the list
and passed to RConnection::Start. More than one will force
RConnection::Start to return KErrArgument
@publishedAll
@param - a pointer to an SMetaDataECom object
*/
	{
	if(aFamily == NULL)
		{
		User::Leave(KErrArgument);
		}
	iPrefs.AppendL(aFamily);
	}

EXPORT_C SMetaData* TConnPrefList::operator[](TInt aIndex)
/**
This retrieves a pointer to the object at the given index
@publishedAll
@param - aIndex - the position of the object in the list
@return a pointer to the object in memory
*/
	{
	if(iPrefs.Count() > aIndex)
		{
		return iPrefs[aIndex];
		}
	User::Leave(KErrArgument);
	return NULL;
	}

EXPORT_C void TConnPrefList::Remove(TInt aIndex)
/**
This removes the objet at the given index from the list
@publishedAll
@param - aIndex - the position of the object in the list
*/
	{
	if(iPrefs.Count() > aIndex)
		{
		iPrefs.Remove(aIndex);
		}
	}

EXPORT_C TInt TConnPrefList::Count()
/**
This returns an integer value representing the number of objects in the list
@publishedAll
@return - integer representing the number of objects in the list
*/
	{
	return iPrefs.Count();
	}


