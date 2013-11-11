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
// SUPL POS - Implementation of SUPL POS encoding & decoding.
// 
//

#include "suplpos.h"
#include "asn1per.h"


/**
Implemetation of CSuplPosPayloadContent Class
*/
EXPORT_C CSuplPosPayloadContent* CSuplPosPayloadContent::NewLC()
	{
	CSuplPosPayloadContent* self = new (ELeave) CSuplPosPayloadContent();
	CleanupStack::PushL(self);
	return self;				
	}

EXPORT_C CSuplPosPayloadContent* CSuplPosPayloadContent::NewL()
	{
	CSuplPosPayloadContent* self = CSuplPosPayloadContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}
EXPORT_C CSuplPosPayloadContent::~CSuplPosPayloadContent()
	{
	iPayload.Close();
	}

void CSuplPosPayloadContent::ConstructL()
	{
	}
	
CSuplPosPayloadContent::CSuplPosPayloadContent()
	{		
	}

TBool CSuplPosPayloadContent::operator==(const CSuplPosPayloadContent& other) const
	{
	return  (iPayloadType==other.iPayloadType &&
			!iPayload.CompareF(other.iPayload));
	}

/**
Implemetation of CSuplPosPayload Class
*/
CSuplPosPayload* CSuplPosPayload::NewLC(CSuplPosPayloadContent* aPosPayloadContent)
	{
	CSuplPosPayload* self = new (ELeave) CSuplPosPayload(aPosPayloadContent);
	CleanupStack::PushL(self);
	return self;		
	}


CSuplPosPayload* CSuplPosPayload::NewL(CSuplPosPayloadContent* aPosPayloadContent)
	{
	CSuplPosPayload* self = CSuplPosPayload::NewLC(aPosPayloadContent);
	CleanupStack::Pop(self);
	return self;			
	}
	
CSuplPosPayload::CSuplPosPayload(CSuplPosPayloadContent* aPosPayloadContent)
: iPosPayloadContent(aPosPayloadContent)
	{	
	}

CSuplPosPayload::~CSuplPosPayload()

	{
	}
	
void CSuplPosPayload::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	// PosPayLoad ::= CHOICE {
	//  tia801payload  OCTET STRING(SIZE (1..8192)),
	//  rrcPayload     OCTET STRING(SIZE (1..8192)),
	//  rrlpPayload    OCTET STRING(SIZE (1..8192)),
	//  ...}
	
	TBool isExtended(EFalse);
	if (iPosPayloadContent->iPayloadType == ESuplPosPayloadTia801||
		iPosPayloadContent->iPayloadType == ESuplPosPayloadRrc ||
		iPosPayloadContent->iPayloadType == ESuplPosPayloadRrlp)
		{
		isExtended=EFalse;	
		}
	else
		{
		User::Leave(KErrArgument);
		}

	if (iPosPayloadContent->iPayloadType != ESuplPosPayloadRrc)
		{
		User::Leave(KErrNotSupported);			
		}

	//Enocde the SUPL message cellinfo choice index
	CAsn1PerEncodingFragment* payloadChoiceEnc=CAsn1PerChoice::EncodeLC(aPerEncoding.iParentBitLength,
																	iPosPayloadContent->iPayloadType, 
																	2,
																	KAsn1PerExtensible,
																	isExtended);
	aPerEncoding.AddElementL(payloadChoiceEnc);	
	CleanupStack::PopAndDestroy(payloadChoiceEnc);


	switch (iPosPayloadContent->iPayloadType)
		{
	case ESuplPosPayloadTia801:
		// Encode the tia801payload
		User::Leave(KErrNotSupported);
		break;
		
	case ESuplPosPayloadRrc:
		{
		// Encode the rrcPayload		
		CAsn1PerEncodingFragment* rrcPayloadEnc= CAsn1PerOctetString::EncodeLC(aPerEncoding.iParentBitLength,
																			iPosPayloadContent->iPayload,
																			KAsn1LengthVariable,
																			KAsn1PerMaxConstrained,
																			1,
																			8192,
																			KAsn1PerNonExtensible,
																			KAsn1PerNonExtended,
																			KAsn1PerNoPadding);
		aPerEncoding.AddElementL(rrcPayloadEnc);	
		CleanupStack::PopAndDestroy(rrcPayloadEnc);			
		}
		break;
		
	case ESuplPosPayloadRrlp:
		// Encode the rrlpPayload	
		User::Leave(KErrNotSupported);			
		break;
		
	default:
		User::Leave(KErrArgument);
		}
	
	
	}

void CSuplPosPayload::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	// PosPayLoad ::= CHOICE {
	//  tia801payload  OCTET STRING(SIZE (1..8192)),
	//  rrcPayload     OCTET STRING(SIZE (1..8192)),
	//  rrlpPayload    OCTET STRING(SIZE (1..8192)),
	//  ...}
	
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	// Decode the payload type, which is extensible
	TInt payLoadType(0);
	CAsn1PerChoice::DecodeL(aBufferToDecode,
							startPos,
							2,
							KAsn1PerExtensible,
							bitsDecoded,
							isExtended,
							payLoadType	
							);
							
	iPosPayloadContent->iPayloadType=(TSuplPosPayloadType)payLoadType;
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	
	switch (iPosPayloadContent->iPayloadType)
		{
	case ESuplPosPayloadTia801:
		// Decode the tia801payload
		User::Leave(KErrNotSupported);	
		break;
		
	case ESuplPosPayloadRrc:
		// Decode the rrcPayload
		CAsn1PerOctetString::DecodeL(aBufferToDecode,
									startPos,
									KAsn1LengthVariable,
									KAsn1PerMaxConstrained,
									1,
									8192,	
									KAsn1PerNonExtensible,
									KAsn1PerNoPadding,
									isExtended,
									bitsDecoded, 
									iPosPayloadContent->iPayload
									);
		iPosPayloadContent->iPayloadOctetLength=iPosPayloadContent->iPayload.Length();
		
		// SUPL-issue. This code should be changed when SiRF fixes their SUPL server issue.
		//
		// A consequence of having to send a QoP in the START message (due to SiRF server bug)
		// the SUPL POS payload is not an RRC Assistance Data Delivery element but a Meassurement Control.
		// That means that Assistance Data starts 37 bits into the RRC payload.
		// Those 37 bits correpond to the encoding of (refer to RRC-ASN.1 encoding rules): DL-DCCH-MESSAGE, 
		// Meassurement Control, Later-than-r3, r4, Measurement Control-r4-IES, MeasurementControl-r4-IES, 
		// MeassurementCommand-r4, MeassurementType-r4, UE-POSITIONING-MEASUREMENT-r4,
		// UE-POSITIONING-GPS-ASSISTANCE-DATA.
		// Those 37 bits are: "22 80 06 42" and the first five bits of the subsequent "96".
		//
		iPosPayloadContent->iPayloadStartBitPos=(startPos+bitsDecoded-iPosPayloadContent->iPayloadOctetLength*8) + 37;
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;		
		break;
		
	case ESuplPosPayloadRrlp:
		// Decode the rrlpPayload	
		User::Leave(KErrNotSupported);			
		break;
		
	default:
		User::Leave(KErrArgument);
		}

	}
	
void CSuplPosPayload::ConstructL()
	{
	}	


/**
Implemetation of CSuplPosContent Class
*/
EXPORT_C CSuplPosContent* CSuplPosContent::NewLC()
	{
	CSuplPosContent* self = new (ELeave) CSuplPosContent();
	CleanupStack::PushL(self);
	return self;		
	}

EXPORT_C CSuplPosContent* CSuplPosContent::NewL()
	{
	CSuplPosContent* self = CSuplPosContent::NewLC();
	CleanupStack::Pop(self);
	return self;						
	}
	
EXPORT_C CSuplPosContent::~CSuplPosContent()
	{
	delete iPosPayloadContent;
	}

CSuplPosContent::CSuplPosContent()
	{	
	}

void CSuplPosContent::ConstructL()
	{		
	}

TBool CSuplPosContent::operator==(const CSuplPosContent& other) const
	{
	return (iVelocityPresent==other.iVelocityPresent &&
			*iPosPayloadContent==*other.iPosPayloadContent);
	}
	

/**
Implemetation of CSuplPos Class
*/

CSuplPos* CSuplPos::NewL(CSuplPosContent* aSuplPosContent)
	{
	CSuplPos* self = CSuplPos::NewLC(aSuplPosContent);
	CleanupStack::Pop(self);
	return self;						
	};
CSuplPos* CSuplPos::NewLC(CSuplPosContent* aSuplPosContent)
	{
	CSuplPos* self = new (ELeave) CSuplPos(aSuplPosContent);
	CleanupStack::PushL(self);
	return self;
	};
		
CSuplPos::CSuplPos(CSuplPosContent* aSuplPosContent) 
: iSuplPosContent(aSuplPosContent)
	{
	}

CSuplPos::~CSuplPos()
	{
	delete iPosPayload;	
	}

void CSuplPos::ConstructL()
	{
	iPosPayload=CSuplPosPayload::NewL(iSuplPosContent->iPosPayloadContent);
	};


void CSuplPos::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{	
	// SUPLPOS ::= SEQUENCE {
	//   posPayLoad  PosPayLoad,
	//   velocity    Velocity OPTIONAL,
	//  ...}
	
	
	// Still use iEncoding, since SUPL POS message is not an extended SUPL message
	
	// SUPL POS Specific part is extensible, encode "E" bit.	
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);
	
	
	//Encode OPTION BIT map	
	CAsn1PerEncodingFragment* opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, 
														iSuplPosContent->iVelocityPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);
	
	//Encode the Pos payload
	iPosPayload=CSuplPosPayload::NewL(iSuplPosContent->iPosPayloadContent);
	iPosPayload->EncodeL(aPerEncoding);
	
	if (iSuplPosContent->iVelocityPresent)
		{
		User::Leave(KErrNotSupported);
		}
	}
	
	
void CSuplPos::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	
	// SUPLPOS ::= SEQUENCE {
	//   posPayLoad  PosPayLoad,
	//   velocity    Velocity OPTIONAL,
	//  ...}


	// Common Part has been done when extracting the message type
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	//Decode "E" bit
	CAsn1PerSequence::DecodeL(aBufferToDecode,
							startPos,
							KAsn1PerExtensible,
							bitsDecoded,
							isExtended);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
							
	//Decode OPTION bit	map
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplPosContent->iVelocityPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	

	//Decode the Pos Payload
	iSuplPosContent->iPosPayloadContent=CSuplPosPayloadContent::NewL();
	iPosPayload=CSuplPosPayload::NewL(iSuplPosContent->iPosPayloadContent);
	iPosPayload->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	


	if (iSuplPosContent->iVelocityPresent)
		{
		User::Leave(KErrNotSupported);
		}			
	}



