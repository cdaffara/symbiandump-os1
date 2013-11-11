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
Implementation of CAsn1PerEncoding
*/

CAsn1PerEncoding::CAsn1PerEncoding(TUint aParentLength, TBool aNeedPadding)
:iParentBitLength(aParentLength), iNeedPadding(aNeedPadding)
	{		
	}

CAsn1PerEncoding::~CAsn1PerEncoding()
	{
	iContent.Close();	
	}

/**
Implementation of CAsn1PerEncodingFragment
*/
CAsn1PerEncodingFragment* CAsn1PerEncodingFragment::NewL(TUint aParentLength, TBool aNeedPadding)
	{
	CAsn1PerEncodingFragment* self = NewLC(aParentLength, aNeedPadding);
	CleanupStack::Pop(self);
	return self;
	}

CAsn1PerEncodingFragment* CAsn1PerEncodingFragment::NewLC(TUint aParentLength, TBool aNeedPadding)
	{
	CAsn1PerEncodingFragment* self = new (ELeave) CAsn1PerEncodingFragment(aParentLength, aNeedPadding);
	CleanupStack::PushL(self);
	return self;		
	}

	
void CAsn1PerEncodingFragment::AddElementL(CAsn1PerEncoding* aEncoding)
	{
	if (aEncoding->IsFragment())
		{
		CAsn1PerEncodingFragment* sourceEncoding=(CAsn1PerEncodingFragment*)aEncoding;
		
		TUint destOctetCount=iContent.Length();
		TUint destBitCount=destOctetCount*8;
		TUint sourceOctetcount=sourceEncoding->iContent.Length();	
		TUint newOctetCount=destOctetCount+sourceOctetcount;
				
		iContent.ReAllocL(newOctetCount);
		//iContent.SetLength(newOctetCount);
		TInt temp1=iContent.Length();
		iContent.Append(sourceEncoding->iContent);
		TInt temp2=iContent.Length();
		iBitLength+=sourceEncoding->iBitLength;

		TUint sourceFragmentCount=sourceEncoding->iFragmentRecord.Count();
		for (TInt i=0;i<sourceFragmentCount;i++)
			{
			sourceEncoding->iFragmentRecord[i].iStartBit+=destBitCount;
			iFragmentRecord.AppendL(sourceEncoding->iFragmentRecord[i]);
			}
		}
	else
		{
		User::Leave(KErrArgument);	
		}			
	}


CAsn1PerEncodingFragment::CAsn1PerEncodingFragment(TUint aParentBitLength, TBool aNeedPadding)
: CAsn1PerEncoding(aParentBitLength, aNeedPadding)
	{
		
	}

CAsn1PerEncodingFragment::~CAsn1PerEncodingFragment()
	{
	iFragmentRecord.Close();
	}


/**
Implementation of CAsn1PerEncodingBuffer
*/
CAsn1PerEncodingBuffer* CAsn1PerEncodingBuffer::NewL(TUint aParentLength, TBool aNeedPadding)
	{
	CAsn1PerEncodingBuffer* self = NewLC(aParentLength, aNeedPadding);
	CleanupStack::Pop(self);
	return self;
	}

CAsn1PerEncodingBuffer* CAsn1PerEncodingBuffer::NewLC(TUint aParentLength, TBool aNeedPadding)
	{
	CAsn1PerEncodingBuffer* self = new (ELeave) CAsn1PerEncodingBuffer(aParentLength, aNeedPadding);
	CleanupStack::PushL(self);
	return self;		
	}

void CAsn1PerEncodingBuffer::AddElementL(CAsn1PerEncoding* aEncoding)
	{
	if (aEncoding->IsFragment())
		{
		
		CAsn1PerEncodingFragment* sourceEncoding=(CAsn1PerEncodingFragment*)aEncoding;
		
		//Calculate the octet required
		TUint8 surplusBits=sourceEncoding->iBitLength%KOctetBitsLength;	
		TInt octetRequired=(surplusBits)? sourceEncoding->iBitLength/8+1 : sourceEncoding->iBitLength/8;			

		//Calulate the new length and allocate them
		TUint destOctetCount=iContent.Length();
		TUint newOctetCount=destOctetCount+octetRequired;
		iContent.ReAllocL(newOctetCount);
		iContent.SetLength(newOctetCount);

		//Set "0" to the new extended buffer 
		iContent.MidTPtr(destOctetCount).FillZ();

		TUint currentBitLength=iBitLength;
		TInt destOctetNum=currentBitLength/8;
		TInt destBitNum=8-(currentBitLength%8);
				
		//go through all the Fragments
		TUint sourceFragmentCount=sourceEncoding->iFragmentRecord.Count();
		for (TInt i=0;i<sourceFragmentCount;i++)
			{

			CAsn1PerUtil::CopyDataTrunkL(sourceEncoding->iContent, 
										sourceEncoding->iFragmentRecord[i].iStartBit, 
										sourceEncoding->iFragmentRecord[i].iBitLength, 
										iContent,
										destOctetNum,
										destBitNum);
			
			}
			
		//Update the bit length
		iBitLength+=sourceEncoding->iBitLength;		
		}
	else
		{
		User::Leave(KErrArgument);	
		}	
		
	}

CAsn1PerEncodingBuffer::CAsn1PerEncodingBuffer(TUint aParentBitLength, TBool aNeedPadding)
: CAsn1PerEncoding(aParentBitLength, aNeedPadding)
	{
		
	}
	
CAsn1PerEncodingBuffer::~CAsn1PerEncodingBuffer()
	{
		
	}


CAsn1PerEncodingFragment* CAsn1PerSequence::EncodeL(TInt aExistingBitLength, const TBool aIsExtensible, const TBool aIsExtended)
	{
	CAsn1PerEncodingFragment* ext= CAsn1PerSequence::EncodeLC(aExistingBitLength, aIsExtensible, aIsExtended);	
	CleanupStack::Pop(ext);
	return ext;
	}


CAsn1PerEncodingFragment* CAsn1PerSequence::EncodeLC(TInt aExistingBitLength, const TBool aIsExtensible, const TBool aIsExtended)
	{
	CAsn1PerEncodingFragment* ext(NULL);
	if (aIsExtensible)
		{
		ext=CAsn1PerBoolean::EncodeL(aExistingBitLength, 
									aIsExtended, 
									EFalse);
		CleanupStack::PushL(ext);			
		}
	else
		{
		ext=CAsn1PerEncodingFragment::NewLC(aExistingBitLength, EFalse);	
		}	
	
	return ext;	
	}




void CAsn1PerSequence::DecodeL(const TDesC8& aBuf,
							const TInt aStartBit, 
							const TBool aIsExtensible,
							TInt& aBitDecoded,
							TBool& aIsExtended)
	{
	aBitDecoded=0;
	aIsExtended=EFalse;
	
	//We only get one "E" bit if it is extensible
	if (aIsExtensible)
		{
		CAsn1PerBoolean::DecodeL(aBuf, 
								EFalse, 
								aStartBit, 
								aBitDecoded, 
								aIsExtended);	
		}					
	}






