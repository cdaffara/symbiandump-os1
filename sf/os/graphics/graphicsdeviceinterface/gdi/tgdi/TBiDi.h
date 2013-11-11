// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __TBIDI_H__
#define __TBIDI_H__

#include "TGraphicsHarness.h"


inline TBool IsSupplementary(TUint aChar)
/**
@param aChar The 32-bit code point value of a Unicode character.

@return True, if aChar is supplementary character; false, otherwise.
*/
	{
	return (aChar > 0xFFFF);
	}

inline TBool IsHighSurrogate(TText16 aInt16)
/**
@return True, if aText16 is high surrogate; false, otherwise.
*/
	{
	return (aInt16 & 0xFC00) == 0xD800;
	}

inline TBool IsLowSurrogate(TText16 aInt16)
/**
@return True, if aText16 is low surrogate; false, otherwise.
*/
	{
	return (aInt16 & 0xFC00) == 0xDC00;
	}

inline TUint JoinSurrogate(TText16 aHighSurrogate, TText16 aLowSurrogate)
/**
Combine a high surrogate and a low surrogate into a supplementary character.

@return The 32-bit code point value of the generated Unicode supplementary
        character.
*/
	{
	return ((aHighSurrogate - 0xD7F7) << 10) + aLowSurrogate;
	}

inline TText16 GetHighSurrogate(TUint aChar)
/**
Retrieve the high surrogate of a supplementary character.

@param aChar The 32-bit code point value of a Unicode character.

@return High surrogate of aChar, if aChar is a supplementary character; 
        aChar itself, if aChar is not a supplementary character.
*/
	{
	return STATIC_CAST(TText16, 0xD7C0 + (aChar >> 10));
	}

inline TText16 GetLowSurrogate(TUint aChar)
/**
Retrieve the low surrogate of a supplementary character.

@param aChar The 32-bit code point value of a Unicode character.

@return Low surrogate of aChar, if aChar is a supplementary character; 
        zero, if aChar is not a supplementary character.
*/
	{
	return STATIC_CAST(TText16, 0xDC00 | (aChar & 0x3FF));
	}

void AppendCharacter(HBufC *aDes, TUint aChar);
void AppendCharacter(TBuf<24> *aDes, TUint aChar);


// Class to implement a wrapping iterator meant for extracting 16 bit characters
// from a block of text
// This class can handle surrogate pairs correctly.
class TextIterator
	{
public:
	TextIterator(const TText16* aData, const TInt aLength) : 
	  iData(aData), iLength(aLength), iIndex(0), iWrapped(EFalse), iCombineSurrogatePairs(ETrue)
		{
		}	
	TextIterator(const TText16* aData, const TInt aLength, const TInt aIndex) : 
		iData(aData), iLength(aLength), iIndex(aIndex), iWrapped(EFalse), iCombineSurrogatePairs(ETrue)
		{
		}
	TextIterator(const TText16* aData, const TInt aLength, const TInt aIndex, TBool aCombineSurrogatePairs) : 
		iData(aData), iLength(aLength), iIndex(aIndex), iWrapped(EFalse), iCombineSurrogatePairs(aCombineSurrogatePairs)
		{
		}
	// Next character from data - wrap pointer to keep within data.
	// Get current char, then move the internal pointer forward to next char.
	TUint NextChar()
		{
		TUint ch = Char();
		if (iCombineSurrogatePairs && IsSupplementary(ch))
			iIndex += 2;
		else
			iIndex += 1;
		// wrap
		if (iIndex >= iLength)
			{
			iIndex = 0;
			iWrapped = ETrue;
			}
		return ch;
		}
	// Move the internal pointer backward, then return the char pointed by internal char.
	// Panic if iIndex already = 0.
	TUint PreviousChar()
		{
		ASSERT(iIndex > 0);
		iIndex--;
		if (iCombineSurrogatePairs && IsLowSurrogate(iData[iIndex]))
			{
			ASSERT(iIndex > 0);
			iIndex--;
			ASSERT(IsHighSurrogate(iData[iIndex]));
			}
		else if (iCombineSurrogatePairs && IsHighSurrogate(iData[iIndex]))
			{
			ASSERT(EFalse);
			}
		else
			{
			// do nothing
			}
		return Char();
		}
	//Reset the iterator to the original values
	void Reset()
		{
		iIndex = 0;
		iWrapped = EFalse;
		}
	void SetIndex(const TInt aIndex)
		{
		iIndex = aIndex;
		iWrapped = EFalse;
		}
	//Has the iterator wrapped back to the start of the buffer at least once?
	TBool Wrapped() const
		{
		return iWrapped;
		}
	//Where is the current buffer index?
	TInt Index() const
		{
		return iIndex;
		}
	//Return pointer to data
	const TText16* Ptr() const
		{
		return iData;
		}
	//Get length of data
	TInt Length() const
		{
		return iLength;
		}
	//Get character at current index
	TUint Char() const
		{
		TUint ch = 0xFFFF;
		TText16 i16 = iData[iIndex];
		if (iCombineSurrogatePairs && IsHighSurrogate(i16))
			{
			ASSERT(iIndex+1 < iLength);
			TText16 low = iData[iIndex+1];
			ch = JoinSurrogate(i16, low);
			}
		else if (iCombineSurrogatePairs && IsLowSurrogate(i16))
			{
			ASSERT(EFalse);
			}
		else
			{
			ch = i16;
			}
		return ch;
		}
	// Fill aBuffer with a char, fill at aIndex, guarded by aMaxIndex (excluded).
	// After return, aIndex points to the next position, if aUpdate_aIndex=ETrue.
	// aUpdate_aIndex: do you want to update the parameter aIndex?
	// aUpdate_iIndex: do you want to update the member variable iIndex?
	// aChar: [out] current char
	TBool NextCharInto(TDes &aBuffer, TInt &aIndex, TInt aMaxIndex=-1, TBool aUpdate_aIndex=ETrue, TBool aUpdate_iIndex=ETrue, TUint *aChar=NULL)
		{
		if (aMaxIndex < 0)
			aMaxIndex = aBuffer.Length();
		if (aIndex >= aMaxIndex)
			return EFalse;

		TUint ch = Char();
		if (iCombineSurrogatePairs && IsSupplementary(ch))
			{
			if (aIndex+1 >= aMaxIndex)
				return EFalse;
			aBuffer[aIndex] = GetHighSurrogate(ch);
			aBuffer[aIndex+1] = GetLowSurrogate(ch);
			if (aUpdate_aIndex)
				aIndex += 2;
			}
		else
			{
			aBuffer[aIndex] = (TText16)ch;
			if (aUpdate_aIndex)
				aIndex++;
			}
		if (aChar)
			*aChar = ch;
		if (aUpdate_iIndex)
			{
			if (iCombineSurrogatePairs && IsSupplementary(ch))
				iIndex += 2;
			else
				iIndex += 1;
			if (iIndex >= iLength)
				{
				iIndex = 0;
				iWrapped = ETrue;
				}
			}
		return ETrue;
		}
	// Fill aBuffer from aIndex to aMaxIndex (excluded).
	// aMaxIndex=-1 means fill to index=aBuffer.Length.
	// After return, aIndex points to the next position, if aUpdate_aIndex=ETrue.
	TBool FillInto(TDes &aBuffer, TInt &aIndex, TInt aMaxIndex=-1)
		{
		if (aMaxIndex == -1)
			aMaxIndex = aBuffer.Length();
		while (aIndex < aMaxIndex)
			{
			TBool b = NextCharInto(aBuffer, aIndex, aMaxIndex);
			if (!b)
				{
				return ETrue;
				}
			}
		return ETrue;
		}

private:
	const TText16* iData;
	TInt  iLength;
	TInt  iIndex;
	TBool iWrapped;
	TBool iCombineSurrogatePairs;	// ETrue = combine surrogates; EFalse = take single surrogate as character
	};

class CTBiDiStep : public CTGraphicsStep
	{
public:
	CTBiDiStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTBiDiStep,"TBiDi");


#endif
