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
// Asn.1 Packed Encoding Rules - Encode & Decode classes
// 
//

/**
 @file
 @internalComponent 
*/


#ifndef __ASN1PER_H__
#define __ASN1PER_H__

#include <e32base.h>


const TUint8 KBit8Set=0x80;
const TUint8 KEncodedBoolLength=1;
const TUint8 KOctetBitsLength=8;
const TUint KConstraintLengthLimit=64*1024;

const TBool KAsn1PerExtensible = ETrue;
const TBool KAsn1PerNonExtensible = EFalse;
const TBool KAsn1PerExtended = ETrue;
const TBool KAsn1PerNonExtended = EFalse;
const TBool KAsn1PerMaxConstrained = ETrue;
const TBool KAsn1PerMaxNotConstrained = EFalse;
const TBool KAsn1PerMinConstrained = ETrue;
const TBool KAsn1PerMinNotConstrained = EFalse;
const TBool KAsn1PerPadding = ETrue;
const TBool KAsn1PerNoPadding = EFalse;
const TBool KAsn1LengthVariable = ETrue;
const TBool KAsn1LengthNonVariable = EFalse;



enum TAsn1PerStringType
	{
	EAsn1PerOctetString,
	EAsn1PerBitString			
	};


/**
Class to hold the encoding buffer reference
*/

class TFragmentRecord
	{
public:
	TUint iStartBit;
	TUint iBitLength;		
	};

class CAsn1PerEncoding : public CBase
	{
public:
	virtual ~CAsn1PerEncoding();
	virtual void AddElementL(CAsn1PerEncoding* aEncoding)=0;
	virtual TBool IsFragment()=0;
	
protected:
	CAsn1PerEncoding(TUint aParentBitLength, TBool aNeedPadding);
	
public:	
	RBuf8 iContent;
	TUint iBitLength;
	TUint iParentBitLength;
	TBool iNeedPadding;
	};
	
class CAsn1PerEncodingFragment : public CAsn1PerEncoding
	{
public:	
	static CAsn1PerEncodingFragment* NewL(TUint aParentBitLength, TBool aNeedPadding);
	static CAsn1PerEncodingFragment* NewLC(TUint aParentBitLength, TBool aNeedPadding);
	virtual void AddElementL(CAsn1PerEncoding* aEncoding);
	virtual TBool IsFragment(){return ETrue;};
	~CAsn1PerEncodingFragment();
	
private:
	CAsn1PerEncodingFragment(TUint aParentBitLength, TBool aNeedPadding);		
	
public:
	RArray<TFragmentRecord> iFragmentRecord;		
	};
	
class CAsn1PerEncodingBuffer : public CAsn1PerEncoding
	{
public:	
	static CAsn1PerEncodingBuffer* NewL(TUint aParentBitLength, TBool aNeedPadding);
	static CAsn1PerEncodingBuffer* NewLC(TUint aParentBitLength, TBool aNeedPadding);
	virtual void AddElementL(CAsn1PerEncoding* aEncoding);
	virtual TBool IsFragment(){return EFalse;};
	~CAsn1PerEncodingBuffer();
private:
	CAsn1PerEncodingBuffer(TUint aParentBitLength, TBool aNeedPadding);			
	};
		
class CAsn1PerUtil
	{
public:	
	static void CopyDataTrunkL(const TDesC8& asourceBuf, 
								const TInt aStartBit, 
								const TInt aBitCount, 
								TDes8& aDestBuf,
								TInt& aDestOctetNum,
								TInt& aDestBitNum);

	static void DecodeStringEBitAndLengthL(const TDesC8& aBuf,
											const TInt aStartBit,
											const TBool aLengthIsVariable,
											const TBool aLengthIsConstrained,
											const TInt aMinLength,
											const TInt aMaxLength,	
											const TBool aIsExtensible,
											TBool& aIsExtended,
											TInt& aBitsDecoded, 
											TInt& aStringLength);
											
	static void UnAlignEncodeIntegerL(const TInt64 aMaxValue, 
									const TInt64 aMinValue, 
									const TInt64 aValue,
									const TBool aAllocMore,
									CAsn1PerEncodingFragment* aEncoding);
												
												
	static CAsn1PerEncodingFragment* EncodeStringL(TUint aExistingBitLength,
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
												);
	static TUint CovertStringToUintL(const TDesC8& aString, TUint8 aNum);
											
	}; 

/** 
Class for encoding and decoding of SEQUENCE OF type.
*/

class CAsn1PerSequenceof
	{
public:
	static CAsn1PerEncodingFragment* EncodeL(const TInt aExistingLength,
											const TInt aSize,
											const TBool aIsMinSizeConstrained,
											const TInt aMinSize,
											const TBool aIsMaxSizeConstrained,
											const TInt aMaxSize,
											const TBool aIsExtensible,
											const TBool aIsExtended);
											
											
	static CAsn1PerEncodingFragment* EncodeLC(const TInt aExistingLength,
											const TInt aSize,
											const TBool aIsMinSizeConstrained,
											const TInt aMinSize,
											const TBool aIsMaxSizeConstrained,
											const TInt aMaxSize,
											const TBool aIsExtensible,
											const TBool aIsExtended);
											

	static void DecodeL(const TDesC8& aBuf,
						const TInt aStartBit,
						const TBool aIsMinSizeConstrained,
						const TInt aMinSize,
						const TBool aIsMaxSizeConstrained,
						const TInt aMaxSize,
						const TBool aIsExtensible,
						TInt& aBitDecoded,
						TInt& aSize,
						TBool& aIsExtended);
	};



/** 
Class for encoding and decoding of SEQUENCE type.
*/

class CAsn1PerSequence
	{
public:
	static CAsn1PerEncodingFragment* EncodeL(const TInt aExistingLength,
											const TBool aIsExtensible,
											const TBool aIsExtended=EFalse);
											
											
	static CAsn1PerEncodingFragment* EncodeLC(const TInt aExistingLength,
											const TBool aIsExtensible,
											const TBool aIsExtended=EFalse);
											

	static void DecodeL(const TDesC8& aBuf,
						const TInt aStartBit,
						const TBool aIsExtensible,
						TInt& aBitDecoded,
						TBool& aIsExtended);
	};



class CAsn1PerComplexType : public CBase
{
public:
	virtual void Padding();
	
protected:
	CAsn1PerComplexType(CAsn1PerEncoding* aEncoding, const TBool aIsPaddingNeeded);
	CAsn1PerEncoding* iEncoding;
	TBool iIsPaddingNeeded;
	
};


/** 
Class for encoding & decoding Choice values.
which just contains static funtions.
*/

class CAsn1PerChoice
	{
public:

	static CAsn1PerEncodingFragment* EncodeL(TUint aExistingBitLength,
											const TInt aChoice,
											const TInt aMaxChoice,
											const TBool aIsExtensible,
											const TBool aIsExtended);
											
											
	static CAsn1PerEncodingFragment* EncodeLC(TUint aExistingBitLength,
											const TInt aChoice,
											const TInt aMaxChoice,
											const TBool aIsExtensible,
											const TBool aIsExtended);
											
	
	static void DecodeL(const TDesC8& aBuf,
						const TInt aStartBit,
						const TInt aMaxChoice,
						const TBool aIsExtensible,
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TInt& aChoiceValue
						);
	};




class CAsn1PerEnum
{
public:
	static CAsn1PerEncodingFragment* EncodeL(TUint aExistingBitLength,
									const TInt aChoice,
									const TInt aMaxChoice,
									const TBool aIsExtensible,
									const TBool aIsExtended,
									const TBool aIsPaddingNeeded
									);

	static CAsn1PerEncodingFragment* EncodeLC(TUint aExistingBitLength,
									const TInt aChoice,
									const TInt aMaxChoice,
									const TBool aIsExtensible,
									const TBool aIsExtended,
									const TBool aIsPaddingNeeded
									);


	static void DecodeL(const TDesC8& aBuf,
									const TInt aStartBit,
									const TInt aMaxChoice,
									const TBool aIsExtensible,
									const TBool aIsPadded,
									TInt& aBitsDecoded,
									TBool& aIsExtended,
									TInt& aChoiceValue
									);	
};









/** 
Class for encoding & decoding Boolean values,
which just contains static funtions.
*/
class CAsn1PerBoolean
	{
public:
	static CAsn1PerEncodingFragment* EncodeL(TUint aExistingBitLength, 
											const TBool aValue, 
											const TBool aIsPaddingNeeded);
											
	static CAsn1PerEncodingFragment* EncodeLC(TUint aExistingBitLength, 
											const TBool aValue, 
											const TBool aIsPaddingNeeded);
											
	static void DecodeL(const TDesC8& aBuf, 
						const TBool aIsPadded,
						const TInt aStartBit, 
						TInt& aBitDecoded, 
						TBool& aValue);
	};


/** 
Class for encoding & decoding Boolean values.
which just contains static funtions.
*/
class CAsn1PerInt
	{
public:

	static CAsn1PerEncodingFragment* EncodeL(TUint aExistingBitLength,
											const TInt64 aValue,
											const TBool aMinValueIsConstrained,
											const TInt64 aMinValue,
											const TBool aMaxValueIsconstrained,
											const TInt64 aMaxValue,
											const TBool aIsExtensible,
											const TBool aIsExtended,
											const TBool aIsPaddingNeeded);



	static CAsn1PerEncodingFragment* EncodeLC(TUint aExistingBitLength,
											const TInt64 aValue,
											const TBool aMinValueIsConstrained,
											const TInt64 aMinValue,
											const TBool aMaxValueIsconstrained,
											const TInt64 aMaxValue,
											const TBool aIsExtensible,
											const TBool aIsExtended,
											const TBool aIsPaddingNeeded);


	/**
	@param 
	@return
	*/
	static void DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TInt64& aValue);
	
	static void DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TInt& aValue);

	static void DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TInt8& aValue);
						

	static void DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TUint32& aValue);				

						
	static void DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TUint16& aValue);

	static void DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit,
						const TBool aMinValueIsConstrained,
						const TInt64 aMinValue,
						const TBool aMaxValueIsconstrained,
						const TInt64 aMaxValue,
						const TBool aIsExtensible,						
						TInt& aBitsDecoded,
						TBool& aIsExtended,
						TUint8& aValue);
						
	};



/** 
Class for encoding & decoding Octet String values.
which just contains static funtions.
*/
class CAsn1PerOctetString
	{
public:
	/**
	@param 
	@return
	*/
	static CAsn1PerEncodingFragment* EncodeL(TUint aExistingBitLength,
											const TDesC8& aBuf,
											const TBool aLengthIsVariable,
											const TBool aLengthIsConstrained,
											const TInt aMinLength,
											const TInt aMaxLength,
											const TBool aIsExtensible,
											const TBool aIsExtended,
											const TBool aIsPaddingNeeded
											);									
	/**
	@param 
	@return
	*/
	static CAsn1PerEncodingFragment* EncodeLC(TUint aExistingBitLength,
											const TDesC8& aBuf,
											const TBool aLengthIsVariable,
											const TBool aLengthIsConstrained,
											const TInt aMinLength,
											const TInt aMaxLength,
											const TBool aIsExtensible,
											const TBool aIsExtended,
											const TBool aIsPaddingNeeded
											);
									
	/**
	@param 
	@return
	*/
	static void DecodeL(const TDesC8& aBuf,
						const TInt aStartBit,
						const TBool aLengthIsVariable,
						const TBool aLengthIsConstrained,
						const TInt aMinLength,
						const TInt aMaxLength,	
						const TBool aIsExtensible,
						const TBool aIsPadded,
						TBool& aIsExtended,
						TInt& aBitsDecoded, 
						RBuf8& aDecodedBuf);						
	};

/** 
Class for encoding & decoding Bit String values.
which just contains static funtions.
*/
class CAsn1PerBitString
	{
public:
	static CAsn1PerEncodingFragment* EncodeL(TUint aExistingBitLength,
											const TDesC8& aBuf,
											const TInt aBitLength,
											const TBool aLengthIsVariable,
											const TBool aLengthIsConstrained,
											const TInt aMinLength,
											const TInt aMaxLength,
											const TBool aIsExtensible,
											const TBool aIsExtended,
											const TBool aIsPaddingNeeded
											);
									
	static void DecodeL(const TDesC8& aBuf,
						const TInt aStartBit,
						const TBool aLengthIsVariable,
						const TBool aLengthIsConstrained,
						const TInt aMinLength,
						const TInt aMaxLength,	
						const TBool aIsExtensible,
						TBool& aIsExtended,
						TInt& aBitsDecoded, 
						RBuf8& aDecodedBuf,
						TInt& aStringBitLength);
	};

/** 
Class for encoding & decoding Option values.
which just contains static funtions.
*/
class CAsn1PerOption
	{
public:
	static CAsn1PerEncodingFragment* EncodeL(TUint aExistingBitLength, const TBool aOption);
	static CAsn1PerEncodingFragment* EncodeLC(TUint aExistingBitLength, const TBool aOption);

	static void DecodeL(const TDesC8& aBuf, 
						const TInt aStartBit, 
						TInt& aBitsDecoded,
						TBool& aOption);
	};

/** 
Class for encoding UTCTime.
which just contains static funtions.
*/
class CAsn1PerUTCTime
{
public:
	static 	CAsn1PerEncodingFragment* EncodeLC(TUint aExistingBitLength,
											  const TDesC8& aBuf,
											  const TBool aIsPaddingNeeded
											  );
};

/** 
Class for encoding & decoding Enumurate values.
which just contains static funtions.
TODO: we may not need this, as it is the same as choice
*/
/*
class CAsn1PerEnum
	{
public:
	static CAsn1PerEncoding* EncodeL(const TBool aBool);
	static TDecodeError* DecodeL(const TDesC8&, const TInt&, TInt&, TBool&);
	};
*/
#endif // __ASN1PER_H__
