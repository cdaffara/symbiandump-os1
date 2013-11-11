// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined METATYPE_H)
#define METATYPE_H

#include <e32base.h>
#include <e32std.h>

namespace Meta
{

template<class TYPE> class TMeta;
typedef TMeta<TInt> TMetaNumber;
typedef TMeta<TTime> TMetaTime;

class MMetaType
/**

Abstract interface of a helper meta type. A meta type is responsible for handling
a particular type. A meta type knows how to copy an object of the type, store it
in a descriptor, load, etc, etc.

@internalComponent
@released since v9.0 */
    {
public:
	virtual TInt Load(TPtrC8& aBuffer) = 0;
	virtual TInt Store(TDes8& aBuffer) const = 0;
    virtual void Copy(const TAny* aData) = 0;
	virtual TInt Length() const = 0;
    };

template<class TYPE>
class TMeta : public MMetaType
/**

Implementation of MMetaType for simple types (e.g. symbian T-types).

@internalComponent
@released since v9.0 */
    {
public:
    inline static MMetaType* NewL(const TAny* aMem, const TAny* aData);

public:
	virtual TInt Load(TPtrC8& aBuffer);
	virtual TInt Store(TDes8& aBuffer) const;
    virtual void Copy(const TAny* aData);
	virtual TInt Length() const;

protected:
    TMeta(const TAny* aData);

private:
    const TYPE* iData;
    };



template<class TYPE>
NONSHARABLE_CLASS(TMetaNS) : public MMetaType
/**

Implementation of MMetaType for simple types (e.g. symbian T-types).
This version of TMeta avoids the unwanted exports of the typeinfo and vtable in ARMv5 builds
Note: A defect will be raised to fix TMeta, and remove this class.

@internalComponent
@released since v9.5
*/
    {
public:
    inline static MMetaType* NewL(const TAny* aMem, const TAny* aData);

public:
	virtual TInt Load(TPtrC8& aBuffer);
	virtual TInt Store(TDes8& aBuffer) const;
    virtual void Copy(const TAny* aData);
	virtual TInt Length() const;

protected:
    TMetaNS(const TAny* aData);

private:
    const TYPE* iData;
    };

template<class TYPE>
class TMetaObject : public MMetaType
/**

Implementation of MMetaType for meta objects

@internalComponent
@released since v9.0 */
	{
public:
    inline static MMetaType* NewL(const TAny* aMem, const TAny* aData);

public:
	virtual TInt Load(TPtrC8& aBuffer);
	virtual TInt Store(TDes8& aBuffer) const;
    virtual void Copy(const TAny* aData);
	virtual TInt Length() const;

protected:
    TMetaObject(const TAny* aData);

private:
    TYPE* iData;
	};

template<class TYPE>
class TMetaObjectPtr : public MMetaType
/**

Implementation of MMetaType for meta pointers to objects that are never created

@internalComponent
@released since v9.0 */
	{
public:
    inline static MMetaType* NewL(const TAny* aMem, const TAny* aData);

public:
	virtual TInt Load(TPtrC8& aBuffer);
	virtual TInt Store(TDes8& aBuffer) const;
    virtual void Copy(const TAny* aData);
	virtual TInt Length() const;

protected:
    TMetaObjectPtr(const TAny* aData);

private:
    TYPE** iData;
	};

template<class TYPE>
class TMetaPtr : public MMetaType
/**

Implementation of MMetaType for pointers to meta objects

@internalComponent
@released since v9.0 */
	{
public:
    inline static MMetaType* NewL(const TAny* aMem, const TAny* aData);

	virtual TInt Load(TPtrC8& aBuffer);
	virtual TInt Store(TDes8& aBuffer) const;
    virtual void Copy(const TAny* aData);
	virtual TInt Length() const;

protected:
    TMetaPtr(const TAny* aData);

private:
    TYPE** iData;
	};

template<class TYPE>
MMetaType* TMeta<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type of a particular type.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMeta<TYPE>(aData);
    }

template<class TYPE>
inline TMeta<TYPE>::TMeta(const TAny* aData)
:    iData((TYPE*)aData)
/**
 * Constructor
 */
    {
    __ASSERT_DEBUG(iData!=NULL,User::Panic(_L("TMetaType"),KErrArgument));
    }

template<class TYPE>
TInt TMeta<TYPE>::Load(TPtrC8& aBuffer)
/**
 * Loads content of a meta object (in iData) from a descriptor
 */
    {
	TInt len = Length();
	if (aBuffer.Length() < len)
		{
		return KErrArgument;
		}
    Mem::Copy((TYPE*)iData,aBuffer.Ptr(),len);
	aBuffer.Set(aBuffer.Ptr()+len, aBuffer.Length()-len); //update pointer
	return KErrNone;
    }

template<class TYPE>
TInt TMeta<TYPE>::Store(TDes8& aBuffer) const
/**
 * Stores content of a meta object (in iData) to a descriptor
 */
    {
	TInt len = Length();
	if (aBuffer.MaxLength() - aBuffer.Length() < len)
		{
		return KErrOverflow;
		}
	aBuffer.Append((TUint8*)iData, len);
	return KErrNone;
    }

template<class TYPE>
void TMeta<TYPE>::Copy(const TAny* aData)
/**
 * Copies content of a meta object (in aData) into another meta object (in iData).
 * This is a MMetaType implementation for simple (T) types so it just copies the memory.
 */
    {
    Mem::Copy((TAny*)iData,aData,Length());
    }

template<class TYPE>
TInt TMeta<TYPE>::Length() const
/**
 * Returns the length of the handled meta object.
 */
    {
    return sizeof(TYPE);
    }







//-----------------------
template<class TYPE>
MMetaType* TMetaNS<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type of a particular type.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaNS<TYPE>(aData);
    }

template<class TYPE>
inline TMetaNS<TYPE>::TMetaNS(const TAny* aData)
:    iData((TYPE*)aData)
/**
 * Constructor
 */
    {
    __ASSERT_DEBUG(iData!=NULL,User::Panic(_L("TMetaType"),KErrArgument));
    }

template<class TYPE>
TInt TMetaNS<TYPE>::Load(TPtrC8& aBuffer)
/**
 * Loads content of a meta object (in iData) from a descriptor
 */
    {
	TInt len = Length();
	if (aBuffer.Length() < len)
		{
		return KErrArgument;
		}
    Mem::Copy((TYPE*)iData,aBuffer.Ptr(),len);
	aBuffer.Set(aBuffer.Ptr()+len, aBuffer.Length()-len); //update pointer
	return KErrNone;
    }

template<class TYPE>
TInt TMetaNS<TYPE>::Store(TDes8& aBuffer) const
/**
 * Stores content of a meta object (in iData) to a descriptor
 */
    {
	TInt len = Length();
	if (aBuffer.MaxLength() - aBuffer.Length() < len)
		{
		return KErrOverflow;
		}
	aBuffer.Append((TUint8*)iData, len);
	return KErrNone;
    }

template<class TYPE>
void TMetaNS<TYPE>::Copy(const TAny* aData)
/**
 * Copies content of a meta object (in aData) into another meta object (in iData).
 * This is a MMetaType implementation for simple (T) types so it just copies the memory.
 */
    {
    Mem::Copy((TAny*)iData,aData,Length());
    }

template<class TYPE>
TInt TMetaNS<TYPE>::Length() const
/**
 * Returns the length of the handled meta object.
 */
    {
    return sizeof(TYPE);
    }
//-----------------------



template<class TYPE>
MMetaType* TMetaObject<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type of a particular type.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaObject<TYPE>(aData);
    }

template<class TYPE>
inline TMetaObject<TYPE>::TMetaObject(const TAny* aData)
:    iData((TYPE*)aData)
/**
 * Constructor
 */
    {
    __ASSERT_DEBUG(iData!=NULL,User::Panic(_L("TMetaType"),KErrArgument));
    }

template<class TYPE>
TInt TMetaObject<TYPE>::Load(TPtrC8& aBuffer)
/**
 * Loads content of a meta object (in iData) from a descriptor
 */
   {
	TInt err = iData->GetTypeId().Check( aBuffer );
	return err == KErrNone ? iData->Load(aBuffer) : err;
   }

template<class TYPE>
TInt TMetaObject<TYPE>::Store(TDes8& aBuffer) const
/**
 * Stores content of a meta object (in iData) to a descriptor
 */
    {
	return iData->Store(aBuffer);
    }

template<class TYPE>
void TMetaObject<TYPE>::Copy(const TAny* aData)
/**
 * Copies content of a meta object (in aData) into another meta object (in iData).
 */
    {
	iData->Copy(*((TYPE*)aData));
    }

template<class TYPE>
TInt TMetaObject<TYPE>::Length() const
/**
 * Returns the length of the handled meta object.
 */
    {
	return iData->Length();
    }

template<class TYPE>
MMetaType* TMetaObjectPtr<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type of a particular type.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaObjectPtr<TYPE>(aData);
    }

template<class TYPE>
inline TMetaObjectPtr<TYPE>::TMetaObjectPtr(const TAny* aData)
:    iData((TYPE**)aData)
/**
 * Constructor
 */
    {
    __ASSERT_DEBUG(iData!=NULL,User::Panic(_L("TMetaType"),KErrArgument));
    __ASSERT_DEBUG(*iData!=NULL,User::Panic(_L("TMetaType"),KErrArgument));
    }

template<class TYPE>
TInt TMetaObjectPtr<TYPE>::Load(TPtrC8& aBuffer)
/**
 * Loads content of a meta object (in iData) from a descriptor
 */
   {
	TInt err = (*iData)->GetTypeId().Check( aBuffer );
	return err == KErrNone ? (*iData)->Load(aBuffer) : err;
   }

template<class TYPE>
TInt TMetaObjectPtr<TYPE>::Store(TDes8& aBuffer) const
/**
 * Stores content of a meta object (in iData) to a descriptor
 */
    {
	return (*iData)->Store(aBuffer);
    }

template<class TYPE>
void TMetaObjectPtr<TYPE>::Copy(const TAny* aData)
/**
 * Copies content of a meta object (in aData) into another meta object (in iData).
 */
    {
	(*iData)->Copy(*((TYPE*)aData));
    }

template<class TYPE>
TInt TMetaObjectPtr<TYPE>::Length() const
/**
 * Returns the length of the handled meta object.
 */
    {
	return (*iData)->Length();
    }

template<class TYPE>
inline TMetaPtr<TYPE>::TMetaPtr(const TAny* aData)
:    iData((TYPE**)aData)
/**
 * Constructor
 */
    {
    }

template<class TYPE>
MMetaType* TMetaPtr<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type of a particular type.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaPtr<TYPE>(aData);
    }

template<class TYPE>
TInt TMetaPtr<TYPE>::Load(TPtrC8& aBuffer)
/**
 * Loads content of a meta object (in iData) from a descriptor
 */
    {
	if (aBuffer.Length() < (TInt) sizeof(TUint32))
		{
		return KErrArgument;
		}

	// Check for a NULL pointer when stored (First four bytes == 0)
	if (*((TUint32*)aBuffer.Ptr()) == 0)
		{
		// The pointer was NULL when it was stored
		aBuffer.Set(aBuffer.Ptr() + sizeof(TUint32), aBuffer.Length() - sizeof(TUint32));
		if (*iData != NULL)
			{
			delete *iData;
			*iData = NULL;
			}
		return KErrNone;
		}

	if (*iData == NULL)
		{
		TRAPD(ret, *iData = reinterpret_cast<TYPE*>(TYPE::LoadL(aBuffer)));
		return ret;
		}
	else
		{
		return (*iData)->Load(aBuffer);
		}
    }

template<class TYPE>
TInt TMetaPtr<TYPE>::Store(TDes8& aBuffer) const
/**
 * Stores content of a meta object (in iData) to a descriptor
 */
    {
	if (*iData == NULL)
		{
		const TUint32 KNullInt = 0;
		aBuffer.Append((TUint8*)&KNullInt, sizeof(TUint32));
		return KErrNone;
		}
	else
		{
		return (*iData)->Store(aBuffer);
		}
    }

template<class TYPE>
void TMetaPtr<TYPE>::Copy(const TAny* aData)
/**
 * Copies content of a meta object (in aData) into another meta object (in iData).
 */
    {
	(*iData)->Copy(*((TYPE*)aData));
    }

template<class TYPE>
TInt TMetaPtr<TYPE>::Length() const
/**
 * Returns the length of the handled meta object.
 */
    {
	if (*iData == NULL)
		{
		// Add length of null word (Used to represent a NULL pointer)
		return sizeof(TUint32);
		}
	else
		{
		return (*iData)->Length();
		}
    }

} //namespace Meta


#endif //METATYPE_H

