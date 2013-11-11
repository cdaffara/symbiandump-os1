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
// Implementation of encoding and decoding GPS Navigation model
// 
//

#include "rrcgpsnavigationmodel.h"
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsassistancenavigationmodel.h>



CRrcSf1RevdContent* CRrcSf1RevdContent::NewL()
	{
	CRrcSf1RevdContent* self = CRrcSf1RevdContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcSf1RevdContent* CRrcSf1RevdContent::NewLC()
	{
	CRrcSf1RevdContent* self = new (ELeave) CRrcSf1RevdContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcSf1RevdContent::~CRrcSf1RevdContent()
	{
	}




CRrcSf1Revd* CRrcSf1Revd::NewL(CRrcSf1RevdContent* aSf1RevdContent)
	{
	CRrcSf1Revd* self = CRrcSf1Revd::NewLC(aSf1RevdContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcSf1Revd* CRrcSf1Revd::NewLC(CRrcSf1RevdContent* aSf1RevdContent)
	{
	CRrcSf1Revd* self = new (ELeave) CRrcSf1Revd(aSf1RevdContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcSf1Revd::~CRrcSf1Revd()
	{
	}

CRrcSf1Revd::CRrcSf1Revd(CRrcSf1RevdContent* aSf1RevdContent)
:iSf1RevdContent(aSf1RevdContent)
	{
	}

void CRrcSf1Revd::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcSf1Revd::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// SubFrame1Reserved ::= 				SEQUENCE {
		// 	reserved1							BIT STRING (SIZE (23)),
		//	reserved2							BIT STRING (SIZE (24)),
		//	reserved3							BIT STRING (SIZE (24)),
		//	reserved4							BIT STRING (SIZE (16))
		//}
		
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	//No "E" Bit
	
	RBuf8 stringData;
	TInt stringLen(0);
	
	// Decode reserved1
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						23,
						23,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iSf1RevdContent->iReserved1=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode reserved2
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						24,
						24,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iSf1RevdContent->iReserved2=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
		
	// Decode reserved3
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						24,
						24,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iSf1RevdContent->iReserved3=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode reserved4
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iSf1RevdContent->iReserved4=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	}



CRrcEphemerisParameterContent* CRrcEphemerisParameterContent::NewL()
	{
	CRrcEphemerisParameterContent* self = CRrcEphemerisParameterContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcEphemerisParameterContent* CRrcEphemerisParameterContent::NewLC()
	{
	CRrcEphemerisParameterContent* self = new (ELeave) CRrcEphemerisParameterContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcEphemerisParameterContent::~CRrcEphemerisParameterContent()
	{
	delete iSf1RevdContent;
	}




CRrcEphemerisParameter* CRrcEphemerisParameter::NewL(CRrcEphemerisParameterContent* aEphemerisParameterContent)
	{
	CRrcEphemerisParameter* self = CRrcEphemerisParameter::NewLC(aEphemerisParameterContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcEphemerisParameter* CRrcEphemerisParameter::NewLC(CRrcEphemerisParameterContent* aEphemerisParameterContent)
	{
	CRrcEphemerisParameter* self = new (ELeave) CRrcEphemerisParameter(aEphemerisParameterContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcEphemerisParameter::~CRrcEphemerisParameter()
	{
	delete iSf1Revd;
	}

CRrcEphemerisParameter::CRrcEphemerisParameter(CRrcEphemerisParameterContent* aEphemerisParameterContent)
:iEphemerisParameterContent(aEphemerisParameterContent)
	{
	}

void CRrcEphemerisParameter::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcEphemerisParameter::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// EphemerisParameter ::=						SEQUENCE {
		// 	codeOnL2							BIT STRING (SIZE (2)),
		// 	uraIndex							BIT STRING (SIZE (4)),
		// 	satHealth							BIT STRING (SIZE (6)),
		// 	iodc								BIT STRING (SIZE (10)),
		// 	l2Pflag								BIT STRING (SIZE (1)),
		// 	sf1Revd								SubFrame1Reserved,
		// 	t-GD								BIT STRING (SIZE (8)),
		// 	t-oc								BIT STRING (SIZE (16)),
		// 	af2									BIT STRING (SIZE (8)),
		// 	af1									BIT STRING (SIZE (16)),
		// 	af0									BIT STRING (SIZE (22)),
		// 	c-rs								BIT STRING (SIZE (16)),
		// 	delta-n								BIT STRING (SIZE (16)),
		// 	m0									BIT STRING (SIZE (32)),
		// 	c-uc								BIT STRING (SIZE (16)),
		// 	e									BIT STRING (SIZE (32)),
		// 	c-us								BIT STRING (SIZE (16)),
		// 	a-Sqrt								BIT STRING (SIZE (32)),
		// 	t-oe								BIT STRING (SIZE (16)),
		// 	fitInterval							BIT STRING (SIZE (1)),
		// 	aodo								BIT STRING (SIZE (5)),
		// 	c-ic								BIT STRING (SIZE (16)),
		// 	omega0								BIT STRING (SIZE (32)),
		// 	c-is								BIT STRING (SIZE (16)),
		// 	i0									BIT STRING (SIZE (32)),
		// 	c-rc								BIT STRING (SIZE (16)),
		// 	omega								BIT STRING (SIZE (32)),
		// 	omegaDot							BIT STRING (SIZE (24)),
		// 	iDot								BIT STRING (SIZE (14))
		// }
		
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;
	
	//No "E" Bit
	
	RBuf8 stringData;
	TInt stringLen(0);
	
	// Decode codeOnL2
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						2,
						2,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iCodeOnL2=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode UraIndex	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						4,
						4,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iUraIndex=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
		
	// Decode SatHealth	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						6,
						6,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iSatHealth=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode Iodc	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						10,
						10,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iIodc=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode L2Pflag	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						1,
						1,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iL2Pflag=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode Sf1Revd
	iEphemerisParameterContent->iSf1RevdContent=CRrcSf1RevdContent::NewL();
	iSf1Revd=CRrcSf1Revd::NewL(iEphemerisParameterContent->iSf1RevdContent);
	iSf1Revd->DecodeFromL(aBufferToDecode, startPos,bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	
	// Decode iTGD	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						8,
						8,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iTGD=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iToc	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iToc=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iAf2	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						8,
						8,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iAf2=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode iAf1	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
						
						
	iEphemerisParameterContent->iAf1=stringData;	
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iAf0	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						22,
						22,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iAf0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iCrs	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iCrs=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iDeltaN	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iDeltaN=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iM0	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						32,
						32,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iM0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iCuc	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iCuc=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iE	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						32,
						32,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iE=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iCus	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iCus=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iASqrt	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						32,
						32,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iASqrt=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iToe	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iToe=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iFitInterval	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						1,
						1,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iFitInterval=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iAodo	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						5,
						5,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iAodo=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iCic	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iCic=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iOmega0	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						32,
						32,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iOmega0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iCis	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iCis=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iI0	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						32,
						32,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iI0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iCrc	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						16,
						16,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iCrc=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iOmega	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						32,
						32,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iOmega=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iCmegaDot	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						24,
						24,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iCmegaDot=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iDot	
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						14,
						14,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);
	iEphemerisParameterContent->iDot=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	}





void CRrcEphemerisParameter::WriteLBSAssistanceDataL(RNavigationModelSatInfoBuilder* aNavigationModelSatInfoBuilder)
	{
	REphemerisParameterBuilder ephemerisBuilder;
	aNavigationModelSatInfoBuilder->GetFieldBuilder(TNavigationModelSatInfo::EEphemerisParameter, ephemerisBuilder);
	
	TUint value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iCodeOnL2, 1);
	ephemerisBuilder.SetField(TEphemerisParameter::ECodeOnL2, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iUraIndex, 1);
	ephemerisBuilder.SetField(TEphemerisParameter::EUraIndex, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iSatHealth, 1);
	ephemerisBuilder.SetField(TEphemerisParameter::ESatHealth, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iIodc, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::EIodc, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iL2Pflag, 1);
	ephemerisBuilder.SetField(TEphemerisParameter::EL2Pflag, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iTGD, 1);
	ephemerisBuilder.SetField(TEphemerisParameter::ETGD, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iToc, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::EToc, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iAf2, 1);
	ephemerisBuilder.SetField(TEphemerisParameter::EAf2, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iAf1, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::EAf1, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iAf0, 3);
	ephemerisBuilder.SetField(TEphemerisParameter::EAf0, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iCrs, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::ECrs, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iDeltaN, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::EDeltaN, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iM0, 4);
	ephemerisBuilder.SetField(TEphemerisParameter::EM0, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iCuc, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::ECuc, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iE, 4);
	ephemerisBuilder.SetField(TEphemerisParameter::EE, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iCus, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::ECus, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iASqrt, 4);
	ephemerisBuilder.SetField(TEphemerisParameter::EASqrt, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iToe, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::EToe, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iFitInterval, 1);
	ephemerisBuilder.SetField(TEphemerisParameter::EFitInterval, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iAodo, 1);
	ephemerisBuilder.SetField(TEphemerisParameter::EAodo, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iCic, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::ECic, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iOmega0, 4);
	ephemerisBuilder.SetField(TEphemerisParameter::EOmega0, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iCis, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::ECis, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iI0, 4);
	ephemerisBuilder.SetField(TEphemerisParameter::EI0, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iCrc, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::ECrc, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iOmega, 4);
	ephemerisBuilder.SetField(TEphemerisParameter::EOmega, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iCmegaDot, 3);
	ephemerisBuilder.SetField(TEphemerisParameter::EOmegaDot, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iEphemerisParameterContent->iDot, 2);
	ephemerisBuilder.SetField(TEphemerisParameter::EIDot, value);
		
	}



CRrcNavigationModelSatInfoContent* CRrcNavigationModelSatInfoContent::NewL()
	{
	CRrcNavigationModelSatInfoContent* self = CRrcNavigationModelSatInfoContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcNavigationModelSatInfoContent* CRrcNavigationModelSatInfoContent::NewLC()
	{
	CRrcNavigationModelSatInfoContent* self = new (ELeave) CRrcNavigationModelSatInfoContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcNavigationModelSatInfoContent::~CRrcNavigationModelSatInfoContent()
	{
	delete iEphemerisParameterContent;
	}




CRrcNavigationModelSatInfo* CRrcNavigationModelSatInfo::NewL(CRrcNavigationModelSatInfoContent* aNavigationModelSatInfoContent)
	{
	CRrcNavigationModelSatInfo* self = CRrcNavigationModelSatInfo::NewLC(aNavigationModelSatInfoContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcNavigationModelSatInfo* CRrcNavigationModelSatInfo::NewLC(CRrcNavigationModelSatInfoContent* aNavigationModelSatInfoContent)
	{
	CRrcNavigationModelSatInfo* self = new (ELeave) CRrcNavigationModelSatInfo(aNavigationModelSatInfoContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcNavigationModelSatInfo::~CRrcNavigationModelSatInfo()
	{
	delete iEphemerisParameter;
	}

CRrcNavigationModelSatInfo::CRrcNavigationModelSatInfo(CRrcNavigationModelSatInfoContent* aNavigationModelSatInfoContent)
:iNavigationModelSatInfoContent(aNavigationModelSatInfoContent)
	{
	}

void CRrcNavigationModelSatInfo::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcNavigationModelSatInfo::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// NavigationModelSatInfo ::=			SEQUENCE {
		//		satID								SatID,
		//		satelliteStatus						SatelliteStatus,
		//		ephemerisParameter					EphemerisParameter		OPTIONAL
		//	}
		
		// SatID ::=							INTEGER (0..63)
		
		// SatelliteStatus ::=					ENUMERATED {
		//										ns-NN-U,
		//										es-SN,
		//										es-NN-U,
		//										rev2,
		//										rev }
		
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	//No "E" Bit
	
	//Decode EphemerisParameter	option
	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iNavigationModelSatInfoContent->iEphemerisParameterPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	//Decode SatID
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						63,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iNavigationModelSatInfoContent->iSatId);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	//Decode SatelliteStatus
	TInt enumValue;
	CAsn1PerEnum::DecodeL(aBufferToDecode,
						startPos,
						4,
						KAsn1PerNonExtensible,
						KAsn1PerNoPadding,
						bitsDecoded,
						isExtended,
						enumValue
						);
						
	iNavigationModelSatInfoContent->iSatelliteStatus=(TRrcSatelliteStatus)enumValue;
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	//Decode EphemerisParameter
	
	if (iNavigationModelSatInfoContent->iEphemerisParameterPresent)
		{
		iNavigationModelSatInfoContent->iEphemerisParameterContent=CRrcEphemerisParameterContent::NewL();
		iEphemerisParameter=CRrcEphemerisParameter::NewL(iNavigationModelSatInfoContent->iEphemerisParameterContent);
		iEphemerisParameter->DecodeFromL(aBufferToDecode, startPos,bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;		
		}
	}

void CRrcNavigationModelSatInfo::WriteLBSAssistanceDataL(RNavigationModelSatInfoBuilder* aNavigationModelSatInfoBuilder)
	{
	
	aNavigationModelSatInfoBuilder->SetField(TNavigationModelSatInfo::ESatId, (TUint)iNavigationModelSatInfoContent->iSatId);
	aNavigationModelSatInfoBuilder->SetField(TNavigationModelSatInfo::ESatelliteStatus, (TSatelliteStatus)iNavigationModelSatInfoContent->iSatelliteStatus);


	if (iNavigationModelSatInfoContent->iEphemerisParameterPresent)
		{
		iEphemerisParameter->WriteLBSAssistanceDataL(aNavigationModelSatInfoBuilder);
		}	
	}


CRrcNavigationModelSatInfoListContent* CRrcNavigationModelSatInfoListContent::NewL()
	{
	CRrcNavigationModelSatInfoListContent* self = CRrcNavigationModelSatInfoListContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcNavigationModelSatInfoListContent* CRrcNavigationModelSatInfoListContent::NewLC()
	{
	CRrcNavigationModelSatInfoListContent* self = new (ELeave) CRrcNavigationModelSatInfoListContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcNavigationModelSatInfoListContent::~CRrcNavigationModelSatInfoListContent()
	{
	iNavigationModelSatInfoListContent.ResetAndDestroy();
	}



CRrcNavigationModelSatInfoList* CRrcNavigationModelSatInfoList::NewL(CRrcNavigationModelSatInfoListContent* aRrcNavigationModelSatInfoListContent)
	{
	CRrcNavigationModelSatInfoList* self = CRrcNavigationModelSatInfoList::NewLC(aRrcNavigationModelSatInfoListContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcNavigationModelSatInfoList* CRrcNavigationModelSatInfoList::NewLC(CRrcNavigationModelSatInfoListContent* aRrcNavigationModelSatInfoListContent)
	{
	CRrcNavigationModelSatInfoList* self = new (ELeave) CRrcNavigationModelSatInfoList(aRrcNavigationModelSatInfoListContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcNavigationModelSatInfoList::~CRrcNavigationModelSatInfoList()
	{
	iNavigationModelSatInfoArray.ResetAndDestroy();
	}

CRrcNavigationModelSatInfoList::CRrcNavigationModelSatInfoList(CRrcNavigationModelSatInfoListContent* aRrcNavigationModelSatInfoListContent)
:iNavigationModelSatInfoArrayContent(aRrcNavigationModelSatInfoListContent)
	{
	}

void CRrcNavigationModelSatInfoList::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcNavigationModelSatInfoList::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// NavigationModelSatInfoList ::=		SEQUENCE (SIZE (1..maxSat)) OF
		//										NavigationModelSatInfo
				
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	TInt size;
	// Decode Squence of NavigationModelSatInfo
	CAsn1PerSequenceof::DecodeL(aBufferToDecode,
						startPos,
						KAsn1PerMinConstrained,
						1,
						KAsn1PerMaxConstrained,
						16,
						KAsn1PerNonExtensible,
						bitsDecoded,
						size,
						isExtended);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	
	
	// Decode each NavigationModelSatInfo in the list
	for (TInt i=0;i<size;i++)
		{
		CRrcNavigationModelSatInfoContent* content=CRrcNavigationModelSatInfoContent::NewLC();
		iNavigationModelSatInfoArrayContent->iNavigationModelSatInfoListContent.AppendL(content);
		CleanupStack::Pop(content);
		CRrcNavigationModelSatInfo* satInfo=CRrcNavigationModelSatInfo::NewLC(content);
		iNavigationModelSatInfoArray.AppendL(satInfo);
		CleanupStack::Pop(satInfo);
		satInfo->DecodeFromL(aBufferToDecode, startPos,bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;
		}	
	}


void CRrcNavigationModelSatInfoList::WriteLBSAssistanceDataL(RUEPositioningGpsNavigationModelBuilder* aGpsNavigationModelBuilder)
	{
	// Populate the data item
	RNavigationModelSatInfoArrayBuilder navigationModelSatInfoArrayBuilder;
	aGpsNavigationModelBuilder->GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, navigationModelSatInfoArrayBuilder);		
	TInt count=iNavigationModelSatInfoArrayContent->iNavigationModelSatInfoListContent.Count();
	for (TInt i=0;i<count;i++)
		{
		RNavigationModelSatInfoBuilder navigationModelSatInfoBuilder;	
		navigationModelSatInfoArrayBuilder.GetFieldBuilder(i, navigationModelSatInfoBuilder);
		iNavigationModelSatInfoArray[i]->WriteLBSAssistanceDataL(&navigationModelSatInfoBuilder);	
		}		
	}


CRrcGpsNavigationModelContent* CRrcGpsNavigationModelContent::NewL()
	{
	CRrcGpsNavigationModelContent* self = CRrcGpsNavigationModelContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsNavigationModelContent* CRrcGpsNavigationModelContent::NewLC()
	{
	CRrcGpsNavigationModelContent* self = new (ELeave) CRrcGpsNavigationModelContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsNavigationModelContent::~CRrcGpsNavigationModelContent()
	{
	delete iNavigationModelSatInfoListContent;
	}





CRrcGpsNavigationModel* CRrcGpsNavigationModel::NewL(CRrcGpsNavigationModelContent* aGpsNavigationModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsNavigationModel* self = CRrcGpsNavigationModel::NewLC(aGpsNavigationModelContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsNavigationModel* CRrcGpsNavigationModel::NewLC(CRrcGpsNavigationModelContent* aGpsNavigationModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsNavigationModel* self = new (ELeave) CRrcGpsNavigationModel(aGpsNavigationModelContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsNavigationModel::~CRrcGpsNavigationModel()
	{
	delete iNavigationModelSatInfoList;
	}

CRrcGpsNavigationModel::CRrcGpsNavigationModel(CRrcGpsNavigationModelContent* aGpsNavigationModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iGpsNavigationModelContent(aGpsNavigationModelContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcGpsNavigationModel::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsNavigationModel::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// UE-Positioning-GPS-NavigationModel ::=			SEQUENCE {
		// 	navigationModelSatInfoList			NavigationModelSatInfoList
		//}	
	
	aDecodedBits=0;
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" Bit
	
	// Decode NavigationModelSatInfoList
	iGpsNavigationModelContent->iNavigationModelSatInfoListContent=CRrcNavigationModelSatInfoListContent::NewL();
	iNavigationModelSatInfoList=CRrcNavigationModelSatInfoList::NewL(iGpsNavigationModelContent->iNavigationModelSatInfoListContent);
	iNavigationModelSatInfoList->DecodeFromL(aBufferToDecode, startPos,bitsDecoded);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	
	}

void CRrcGpsNavigationModel::WriteLBSAssistanceDataL()
	{
	//Fetch a GpsNavigationModel data item.
	RUEPositioningGpsNavigationModelBuilder* gpsNavigationModelBuilder;
	iLbsAssistanceDataBuilderSet->GetDataBuilder(gpsNavigationModelBuilder);

	iNavigationModelSatInfoList->WriteLBSAssistanceDataL(gpsNavigationModelBuilder);

	TTime timeNow;
	timeNow.UniversalTime();
	gpsNavigationModelBuilder->SetTimeStamp(timeNow);	
	}
	
	
