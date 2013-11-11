// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <bluetoothav.h>

#include "avctppacketstatics.h" 

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVLIB);
#endif

#ifdef _DEBUG
PANICCATEGORY("pktstats");
#endif

namespace SymbianAvctp
	{
enum TAvctpLibPanic
	{
	/** Unknown Packet Type */
	EAvctpLibUnknownPacketType = 0,
	/** Incorrect Packet Type */
	EAvctpLibIncorrectPacketType = 1,
	/** Invalid Transaction label */
	EAvctpLibInvalidTransactionLabel = 2,	
	};
	
void Panic(TAvctpLibPanic aPanic)
	{
	_LIT(KAvctpLibPanicName, "BtAvAvctpPanic");
	User::Panic(KAvctpLibPanicName, aPanic);
	}
	
EXPORT_C TPduHeaderLength ConvertPacketTypeToHeaderLength(TPacketType aPktType)
	{
	TPduHeaderLength ret = ENormalHeaderLength;	
	switch (aPktType) {
		case ENormalPkt:
			// leave ret as ENormalHeaderLength;
			break;
		case EStartFrag:
			ret = EStartFragHeaderLength;
			break;
		case EContinueFrag:
		case EEndFrag:
			ret = EOtherFragHeaderLength;
			break;
		default:
			Panic(EAvctpLibUnknownPacketType);
			break;	
		}		
	return ret;
	}

/**
Provides a method to assert that AVCTP transaction Labels
are valid

  @internalTechnology
  @param aLabel the label to check
*/	
EXPORT_C void AssertValidTransactionLabel(TTransactionLabel aLabel)
	{
	__ASSERT_ALWAYS (IsValidTransactionLabel(aLabel),
				  	 Panic(EAvctpLibInvalidTransactionLabel));
	}


/**
Provides a method to validate AVCTP transaction Labels

  @internalTechnology
  @param aLabel the label to check
  @return ETrue if the label is valid, EFalse otherwise
*/	
EXPORT_C TBool IsValidTransactionLabel(TTransactionLabel aLabel)
	{
	return (aLabel <  KMaxTransactionLabel);
	}

/**
The AVCTP transaction label

  @internalComponent
  @return The transaction label
*/
EXPORT_C TTransactionLabel TransactionLabel(const TDesC8& aData)
	{
	ASSERT_DEBUG(aData.Length() >= EOtherFragHeaderLength); // since EStartFragHeaderLength & ENormalHeaderLength are > EOtherFragHeaderLength

	return static_cast<TTransactionLabel>((*const_cast<TUint8*>(aData.Ptr()) & KTransactionMask) >> 4);
	}

	
/**
The AVCTP message type

  @internalComponent
  @return The message type
*/
EXPORT_C TMessageType MessageType(const TDesC8& aData)
	{
	ASSERT_DEBUG(aData.Length() >= EOtherFragHeaderLength); // since EStartFragHeaderLength & ENormalHeaderLength are > EOtherFragHeaderLength

	return static_cast<TMessageType>(((*const_cast<TUint8*>(aData.Ptr()) & KMessageTypeMask) >> 1) + 1);
	}
	
	
/**
The AVCTP packet type

  @internalComponent
  @return The packet type
*/
EXPORT_C TPacketType PacketType(const TDesC8& aData)
	{
	ASSERT_DEBUG(aData.Length() >= EOtherFragHeaderLength); // since EStartFragHeaderLength & ENormalHeaderLength are > EOtherFragHeaderLength

	return static_cast<TPacketType>((*const_cast<TUint8*>(aData.Ptr()) & KPacketTypeMask) >> 2);
	}	
	
/**
The AVCTP PID

  @internalComponent
  @return The packet's PID
*/
EXPORT_C TPid Pid(const TDesC8& aData)
	{
	ASSERT_DEBUG(aData.Length() >= ENormalHeaderLength); // since EStartFragHeaderLength is > ENormalHeaderLength
	
	TPacketType pkt = PacketType(aData);
	TPid pid = 0;
	
	switch (pkt)
		{
		case ENormalPkt:
			{		
			pid = TPid(BigEndian::Get16(const_cast<TUint8*>(aData.Ptr()) + 1));
			break;
			}
		case EStartFrag:
			{
			pid = TPid(BigEndian::Get16(const_cast<TUint8*>(aData.Ptr()) + 2));
			break;
			}
		default:
			{
			Panic(EAvctpLibIncorrectPacketType);
			}
		}
	return pid;
	}
	
	
/**
Check that the invalid PID bit is not set.

  @internalComponent
  @return ETrue if the bit is not set, EFalse otherwise
*/
EXPORT_C TBool IsValidPid(const TDesC8& aData)
	{
	ASSERT_DEBUG(aData.Length() >= EOtherFragHeaderLength); // since EStartFragHeaderLength & ENormalHeaderLength are > EOtherFragHeaderLength
	__ASSERT_DEBUG(PacketType(aData) == ENormalPkt || 
				   PacketType(aData) == EStartFrag, 
				   Panic(EAvctpLibIncorrectPacketType));
				   
	return ((*const_cast<TUint8*>(aData.Ptr()) & KIsValidPidMask) == 0);
	}

	
EXPORT_C TUint8 FragmentsInSdu(const TDesC8& aData)
	{
	ASSERT_DEBUG(aData.Length() >= EStartFragHeaderLength);
	__ASSERT_DEBUG(PacketType(aData) == EStartFrag, Panic(EAvctpLibIncorrectPacketType));
	
	return *(const_cast<TUint8*>(aData.Ptr()) + 1);
	}

} // end of namespace SymbianAvctp

// EOF

