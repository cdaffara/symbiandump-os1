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
// RRCGPSACQUISITIONASSITANCE.CPP
// Implementation of encoding and decoding GPS Acquisition Assitance
// 
//

#include "rrcgpsacquisitionassistance.h"
#include <lbs/lbsassistancedatabuilderset.h>



CRrcExtraDopplerInfoContent* CRrcExtraDopplerInfoContent::NewL()
	{
	CRrcExtraDopplerInfoContent* self = CRrcExtraDopplerInfoContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcExtraDopplerInfoContent* CRrcExtraDopplerInfoContent::NewLC()
	{
	CRrcExtraDopplerInfoContent* self = new (ELeave) CRrcExtraDopplerInfoContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcExtraDopplerInfoContent::~CRrcExtraDopplerInfoContent()
	{
	}

CRrcExtraDopplerInfoContent::CRrcExtraDopplerInfoContent()
	{
	}

CRrcExtraDopplerInfo* CRrcExtraDopplerInfo::NewL(CRrcExtraDopplerInfoContent* aExtraDopplerInfoContent)
	{
	CRrcExtraDopplerInfo* self = CRrcExtraDopplerInfo::NewLC(aExtraDopplerInfoContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcExtraDopplerInfo* CRrcExtraDopplerInfo::NewLC(CRrcExtraDopplerInfoContent* aExtraDopplerInfoContent)
	{
	CRrcExtraDopplerInfo* self = new (ELeave) CRrcExtraDopplerInfo(aExtraDopplerInfoContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcExtraDopplerInfo::~CRrcExtraDopplerInfo()
	{
	}

CRrcExtraDopplerInfo::CRrcExtraDopplerInfo(CRrcExtraDopplerInfoContent* aExtraDopplerInfoContent)
:iExtraDopplerInfoContent(aExtraDopplerInfoContent)
	{
	}

void CRrcExtraDopplerInfo::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcExtraDopplerInfo::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	// ExtraDopplerInfo ::=				SEQUENCE {
	//	-- Actual value doppler1stOrder = IE value * 0.023
	//	doppler1stOrder						INTEGER (-42..21),
	//	dopplerUncertainty					DopplerUncertainty
	//}
	
	//DopplerUncertainty ::=				ENUMERATED {
	//										hz12-5, hz25, hz50, hz100, hz200,
	//										spare3, spare2, spare1 }


	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT
	// Decode doppler1stOrder
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						-42,
						KAsn1PerMaxConstrained,
						21,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iExtraDopplerInfoContent->iDoppler1stOrder);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;


	// Decode dopplerUncertainty
	TInt chioceValue;
	CAsn1PerEnum::DecodeL(aBufferToDecode,
						startPos,
						7,
						KAsn1PerNonExtensible,
						KAsn1PerNoPadding,
						bitsDecoded,
						isExtended,
						chioceValue
						);
						
	iExtraDopplerInfoContent->iDopplerUncertainty=(TRrcDopplerUncertainty)chioceValue;
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	
	}


void CRrcExtraDopplerInfo::WriteLBSAssistanceDataL(RAcquisitionSatInfoBuilder* aAcquisitionSatInfoBuilder)
	{
	RExtraDopplerInfoBuilder extraDopplerInfoBuilder;
	aAcquisitionSatInfoBuilder->GetFieldBuilder(TAcquisitionSatInfo::EExtraDopplerInfo, extraDopplerInfoBuilder);
	extraDopplerInfoBuilder.SetField(TExtraDopplerInfo::EDoppler1stOrder, (TInt)iExtraDopplerInfoContent->iDoppler1stOrder);
	extraDopplerInfoBuilder.SetField(TExtraDopplerInfo::EDopplerUncertainty, (TDopplerUncertainty)iExtraDopplerInfoContent->iDopplerUncertainty);
		
	}














CRrcAzimuthAndElevationContent* CRrcAzimuthAndElevationContent::NewL()
	{
	CRrcAzimuthAndElevationContent* self = CRrcAzimuthAndElevationContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAzimuthAndElevationContent* CRrcAzimuthAndElevationContent::NewLC()
	{
	CRrcAzimuthAndElevationContent* self = new (ELeave) CRrcAzimuthAndElevationContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcAzimuthAndElevationContent::~CRrcAzimuthAndElevationContent()
	{
	}

CRrcAzimuthAndElevationContent::CRrcAzimuthAndElevationContent()
	{
	}

CRrcAzimuthAndElevation* CRrcAzimuthAndElevation::NewL(CRrcAzimuthAndElevationContent* aAzimuthAndElevationContent)
	{
	CRrcAzimuthAndElevation* self = CRrcAzimuthAndElevation::NewLC(aAzimuthAndElevationContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAzimuthAndElevation* CRrcAzimuthAndElevation::NewLC(CRrcAzimuthAndElevationContent* aAzimuthAndElevationContent)
	{
	CRrcAzimuthAndElevation* self = new (ELeave) CRrcAzimuthAndElevation(aAzimuthAndElevationContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcAzimuthAndElevation::~CRrcAzimuthAndElevation()
	{
	}

CRrcAzimuthAndElevation::CRrcAzimuthAndElevation(CRrcAzimuthAndElevationContent* aAzimuthAndElevationContent)
:iAzimuthAndElevationContent(aAzimuthAndElevationContent)
	{
	}

void CRrcAzimuthAndElevation::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcAzimuthAndElevation::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// AzimuthAndElevation ::=				SEQUENCE {
		//	-- Actual value azimuth = IE value * 11.25
		//	azimuth								INTEGER (0..31),
		//	-- Actual value elevation = IE value * 11.25
		//	elevation							INTEGER (0..7)
		//}

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT

	// Decode azimuth
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						31,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAzimuthAndElevationContent->iAzimuth);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;


	// Decode elevation
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						7,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAzimuthAndElevationContent->iElevation);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	}

void CRrcAzimuthAndElevation::WriteLBSAssistanceDataL(RAcquisitionSatInfoBuilder* aAcquisitionSatInfoBuilder)
	{

	RTAzimuthAndElevationBuilder azimuthAndElevationBuilder;
	aAcquisitionSatInfoBuilder->GetFieldBuilder(TAcquisitionSatInfo::EAzimuthAndElevation, azimuthAndElevationBuilder);
	azimuthAndElevationBuilder.SetField(TAzimuthAndElevation::EAzimuth, (TUint)iAzimuthAndElevationContent->iAzimuth);
	azimuthAndElevationBuilder.SetField(TAzimuthAndElevation::EElevation, (TUint)iAzimuthAndElevationContent->iElevation);			
	}




















CRrcAcquisitionSatInfoContent* CRrcAcquisitionSatInfoContent::NewL()
	{
	CRrcAcquisitionSatInfoContent* self = CRrcAcquisitionSatInfoContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAcquisitionSatInfoContent* CRrcAcquisitionSatInfoContent::NewLC()
	{
	CRrcAcquisitionSatInfoContent* self = new (ELeave) CRrcAcquisitionSatInfoContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcAcquisitionSatInfoContent::~CRrcAcquisitionSatInfoContent()
	{
	delete iExtraDopplerInfoContent;
	delete iAzimuthAndElevationContent;

	}

CRrcAcquisitionSatInfoContent::CRrcAcquisitionSatInfoContent()
	{
	}
	
void CRrcAcquisitionSatInfoContent::ConstructL()
	{
	}


CRrcAcquisitionSatInfo* CRrcAcquisitionSatInfo::NewL(CRrcAcquisitionSatInfoContent* aAcquisitionSatInfoContent)
	{
	CRrcAcquisitionSatInfo* self = CRrcAcquisitionSatInfo::NewLC(aAcquisitionSatInfoContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAcquisitionSatInfo* CRrcAcquisitionSatInfo::NewLC(CRrcAcquisitionSatInfoContent* aAcquisitionSatInfoContent)
	{
	CRrcAcquisitionSatInfo* self = new (ELeave) CRrcAcquisitionSatInfo(aAcquisitionSatInfoContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcAcquisitionSatInfo::~CRrcAcquisitionSatInfo()
	{
	delete iExtraDopplerInfo;
	delete iAzimuthAndElevation;	
	}

CRrcAcquisitionSatInfo::CRrcAcquisitionSatInfo(CRrcAcquisitionSatInfoContent* aAcquisitionSatInfoContent)
:iAcquisitionSatInfoContent(aAcquisitionSatInfoContent)
	{
	}

void CRrcAcquisitionSatInfo::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcAcquisitionSatInfo::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		//AcquisitionSatInfo ::=				SEQUENCE {
		//	satID								SatID,
		//	-- Actual value dopplerOthOrder = IE value * 2.5
		//	doppler0thOrder						INTEGER (-2048..2047),
		//	extraDopplerInfo					ExtraDopplerInfo					OPTIONAL,
		//	codePhase							INTEGER (0..1022),
		//	integerCodePhase					INTEGER (0..19),
		//	gps-BitNumber						INTEGER (0..3),
		//	codePhaseSearchWindow				CodePhaseSearchWindow,
		//	azimuthAndElevation					AzimuthAndElevation					OPTIONAL
		//}
		
		//CodePhaseSearchWindow ::=			ENUMERATED {
		//										w1023, w1, w2, w3, w4, w6, w8,
		//										w12, w16, w24, w32, w48, w64,
		//										w96, w128, w192 }	
		//SatID ::=							INTEGER (0..63)


	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT
	// Decode option bit map
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iAcquisitionSatInfoContent->iExtraDopplerInfoPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	

	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iAcquisitionSatInfoContent->iAzimuthAndElevationPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode SatID
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						63,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAcquisitionSatInfoContent->iSatID);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;


	// Decode doppler0thOrder
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						-2048,
						KAsn1PerMaxConstrained,
						2047,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAcquisitionSatInfoContent->iDoppler0thOrder);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode extraDopplerInfo
	if (iAcquisitionSatInfoContent->iExtraDopplerInfoPresent)
		{
		iAcquisitionSatInfoContent->iExtraDopplerInfoContent=CRrcExtraDopplerInfoContent::NewL();
		iExtraDopplerInfo=CRrcExtraDopplerInfo::NewL(iAcquisitionSatInfoContent->iExtraDopplerInfoContent);
		iExtraDopplerInfo->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;
		}

	// Decode codePhase
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						1022,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAcquisitionSatInfoContent->iCodePhase);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	//Decode integerCodePhase
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						19,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAcquisitionSatInfoContent->iIntegerCodePhase);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	//Decode integerCodePhase
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						3,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAcquisitionSatInfoContent->iGpsBitNumber);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	

	//Decode codePhaseSearchWindow
	TInt chioceValue;
	CAsn1PerEnum::DecodeL(aBufferToDecode,
						startPos,
						15,
						KAsn1PerNonExtensible,
						KAsn1PerNoPadding,
						bitsDecoded,
						isExtended,
						chioceValue
						);
						
	iAcquisitionSatInfoContent->iCodePhaseSearchWindow=(TRrcCodePhaseSearchWindow)chioceValue;
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	

	// Decode AzimuthAndElevation
	if (iAcquisitionSatInfoContent->iAzimuthAndElevationPresent)
		{
		iAcquisitionSatInfoContent->iAzimuthAndElevationContent=CRrcAzimuthAndElevationContent::NewL();
		iAzimuthAndElevation=CRrcAzimuthAndElevation::NewL(iAcquisitionSatInfoContent->iAzimuthAndElevationContent);	
		iAzimuthAndElevation->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;
		}

	}

void CRrcAcquisitionSatInfo::ConstructL()
	{
	}

void CRrcAcquisitionSatInfo::WriteLBSAssistanceDataL(RAcquisitionSatInfoBuilder* aAcquisitionSatInfoBuilder)
	{
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::ESatID, (TUint)iAcquisitionSatInfoContent->iSatID);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::EDoppler0thOrder, (TInt)iAcquisitionSatInfoContent->iDoppler0thOrder);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::ECodePhase, (TUint)iAcquisitionSatInfoContent->iCodePhase);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::EIntegerCodePhase, (TUint)iAcquisitionSatInfoContent->iIntegerCodePhase);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::EGpsBitNumber, (TUint)iAcquisitionSatInfoContent->iGpsBitNumber);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::ECodePhaseSearchWindow, (TCodePhaseSearchWindow)iAcquisitionSatInfoContent->iCodePhaseSearchWindow);

	if (iAcquisitionSatInfoContent->iExtraDopplerInfoPresent)
		{
		iExtraDopplerInfo->WriteLBSAssistanceDataL(aAcquisitionSatInfoBuilder);			
		}
		
	if (iAcquisitionSatInfoContent->iAzimuthAndElevationPresent)
		{
		iAzimuthAndElevation->WriteLBSAssistanceDataL(aAcquisitionSatInfoBuilder);	
		}
	}


CRrcAcquisitionSatInfoListContent* CRrcAcquisitionSatInfoListContent::NewL()
	{
	CRrcAcquisitionSatInfoListContent* self = CRrcAcquisitionSatInfoListContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAcquisitionSatInfoListContent* CRrcAcquisitionSatInfoListContent::NewLC()
	{
	CRrcAcquisitionSatInfoListContent* self = new (ELeave) CRrcAcquisitionSatInfoListContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcAcquisitionSatInfoListContent::~CRrcAcquisitionSatInfoListContent()
	{
	iAcquisitionSatInfoContentList.ResetAndDestroy();
	}

CRrcAcquisitionSatInfoListContent::CRrcAcquisitionSatInfoListContent()
	{
	}

CRrcAcquisitionSatInfoList* CRrcAcquisitionSatInfoList::NewL(CRrcAcquisitionSatInfoListContent* aAcquisitionSatInfoListContent)
	{
	CRrcAcquisitionSatInfoList* self = CRrcAcquisitionSatInfoList::NewLC(aAcquisitionSatInfoListContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAcquisitionSatInfoList* CRrcAcquisitionSatInfoList::NewLC(CRrcAcquisitionSatInfoListContent* aAcquisitionSatInfoListContent)
	{
	CRrcAcquisitionSatInfoList* self = new (ELeave) CRrcAcquisitionSatInfoList(aAcquisitionSatInfoListContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcAcquisitionSatInfoList::~CRrcAcquisitionSatInfoList()
	{
	iAcquisitionSatInfoList.ResetAndDestroy();
	}

CRrcAcquisitionSatInfoList::CRrcAcquisitionSatInfoList(CRrcAcquisitionSatInfoListContent* aAcquisitionSatInfoListContent)
:iAcquisitionSatInfoListContent(aAcquisitionSatInfoListContent)
	{
	}

void CRrcAcquisitionSatInfoList::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcAcquisitionSatInfoList::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// AcquisitionSatInfoList ::=			SEQUENCE (SIZE (1..maxSat)) OF
		//													AcquisitionSatInfo
		

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
	
	// Decode each AcquisitionSatInfo in the list
	for (TInt i=0;i<size;i++)
		{
		CRrcAcquisitionSatInfoContent* content=CRrcAcquisitionSatInfoContent::NewLC();
		iAcquisitionSatInfoListContent->iAcquisitionSatInfoContentList.AppendL(content);
		CleanupStack::Pop(content);
		CRrcAcquisitionSatInfo* satInfo=CRrcAcquisitionSatInfo::NewLC(content);
		iAcquisitionSatInfoList.AppendL(satInfo);
		CleanupStack::Pop(satInfo);
		satInfo->DecodeFromL(aBufferToDecode, startPos,bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;
		}	

	}


void CRrcAcquisitionSatInfoList::WriteLBSAssistanceDataL(RUEPositioningGpsAcquisitionAssistanceBuilder* aGpsAcquisitionAssistanceBuilder)
	{
	
	RAcquisitionSatInfoArrayBuilder acquisitionSatInfoArrayBuilder;
	aGpsAcquisitionAssistanceBuilder->GetArrayBuilder(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray, acquisitionSatInfoArrayBuilder);	
	TInt count=iAcquisitionSatInfoListContent->iAcquisitionSatInfoContentList.Count();
	for (TInt i=0;i<count;i++)
		{
		RAcquisitionSatInfoBuilder acquisitionSatInfoBuilder;
		acquisitionSatInfoArrayBuilder.GetFieldBuilder(i, acquisitionSatInfoBuilder);
		iAcquisitionSatInfoList[i]->WriteLBSAssistanceDataL(&acquisitionSatInfoBuilder);
		}
	}

	
	
	
CRrcGpsAcquisitionAssistanceContent* CRrcGpsAcquisitionAssistanceContent::NewL()
	{
	CRrcGpsAcquisitionAssistanceContent* self = CRrcGpsAcquisitionAssistanceContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsAcquisitionAssistanceContent* CRrcGpsAcquisitionAssistanceContent::NewLC()
	{
	CRrcGpsAcquisitionAssistanceContent* self = new (ELeave) CRrcGpsAcquisitionAssistanceContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsAcquisitionAssistanceContent::~CRrcGpsAcquisitionAssistanceContent()
	{
	delete iUtranGpsReferenceTimeContent;
	delete iAcquisitionSatInfoListContent;	
	}

CRrcGpsAcquisitionAssistanceContent::CRrcGpsAcquisitionAssistanceContent()
	{
		
	}



CRrcGpsAcquisitionAssistance* CRrcGpsAcquisitionAssistance::NewL(CRrcGpsAcquisitionAssistanceContent* aGpsAcquisitionAssistanceContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsAcquisitionAssistance* self = CRrcGpsAcquisitionAssistance::NewLC(aGpsAcquisitionAssistanceContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsAcquisitionAssistance* CRrcGpsAcquisitionAssistance::NewLC(CRrcGpsAcquisitionAssistanceContent* aGpsAcquisitionAssistanceContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsAcquisitionAssistance* self = new (ELeave) CRrcGpsAcquisitionAssistance(aGpsAcquisitionAssistanceContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsAcquisitionAssistance::~CRrcGpsAcquisitionAssistance()
	{
	delete iUtranGpsReferenceTime;
	delete iAcquisitionSatInfoList;	
	}

CRrcGpsAcquisitionAssistance::CRrcGpsAcquisitionAssistance(CRrcGpsAcquisitionAssistanceContent* aGpsAcquisitionAssistanceContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iAcquisitionAssistanceContent(aGpsAcquisitionAssistanceContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcGpsAcquisitionAssistance::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsAcquisitionAssistance::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// UE-Positioning-GPS-AcquisitionAssistance ::=	SEQUENCE {
		//	 	gps-ReferenceTime					INTEGER (0..604799999),
		//		utran-GPSReferenceTime				UTRAN-GPSReferenceTime			OPTIONAL,
		//		satelliteInformationList			AcquisitionSatInfoList
		//	}

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT
	// Decode Option Bit map
	CAsn1PerOption::DecodeL(aBufferToDecode, 
				startPos, 
				bitsDecoded, 
				iAcquisitionAssistanceContent->iUtranGpsReferenceTimePresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	// Decode gps-ReferenceTime
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						604799999,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAcquisitionAssistanceContent->iGpsReferenceTime);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode UTRAN-GPSReferenceTime
	if (iAcquisitionAssistanceContent->iUtranGpsReferenceTimePresent)
		{
		iAcquisitionAssistanceContent->iUtranGpsReferenceTimeContent=CRrcUtranGpsReferenceTimeContent::NewL();
		iUtranGpsReferenceTime=CRrcUtranGpsReferenceTime::NewL(iAcquisitionAssistanceContent->iUtranGpsReferenceTimeContent);
		iUtranGpsReferenceTime->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}

	// Decode AcquisitionSatInfoList
	iAcquisitionAssistanceContent->iAcquisitionSatInfoListContent=CRrcAcquisitionSatInfoListContent::NewL();
	iAcquisitionSatInfoList=CRrcAcquisitionSatInfoList::NewL(iAcquisitionAssistanceContent->iAcquisitionSatInfoListContent);
	iAcquisitionSatInfoList->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	}

void CRrcGpsAcquisitionAssistance::WriteLBSAssistanceDataL()
	{
	//Fetch a GpsAcquisitionAssistance data builder.
	RUEPositioningGpsAcquisitionAssistanceBuilder* gpsAcquisitionAssistanceBuilder;
	iLbsAssistanceDataBuilderSet->GetDataBuilder(gpsAcquisitionAssistanceBuilder);

	gpsAcquisitionAssistanceBuilder->SetField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime, (TUint)iAcquisitionAssistanceContent->iGpsReferenceTime);
	
	iAcquisitionSatInfoList->WriteLBSAssistanceDataL(gpsAcquisitionAssistanceBuilder);
	
	if (iAcquisitionAssistanceContent->iUtranGpsReferenceTimePresent)
		{	
		iUtranGpsReferenceTime->WriteLBSAssistanceDataL(gpsAcquisitionAssistanceBuilder);	
		}
	
	TTime timeNow;
	timeNow.UniversalTime();
	gpsAcquisitionAssistanceBuilder->SetTimeStamp(timeNow);	
	}


