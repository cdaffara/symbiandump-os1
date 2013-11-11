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
// ASN.1 packed encoding rules - Encoding & Decoding of Enum value
// 
//

#include "asn1per.h"

CAsn1PerEncodingFragment* CAsn1PerEnum::EncodeL(TUint aExistingBitLength,
												const TInt aChoice,
												const TInt aMaxChoice,
												const TBool aIsExtensible,
												const TBool aIsExtended,
												const TBool aIsPaddingNeeded)
	{
	
	CAsn1PerEncodingFragment* encodeFragment=CAsn1PerEncodingFragment::NewLC(aExistingBitLength, aIsPaddingNeeded);
	
	// Handle extensible integer type
	if (aIsExtensible)
		{	
		CAsn1PerEncodingFragment* ebit=CAsn1PerBoolean::EncodeLC(aExistingBitLength, 
															aIsExtended, 
															EFalse);
														
		encodeFragment->AddElementL(ebit);
		CleanupStack::PopAndDestroy(ebit);									

		if (aIsExtended)
			{
			// TODO:Encode the extended integer Separately
			User::Leave(KErrNotSupported);
			CleanupStack::Pop(encodeFragment);
			return encodeFragment;
			}
		}
	
	if (aMaxChoice>KConstraintLengthLimit || aIsExtended)
		{
		// TODO: decode differently
		//
		User::Leave(KErrNotSupported);
		}
	else
		{
		CAsn1PerUtil::UnAlignEncodeIntegerL(aMaxChoice, 0, aChoice, EFalse, encodeFragment);	
		}
		
	CleanupStack::Pop(encodeFragment);
	return encodeFragment;
		
	}

CAsn1PerEncodingFragment* CAsn1PerEnum::EncodeLC(TUint aExistingBitLength,
												const TInt aChoice,
												const TInt aMaxChoice,
												const TBool aIsExtensible,
												const TBool aIsExtended,
												const TBool aIsPaddingNeeded)
	{
	
	CAsn1PerEncodingFragment* encodeFragment=EncodeL(aExistingBitLength, 
													aChoice,
													aMaxChoice,
													aIsExtensible,
													aIsExtended,
													aIsPaddingNeeded);
	
		
	CleanupStack::PushL(encodeFragment);
	return encodeFragment;
		
	}



void CAsn1PerEnum::DecodeL(const TDesC8& aBuf,
							const TInt aStartBit,
							const TInt aMaxChoice,
							const TBool aIsExtensible,
							const TBool /*aIsPadded*/,
							TInt& aBitsDecoded,
							TBool& aIsExtended,
							TInt& aChoiceValue
							)
							
	{
	if(aStartBit>=(aBuf.Length()*KOctetBitsLength))
		{
		User::Leave(KErrUnderflow);	
		}
	
	//Enum start from 0
	
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
	
	if (aMaxChoice>KConstraintLengthLimit || aIsExtended)
		{
		// TODO: decode differently
		//
		}
	else
		{
		TBool choiceExt(EFalse);
		CAsn1PerInt::DecodeL(aBuf, 
							currentBitPos,
							ETrue,
							0,
							ETrue,
							aMaxChoice,
							EFalse,
							bitDecoded,
							choiceExt,
							aChoiceValue);
							
		currentBitPos+=bitDecoded;
		aBitsDecoded+=bitDecoded;
		bitDecoded=0;
		}
	}






