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
// ASN.1 packed encoding rules - Encoding & Decoding of TInt
// 
//

#include "asn1per.h"

CAsn1PerEncodingFragment* CAsn1PerInt::EncodeLC(TUint aExistingBitLength,
												const TInt64 aValue,
												const TBool aMinValueIsConstrained,
												const TInt64 aMinValue,
												const TBool aMaxValueIsConstrained,
												const TInt64 aMaxValue,
												const TBool aIsExtensible,
												const TBool aIsExtended,
												const TBool aIsPaddingNeeded)
	{	
	CAsn1PerEncodingFragment* encodeFragment=CAsn1PerEncodingFragment::NewLC(aExistingBitLength, aIsPaddingNeeded);
	
	// Handle extensible integer type
	if (aIsExtensible)
		{
		User::Leave(KErrNotSupported);	
		CAsn1PerEncodingFragment* ebit=CAsn1PerBoolean::EncodeL(aExistingBitLength, 
															aIsExtended, 
															EFalse);

		CleanupStack::PushL(ebit);														
		encodeFragment->AddElementL(ebit);
		CleanupStack::PopAndDestroy(ebit);
											
		if (aIsExtended)
			{
			// TODO:Encode the extended integer Separately
			CleanupStack::Pop(encodeFragment);
			return encodeFragment;
			}
		}

	// Start to encode the non-extended or non-extensible integer	
	
	// Constrained integer types unaligned encoding 
	if (aMinValueIsConstrained && aMaxValueIsConstrained)
		{
		if (aMinValue>aMaxValue || aValue<aMinValue || aValue>aMaxValue)
			{
			User::Leave(KErrArgument);				
			}
		
		// Fixed the integer
		if (aMinValue==aMaxValue)
			{			
			return encodeFragment;
			}
			
		//Encode the integer 			
		CAsn1PerUtil::UnAlignEncodeIntegerL(aMaxValue, aMinValue, aValue, EFalse, encodeFragment);		
		}
	else
		{
		User::Leave(KErrNotSupported);
		}			
		
	return encodeFragment;
	}


CAsn1PerEncodingFragment* CAsn1PerInt::EncodeL(TUint aExistingBitLength,
												const TInt64 aValue,
												const TBool aMinValueIsConstrained,
												const TInt64 aMinValue,
												const TBool aMaxValueIsConstrained,
												const TInt64 aMaxValue,
												const TBool aIsExtensible,
												const TBool aIsExtended,
												const TBool aIsPaddingNeeded)

	{
	CAsn1PerEncodingFragment* encodeFragment=EncodeLC(aExistingBitLength,
													aValue,
													aMinValueIsConstrained,
													aMinValue,
													aMaxValueIsConstrained,
													aMaxValue,
													aIsExtensible,
													aIsExtended,
													aIsPaddingNeeded);
	
	CleanupStack::Pop(encodeFragment);
	return encodeFragment;		
	}


void CAsn1PerInt::DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TInt64& aValue)
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
		User::Leave(KErrNotSupported);
		CAsn1PerBoolean::DecodeL(aBuf, EFalse, currentBitPos, bitDecoded, aIsExtended);
		currentBitPos+=bitDecoded;
		aBitsDecoded+=bitDecoded;
		bitDecoded=0;	
		}
	
	//Only Handle Constraint integer
	if (aMinValueIsConstrained && aMaxValueIsconstrained)
		{
		//Calculate the range
		TInt64 range=aMaxValue-aMinValue;
		if (range<0)
			{
			User::Leave(KErrArgument);	
			}
		
		// range include only one integer.
		if (range==0)
			{
			return;	
			}
		
		//Calculate the bit to decode
							
		while (range>0)
			{
			bitDecoded++;			
			range=range>>1;
			}
		
		//Calculate the octet needed
		TInt surplusBitCount=bitDecoded%8;
		TInt octetCount=(surplusBitCount)? bitDecoded/8+1 : bitDecoded/8;
							
		if (aStartBit+bitDecoded>aBuf.Length()*8)
			{
			User::Leave(KErrUnderflow);	
			}
		
		//Allocate memory
		HBufC8* destBuf=HBufC8::NewLC(octetCount);
		TPtr8 des=destBuf->Des();
		des.SetLength(octetCount);
		des.FillZ();
		
		//Copy the integer value from right most.
		TInt destBitNum=(surplusBitCount)? surplusBitCount : 8;
		TInt destOctetNum=0;		

		//Copy ...		
		CAsn1PerUtil::CopyDataTrunkL(aBuf, currentBitPos, bitDecoded, des, destOctetNum, destBitNum);


		aValue=CAsn1PerUtil::CovertStringToUintL(des, octetCount);
		aValue+=aMinValue;
		
		//Update the decoded bits length and start position
		currentBitPos+=bitDecoded;
		aBitsDecoded+=bitDecoded;
		CleanupStack::PopAndDestroy(destBuf);					
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}
	}

void CAsn1PerInt::DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TInt& aValue)
	{
	TInt64 value(0);
	CAsn1PerInt::DecodeL(aBuf, 
						aStartBit,						
						aMinValueIsConstrained,
						aMinValue,
						aMaxValueIsconstrained,
						aMaxValue,
						aIsExtensible,						
						aBitsDecoded,
						aIsExtended,
						value);						
	aValue=value;		
	}

void CAsn1PerInt::DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TUint16& aValue)

	{
	TInt64 value(0);
	CAsn1PerInt::DecodeL(aBuf, 
						aStartBit,						
						aMinValueIsConstrained,
						aMinValue,
						aMaxValueIsconstrained,
						aMaxValue,
						aIsExtensible,						
						aBitsDecoded,
						aIsExtended,
						value);						
	aValue=value;
	}

void CAsn1PerInt::DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TUint32& aValue)

	{
	TInt64 value(0);
	CAsn1PerInt::DecodeL(aBuf, 
						aStartBit,						
						aMinValueIsConstrained,
						aMinValue,
						aMaxValueIsconstrained,
						aMaxValue,
						aIsExtensible,						
						aBitsDecoded,
						aIsExtended,
						value);						
	aValue=value;
	}

void CAsn1PerInt::DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TUint8& aValue)

	{
	TInt64 value(0);
	CAsn1PerInt::DecodeL(aBuf, 
						aStartBit,						
						aMinValueIsConstrained,
						aMinValue,
						aMaxValueIsconstrained,
						aMaxValue,
						aIsExtensible,						
						aBitsDecoded,
						aIsExtended,
						value);						
	aValue=value;
	}

void CAsn1PerInt::DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TInt8& aValue)

	{
	TInt64 value(0);
	CAsn1PerInt::DecodeL(aBuf, 
						aStartBit,						
						aMinValueIsConstrained,
						aMinValue,
						aMaxValueIsconstrained,
						aMaxValue,
						aIsExtensible,						
						aBitsDecoded,
						aIsExtended,
						value);						
	aValue=value;
	}



