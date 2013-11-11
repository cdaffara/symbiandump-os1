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
 @internalComponent
*/

#include "bttypes.h" 

enum TBTDevAddrPanics
	{
	EBTDevAddrBadDescriptor = 0,
	EBTDevAddrBadTInt64 = 1,
	};

void Panic(TBTDevAddrPanics aCode)
	{
	_LIT(KPanicName, "BTDevAddr");
	User::Panic(KPanicName, aCode);
	}
	
/** 
Default constructor.
The address is initialised to 0. 
*/
EXPORT_C TBTDevAddr::TBTDevAddr()
	{
	Reset();
	}

/** 
Constructs a device address from a data buffer.

The buffer is copied directly into the object. The function panics if aDes 
does not have a length of 6 bytes (48 bits).

@param aDes Data buffer for device address 
*/
EXPORT_C TBTDevAddr::TBTDevAddr(const TDesC8 &aDes)
	{
	__ASSERT_ALWAYS(aDes.Length() == KBTDevAddrSize, Panic(EBTDevAddrBadDescriptor));
	Mem::Copy(&iAddr[0], aDes.Ptr(), KBTDevAddrSize);
	}

/** 
Constructs a device address from a TInt64.

The function panics if the most significant 16 bits of aInt are non-zero, 
as device addresses are 48 bits in size. 

@param aInt Value for device address. 
*/
EXPORT_C TBTDevAddr::TBTDevAddr(const TInt64 &aInt)
	{
	__ASSERT_ALWAYS( (I64HIGH(aInt) & 0xffff0000u) == 0, Panic(EBTDevAddrBadTInt64));
	TInt64 int64 (aInt);
	for (TInt i = KBTDevAddrSize-1; i>=0; --i)
		{
		iAddr[i] = TUint8(I64LOW(int64));
		int64 >>= 8;
		}
	}

/** 
Convert Readable string into a dev addr.
	
Address may have leading space and "0x" characters.
@param	aSource	Readable Hex representation of address
@return EPOC error code on fail, or else number of characters used from aSource in conversion
**/
EXPORT_C TInt TBTDevAddr::SetReadable(const TDesC& aSource)
	{
	TLex lex(aSource);

	lex.SkipSpaceAndMark();

	if (lex.Get() != '0' || lex.Get() != 'x')
		lex.UnGetToMark();

	TInt64 addr;

	TInt ret = lex.Val(addr, EHex);
	if (ret)
		return ret;
	if (addr > MAKE_TINT64(0x0000ffffu,0xffffffffu))
		return KErrOverflow;

	*this=TBTDevAddr(addr);
	return lex.Offset();
	}

/** 
Extract the Bluetooth device address into a human-readable format, in aDest.

Output is in hexadecimal bigendian format, with optional padding characters.

Note: aDest must be large enough to hold the entire output, or else a
a panic will occur.

@param aDest Descriptor where result is placed.
@param aPrepend Added onto front of output.
@param aByteSeperator Inserted between each byte of address.
@param aAppend Added onto end of output.
*/
EXPORT_C void TBTDevAddr::GetReadable(TDes& aDest, const TDesC& aPrepend, const TDesC& aByteSeperator, const TDesC& aAppend) const
	{
	__ASSERT_ALWAYS(aDest.MaxLength() >= aPrepend.Length() + (KBTDevAddrSize * (2 + aByteSeperator.Length())) - aByteSeperator.Length() + aAppend.Length(), Panic(EBTDevAddrBadDescriptor));
	_LIT(KReadableFormat, "%S%02x%S%02x%S%02x%S%02x%S%02x%S%02x%S");
	aDest.Format(KReadableFormat, &aPrepend, 
				 iAddr[0], &aByteSeperator, 
				 iAddr[1], &aByteSeperator, 
				 iAddr[2], &aByteSeperator, 
				 iAddr[3], &aByteSeperator, 
				 iAddr[4], &aByteSeperator, 
				 iAddr[5], &aAppend);
	}

/**
Comparison operator.
@param aAddr The device address to compare to this object.
@return ETrue if the addresses are equal, EFalse if not.
*/
EXPORT_C TBool TBTDevAddr::operator==(const TBTDevAddr& aAddr) const
	{
	return Mem::Compare(&iAddr[0],KBTDevAddrSize,&aAddr.iAddr[0],KBTDevAddrSize) == 0;
	}

/**
Inequality operator.
@param aAddr The device address to compare to this object.
@return EFalse if the addresses are equal, ETrue if not.
*/
EXPORT_C TBool TBTDevAddr::operator!=(const TBTDevAddr& aAddr) const
	{
	return !(*this==aAddr);
	}

/**
Access a single element of the address.
@param aIndex The index of the element to access.
@return The element.  The reference remains valid as long as this object is in scope.
*/
EXPORT_C const TUint8 &TBTDevAddr::operator[](TInt aIndex) const
	{
	return iAddr[aIndex];
	}

/**
Access a single element of the address.
@param aIndex The index of the element to access.
@return The element.  The reference remains valid as long as this object is in scope.
*/
EXPORT_C TUint8 &TBTDevAddr::operator[](TInt aIndex)
	{
	return iAddr[aIndex];
	}

/**
Reset the data contained within the object.
*/
EXPORT_C void TBTDevAddr::Reset()
	{
	iAddr.Reset();
	}

/**
Access value as a bigendian descriptor.
@return A non-const descriptor.
*/
EXPORT_C TPtr8 TBTDevAddr::Des()
	{
	return TPtr8(&iAddr[0], KBTDevAddrSize, KBTDevAddrSize);
	}

/**
Access value as a bigendian descriptor.
@return A const descriptor.
*/
EXPORT_C const TPtrC8 TBTDevAddr::Des() const
	{
	return TPtrC8(&iAddr[0], KBTDevAddrSize);
	}


/** 
Extract the Bluetooth device address into a human-readable format, in aDest.

Output is in hexadecimal bigendian format, with no padding characters.

Note: aDest must be large enough to hold the entire output, or else a
a panic will occur.

@param aDest Descriptor where result is placed.
**/
EXPORT_C void TBTDevAddr::GetReadable(TDes& aDest) const
	{
	GetReadable(aDest, KNullDesC, KNullDesC, KNullDesC);
	}

/**
Less than operator.
@param aAddr The device address to compare to this object.
@return EFalse if the aAddr is greater than this TBTDevAddr, ETrue if not.
*/
EXPORT_C TBool TBTDevAddr::operator<=(const TBTDevAddr& aAddr) const
	{
	for (TUint index = 0; index < KBTDevAddrSize; index++)
		{
		if (iAddr[index] < aAddr.iAddr[index])
			{
			return ETrue;
			}
		if (iAddr[index] > aAddr.iAddr[index])
			{
			return EFalse;
			}
		}
	// addresses are the same
	return ETrue;
	}

// EOF
