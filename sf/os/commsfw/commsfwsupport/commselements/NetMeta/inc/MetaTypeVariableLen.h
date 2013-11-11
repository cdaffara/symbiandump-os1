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

#if (!defined METATYPEVARIABLELEN_H)
#define METATYPEVARIABLELEN_H

#include <elements/metatype.h>

namespace Meta
{

template<class TYPE> class TMetaVarLen8;
template<class TYPE> class TMetaVarLen16;
typedef TMetaVarLen8<TDes8> TMetaDes8;
typedef TMetaVarLen16<TDes16> TMetaDes16;

#if defined(_UNICODE)
typedef TMetaDes16 TMetaDes;
#else
typedef TMetaDes8 TMetaDes;
#endif

class TMetaVarLenBase : public MMetaType
	{
	public:
		IMPORT_C virtual TInt Load(TPtrC8& aBuffer);
		IMPORT_C virtual TInt Store(TDes8& aBuffer) const;
		IMPORT_C virtual void Copy(const TAny* aData);
		IMPORT_C virtual TInt Length() const;
		
	protected:
        TInt CheckBuf(TPtrC8& aBuffer);
        
		virtual void CopyBuf(TPtrC8& aBuffer,TInt aLen) = 0;
		virtual void AppendToBuf( TDes8& aBuffer ) const = 0;
	};
	
template<class TYPE>
class TMetaVarLen : public TMetaVarLenBase
/**

Implementation of MMetaType for anything that provides Length(), SetLength(), MaxLength(),
Copy() and Left() functions with arbitrarry element size

@internalComponent
@released since v9.0 */
	{
public:
	virtual void Copy(const TAny* aData);

protected:
	TMetaVarLen(const TAny* aData);

protected:
	TYPE* iData;
	};

template<class TYPE>
inline TMetaVarLen<TYPE>::TMetaVarLen(const TAny* aData)
:    iData((TYPE*)aData)
/**
 * Constructor
 */
    {
    __ASSERT_DEBUG(iData!=NULL,User::Panic(_L("TMetaVarLen"),KErrArgument));
    }

template<class TYPE>
void TMetaVarLen<TYPE>::Copy(const TAny* aData)
/**
 * Copies content of the member
 */
	{
	const TYPE& var = *((TYPE*)aData);
	iData->Copy(var.Left( iData->MaxLength() ));
	}

template<class TYPE>
class TMetaVarLen8 : public TMetaVarLen<TYPE>
/**

Implementation of MMetaType for anything that provides Length(), SetLength(), MaxLength(),
Copy() and Left() functions with element size 1 byte
Like for example TDes8, TBuf8, TPtr8.

@internalComponent
@released since v9.0 */
	{
public:
	inline static MMetaType* NewL(const TAny* aMem, const TAny* aData);

	virtual TInt Length() const;

protected:
	TMetaVarLen8(const TAny* aData);
	virtual void AppendToBuf( TDes8& aBuffer ) const;
	virtual void CopyBuf(TPtrC8& aBuffer,TInt aLen);

	};

template<class TYPE>
class TMetaVarLen16 : public TMetaVarLen<TYPE>
/**

Implementation of MMetaType for anything that provides Length(), SetLength(), MaxLength(),
Copy() and Left() functions with element size 2 bytes
Like for example TDes16, TBuf16, TPtr16 (ELEMENT is short).

@internalComponent
@released since v9.0 */
	{
public:
	inline static MMetaType* NewL(const TAny* aMem, const TAny* aData);

	virtual TInt Length() const;

protected:
	TMetaVarLen16(const TAny* aData);
	virtual void AppendToBuf( TDes8& aBuffer ) const;
	virtual void CopyBuf(TPtrC8& aBuffer,TInt aLen);

	};


template<class TYPE>
inline TMetaVarLen16<TYPE>::TMetaVarLen16(const TAny* aData)
:    TMetaVarLen<TYPE>(aData)
/**
 * Constructor
 */
    {
    }

template<class TYPE>
MMetaType* TMetaVarLen16<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type of a particular type.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaVarLen16<TYPE>(aData);
    }

template<class TYPE>
void TMetaVarLen16<TYPE>::AppendToBuf( TDes8& aBuffer ) const
	{
	//copy whatever fits in (no loosing high byte as the ::Copy normally would)
	//a litle bit dirty solution assuming a flat memory model and byte addressing mode
	aBuffer.Append(reinterpret_cast<const TUint8*>(this->iData->Ptr()), this->iData->Length() * 2);
	}

template<class TYPE>
void TMetaVarLen16<TYPE>::CopyBuf(TPtrC8& aBuffer,TInt aLen)
	{
	//copy whatever fits in
	//a litle bit dirty solution assuming a flat memory model and byte addressing mode
	this->iData->Copy( reinterpret_cast<const TUint16*>(aBuffer.Ptr()), Min( this->iData->MaxLength(), aLen / 2 ));
	}

template<class TYPE>
TInt TMetaVarLen16<TYPE>::Length() const
/**
 * Returns the length of the data member
 */
	{
	return this->iData->Length() * 2 + sizeof(TUint32);
	}

template<class TYPE>
inline TMetaVarLen8<TYPE>::TMetaVarLen8(const TAny* aData)
:    TMetaVarLen<TYPE>(aData)
/**
 * Constructor
 */
    {
    }

template<class TYPE>
MMetaType* TMetaVarLen8<TYPE>::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type of a particular type.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaVarLen8<TYPE>(aData);
    }

template<class TYPE>
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
void TMetaVarLen8<TYPE>::CopyBuf(TPtrC8& aBuffer,TInt aLen)
	{
	//copy whatever fits in
	this->iData->Copy( aBuffer.Left( Min(this->iData->MaxLength(), aLen) ) );
	}
#else
void TMetaVarLen8<TYPE>::CopyBuf(TPtrC8& aBuffer,TInt /*aLen*/)
	{
	this->iData->Copy( aBuffer.Left( this->iData->MaxLength() ) );
	}
#endif


template<class TYPE>
void TMetaVarLen8<TYPE>::AppendToBuf( TDes8& aBuffer ) const
	{
	aBuffer.Append(*this->iData);
	}


template<class TYPE>
TInt TMetaVarLen8<TYPE>::Length() const
/**
 * Returns the length of the data member
 */
	{
	TInt len = this->iData->Length() + sizeof(TUint32);
	if (len & 1) 
		{
		len++;
		}
	return len;
	}




} //namespace Meta


#endif
//METATYPEVARIABLELEN_H

