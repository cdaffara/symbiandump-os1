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
// SUPL END - Implementation of SUPL END encoding & decoding.
// 
//

#include "suplend.h"
#include "asn1per.h"


/**
Implemetation of CSuplEndContent Class
*/

EXPORT_C CSuplEndContent* CSuplEndContent::NewL()
	{
	CSuplEndContent* self = CSuplEndContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

EXPORT_C CSuplEndContent* CSuplEndContent::NewLC()
	{
	CSuplEndContent* self = new (ELeave) CSuplEndContent();
	CleanupStack::PushL(self);
	return self;		
	}

CSuplEndContent::CSuplEndContent()
	{		
	}

EXPORT_C CSuplEndContent::~CSuplEndContent()
	{
	}

TBool CSuplEndContent::operator==(const CSuplEndContent& other) const
	{
	TBool ret(EFalse);
	if (iPositionPresent==other.iPositionPresent &&
		iStatusCodePresent==other.iStatusCodePresent &&
		iVerPresent==other.iVerPresent)
		{
		ret=ETrue;	
		}
	return ret;
	}
	
/**
Implemetation of CSuplEnd Class
*/
CSuplEnd* CSuplEnd::NewL(CSuplEndContent* aSuplEndContent)
	{
	CSuplEnd* self = CSuplEnd::NewLC(aSuplEndContent);
	CleanupStack::Pop(self);
	return self;					
	};

CSuplEnd* CSuplEnd::NewLC(CSuplEndContent* aSuplEndContent)
	{
	CSuplEnd* self = new (ELeave) CSuplEnd(aSuplEndContent);
	CleanupStack::PushL(self);
	return self;
	};
		
CSuplEnd::CSuplEnd(CSuplEndContent* aSuplEndContent) 
: iSuplEndContent(aSuplEndContent)
	{
	}

CSuplEnd::~CSuplEnd()
	{
	}

void CSuplEnd::ConstructL()
	{
	};


void CSuplEnd::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{	
	//SUPLEND ::= SEQUENCE {
	//  position    Position OPTIONAL,
	//  statusCode  StatusCode OPTIONAL,
	//  ver         Ver OPTIONAL,
	//  ...}

	// Still use aPerEncoding, since SUPL END message is not an extended SUPL message

	if (iSuplEndContent->iPositionPresent || 
		iSuplEndContent->iStatusCodePresent || 
		iSuplEndContent->iVerPresent)
		{
		User::Leave(KErrNotSupported);	
		}
	
	// SUPL START Specific part is extensible, encode "E" bit.	
	CAsn1PerEncodingFragment* seqEbit=CAsn1PerSequence::EncodeLC(aPerEncoding.iParentBitLength,
																KAsn1PerExtensible, 
																KAsn1PerNonExtended);
																

	aPerEncoding.AddElementL(seqEbit);
	CleanupStack::PopAndDestroy(seqEbit);
	
	//Encode the Qop OPTION BIT map	
	CAsn1PerEncodingFragment* opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, 
														iSuplEndContent->iPositionPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);
	
	opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, 
								iSuplEndContent->iStatusCodePresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);
	
	opt=CAsn1PerOption::EncodeLC(aPerEncoding.iParentBitLength, 
								iSuplEndContent->iVerPresent);
	aPerEncoding.AddElementL(opt);
	CleanupStack::PopAndDestroy(opt);		
	}
		
void CSuplEnd::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//SUPLEND ::= SEQUENCE {
	//  position    Position OPTIONAL,
	//  statusCode  StatusCode OPTIONAL,
	//  ver         Ver OPTIONAL,
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
						
							
	//Decode OPTION bit	map
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplEndContent->iPositionPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	
	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplEndContent->iStatusCodePresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iSuplEndContent->iVerPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;



	if (iSuplEndContent->iPositionPresent || 
		iSuplEndContent->iStatusCodePresent || 
		iSuplEndContent->iVerPresent)
		{
		// Don't decode any more bits
		}	
	}












