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
// Implementation of encoding and decoding GPS SAT LIST
// 
//

#include "rrcgpsbadsatlist.h"
#include <lbs/lbsassistancedatabuilderset.h>


CRrcGpsBadSatListContent* CRrcGpsBadSatListContent::NewL()
	{
	CRrcGpsBadSatListContent* self = CRrcGpsBadSatListContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsBadSatListContent* CRrcGpsBadSatListContent::NewLC()
	{
	CRrcGpsBadSatListContent* self = new (ELeave) CRrcGpsBadSatListContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsBadSatListContent::~CRrcGpsBadSatListContent()
	{
	}


CRrcGpsBadSatList* CRrcGpsBadSatList::NewL(CRrcGpsBadSatListContent* aGpsBadSatListContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsBadSatList* self = CRrcGpsBadSatList::NewLC(aGpsBadSatListContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsBadSatList* CRrcGpsBadSatList::NewLC(CRrcGpsBadSatListContent* aGpsBadSatListContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsBadSatList* self = new (ELeave) CRrcGpsBadSatList(aGpsBadSatListContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsBadSatList::~CRrcGpsBadSatList()
	{
	}

CRrcGpsBadSatList::CRrcGpsBadSatList(CRrcGpsBadSatListContent* aGpsBadSatListContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iBadSatListContent(aGpsBadSatListContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcGpsBadSatList::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);	
	}

void CRrcGpsBadSatList::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// BadSatList ::=						SEQUENCE (SIZE (1..maxSat)) OF
		//										INTEGER (0..63)	
		
		
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	TInt size;
	// Decode BadSatList
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
		TUint8 badSat;
		CAsn1PerInt::DecodeL(aBufferToDecode, 
							startPos,
							KAsn1PerMinConstrained,
							0,
							KAsn1PerMaxConstrained,
							63,
							KAsn1PerNonExtensible,						
							bitsDecoded,
							isExtended,
							badSat);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;
		iBadSatListContent->iBadSatList[i]=badSat;
		}
	iBadSatListContent->iNumberOfBadSat=size;
	}


void CRrcGpsBadSatList::WriteLBSAssistanceDataL()
	{
	//Fetch a GpsBadSatList data item.
	RBadSatListBuilder* badSatListBuilder;
	iLbsAssistanceDataBuilderSet->GetDataBuilder(badSatListBuilder);

	// Populate the data item
	RBadSatArrayBuilder badSatArrayBuilder;
	badSatListBuilder->GetArrayBuilder(TBadSatList::EBadSatIdArray, badSatArrayBuilder);		
		
	for (TInt i=0;i<iBadSatListContent->iNumberOfBadSat;i++)
		{	
		badSatArrayBuilder.SetElement(i, iBadSatListContent->iBadSatList[i]);
		}
		
	TTime timeNow;
	timeNow.UniversalTime();
	badSatListBuilder->SetTimeStamp(timeNow);	
	}

