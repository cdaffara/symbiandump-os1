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
// RRCGPSREFERENCELOCATION.CPP
// Implementation of encoding and decoding GPS Reference Location
// 
//

#include "rrcgpsionosphericmodel.h"
#include <lbs/lbsassistancedatabuilderset.h>


CRrcGpsIonosphericModelContent* CRrcGpsIonosphericModelContent::NewL()
	{
	CRrcGpsIonosphericModelContent* self = CRrcGpsIonosphericModelContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsIonosphericModelContent* CRrcGpsIonosphericModelContent::NewLC()
	{
	CRrcGpsIonosphericModelContent* self = new (ELeave) CRrcGpsIonosphericModelContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsIonosphericModelContent::~CRrcGpsIonosphericModelContent()
	{
	}

CRrcGpsIonosphericModelContent::CRrcGpsIonosphericModelContent()
	{		
	}



CRrcGpsIonosphericModel* CRrcGpsIonosphericModel::NewL(CRrcGpsIonosphericModelContent* aGpsIonosphericModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsIonosphericModel* self = CRrcGpsIonosphericModel::NewLC(aGpsIonosphericModelContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsIonosphericModel* CRrcGpsIonosphericModel::NewLC(CRrcGpsIonosphericModelContent* aGpsIonosphericModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsIonosphericModel* self = new (ELeave) CRrcGpsIonosphericModel(aGpsIonosphericModelContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsIonosphericModel::~CRrcGpsIonosphericModel()
	{
	}

CRrcGpsIonosphericModel::CRrcGpsIonosphericModel(CRrcGpsIonosphericModelContent* aGpsIonosphericModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iIonosphericModelContent(aGpsIonosphericModelContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcGpsIonosphericModel::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsIonosphericModel::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		//UE-Positioning-GPS-IonosphericModel ::=		SEQUENCE {
		//	alfa0								BIT STRING (SIZE (8)),
		//	alfa1								BIT STRING (SIZE (8)),
		//	alfa2								BIT STRING (SIZE (8)),
		//	alfa3								BIT STRING (SIZE (8)),
		//	beta0								BIT STRING (SIZE (8)),
		//	beta1								BIT STRING (SIZE (8)),
		//	beta2								BIT STRING (SIZE (8)),
		//	beta3								BIT STRING (SIZE (8))
		//}

	
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT		
	RBuf8 stringData;
	TInt stringLen(0);
	
	
	// Decode alfa0
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
	iIonosphericModelContent->iAlfa0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode alfa1
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

	iIonosphericModelContent->iAlfa1=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode alfa2
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

	iIonosphericModelContent->iAlfa2=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	

	// Decode alfa3
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

	iIonosphericModelContent->iAlfa3=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode beta0
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

	iIonosphericModelContent->iBeta0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;


	// Decode beta1
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

	iIonosphericModelContent->iBeta1=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;


	// Decode beta2
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

	iIonosphericModelContent->iBeta2=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode beta3
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

	iIonosphericModelContent->iBeta3=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	}

void CRrcGpsIonosphericModel::WriteLBSAssistanceDataL()
	{
	//Fetch a GpsIonosphericModel data item.
	RUEPositioningGpsIonosphericModelBuilder* gpsIonosphericModelBuilder;
	iLbsAssistanceDataBuilderSet->GetDataBuilder(gpsIonosphericModelBuilder);
		
	gpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa0, *(TUint*)iIonosphericModelContent->iAlfa0.Ptr());	
	gpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa1, *(TUint*)iIonosphericModelContent->iAlfa1.Ptr());	
	gpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa2, *(TUint*)iIonosphericModelContent->iAlfa2.Ptr());	
	gpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa3, *(TUint*)iIonosphericModelContent->iAlfa3.Ptr());	
	gpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta0, *(TUint*)iIonosphericModelContent->iBeta0.Ptr());	
	gpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta1, *(TUint*)iIonosphericModelContent->iBeta1.Ptr());	
	gpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta2, *(TUint*)iIonosphericModelContent->iBeta2.Ptr());	
	gpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta3, *(TUint*)iIonosphericModelContent->iBeta3.Ptr());
	
	TTime timeNow;
	timeNow.UniversalTime();
	gpsIonosphericModelBuilder->SetTimeStamp(timeNow);	
	}


