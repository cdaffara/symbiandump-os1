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
// ASN.1 packed encoding rules utility functions
// 
//

#include "asn1per.h"


TUint CAsn1PerUtil::CovertStringToUintL(const TDesC8& aString, TUint8 aNum)
	{
	TInt retValue(0);
	const TUint8* tmpConstPtr=aString.Ptr();
	TUint8* tmpPtr=const_cast<TUint8*>(tmpConstPtr);

	switch (aNum)	
		{
	case 1:
		retValue=tmpPtr[0];
		break;
	case 2:
		retValue=(tmpPtr[0]<<8)+tmpPtr[1];
		break;
	case 3:
		retValue=(tmpPtr[0]<<16)+(tmpPtr[1]<<8)+tmpPtr[2];
		break;
	case 4:
		retValue=(tmpPtr[0]<<24)+(tmpPtr[1]<<16)+(tmpPtr[2]<<8)+tmpPtr[3];
		break;
	default:
		User::Leave(KErrArgument);
		}
	return retValue;
	}

	
void CAsn1PerUtil::DecodeStringEBitAndLengthL(const TDesC8& aBuf,
											const TInt aStartBit,
											const TBool aLengthIsVariable,
											const TBool aLengthIsConstrained,
											const TInt aMinLength,
											const TInt aMaxLength,	
											const TBool aIsExtensible,
											TBool& aIsExtended,
											TInt& aBitsDecoded, 
											TInt& aStringLength)
	{

	if(aStartBit>=(aBuf.Length()*KOctetBitsLength))
		{
		User::Leave(KErrUnderflow);	
		}

	TInt currentBitPos=aStartBit;
	TInt bitDecoded=0;
	aBitsDecoded=0;
	aIsExtended=EFalse;

	//Try to get the length of the string.
	if (aLengthIsVariable)
		{
		// Length Variable, so decode the length determinant		
		TInt range(0);
		if (aLengthIsConstrained)
			{
			range=aMaxLength-aMinLength;
			if (range<0)
				{
				User::Leave(KErrArgument);	
				}
			}
			
		if (aIsExtensible)
			{
			User::Leave(KErrNotSupported);
			CAsn1PerBoolean::DecodeL(aBuf, EFalse, currentBitPos, bitDecoded, aIsExtended);
			currentBitPos+=bitDecoded;
			aBitsDecoded+=bitDecoded;
			bitDecoded=0;					
			}
			
		if (!aLengthIsConstrained || range+1>KConstraintLengthLimit || (aIsExtensible && aIsExtended))
			{
			//Decode a general length determinant
			//aStringLength=				
			}
		else
			{
			TBool lengthExt(EFalse);
			CAsn1PerInt::DecodeL(aBuf, 
								currentBitPos,
								ETrue,
								aMinLength,
								ETrue,
								aMaxLength,
								EFalse,
								bitDecoded,
								lengthExt,
								aStringLength);
													
			currentBitPos+=bitDecoded;
			aBitsDecoded+=bitDecoded;
			bitDecoded=0;
			}			
		}
	else
		{
		//Fix length string, no length determinant.
		if (aMinLength!=aMaxLength)
			{
			User::Leave(KErrArgument);	
			}
		aStringLength=aMinLength;	
		}
	}


void CAsn1PerUtil::CopyDataTrunkL(const TDesC8& asourceBuf, 
								const TInt aStartBit, 
								const TInt aBitCount, 
								TDes8& aDestBuf,
								TInt& aDestOctetNum,
								TInt& aDestBitNum)
	{
	
	
	
	//Calculate the destination buffer length	
	TInt desBufferAbsoluteLength=aDestBuf.Length();
	TInt desBufferLength=desBufferAbsoluteLength-aDestOctetNum;
	TInt desBufferBitsLength=(desBufferLength-1)*8+aDestBitNum;

	//Check the destination buffer is big enough.
	if (desBufferBitsLength<aBitCount)
		{
		User::Leave(KErrArgument);	
		}
		
	//Calculate the source buffer start and end point	
	TInt sourceStartOctetNum=aStartBit/8;
	TInt sourceStartBitNum=8-aStartBit%8;	
	TInt sourceEndOctetNum=(aStartBit+aBitCount-1)/8;
	TInt sourceEndBitNum=8-(aStartBit+aBitCount-1)%8;
 	
 	
 	
	while (sourceStartOctetNum<=sourceEndOctetNum && aDestOctetNum < desBufferAbsoluteLength)
		{
		// Retrieve source data bits - making sure leftmost bits are cleared
		TUint8 sourceBits = asourceBuf[sourceStartOctetNum];
		sourceBits = sourceBits << (8 - sourceStartBitNum);
		sourceBits = sourceBits >> (8 - sourceStartBitNum);
						
		if (sourceStartOctetNum<sourceEndOctetNum)
			{
			if (sourceStartBitNum>aDestBitNum)
				{
				// Shift source content to the right to obtain bits that will fit
				sourceBits = sourceBits >> (sourceStartBitNum - aDestBitNum);
				
				// Add source bits into destination byte 
				aDestBuf[aDestOctetNum] |= sourceBits;
				
				// Adjust bit and byte numbers for next time around
				++aDestOctetNum;
				sourceStartBitNum -= aDestBitNum;
				aDestBitNum = 8;	
				}
			else
				{						
				// Shift source content to the left to fit at correct position
				sourceBits = sourceBits << (aDestBitNum - sourceStartBitNum);
				
				// Add source bits into destination byte 
				aDestBuf[aDestOctetNum] |= sourceBits;
				
				// Adjust bit and byte numbers for next time around
				aDestBitNum -= sourceStartBitNum;
				
				//Not sure the if clause will happen
				if (0 == aDestBitNum)
					{
					++aDestOctetNum;
					aDestBitNum = 8;
					}
				sourceStartBitNum = 8;
				++sourceStartOctetNum;
				}
			}
		else
			{
			//Last octet
			//Clear the rightmost bits
			sourceBits = sourceBits >> (sourceEndBitNum-1);
			sourceBits = sourceBits << (sourceEndBitNum-1);
			
			if (sourceStartBitNum-sourceEndBitNum+1>aDestBitNum)
				{
				sourceBits = sourceBits >> (sourceStartBitNum-aDestBitNum);	
				//sourceBits = sourceBits >> (8-aDestBitNum);
				//sourceBits = sourceBits << (8-aDestBitNum);
				
				aDestBuf[aDestOctetNum] |= sourceBits;
				
				++aDestOctetNum;
				sourceStartBitNum -= aDestBitNum;
				aDestBitNum = 8;
				}
			else
				{
				//sourceBits = sourceBits << (8-sourceStartBitNum);
				//sourceBits = sourceBits >> (8-sourceStartBitNum);
				if (sourceStartBitNum>aDestBitNum)
					{
					sourceBits=sourceBits>>(sourceStartBitNum-aDestBitNum);
					}
				else
					{
					sourceBits=sourceBits<<(aDestBitNum-sourceStartBitNum);
					}	
								
				aDestBuf[aDestOctetNum] |= sourceBits;
				aDestBitNum -= (sourceStartBitNum-sourceEndBitNum+1);
				
				if (0 == aDestBitNum)
					{
					++aDestOctetNum;
					aDestBitNum = 8;
					}
					
				//All done, to exit the loop	
				++sourceStartOctetNum;
				}											
			}								
		}		
	}


void CAsn1PerUtil::UnAlignEncodeIntegerL(const TInt64 aMaxValue, 
										const TInt64 aMinValue, 
										const TInt64 aValue,
										const TBool aAllocMore, 
										CAsn1PerEncodingFragment* aEncoding)
	{
	//Calculate the bits needed.
	TUint range=aMaxValue-aMinValue;
	TUint8 bitsRequired=0;		
	while (range>0)
		{
		bitsRequired++;			
		range=range>>1;
		}	
	
	//Calculate the Octet needed to be allocated
	TUint8 surplusBits=bitsRequired%KOctetBitsLength;
	TInt octetRequired=(surplusBits)? bitsRequired/8+1 : bitsRequired/8;

	//Calculate the Current Octet count and the new Octet count
	TUint currentOctetCount=aEncoding->iContent.Length();
	TUint newOctetLength=currentOctetCount+octetRequired;
	
	//Need to reallcate memory
	if (newOctetLength > aEncoding->iContent.MaxLength())
		{
		if (aAllocMore)
			{
			aEncoding->iContent.ReAllocL(newOctetLength+aValue);				
			}
		else
			{
			aEncoding->iContent.ReAllocL(newOctetLength);				
			}
		}
		
	//update the buffer length
	aEncoding->iContent.SetLength(newOctetLength);
	
	//Set "0" to the new extended buffer 
	aEncoding->iContent.MidTPtr(currentOctetCount).FillZ();
	
	//update the fragment record
	TFragmentRecord record;
	record.iBitLength=bitsRequired;
	record.iStartBit=8*currentOctetCount+(8-surplusBits)%8;
	aEncoding->iFragmentRecord.AppendL(record);
	
	//update the bit length
	aEncoding->iBitLength+=bitsRequired;
			
	// Calculate value offset.
	TInt valueOffset=aValue-aMinValue;
	TUint copiedNewOctetLength=newOctetLength;
	
	// Set value Offset into descriptor 
	while ((valueOffset > 0) && (octetRequired > 0))
		{
		TInt8 octetValue = (TInt8)(valueOffset & 0xFF);
		aEncoding->iContent[copiedNewOctetLength - 1] = octetValue;
		valueOffset = valueOffset >> 8;
		--octetRequired;
		--copiedNewOctetLength;
		}		
	}
		
CAsn1PerEncodingFragment* CAsn1PerUtil::EncodeStringL(TUint aExistingBitLength,
														const TDesC8& aBuf,
														const TBool aLengthIsVariable,
														const TBool aLengthIsConstrained,
														const TInt aMinLength,
														const TInt aMaxLength,
														const TBool aIsExtensible,
														const TBool aIsExtended,
														const TBool aIsPaddingNeeded,
														const TInt aStringBitLength,
														const TInt aStringType
														)


	{
	TUint stringLen=aBuf.Length();
	
	//Instanciate the Encoding buffer for this string
	CAsn1PerEncodingFragment* encodeFragment=CAsn1PerEncodingFragment::NewLC(aExistingBitLength, aIsPaddingNeeded);
		
	//Handle variable length octet string first
	if (aLengthIsVariable)
		{
		TInt range(0);
		if (aLengthIsConstrained)
			{
			// Calculate the allowed string range.
			TInt range=aMaxLength-aMinLength;
				
			if (aStringType==EAsn1PerOctetString)
				{
				if (range<=0 || stringLen<aMinLength || stringLen>aMaxLength)
					{
					User::Leave(KErrArgument);
					}					
				}
			else
				{
				if (range<=0 || aStringBitLength<aMinLength || aStringBitLength>aMaxLength)
					{
					User::Leave(KErrArgument);
					}					
					
				}	
				
			}
			
		if (aIsExtensible)
			{
			User::Leave(KErrNotSupported);
			
			//encode "E" bit.					
			CAsn1PerEncodingFragment* ebit=CAsn1PerBoolean::EncodeL(aExistingBitLength, 
																	aIsExtended, 
																	EFalse);
			CleanupStack::PushL(ebit);														
			encodeFragment->AddElementL(ebit);
			CleanupStack::PopAndDestroy(ebit);									
			}
		
		if (range+1>KConstraintLengthLimit || !aLengthIsConstrained || (aIsExtensible && aIsExtended))
			{
			//TODO: need to encode a general length determinant
			}
		else
			{
			//Encode the length determinant	
			if (aStringType==EAsn1PerOctetString)
				{
				CAsn1PerUtil::UnAlignEncodeIntegerL(aMaxLength, aMinLength, stringLen, ETrue, encodeFragment);					
				}
			else
				{
				CAsn1PerUtil::UnAlignEncodeIntegerL(aMaxLength, aMinLength, aStringBitLength, ETrue, encodeFragment);					
				}	
			}					
		}
	
	//Allocate memory if required
	TInt currentOctetCount=encodeFragment->iContent.Length();
	TInt newOctetLength=currentOctetCount+stringLen;
	
	if (newOctetLength > encodeFragment->iContent.MaxLength())
			{
			encodeFragment->iContent.ReAllocL(newOctetLength);	
			}
	
	// update the string cotent record		
	TFragmentRecord record;
	record.iBitLength=aStringBitLength;;
	record.iStartBit=currentOctetCount*KOctetBitsLength;
	encodeFragment->iFragmentRecord.AppendL(record);
	
	// update the bit length in buffer
	encodeFragment->iBitLength+=aStringBitLength;;
	
	//Append the string
	encodeFragment->iContent.Append(aBuf);

	//update the buffer length
	encodeFragment->iContent.SetLength(newOctetLength);
	
	CleanupStack::Pop(encodeFragment);
	return encodeFragment;		
	}




















