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
//

#include <es_sock.h>
#include <bttypes.h>
#include "btsocketpanic.h"


static const TUint8 KBluetoothBaseUUID[] =
	{
	/*Replaced by short UUID: 0x00, 0x00, 0x00, 0x00*/
	0x00, 0x00, 0x10, 0x00, 0x80, 0x00,
	0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
	};

static const TUint8 KSymbianBaseUUID[] =
	{
	/*Replaced by TUid: 0x00, 0x00, 0x00, 0x00*/
	0x00, 0x00, 0x10, 0x00, 0x80, 0x00,
	0x00, 0x00, 0x1B, 0x20, 0xC7, 0xAE
	};

/**
Default constructor.
Sets the UUID to all zeros.
**/
EXPORT_C TUUID::TUUID()
	{
	iUUID.Reset();
	}

/**
Construct UUID from TUint32.
The Bluetooth base UUID will be appended to the value passed.
Use this form of constructor for building 16 or 32 bit short-form
UUIDs.
@param aLong is a Big-endian format TUint32 containing the UUID.
**/
EXPORT_C TUUID::TUUID(TUint32 aLong)
	{
	BigEndian::Put32(iUUID.Begin(), aLong);
	Mem::Copy(iUUID.Begin()+4, KBluetoothBaseUUID, sizeof(KBluetoothBaseUUID));
	}

/**
128 bit UUID constructor.
Use this constructor to build full length 128 bit UUIDs.
@param	aHH Highest order word (bits 96 - 127) in its Big-endian format
@param	aHL Second highest order word (bits 64 - 95) in its Big-endian format
@param	aLH Secong lowset order word (bits 32 - 63) in its Big-endian format
@param	aLL Low order word (bits 0 - 31) in its Big-endian format
**/
EXPORT_C TUUID::TUUID(TUint32 aHH, TUint32 aHL, TUint32 aLH, TUint32 aLL)
	{
	BigEndian::Put32(&iUUID[0], aHH);
	BigEndian::Put32(&iUUID[4], aHL);
	BigEndian::Put32(&iUUID[8], aLH);
	BigEndian::Put32(&iUUID[12],aLL);
	}

/**
Set Bluetooth UUID from an EPOC UUID.
Uses a well known reserved range. This defines a one-to-one
mapping between all EPOC UIDs and and a range of Bluetooth UUID.

Base UUID used is 00000000-0000-1000-8000-00001B20C7AE; the first four
bytes are replaced by the 32 bit Sybmian OS Uid, in big-endian form.

@param aUid	EPOC UID to set this Bluetooth UUID from
**/
EXPORT_C TUUID::TUUID(const TUid& aUid)
	{
	BigEndian::Put32(iUUID.Begin(), aUid.iUid);
	Mem::Copy(iUUID.Begin()+4, KSymbianBaseUUID, sizeof(KSymbianBaseUUID));
	}

/**
Set the UUID from a binary descriptor.
Can accept a 2, 4, or 16 byte descriptor.
Any other value will cause a leave with @c KErrArgument.
If a 2 or 4 byte descriptor is set, the rest of the UUID will be filled
with the Bluetooth well-known base UUID.

@param aDes The binary descriptor. Note that the descriptor is interpretted to be in Big-endian format,
i.e. index 0 holds the most significant byte.
@leave This method will leave if an error occurs.
**/
EXPORT_C void TUUID::SetL(const TDesC8& aDes)
	{
	TPtr8 dst(iUUID.Begin(), KSdpUUIDMaxLength);
	switch(aDes.Length())
		{
	// only 2 bytes given - so fill first two terms in iUUUD array with zeros
	// then fall through to fill next two terms with contents of aDes
	// then last 12 terms with default extension
	case 2:
		dst.FillZ(2); //fill first two bytes of 'dst' with zeros
	// if fall through to here see comment above 'case 2'
	// otherwise: 4 bytes given - so fill first four terms in iUUUD array with contents of aDes
	// then last 12 terms with default extension
	case 4:
		dst.Append(aDes);
		dst.Append(KBluetoothBaseUUID, sizeof(KBluetoothBaseUUID));
		break;
	// all 16 bytes given - copy them all to the iUUID array.
	case 16:
		dst.Copy(aDes);
		break;
	default:
		User::Leave(KErrArgument);
		}
	}

/**
Returns the full 128 bit version of the UUID.
@return The long form version of the UUID.
**/
EXPORT_C const TPtrC8 TUUID::LongForm() const
	{
	return TPtrC8(iUUID.Begin(), KSdpUUIDMaxLength);
	}

/**
Get the smallest version the UUID will compress down to.
Might be 2, 4, or 16 bytes.
If 2 or 4 bytes, the ommitted bytes are implicity those
from the Bluetooth base UUID.
@return TPtrC8 pointing to 2, 4, or 16 bytes of significant address.
**/
EXPORT_C const TPtrC8 TUUID::ShortestForm() const
	{
	TPtrC8 uuid=LongForm();
	
	TInt minSize = MinimumSize();
	
	switch (minSize)
		{
		case 16:
			// 128 bit
			return uuid;
		
		case 4:
			// 32 bit
			return uuid.Left(4);
		}
	
	// 16 bit version
	return uuid.Mid(2,2);
	}

// HACKME used to hack the size of the TUUID returned, see AttrValue
EXPORT_C const TPtrC8 TUUID::Des() const
	{
	return ShortestForm();
	}


/**
 * Return the UUID with a specified length.
 *
 * If 2 or 4 bytes, the ommitted bytes are implicity those from the Bluetooth
 * base UUID.
 *
 * @param aLength The required length (2, 4 or 16 bytes).
 * @return TPtrC8 pointing to 2, 4, or 16 bytes of significant UUID data.
 * @leave KErrArgument The UUID length requested is invalid.
 * @leave KErrTotalLossOfPrecision The UUID minimum size is less than the requested size.
 * @deprecated use TUUID::SpecifiedLengthL() that implements the right size comparison logic
 */
EXPORT_C const TPtrC8 TUUID::FixedLengthL(TInt aLength) const
	{
	TPtrC8 uuid=LongForm();
	
	TInt minSize = MinimumSize();
	if (minSize < aLength)
		User::Leave(KErrTotalLossOfPrecision);
	
	switch (aLength)
		{
		case 2:
			return uuid.Mid(2,2);
		
		case 4:
			return uuid.Left(4);

		case 16:
			// Will always be sufficient to represent this UUID.
			// Rather than returning directly, break out of switch
			// to convince compiler that function will always return a value.
			break;

		default:
			User::Leave(KErrArgument);
		}
	
	return uuid;
	}


/**
 * Returns the minimum size of this UUID.
 */
EXPORT_C TInt TUUID::MinimumSize() const
	{
	TPtrC8 uuid=LongForm();

	TInt minSize = 16;
	if (uuid.Mid(4) == TPtrC8(KBluetoothBaseUUID, sizeof(KBluetoothBaseUUID)))
		minSize = 4;
	if ((minSize == 4) && (BigEndian::Get16(uuid.Ptr()) == 0))
		minSize = 2;
	
	return minSize;
	}
	/**
Comparison operator.
@param aUUID Object to compare to this.
@return ETrue is aUUID is the same as this, EFalse if not.
*/
EXPORT_C TBool TUUID::operator==(const TUUID& aUUID) const
	{
	return Mem::Compare(iUUID.Begin(), KSdpUUIDMaxLength, aUUID.iUUID.Begin(), KSdpUUIDMaxLength)==0;
	}

/**
Inequality operator.
@param aUUID Object to compare to this.
@return EFalse is aUUID is the same as this, ETrue if not.
*/
EXPORT_C TBool TUUID::operator!=(const TUUID& aUUID) const
	{
	return !(*this==aUUID);
	}

/**
Access a single element of the UUID.
@param aIndex The index of the element to access.
@return The element.  The reference remains valid as long as this object is in scope.
*/
EXPORT_C const TUint8 &TUUID::operator[](TInt aIndex) const
	{
	return iUUID[aIndex];
	}

/**
Access a single element of the UUID.
@param aIndex The index of the element to access.
@return The element.  The reference remains valid as long as this object is in scope.
*/
EXPORT_C TUint8 &TUUID::operator[](TInt aIndex)
	{
	return iUUID[aIndex];
	}

/**
 * Return the UUID with a specified length.
 *
 * Does not allow compressing a UUID to less than its shortest form.
 * If 2 or 4 bytes, the ommitted bytes are implicity those from the Bluetooth
 * base UUID.
 *
 * @param aLength The required length (2, 4 or 16 bytes).
 * @return TPtrC8 pointing to 2, 4, or 16 bytes of significant UUID data.
 * @leave KErrArgument The UUID length requested is invalid.
 * @leave KErrTotalLossOfPrecision The UUID cannot be compressed to the requested length.
 */
EXPORT_C const TPtrC8 TUUID::SpecifiedLengthL(TInt aLength) const
	{
	TPtrC8 uuid=LongForm();
	
	TInt minSize = MinimumSize();
	if (aLength < minSize)
		User::Leave(KErrTotalLossOfPrecision);
	
	switch (aLength)
		{
		case 2:
			return uuid.Mid(2,2);
		
		case 4:
			return uuid.Left(4);

		case 16:
			// Will always be sufficient to represent this UUID.
			// Rather than returning directly, break out of switch
			// to convince compiler that function will always return a value.
			break;

		default:
			User::Leave(KErrArgument);
		}
	
	return uuid;
	}

/**
Set the UUID from a binary descriptor containing Little-endian format UUID.
Can accept a 2, 4, or 16 byte descriptor.
Any other value will cause a leave with @c KErrArgument.
If a 2 or 4 byte descriptor is set, the rest of the UUID will be filled
with the Bluetooth well-known base UUID.

@param aDes The binary descriptor. Note that the descriptor is interpretted to be in Little-endian format,
i.e. index 0 holds the most significant byte.
@leave This method will leave if an error occurs.
**/
EXPORT_C void TUUID::SetFromLittleEndianL(const TDesC8& aDes)
	{
	TBuf8<KSdpUUIDMaxLength> buf;
	TInt length = aDes.Length();
	__ASSERT_ALWAYS(length <= KSdpUUIDMaxLength, User::Leave(KErrArgument));
	for(TInt i = length - 1; i >=0 ; i--)
		{
		buf.Append(aDes[i]);
		}
	SetL(buf);
	}

// ------ RUUIDContainer --------------------------------------------------------------------------


EXPORT_C void RUUIDContainer::Close()
	{
	iUUIDs.Close();
	}

EXPORT_C void RUUIDContainer::Reset()
	{
	iUUIDs.Reset();
	iUUIDs.Compress();
	}

EXPORT_C TInt RUUIDContainer::Count() const
	{
	return iUUIDs.Count();
	}

/**
Check if a particular TUUID is present in the RUUIDContainer
@param aUuid the TUUID to be looked up in the container class
@return ETrue if it is present otherwise EFalse
*/
EXPORT_C TBool RUUIDContainer::IsPresent(const TUUID& aUuid) const
	{
	TBool ret = EFalse;
	TUint count = iUUIDs.Count();
	for (TUint i = 0; i < count; i++)
		{
		if (aUuid == iUUIDs[i])
			{
			ret = ETrue;
			break;
			}
		}
	return ret;
	}

/**
Add an TUUID into RUUIDContainer
@param aUuid the TUUID to be added into the container class
@return a system wide error code.
*/
EXPORT_C TInt RUUIDContainer::Add(const TUUID& aUuid)
	{
	if (!IsPresent(aUuid))
		{
		return iUUIDs.Append(aUuid);
		}
	else
		{
		return KErrAlreadyExists;
		}
	}

/**
Access a single UUID.
@param aIndex The index of the UUID to access.
@return The UUID.  The reference remains valid as long as this object is in scope.
*/
EXPORT_C const TUUID &RUUIDContainer::At(TInt aIndex) const
	{
	return iUUIDs[aIndex];
	}

/**
Access a single UUID.
@param aIndex The index of the UUID to access.
@return The UUID.  The reference remains valid as long as this object is in scope.
*/
EXPORT_C const TUUID &RUUIDContainer::operator[](TInt aIndex) const
	{
	return iUUIDs[aIndex];
	}

/**
Access a single UUID.
@param aIndex The index of the UUID to access.
@return The UUID.  The reference remains valid as long as this object is in scope.
*/
EXPORT_C TUUID &RUUIDContainer::operator[](TInt aIndex)
	{
	return iUUIDs[aIndex];
	}

EXPORT_C void RExtendedInquiryResponseUUIDContainer::Close()
	{
	iUUIDs.Close();
	}

/**
Retrieve the UUID container class.
@return RUUIDContainer, the UUID container class, which contains a set of UUIDs.
*/
EXPORT_C RUUIDContainer& RExtendedInquiryResponseUUIDContainer::UUIDs()
	{
	return iUUIDs;
	}

/**
Check if the Extended Inquiry Response has a complete set of UUIDs.
@param aType a specifc type of UUID. It indicates which type of UUID the check will be carried upon.
This could be 16 bit, 32 bit or 128 bit TUUID.
@return the result of the check, ETrue means it's a complete set of UUIDs, otherwise EFalse.
*/
EXPORT_C TBool RExtendedInquiryResponseUUIDContainer::GetCompleteness(TUUIDType aType) const
	{
	// Check the bit for this UUID type. If set,
	// data is partial, else data is complete
	TInt completeness = iCompleteness & aType;
	if (completeness > 0)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
Set the completeness for a set of specific type UUID (16, 32 or 128 bit UUID) in Extended Inquiry Response.
@param aType a specifc type of UUID. This could be 16 bit, 32 bit or 128 bit TUUID.
@param aIsComplete ETrue means it's a complete set of UUIDs, otherwise EFalse.
*/
EXPORT_C void RExtendedInquiryResponseUUIDContainer::SetCompleteness(TUUIDType aType, TBool aIsComplete)
	{
	TInt setcomp = 0;
	if (aIsComplete == EFalse)
		{
		setcomp = aType;
		}
	// Set or clear the bit for this UUID type.
	// If set, data is partial, else data is complete
	iCompleteness &= ~(aType);
	iCompleteness ^= setcomp;
	}

//EOF

