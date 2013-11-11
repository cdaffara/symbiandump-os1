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
// SUPL POS INIT - Implementation of SUPL POS INIT encoding & decoding.
// 
//

#include "suplposinit.h"
#include "asn1per.h"


/**
Implemetation of CSuplPosInitContent Class
*/
EXPORT_C CSuplPosInitContent* CSuplPosInitContent::NewL()
	{
	CSuplPosInitContent* self = CSuplPosInitContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}
	
EXPORT_C CSuplPosInitContent* CSuplPosInitContent::NewLC()
	{
	CSuplPosInitContent* self = new (ELeave) CSuplPosInitContent();
	CleanupStack::PushL(self);
	return self;		
	}
	
CSuplPosInitContent::CSuplPosInitContent()
	{		
	}
	
EXPORT_C CSuplPosInitContent::~CSuplPosInitContent()
	{
	delete iRequestedAssistDataContent;
	delete iSetCapContent;
	delete iLocationIdContent;
	delete iPositionContent;
	}

TBool CSuplPosInitContent::operator==(const CSuplPosInitContent& other) const
	{
	TBool ret(EFalse);
	if   (iRequestedAssistDataPresent==other.iRequestedAssistDataPresent &&
			iPositionPresent==other.iPositionPresent &&
			iSuplPosPresent==other.iSuplPosPresent &&
			iVerPresent==other.iVerPresent &&
			*iSetCapContent==*other.iSetCapContent &&
			*iLocationIdContent==*other.iLocationIdContent)
		{
		if (iRequestedAssistDataPresent==EFalse || *iRequestedAssistDataContent==*other.iRequestedAssistDataContent)
			{
			ret=ETrue;
			}
		}
	return ret;			
	}
	
/**
Implemetation of CSuplPosInit Class
*/
CSuplPosInit* CSuplPosInit::NewL(CSuplPosInitContent* aSuplPosInitContent)
	{
	CSuplPosInit* self = CSuplPosInit::NewLC(aSuplPosInitContent);
	CleanupStack::Pop(self);
	return self;				
	};

CSuplPosInit* CSuplPosInit::NewLC(CSuplPosInitContent* aSuplPosInitContent)
	{
	CSuplPosInit* self = new (ELeave) CSuplPosInit(aSuplPosInitContent);
	CleanupStack::PushL(self);
	return self;
	};
		
CSuplPosInit::CSuplPosInit(CSuplPosInitContent* aSuplPosInitContent) 
: iSuplPosInitContent(aSuplPosInitContent)
	{
	}

CSuplPosInit::~CSuplPosInit()
	{
	delete iPosition;
	delete iRequestedAssistData;
	delete iSetCap;
	delete iLocationId;
	}

void CSuplPosInit::ConstructL()
	{
	};
	
void CSuplPosInit::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	//	SUPLPOSINIT ::= SEQUENCE {
	//	  sETCapabilities      SETCapabilities,
	//	  requestedAssistData  RequestedAssistData OPTIONAL,
	//	  locationId           LocationId,
	//	  position             Position OPTIONAL,
	//	  sUPLPOS              SUPLPOS OPTIONAL,
	//	  ver                  Ver OPTIONAL,
	//	  ...}
	
	
	// Still use aPerEncoding, since SUPL POSINIT message is not an extended SUPL message
	
	// SUPL START Specific part is extensible, encode "E" bit.	
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);
	
	//Encode OPTION BIT map	
	CAsn1PerEncodingFragment* opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, 
														iSuplPosInitContent->iRequestedAssistDataPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);


	opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, 
								iSuplPosInitContent->iPositionPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

	opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, 
								iSuplPosInitContent->iSuplPosPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

	opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, 
								iSuplPosInitContent->iVerPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

	//Encode the SET Capabilities
	iSetCap=CSuplSetCapabilities::NewL(iSuplPosInitContent->iSetCapContent);
	iSetCap->EncodeL(aPerEncoding);
	
	//Encode the assistance data
	if (iSuplPosInitContent->iRequestedAssistDataPresent)
		{
		iRequestedAssistData=CSuplRequestedAssistData::NewL(iSuplPosInitContent->iRequestedAssistDataContent);
		iRequestedAssistData->EncodeL(aPerEncoding);			
		}

	//Encode the Location ID
	iLocationId=CSuplLocationId::NewL(iSuplPosInitContent->iLocationIdContent);
	iLocationId->EncodeL(aPerEncoding);
	
	if (iSuplPosInitContent->iSuplPosPresent ||iSuplPosInitContent->iVerPresent)
		{
		User::Leave(KErrNotSupported);			
		}

	//Encode the Position
	if (iSuplPosInitContent->iPositionPresent)
		{
		iPosition = CSuplPosition::NewL(iSuplPosInitContent->iPositionContent);
		iPosition->EncodeL(aPerEncoding);
		}
	}
		
void CSuplPosInit::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	
	//	SUPLPOSINIT ::= SEQUENCE {
	//	  sETCapabilities      SETCapabilities,
	//	  requestedAssistData  RequestedAssistData OPTIONAL,
	//	  locationId           LocationId,
	//	  position             Position OPTIONAL,
	//	  sUPLPOS              SUPLPOS OPTIONAL,
	//	  ver                  Ver OPTIONAL,
	//	  ...}
		
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
							iSuplPosInitContent->iRequestedAssistDataPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	

	
	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplPosInitContent->iPositionPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	

	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplPosInitContent->iSuplPosPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	

	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplPosInitContent->iVerPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	


	
	//Decode the SET Capabilities
	iSuplPosInitContent->iSetCapContent=CSuplSetCapabilitiesContent::NewL();
	iSetCap=CSuplSetCapabilities::NewL(iSuplPosInitContent->iSetCapContent);
	iSetCap->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	

	
	//Decode the assitance data
	if (iSuplPosInitContent->iRequestedAssistDataPresent)
		{
		iSuplPosInitContent->iRequestedAssistDataContent=CSuplRequestedAssistDataContent::NewL();
		iRequestedAssistData=CSuplRequestedAssistData::NewL(iSuplPosInitContent->iRequestedAssistDataContent);
		iRequestedAssistData->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;	
		startPos+=bitsDecoded;	
		}
	
	//Decode the Location ID
	iSuplPosInitContent->iLocationIdContent=CSuplLocationIdContent::NewL();
	iLocationId=CSuplLocationId::NewL(iSuplPosInitContent->iLocationIdContent);
	iLocationId->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	if (iSuplPosInitContent->iPositionPresent || iSuplPosInitContent->iSuplPosPresent ||iSuplPosInitContent->iVerPresent)
		{
		User::Leave(KErrNotSupported);			
		}
	
	}
	

/**
Implemetation of CSuplRequestedAssistData Class
*/

CSuplRequestedAssistData* CSuplRequestedAssistData::NewL(CSuplRequestedAssistDataContent* aRequestedAssistDataContent)
	{
	CSuplRequestedAssistData* self = CSuplRequestedAssistData::NewLC(aRequestedAssistDataContent);
	CleanupStack::Pop(self);
	return self;			
	}

CSuplRequestedAssistData* CSuplRequestedAssistData::NewLC(CSuplRequestedAssistDataContent* aRequestedAssistDataContent)
	{
	CSuplRequestedAssistData* self = new (ELeave) CSuplRequestedAssistData(aRequestedAssistDataContent);
	CleanupStack::PushL(self);
	return self;									
	}
CSuplRequestedAssistData::~CSuplRequestedAssistData()
	{
	//delete iNavigationModelData;
	}

void CSuplRequestedAssistData::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
		// RequestedAssistData ::= SEQUENCE {
		//   almanacRequested                BOOLEAN,
		//   utcModelRequested               BOOLEAN,
		//   ionosphericModelRequested       BOOLEAN,
		//   dgpsCorrectionsRequested        BOOLEAN,
		//   referenceLocationRequested      BOOLEAN,
		//   referenceTimeRequested          BOOLEAN,
		//   acquisitionAssistanceRequested  BOOLEAN,
		//   realTimeIntegrityRequested      BOOLEAN,
		//   navigationModelRequested        BOOLEAN,
		//   navigationModelData             NavigationModel OPTIONAL,
		//   ...}
		
		
	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);


	// Option bit map for navigationModelData
	CAsn1PerEncodingFragment* opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iRequestedAssistDataContent->iNavigationModelDataPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

		
	// Encode almanacRequested
	CAsn1PerEncodingFragment* enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
														iRequestedAssistDataContent->iAlmanacRequested, 
														KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode utcModelRequested
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
								iRequestedAssistDataContent->iUtcModelRequested, 
								KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode ionosphericModelRequested
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
								iRequestedAssistDataContent->iIonosphericModelRequested, 
								KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
		
	// Encode dgpsCorrectionsRequested
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
								iRequestedAssistDataContent->iDgpsCorrectionsRequested, 
								KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
		
	// Encode referenceLocationRequested
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
								iRequestedAssistDataContent->iReferenceLocationRequested, 
								KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);

	// Encode referenceTimeRequested
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
								iRequestedAssistDataContent->iReferenceTimeRequested, 
								KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);

	// Encode acquisitionAssistanceRequested
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
								iRequestedAssistDataContent->iAcquisitionAssistanceRequested, 
								KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);
	
	// Encode realTimeIntegrityRequested
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
								iRequestedAssistDataContent->iRealTimeIntegrityRequested, 
								KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);

	// Encode navigationModelRequested
	enc=CAsn1PerBoolean::EncodeLC(aPerEncoding.iParentBitLength, 
								iRequestedAssistDataContent->iNavigationModelRequested, 
								KAsn1PerNoPadding);

	aPerEncoding.AddElementL(enc);
	CleanupStack::PopAndDestroy(enc);		
	
	if (iRequestedAssistDataContent->iNavigationModelDataPresent)
		{
		User::Leave(KErrNotSupported);	
		}
	
	}

void CSuplRequestedAssistData::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// RequestedAssistData ::= SEQUENCE {
		//   almanacRequested                BOOLEAN,
		//   utcModelRequested               BOOLEAN,
		//   ionosphericModelRequested       BOOLEAN,
		//   dgpsCorrectionsRequested        BOOLEAN,
		//   referenceLocationRequested      BOOLEAN,
		//   referenceTimeRequested          BOOLEAN,
		//   acquisitionAssistanceRequested  BOOLEAN,
		//   realTimeIntegrityRequested      BOOLEAN,
		//   navigationModelRequested        BOOLEAN,
		//   navigationModelData             NavigationModel OPTIONAL,
		//   ...}

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

	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iNavigationModelDataPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iAlmanacRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
							
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iUtcModelRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
							
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iIonosphericModelRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iDgpsCorrectionsRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iReferenceLocationRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iReferenceTimeRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iAcquisitionAssistanceRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iRealTimeIntegrityRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iRequestedAssistDataContent->iNavigationModelRequested);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	
	
	if (iRequestedAssistDataContent->iNavigationModelDataPresent)
		{
		User::Leave(KErrNotSupported);	
		}		
	}

CSuplRequestedAssistData::CSuplRequestedAssistData(CSuplRequestedAssistDataContent* aRequestedAssistDataContent)
:iRequestedAssistDataContent(aRequestedAssistDataContent)
	{		
	}

void CSuplRequestedAssistData::ConstructL()
	{
	//iNavigationModelData=CSuplNavigationModelData::NewL(iRequestedAssistDataContent->iNavigationModelDataConent);	
	}
	
/**
Implemetation of CSuplRequestedAssistDataContent Class
*/
EXPORT_C CSuplRequestedAssistDataContent* CSuplRequestedAssistDataContent::NewL()
	{
	CSuplRequestedAssistDataContent* self = CSuplRequestedAssistDataContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

EXPORT_C CSuplRequestedAssistDataContent* CSuplRequestedAssistDataContent::NewLC()
	{
	CSuplRequestedAssistDataContent* self = new (ELeave) CSuplRequestedAssistDataContent();
	CleanupStack::PushL(self);
	return self;											
	}

EXPORT_C CSuplRequestedAssistDataContent::~CSuplRequestedAssistDataContent()
	{
	//delete iNavigationModelDataConent;
	}
	
void CSuplRequestedAssistDataContent::ConstructL()
	{
	}
	
TBool CSuplRequestedAssistDataContent::operator==(const CSuplRequestedAssistDataContent& other) const
	{
	TBool ret(EFalse);
	if (iAlmanacRequested==other.iAlmanacRequested &&
		iUtcModelRequested==other.iUtcModelRequested &&
		iIonosphericModelRequested==other.iIonosphericModelRequested &&
		iDgpsCorrectionsRequested==other.iDgpsCorrectionsRequested &&
		iReferenceLocationRequested==other.iReferenceLocationRequested &&
		iReferenceTimeRequested==other.iReferenceTimeRequested &&
		iAcquisitionAssistanceRequested==other.iAcquisitionAssistanceRequested &&
		iRealTimeIntegrityRequested==other.iRealTimeIntegrityRequested &&
		iNavigationModelDataPresent==other.iNavigationModelDataPresent &&
		iNavigationModelRequested==other.iNavigationModelRequested)
		{
		ret=ETrue;	
		}
	return ret;	
	}



/**
Implemetation of CSuplPosition Class
*/
CSuplPosition::~CSuplPosition()
	{
	delete iPositionEstimate;
	}

CSuplPosition* CSuplPosition::NewLC(CSuplPositionContent* aPositionContent)
	{
	CSuplPosition* self = new (ELeave) CSuplPosition(aPositionContent);
	CleanupStack::PushL(self);
	return self;		
	}

CSuplPosition* CSuplPosition::NewL(CSuplPositionContent* aPositionContent)
	{
	CSuplPosition* self = CSuplPosition::NewLC(aPositionContent);
	CleanupStack::Pop(self);
	return self;			
	}
	
CSuplPosition::CSuplPosition(CSuplPositionContent* aPositionContent)
	:iPositionContent(aPositionContent)
	{	
	}


void CSuplPosition::EncodeL(CAsn1PerEncoding& aPerEncoding) // Change this !!!
	{
	//Position ::= SEQUENCE {
	//  timestamp         UTCTime, -- shall include seconds and shall use UTC time. 
	//  positionEstimate  PositionEstimate,
	//  velocity          Velocity OPTIONAL,
	//  ...}

	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);

	if (iPositionContent->iVelocityPresent)
		{
		User::Leave(KErrNotSupported);	
		}

	// Option bit map
	CAsn1PerEncodingFragment* velocityOpt = CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iPositionContent->iVelocityPresent);
	aPerEncoding.AddElementL(velocityOpt);
	CleanupStack::PopAndDestroy(velocityOpt);
	
	// Encode UTCTimeStamp. Convert first iUtcTimeStamp to YYMMDDhhmmssZ.
	TDateTime dateTime = iPositionContent->iUtcTimeStamp.DateTime();
	TInt second = dateTime.Second(); // Range for PER 00 to 59
	TBuf8<13> utcTime(13); // Long enough for the ASCII codes of string YYMMDDhhmmssZ
	utcTime[0]=(((dateTime.Year() % 1000) % 100) / 10) + 48; // second last number in the year
	utcTime[1]=(((dateTime.Year() % 1000) % 100) % 10) + 48; // last number in the year
	utcTime[2]=((dateTime.Month() + 1) / 10) + 48;
	utcTime[3]=((dateTime.Month() + 1) % 10) + 48;
	utcTime[4]=((dateTime.Day() + 1) / 10) + 48;
	utcTime[5]=((dateTime.Day() + 1) % 10) + 48;
	utcTime[6]=(dateTime.Hour() / 10) + 48;
	utcTime[7]=(dateTime.Hour() % 10) + 48;
	utcTime[8]=(dateTime.Minute() / 10) + 48;
	utcTime[9]=(dateTime.Minute() % 10) + 48;
	utcTime[10]=(dateTime.Second() / 10) + 48;
	utcTime[11]=(dateTime.Second() % 10) + 48;
	utcTime[12]= 0x5A; // ASCII for "Z"
	
	CAsn1PerEncodingFragment* utcTimeStamp=CAsn1PerUTCTime::EncodeLC(aPerEncoding.iParentBitLength,
													utcTime,
													KAsn1PerNoPadding
													);

	aPerEncoding.AddElementL(utcTimeStamp);
	CleanupStack::PopAndDestroy(utcTimeStamp);
	
	// Encode the CSuplPositionEstimate
	iPositionEstimate=CSuplPositionEstimate::NewL(iPositionContent->iPositionEstimateContent);
	iPositionEstimate->EncodeL(aPerEncoding);
	}

void CSuplPosition::ConstructL()
	{
	iPositionEstimate = CSuplPositionEstimate::NewL(iPositionContent->iPositionEstimateContent);
	}	


/**
Implemetation of CSuplPositionContent Class
*/
EXPORT_C CSuplPositionContent* CSuplPositionContent::NewLC()
	{
	CSuplPositionContent* self = new (ELeave) CSuplPositionContent();
	CleanupStack::PushL(self);
	return self;				
	}

EXPORT_C CSuplPositionContent* CSuplPositionContent::NewL()
	{
	CSuplPositionContent* self = CSuplPositionContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}
	
EXPORT_C CSuplPositionContent::~CSuplPositionContent()
	{
	delete iPositionEstimateContent;
	}

void CSuplPositionContent::ConstructL()
	{
	}

CSuplPositionContent::CSuplPositionContent():
iVelocityPresent(EFalse)
	{
	}

TBool CSuplPositionContent::operator==(const CSuplPositionContent& other) const
	{
	return (iUtcTimeStamp==other.iUtcTimeStamp && 
			*iPositionEstimateContent==*other.iPositionEstimateContent);
	}


/**
Implemetation of CSuplPositionEstimate Class
*/
EXPORT_C CSuplPositionEstimate* CSuplPositionEstimate::NewLC(CSuplPositionEstimateContent* aPositionEstimateContent)
	{
	CSuplPositionEstimate* self = new (ELeave) CSuplPositionEstimate(aPositionEstimateContent);
	CleanupStack::PushL(self);	
	return self;										
	}

EXPORT_C CSuplPositionEstimate* CSuplPositionEstimate::NewL(CSuplPositionEstimateContent* aPositionEstimateContent)
	{
	CSuplPositionEstimate* self = CSuplPositionEstimate::NewLC(aPositionEstimateContent);
	CleanupStack::Pop(self);
	return self;													
	}
EXPORT_C CSuplPositionEstimate::~CSuplPositionEstimate()
	{
	}
void CSuplPositionEstimate::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{

	//PositionEstimate ::= SEQUENCE {
  	//latitudeSign  ENUMERATED {north, south},
	//latitude      INTEGER(0..8388607),
	//longitude     INTEGER(-8388608..8388607),
	//uncertainty
	//  SEQUENCE {uncertaintySemiMajor  INTEGER(0..127),
	//            uncertaintySemiMinor  INTEGER(0..127),
	//            orientationMajorAxis  INTEGER(0..180)} OPTIONAL,
	//-- angle in degree between major axis and North
	//confidence    INTEGER(0..100) OPTIONAL,
	//altitudeInfo  AltitudeInfo OPTIONAL,
	//...}

	// Add sequence "E" bit
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);

	if (iPositionEstimateContent->iUncertainty || iPositionEstimateContent->iConfidence
		|| iPositionEstimateContent->iAltitudInfo)
		{
		User::Leave(KErrNotSupported);	
		}
	
	// Option bit map
	CAsn1PerEncodingFragment* uncertaintyOpt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iPositionEstimateContent->iUncertainty);
	aPerEncoding.AddElementL(uncertaintyOpt);
	CleanupStack::PopAndDestroy(uncertaintyOpt);

	CAsn1PerEncodingFragment* confidenceOpt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iPositionEstimateContent->iConfidence);
	aPerEncoding.AddElementL(confidenceOpt);
	CleanupStack::PopAndDestroy(confidenceOpt);
	
	CAsn1PerEncodingFragment* altitudInfoOpt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, iPositionEstimateContent->iAltitudInfo);
	aPerEncoding.AddElementL(altitudInfoOpt);
	CleanupStack::PopAndDestroy(altitudInfoOpt);

	//Encode latitude sign enum
	CAsn1PerEncodingFragment* latitudeSign=CAsn1PerEnum::EncodeLC(aPerEncoding.iParentBitLength,
															iPositionEstimateContent->iLatitudeSign,
															1,
															KAsn1PerNonExtensible,
															KAsn1PerNonExtended,
															KAsn1PerNoPadding);
	aPerEncoding.AddElementL(latitudeSign);
	CleanupStack::PopAndDestroy(latitudeSign);	

	//Encode the latitude
	CAsn1PerEncodingFragment* latitude=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iPositionEstimateContent->iLatitude, 
														KAsn1PerMinConstrained, 
														0, 
														KAsn1PerMaxConstrained, 
														8388607, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(latitude);
	CleanupStack::PopAndDestroy(latitude);

	//Encode the longitude
	CAsn1PerEncodingFragment* longitude=CAsn1PerInt::EncodeLC(aPerEncoding.iParentBitLength, 
														iPositionEstimateContent->iLongitude, 
														KAsn1PerMinConstrained, 
														-8388608, 
														KAsn1PerMaxConstrained, 
														8388607, 
														KAsn1PerNonExtensible, 
														KAsn1PerNonExtended,
														KAsn1PerNoPadding);


	aPerEncoding.AddElementL(longitude);
	CleanupStack::PopAndDestroy(longitude);
	}

void CSuplPositionEstimate::ConstructL()
	{
	}

CSuplPositionEstimate::CSuplPositionEstimate(CSuplPositionEstimateContent* aPositionEstimateContent)
:iPositionEstimateContent(aPositionEstimateContent)
	{
	}	

/**
Implemetation of CSuplPositionEstimateContent Class
*/

EXPORT_C CSuplPositionEstimateContent* CSuplPositionEstimateContent::NewLC()
	{
	CSuplPositionEstimateContent* self = new (ELeave) CSuplPositionEstimateContent();
	CleanupStack::PushL(self);
	return self;												
	}

EXPORT_C CSuplPositionEstimateContent* CSuplPositionEstimateContent::NewL()
	{
	CSuplPositionEstimateContent* self = CSuplPositionEstimateContent::NewLC();
	CleanupStack::Pop(self);
	return self;															
	}

void CSuplPositionEstimateContent::ConstructL()
	{
	}
	
CSuplPositionEstimateContent::CSuplPositionEstimateContent():
	iUncertainty(EFalse),
	iConfidence(EFalse),
	iAltitudInfo(EFalse)
	{		
	}
	
EXPORT_C CSuplPositionEstimateContent::~CSuplPositionEstimateContent()
	{
	}

TBool CSuplPositionEstimateContent::operator==(const CSuplPositionEstimateContent& other) const
	{
	return (iLatitude==other.iLatitude &&
	        iLongitude==other.iLongitude &&
	        iLatitudeSign==other.iLatitudeSign);
	}
