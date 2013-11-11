// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/hci/hciframe.h>

#include <bluetooth/hci/hciconsts.h>
#include <bluetooth/hci/aclpacketconsts.h>
#include "hcipanic.h"

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COREHCI_BASE);
#endif
	
//
// Base Command Frame
//
CHctlFrameBase::CHctlFrameBase(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize)
 :  iHctlHeaderSize(aHctlHeaderSize),
	iHctlTrailerSize(aHctlTrailerSize),
	iFramePtr(NULL,0)
	{
	LOG_FUNC
	}

CHctlFrameBase::~CHctlFrameBase()
	{
	LOG_FUNC
	
	delete iHctlFrame;
	}

void CHctlFrameBase::ConstructL(TUint32 aHCIFrameSize)
	{
	LOG_FUNC
	
	iHctlFrame = HBufC8::NewMaxL(iHctlHeaderSize + aHCIFrameSize + iHctlTrailerSize);
	iFramePtr.Set(iHctlFrame->Des());
	}

//
// Base Data Command Frame
//
CHctlDataFrameBase::CHctlDataFrameBase(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize)
 :  CHctlFrameBase(aHctlHeaderSize, aHctlTrailerSize)
	{
	LOG_FUNC
	}

/**
Sets the connection handle field in the data frame
@param aConnectionHandle the connection handle of the logical transport (ACL, SCO etc)
*/
EXPORT_C void CHctlDataFrameBase::SetConnectionHandle(THCIConnHandle aConnectionHandle)
	{
	LOG_FUNC
	
	// The connection handle is the first field in a data frame.
	iFramePtr[iHctlHeaderSize] = static_cast<TUint8>(aConnectionHandle & 0xff);
	iFramePtr[iHctlHeaderSize + 1] = static_cast<TUint8>(aConnectionHandle >> 8);
	}

EXPORT_C /*static*/ THCIConnHandle CHctlDataFrameBase::ConnectionHandle(const TDesC8& aHCIDataFrame)
	{
	LOG_STATIC_FUNC
	
	__ASSERT_DEBUG(aHCIDataFrame.Length() >= KHCIDataPacketLengthFieldOffset, PANIC(KCoreHciBasePanicCat, EInvalidDataFrameLength));
	
	THCIConnHandle connHandle = aHCIDataFrame[0];
	connHandle |= ((aHCIDataFrame[1] & ~KPacketDataFlagMask) << 8);
	return connHandle;
	}
	

EXPORT_C const TDesC8& CHctlDataFrameBase::HCTLPayload() const
	{
	LOG_FUNC
	
	return iFramePtr;
	}


//
// ACL Command Frame
//
/**
Two-phase constructor for the ACL Data frame
@param aPayloadSize the maximum amount of ACL data that will be put into the frame
@param aHctlHeaderSize the size of the HCTL header to add to the frame
@param aHctlTrailerSize the size of the HCTL trailer to add to the frame
@return pointer to the allocated ACL data frame
*/
EXPORT_C /*static*/ CHctlAclDataFrame* CHctlAclDataFrame::NewL(TUint16 aPayloadSize, TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize)
	{
	LOG_STATIC_FUNC
	
	CHctlAclDataFrame* frame = new (ELeave) CHctlAclDataFrame(aHctlHeaderSize, aHctlTrailerSize);
	CleanupStack::PushL(frame);
	
	// Add the HCI ACL data frame header length to the payload length.
	frame->ConstructL(KHCIACLDataPacketHeaderLength + aPayloadSize);
	
	CleanupStack::Pop();
	return frame;
	}

CHctlAclDataFrame::CHctlAclDataFrame(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize)
 :  CHctlDataFrameBase(aHctlHeaderSize, aHctlTrailerSize)
	{
	LOG_FUNC
	}

EXPORT_C /*static*/ TAclPacketBoundaryFlag CHctlAclDataFrame::PacketBoundaryFlag(const TDesC8& aData)
	{
	LOG_STATIC_FUNC
	
	__ASSERT_DEBUG(aData.Length() >= KHCIACLDataPacketHeaderLength, PANIC(KCoreHciBasePanicCat, EInvalidAclDataFrameLength));

	TUint8 flags = aData[1] >> KPacketPBBCFlagShift;
	TUint8 flag = flags & KPacketPBFlagMask;
	return static_cast<TAclPacketBoundaryFlag>(flag);
	}

EXPORT_C /*static*/ TAclPacketBroadcastFlag CHctlAclDataFrame::PacketBroadcastFlag(const TDesC8& aData)
	{
	LOG_STATIC_FUNC
	
	__ASSERT_DEBUG(aData.Length() >= KHCIACLDataPacketHeaderLength, PANIC(KCoreHciBasePanicCat, EInvalidAclDataFrameLength));

	TUint8 flags = aData[1] >> KPacketPBBCFlagShift;
	TUint8 flag = (flags & KPacketBCFlagMask) >> KPacketPBtoBCFlagShift;
	return static_cast<TAclPacketBroadcastFlag>(flag);
	}
	
/**
Places ACL data into the frame
@param aData the ACL data
*/
EXPORT_C void CHctlAclDataFrame::SetDataPayload(const TDesC8& aData)
	{
	LOG_FUNC
	
	TInt len = aData.Length();
	__ASSERT_DEBUG(len <= KHCTLMaxACLDataSize, PANIC(KCoreHciBasePanicCat, EInvalidAclDataFrameLength));

	iFramePtr.SetLength(iHctlHeaderSize + KHCIACLDataPacketHeaderLength + len + iHctlTrailerSize);
	iFramePtr.Replace(iHctlHeaderSize + KHCIACLDataPacketHeaderLength, len, aData);
	Finalise();
	}

void CHctlAclDataFrame::Finalise()
	{
	LOG_FUNC
	
	// Set the HCI data frame length.
	TUint16 length = iFramePtr.Length() - (iHctlHeaderSize + KHCIACLDataPacketHeaderLength + iHctlTrailerSize);
	__ASSERT_DEBUG((length <= KHCTLMaxACLDataSize) && (length != 0), PANIC(KCoreHciBasePanicCat, EInvalidAclDataFrameLength));
	
	iFramePtr[iHctlHeaderSize + KHCIDataPacketLengthFieldOffset] = static_cast<TUint8>(length & 0xff);
	iFramePtr[iHctlHeaderSize + KHCIDataPacketLengthFieldOffset + 1] = static_cast<TUint8>(length >> 8);
	}

/**
Sets the ACL flags field in the frame
@param aFlag the flags to set
*/
EXPORT_C void CHctlAclDataFrame::SetFlags(TAclPacketBoundaryFlag aBoundaryFlag, TAclPacketBroadcastFlag aBroadcastFlag)
	{
	LOG_FUNC

	TUint8 flags = aBoundaryFlag | (aBroadcastFlag << KPacketPBtoBCFlagShift);
	iFramePtr[iHctlHeaderSize + 1] |= flags << KPacketPBBCFlagShift;
	}

/**
Return the connection handle from the frame
@return the connection handle from the frame
*/
EXPORT_C TUint16 CHctlAclDataFrame::ConnectionHandle() const
	{
	LOG_FUNC
	
	return CHctlDataFrameBase::ConnectionHandle(iFramePtr.Mid(iHctlHeaderSize, KHCIACLDataPacketHeaderLength));
	}


//
// Synchronous (SCO, eSCO) Command Frame
//
/**
Two-phase constructor for the SCO Data frame
@param aPayloadSize the maximum amount of SCO data that will be put into the frame
@param aHctlHeaderSize the size of the HCTL header to add to the frame
@param aHctlTrailerSize the size of the HCTL trailer to add to the frame
@return pointer to the allocated SCO data frame
*/
EXPORT_C CHctlSynchronousDataFrame* CHctlSynchronousDataFrame::NewL(TUint8 aPayloadSize, TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize)
	{
	LOG_STATIC_FUNC
	
	CHctlSynchronousDataFrame* frame = new (ELeave) CHctlSynchronousDataFrame(aHctlHeaderSize, aHctlTrailerSize);
	CleanupStack::PushL(frame);
	
	// Add the HCI Synchronous data frame header length to the payload length.
	frame->ConstructL(KHCISynchronousDataPacketHeaderLength + aPayloadSize);
	
	CleanupStack::Pop(frame);
	return frame;
	}

CHctlSynchronousDataFrame::CHctlSynchronousDataFrame(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize)
 :  CHctlDataFrameBase(aHctlHeaderSize, aHctlTrailerSize)
	{
	LOG_FUNC
	}

/**
Places SCO data into the frame
@param aData the SCO data
*/
void CHctlSynchronousDataFrame::SetDataPayload(const TDesC8& aData)
	{
	LOG_FUNC
	
	TInt len = aData.Length();
	iFramePtr.SetLength(iHctlHeaderSize + KHCISynchronousDataPacketHeaderLength + len + iHctlTrailerSize);
	iFramePtr.Replace(iHctlHeaderSize + KHCISynchronousDataPacketHeaderLength, len, aData);
	Finalise();
	}

void CHctlSynchronousDataFrame::Finalise()
	{
	LOG_FUNC
	
	// Set the HCI data frame length.
	TUint8 length = iFramePtr.Length() - (iHctlHeaderSize + KHCISynchronousDataPacketHeaderLength + iHctlTrailerSize);
	__ASSERT_DEBUG(length <= KHCTLMaxSynchronousDataSize, PANIC(KCoreHciBasePanicCat, EInvalidScoDataFrameLength));
	
	iFramePtr[iHctlHeaderSize + KHCIDataPacketLengthFieldOffset] = length;
	}


//
// HCI Command Frame
//
CHctlCommandFrame::CHctlCommandFrame(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize)
  : CHctlFrameBase(aHctlHeaderSize, aHctlTrailerSize)
	{
	LOG_FUNC
	}

/**
two-phase constructor
@return pointer to command frame for required opcode
*/
EXPORT_C CHctlCommandFrame* CHctlCommandFrame::NewL(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize)
	{
	LOG_STATIC_FUNC
	
	CHctlCommandFrame* frame = new (ELeave) CHctlCommandFrame(aHctlHeaderSize, aHctlTrailerSize);
	CleanupStack::PushL(frame);

	frame->ConstructL(KHCIMaxCommandLength + KHCICommandPacketHeaderLength);
	frame->ResetCommand();

	CleanupStack::Pop(frame);
	return frame;
	}

EXPORT_C void CHctlCommandFrame::SetOpcode(THCIOpcode aOpcode)
	{
	LOG_FUNC
	
	iFramePtr[iHctlHeaderSize] = static_cast<TUint8>(aOpcode & 0xff);
	iFramePtr[iHctlHeaderSize + 1] = static_cast<TUint8>(aOpcode >> 8);
	}

EXPORT_C void CHctlCommandFrame::FinaliseCommand()
	{
	LOG_FUNC
	
	// Add the trailer octets to the total length.
	iFramePtr.SetLength(iFramePtr.Length() + iHctlTrailerSize);

	// Set the HCI command length.
	TUint8 length = iFramePtr.Length() - (iHctlHeaderSize + KHCICommandPacketHeaderLength + iHctlTrailerSize);
	__ASSERT_DEBUG(length <= KHCIMaxCommandLength, PANIC(KCoreHciBasePanicCat, EInvalidCommandFrameLength));
	
	iFramePtr[iHctlHeaderSize + KHCICommandLengthFieldOffset] = length;
	}

EXPORT_C void CHctlCommandFrame::ResetCommand()
	{
	LOG_FUNC
	
	// Reset the frame to be just the length of the HCTL header
	// plus the HCI command header octets.
	iFramePtr.SetLength(iHctlHeaderSize + KHCICommandPacketHeaderLength);
	}

/**
Append an octet into the command
@param aByte octet to append
*/
EXPORT_C void CHctlCommandFrame::PutByte(TUint8 aByte)
	{
	LOG_FUNC
	
	TInt length = iFramePtr.Length();
	iFramePtr.SetLength(length + 1);
	iFramePtr[length] = aByte;
	}

/**
Append two octets into the command
@param aVal two octets to append
*/
EXPORT_C void CHctlCommandFrame::PutBytes16(TUint16 aVal)
	{
	LOG_FUNC

	TInt length = iFramePtr.Length();
	iFramePtr.SetLength(length + 2);
	iFramePtr[length + 1] = TUint8(aVal >> 8);
	iFramePtr[length] = TUint8(aVal & 0xff);
	}

/**
Append three octets into the command.
Note the most signficant octet in the 32bit value passed in will be ignored.
@param aVal three octets to append
*/
EXPORT_C void CHctlCommandFrame::PutBytes24(TUint32 aVal)
	{
	LOG_FUNC

	PutBytes32(aVal, 3);
	}

/**
Append four octets into the command
@param aVal four octets to append
*/
EXPORT_C void CHctlCommandFrame::PutBytes32(TUint32 aVal)
	{
	LOG_FUNC

	PutBytes32(aVal, 4);
	}

/**
Append 1-4 bytes into command packet
@param aVal the value
@param aBumOfBytes the number of octets that are valid
@panic if aNumOfBytes!=4,3,2,1
*/
EXPORT_C void CHctlCommandFrame::PutBytes32(TUint32 aVal,TUint8 aNumOfBytes)
	{
	LOG_FUNC
	
	TInt length = iFramePtr.Length();
	iFramePtr.SetLength(length + aNumOfBytes);
	switch (aNumOfBytes)
		{
		// deliberate fallthroughs...
	case 4:
		iFramePtr[length + 3] = TUint8(aVal >> 24);
	case 3:
		iFramePtr[length + 2] = TUint8(aVal >> 16 & 0xff);
	case 2:
		iFramePtr[length + 1] = TUint8(aVal >> 8 & 0xff);
	case 1:
		iFramePtr[length] = TUint8(aVal & 0xff);
		break;
	default:
		__ASSERT_DEBUG(EFalse, PANIC(KCoreHciBasePanicCat, EBadArgumentValueOutOfBounds));
		}
	}

/**
Append a stream of binary data into the command
@param aData The binary data to be appended
*/
EXPORT_C void CHctlCommandFrame::PutString(const TDesC8& aString)
	{
	LOG_FUNC

	iFramePtr.Append(aString);
	}

/**
Append a string into the command, and if neccessary pad it with '\0' to make it the
required length.
@param aString The string to append.
@param aRequiredLength The requried length of the string in the command.
*/
EXPORT_C void CHctlCommandFrame::PutPaddedString(const TDesC8& aString, TInt aRequiredLength)
	{
	LOG_FUNC

	__ASSERT_DEBUG(aRequiredLength >= aString.Length(), PANIC(KCoreHciBasePanicCat, EMoreCommandDataThanRequired));
	PutString(aString);
	const TChar padByte = '\0';
	iFramePtr.AppendFill(padByte, aRequiredLength - aString.Length());
	}

/**
Append a Bluetooth device address into the command
@param aBdAddr reference to device address to append
*/
EXPORT_C void CHctlCommandFrame::PutDevAddr(const TBTDevAddr& aBdAddr)
	{
	LOG_FUNC
	
	TInt length = iFramePtr.Length();
	iFramePtr.SetLength(length + KBTMaxDevAddrSize);
	for (TInt i = 0; i < KBTMaxDevAddrSize; i++)
		{
		// big endian put of device address
		iFramePtr[length + i] = aBdAddr[KBTMaxDevAddrSize - 1 - i];
		}	
	}

/**
Append an AFH host channel classification into the command
@param aClassification Descriptor containing bitmap of channels to be set as 'bad' (i.e. busy)
*/
EXPORT_C void CHctlCommandFrame::PutAFHHostChannelClassification(const TDesC8& aClassification)
// big endian put of channel classification
	{
	LOG_FUNC
	
	TInt length = iFramePtr.Length();
	TInt lengthClassification = aClassification.Length();
	__ASSERT_DEBUG((iFramePtr.MaxLength() - length - lengthClassification) >= 0, PANIC(KCoreHciBasePanicCat, ECommandBufferOverflow));

	iFramePtr.SetLength(length + lengthClassification);
	for (TInt i = 0; i < lengthClassification; i++)
		{
		iFramePtr[length + i] = aClassification[lengthClassification - 1 - i];
		}	
	}

/**
returns reference to the entire Host Controller Transport frame including transport headers and trailers
@return reference to the HCTL frame buffer
*/
EXPORT_C const TDesC8& CHctlCommandFrame::HCTLPayload() const
	{
	LOG_FUNC
	
	// Ensure that the command is finalised.
	__ASSERT_DEBUG(iFramePtr[iHctlHeaderSize + KHCICommandLengthFieldOffset] == (iFramePtr.Length() - (iHctlHeaderSize + KHCICommandPacketHeaderLength + iHctlTrailerSize)),
	               PANIC(KCoreHciBasePanicCat, ECommandNotFinalised));
	return iFramePtr;
	}

/**
Append a link key within a HCI command frame
@param aKey the linkkey
*/
EXPORT_C void CHctlCommandFrame::PutLinkKey(const TBTLinkKey& aKey)
	{
	LOG_FUNC
	
	TPtrC8 key (aKey().iLinkKey, KHCILinkKeySize);

	TInt length = iFramePtr.Length();
	iFramePtr.SetLength(length + KHCILinkKeySize);

	// little endian put - lsb at lowest address
	for (TUint i = 0; i < KHCILinkKeySize; i++)
		{
		iFramePtr[length + i] = key[KHCILinkKeySize - 1 - i];
		}
	}


/**
Append a connection handle within a HCI command frame
@param aConnectionHandle the connection handle
*/
EXPORT_C void CHctlCommandFrame::PutConnectionHandle(THCIConnectionHandle aConnectionHandle)
	{
	LOG_FUNC
	
	PutBytes16(aConnectionHandle.ConnHandle());
	}

/**
Appends a simple pairing hash C value within a HCI frame
@param aHashC the hash C value
*/
EXPORT_C void CHctlCommandFrame::PutSimplePairingHash(const TBluetoothSimplePairingHash& aHash)
	{
	LOG_FUNC
	
	TInt length = iFramePtr.Length();
	
	iFramePtr.SetLength(length + KBluetoothSimplePairingHashSize);
	
	for (TUint i = 0; i < KBluetoothSimplePairingHashSize; i++)
		{
		iFramePtr[length + i] = aHash[KBluetoothSimplePairingHashSize - 1 - i];
		}
	}

/**
Appends a simple pairing randomizer R value with a HCI frame
@param aRandomizerR the randomizer R value
*/
EXPORT_C void CHctlCommandFrame::PutSimplePairingRandomizer(const TBluetoothSimplePairingRandomizer& aRandomizer)
	{
	LOG_FUNC

	TInt length = iFramePtr.Length();
	
	iFramePtr.SetLength(length + KBluetoothSimplePairingRandomizerSize);
	
	for (TUint i = 0; i < KBluetoothSimplePairingRandomizerSize; i++)
		{
		iFramePtr[length + i] = aRandomizer[KBluetoothSimplePairingRandomizerSize - 1 - i];
		}

	}

