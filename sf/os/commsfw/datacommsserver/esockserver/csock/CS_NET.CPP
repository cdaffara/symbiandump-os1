// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CS_STD.H"

//
// Byte Order
// Might make this lot inline...
//

#pragma warning( disable : 4244 ) // possible loss of data warning


EXPORT_C TUint32 ByteOrder::Swap32(TUint32 aVal)
/** Reverses the order of the bytes in a 32-bit value.

@param aVal The value to reverse. 
@return The reversed value. */
	{
	return ((aVal>>24)&0xff) | ((aVal>>8)&0xff00) | ((aVal<<8)&0xff0000) | ((aVal<<24)&0xff000000);
	}

EXPORT_C TUint16 ByteOrder::Swap16(TUint16 aVal)
/** Reverses the order of the bytes in a 16-bit value.

@param aVal The value to reverse. 
@return The reversed value. */
	{
	return ((aVal>>8) & 0xff) | ((aVal<<8) & 0xff00);
	}

EXPORT_C TUint32 BigEndian::Get32(const TUint8 *aPtr)
/** Gets a 32-bit integer value which is in big-endian format from a byte stream.

@param aPtr A pointer to a byte stream. 
@return A 32-bit long integer value in native machine format. */
	{
	return (aPtr[0]<<24) | (aPtr[1]<<16) | (aPtr[2]<<8) | aPtr[3];
	}

EXPORT_C void BigEndian::Put32(TUint8 *aPtr, TUint32 aVal)
/** Inserts a 32-bit value into a byte stream in big-endian format.

@param aPtr A pointer to a byte stream. 
@param aVal A 32-bit long integer value in native machine format. */
	{
	aPtr[0] = aVal >> 24;
	aPtr[1] = (aVal >> 16) & 0xff;
	aPtr[2] = (aVal >> 8) & 0xff;
	aPtr[3] = aVal & 0xff;
	}

EXPORT_C TUint16 BigEndian::Get16(const TUint8 *aPtr)
/** Gets a 16-bit value integer which is in big-endian format from a byte stream.

@param aPtr A pointer to a byte stream. 
@return A 16-bit long integer value in native machine format. */
	{
	return (aPtr[0]<<8) | aPtr[1];
	}

EXPORT_C void BigEndian::Put16(TUint8 *aPtr, TUint16 aVal)
/** Inserts a 16-bit value into a byte stream in big-endian format.

@param aPtr A pointer to a byte stream. 
@param aVal A 16-bit long integer value in native machine format. */
	{
	aPtr[0] = aVal >> 8;
	aPtr[1] = aVal & 0xff;
	}

EXPORT_C TUint32 LittleEndian::Get32(const TUint8 *aPtr)
/** Gets a 32-bit integer value which is in little-endian format from a byte stream.

@param aPtr A pointer to a byte stream. 
@return A 32-bit long integer value in native machine format. */
	{
	return (aPtr[3]<<24) | (aPtr[2]<<16) | (aPtr[1]<<8) | aPtr[0];
	}

EXPORT_C void LittleEndian::Put32(TUint8 *aPtr, TUint32 aVal)
/** Inserts a 32-bit value into a byte stream in little-endian format.

@param aPtr A pointer to a byte stream 
@param aVal A 32-bit long integer value in native machine format. */
	{
	aPtr[3] = aVal >> 24;
	aPtr[2] = (aVal >> 16) & 0xff;
	aPtr[1] = (aVal >> 8) & 0xff;
	aPtr[0] = aVal & 0xff;
	}

EXPORT_C TUint16 LittleEndian::Get16(const TUint8 *aPtr)
/** Gets a 16-bit value integer which is in little-endian format from a byte stream.

@param aPtr A pointer to a byte stream. 
@return A 16-bit long in native machine format. */
	{
	return (aPtr[1]<<8) | aPtr[0];
	}

EXPORT_C void LittleEndian::Put16(TUint8 *aPtr, TUint16 aVal)
/** Inserts a 16-bit value into a byte stream in little-endian format.

@param aPtr A pointer to a byte stream.
@param aVal A 32-bit long integer value in native machine format. */
	{
	aPtr[0] = aVal & 0xff;
	aPtr[1] = aVal >> 8;
	}

#pragma warning( default : 4244 ) // possible loss of data warning


//
// Socket Address
//

EXPORT_C TSockAddr::TSockAddr()
	: TBuf8<KMaxSockAddrSize>()
/** Default constructor initialises the address to be zero-filled. */
	{
	SetLength(KMaxSockAddrSize);
	SetFamily(KAFUnspec);
	}

EXPORT_C TSockAddr::TSockAddr(TUint aFamily)
/**
Constructs the address with the family member data specified.

@released.
@param aFamily, Address family of address object.
*/
	: TBuf8<KMaxSockAddrSize>()
/** Constructs the address with the family member data specified.

@param aFamily Address family Id. */
	{
	SetLength(KMaxSockAddrSize);
	SetFamily(aFamily);
	}

EXPORT_C TUint TSockAddr::Family() const
/** Gets the address family and thereby the type of address. The family may be 
used to effect 'down-casting'.

@return Address family Id. */
	{
	return BasePtr()->iFamily;
	}

EXPORT_C void TSockAddr::SetFamily(TUint aFamily)
/** Sets the address family of an address.

@param aFamily Address family Id. */
	{
	BasePtr()->iFamily = aFamily;
	}

EXPORT_C void TSockAddr::SetPort(TUint aPort)
//
// Set Generic Port
//
/** Sets the address port of an address.

@param aPort New port value for address */
	{
	BasePtr()->iPort = aPort;
	}

EXPORT_C TUint TSockAddr::Port() const
//
// Get Generic Port
//
/** Gets the port value.

@return Port value */
	{
	return BasePtr()->iPort;
	}

EXPORT_C TBool TSockAddr::CmpPort(const TSockAddr &anAddr) const
/**
Compare two ports.

Compares ports in two addresses

@param anAddr, socket address.
@return ETrue if the ports match and EFalse if they do not 
*/
	{
	return (BasePtr()->iFamily==anAddr.BasePtr()->iFamily) 
		&& (BasePtr()->iPort==anAddr.BasePtr()->iPort);
	}

EXPORT_C void TSockAddr::SetUserLen(TInt aLen)
//
// Set descriptor length to actual length of
// subclass.
//
/** Sets the length of data it appends after the base class' data. This function 
is mainly intended for use in derived classes.

@param aLen New length for appended data. */
	{
	SetLength(aLen+Align4(sizeof(SSockAddr)));
	}

EXPORT_C TInt TSockAddr::GetUserLen()
//
// Get length of user part of TSockAddr
//
/** Gets the number of bytes a derived class has set beyond the SSockAddr part 
of the address. This function is mainly intended for use in derived classes.

@return User length */
	{
	return Length()-Align4(sizeof(SSockAddr));
	}
