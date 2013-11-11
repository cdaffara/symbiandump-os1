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
// CAsn1PerSequence.CPP
// ASN.1 packed encoding rules - Sequence encoding
// 
//

#include "asn1per.h"


/**
Implementation of CAsn1PerSequenceof
*/

CAsn1PerEncodingFragment* CAsn1PerSequenceof::EncodeL(const TInt aExistingLength,
											const TInt aSize,
											const TBool aIsMinSizeConstrained,
											const TInt aMinSize,
											const TBool aIsMaxSizeConstrained,
											const TInt aMaxSize,
											const TBool aIsExtensible,
											const TBool aIsExtended)
	{
	CAsn1PerEncodingFragment* encodeFragment=CAsn1PerSequenceof::EncodeLC(aExistingLength,
																		aSize,
																		aIsMinSizeConstrained,
																		aMinSize,
																		aIsMaxSizeConstrained,
																		aMaxSize,
																		aIsExtensible,
																		aIsExtended);
	CleanupStack::Pop(encodeFragment);
	return encodeFragment;
	}


CAsn1PerEncodingFragment* CAsn1PerSequenceof::EncodeLC(const TInt aExistingLength,
											const TInt aSize,
											const TBool aIsMinSizeConstrained,
											const TInt aMinSize,
											const TBool aIsMaxSizeConstrained,
											const TInt aMaxSize,
											const TBool aIsExtensible,
											const TBool aIsExtended)
	{
	CAsn1PerEncodingFragment* encodeFragment=CAsn1PerEncodingFragment::NewLC(aExistingLength, EFalse);
	if (aIsExtensible)
		{
		
		CAsn1PerEncodingFragment* ebit=CAsn1PerBoolean::EncodeLC(aExistingLength, 
																aIsExtended, 
																EFalse);
		encodeFragment->AddElementL(ebit);
		CleanupStack::PopAndDestroy(ebit);

		if (aIsExtended)
			{
			User::Leave(KErrNotSupported);
			}		
		}

	if (aIsMinSizeConstrained && aIsMaxSizeConstrained)
		{
		CAsn1PerUtil::UnAlignEncodeIntegerL(aMaxSize, aMinSize, aSize, EFalse, encodeFragment);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}


	return encodeFragment;


	}


void CAsn1PerSequenceof::DecodeL(const TDesC8& aBuf,
								const TInt aStartBit,
								const TBool aIsMinSizeConstrained,
								const TInt aMinSize,
								const TBool aIsMaxSizeConstrained,
								const TInt aMaxSize,
								const TBool aIsExtensible,
								TInt& aBitsDecoded,
								TInt& aSize,
								TBool& aIsExtended)
	{

	if(aStartBit>=(aBuf.Length()*KOctetBitsLength))
		{
		User::Leave(KErrUnderflow);	
		}


	TInt currentBitPos=aStartBit;
	TInt bitDecoded=0;
	aBitsDecoded=0;
	aIsExtended=EFalse;

	if (aIsExtensible)
		{
		CAsn1PerBoolean::DecodeL(aBuf, EFalse, currentBitPos, bitDecoded, aIsExtended);
		currentBitPos+=bitDecoded;
		aBitsDecoded+=bitDecoded;
		bitDecoded=0;					
		}

	if (aIsExtended)
		{
		// TODO: decode differently
		//

		}
	else
		{
		TBool iteratorExt(EFalse);
		CAsn1PerInt::DecodeL(aBuf, 
							currentBitPos,
							aIsMinSizeConstrained,
							aMinSize,
							aIsMaxSizeConstrained,
							aMaxSize,
							EFalse,
							bitDecoded,
							iteratorExt,
							aSize);
							
		currentBitPos+=bitDecoded;
		aBitsDecoded+=bitDecoded;
		bitDecoded=0;
		}

	}


