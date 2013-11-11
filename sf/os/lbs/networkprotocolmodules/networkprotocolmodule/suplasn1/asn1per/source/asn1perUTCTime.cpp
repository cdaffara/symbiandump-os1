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
// ASN.1 packed encoding rules -  UTC Time
// 
//

#include "asn1per.h"

CAsn1PerEncodingFragment* CAsn1PerUTCTime::EncodeLC(TUint aExistingBitLength,
													const TDesC8& aBuf,
													const TBool aIsPaddingNeeded
													)

	{
	//Get the UTCTime Buffer length
	TUint stringLen=aBuf.Length();

	//Instanciate the Encoding buffer for this string
	CAsn1PerEncodingFragment* encodeFragment=CAsn1PerEncodingFragment::NewLC(aExistingBitLength, aIsPaddingNeeded);

	//Calculate the length of the encoded UTC Time
	TUint8 bitsRequired=8+stringLen*7;
	TUint8 surplusBits=bitsRequired%KOctetBitsLength;
	TInt octetRequired=(surplusBits)? bitsRequired/8+1 : bitsRequired/8;

	//Need to reallocate memory
	encodeFragment->iContent.ReAllocL(octetRequired);

	//update the buffer length
	encodeFragment->iContent.SetLength(octetRequired);
	
	//Set "0" to the new extended buffer 
	encodeFragment->iContent.MidTPtr(0).FillZ();


	//update the fragment record
	TFragmentRecord record;
	record.iBitLength=bitsRequired;
	record.iStartBit=0;
	encodeFragment->iFragmentRecord.AppendL(record);
	
	//update the bit length
	encodeFragment->iBitLength = bitsRequired;

	
	//Set the Length Determinator
	TInt destineOctetPos=0; 
	encodeFragment->iContent[destineOctetPos]=stringLen;
	destineOctetPos+=1;
	
	TInt destineBitPos=8;

	//Copy the UTCTime (VisibleString)
	for (TInt i=0; i<stringLen; i++)
		{
		CAsn1PerUtil::CopyDataTrunkL(aBuf, i*8+1 , 7, encodeFragment->iContent, destineOctetPos, destineBitPos);
		}
	

	return encodeFragment;
	}




//CAsn1PerEncodingFragment* CAsn1PerUTCTime::EncodeL(TUint aExistingBitLength,
//													const TDesC8& aBuf,
//													const TBool aIsPaddingNeeded
//													)
//
//	{
//	
//	
//	CAsn1PerEncodingFragment* enc=CAsn1PerUTCTime::EncodeL(aExistingBitLength,
//															aBuf,
//															EAsn1PerOctetString
//															);
//	CleanupStack::PushL(enc);
//	return enc;														
//	}



	
