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

#include <elements/metabuffer.h>
#include <elements/metadata.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNetMetaMtBfr, "ElemNetMetaMtBfr");
#endif

using namespace Meta;

TMetaBuf8::TMetaBuf8(const TAny* aData)
	: TMetaVarLen8<RBuf8>(aData)
	{
	}

EXPORT_C MMetaType* TMetaBuf8::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type for RBuf8.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaBuf8(aData);
    }

EXPORT_C TInt TMetaBuf8::Load(TPtrC8& aBuffer)
	{
	TInt errlen = CheckBuf( aBuffer );
	if (errlen < 0)
		{
		return errlen;
		}
	if ( iData->MaxLength() < errlen )
		{
		iData->Close();
		TInt err = iData->Create(errlen);
		if ( err != KErrNone )
			return err;
		}
	//and copy whatever we can into the client descriptor
	CopyBuf(aBuffer,errlen);

	TInt padByte = errlen % 2;
	aBuffer.Set(aBuffer.Ptr() + errlen + padByte, aBuffer.Length() - errlen - padByte); //update pointer
	return KErrNone;
	}

EXPORT_C TInt TMetaBuf8::Store(TDes8& aBuffer) const
	{
	// Ask the descriptor how much data does it currently store
	TUint32 size = this->iData->Length();

   	// The length variable and the data must fit
	if ((TUint32)(aBuffer.MaxLength() - aBuffer.Length()) < (size + sizeof(TUint32)))
		{
		return KErrOverflow;
		}

    // Store
	aBuffer.Append((TUint8*)&size, sizeof(TUint32));
	AppendToBuf(aBuffer);
	if (size & 1)
		{
		// Pad odd lengths to an even number to avoid potential
		// alignment issues on target
		TUint8 pad(0);
		aBuffer.Append(&pad, 1);
		}
	
	return KErrNone;
	}

EXPORT_C void TMetaBuf8::Copy(const TAny* aData)
	{
	const RBuf8& var = *((RBuf8*)aData);
	if ( iData->MaxLength() < var.Length() )
		{
		iData->Close();
		TInt err = iData->Create(var.Length());
		if ( err != KErrNone )
			return;
		}
	TMetaVarLen8<RBuf8>::Copy(aData);
	}

EXPORT_C TInt TMetaBuf8::Length() const
	{
	return TMetaVarLen8<RBuf8>::Length();
	}

TMetaBuf16::TMetaBuf16(const TAny* aData)
	: TMetaVarLen16<RBuf16>(aData)
	{
	}	

EXPORT_C MMetaType* TMetaBuf16::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a meta type for RBuf8.
 * Used for attribure registration (in the data v-table).
 */
    {
    return ::new ((TUint8*)aMem) TMetaBuf16(aData);
    }	

EXPORT_C TInt TMetaBuf16::Load(TPtrC8& aBuffer)
	{
	TInt errlen = CheckBuf( aBuffer );
	if (errlen < 0)
		{
		return errlen;
		}
	
	// errlen is in bytes we want unicode chars
	__ASSERT_DEBUG((errlen & 1) == 0, User::Panic(KSpecAssert_ElemNetMetaMtBfr, 1));
	if ( iData->MaxLength() < errlen / 2)
		{
		iData->Close();
		TInt err = iData->Create(errlen / 2);
		if ( err != KErrNone )
			return err;
		}
	//and copy whatever we can into the client descriptor
	CopyBuf(aBuffer,errlen);

	aBuffer.Set(aBuffer.Ptr() + errlen, aBuffer.Length() - errlen); //update pointer
	return KErrNone;
	}

EXPORT_C TInt TMetaBuf16::Store(TDes8& aBuffer) const
	{
	return TMetaVarLen16<RBuf16>::Store(aBuffer);
	}

EXPORT_C void TMetaBuf16::Copy(const TAny* aData)
	{
	const RBuf16& var = *((RBuf16*)aData);
	if ( iData->MaxLength() < var.Length() )
		{
		iData->Close();
		TInt err = iData->Create(var.Length());
		if ( err != KErrNone )
			return;
		}
	TMetaVarLen16<RBuf16>::Copy(aData);
	}

EXPORT_C TInt TMetaBuf16::Length() const
	{
	return TMetaVarLen16<RBuf16>::Length();
	}



