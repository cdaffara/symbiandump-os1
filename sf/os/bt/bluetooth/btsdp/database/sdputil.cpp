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
#include <btsdp.h>
#include "sdputil.h"

void DbPanic(TSdpDbPanic aCode)
	{
	User::Panic(KSdpDatabasePanicString, aCode);
	}

void ParsePanic(TSdpParsePanic aCode)
	{
	User::Panic(KSdpParsePanicString, aCode);
	}

EXPORT_C TUint SdpUtil::GetUint(const TDesC8& aData)
/** Gets an unsigned integer from a descriptor.

@param aData The descriptor containing the integer
@return The unsigned integer contained in the descriptor
*/
	{
	switch(aData.Length())
		{
	case 0:
		return 0;
	case 1:
		return aData[0];
	case 2:
		return BigEndian::Get16(aData.Ptr());
	case 4:
		return BigEndian::Get32(aData.Ptr());
	default:
		ParsePanic(EGetUintBadDescriptorLength);
		return 0;
		}
	}

EXPORT_C void SdpUtil::PutUint(TUint8* aPtr, TInt64 aInt, TInt aNumberOfBytes)
/** Puts an unsigned integer into a descriptor.

@param aPtr Pointer to the descriptor's data field
@param aInt Integer to be copied into descriptor
@param aNumberOfBytes Number of bytes occupied by integer
*/
	{
	for(TInt i = aNumberOfBytes - 1; i >= 0; --i)
		{
		aPtr[i] = (static_cast<TUint8>(I64LOW(aInt)));
		aInt >>= 8;
		} 
	}

/*
Puts a TUint64 into a descriptor

@param aPtr Pointer to the descriptor's data field
@param aNumber The unsigned 64 bit integer to be copied into the descriptor
*/
EXPORT_C void SdpUtil::PutUint64(TUint8* aPtr, const TUint64& aNumber)
	{
	TUint8 temp;
	TUint64 number = aNumber;
	
	for (TInt i = sizeof(TUint64) -1; i >= 0; i--)
		{
		temp = number & 0xFF; 	// transfer the lowest byte..
		aPtr[i] = temp;			//.. into the right hand side of the descriptor
		number >>= 8;
		} 
	}
	

/*
Gets a TUint64 from a descriptor

@param aData The descriptor containing the integer
@param aNumber The unsigned 64 bit integer contained in the descriptor
*/
EXPORT_C void SdpUtil::GetUint64(const TDesC8& aData, TUint64& aNumber)
	{
	if (aData.Length() != 8) //i.e. 64/8
		{
		ParsePanic(EGetUint64BadDescriptorLength);
		return;
		}
		
	const TUint8* dataPtr = aData.Ptr();
	
	aNumber = 	(static_cast<TUint64>(dataPtr[0])<<56) | (static_cast<TUint64>(dataPtr[1])<<48) | 
				(static_cast<TUint64>(dataPtr[2])<<40) | (static_cast<TUint64>(dataPtr[3])<<32) |
				(static_cast<TUint64>(dataPtr[4])<<24) | (static_cast<TUint64>(dataPtr[5])<<16) |
				(static_cast<TUint64>(dataPtr[6])<<8) | (static_cast<TUint64>(dataPtr[7]) );
				
	}


EXPORT_C void SdpUtil::PutUint128(TUint8* aPtr, const TUint64& aLo, const TUint64& aHi)
/**
Puts two unsigned 64 bit integers into a descriptor.

@param aPtr Pointer to the descriptor's data field
@param aLo The 64 lower bits of a 128 bit integer to be copied into the descriptor
@param aHi  The 64 higher bits of a 128 bit integer to be copied into the descriptor
*/
	{
	TUint8 tempLow;
	TUint8 tempHigh;	
	TUint64 lowNumber = aLo;
	TUint64 highNumber = aHi;
	TInt offset = sizeof(TUint64);
	
	for(TInt i = offset -1; i >= 0; i--)
		{
		tempLow = lowNumber & 0xFF; // transfer the lowest byte..
		aPtr[i+offset] = tempLow;	//.. into the right hand side of the descriptor
		lowNumber >>= 8;
		
		tempHigh = highNumber & 0xFF; 
		aPtr[i] = tempHigh;
		highNumber >>= 8;
		} 
	
	}

	
/*
Gets two TUint64s, one being the high 64 bits and one being the low 64 bits of a 128 bit integer 

@param aData The descriptor containing the integer
@param aLo The unsigned 64 bit integer contains the lower bits of the 128 bit integer contained in the descriptor
@param aHi  The unsigned 64 bit integer contains the higher bits of the 128 bit integer contained in the descriptor
*/
EXPORT_C void SdpUtil::GetUint128(const TDesC8& aData, TUint64& aLo, TUint64& aHi)
	{
	if (aData.Length() != 16) //i.e. 128/8
		{
		ParsePanic(EGetUint128BadDescriptorLength);
		}
		
	const TUint8* dataPtr = aData.Ptr();

	// left hand side of the descriptor transfers to the high TUint64
	aHi = 	(static_cast<TUint64>(dataPtr[0])<<56) | (static_cast<TUint64>(dataPtr[1])<<48) | 
			(static_cast<TUint64>(dataPtr[2])<<40) | (static_cast<TUint64>(dataPtr[3])<<32) |
			(static_cast<TUint64>(dataPtr[4])<<24) | (static_cast<TUint64>(dataPtr[5])<<16) |
			(static_cast<TUint64>(dataPtr[6])<<8) | (static_cast<TUint64>(dataPtr[7]) );
				
	// right hand side of the descriptor transfers to the low TUint64
	aLo = 	(static_cast<TUint64>(dataPtr[8])<<56) | (static_cast<TUint64>(dataPtr[9])<<48) | 
			(static_cast<TUint64>(dataPtr[10])<<40) | (static_cast<TUint64>(dataPtr[11])<<32) |
			(static_cast<TUint64>(dataPtr[12])<<24) | (static_cast<TUint64>(dataPtr[13])<<16) |
			(static_cast<TUint64>(dataPtr[14])<<8) | (static_cast<TUint64>(dataPtr[15]) );

	}
