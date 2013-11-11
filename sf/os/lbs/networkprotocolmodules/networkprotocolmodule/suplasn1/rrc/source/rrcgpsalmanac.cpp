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
// Implementation of encoding and decoding GPS Almanac
// 
//

#include "rrcgpsalmanac.h"
#include <lbs/lbsassistancedatabuilderset.h>



CRrcAlmanacSatInfoContent* CRrcAlmanacSatInfoContent::NewL()
	{
	CRrcAlmanacSatInfoContent* self = CRrcAlmanacSatInfoContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAlmanacSatInfoContent* CRrcAlmanacSatInfoContent::NewLC()
	{
	CRrcAlmanacSatInfoContent* self = new (ELeave) CRrcAlmanacSatInfoContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcAlmanacSatInfoContent::~CRrcAlmanacSatInfoContent()
	{
	}

CRrcAlmanacSatInfo* CRrcAlmanacSatInfo::NewL(CRrcAlmanacSatInfoContent* aAlmanacSatInfoContent)
	{
	CRrcAlmanacSatInfo* self = CRrcAlmanacSatInfo::NewLC(aAlmanacSatInfoContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAlmanacSatInfo* CRrcAlmanacSatInfo::NewLC(CRrcAlmanacSatInfoContent* aAlmanacSatInfoContent)
	{
	CRrcAlmanacSatInfo* self = new (ELeave) CRrcAlmanacSatInfo(aAlmanacSatInfoContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcAlmanacSatInfo::~CRrcAlmanacSatInfo()
	{
	}

CRrcAlmanacSatInfo::CRrcAlmanacSatInfo(CRrcAlmanacSatInfoContent* aAlmanacSatInfoContent)
:iAlmanacSatInfoContent(aAlmanacSatInfoContent)
	{
	}

void CRrcAlmanacSatInfo::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcAlmanacSatInfo::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//AlmanacSatInfo ::=					SEQUENCE {
	//	dataID								INTEGER (0..3),
	//	satID								SatID,
	//	e									BIT STRING (SIZE (16)),
	//	t-oa								BIT STRING (SIZE (8)),
	//	deltaI								BIT STRING (SIZE (16)),
	//	omegaDot							BIT STRING (SIZE (16)),
	//	satHealth							BIT STRING (SIZE (8)),
	//	a-Sqrt								BIT STRING (SIZE (24)),
	//	omega0								BIT STRING (SIZE (24)),
	//	m0									BIT STRING (SIZE (24)),
	//	omega								BIT STRING (SIZE (24)),
	//	af0									BIT STRING (SIZE (11)),
	//	af1									BIT STRING (SIZE (11))
	//}
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" Bit
	// Decode dataID
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						3,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAlmanacSatInfoContent->iDataID);
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
						iAlmanacSatInfoContent->iSatID);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	RBuf8 stringData;
	TInt stringLen(0);
	
	// Decode iE
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

	iAlmanacSatInfoContent->iE=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode iToa
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

	iAlmanacSatInfoContent->iToa=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iDeltaI
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

	iAlmanacSatInfoContent->iDeltaI=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iOmegaDot
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

	iAlmanacSatInfoContent->iOmegaDot=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iSatHealth
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

	iAlmanacSatInfoContent->iSatHealth=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iASqrt
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

	iAlmanacSatInfoContent->iASqrt=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode iOmega0
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

	iAlmanacSatInfoContent->iOmega0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iM0
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

	iAlmanacSatInfoContent->iM0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iOmega
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

	iAlmanacSatInfoContent->iOmega=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode iAf0
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						11,
						11,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);

	iAlmanacSatInfoContent->iAf0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode iAf1
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						11,
						11,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);

	iAlmanacSatInfoContent->iAf1=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	}

void CRrcAlmanacSatInfo::WriteLBSAssistanceDataL(RAlmanacSatInfoBuilder* aAlmanacSatInfoBuilder)
	{
	
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EDataID, (TUint)iAlmanacSatInfoContent->iDataID);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::ESatID, (TUint)iAlmanacSatInfoContent->iSatID);
	
	TUint value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iE, 2);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EE, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iToa, 1);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EToa, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iDeltaI, 2);	
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EDeltaI, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iOmegaDot, 2);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EOmegaDot, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iSatHealth, 1);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::ESatHealth, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iASqrt, 3);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EASqrt, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iOmega0, 3);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EOmega0, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iM0, 3);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EM0, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iOmega, 3);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EOmega, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iAf0, 2);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EAf0, value);
	
	value=CAsn1PerUtil::CovertStringToUintL(iAlmanacSatInfoContent->iAf1, 2);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EAf1, value);
	}







CRrcAlmanacSatInfoListContent* CRrcAlmanacSatInfoListContent::NewL()
	{
	CRrcAlmanacSatInfoListContent* self = CRrcAlmanacSatInfoListContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAlmanacSatInfoListContent* CRrcAlmanacSatInfoListContent::NewLC()
	{
	CRrcAlmanacSatInfoListContent* self = new (ELeave) CRrcAlmanacSatInfoListContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcAlmanacSatInfoListContent::~CRrcAlmanacSatInfoListContent()
	{
	iAlmanacSatInfoList.ResetAndDestroy();
	}


CRrcAlmanacSatInfoList* CRrcAlmanacSatInfoList::NewL(CRrcAlmanacSatInfoListContent* aAlmanacSatInfoListContent)
	{
	CRrcAlmanacSatInfoList* self = CRrcAlmanacSatInfoList::NewLC(aAlmanacSatInfoListContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcAlmanacSatInfoList* CRrcAlmanacSatInfoList::NewLC(CRrcAlmanacSatInfoListContent* aAlmanacSatInfoListContent)
	{
	CRrcAlmanacSatInfoList* self = new (ELeave) CRrcAlmanacSatInfoList(aAlmanacSatInfoListContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcAlmanacSatInfoList::~CRrcAlmanacSatInfoList()
	{
	iAlmanacSatInfoList.ResetAndDestroy();
	}

CRrcAlmanacSatInfoList::CRrcAlmanacSatInfoList(CRrcAlmanacSatInfoListContent* aAlmanacSatInfoListContent)
:iAlmanacSatInfoListContent(aAlmanacSatInfoListContent)
	{
	}

void CRrcAlmanacSatInfoList::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcAlmanacSatInfoList::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	// AlmanacSatInfoList ::=				SEQUENCE (SIZE (1..maxSat)) OF
	//										AlmanacSatInfo


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
		CRrcAlmanacSatInfoContent* content=CRrcAlmanacSatInfoContent::NewLC();
		iAlmanacSatInfoListContent->iAlmanacSatInfoList.AppendL(content);
		CleanupStack::Pop(content);
		CRrcAlmanacSatInfo* satInfo=CRrcAlmanacSatInfo::NewLC(content);
		iAlmanacSatInfoList.AppendL(satInfo);
		CleanupStack::Pop(satInfo);
		satInfo->DecodeFromL(aBufferToDecode, startPos,bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;
		}	

	}


void CRrcAlmanacSatInfoList::WriteLBSAssistanceDataL(RUEPositioningGpsAlmanacBuilder* aGpsAlmanacBuilder)
	{
	// Populate the data item
	RAlmanacSatInfoArrayBuilder almanacSatInfoArrayBuilder;
	aGpsAlmanacBuilder->GetArrayBuilder(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray, almanacSatInfoArrayBuilder);		
	TInt count=iAlmanacSatInfoListContent->iAlmanacSatInfoList.Count();
	for (TInt i=0;i<count;i++)
		{
		RAlmanacSatInfoBuilder almanacSatInfoBuilder;	
		almanacSatInfoArrayBuilder.GetFieldBuilder(i, almanacSatInfoBuilder);
		iAlmanacSatInfoList[i]->WriteLBSAssistanceDataL(&almanacSatInfoBuilder);	
		}
	}







CRrcGpsAlmanacContent* CRrcGpsAlmanacContent::NewL()
	{
	CRrcGpsAlmanacContent* self = CRrcGpsAlmanacContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsAlmanacContent* CRrcGpsAlmanacContent::NewLC()
	{
	CRrcGpsAlmanacContent* self = new (ELeave) CRrcGpsAlmanacContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsAlmanacContent::~CRrcGpsAlmanacContent()
	{
	delete iAlmanacSatInfoListContent;
	}



CRrcGpsAlmanac* CRrcGpsAlmanac::NewL(CRrcGpsAlmanacContent* aGpsAlmanacContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsAlmanac* self = CRrcGpsAlmanac::NewLC(aGpsAlmanacContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsAlmanac* CRrcGpsAlmanac::NewLC(CRrcGpsAlmanacContent* aGpsAlmanacContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsAlmanac* self = new (ELeave) CRrcGpsAlmanac(aGpsAlmanacContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcGpsAlmanac::~CRrcGpsAlmanac()
	{
	delete iAlmanacSatInfoList;
	}

CRrcGpsAlmanac::CRrcGpsAlmanac(CRrcGpsAlmanacContent* aGpsAlmanacContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iGpsAlmanacContent(aGpsAlmanacContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcGpsAlmanac::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsAlmanac::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		//UE-Positioning-GPS-Almanac ::=					SEQUENCE {
		//	wn-a								BIT STRING (SIZE (8)),
		//	almanacSatInfoList					AlmanacSatInfoList,
		//	sv-GlobalHealth						BIT STRING (SIZE (364))				OPTIONAL
		//}


	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT
	// Decode option bit for sv-GlobalHealth	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAlmanacContent->iSvGlobalHealthPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	// Decode wn-a
	
	RBuf8 stringData;
	TInt stringLen(0);
	
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

	iGpsAlmanacContent->iWna=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode AlmanacSatInfoList
	iGpsAlmanacContent->iAlmanacSatInfoListContent=CRrcAlmanacSatInfoListContent::NewL();
	iAlmanacSatInfoList=CRrcAlmanacSatInfoList::NewL(iGpsAlmanacContent->iAlmanacSatInfoListContent);
	iAlmanacSatInfoList->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	
	if (iGpsAlmanacContent->iSvGlobalHealthPresent)
		{
		// Decode sv-GlobalHealth
		CAsn1PerBitString::DecodeL(aBufferToDecode,
							startPos,
							KAsn1LengthNonVariable,
							KAsn1PerMaxConstrained,
							364,
							364,	
							KAsn1PerNonExtensible,
							isExtended,
							bitsDecoded, 
							stringData,
							stringLen);

		iGpsAlmanacContent->SvGlobalHealth=stringData;
		stringData.Close();
		aDecodedBits+=bitsDecoded;	
		startPos+=bitsDecoded;			
		}
	}

void CRrcGpsAlmanac::ConstructL()
	{
	}
	
void CRrcGpsAlmanac::WriteLBSAssistanceDataL()
	{
	RUEPositioningGpsAlmanacBuilder* gpsAlmanacBuilder;
	iLbsAssistanceDataBuilderSet->GetDataBuilder(gpsAlmanacBuilder);

	gpsAlmanacBuilder->SetField(TUEPositioningGpsAlmanac::EWnA, *(TUint*)iGpsAlmanacContent->iWna.Ptr());
	if (iGpsAlmanacContent->iSvGlobalHealthPresent)
		{
		gpsAlmanacBuilder->SetField(TUEPositioningGpsAlmanac::ESvGlobalHealth, iGpsAlmanacContent->SvGlobalHealth);	
		}
	
	iAlmanacSatInfoList->WriteLBSAssistanceDataL(gpsAlmanacBuilder);
	
	TTime timeNow;
	timeNow.UniversalTime();
	gpsAlmanacBuilder->SetTimeStamp(timeNow);	
	}



