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
// SUPL START - Implementation of SUPL START encoding & decoding.
// 
//

#include "suplstart.h"
#include "asn1per.h"


/**
Implemetation of CSuplStartContent Class
*/

EXPORT_C CSuplStartContent* CSuplStartContent::NewL()
	{
	CSuplStartContent* self = CSuplStartContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

EXPORT_C CSuplStartContent* CSuplStartContent::NewLC()
	{
	CSuplStartContent* self = new (ELeave) CSuplStartContent();
	CleanupStack::PushL(self);
	return self;		
	}
	
CSuplStartContent::CSuplStartContent()
	{
	
	}

void CSuplStartContent::ConstructL()
	{
	}
	
EXPORT_C CSuplStartContent::~CSuplStartContent()
	{
	delete iSetCapabilitiesContent;
	delete iLocationIdContent;
	delete iQoPContent;
	}

TBool CSuplStartContent::operator==(const CSuplStartContent& other) const
	{
	
	return (iQopPresent == other.iQopPresent && 
			*iSetCapabilitiesContent == *other.iSetCapabilitiesContent && 
			*iLocationIdContent==*other.iLocationIdContent &&
			*iQoPContent == *other.iQoPContent);			
	}	

	
/**
Implemetation of CSuplStart Class
*/

CSuplStart* CSuplStart::NewL(CSuplStartContent* aSuplStartContent)
	{
	CSuplStart* self = CSuplStart::NewLC(aSuplStartContent);
	CleanupStack::Pop(self);
	return self;					
	}

CSuplStart* CSuplStart::NewLC(CSuplStartContent* aSuplStartContent)
	{
	CSuplStart* self = new (ELeave) CSuplStart(aSuplStartContent);
	CleanupStack::PushL(self);
	return self;
	};
		
CSuplStart::CSuplStart(CSuplStartContent* aSuplStartContent) 
: iSuplStartContent(aSuplStartContent)
	{
	}

CSuplStart::~CSuplStart()
	{
	delete iSetCap;
	delete iLocationId;
	delete iQoP;
	}

void CSuplStart::ConstructL()
	{
	}

void CSuplStart::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
// SUPLSTART ::= SEQUENCE {
//  sETCapabilities  SETCapabilities,
//  locationId       LocationId,
//  qoP              QoP OPTIONAL,
//  ...}
	
	// SUPL START Specific part is extensible, encode "E" bit.	
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);
	
	//Encode the Qop OPTION BIT map	
	CAsn1PerEncodingFragment* opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iSuplStartContent->iQopPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);
	
	//Encode the SET Capabilities
	iSetCap=CSuplSetCapabilities::NewL(iSuplStartContent->iSetCapabilitiesContent);
	iSetCap->EncodeL(aPerEncoding);
	
	//Encode the Location ID
	iLocationId=CSuplLocationId::NewL(iSuplStartContent->iLocationIdContent);
	iLocationId->EncodeL(aPerEncoding);
	
	//Encode the QoP
	if (iSuplStartContent->iQopPresent)
		{
		iQoP = CSuplQoP::NewL(iSuplStartContent->iQoPContent);
		iQoP->EncodeL(aPerEncoding);
		}
	}
		
void CSuplStart::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
// SUPLSTART ::= SEQUENCE {
//  sETCapabilities  SETCapabilities,
//  locationId       LocationId,
//  qoP              QoP OPTIONAL,
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
							
	//the Qop OPTION bit	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplStartContent->iQopPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	
	//Decode the SET Capabilities
	iSuplStartContent->iSetCapabilitiesContent=CSuplSetCapabilitiesContent::NewL();
	iSetCap=CSuplSetCapabilities::NewL(iSuplStartContent->iSetCapabilitiesContent);	
	iSetCap->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	
	//Decode the Location ID
	iSuplStartContent->iLocationIdContent=CSuplLocationIdContent::NewL();
	iLocationId=CSuplLocationId::NewL(iSuplStartContent->iLocationIdContent);
	iLocationId->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode QoP if present
	if (iSuplStartContent->iQopPresent)
		{
		iSuplStartContent->iQoPContent = CSuplQoPContent::NewL();
		iQoP = CSuplQoP::NewL(iSuplStartContent->iQoPContent);
		iQoP->DecodeFromL(aBufferToDecode,startPos, bitsDecoded);
		}
	}


/**
Implemetation of CSuplCellInfo Class
*/
CSuplCellInfo* CSuplCellInfo::NewLC(CSuplCellInfoContent* aCellInfoContent)
	{
	CSuplCellInfo* self = new (ELeave) CSuplCellInfo(aCellInfoContent);
	CleanupStack::PushL(self);
	return self;		
	}


CSuplCellInfo* CSuplCellInfo::NewL(CSuplCellInfoContent* aCellInfoContent)
	{
	CSuplCellInfo* self = CSuplCellInfo::NewLC(aCellInfoContent);
	CleanupStack::Pop(self);
	return self;			
	}
	
CSuplCellInfo::CSuplCellInfo(CSuplCellInfoContent* aCellInfoContent)
: iCellInfoContent(aCellInfoContent)
	{	
	}

CSuplCellInfo::~CSuplCellInfo()
	{
	delete iGsmCellInformation;
	delete iWcdmaCellInformation;
	delete iCdmaCellInformation;
	}
	
void CSuplCellInfo::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	//CellInfo ::= CHOICE {
	//  gsmCell    GsmCellInformation,
	//  wcdmaCell  WcdmaCellInformation,
	//  cdmaCell   CdmaCellInformation,
	//  ...}

	TBool isExtended(EFalse);
	if (iCellInfoContent->iCellInformation==EGsmCellInformation ||
		iCellInfoContent->iCellInformation==EWcdmaCellInformation ||
		iCellInfoContent->iCellInformation==ECdmaCellInformation)
		{
		isExtended=EFalse;	
		}
	else
		{
		User::Leave(KErrArgument);
		}

	//Enocde the SUPL message cellinfo choice index
	CAsn1PerEncodingFragment* cellChoiceEnc=CAsn1PerChoice::EncodeLC(aPerEncoding.iParentBitLength,
																	iCellInfoContent->iCellInformation, 
																	2,
																	KAsn1PerExtensible,
																	isExtended);
	aPerEncoding.AddElementL(cellChoiceEnc);	
	CleanupStack::PopAndDestroy(cellChoiceEnc);

	switch (iCellInfoContent->iCellInformation)
		{
	case EGsmCellInformation:
	
		// Encode the GsmCellInfo
		iGsmCellInformation=CSuplGsmCellInformation::NewL(iCellInfoContent->iGsmCellInformationContent);
		iGsmCellInformation->EncodeL(aPerEncoding);	
		break;
		
	case EWcdmaCellInformation:
		// Encode the WcdmaCellInfo
		iWcdmaCellInformation=CSuplWcdmaCellInformation::NewL(iCellInfoContent->iWcdmaCellInformationContent);
		iWcdmaCellInformation->EncodeL(aPerEncoding);	
		break;
		
	case ECdmaCellInformation:
		// Encode the cdmaCellInfo	
		iCdmaCellInformation=CSuplCdmaCellInformation::NewL(iCellInfoContent->iCdmaCellInformationContent);
		iCdmaCellInformation->EncodeL(aPerEncoding);			
		break;
		
	default:
		User::Leave(KErrArgument);
		}
	}

void CSuplCellInfo::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//CellInfo ::= CHOICE {
	//  gsmCell    GsmCellInformation,
	//  wcdmaCell  WcdmaCellInformation,
	//  cdmaCell   CdmaCellInformation,
	//  ...}

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	// Decode the cell info Chioce, which is extensible
	TInt cellInfoType(0);
	CAsn1PerChoice::DecodeL(aBufferToDecode,
							startPos,
							2,
							KAsn1PerExtensible,
							bitsDecoded,
							isExtended,
							cellInfoType	
							);
	iCellInfoContent->iCellInformation=(TCellInformationChoice)cellInfoType;
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	
	switch (iCellInfoContent->iCellInformation)
		{
	case EGsmCellInformation:
		// Decode the GsmCellInfo
		iCellInfoContent->iGsmCellInformationContent=CSuplGsmCellInformationContent::NewL();
		iGsmCellInformation=CSuplGsmCellInformation::NewL(iCellInfoContent->iGsmCellInformationContent);
		iGsmCellInformation->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);	
		break;
		
	case EWcdmaCellInformation:
		// Decode the WcdmaCellInfo
		iCellInfoContent->iWcdmaCellInformationContent=CSuplWcdmaCellInformationContent::NewL();
		iWcdmaCellInformation=CSuplWcdmaCellInformation::NewL(iCellInfoContent->iWcdmaCellInformationContent);
		iWcdmaCellInformation->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		break;
		
	case ECdmaCellInformation:
		// Decode the cdmaCellInfo
		iCellInfoContent->iCdmaCellInformationContent=CSuplCdmaCellInformationContent::NewL();
		iCdmaCellInformation=CSuplCdmaCellInformation::NewL(iCellInfoContent->iCdmaCellInformationContent);
		iCdmaCellInformation->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);			
		break;
		
	default:
		User::Leave(KErrArgument);
		}
	
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	}
	
void CSuplCellInfo::ConstructL()
	{
	}	


/**
Implemetation of CSuplCellInfoContent Class
*/
EXPORT_C CSuplCellInfoContent* CSuplCellInfoContent::NewLC()
	{
	CSuplCellInfoContent* self = new (ELeave) CSuplCellInfoContent();
	CleanupStack::PushL(self);
	return self;				
	}

EXPORT_C CSuplCellInfoContent* CSuplCellInfoContent::NewL()
	{
	CSuplCellInfoContent* self = CSuplCellInfoContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}
	
EXPORT_C CSuplCellInfoContent::~CSuplCellInfoContent()
	{
	delete iGsmCellInformationContent;
	delete iWcdmaCellInformationContent;
	delete iCdmaCellInformationContent;
	}

void CSuplCellInfoContent::ConstructL()
	{
	}
	
CSuplCellInfoContent::CSuplCellInfoContent()
	{
		
	}
	
TBool CSuplCellInfoContent::operator==(const CSuplCellInfoContent& other) const
	{
	TBool ret(EFalse);
	
	if (iCellInformation==other.iCellInformation)
		{	
		switch (iCellInformation)
			{
		case EGsmCellInformation:
			ret=(*iGsmCellInformationContent==*other.iGsmCellInformationContent);
			break;
		case EWcdmaCellInformation:
			ret=(*iWcdmaCellInformationContent==*other.iWcdmaCellInformationContent);
			break;
		case ECdmaCellInformation:
			ret=(*iCdmaCellInformationContent==*other.iCdmaCellInformationContent);
			break;
		default:;
			}	
		}
		
	return ret;	
	}



/**
Implemetation of CSuplLocationId Class
*/

CSuplLocationId* CSuplLocationId::NewLC(CSuplLocationIdContent* aLocationIdContent)
	{
	CSuplLocationId* self = new (ELeave) CSuplLocationId(aLocationIdContent);
	CleanupStack::PushL(self);
	return self;		
	}

CSuplLocationId* CSuplLocationId::NewL(CSuplLocationIdContent* aLocationIdContent)
	{
	CSuplLocationId* self = CSuplLocationId::NewLC(aLocationIdContent);
	CleanupStack::Pop(self);
	return self;			
	}
	
CSuplLocationId::CSuplLocationId(CSuplLocationIdContent* aLocationIdContent)
: iLocationIdContent(aLocationIdContent)
	{	
	}

CSuplLocationId::~CSuplLocationId()
	{
	delete iCellInfo;
	}
	
void CSuplLocationId::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	// LocationId ::= SEQUENCE {cellInfo  CellInfo,
	//                         status    Status,
	//                         ...}
	
	// Status ::= ENUMERATED {stale(0), current(1), unknown(2), ...
    //                   }
	
	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);
	
	// Encode the CellInfo
	iCellInfo=CSuplCellInfo::NewL(iLocationIdContent->iCellInfoContent);
	iCellInfo->EncodeL(aPerEncoding);
	
	//Encode Status enum
	CAsn1PerEncodingFragment* status=CAsn1PerEnum::EncodeLC(aPerEncoding.iParentBitLength,
															iLocationIdContent->iLocationStatus,
															2,
															KAsn1PerExtensible,
															KAsn1PerNonExtended,
															KAsn1PerNoPadding);
	aPerEncoding.AddElementL(status);
	CleanupStack::PopAndDestroy(status);		
	}

void CSuplLocationId::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	
	// LocationId ::= SEQUENCE {cellInfo  CellInfo,
	//                         status    Status,
	//                         ...}
	
	// Status ::= ENUMERATED {stale(0), current(1), unknown(2), ...
    //                   }
	

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
	
	// Decode Cell info
	iLocationIdContent->iCellInfoContent=CSuplCellInfoContent::NewL();
	iCellInfo=CSuplCellInfo::NewL(iLocationIdContent->iCellInfoContent);
	iCellInfo->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	

	//Decode Status
	TInt chioceValue;
	CAsn1PerEnum::DecodeL(aBufferToDecode,
						startPos,
						2,
						KAsn1PerExtensible,
						KAsn1PerNoPadding,
						bitsDecoded,
						isExtended,
						chioceValue
						);
						
	iLocationIdContent->iLocationStatus=(TSuplLocationStatus)chioceValue;
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	
	}
	
void CSuplLocationId::ConstructL()
	{
	iCellInfo=CSuplCellInfo::NewL(iLocationIdContent->iCellInfoContent);
	}	


/**
Implemetation of CSuplLocationIdContent Class
*/
EXPORT_C CSuplLocationIdContent* CSuplLocationIdContent::NewLC()
	{
	CSuplLocationIdContent* self = new (ELeave) CSuplLocationIdContent();
	CleanupStack::PushL(self);
	return self;				
	}

EXPORT_C CSuplLocationIdContent* CSuplLocationIdContent::NewL()
	{
	CSuplLocationIdContent* self = CSuplLocationIdContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}
EXPORT_C CSuplLocationIdContent::~CSuplLocationIdContent()
	{
	delete iCellInfoContent;
	}

void CSuplLocationIdContent::ConstructL()
	{
	}
	
CSuplLocationIdContent::CSuplLocationIdContent()
	{
		
	}

TBool CSuplLocationIdContent::operator==(const CSuplLocationIdContent& other) const
	{
	return (iLocationStatus==other.iLocationStatus && 
			*iCellInfoContent==*other.iCellInfoContent);
	}



/**
Implemetation of CSuplSetCapabilities Class
*/
CSuplSetCapabilities* CSuplSetCapabilities::NewLC(CSuplSetCapabilitiesContent* aSetCapabilitiesContent)
	{
	CSuplSetCapabilities* self = new (ELeave) CSuplSetCapabilities(aSetCapabilitiesContent);
	CleanupStack::PushL(self);
	return self;				
	}

CSuplSetCapabilities* CSuplSetCapabilities::NewL(CSuplSetCapabilitiesContent* aSetCapabilitiesContent)
	{
	CSuplSetCapabilities* self = CSuplSetCapabilities::NewLC(aSetCapabilitiesContent);
	CleanupStack::Pop(self);
	return self;							
	}

CSuplSetCapabilities::~CSuplSetCapabilities()
	{
	delete iPosTechnology;
	delete iPosProtocol;		
	}
	
void CSuplSetCapabilities::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	// SETCapabilities ::= SEQUENCE {
	//  posTechnology  PosTechnology,
	//  prefMethod     PrefMethod,
	//  posProtocol    PosProtocol,
	//  ...}
	
	// PrefMethod ::= ENUMERATED {
	//  agpsSETassistedPreferred, agpsSETBasedPreferred, noPreference}
	

	// Encode "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);
	
	// Encode Pos technology
	iPosTechnology=CSuplPosTechnology::NewL(iSetCapabilitiesContent->iPosTechnologyContent);
	iPosTechnology->EncodeL(aPerEncoding);

	// Encode Pos method
	CAsn1PerEncodingFragment* posMethod=CAsn1PerEnum::EncodeLC(aPerEncoding.iParentBitLength,
															iSetCapabilitiesContent->iPrefMethod,
															2,
															KAsn1PerNonExtensible,
															KAsn1PerNonExtended,
															KAsn1PerNoPadding);
	aPerEncoding.AddElementL(posMethod);
	CleanupStack::PopAndDestroy(posMethod);
	
	// Encode Pos protocol
	iPosProtocol=CSuplPosProtocol::NewL(iSetCapabilitiesContent->iPosProtocolContent);
	iPosProtocol->EncodeL(aPerEncoding);
	}

void CSuplSetCapabilities::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{

	// SETCapabilities ::= SEQUENCE {
	//  posTechnology  PosTechnology,
	//  prefMethod     PrefMethod,
	//  posProtocol    PosProtocol,
	//  ...}
	
	// PrefMethod ::= ENUMERATED {
	//  agpsSETassistedPreferred, agpsSETBasedPreferred, noPreference}


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
	
	// Decode pos technology
	iSetCapabilitiesContent->iPosTechnologyContent=CSuplPosTechnologyContent::NewL();
	iPosTechnology=CSuplPosTechnology::NewL(iSetCapabilitiesContent->iPosTechnologyContent);
	iPosTechnology->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	

	//Decode pos method
	TInt posMethod;
	CAsn1PerEnum::DecodeL(aBufferToDecode,
						startPos,
						2,
						KAsn1PerNonExtensible,
						KAsn1PerNoPadding,
						bitsDecoded,
						isExtended,
						posMethod
						);
						
	iSetCapabilitiesContent->iPrefMethod=(TSuplPrefMethod)posMethod;
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	

	//Decode Pos protocol
	iSetCapabilitiesContent->iPosProtocolContent=CSuplPosProtocolContent::NewL();
	iPosProtocol=CSuplPosProtocol::NewL(iSetCapabilitiesContent->iPosProtocolContent);
	iPosProtocol->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;		
	}
	
void CSuplSetCapabilities::ConstructL()
	{
	}
	
CSuplSetCapabilities::CSuplSetCapabilities(CSuplSetCapabilitiesContent* aSetCapabilitiesContent)
: iSetCapabilitiesContent(aSetCapabilitiesContent)
	{
	}


/**
Implemetation of CSuplSetCapabilitiesContent Class
*/
EXPORT_C CSuplSetCapabilitiesContent* CSuplSetCapabilitiesContent::NewLC()
	{
	CSuplSetCapabilitiesContent* self = new (ELeave) CSuplSetCapabilitiesContent();
	CleanupStack::PushL(self);
	return self;						
	}

EXPORT_C CSuplSetCapabilitiesContent* CSuplSetCapabilitiesContent::NewL()
	{
	CSuplSetCapabilitiesContent* self = CSuplSetCapabilitiesContent::NewLC();
	CleanupStack::Pop(self);
	return self;							
		
	}

void CSuplSetCapabilitiesContent::ConstructL()
	{
	}

EXPORT_C CSuplSetCapabilitiesContent::~CSuplSetCapabilitiesContent()
	{
	delete iPosTechnologyContent;
	delete iPosProtocolContent;
	}

TBool CSuplSetCapabilitiesContent::operator==(const CSuplSetCapabilitiesContent& other) const
	{
	return (*iPosTechnologyContent == *other.iPosTechnologyContent && 
			*iPosProtocolContent==*other.iPosProtocolContent &&
			iPrefMethod==other.iPrefMethod);
	}

/**
Implemetation of CSuplPosProtocol Class
*/

CSuplPosProtocol* CSuplPosProtocol::NewLC(CSuplPosProtocolContent* aPosProtocolContent)
	{
	CSuplPosProtocol* self = new (ELeave) CSuplPosProtocol(aPosProtocolContent);
	CleanupStack::PushL(self);
	return self;						
	}

CSuplPosProtocol* CSuplPosProtocol::NewL(CSuplPosProtocolContent* aPosProtocolContent)
	{
	CSuplPosProtocol* self = CSuplPosProtocol::NewLC(aPosProtocolContent);
	CleanupStack::Pop(self);
	return self;									
	}


void CSuplPosProtocol::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	// PosProtocol ::= SEQUENCE {
	//  tia801  BOOLEAN,
	//  rrlp    BOOLEAN,
	//  rrc     BOOLEAN,
	// ...}

	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);

	// Encode iTia801
	CAsn1PerEncodingFragment* enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosProtocolContent->iTia801, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode iRrlp
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosProtocolContent->iRrlp, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode iRrc
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosProtocolContent->iRrc, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);

		
	}

void CSuplPosProtocol::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	// PosProtocol ::= SEQUENCE {
	//  tia801  BOOLEAN,
	//  rrlp    BOOLEAN,
	//  rrc     BOOLEAN,
	// ...}


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
	
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosProtocolContent->iTia801);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
							
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosProtocolContent->iRrlp);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosProtocolContent->iRrc);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	}
	
CSuplPosProtocol::CSuplPosProtocol(CSuplPosProtocolContent* aPosProtocolContent)
	:iPosProtocolContent(aPosProtocolContent)
	{	
	}
	
CSuplPosProtocol::~CSuplPosProtocol()
	{	
	}
	
	
/**
Implemetation of CSuplPosProtocolContent Class
*/
EXPORT_C CSuplPosProtocolContent* CSuplPosProtocolContent::NewLC()
	{
	CSuplPosProtocolContent* self = new (ELeave) CSuplPosProtocolContent();
	CleanupStack::PushL(self);
	return self;								
	}

EXPORT_C CSuplPosProtocolContent* CSuplPosProtocolContent::NewL()
	{
	CSuplPosProtocolContent* self = CSuplPosProtocolContent::NewLC();
	CleanupStack::Pop(self);
	return self;											
	}

CSuplPosProtocolContent::CSuplPosProtocolContent()
	{
		
	}
	
EXPORT_C CSuplPosProtocolContent::~CSuplPosProtocolContent()
	{
	}	

TBool CSuplPosProtocolContent::operator==(const CSuplPosProtocolContent& other) const
	{
	return (iTia801==other.iTia801 && 
			iRrlp==other.iRrlp &&
			iRrc==other.iRrc);
	}


/**
Implemetation of CSuplWcdmaCellInformation Class
*/
CSuplWcdmaCellInformation* CSuplWcdmaCellInformation::NewL(CSuplWcdmaCellInformationContent* aWcdmaCellInformationContent)
	{
	CSuplWcdmaCellInformation* self = CSuplWcdmaCellInformation::NewLC(aWcdmaCellInformationContent);
	CleanupStack::Pop(self);
	return self;													
	}

CSuplWcdmaCellInformation* CSuplWcdmaCellInformation::NewLC(CSuplWcdmaCellInformationContent* aWcdmaCellInformationContent)
	{
	CSuplWcdmaCellInformation* self = new (ELeave) CSuplWcdmaCellInformation(aWcdmaCellInformationContent);
	CleanupStack::PushL(self);
	return self;										
	}

	
CSuplWcdmaCellInformation::~CSuplWcdmaCellInformation()
	{
	// We do not support this options for now.	
	//delete iFrequencyInfo;
	//delete iMeasuredResultsList;
	}
void CSuplWcdmaCellInformation::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	//WcdmaCellInformation ::= SEQUENCE {
	//  refMCC                 INTEGER(0..999), -- Mobile Country Code
	//  refMNC                 INTEGER(0..999), -- Mobile Network Code
	//  refUC                  INTEGER(0..268435455), -- Cell identity
	//  frequencyInfo          FrequencyInfo OPTIONAL,
	//  primaryScramblingCode  INTEGER(0..511) OPTIONAL,
	//  measuredResultsList    MeasuredResultsList OPTIONAL,
	//  ...}
	
	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);

	if (iWcdmaCellInformationContent->iFrequencyInfoPresent || 
		iWcdmaCellInformationContent->iPrimaryScramblingCodePresent || 
		iWcdmaCellInformationContent->iMeasuredResultsListPresent)
		{
		User::Leave(KErrNotSupported);	
		}
	
	// Option bit map
	CAsn1PerEncodingFragment* opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iWcdmaCellInformationContent->iFrequencyInfoPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);
	
	opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iWcdmaCellInformationContent->iPrimaryScramblingCodePresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

	opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iWcdmaCellInformationContent->iMeasuredResultsListPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);
	
	//Encode the refMCC
	CAsn1PerEncodingFragment* refMCC=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iWcdmaCellInformationContent->iRefMCC, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														999, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refMCC);
	CleanupStack::PopAndDestroy(refMCC);

	//Encode the refMNC
	CAsn1PerEncodingFragment* refMNC=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iWcdmaCellInformationContent->iRefMNC, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														999, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refMNC);
	CleanupStack::PopAndDestroy(refMNC);

	//Encode the refUC
	CAsn1PerEncodingFragment* refUC=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iWcdmaCellInformationContent->iRefUC, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														268435455, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refUC);
	CleanupStack::PopAndDestroy(refUC);	
			
	}

void CSuplWcdmaCellInformation::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//WcdmaCellInformation ::= SEQUENCE {
	//  refMCC                 INTEGER(0..999), -- Mobile Country Code
	//  refMNC                 INTEGER(0..999), -- Mobile Network Code
	//  refUC                  INTEGER(0..268435455), -- Cell identity
	//  frequencyInfo          FrequencyInfo OPTIONAL,
	//  primaryScramblingCode  INTEGER(0..511) OPTIONAL,
	//  measuredResultsList    MeasuredResultsList OPTIONAL,
	//  ...}

			
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
	
	//Decode option bit map
	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iWcdmaCellInformationContent->iFrequencyInfoPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iWcdmaCellInformationContent->iPrimaryScramblingCodePresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;


	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iWcdmaCellInformationContent->iMeasuredResultsListPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	if (iWcdmaCellInformationContent->iFrequencyInfoPresent || 
		iWcdmaCellInformationContent->iPrimaryScramblingCodePresent || 
		iWcdmaCellInformationContent->iMeasuredResultsListPresent)
		{
		User::Leave(KErrNotSupported);	
		}	
	
	//Decode RefMCC
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						999,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iWcdmaCellInformationContent->iRefMCC);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;
	
	//Decode RefMNC
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						999,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iWcdmaCellInformationContent->iRefMNC);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;

	//Decode refUC
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						268435455,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iWcdmaCellInformationContent->iRefUC);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;

	}
	
void CSuplWcdmaCellInformation::ConstructL()
	{
	}

CSuplWcdmaCellInformation::CSuplWcdmaCellInformation(CSuplWcdmaCellInformationContent* aWcdmaCellInformationContent)
:iWcdmaCellInformationContent(aWcdmaCellInformationContent)
	{
	}	
	
/**
Implemetation of CSuplWcdmaCellInformationContent Class
*/
EXPORT_C CSuplWcdmaCellInformationContent* CSuplWcdmaCellInformationContent::NewLC()
	{
	CSuplWcdmaCellInformationContent* self = new (ELeave) CSuplWcdmaCellInformationContent();
	CleanupStack::PushL(self);
	//self->ConstructL();	
	return self;												
	}

EXPORT_C CSuplWcdmaCellInformationContent* CSuplWcdmaCellInformationContent::NewL()
	{
	CSuplWcdmaCellInformationContent* self = CSuplWcdmaCellInformationContent::NewLC();
	CleanupStack::Pop(self);
	return self;															
	}
	
EXPORT_C CSuplWcdmaCellInformationContent::~CSuplWcdmaCellInformationContent()
	{
		
	}

void CSuplWcdmaCellInformationContent::ConstructL()
	{
	}

TBool CSuplWcdmaCellInformationContent::operator==(const CSuplWcdmaCellInformationContent& other) const
	{
	return (iFrequencyInfoPresent==other.iFrequencyInfoPresent && 
			iPrimaryScramblingCodePresent==other.iPrimaryScramblingCodePresent &&
			iMeasuredResultsListPresent==other.iMeasuredResultsListPresent &&
			iRefMCC==other.iRefMCC &&
			iRefMNC==other.iRefMNC &&
			iRefUC==other.iRefUC);
	}


/**
Implemetation of CSuplGsmCellInformation Class
*/

CSuplGsmCellInformation* CSuplGsmCellInformation::NewLC(CSuplGsmCellInformationContent* aGsmCellInformationContent)
	{
	CSuplGsmCellInformation* self = new (ELeave) CSuplGsmCellInformation(aGsmCellInformationContent);
	CleanupStack::PushL(self);	
	return self;										
	}

CSuplGsmCellInformation* CSuplGsmCellInformation::NewL(CSuplGsmCellInformationContent* aGsmCellInformationContent)
	{
	CSuplGsmCellInformation* self = CSuplGsmCellInformation::NewLC(aGsmCellInformationContent);
	CleanupStack::Pop(self);
	return self;													
	}
CSuplGsmCellInformation::~CSuplGsmCellInformation()
	{
	}
void CSuplGsmCellInformation::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	//GsmCellInformation ::= SEQUENCE {
	//  refMCC  INTEGER(0..999), -- Mobile Country Code
	//  refMNC  INTEGER(0..999), -- Mobile Network Code
	//  refLAC  INTEGER(0..65535), -- Location area code
	//  refCI   INTEGER(0..65535), -- Cell identity
	//  nMR     NMR OPTIONAL,
	//  tA      INTEGER(0..255) OPTIONAL, --Timing Advance
	//  ...}


	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);

	if (iGsmCellInformationContent->iNMRPresent || iGsmCellInformationContent->iTAPresent)
		{
		User::Leave(KErrNotSupported);	
		}
	
	// Option bit map
	CAsn1PerEncodingFragment* nmrOpt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iGsmCellInformationContent->iNMRPresent);
	aPerEncoding.AddElementL(nmrOpt);
	CleanupStack::PopAndDestroy(nmrOpt);

	CAsn1PerEncodingFragment* taOpt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iGsmCellInformationContent->iTAPresent);
	aPerEncoding.AddElementL(taOpt);
	CleanupStack::PopAndDestroy(taOpt);

	//Encode the refMCC
	CAsn1PerEncodingFragment* refMCC=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iGsmCellInformationContent->iRefMCC, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														999, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refMCC);
	CleanupStack::PopAndDestroy(refMCC);

	//Encode the refMNC
	CAsn1PerEncodingFragment* refMNC=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iGsmCellInformationContent->iRefMNC, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														999, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refMNC);
	CleanupStack::PopAndDestroy(refMNC);

	//Encode the refLAC
	CAsn1PerEncodingFragment* refLAC=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iGsmCellInformationContent->iRefLAC, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														65535, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refLAC);
	CleanupStack::PopAndDestroy(refLAC);

	//Encode the refCI
	CAsn1PerEncodingFragment* refCI=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iGsmCellInformationContent->iRefCI, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														65535, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refCI);
	CleanupStack::PopAndDestroy(refCI);
		
	}

void CSuplGsmCellInformation::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//GsmCellInformation ::= SEQUENCE {
	//  refMCC  INTEGER(0..999), -- Mobile Country Code
	//  refMNC  INTEGER(0..999), -- Mobile Network Code
	//  refLAC  INTEGER(0..65535), -- Location area code
	//  refCI   INTEGER(0..65535), -- Cell identity
	//  nMR     NMR OPTIONAL,
	//  tA      INTEGER(0..255) OPTIONAL, --Timing Advance
	//  ...}
		
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
	
	//Decode option bit map
	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGsmCellInformationContent->iNMRPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGsmCellInformationContent->iTAPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	
	//Decode RefMCC
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						999,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iGsmCellInformationContent->iRefMCC);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;
	
	//Decode RefMNC
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						999,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iGsmCellInformationContent->iRefMNC);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;

	//Decode RefLAC
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						65535,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iGsmCellInformationContent->iRefLAC);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;

	//Decode RefCI
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						65535,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iGsmCellInformationContent->iRefCI);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;	
	
	if (iGsmCellInformationContent->iNMRPresent || iGsmCellInformationContent->iTAPresent)
		{
		User::Leave(KErrNotSupported);	
		}	
	}
	
void CSuplGsmCellInformation::ConstructL()
	{
	}

CSuplGsmCellInformation::CSuplGsmCellInformation(CSuplGsmCellInformationContent* aGsmCellInformationContent)
:iGsmCellInformationContent(aGsmCellInformationContent)
	{
	}	
	
/**
Implemetation of CSuplGsmCellInformationContent Class
*/
EXPORT_C CSuplGsmCellInformationContent* CSuplGsmCellInformationContent::NewLC()
	{
	CSuplGsmCellInformationContent* self = new (ELeave) CSuplGsmCellInformationContent();
	CleanupStack::PushL(self);
	//self->ConstructL();
	return self;												
	}

EXPORT_C CSuplGsmCellInformationContent* CSuplGsmCellInformationContent::NewL()
	{
	CSuplGsmCellInformationContent* self = CSuplGsmCellInformationContent::NewLC();
	CleanupStack::Pop(self);
	return self;															
	}


void CSuplGsmCellInformationContent::ConstructL()
	{
	}

EXPORT_C CSuplGsmCellInformationContent::~CSuplGsmCellInformationContent()
	{
	}
	
CSuplGsmCellInformationContent::CSuplGsmCellInformationContent()
	{
	}	


TBool CSuplGsmCellInformationContent::operator==(const CSuplGsmCellInformationContent& other) const
	{
	return (iNMRPresent==other.iNMRPresent && 
			iTAPresent==other.iTAPresent &&
			iRefMCC==other.iRefMCC &&
			iRefMNC==other.iRefMNC &&
			iRefLAC==other.iRefLAC &&
			iRefCI==other.iRefCI);
	}
	
/**
Implemetation of CSuplcdmaCellInformation Class
*/

CSuplCdmaCellInformation* CSuplCdmaCellInformation::NewLC(CSuplCdmaCellInformationContent* aCdmaCellInformationContent)
	{
	CSuplCdmaCellInformation* self = new (ELeave) CSuplCdmaCellInformation(aCdmaCellInformationContent);
	CleanupStack::PushL(self);	
	return self;										
	}

CSuplCdmaCellInformation* CSuplCdmaCellInformation::NewL(CSuplCdmaCellInformationContent* aCdmaCellInformationContent)
	{
	CSuplCdmaCellInformation* self = CSuplCdmaCellInformation::NewLC(aCdmaCellInformationContent);
	CleanupStack::Pop(self);
	return self;													
	}
	
CSuplCdmaCellInformation::~CSuplCdmaCellInformation()
	{
	}
void CSuplCdmaCellInformation::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	// CdmaCellInformation ::= SEQUENCE {
	//  refNID         INTEGER(0..65535), -- Network Id
	//  refSID         INTEGER(0..32767), -- System Id
	//  refBASEID      INTEGER(0..65535), -- Base Station Id
	//  refBASELAT     INTEGER(0..4194303), -- Base Station Latitude
	//  reBASELONG     INTEGER(0..8388607), -- Base Station Longitude
	//  refREFPN       INTEGER(0..511), -- Base Station PN Code
	//  refWeekNumber  INTEGER(0..65535), -- GPS Week Number
	//  refSeconds     INTEGER(0..4194303)-- GPS Seconds -- ,
	//  ...}

	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);
		
	//Encode the refNID
	CAsn1PerEncodingFragment* refNID=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iCdmaCellInformationContent->iRefNID, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														65535, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refNID);
	CleanupStack::PopAndDestroy(refNID);

	//Encode the refSID
	CAsn1PerEncodingFragment* refSID=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iCdmaCellInformationContent->iRefSID, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														32767, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refSID);
	CleanupStack::PopAndDestroy(refSID);

	//Encode the refBASEID
	CAsn1PerEncodingFragment* refBASEID=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iCdmaCellInformationContent->iRefBASEID, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														65535, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refBASEID);
	CleanupStack::PopAndDestroy(refBASEID);	

	//Encode the refBASEID
	CAsn1PerEncodingFragment* refBASELAT=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iCdmaCellInformationContent->iRefBASELAT, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														4194303, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refBASELAT);
	CleanupStack::PopAndDestroy(refBASELAT);	

	//Encode the reBASELONG
	CAsn1PerEncodingFragment* reBASELONG=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iCdmaCellInformationContent->iReBASELONG, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														8388607, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(reBASELONG);
	CleanupStack::PopAndDestroy(reBASELONG);	

	//Encode the refREFPN
	CAsn1PerEncodingFragment* refREFPN=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iCdmaCellInformationContent->iRefREFPN, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														511, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refREFPN);
	CleanupStack::PopAndDestroy(refREFPN);
	
	//Encode the refWeekNumber
	CAsn1PerEncodingFragment* refWeekNumber=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iCdmaCellInformationContent->iRefWeekNumber, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														65535, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refWeekNumber);
	CleanupStack::PopAndDestroy(refWeekNumber);
	
	//Encode the refSeconds
	CAsn1PerEncodingFragment* refSeconds=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iCdmaCellInformationContent->iRefSeconds, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														4194303, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(refSeconds);
	CleanupStack::PopAndDestroy(refSeconds);		
	}

void CSuplCdmaCellInformation::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	// CdmaCellInformation ::= SEQUENCE {
	//  refNID         INTEGER(0..65535), -- Network Id
	//  refSID         INTEGER(0..32767), -- System Id
	//  refBASEID      INTEGER(0..65535), -- Base Station Id
	//  refBASELAT     INTEGER(0..4194303), -- Base Station Latitude
	//  reBASELONG     INTEGER(0..8388607), -- Base Station Longitude
	//  refREFPN       INTEGER(0..511), -- Base Station PN Code
	//  refWeekNumber  INTEGER(0..65535), -- GPS Week Number
	//  refSeconds     INTEGER(0..4194303)-- GPS Seconds -- ,
	//  ...}

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
	
	//Decode refNID
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						65535,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iCdmaCellInformationContent->iRefNID);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;
	
	//Decode refSID
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						32767,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iCdmaCellInformationContent->iRefSID);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;

	//Decode refBASEID
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						65535,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iCdmaCellInformationContent->iRefBASEID);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;

	//Decode refBASELAT
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						4194303,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iCdmaCellInformationContent->iRefBASELAT);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;	

	//Decode reBASELONG
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						8388607,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iCdmaCellInformationContent->iReBASELONG);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;	

	//Decode refREFPN
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						511,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iCdmaCellInformationContent->iRefREFPN);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;	

	//Decode refWeekNumber
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						65535,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iCdmaCellInformationContent->iRefWeekNumber);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;	

	//Decode refSeconds
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						4194303,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iCdmaCellInformationContent->iRefSeconds);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;
	}
	
void CSuplCdmaCellInformation::ConstructL()
	{
	}

CSuplCdmaCellInformation::CSuplCdmaCellInformation(CSuplCdmaCellInformationContent* aCdmaCellInformationContent)
:iCdmaCellInformationContent(aCdmaCellInformationContent)
	{
	}	
	
/**
Implemetation of CSuplCdmaCellInformationContent Class
*/
EXPORT_C CSuplCdmaCellInformationContent* CSuplCdmaCellInformationContent::NewLC()
	{
	CSuplCdmaCellInformationContent* self = new (ELeave) CSuplCdmaCellInformationContent();
	CleanupStack::PushL(self);
	return self;												
	}

EXPORT_C CSuplCdmaCellInformationContent* CSuplCdmaCellInformationContent::NewL()
	{
	CSuplCdmaCellInformationContent* self = CSuplCdmaCellInformationContent::NewLC();
	CleanupStack::Pop(self);
	return self;															
	}


void CSuplCdmaCellInformationContent::ConstructL()
	{
	}

EXPORT_C CSuplCdmaCellInformationContent::~CSuplCdmaCellInformationContent()
	{		
	}

CSuplCdmaCellInformationContent::CSuplCdmaCellInformationContent()
	{		
	}

TBool CSuplCdmaCellInformationContent::operator==(const CSuplCdmaCellInformationContent& other) const
	{
	return (iRefNID==other.iRefNID && 
			iRefSID==other.iRefSID &&
			iRefBASEID==other.iRefBASEID &&
			iRefBASELAT==other.iRefBASELAT &&
			iReBASELONG==other.iReBASELONG &&
			iRefREFPN==other.iRefREFPN &&
			iRefWeekNumber==other.iRefWeekNumber &&
			iRefSeconds==other.iRefSeconds);
	}


/**
Implemetation of CSuplPosTechnology Class
*/

CSuplPosTechnology* CSuplPosTechnology::NewLC(CSuplPosTechnologyContent* aSuplPosTechnologyContent)
	{
	CSuplPosTechnology* self = new (ELeave) CSuplPosTechnology(aSuplPosTechnologyContent);
	CleanupStack::PushL(self);
	return self;														
	}

CSuplPosTechnology* CSuplPosTechnology::NewL(CSuplPosTechnologyContent* aSuplPosTechnologyContent)
	{
	CSuplPosTechnology* self = CSuplPosTechnology::NewLC(aSuplPosTechnologyContent);
	CleanupStack::Pop(self);
	return self;																	
	}


void CSuplPosTechnology::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{	
	// PosTechnology ::= SEQUENCE {
	//  agpsSETassisted  BOOLEAN,
	//  agpsSETBased     BOOLEAN,
	//  autonomousGPS    BOOLEAN,
	//  aFLT             BOOLEAN,
	//  eCID             BOOLEAN,
	//  eOTD             BOOLEAN,
	//  oTDOA            BOOLEAN,
	//  ...}
		
	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);

	// Encode AgpsSETassisted
	CAsn1PerEncodingFragment* enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosTechnologyContent->iAgpsSETassisted, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode AgpsSETBased
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosTechnologyContent->iAgpsSETBased, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode iAutonomousGPS
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosTechnologyContent->iAutonomousGPS, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode iAFLT
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosTechnologyContent->iAFLT, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);

	// Encode iECID
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosTechnologyContent->iECID, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
		
	// Encode iEOTD
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosTechnologyContent->iEOTD, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode iOTDOA
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iPosTechnologyContent->iOTDOA, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	}

void CSuplPosTechnology::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	
	// PosTechnology ::= SEQUENCE {
	//  agpsSETassisted  BOOLEAN,
	//  agpsSETBased     BOOLEAN,
	//  autonomousGPS    BOOLEAN,
	//  aFLT             BOOLEAN,
	//  eCID             BOOLEAN,
	//  eOTD             BOOLEAN,
	//  oTDOA            BOOLEAN,
	//  ...}
	
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
	
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosTechnologyContent->iAgpsSETassisted);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
							
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosTechnologyContent->iAgpsSETBased);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosTechnologyContent->iAutonomousGPS);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
							
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosTechnologyContent->iAFLT);							
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
							
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosTechnologyContent->iECID);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
	
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosTechnologyContent->iEOTD);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iPosTechnologyContent->iOTDOA);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	}
	
void CSuplPosTechnology::ConstructL()
	{
	}
	
CSuplPosTechnology::CSuplPosTechnology(CSuplPosTechnologyContent* aSuplPosTechnologyContent)
:iPosTechnologyContent(aSuplPosTechnologyContent)
	{
	}

CSuplPosTechnology::~CSuplPosTechnology()
	{
		
	}
	
/**
Implemetation of CSuplWcdmaCellInformationContent Class
*/
EXPORT_C CSuplPosTechnologyContent* CSuplPosTechnologyContent::NewLC()
	{
	CSuplPosTechnologyContent* self = new (ELeave) CSuplPosTechnologyContent();
	CleanupStack::PushL(self);
	return self;																
	}

EXPORT_C CSuplPosTechnologyContent* CSuplPosTechnologyContent::NewL()
	{
	CSuplPosTechnologyContent* self = CSuplPosTechnologyContent::NewLC();
	CleanupStack::Pop(self);
	return self;																	
	}

CSuplPosTechnologyContent::CSuplPosTechnologyContent()
	{
	}


EXPORT_C CSuplPosTechnologyContent::~CSuplPosTechnologyContent()
	{
	}
	
void CSuplPosTechnologyContent::ConstructL()
	{
	}

TBool CSuplPosTechnologyContent::operator==(const CSuplPosTechnologyContent& other) const
	{
	return (iAgpsSETassisted==other.iAgpsSETassisted && 
			iAgpsSETBased==other.iAgpsSETBased &&
			iAutonomousGPS==other.iAutonomousGPS &&
			iAFLT==other.iAFLT &&
			iECID==other.iECID &&
			iEOTD==other.iEOTD &&
			iOTDOA==other.iOTDOA);
	}



/**
Implemetation of CSuplQoP Class
*/
CSuplQoP* CSuplQoP::NewLC(CSuplQoPContent* aQoPContent)
	{
	CSuplQoP* self = new (ELeave) CSuplQoP(aQoPContent);
	CleanupStack::PushL(self);
	return self;		
	}

CSuplQoP* CSuplQoP::NewL(CSuplQoPContent* aQoPContent)
	{
	CSuplQoP* self = CSuplQoP::NewLC(aQoPContent);
	CleanupStack::Pop(self);
	return self;			
	}
	
CSuplQoP::CSuplQoP(CSuplQoPContent* aQoPContent)
: iQoPContent(aQoPContent)
	{	
	}

CSuplQoP::~CSuplQoP()
	{
	}
	
void CSuplQoP::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	
	// QoP ::= SEQUENCE {
  	//	horacc     INTEGER(0..127),
  	//	veracc     INTEGER(0..127) OPTIONAL, -- as defined in 3GPP TS 23.032 "uncertainty altitude"-
  	//	maxLocAge  INTEGER(0..65535) OPTIONAL,
  	//	delay      INTEGER(0..7) OPTIONAL, -- as defined in 3GPP TS 44.031
  	//	...}
    // 
	
	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);
	
	// Encode the Options bit map
	CAsn1PerEncodingFragment* veraccOpt = CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iQoPContent->iVeraccPresent);
	aPerEncoding.AddElementL(veraccOpt);
	CleanupStack::PopAndDestroy(veraccOpt);
	
	CAsn1PerEncodingFragment* maxLocAgeOpt = CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iQoPContent->iMaxLocAgePresent);
	aPerEncoding.AddElementL(maxLocAgeOpt);
	CleanupStack::PopAndDestroy(maxLocAgeOpt);
	
	CAsn1PerEncodingFragment* delayOpt = CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iQoPContent->iDelayPresent);
	aPerEncoding.AddElementL(delayOpt);
	CleanupStack::PopAndDestroy(delayOpt);
	
	//Encode horizontal accuracy (horacc)
	CAsn1PerEncodingFragment* horacc=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iQoPContent->iHoracc, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														127, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(horacc);
	CleanupStack::PopAndDestroy(horacc);		
	}

void CSuplQoP::DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits)
	{
	
	// QoP ::= SEQUENCE {
  	//	horacc     INTEGER(0..127),
  	//	veracc     INTEGER(0..127) OPTIONAL, -- as defined in 3GPP TS 23.032 "uncertainty altitude"-
  	//	maxLocAge  INTEGER(0..65535) OPTIONAL,
  	//	delay      INTEGER(0..7) OPTIONAL, -- as defined in 3GPP TS 44.031
  	//	...}
    // 
	
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
							
	//the verac OPTION bit	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iQoPContent->iVeraccPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	
	//the maxLocAge OPTION bit	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iQoPContent->iMaxLocAgePresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	//the maxLocAge OPTION bit	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iQoPContent->iDelayPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// decode horizontal accuracy
	//
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						127,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iQoPContent->iHoracc);
	aDecodedBits+=bitsDecoded;					
	startPos+=bitsDecoded;
	}


void CSuplQoP::ConstructL()
	{
	}	


/**
Implemetation of CSuplQoPContent Class
*/
EXPORT_C CSuplQoPContent* CSuplQoPContent::NewLC()
	{
	CSuplQoPContent* self = new (ELeave) CSuplQoPContent();
	CleanupStack::PushL(self);
	return self;				
	}

EXPORT_C CSuplQoPContent* CSuplQoPContent::NewL()
	{
	CSuplQoPContent* self = CSuplQoPContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}
EXPORT_C CSuplQoPContent::~CSuplQoPContent()
	{
	}

void CSuplQoPContent::ConstructL()
	{
	}

CSuplQoPContent::CSuplQoPContent()
	{
	}

TBool CSuplQoPContent::operator==(const CSuplQoPContent& other) const
	{
	return (iHoracc==other.iHoracc);
	}




