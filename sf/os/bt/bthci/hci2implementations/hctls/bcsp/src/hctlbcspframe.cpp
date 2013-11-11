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

/**
 @file
 @internalComponent
*/

#include "hctlbcspframe.h"

#include "bcsputils.h"
#include "hctlbcspconsts.h"
#include "debug.h"

const TUint16 KCrcTable[] = 
/**
	Global CRC table
*/
	{
	0x0000, 0x1081, 0x2102, 0x3183,
	0x4204, 0x5285, 0x6306, 0x7387,
	0x8408, 0x9489, 0xa50a, 0xb58b,
	0xc60c, 0xd68d,	0xe70e, 0xf78f
	};

/**
Implementation of Class THctlBcspPacketType
*/


/**
	Method to set BCSP ProtocolId
*/
TInt THCTLBcspPacketType::SetProtocolId(TUint8 aProtocolId)
	{
	LOG_FUNC

#ifdef __ARMCC__
// Suppressing the warning "Pointless comparison of unsigned integer with zero"
// as although a constant may be zero it is present so as to make the code more
// understandable.
# pragma diag_suppress 186
#endif // __ARMCC__

	// if (>2 && <5 || >7) then fail
if ((aProtocolId > KBcspBcCmdChnl && aProtocolId < KBcspCommEvntChnl) || (aProtocolId > KBcspSynchronousDataChnl))
	{
	LOG1(_L8("THCTLBcspPacketType: Header unsupported, this used to be a BCSP 6 panic, now error handled. aProtocolId = %d"), aProtocolId);
	return KErrNotSupported;
	}	               
	               
#ifdef __ARMCC__
# pragma diag_default 186
#endif // __ARMCC__

	iProtocolId = aProtocolId;
	return KErrNone;
	}

TUint8 THCTLBcspPacketType::ProtocolId() const
	{
	return iProtocolId;
	}

/**
Implementation of Class THctlBcspFlags
*/

TBool THCTLBcspFlags::CrcEnabled() const
	{
	return (iFlagsField & KBcspCrcPresentMask);
	}

TUint8 THCTLBcspFlags::Ack() const
	{
	return ((iFlagsField & KBcspAckMask) >> KBcspAckFieldOffset);
	}

TUint8 THCTLBcspFlags::Sequence() const
	{
	return (iFlagsField & KBcspSeqMask);
	}

TBool THCTLBcspFlags::IsReliableProtcolType() const
	{
	return (iFlagsField & KBcspProtocolTypeMask);
	}

TUint8 THCTLBcspFlags::FlagsField() const
	{
	return iFlagsField;
	}

/**
	Trivial setter will panic if we attempt to set an invalid value
*/
void THCTLBcspFlags::SetAck(TUint8 aValue) 
	{
	__ASSERT_DEBUG((aValue < KMaxWindowSize), PANIC(KBcspPanicCat, EBcspInvalidAckValue));
	// Clear the existing value and set the new value.
	iFlagsField &= ~KBcspAckMask;
	iFlagsField |= (aValue << KBcspAckFieldOffset);
	}

/**
	Trivial setter will panic if we attempt to set an invalid value
*/
void THCTLBcspFlags::SetSequence(TUint8 aSeq)
	{
	__ASSERT_DEBUG((aSeq < KMaxWindowSize), PANIC(KBcspPanicCat, EBcspInvalidSequenceValue));
	iFlagsField &= ~KBcspSeqMask;
	iFlagsField |= aSeq;
	}

/**
	Trivial setter
*/
void THCTLBcspFlags::SetCrcEnabled(TBool aCrcEnabled)
	{
	if(aCrcEnabled)
		{
		iFlagsField |= KBcspCrcPresentMask;
		}
	else
		{
		iFlagsField &= ~KBcspCrcPresentMask;
		}
	}

void THCTLBcspFlags::SetIsReliableProtcolType(TBool aIsReliableProtcolType)
	{
	if(aIsReliableProtcolType)
		{
		iFlagsField |= KBcspProtocolTypeMask;
		}
	else
		{
		iFlagsField &= ~KBcspProtocolTypeMask;
		}
	}

void THCTLBcspFlags::SetFlagsField(TUint8 aFlagsField)
	{
	iFlagsField = aFlagsField;
	}
	
/**
Implementation of Class CHCTLBcspFrame
*/
CHCTLBcspFrame::CHCTLBcspFrame()
	{
	}

/**
	Method to set the flags in the Header byte of a BCSP frame
*/
void CHCTLBcspFrame::SetFlags(TUint8 aAck, TBool aIsCrcEnabled, TBool aIsReliableProtcolType)
	{
	LOG_FUNC
	iFlagHeaderByte.SetAck(aAck);
	iFlagHeaderByte.SetCrcEnabled(aIsCrcEnabled);
	iFlagHeaderByte.SetIsReliableProtcolType(aIsReliableProtcolType);
	}

void CHCTLBcspFrame::SetSequence(TUint8 aSeq)
	{
	iFlagHeaderByte.SetSequence(aSeq);
	}

TInt CHCTLBcspFrame::SetProtocolId(TUint8 aProtocolId)
	{
	return iProtocolId.SetProtocolId(aProtocolId);
	}

TBool CHCTLBcspFrame::CrcEnabled() const
	{
	return iFlagHeaderByte.CrcEnabled();
	}

TUint8 CHCTLBcspFrame::CheckSum() const
	{
	return iCheckSum;
	}

TUint8 CHCTLBcspFrame::ProtocolId() const
	{
	return iProtocolId.ProtocolId();
	}

TUint8 CHCTLBcspFrame::FlagsField() const
	{
	return iFlagHeaderByte.FlagsField();
	}

TUint16 CHCTLBcspFrame::PayloadLength() const
	{
	return static_cast<TUint16>(iPayloadPtr.Size());
	}

TInt CHCTLBcspFrame::ValidateChecksum(const TDesC8& aHeader)
/**
	Method to validate the checksum of a received frame header
*/
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	if (aHeader.Length() < KBcspHeaderBytes)
		{
		rerr = KErrBcspHeaderCorrupt;
		}
	else if(aHeader[KBcspHeaderChecksumByteIndex] != CalcCheckSum(aHeader))
		{
		rerr = KErrBcspHeaderCorrupt;
		}

	return rerr;
	}

TDesC8& CHCTLBcspFrame::Payload()
/**
	Trivial getter method
*/
	{
	return iPayloadPtr;
	}

TUint8 CHCTLBcspFrame::Sequence() const
/**
	Trivial getter method
*/
	{
	return iFlagHeaderByte.Sequence();
	}

TUint8 CHCTLBcspFrame::Ack() const
/**
	Trivial getter method
*/
	{
	return iFlagHeaderByte.Ack();
	}

TBool CHCTLBcspFrame::IsReliableProtcolType() const
/**
	Trivial getter method
*/
	{
	return iFlagHeaderByte.IsReliableProtcolType();
	}

TUint16 CHCTLBcspFrame::CalcCRC(const TDesC8& aData)
/**
 	@param aData - Parameters Buffer upon which to calculate crc
 	@return CRC Value TUint16
*/
 	{
 	LOG_FUNC
  	TUint16 crcValue = 0xffff;	// Fill the shift register with 1s
  	TUint8 dataByte;
  	
  	for (TInt i=0;i<aData.Length();i++)
  		{
  		dataByte = aData[i];
  		crcValue = static_cast<TUint16>((crcValue >> 4) ^ KCrcTable[(crcValue^dataByte) & 0xf]);
  		crcValue = static_cast<TUint16>((crcValue >> 4) ^ KCrcTable[(crcValue^(dataByte >> 4)) & 0xf]);
  		}
  	return ReverseCrcBits(crcValue); // Reverse all the bits in the CRC
  	}

TUint16 CHCTLBcspFrame::ReverseCrcBits(TUint16 aCRCValue)
/**
	Method to reverse the CRC Value
	@param aCRCValue
*/
	{
	TUint16 result = 0; 
	for(TInt b=0;b<16;b++)
		{
		result = static_cast<TUint16>(result << 1) ;
		result |= (aCRCValue & 1);
		aCRCValue = static_cast<TUint16>(aCRCValue >> 1);
		}
	return result;
	}



TUint8 CHCTLBcspFrame::CalcCheckSum(const TDesC8& aHeader)
/**
	Method to calculate the checksum
	The Checksum error is used to check the integrity of the header:
*/
	{
	LOG_FUNC
	__ASSERT_DEBUG(aHeader.Length() >= KBcspHeaderBytes, PANIC(KBcspPanicCat, EBadBcspFrame));
	TUint8 tempsum = static_cast <TUint8>(aHeader[KBcspHeaderFlagsByteIndex] + 
	                                      aHeader[KBcspHeaderByte2Index] + 
	                                      aHeader[KBcspHeaderByte3Index]);
	return ~tempsum;
	}

void CHCTLBcspFrame::Reset()
/**
	Method to reset the frame
*/
	{
	LOG_FUNC
	iIsValid = EFalse;
	}

TBool CHCTLBcspFrame::IsValid() const
/**
	Trvial getter method
*/
 	{
 	return iIsValid;
 	}


/**
Implementation of Class CTxHctlBcspFrame
*/

CTxHctlBcspFrame* CTxHctlBcspFrame::NewLC(const TDesC8& aHCIPayload)
	{
	LOG_STATIC_FUNC
	CTxHctlBcspFrame* self= new(ELeave) CTxHctlBcspFrame();
	CleanupStack::PushL(self);
	self->ConstructL(aHCIPayload);
	return self;
	}

CTxHctlBcspFrame* CTxHctlBcspFrame::NewLC(TInt aFrameSize)
	{
	LOG_STATIC_FUNC
	CTxHctlBcspFrame* self= new(ELeave) CTxHctlBcspFrame();
	CleanupStack::PushL(self);
	self->ConstructL(aFrameSize);
	return self;
	}

CTxHctlBcspFrame* CTxHctlBcspFrame::NewL(const TDesC8& aHCIPayload)
	{
	LOG_STATIC_FUNC
	CTxHctlBcspFrame *self=NewLC(aHCIPayload);
	CleanupStack::Pop(self);
	return self;
	}

CTxHctlBcspFrame* CTxHctlBcspFrame::NewL(TInt aFrameSize)
	{
	LOG_STATIC_FUNC
	CTxHctlBcspFrame *self=NewLC(aFrameSize);
	CleanupStack::Pop(self);
	return self;
	}

void CTxHctlBcspFrame::ConstructL(const TDesC8& aHCIPayload)
	{
	LOG_FUNC

	// Instantiates iFrame and the TPtr8 iFramePtr to it
	// Then calls the SetPayload() method with the param aHCIPayload
	iFrame = HBufC8::NewL(aHCIPayload.Length() + KBcspHeaderBytes + KBcspCrcBytes);
	iFramePtr.Set(iFrame->Des());

	SetPayload(aHCIPayload);
	}

void CTxHctlBcspFrame::ConstructL(TInt aFrameSize)
	{
	LOG_FUNC

	// Instantiates iFrame and the TPtr8 iFramePtr to it
	// then calls the SetPayload() method.
	iFrame = HBufC8::NewL(KBcspHeaderBytes + KBcspCrcBytes + aFrameSize);
	iFramePtr.Set(iFrame->Des());

	SetPayload();
	}

void CTxHctlBcspFrame::SetPayload(const TDesC8 &aPayload)
	{
	iFramePtr.SetMax();
	TPtr8 payload(iFramePtr.MidTPtr(KBcspHeaderBytes));

	payload = aPayload.Right(payload.MaxSize());
	iPayloadPtr.Set(payload);

	iFramePtr.SetLength(KBcspHeaderBytes + aPayload.Size());

	iIsValid = ETrue;
	}

void CTxHctlBcspFrame::SetPayload()
	{
	LOG_FUNC
	iPayloadPtr.Set(KNullDesC8);

	iFramePtr.SetLength(KBcspHeaderBytes);

	iIsValid = ETrue;
	}

CTxHctlBcspFrame::CTxHctlBcspFrame()
  : iFramePtr(NULL, 0, 0)
	{
	LOG_FUNC
	}

CTxHctlBcspFrame::~CTxHctlBcspFrame()
	{
	LOG_FUNC
	delete iFrame;
	}

void CTxHctlBcspFrame::BuildFrame()
/**
	Build  BCSP frame method

	Zero the frame Ptr
	Set the frame length to KMaxHeaderBytes - to address the BCSP header bytes

	Populate all 4 header bytes with the Flag byte,PayloadLength, ProtocolId and checksum

	Set the frame length to Payloadlength + KMaxBcspHeaderBytes

	Populate the BCSP frame payload

	Check if the CRC flag is set if it is then Calculate the CRC of the Whole Frame and append the crc

*/
	{
	LOG_FUNC
	iFramePtr.SetLength(KBcspHeaderBytes);
	iFramePtr[KBcspHeaderFlagsByteIndex] = iFlagHeaderByte.FlagsField();
	iFramePtr[KBcspHeaderByte2Index] = static_cast<TUint8>(ProtocolId() | ((PayloadLength() & 0xf) << 4));
	iFramePtr[KBcspHeaderByte3Index] = static_cast<TUint8>((PayloadLength() & 0xff0) >> 4);
	iFramePtr[KBcspHeaderChecksumByteIndex] = CalcCheckSum(iFramePtr);

	iFramePtr.SetLength(PayloadLength() + KBcspHeaderBytes);

	if (PayloadLength() > 0)
		{
		iPayloadPtr.Set(iFramePtr.Ptr() + KBcspHeaderBytes, PayloadLength());
		}

	if (CrcEnabled())
		{
		TUint16 crcValue = CalcCRC(iFramePtr.Left(PayloadLength() + KBcspHeaderBytes));
		iFramePtr.Append((crcValue & 0xff00) >> 8); //Append first byte of CRC
		iFramePtr.Append(crcValue & 0x00ff);        //Append second Byte of CRC
		}
	}

void CTxHctlBcspFrame::Reset()
/**
	Reset the BCSP frame and set retries to 0
*/
	{
	LOG_FUNC
	CHCTLBcspFrame::Reset();
	iRetries = 0;
	}

void CTxHctlBcspFrame::SlipEncodeFrame(TPtr8 aSlipFrame)
/**
	SlipEncoding method
	SLIP is outlined in RFC 1055. The principle is to surround each packet
	with a start and an end special byte. This allows the beginning and the end of a packet to be
	found and consequently the length of the packet to be known.
	The packet encoding and decoding scheme has two basic rules:
	Because the special byte can only appear in the stream as a delimiter, it has to be replaced inside
	the payload by an escape sequence, composed of two bytes: 0xc0 is mapped to 0xDB 0xDC.
	Consequently 0xDB is also a special byte, and each occurrence of this escape byte is replaced
	by an another sequence of two bytes: 0xDB 0xDD
	This method is very quick and simple for encoding a packet. The corresponding Decoding
	procedure is used for received packets.
	Moreover, as explained in the section regarding detecting errors, the SLIP layer provides a
	method of error detection.
*/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iFrame != NULL, PANIC(KBcspPanicCat, EBadFramePointer));

	aSlipFrame.Zero();
	aSlipFrame.SetMax();

	TInt i=0;
	TInt j=0;

	aSlipFrame[j++] = KSlipWrapperByte;

	for (i=0;i<iFrame->Length();i++)
		{
		switch((*iFrame)[i])
			{
			case KSlipWrapperByte:
				{
				aSlipFrame[j++] = KSlipByteDB;
				aSlipFrame[j++] = KSlipByteDC;
				break;
				}
			case 0xdb:
				{
				aSlipFrame[j++] = KSlipByteDB;
				aSlipFrame[j++] = KSlipByteDD;
				break;
				}

			default:
				{
				aSlipFrame[j++] = (*iFrame)[i];
				break;
				}
			}
		}

	aSlipFrame[j++] = KSlipWrapperByte;
	aSlipFrame.SetLength(j);
	}



/**
Implementation of Class CRxHctlBcspFrame
*/


CRxHctlBcspFrame* CRxHctlBcspFrame::NewL()
/**
	CRxHctlBcspFrame::NewL() method
	
	@return self
*/
	{
	LOG_STATIC_FUNC
	return new(ELeave) CRxHctlBcspFrame;
	}

TInt CRxHctlBcspFrame::SlipDecodeFrame(const TDesC8& aReceivedFrame)
/**
	Slip decoding methopd for received frames
*/
	{
	LOG_FUNC
	TInt rerr = KErrNone;

 	// TPtrC8 packet = aReceivedFrame;
	TPtr8 packet(const_cast<TUint8*>(aReceivedFrame.Ptr()),
			     aReceivedFrame.Size(),
				 aReceivedFrame.Size());

	if ((packet.Length() < KBcspHeaderBytes) || (packet[packet.Length()-1] != KSlipWrapperByte) || (packet[0] == KSlipWrapperByte))
		{
		/** 
		Fail slip decoding for one of or none of these reasons:
			1. first byte is another slip wrapper byte (One has been received already and discarded in terminating reads)
			   should not receive 2 consecutive slip wrapper byte in any one slip packet
			2. The final packet byte is not a slip wrapper byte
		
		NB. A good slip Packet at this stage should have x-bytes appended with one slip wrapper byte.
		This is because the first slip wrapper byte was already picked up on the first terminated read.
		*/
		rerr = KErrSlipCorrupted;
		}
	else
		{
		packet.SetLength(packet.Length() - 1); // Remove the last 0xc0 byte.

		TInt  i = 0;
		TInt  offset = 0;
		TInt  length = packet.Length();

		while ( (i<length) && (rerr == KErrNone) )
			{
			switch (packet[i])
				{
				case 0xc0:	// If this appears do nothing as it should be either the header or trailer byte checked above
					rerr = KErrSlipCorrupted;
					break;

				case 0xdb:
					i++; // Check the next byte immediately
					offset--;
					if ( i<length )
						{
						switch (packet[i])
							{
							case 0xdc:
								packet[i+offset] = 0xc0;
								break;

							case 0xdd:
								packet[i+offset] = 0xdb;
								break;

							default:
								rerr = KErrSlipCorrupted;
							};
						}
					else
						{
						rerr = KErrSlipCorrupted;
						}
					break;

				default:
					if ( offset )
						{
						packet[i+offset] = packet[i];
						}
					break;
				}
			i++;
			}

		if(rerr == KErrNone)
			{
			packet.SetLength(length + offset);
			rerr = Set(packet);
			}
		}

	return rerr;
	}

TInt CRxHctlBcspFrame::Set(const TDesC8& aFrame)
/**
	Populate a BCSP frame object and vaildate its contents via the various BCSP error 
	checking methods
*/
	{
	LOG_FUNC
	TInt err = ValidateChecksum(aFrame);	// Do Checksum check
	TInt errId = KErrNone;
	TUint16 payloadLen = 0;

	if (err == KErrNone)
		{
		ParseFlagByte(aFrame);	// populate THctlBcspFlags members
		
		errId = ParsePayloadLenProtId(aFrame, payloadLen);	
		if (errId)
			{
			return errId;	// corrupted Id, no point in continuing 	
			}

		// Check header payload length field is the same as the actual HCIPayloadLength
		if (CrcEnabled())
			{
			TInt actualPayloadLen = aFrame.Length() - KBcspCrcBytes - KBcspHeaderBytes;
			if(actualPayloadLen >= 0) // Check payload is sufficient length
				{
				iPayloadPtr.Set(aFrame.Ptr() + KBcspHeaderBytes, actualPayloadLen);

				if (PayloadLength() != payloadLen) // Check payload length
					{
					LOG(_L8("CRxHctlBcspFrame: KErrBcspCorruptedHCIPayload..."));
					err = KErrBcspCorruptedHCIPayload;
					}
				else if (CheckCRC(aFrame) != KErrNone) // Do CRC Check
					{
					LOG(_L8("CRxHctlBcspFrame: KErrBCSPCRCCheckFailed..."));
					err = KErrBcspCRCCheckFailed;
					}
				}
			else
				{
				LOG(_L8("CRxHctlBcspFrame: KErrBcspCorruptedHCIPayload..."));
				err = KErrBcspCorruptedHCIPayload;
				}
			}
		else 
			{
			TInt actualPayloadLen = aFrame.Length() - KBcspHeaderBytes;
			// There is no need to check that the payload is of sufficient length since
			// the ValidateChecksum call would have failed and stopped us getting here.
			__ASSERT_DEBUG(actualPayloadLen >= 0, PANIC(KBcspPanicCat, EBadBcspFrame));
			iPayloadPtr.Set(aFrame.Ptr() + KBcspHeaderBytes, actualPayloadLen);
			if (PayloadLength() != payloadLen) 
				{
				LOG(_L8("HCTLBcsp: KErrBcspCorruptedHCIPayload..."));
				LOGHEXDESC(aFrame);
				err = KErrBcspCorruptedHCIPayload;
				}
			}
		}

	iIsValid = (err == KErrNone) ? ETrue : EFalse;

	return err;
	}


CRxHctlBcspFrame::CRxHctlBcspFrame()
  : CHCTLBcspFrame()
	{
	}

void CRxHctlBcspFrame::ParseFlagByte(const TDesC8& aHeader)
	{
	LOG_FUNC
	LOG1(_L8("Parse HeaderByte %02x"), aHeader[KBcspHeaderFlagsByteIndex]);

	iFlagHeaderByte.SetFlagsField(aHeader[KBcspHeaderFlagsByteIndex]);
	}

TInt CRxHctlBcspFrame::ParsePayloadLenProtId(const TDesC8& aHeader, TUint16& aRetPayloadLen)
/**
	Method to extract the Payload Length and Protocol Id from the 2nd and 3rd header bytes
*/
	{
	LOG_FUNC
	TInt err = KErrNone;

	err = SetProtocolId((aHeader[KBcspHeaderByte2Index] & KBcspProtocolIdMask));
	if (!err)
		{
		aRetPayloadLen = static_cast<TUint16>(((aHeader[KBcspHeaderByte2Index] & KBcspPayloadLen1stBitsMask) >> 4) | (aHeader[KBcspHeaderByte3Index] << 4));
		}
	
	return err;
	}

TInt CRxHctlBcspFrame::CheckCRC(const TDesC8 &aFrame)
/**
	Method to check and validate the CRC
*/
	{
	LOG_FUNC
	TPtrC8 frame(aFrame.Ptr(), aFrame.Size() - KBcspCrcBytes);

	TUint16 tempCRCvalue;
	TUint16 crcValue = CalcCRC(frame);	// Calculates and sets iCRCValue

	//compare with hctl trailer CRC value
	TBuf8<2> crcval;
	crcval = aFrame.Right(KBcspCrcBytes);
	
	tempCRCvalue = static_cast<TUint16>((crcval[0] << 8) + crcval[1]);

	return ((crcValue == tempCRCvalue) ? KErrNone : KErrBcspCRCCheckFailed);
	}
