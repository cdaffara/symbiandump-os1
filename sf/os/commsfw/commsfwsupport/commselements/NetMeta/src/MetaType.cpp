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

#include <elements/metatypearray.h>
#include <elements/metatypevariablelen.h>
#include <elements/metadata.h>

using namespace Meta;

enum TMetaPanic
	{
	EUnsupportedMethodCall = 0
	};   

EXPORT_C TInt TMetaVarLenBase::Load(TPtrC8& aBuffer)
/**
 * Loads content of descriptor data member
 */
	{
	// Check if the descriptor is long enough
	TInt errlen = CheckBuf( aBuffer );
	if (errlen < 0)
		{
		return errlen;
		}

	//and copy whatever we can into the client descriptor
	CopyBuf(aBuffer,errlen);

	aBuffer.Set(aBuffer.Ptr() + errlen, aBuffer.Length() - errlen); //update pointer
	return KErrNone;
	}

EXPORT_C TInt TMetaVarLenBase::Store(TDes8& aBuffer) const
/**
 * Stores content of a descriptor data member
 */
	{
	// Ask the descriptor how much data does it currently store
	TUint32 size = Length() - sizeof(TUint32);

   	// The length variable and the data must fit
	if ((TUint32)(aBuffer.MaxLength() - aBuffer.Length()) < (size + sizeof(TUint32)))
		{
		return KErrOverflow;
		}

    // Store
	aBuffer.Append((TUint8*)&size, sizeof(TUint32));
	AppendToBuf( aBuffer );
	return KErrNone;
	}

EXPORT_C void TMetaVarLenBase::Copy(const TAny* aData)
	{
	(void)aData;
	User::Panic(_L("TMeta"), EUnsupportedMethodCall);	
	}

EXPORT_C TInt TMetaVarLenBase::Length() const
	{
	User::Panic(_L("TMeta"), EUnsupportedMethodCall);
	return 0;
	}

TInt TMetaVarLenBase::CheckBuf( TPtrC8& aBuffer )
	{
	// Check if the descriptor is long enough
	if (aBuffer.Length() < (TInt)sizeof(TUint32))
		{
		return KErrArgument;
		}
	
	// Check if the descriptor is long enough
    TBuf8<sizeof(TUint32)> lenSubBuf;
    lenSubBuf.Copy(aBuffer.Ptr(), sizeof(TUint32));
    TUint length = *((TUint32*)lenSubBuf.Ptr());	
	if (aBuffer.Length() < (TInt)length)
		{
		return KErrArgument;
		}

	aBuffer.Set(aBuffer.Ptr() + sizeof(TUint32), aBuffer.Length() - sizeof(TUint32)); //update pointer
	return length;
	}

EXPORT_C TInt TMetaArrayBase::Load(TPtrC8& aBuffer)
/**
 * Loads content of a meta object (in iData) from a descriptor
 */
	{
	Reset();
	if (aBuffer.Length() < (TInt)sizeof(TInt))
		{
		return KErrArgument;
		}
	TInt count = (*(TInt*)aBuffer.Ptr());
	TInt size = SizeOfType();
	aBuffer.Set(aBuffer.Ptr()+sizeof(TInt), aBuffer.Length()-sizeof(TInt)); //update pointer
	TInt error = KErrNone;
	for ( TInt n = 0; error == KErrNone && n < count; n++ )
		{
		error = Append( (TUint8*)aBuffer.Ptr() );
		aBuffer.Set(aBuffer.Ptr()+size, aBuffer.Length()-size); //update pointer
		}
	return error;
	}

EXPORT_C TInt TMetaArrayBase::Store(TDes8& aBuffer) const
/**
 * Stores content of a meta object (in iData) to a descriptor
 */
	{
	if (aBuffer.MaxLength() - aBuffer.Length() < (TInt)sizeof(TInt))
		{
		return KErrOverflow;
		}
	TInt count = Count();
	aBuffer.Append((TUint8*)&count, sizeof(TInt));
	TInt size = SizeOfType();
	for ( TInt n = 0; n < count; n++ )
		{
		if (aBuffer.MaxLength() - aBuffer.Length() < size)
			{
			return KErrOverflow;
			}
		aBuffer.Append(At(n), size);
		}
	return KErrNone;
	}




