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
// ASN.1 packed encoding rules - Bits strings
// 
//

#include "asn1per.h"


CAsn1PerEncodingFragment* CAsn1PerBitString::EncodeL(TUint aExistingBitLength,
													const TDesC8& aBuf,
													const TInt aBitLength,
													const TBool aLengthIsVariable,
													const TBool aLengthIsConstrained,
													const TInt aMinLength,
													const TInt aMaxLength,
													const TBool aIsExtensible,
													const TBool aIsExtended,
													const TBool aIsPaddingNeeded
													)
	{
	
	return CAsn1PerUtil::EncodeStringL(aExistingBitLength,
										aBuf,
										aLengthIsVariable,
										aLengthIsConstrained,
										aMinLength,
										aMaxLength,
										aIsExtensible,
										aIsExtended,
										aIsPaddingNeeded,
										aBitLength,
										EAsn1PerBitString
										);	
	}



	

void CAsn1PerBitString::DecodeL(const TDesC8& aBuf,
								const TInt aStartBit,
								const TBool aLengthIsVariable,
								const TBool aLengthIsConstrained,
								const TInt aMinLength,
								const TInt aMaxLength,	
								const TBool aIsExtensible,
								TBool& aIsExtended,
								TInt& aBitsDecoded, 
								RBuf8& aDecodedBuf,
								TInt& aStringBitLength)
	{
	
	TInt currentBitPos=aStartBit;
	TInt bitDecoded=0;
	aBitsDecoded=0;
	aIsExtended=EFalse;
	aStringBitLength=0;
	
	
	CAsn1PerUtil::DecodeStringEBitAndLengthL(aBuf, 
											currentBitPos, 
											aLengthIsVariable, 
											aLengthIsConstrained,
											aMinLength,
											aMaxLength,
											aIsExtensible,
											aIsExtended,
											bitDecoded,
											aStringBitLength);
	
	currentBitPos+=bitDecoded;
	aBitsDecoded+=bitDecoded;
	bitDecoded=0;					

	//check if the input buffer is long enough	
	if (currentBitPos+aStringBitLength>aBuf.Length()*8)
		{
		User::Leave(KErrUnderflow);	
		}
		
	//Finally, decode the string content for cases

	TInt stringOctetLen=(aStringBitLength%8)? aStringBitLength/8+1 : aStringBitLength/8;	
	
	aDecodedBuf.ReAllocL(stringOctetLen);
	aDecodedBuf.SetLength(stringOctetLen);
	aDecodedBuf.FillZ();
		
	//Copy the String to the Dest Buffer .
	TInt destBitNum = 8 - (8*stringOctetLen - aStringBitLength);
	TInt destOctetNum=0;
	
	//Copy ...		
	CAsn1PerUtil::CopyDataTrunkL(aBuf, currentBitPos, aStringBitLength, aDecodedBuf, destOctetNum, destBitNum);
	aBitsDecoded+=aStringBitLength;
	currentBitPos+=aStringBitLength;
	}


