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
// ASN.1 packed encoding rules - Octet strings
// 
//

#include "asn1per.h"

CAsn1PerEncodingFragment* CAsn1PerOctetString::EncodeL(TUint aExistingBitLength,
														const TDesC8& aBuf,
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
										aBuf.Length()*8,
										EAsn1PerOctetString
										);
	}




CAsn1PerEncodingFragment* CAsn1PerOctetString::EncodeLC(TUint aExistingBitLength,
														const TDesC8& aBuf,
														const TBool aLengthIsVariable,
														const TBool aLengthIsConstrained,
														const TInt aMinLength,
														const TInt aMaxLength,
														const TBool aIsExtensible,
														const TBool aIsExtended,
														const TBool aIsPaddingNeeded
														)

	{
	
	
	CAsn1PerEncodingFragment* enc=CAsn1PerUtil::EncodeStringL(aExistingBitLength,
															aBuf,
															aLengthIsVariable,
															aLengthIsConstrained,
															aMinLength,
															aMaxLength,
															aIsExtensible,
															aIsExtended,
															aIsPaddingNeeded,
															aBuf.Length()*8,
															EAsn1PerOctetString
															);
	CleanupStack::PushL(enc);
	return enc;														
	}



	
void CAsn1PerOctetString::DecodeL(const TDesC8& aBuf,
								const TInt aStartBit,
								const TBool aLengthIsVariable,
								const TBool aLengthIsConstrained,
								const TInt aMinLength,
								const TInt aMaxLength,	
								const TBool aIsExtensible,
								const TBool /*aIsPadded*/,
								TBool& aIsExtended,
								TInt& aBitsDecoded, 
								RBuf8& aDecodedBuf)
	{
	
	TInt currentBitPos=aStartBit;
	TInt bitDecoded=0;
	aBitsDecoded=0;
	aIsExtended=EFalse;
	TInt stringLen=0;
	
	CAsn1PerUtil::DecodeStringEBitAndLengthL(aBuf, 
											currentBitPos, 
											aLengthIsVariable, 
											aLengthIsConstrained,
											aMinLength,
											aMaxLength,
											aIsExtensible,
											aIsExtended,
											bitDecoded,
											stringLen);
	
	currentBitPos+=bitDecoded;
	aBitsDecoded+=bitDecoded;
	bitDecoded=0;					
	
	//check if the input buffer is long enough	
	if (currentBitPos+stringLen*8>aBuf.Length()*8)
		{
		User::Leave(KErrUnderflow);	
		}
			
	//Finally, decode the string content for cases
	aDecodedBuf.ReAllocL(stringLen);
	aDecodedBuf.SetLength(stringLen);
	aDecodedBuf.FillZ();
	
	TInt stringBitLength=stringLen*8;
	
	//Copy the String to the Dest Buffer .
	TInt destBitNum=8;
	TInt destOctetNum=0;		
		
	//Copy ...		
	CAsn1PerUtil::CopyDataTrunkL(aBuf, currentBitPos, stringBitLength, aDecodedBuf, destOctetNum, destBitNum);
	aBitsDecoded+=stringBitLength;
	}

