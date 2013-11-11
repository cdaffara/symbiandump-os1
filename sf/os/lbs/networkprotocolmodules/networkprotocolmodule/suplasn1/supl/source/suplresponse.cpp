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
// SUPL RESPONSE - Implementation of SUPL RESPONSE encoding & decoding.
// 
//

#include "suplresponse.h"
#include "asn1per.h"


/**
Implemetation of CSuplResponseContent Class
*/

EXPORT_C CSuplResponseContent* CSuplResponseContent::NewL()
	{
	CSuplResponseContent* self = CSuplResponseContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

EXPORT_C CSuplResponseContent* CSuplResponseContent::NewLC()
	{
	CSuplResponseContent* self = new (ELeave) CSuplResponseContent();
	CleanupStack::PushL(self);
	return self;		
	}
	
CSuplResponseContent::CSuplResponseContent()
	{		
	}

EXPORT_C CSuplResponseContent::~CSuplResponseContent()
	{
	}

TBool CSuplResponseContent::operator==(const CSuplResponseContent& other) const
	{
	return  (iPosMethod==other.iPosMethod &&
			iSlpAddressPresent==other.iSlpAddressPresent &&
			iSetAuthKeyPresent==other.iSetAuthKeyPresent &&
			iKeyId4Present==other.iKeyId4Present);
	}

	
/**
Implemetation of CSuplResponse Class
*/
CSuplResponse* CSuplResponse::NewL(CSuplResponseContent* aSuplResponseContent)
	{
	CSuplResponse* self = CSuplResponse::NewLC(aSuplResponseContent);
	CleanupStack::Pop(self);
	return self;							
	}

CSuplResponse* CSuplResponse::NewLC(CSuplResponseContent* aSuplResponseContent)
	{
	CSuplResponse* self = new (ELeave) CSuplResponse(aSuplResponseContent);
	CleanupStack::PushL(self);
	return self;
	};
		
CSuplResponse::CSuplResponse(CSuplResponseContent* aSuplResponseContent) 
: iSuplResponseContent(aSuplResponseContent)
	{
	}

CSuplResponse::~CSuplResponse()
	{
	}

void CSuplResponse::ConstructL()
	{
	};

void CSuplResponse::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{
	//SUPLRESPONSE ::= SEQUENCE {
	//  posMethod     PosMethod,
	//  sLPAddress    SLPAddress OPTIONAL,
	//  sETAuthKey    SETAuthKey OPTIONAL,
	//  keyIdentity4  KeyIdentity4 OPTIONAL,
	//  ...}
	
	// PosMethod ::= ENUMERATED {
	//   agpsSETassisted(0), agpsSETbased(1), agpsSETassistedpref(2),
	//   agpsSETbasedpref(3), autonomousGPS(4), aFLT(5), eCID(6), eOTD(7), oTDOA(8),
	//   noPosition(9), 
	//   ...}
	
	// Still use aPerEncoding, since SUPL START message is not an extended SUPL message
	
	// SUPL START Specific part is extensible, encode "E" bit.	
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);


	//Encode the option bit map	
	CAsn1PerEncodingFragment* opt=CAsn1PerOption::EncodeLC(0, iSuplResponseContent->iSlpAddressPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

	opt=CAsn1PerOption::EncodeLC(0, iSuplResponseContent->iSetAuthKeyPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

	opt=CAsn1PerOption::EncodeLC(0, iSuplResponseContent->iKeyId4Present);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);

	// Encode Pos method
	CAsn1PerEncodingFragment* posMethod=CAsn1PerEnum::EncodeLC(aPerEncoding.iParentBitLength,
															iSuplResponseContent->iPosMethod,
															9,
															KAsn1PerExtensible,
															KAsn1PerNonExtended,
															KAsn1PerNoPadding);
	aPerEncoding.AddElementL(posMethod);
	CleanupStack::PopAndDestroy(posMethod);	
	
	if (iSuplResponseContent->iSlpAddressPresent || iSuplResponseContent->iKeyId4Present || iSuplResponseContent->iSetAuthKeyPresent)
		{
		User::Leave(KErrNotSupported);	
		}
	}
		
void CSuplResponse::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//SUPLRESPONSE ::= SEQUENCE {
	//  posMethod     PosMethod,
	//  sLPAddress    SLPAddress OPTIONAL,
	//  sETAuthKey    SETAuthKey OPTIONAL,
	//  keyIdentity4  KeyIdentity4 OPTIONAL,
	//  ...}
	
	// PosMethod ::= ENUMERATED {
	//   agpsSETassisted(0), agpsSETbased(1), agpsSETassistedpref(2),
	//   agpsSETbasedpref(3), autonomousGPS(4), aFLT(5), eCID(6), eOTD(7), oTDOA(8),
	//   noPosition(9), 
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
						
							
	//Decode OPTION bit	map
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplResponseContent->iSlpAddressPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	

	
	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplResponseContent->iSetAuthKeyPresent);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	

	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplResponseContent->iKeyId4Present);
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;	
	
	//Decode pos method
	TInt posMethod;
	CAsn1PerEnum::DecodeL(aBufferToDecode,
						startPos,
						9,
						KAsn1PerExtensible,
						KAsn1PerNoPadding,
						bitsDecoded,
						isExtended,
						posMethod
						);
						
	iSuplResponseContent->iPosMethod=(TSuplPosMethod)posMethod;
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	if (iSuplResponseContent->iSlpAddressPresent || iSuplResponseContent->iKeyId4Present || iSuplResponseContent->iSetAuthKeyPresent)
		{
		User::Leave(KErrNotSupported);	
		}
	
	}












