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
// Implementation of encoding and decoding GPS UTC Model
// 
//

#include "rrcgpsutcmodel.h"
#include <lbs/lbsassistancedatabuilderset.h>

CRrcGpsUTCModelContent* CRrcGpsUTCModelContent::NewL()
	{
	CRrcGpsUTCModelContent* self = CRrcGpsUTCModelContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsUTCModelContent* CRrcGpsUTCModelContent::NewLC()
	{
	CRrcGpsUTCModelContent* self = new (ELeave) CRrcGpsUTCModelContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsUTCModelContent::~CRrcGpsUTCModelContent()
	{
	}


CRrcGpsUTCModel* CRrcGpsUTCModel::NewL(CRrcGpsUTCModelContent* aGpsUTCModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsUTCModel* self = CRrcGpsUTCModel::NewLC(aGpsUTCModelContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsUTCModel* CRrcGpsUTCModel::NewLC(CRrcGpsUTCModelContent* aGpsUTCModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsUTCModel* self = new (ELeave) CRrcGpsUTCModel(aGpsUTCModelContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsUTCModel::~CRrcGpsUTCModel()
	{
	}

CRrcGpsUTCModel::CRrcGpsUTCModel(CRrcGpsUTCModelContent* aGpsUTCModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iGpsUTCModelContent(aGpsUTCModelContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcGpsUTCModel::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsUTCModel::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// UE-Positioning-GPS-UTC-Model ::=				SEQUENCE {
		//	a1									BIT STRING (SIZE (24)),
		//	a0									BIT STRING (SIZE (32)),
		//	t-ot								BIT STRING (SIZE (8)),
		//	wn-t								BIT STRING (SIZE (8)),
		//	delta-t-LS							BIT STRING (SIZE (8)),
		//	wn-lsf								BIT STRING (SIZE (8)),
		//	dn									BIT STRING (SIZE (8)),
		//	delta-t-LSF							BIT STRING (SIZE (8))
		//}


	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT		
	RBuf8 stringData;
	TInt stringLen(0);
	
	// Decode a1
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

	iGpsUTCModelContent->iA1=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;


	// Decode a0
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

	iGpsUTCModelContent->iA0=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode t-ot
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

	iGpsUTCModelContent->iTot=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode wn-t
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

	iGpsUTCModelContent->iWnt=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;


	// Decode delta-t-LS
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

	iGpsUTCModelContent->iDeltatLS=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode wn-lsf
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

	iGpsUTCModelContent->iWnLsf=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode dn
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

	iGpsUTCModelContent->iDn=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;

	// Decode delta-t-LSF
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

	iGpsUTCModelContent->iDeltatLSF=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
		
	}

void CRrcGpsUTCModel::WriteLBSAssistanceDataL()
	{
	//Fetch a GpsUTCModel data item.
	RUEPositioningGpsUtcModelBuilder* gpsUtcModelBuilder;
	iLbsAssistanceDataBuilderSet->GetDataBuilder(gpsUtcModelBuilder);

	TUint value=CAsn1PerUtil::CovertStringToUintL(iGpsUTCModelContent->iA1, 3);	
	gpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EA1, value);
		
	value=CAsn1PerUtil::CovertStringToUintL(iGpsUTCModelContent->iA0, 4);	
	gpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EA0, value);
	
	gpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::ETot, *(TUint*)iGpsUTCModelContent->iTot.Ptr());
	gpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EWnt, *(TUint*)iGpsUTCModelContent->iWnt.Ptr());	
	gpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EDeltatLS, *(TUint*)iGpsUTCModelContent->iDeltatLS.Ptr());	
	gpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EWnlsf, *(TUint*)iGpsUTCModelContent->iWnLsf.Ptr());	
	gpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EDn, *(TUint*)iGpsUTCModelContent->iDn.Ptr());	
	gpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EDeltatLSF, *(TUint*)iGpsUTCModelContent->iDeltatLSF.Ptr());	
	
	TTime timeNow;
	timeNow.UniversalTime();
	gpsUtcModelBuilder->SetTimeStamp(timeNow);		
	}
	
	




