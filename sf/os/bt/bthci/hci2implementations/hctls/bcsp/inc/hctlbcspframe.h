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

#ifndef HCTLBCSPFRAME_H
#define HCTLBCSPFRAME_H

#include <e32base.h>

class HCTLBcspFrameHelper;

NONSHARABLE_CLASS(THCTLBcspPacketType)		
/**
T Class for setting the correct protocol ID for the BCSP frame Header Byte
depends on the packet type
*/
    {
public:
	TInt SetProtocolId(TUint8 aProtocolId);
	TUint8 ProtocolId() const;
private:
	TUint8 iProtocolId;
    };


NONSHARABLE_CLASS(THCTLBcspFlags)
/**
T Class for setting the remainder of the information conveyed in the flag header byte
*/
	{
public:
	void SetSequence(TUint8 aSeq); 
	void SetAck(TUint8 aAck); 
	void SetCrcEnabled(TBool aIsCrcEnabled);
	void SetIsReliableProtcolType(TBool aIsReliableProtcolType);
	void SetFlagsField(TUint8 aFlagsField);
	
	TUint8 Sequence() const;
	TUint8 Ack() const;
	TBool CrcEnabled() const;
	TBool IsReliableProtcolType() const;
	TUint8 FlagsField() const;

private:
	TUint8 iFlagsField;
	};

NONSHARABLE_CLASS(CHCTLBcspFrame) : public CBase
/**
Class used to populate a BCSP frame with the correct header,trailer data and payload
*/
	{
public:
	void SetFlags(TUint8 aAck, TBool aIsCrcEnabled, TBool aIsReliableProtcolType);
	void SetSequence(TUint8 aSeq);

	TInt SetProtocolId(TUint8 aProtocolId);
	TBool CrcEnabled() const;
	TUint8 ProtocolId() const;
	TUint8 FlagsField() const;
	TBool IsReliableProtcolType() const;
	TUint8 Sequence() const;
	TUint8 Ack() const;
	TUint8 CheckSum() const;
	TUint16 CalcCRC(const TDesC8& aData);

	TUint16 PayloadLength() const;
	TDesC8& Payload();

	TInt ValidateChecksum(const TDesC8& aHeader);
	virtual TBool IsValid() const;
	virtual void Reset();

protected:
	CHCTLBcspFrame();
	TUint8 CalcCheckSum(const TDesC8& aHeader);

private:    
	TUint16 ReverseCrcBits(TUint16 aCRCValue);

protected:
	TPtrC8 iPayloadPtr;
	TUint16 iCRCValue;
	TUint8 iCheckSum;
	THCTLBcspFlags iFlagHeaderByte;
	TBool iIsValid;

private:
	THCTLBcspPacketType	iProtocolId;
	};


NONSHARABLE_CLASS(CTxHctlBcspFrame) : public CHCTLBcspFrame
/**
CTxHctlBcspFrame
Provides an object which embodies a BCSP frame including methods for populating the 
	HCI payload
	Header bytes
	CRC bytes
Provides all error checking (Packet Integrity) excluding SLIP encoding and decoding
*/
	{
	friend class CHCTLBcspWindow;

public:
	static CTxHctlBcspFrame* NewLC(const TDesC8& aHCIPayload);
	static CTxHctlBcspFrame* NewLC(TInt aFrameSize=0);
	static CTxHctlBcspFrame* NewL(const TDesC8& aHCIPayload);
	static CTxHctlBcspFrame* NewL(TInt aFrameSize=0);

	virtual ~CTxHctlBcspFrame();

	void SetPayload(const TDesC8 &aPayload);
	void SetPayload();
	
	void BuildFrame();
	void SlipEncodeFrame(TPtr8 aSlipFrame);

	void IncrementRetries() { iRetries++; };
	TInt Retries(void) const { return iRetries; };

	void Reset();

private:
    CTxHctlBcspFrame();

    void ConstructL(const TDesC8& aHCIPayload);
	void ConstructL(TInt aFrameSize);

	const TDesC8& BuildHeaderBytes(const TUint8 aFlagHeaderByte, const TUint8 aProtocolId,const TUint16 aPayloadLength);

	TUint16 UpdateCRC(TUint8 aDataByte,TUint16 aCRCValue);
	TUint16 GetBigEndianCRC(TUint16 aCRCValue);

private:
	// When config stuff complete chuck all THctlBcspFrame into iSlipEncoded HBufC8* 
	// queue on SequenceBuf or DatagramBuf
	HBufC8* iFrame;	  
	TPtr8 iFramePtr;
	TInt iRetries; // Number of times that this has been sent so far
	TSglQueLink iLink; // For SqlQue and the Window
	};

NONSHARABLE_CLASS(CRxHctlBcspFrame) : public CHCTLBcspFrame
/**
TRxHctlBcspFrame
Provides an object which embodies a BCSP frame including methods for populating the 
	HCI payload
	Header bytes
	CRC bytes
Provides all error checking (Packet Integrity) excluding SLIP encoding and decoding
*/
	{
public:
	static CRxHctlBcspFrame *NewL();

	TInt SlipDecodeFrame(const TDesC8& aReceivedFrame);

private:
    CRxHctlBcspFrame();
	void ParseFlagByte(const TDesC8& aFlagbyte);
	TInt ParsePayloadLenProtId(const TDesC8& aHeader, TUint16& aRetPayloadLen);
	TInt CheckCRC(const TDesC8& aFrame);
	TInt Set(const TDesC8& aHCIPayload);
	};

#endif // HCTLBCSPFRAME_H
