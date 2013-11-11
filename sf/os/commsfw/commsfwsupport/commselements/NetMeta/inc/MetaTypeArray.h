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

#if (!defined METATYPEARRAY_H)
#define METATYPEARRAY_H

#include <e32base.h>
#include <e32std.h>
#include <elements/metatype.h>

namespace Meta
{

class TMetaArrayBase : public MMetaType
/**

Implementation of MMetaType for RArray. Assumes that the doing a bite copy on the array
template type is ok.

@internalComponent
@released since v9.0 */
	{
public:
	IMPORT_C virtual TInt Load(TPtrC8& aBuffer);
	IMPORT_C virtual TInt Store(TDes8& aBuffer) const;

protected:
	TMetaArrayBase(){};

	virtual TUint8* At( TInt aIndex ) const = 0;
	virtual TInt Count() const = 0;
	virtual TInt Append( TUint8* aData ) = 0;
	virtual TInt SizeOfType() const = 0;
	virtual void Reset() = 0;
	};

template<class TYPE>
class TMetaArray : public TMetaArrayBase
/**

Implementation of MMetaType for RArray. Assumes that the doing a bite copy on the array
template type is ok.

@internalComponent
@released since v9.0 */
   {
public:
	inline static MMetaType* NewL(const TAny* aMem, const TAny* aData);

public:
	virtual void Copy(const TAny* aData);
	virtual TInt Length() const;

protected:
	TMetaArray(const TAny* aData);

	virtual TUint8* At( TInt aIndex ) const;
	virtual TInt Count() const;
	virtual TInt Append( TUint8* aData );
	virtual TInt SizeOfType() const;
	virtual void Reset();

private:
	RArray<TYPE>& iData;
   };

template<class TYPE>
MMetaType* TMetaArray<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type of a particular type.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaArray<TYPE>(aData);
    }

template<class TYPE>
inline TMetaArray<TYPE>::TMetaArray(const TAny* aData)
:    iData(*((RArray<TYPE>*)aData))
/**
 * Constructor
 */
    {
    }

template<class TYPE>
TUint8* TMetaArray<TYPE>::At( TInt aIndex ) const
	{
	return (TUint8*)&iData[aIndex];
	}

template<class TYPE>
TInt TMetaArray<TYPE>::Count() const
	{
	return iData.Count();
	}

template<class TYPE>
TInt TMetaArray<TYPE>::Append( TUint8* aData )
	{
	return iData.Append( *((TYPE*)aData) );
	}

template<class TYPE>
TInt TMetaArray<TYPE>::SizeOfType() const
	{
	return sizeof(TYPE);
	}

template<class TYPE>
void TMetaArray<TYPE>::Reset()
	{
	iData.Reset();
	}

template<class TYPE>
void TMetaArray<TYPE>::Copy(const TAny* aData)
/**
 * Copies content of a meta object (in aData) into another meta object (in iData).
 * This is a MMetaType implementation for simple (T) types so it just copies the memory.
 */
	{
	RArray<TYPE>& array = (*(RArray<TYPE>*)aData);
	iData.Reset();
	for ( TInt n = 0; n < array.Count(); n++ )
		{
		iData.Append(array[n]);
		}
	}

template<class TYPE>
TInt TMetaArray<TYPE>::Length() const
/**
 * Returns the length of the handled meta object.
 */
    {
    return sizeof(TInt) + (iData.Count() * sizeof(TYPE));
    }

} //namespace Meta

#endif //METATYPEARRAY_H

