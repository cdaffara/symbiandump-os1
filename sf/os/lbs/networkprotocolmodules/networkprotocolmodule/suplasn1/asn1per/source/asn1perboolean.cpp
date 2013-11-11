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
// CASN1PERBOOLEAN.CPP
// ASN.1 packed encoding rules - Booleans
// 
//

#include "asn1per.h"


const TUint8 KBitSetArray[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

CAsn1PerEncodingFragment* CAsn1PerBoolean::EncodeL(TUint aExistingBitLength, 
													const TBool aValue, 
													const TBool aIsPaddingNeeded)
	{
	CAsn1PerEncodingFragment* encodeFragment=CAsn1PerEncodingFragment::NewLC(aExistingBitLength, aIsPaddingNeeded);
	
	encodeFragment->iContent.ReAllocL(1);
	encodeFragment->iContent.SetLength(1);
	if (aValue)
		{
		encodeFragment->iContent[0]=KBit8Set;			
		}
	else
		{
		encodeFragment->iContent[0]=0;			
		}	

	//update the fragment record
	TFragmentRecord record;
	record.iBitLength=1;
	record.iStartBit=0;
	encodeFragment->iFragmentRecord.AppendL(record);
	encodeFragment->iBitLength=1;
	
	CleanupStack::Pop(encodeFragment);
	return encodeFragment;
	
	}

CAsn1PerEncodingFragment* CAsn1PerBoolean::EncodeLC(TUint aExistingBitLength, 
													const TBool aValue, 
													const TBool aIsPaddingNeeded)
	{
	CAsn1PerEncodingFragment* encodeFragment=EncodeL(aExistingBitLength, aValue, aIsPaddingNeeded);
	CleanupStack::PushL(encodeFragment);
	return encodeFragment;	
	}

void CAsn1PerBoolean::DecodeL(const TDesC8& aBuf,
							const TBool aIsPadded, 
							const TInt aStartBit, 
							TInt& aBitDecoded, 
							TBool& aValue)
	{
	aBitDecoded=0;
	if(aStartBit>=(aBuf.Length()*KOctetBitsLength))
		{
		User::Leave(KErrUnderflow);	
		}
	
	TInt octetNum=aStartBit/KOctetBitsLength;
	TUint8 bitNum=8-aStartBit%KOctetBitsLength;
	
	
	if (aBuf[octetNum] & KBitSetArray[bitNum-1])
		{
		aValue=ETrue;
		}
	else
		{
		aValue=EFalse;	
		}
	if (aIsPadded)
		{
		User::Leave(KErrNotSupported);
		aBitDecoded=8;	
		}
	else
		{
		aBitDecoded=1;		
		}		
	}









