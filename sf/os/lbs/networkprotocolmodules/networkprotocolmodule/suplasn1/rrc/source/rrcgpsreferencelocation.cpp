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
// Implementation of encoding and decoding GPS Reference Location
// 
//

#include "rrcgpsreferencelocation.h"
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsassistancereferencelocation.h>

CRrcEllipsoidPointAltitudeEllipsoideContent* CRrcEllipsoidPointAltitudeEllipsoideContent::NewL()
	{
	CRrcEllipsoidPointAltitudeEllipsoideContent* self = CRrcEllipsoidPointAltitudeEllipsoideContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcEllipsoidPointAltitudeEllipsoideContent* CRrcEllipsoidPointAltitudeEllipsoideContent::NewLC()
	{
	CRrcEllipsoidPointAltitudeEllipsoideContent* self = new (ELeave) CRrcEllipsoidPointAltitudeEllipsoideContent();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcEllipsoidPointAltitudeEllipsoideContent::~CRrcEllipsoidPointAltitudeEllipsoideContent()
	{
	}

CRrcEllipsoidPointAltitudeEllipsoideContent::CRrcEllipsoidPointAltitudeEllipsoideContent()
	{
	}


void CRrcEllipsoidPointAltitudeEllipsoideContent::ConstructL()
	{
	}





CRrcEllipsoidPointAltitudeEllipsoide* CRrcEllipsoidPointAltitudeEllipsoide::NewL(CRrcEllipsoidPointAltitudeEllipsoideContent* aEllipsoidPointAltitudeEllipsoideContent)
	{
	CRrcEllipsoidPointAltitudeEllipsoide* self = CRrcEllipsoidPointAltitudeEllipsoide::NewLC(aEllipsoidPointAltitudeEllipsoideContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcEllipsoidPointAltitudeEllipsoide* CRrcEllipsoidPointAltitudeEllipsoide::NewLC(CRrcEllipsoidPointAltitudeEllipsoideContent* aEllipsoidPointAltitudeEllipsoideContent)
	{
	CRrcEllipsoidPointAltitudeEllipsoide* self = new (ELeave) CRrcEllipsoidPointAltitudeEllipsoide(aEllipsoidPointAltitudeEllipsoideContent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcEllipsoidPointAltitudeEllipsoide::~CRrcEllipsoidPointAltitudeEllipsoide()
	{
	}

CRrcEllipsoidPointAltitudeEllipsoide::CRrcEllipsoidPointAltitudeEllipsoide(CRrcEllipsoidPointAltitudeEllipsoideContent* aEllipsoidPointAltitudeEllipsoideContent)
:iEllipsoidPointAltitudeEllipsoideContent(aEllipsoidPointAltitudeEllipsoideContent)
	{
	}

void CRrcEllipsoidPointAltitudeEllipsoide::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcEllipsoidPointAltitudeEllipsoide::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		//	EllipsoidPointAltitudeEllipsoide ::=	SEQUENCE {
		//		latitudeSign			ENUMERATED { north, south },
		//		latitude				INTEGER (0..8388607),
		//		longitude				INTEGER (-8388608..8388607),
		//		altitudeDirection		ENUMERATED {height, depth},
		//		altitude				INTEGER (0..32767),
		//		uncertaintySemiMajor	INTEGER (0..127),
		//		uncertaintySemiMinor	INTEGER (0..127),
		//		   -- Actual value orientationMajorAxis = IE value * 2
		//		orientationMajorAxis	INTEGER (0..89),
		//		uncertaintyAltitude		INTEGER (0..127),
		//		confidence				INTEGER (0..100)
		//	}



	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT	
	// Decode latitudeSign
	TInt chioceValue;
	CAsn1PerEnum::DecodeL(aBufferToDecode,
						startPos,
						1,
						KAsn1PerNonExtensible,
						KAsn1PerNoPadding,
						bitsDecoded,
						isExtended,
						chioceValue
						);
						
	iEllipsoidPointAltitudeEllipsoideContent->iLatitudeSign=(TRrcLatitudeSign)chioceValue;
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	

	// Decode latitude
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						8388607,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iEllipsoidPointAltitudeEllipsoideContent->ilatitude);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode longitude
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						-8388608,
						KAsn1PerMaxConstrained,
						8388607,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iEllipsoidPointAltitudeEllipsoideContent->ilongitude);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode altitudeDirection
	TInt enumValue;
	CAsn1PerEnum::DecodeL(aBufferToDecode,
						startPos,
						1,
						KAsn1PerNonExtensible,
						KAsn1PerNoPadding,
						bitsDecoded,
						isExtended,
						enumValue
						);
						
	iEllipsoidPointAltitudeEllipsoideContent->iAltitudeDirection=(TRrcAltitudeDirection)enumValue;
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode altitude
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						32767,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iEllipsoidPointAltitudeEllipsoideContent->iAltitude);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode uncertaintySemiMajor
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						127,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iEllipsoidPointAltitudeEllipsoideContent->iUncertaintySemiMajor);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode uncertaintySemiMinor
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						127,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iEllipsoidPointAltitudeEllipsoideContent->iUncertaintySemiMinor);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode orientationMajorAxis
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						89,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iEllipsoidPointAltitudeEllipsoideContent->iOrientationMajorAxis);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode uncertaintyAltitude
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						127,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iEllipsoidPointAltitudeEllipsoideContent->iUncertaintyAltitude);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	

	// Decode confidence
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						100,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iEllipsoidPointAltitudeEllipsoideContent->iConfidence);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	}

void CRrcEllipsoidPointAltitudeEllipsoide::ConstructL()
	{
	}

void CRrcEllipsoidPointAltitudeEllipsoide::WriteLBSAssistanceDataL(RReferenceLocationBuilder* aReferenceLocationBuilder)
	{	
	REllipsoidPointAltitudeEllipsoideBuilder ellipsoidPointAltBuilder;	
	aReferenceLocationBuilder->GetFieldBuilder(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, ellipsoidPointAltBuilder);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EAltitude, (TUint)iEllipsoidPointAltitudeEllipsoideContent->iAltitude);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitudeSign, (TEllipsoidPointAltitudeEllipsoide::TLatitudeSign)iEllipsoidPointAltitudeEllipsoideContent->iLatitudeSign);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELongitude, iEllipsoidPointAltitudeEllipsoideContent->ilongitude);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EAltitudeDirection, (TEllipsoidPointAltitudeEllipsoide::TAltitudeDirection)iEllipsoidPointAltitudeEllipsoideContent->iAltitudeDirection);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMajor, (TUint)iEllipsoidPointAltitudeEllipsoideContent->iUncertaintySemiMajor);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMinor, (TUint)iEllipsoidPointAltitudeEllipsoideContent->iUncertaintySemiMinor);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EOrientationMajorAxis, (TUint)iEllipsoidPointAltitudeEllipsoideContent->iOrientationMajorAxis);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintyAltitude, (TUint)iEllipsoidPointAltitudeEllipsoideContent->iUncertaintyAltitude);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EConfidence, (TUint)iEllipsoidPointAltitudeEllipsoideContent->iConfidence);
	ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitude, (TUint)iEllipsoidPointAltitudeEllipsoideContent->ilatitude);
	
	}




CRrcReferenceLocationContent* CRrcReferenceLocationContent::NewL()
	{
	CRrcReferenceLocationContent* self = CRrcReferenceLocationContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcReferenceLocationContent* CRrcReferenceLocationContent::NewLC()
	{
	CRrcReferenceLocationContent* self = new (ELeave) CRrcReferenceLocationContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcReferenceLocationContent::~CRrcReferenceLocationContent()
	{
	delete iEllipsoidPointAltitudeEllipsoideContent;
	}

CRrcReferenceLocationContent::CRrcReferenceLocationContent()
	{
	}







CRrcReferenceLocation* CRrcReferenceLocation::NewL(CRrcReferenceLocationContent* aReferenceLocationContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcReferenceLocation* self = CRrcReferenceLocation::NewLC(aReferenceLocationContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcReferenceLocation* CRrcReferenceLocation::NewLC(CRrcReferenceLocationContent* aReferenceLocationContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcReferenceLocation* self = new (ELeave) CRrcReferenceLocation(aReferenceLocationContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcReferenceLocation::~CRrcReferenceLocation()
	{
	delete iEllipsoidPointAltitudeEllipsoide;
	}

CRrcReferenceLocation::CRrcReferenceLocation(CRrcReferenceLocationContent* aReferenceLocationContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iReferenceLocationContent(aReferenceLocationContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcReferenceLocation::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcReferenceLocation::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
//	ReferenceLocation ::=			SEQUENCE {
//		ellipsoidPointAltitudeEllipsoide		EllipsoidPointAltitudeEllipsoide
//	}	

	aDecodedBits=0;
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT
		
	// Decode Utran Reference Time
	iReferenceLocationContent->iEllipsoidPointAltitudeEllipsoideContent=CRrcEllipsoidPointAltitudeEllipsoideContent::NewL();
	iEllipsoidPointAltitudeEllipsoide=CRrcEllipsoidPointAltitudeEllipsoide::NewL(iReferenceLocationContent->iEllipsoidPointAltitudeEllipsoideContent);	
	iEllipsoidPointAltitudeEllipsoide->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	}


void CRrcReferenceLocation::ConstructL()
	{
	}
	
	
void CRrcReferenceLocation::WriteLBSAssistanceDataL()
	{
	//Fetch a ReferenceLocation data item.
	RReferenceLocationBuilder* gpsReferenceLocationBuilder;
	iLbsAssistanceDataBuilderSet->GetDataBuilder(gpsReferenceLocationBuilder);

	iEllipsoidPointAltitudeEllipsoide->WriteLBSAssistanceDataL(gpsReferenceLocationBuilder);
	
	TTime timeNow;
	timeNow.UniversalTime();
	gpsReferenceLocationBuilder->SetTimeStamp(timeNow);
	}
	


